#include "qalgorithms_read_file.h"
#include "qalgorithms_datatypes.h"

#include "../external/simdutf/simdutf.h" // use a fast base64 decode function that makes proper use of SIMD
#include "../external/pugixml-1.14/src/pugixml.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include <cassert>

namespace qAlgorithms
{
    // Decodes from a little endian binary string to a vector of doubles according to a precision integer.
    std::vector<double> decode_little_endian(const std::string &str, bool isDouble)
    {
        // assert(precision == 4 || precision == 8);
        std::vector<unsigned char> bytes(str.begin(), str.end());

        int bytes_size = bytes.size() / (isDouble ? 8 : 4);

        std::vector<double> result(bytes_size);

        if (isDouble)
        {
            for (int i = 0; i < bytes_size; ++i)
            {
                result[i] = reinterpret_cast<double &>(bytes[i * 8]);
            }
        }
        else
        {
            // this branch should generally never be taken, why was it included initially?
            std::cerr << "Warning: it is unexpected that data is stored as 32-bit float.\n";
            for (int i = 0; i < bytes_size; ++i)
            {
                float floatValue;
                std::memcpy(&floatValue, &bytes[i * 4], sizeof(float)); // @todo float is not 32 bit everywhere
                result[i] = static_cast<double>(floatValue);
            }
        }
        return result;
    };

    // Decompresses a string using the zlib library (https://zlib.net/).
    std::string decompress_zlib(const char *compressed_string, size_t stringsize)
    {
        z_stream zs;

        memset(&zs, 0, sizeof(zs));

        inflateInit(&zs);

        zs.next_in = (Bytef *)compressed_string;

        zs.avail_in = stringsize;

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

    // Decodes a Base64 string into a string with binary data using the simdutf library subset chosen by '--with-base64'
    // (https://github.com/simdutf/simdutf/tree/master?tab=readme-ov-file#single-header-version-with-limited-features).
    std::vector<char> decode_base64(const std::string &encoded_string)
    {
        size_t length = encoded_string.size() / 4 * 3;
        std::vector<char> output(length);
        auto simd_res = simdutf::base64_to_binary(encoded_string.c_str(), encoded_string.size(), output.data());

        if (simd_res.error != 0) [[unlikely]]
        {
            return {char(simd_res.count)}; // error message is handled one function above
        }
        output.resize(simd_res.count);
        return output;
    };

    XML_File::XML_File(const std::filesystem::path &file)
    {
        if (file.extension() == ".mzML")
        {
            filetype = mzML;
        }
        else if (file.extension() == ".mzXML")
        {
            assert(false); // @todo support mzXML and other xml based formats
        }

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

    BinaryMetadata XML_File::extract_binary_metadata(const pugi::xml_node &bin)
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

    void XML_File::get_spectrum( // this obviously only extracts data that is in profile mode.
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
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.size() == 1)
            {
                std::cerr << "Error: spectrum " << index << " could not be decoded as base64 correctly. Please ensure your input file is not corrupted.\n";
                // this does not work @todo
                //   << "The error occured at character " << decoded_string[0] << " as reported by the \"simdutf\" library.\n";
                return; // implement a defined way to skip the current file? @todo
            }

            std::string decoded_string1;
            if (spectra_binary_metadata[0].compressed)
            {
                // @todo is there a case where this doesn't apply for all spectra?
                decoded_string1 = decompress_zlib(decoded_string.data(), decoded_string.size());
            }

            *spectrum_mz = decode_little_endian(decoded_string1, spectra_binary_metadata[0].isDouble);

            assert(spectrum_mz->size() == number_traces); // this happens if an index is tried which does not exist in the data
        }

        // bin = node_binary_list.children("binaryDataArray").end();
        // the above line will not work, even if bin only has two elements, because the ++ operator modifies more internal
        // state than a simple index. @todo replace with an explicit access to the first and second element
        bin++;
        { // extract intensity values
            pugi::xml_node node_binary = bin->child("binary");
            std::string encoded_string = node_binary.child_value();
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.size() == 1)
            {
                std::cerr << "Error: spectrum " << index << " could not be decoded as base64 correctly. Please ensure your input file is not corrupted.\n";
                //   << "The error occured at character " << decoded_string[0] << " as reported by the \"simdutf\" library.\n";
                return; // implement a defined way to skip the current file? @todo
            }

            std::string decompressedString;
            if (spectra_binary_metadata[1].compressed)
            {
                decompressedString = decompress_zlib(decoded_string.data(), decoded_string.size()); // @todo is there a case where this doesn't apply for all spectra?
            }

            *spectrum_RT = decode_little_endian(decompressedString, spectra_binary_metadata[1].isDouble);

            assert(spectrum_RT->size() == number_traces); // this happens if an index is tried which does not exist in the data
            assert(spectra_binary_metadata[1].data_name_short == "intensity");
        }
    };

    double XML_File::extract_scan_RT(const pugi::xml_node &spec)
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

    std::vector<double> XML_File::get_spectra_RT(const std::vector<unsigned int> *indices)
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

    std::vector<bool> XML_File::get_spectra_polarity(const std::vector<unsigned int> *indices)
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

    Polarities XML_File::get_polarity_mode(const size_t count)
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
            return Polarities::positive;
        }
        else
        {
            return Polarities::negative;
        }
    };

    std::vector<size_t> XML_File::get_spectra_index(const std::vector<unsigned int> *indices)
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

    std::vector<int> XML_File::get_spectra_level(const std::vector<unsigned int> *indices)
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

    std::vector<bool> XML_File::get_spectra_mode(const std::vector<unsigned int> *indices) // centroid or profile
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

    std::vector<unsigned int> XML_File::filter_spectra(bool ms1, bool polarity, bool centroided)
    {
        // extract relevant properties out of the function call @todo make all relevant fields part of a
        // context struct the class holds
        // XML_Attribute centroidsXMLA;
        // XML_Attribute polarityXMLA;
        // XML_Attribute msLevelXMLA;

        // if (filetype == mzML)
        // {
        //     *centroidsXMLA.name = 'cvParam';
        //     *centroidsXMLA.attr_name = 'accession';
        //     *centroidsXMLA.attr_value = 'MS:1000127';
        // }

        // return a vector of all indices that are relevant to the query. Properties are checked in order of regularity.
        assert(number_spectra > 0);
        std::vector<unsigned int> indices;
        indices.reserve(number_spectra);

        std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes(mzml_root_node);

        // pugi::char_t *cv = "cvParam"; // @todo this does not work, abstract accessor fields somehow

        for (unsigned int i = 0; i < number_spectra; i++)
        {
            pugi::xml_node spec = spectra_nodes[i];

            bool isCentroid = spec.find_child_by_attribute("cvParam", "accession", "MS:1000127");
            if (isCentroid != centroided)
                continue; // this does not allow for processing of partially centroided data

            bool polarityPos = spec.find_child_by_attribute("cvParam", "accession", "MS:1000130");
            if (polarityPos != polarity)
                continue;

            pugi::xml_node level_node = spec.find_child_by_attribute("cvParam", "name", "ms level");
            int level = level_node.attribute("value").as_int();
            bool isMS1 = 1 == level;
            if (isMS1 != ms1)
                continue; // only ms1 or msn data can be retrieved at once.

            indices.push_back(i);
        }
        indices.shrink_to_fit();
        return indices;
    }
}