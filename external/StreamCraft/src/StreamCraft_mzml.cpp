#include "StreamCraft_mzml.hpp"

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include <cassert>

// Decodes from a little endian binary string to a vector of doubles according to a precision integer.
std::vector<double> decode_little_endian(const std::string &str, const int precision)
{
    assert(precision == 4 || precision == 8);
    std::vector<unsigned char> bytes(str.begin(), str.end());

    int bytes_size = (bytes.size() / precision);

    std::vector<double> result(bytes_size);

    for (int i = 0; i < bytes_size; ++i)
    {

        if (precision == 8)
        {
            result[i] = reinterpret_cast<double &>(bytes[i * precision]);
        }
        else
        {
            float floatValue;
            std::memcpy(&floatValue, &bytes[i * precision], sizeof(float));
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

// Lookup-Table für Base64-Decoding: ASCII -> 0..63 oder -1 für ungültig
static constexpr int8_t base64_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, //   0- 15
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, //  16- 31
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63, //  32- 47  + (43), / (47)
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1, //  48- 63  0-9
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, //  64- 79  A-Z
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1, //  80- 95
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, //  96-111  a-z
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1, // 112-127
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 128-143
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 144-159
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 160-175
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 176-191
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 192-207
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 208-223
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 224-239
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1  // 240-255
};

std::string decode_base64(const std::string& input)
{
    std::string out;
    int val = 0;
    int valb = -8;
    for (unsigned char c : input)
    {
        if (c == '=') break; // Padding ignorieren
        int8_t d = base64_table[c];
        if (d == -1) continue; // ungültiges Zeichen skippen
        val = (val << 6) | d;
        valb += 6;
        if (valb >= 0)
        {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

StreamCraft::MZML::MZML(const std::filesystem::path &file)
{
    pugi::xml_document doc; // @todo why does this need to persist beyond the constructor? check access to class member variable

    loading_result = mzml_base_document.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);
    auto loading2 = doc.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

    assert(loading2.status == loading_result.status);

    if (loading_result)
    {
        mzml_root_node = mzml_base_document.document_element();
        assert(mzml_root_node);
    }
    else
    {
        std::cerr << "mzML file could not be opened!" << std::endl
                  << loading_result.description() << std::endl;
        return;
    }

    std::string format = mzml_root_node.name();
    assert(format == "indexedmzML");

    mzml_root_node = mzml_root_node.first_child();

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    number_spectra = spec_list.attribute("count").as_int();

    if (number_spectra > 0)
    {
        auto first_node = spec_list.first_child();
        pugi::xml_node binary_list = first_node.child("binaryDataArrayList");

        size_t counter = 0;

        for (const pugi::xml_node &bin : binary_list.children("binaryDataArray"))
        {
            MZML_BINARY_METADATA mtd = extract_binary_metadata(bin);

            mtd.index = counter;

            spectra_binary_metadata.push_back(mtd);

            counter++;
        }
        number_spectra_binary_arrays = counter;
    }
};

StreamCraft::MZML_BINARY_METADATA StreamCraft::MZML::extract_binary_metadata(const pugi::xml_node &bin)
{
    MZML_BINARY_METADATA mtd;

    pugi::xml_node node_integer_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000519");
    pugi::xml_node node_float_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000521");
    pugi::xml_node node_integer_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000522");
    pugi::xml_node node_float_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000523");

    if (node_float_64)
    {
        mtd.precision_int = 64;
    }
    else if (node_float_32)
    {
        mtd.precision_int = 32;
    }
    else if (node_integer_64)
    {
        mtd.precision_int = 64;
    }
    else if (node_integer_32)
    {
        mtd.precision_int = 32;
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

    for (size_t i = 0; 1 < possible_accessions_binary_data.size(); ++i)
    {
        pugi::xml_node node_data_type = bin.find_child_by_attribute("cvParam", "accession", possible_accessions_binary_data[i].c_str());

        if (node_data_type)
        {
            has_bin_data_type = true;
            mtd.data_value = node_data_type.attribute("value").as_string();
            mtd.data_name_short = possible_short_name_binary_data[i];
            break;
        }
    }
    assert(has_bin_data_type);
    // throw("Encoded data type could not be found matching the mzML official vocabulary!");

    if (mtd.data_name_short == "other") // this should be separated into a check for null / termination condition @todo
    {
        mtd.data_name_short = mtd.data_value;
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

std::vector<std::vector<double>> StreamCraft::MZML::get_spectrum(int index)
{
    std::vector<std::vector<double>> spectrum;
    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return spectrum;
    }

    const pugi::xml_node &spectrum_node = spectra_nodes[index];
    spectrum = extract_spectrum(spectrum_node);

    return spectrum;
}

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

std::vector<std::vector<double>> StreamCraft::MZML::extract_spectrum(const pugi::xml_node &spectrum_node)
{

    pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");
    unsigned int number_bins = node_binary_list.attribute("count").as_int();
    assert(number_spectra_binary_arrays == number_bins);

    std::vector<std::vector<double>> spectrum;
    unsigned int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

    spectrum.resize(number_bins);

    int counter = 0;

    for (auto i = node_binary_list.children("binaryDataArray").begin(); i != node_binary_list.children("binaryDataArray").end(); ++i)
    {
        const pugi::xml_node &bin = *i;

        pugi::xml_node node_binary = bin.child("binary");
        std::string encoded_string = node_binary.child_value();
        std::string decoded_string = decode_base64(encoded_string);

        if (spectra_binary_metadata[counter].compressed)
        {
            decoded_string = decompress_zlib(decoded_string); // @todo is there a case where this doesn't apply for all spectra?
        }

        spectrum[counter] = decode_little_endian(decoded_string, spectra_binary_metadata[counter].precision_int / 8);

        assert(spectrum[counter].size() == number_traces); // this happens if an index is tried which does not exist in the data

        if (spectra_binary_metadata[counter].data_name_short == "time")
        {
            pugi::xml_node node_unit = bin.find_child_by_attribute("cvParam", "unitCvRef", "UO");
            std::string unit = node_unit.attribute("unitName").as_string();

            if (unit == "minute")
            {
                for (double &j : spectrum[counter])
                {
                    j *= 60;
                }
            }
            else
            {
                assert(unit == "second");
            }
        }
        counter++;
    }
    return spectrum;
};

std::vector<std::vector<std::vector<double>>> StreamCraft::MZML::extract_spectra(const std::vector<int> &idxs)
{
    std::vector<std::vector<std::vector<double>>> all_spectra;
    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "No indices given!" << std::endl;
        return all_spectra;
    }

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return all_spectra;
    }

    all_spectra.resize(n);

    // # pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        const int index = idxs[i];

        const pugi::xml_node &spectrum_node = spectra_nodes[index];

        all_spectra[i] = extract_spectrum(spectrum_node);
    }

    return all_spectra;
}

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