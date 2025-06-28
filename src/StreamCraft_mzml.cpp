#include "StreamCraft_mzml.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include <cassert>

// Decodes from a little endian binary string to a vector of doubles according to a precision integer.
std::vector<double> decode_little_endian(const std::string &str, bool isDouble)
{
    // assert(precision == 4 || precision == 8);
    std::vector<unsigned char> bytes(str.begin(), str.end());

    int bytes_size = bytes.size() / (isDouble ? 8 : 4);

    std::vector<double> result(bytes_size);

    for (int i = 0; i < bytes_size; ++i)
    {

        if (isDouble)
        {
            result[i] = reinterpret_cast<double &>(bytes[i * 8]);
        }
        else
        {
            assert(false); // this branch should generally never be taken, why was it included initially?
            float floatValue;
            std::memcpy(&floatValue, &bytes[i * 4], sizeof(float)); // @todo float is not 32 bit everywhere
            result[i] = static_cast<double>(floatValue);
        }
    }
    return result;
};

// Decompresses a string using the zlib library (https://zlib.net/).
std::string decompress_zlib(const std::string &compressed_string)
{
    z_stream zs;

    memset(&zs, 0, sizeof(zs));

    inflateInit(&zs);

    zs.next_in = (Bytef *)compressed_string.data();

    zs.avail_in = compressed_string.size();

    char outbuffer[32768]; // @todo why this size?

    std::string outstring;

    int ret = Z_OK;
    while (ret == Z_OK)
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);

        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out)
        {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    }
    inflateEnd(&zs);

    return outstring;
};

// Decodes a Base64 string into a string with binary data.
// @todo performance critical. Replace with highly optimised library
std::string decode_base64(const std::string &encoded_string)
{
    std::string decoded_string;

    decoded_string.reserve((encoded_string.size() * 3) / 4);

    int val = 0;
    int valb = -8;
    for (char c : encoded_string)
    {
        if (c == '=')
        {
            valb -= 6;
            continue;
        }
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
        }
        else if (c >= 'a' && c <= 'z')
        {
            c -= 'a' - 26;
        }
        else if (c >= '0' && c <= '9')
        {
            c -= '0' - 52;
        }
        else if (c == '+')
        {
            c = 62;
        }
        else if (c == '/')
        {
            c = 63;
        }
        else
        {
            continue;
        }
        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0)
        {
            decoded_string.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decoded_string;
};

StreamCraft::MZML::MZML(const std::filesystem::path &file)
{
    loading_result = mzml_base_document.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

    if (loading_result)
    {
        mzml_root_node = mzml_base_document.document_element();
        assert(mzml_root_node);
    }
    else
    {
        std::cerr << "Error: .mzML file could not be opened. Error description:\n"
                  << loading_result.description() << "\n";
        defective = true;
        return;
    }

    std::string format = mzml_root_node.name();
    assert(format == "indexedmzML");

    mzml_root_node = mzml_root_node.first_child();

    pugi::xpath_node xps_run = mzml_root_node.select_node("//run");

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    number_spectra = spec_list.attribute("count").as_int();

    if (number_spectra > 0)
    {
        auto first_node = spec_list.first_child();
        pugi::xml_node binary_list = first_node.child("binaryDataArrayList");

        size_t counter = 0;

        for (const pugi::xml_node &bin : binary_list.children("binaryDataArray"))
        {
            BinaryMetadata mtd = extract_binary_metadata(bin);

            mtd.index = counter;

            spectra_binary_metadata.push_back(mtd);

            counter++;
        }
        number_spectra_binary_arrays = counter;
    }
};

StreamCraft::BinaryMetadata StreamCraft::MZML::extract_binary_metadata(const pugi::xml_node &bin)
{
    BinaryMetadata mtd;

    pugi::xml_node node_integer_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000519");
    pugi::xml_node node_float_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000521");
    pugi::xml_node node_integer_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000522");
    pugi::xml_node node_float_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000523");

    if (node_float_64)
    {
        // mtd.precision_int = 64;
        mtd.isDouble = true;
    }
    else if (node_float_32)
    {
        // mtd.precision_int = 32;
        mtd.isDouble = false;
    }
    else if (node_integer_64)
    {
        // mtd.precision_int = 64;
        mtd.isDouble = true;
    }
    else if (node_integer_32)
    {
        // mtd.precision_int = 32;
        mtd.isDouble = false;
    }
    else
    {
        assert(false);
        // Encoding precision with accession MS:1000519, MS:1000521, MS:1000522 or MS:1000523 not found
    }

    pugi::xml_node node_comp = bin.find_child_by_attribute("cvParam", "accession", "MS:1000574");
    if (node_comp)
    {
        const std::string compression = node_comp.attribute("name").as_string();

        if (compression == "zlib" || compression == "zlib compression")
        {
            mtd.compressed = true;
        }
        else
        {
            mtd.compressed = false; // @todo what if it isn't zlib compressed?
        }
    }

    bool has_bin_data_type = false;

    std::string tmp;
    for (size_t i = 0; 1 < possible_accessions_binary_data_mzML.size(); ++i)
    {
        pugi::xml_node node_data_type = bin.find_child_by_attribute("cvParam", "accession", possible_accessions_binary_data_mzML[i].c_str());

        if (node_data_type)
        {
            has_bin_data_type = true;
            tmp = node_data_type.attribute("value").as_string();
            mtd.data_name_short = possible_short_name_binary_data_mzML[i];
            break;
        }
    }
    assert(has_bin_data_type);
    // throw("Encoded data type could not be found matching the mzML official vocabulary!");

    if (mtd.data_name_short == "other") // this should be separated into a check for null / termination condition @todo
    {
        mtd.data_name_short += ": " + tmp;
    }

    return mtd;
}

std::vector<pugi::xml_node> link_vector_spectra_nodes(pugi::xml_node mzml_root_node) // @todo this does not need to be called more than once
{
    std::vector<pugi::xml_node> spectra;

    std::string search_run = "//run";
    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());
    pugi::xml_node spec_list = xps_run.node().child("spectrumList");
    assert(spec_list);

    for (pugi::xml_node child = spec_list.first_child(); child; child = child.next_sibling())
    {
        spectra.push_back(child);
    }

    assert(spectra.size() > 0);
    return spectra;
};

void StreamCraft::MZML::get_spectrum( // this obviously only extracts data that is in profile mode.
    std::vector<double> *const spectrum_mz,
    std::vector<double> *const spectrum_RT,
    size_t index)
{
    assert(spectrum_mz->empty() && spectrum_RT->empty());

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "Error: no spectra found for index" << index << "\n";
        return;
    }

    const pugi::xml_node &spectrum_node = spectra_nodes[index];

    pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");
    assert(number_spectra_binary_arrays == 2);

    unsigned int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

    auto bin = node_binary_list.children("binaryDataArray").begin();
    assert(bin != node_binary_list.children("binaryDataArray").end());
    { // extract mz values
        pugi::xml_node node_binary = bin->child("binary");
        std::string encoded_string = node_binary.child_value();
        std::string decoded_string = decode_base64(encoded_string);

        if (spectra_binary_metadata[0].compressed)
        {
            decoded_string = decompress_zlib(decoded_string); // @todo is there a case where this doesn't apply for all spectra?
        }

        *spectrum_mz = decode_little_endian(decoded_string, spectra_binary_metadata[0].isDouble);

        assert(spectrum_mz->size() == number_traces); // this happens if an index is tried which does not exist in the data
    }

    // bin = node_binary_list.children("binaryDataArray").end();
    // the above line will not work, even if bin only has two elements, because the ++ operator modifies more internal
    // state than a simple index. @todo replace with an explicit access to the first and second element
    bin++;
    { // extract intensity values
        pugi::xml_node node_binary = bin->child("binary");
        std::string encoded_string = node_binary.child_value();
        std::string decoded_string = decode_base64(encoded_string);

        if (spectra_binary_metadata[1].compressed)
        {
            decoded_string = decompress_zlib(decoded_string); // @todo is there a case where this doesn't apply for all spectra?
        }

        *spectrum_RT = decode_little_endian(decoded_string, spectra_binary_metadata[1].isDouble);

        assert(spectrum_RT->size() == number_traces); // this happens if an index is tried which does not exist in the data
        assert(spectra_binary_metadata[1].data_name_short == "intensity");
    }
};

double StreamCraft::MZML::extract_scan_RT(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node rt_node = node_scan.find_child_by_attribute("cvParam", "name", "scan start time");
    const std::string rt_unit = rt_node.attribute("unitName").as_string();
    double rt_val = rt_node.attribute("value").as_double();
    if (rt_unit == "second")
    {
        return rt_val;
    }
    if (rt_unit == "minute")
    {
        return rt_val * 60;
    }
    assert(false);
};

std::vector<double> StreamCraft::MZML::get_spectra_RT(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);

    std::vector<double> retention_times;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = indices->at(i);
        pugi::xml_node spec = spectra_nodes[idx];
        double RT = extract_scan_RT(spec);
        retention_times.push_back(RT);
    }

    return retention_times;
};

std::vector<bool> StreamCraft::MZML::get_spectra_polarity(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<bool> polarities;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    for (size_t i = 0; i < indices->size(); ++i)
    {
        int idx = indices->at(i);
        pugi::xml_node spec = spectra_nodes[idx];
        if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000130"))
        {
            polarities.push_back(true);
        }
        else
        {
            assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000129")); // @todo a single check should be enough
            polarities.push_back(false);
        }
    }

    return polarities;
};

StreamCraft::polarity_MZML StreamCraft::MZML::get_polarity_mode(const size_t count)
{
    // @todo this should check if count is in bounds

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    bool positive = false;
    bool negative = false;
    for (size_t i = 0; i < count; ++i)
    {
        pugi::xml_node spec = spectra_nodes[i];
        if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000130"))
        {
            positive = true;
        }
        else
        {
            assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000129")); // @todo a single check should be enough
            negative = true;
        }

        if (positive && negative)
        {
            return mixed;
        }
    }
    if (positive)
    {
        return polarity_MZML::positive;
    }
    else
    {
        return polarity_MZML::negative;
    }
};

std::vector<size_t> StreamCraft::MZML::get_spectra_index(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<size_t> spec_indices;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    for (size_t i = 0; i < indices->size(); ++i)
    {
        int idx = indices->at(i);
        pugi::xml_node spec = spectra_nodes[idx];
        int index = spec.attribute("index").as_int();
        spec_indices.push_back(index);
    }

    return spec_indices;
};

std::vector<int> StreamCraft::MZML::get_spectra_level(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<int> levels;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = indices->at(i);
        pugi::xml_node spec = spectra_nodes[idx];
        pugi::xml_node level_node = spec.find_child_by_attribute("cvParam", "name", "ms level");
        int level = level_node.attribute("value").as_int();
        levels.push_back(level);
    }

    return levels;
};

std::vector<bool> StreamCraft::MZML::get_spectra_mode(const std::vector<unsigned int> *indices) // centroid or profile
{
    assert(indices->size() > 0);
    std::vector<bool> modes;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = indices->at(i);
        pugi::xml_node spec = spectra_nodes[idx];
        if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000128"))
        {
            modes.push_back(true); // profile mode
        }
        else
        {
            // @todo is there any case where the mode is neither profile nor centroid?
            assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000127"));
            modes.push_back(false); // centroided
        }
    }
    return modes;
};