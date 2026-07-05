#include "qalgorithms_read_file.h"
#include "qalgorithms_datatypes.h"

#include "../external/pugixml/pugixml.hpp"
#include "../external/simdutf/simdutf.h" // use a fast base64 decode function that makes proper use of SIMD

// #include <filesystem>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

// zlib-ng should be present on all modern systems, use the __has_include macro
// and redefine the zng_... functions if this doesn't work
#include <zlib-ng.h>

// since we care mostly about speed, we want to use the generally faster zlib-ng for decompression.
// However, we cannot be sure that it exists for a given system. Therefore, it is only included if
// it can be installed for the host

namespace qAlgorithms
{
    int bytesToFloatVec(const std::vector<char> *bytes, const bool isDouble,
                        std::vector<float> *result)
    {
        // cast the byte array resulting from zlib decompression to a float array
        const size_t fsize = sizeof(float);
        const size_t dsize = sizeof(double);
        assert(bytes->size() % (isDouble ? dsize : fsize) == 0);
        size_t lengthDecoded = bytes->size() / (isDouble ? dsize : fsize);

        result->resize(lengthDecoded);
        float *res = result->data();

// for only this block, ignore the alignment change. It is intended behavouir.
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
        if (isDouble)
        {
            // cppcheck-suppress invalidPointerCast
            const double *dbl = (const double *)bytes->data();
            for (size_t i = 0; i < lengthDecoded; i++)
            {
                res[i] = (float)dbl[i]; // result must be cast to float individually
            }
        }
        else
        {
            // encountered one relevant file "in the wild", but this is still rare enough to warrant a warning
            const void *src = bytes->data();
            memcpy(res, src, lengthDecoded * sizeof(float));
        }
#pragma clang diagnostic pop
        return lengthDecoded;
    };

    // Decompresses a string using the zlib library (https://zlib.net/).
    // static void decompress_zlib(const std::vector<char> *compressed_string, std::vector<char> *output_string)
    // {
    //     // max expected compression is factor 6
    //     output_string->resize(compressed_string->size() * 6);
    //     // zlib struct
    //     z_stream infstream;
    //     infstream.zalloc = Z_NULL;
    //     infstream.zfree = Z_NULL;
    //     infstream.opaque = Z_NULL;
    //     // setup input and output
    //     infstream.avail_in = compressed_string->size();         // size of input
    //     infstream.next_in = (Bytef *)compressed_string->data(); // input char array
    //     infstream.avail_out = output_string->size();            // maximum size of output
    //     infstream.next_out = (Bytef *)output_string->data();    // output char array

    //     // the actual DE-compression work.
    //     inflateInit(&infstream);
    //     auto ret_1 = inflate(&infstream, Z_NO_FLUSH);
    //     assert(ret_1 == 1);
    //     auto ret_2 = inflateEnd(&infstream);
    //     assert(ret_2 == 0);
    //     // since resize does not deallocate, this just ensures we can use the vector without making compromises later
    //     output_string->resize(infstream.total_out);
    // };

    static BinaryMetadata extract_binary_metadata(const pugi::xml_node &bin);

    XML_File::XML_File(const path_char *file, const SourceFileType type)
    {
        filetype = type;
        assert(filetype == mzML);

        loading_result = mzml_base_document.load_file(file, pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

        if (loading_result)
        {
            mzml_root_node = mzml_base_document.document_element();
            assert(mzml_root_node);
        }
        else
        {
            fprintf(stderr, "Error: .mzML file could not be opened. Error description:\n%s\n",
                    loading_result.description());
            defective = true;
            return;
        }

        std::string format = mzml_root_node.name();
        assert(format == "indexedmzML");

        mzml_root_node = mzml_root_node.first_child();
        pugi::xpath_node xps_run = mzml_root_node.select_node("//run");
        pugi::xml_node spec_list = xps_run.node().child("spectrumList");
        assert(spec_list);

        number_spectra = spec_list.attribute("count").as_uint();

        if (number_spectra > 0)
        {
            auto range = spec_list.first_child().child("binaryDataArrayList").children("binaryDataArray");
            auto iterator = range.begin();
            this->mtd_mz = extract_binary_metadata(*iterator);
            assert(mtd_mz.data_name_short == "mz");
            iterator++;
            // assert(iterator == range.end());
            this->mtd_intensity = extract_binary_metadata(*iterator);
            assert(mtd_intensity.data_name_short == "intensity");

            if (!(mtd_mz.isDouble && mtd_intensity.isDouble))
                fprintf(stderr, "Warning: it is unexpected that data is stored as 32-bit float.\n");
        }

        linknodes = new std::vector<pugi::xml_node>(number_spectra);
        linknodes->clear();
        for (pugi::xml_node child = spec_list.first_child(); (bool)child; child = child.next_sibling())
        {
            linknodes->push_back(child);
        }
        assert(linknodes->size() == number_spectra);

        isCentroided = isCentroided_fun();

        polarityMode = get_polarity_mode();
        assert(polarityMode != Polarities::unknown_polarity);
    };

    void XML_File::free_linknodes()
    {
        delete linknodes;
        defective = true;
    };

    static BinaryMetadata extract_binary_metadata(const pugi::xml_node &bin)
    {
        // extract type of number representation in binary data
        bool type_double = false, type_float = false, type_int32 = false, type_int64 = false;

        for (pugi::xml_node cvParam = bin.child("cvParam"); cvParam != nullptr; cvParam = cvParam.next_sibling("cvParam"))
        {
            std::string val = cvParam.attribute("accession").value();
            if (val == "MS:1000523")
            {
                type_double = true;
            }
            if (val == "MS:1000521")
            {
                type_float = true;
            }
            if (val == "MS:1000519")
            {
                type_int32 = true;
            }
            if (val == "MS:1000522")
            {
                type_int64 = true;
            }
        }
        assert(type_double != type_float);
        assert(!type_int32);
        assert(!type_int64);

        BinaryMetadata mtd;

        mtd.isDouble = type_double;

        pugi::xml_node node_comp = bin.find_child_by_attribute("cvParam", "accession", "MS:1000574");
        if (node_comp != nullptr)
        {
            const char *compression = node_comp.attribute("name").as_string();
            mtd.compressed = (std::strcmp(compression, "zlib") == 0) ||
                             (std::strcmp(compression, "zlib compression") == 0);
        }
        else
        {
            mtd.compressed = false;
        }

        bool has_bin_data_type = false;
        for (size_t i = 0; 1 < possible_accessions_binary_data_mzML.size(); ++i)
        {
            pugi::xml_node node_data_type = bin.find_child_by_attribute(
                "cvParam", "accession",
                possible_accessions_binary_data_mzML[i].c_str());

            if (node_data_type != nullptr)
            {
                has_bin_data_type = true;
                mtd.data_name_short = possible_short_name_binary_data_mzML[i];
                break;
            }
        }
        assert(has_bin_data_type); // Encoded data type could not be found matching the mzML official vocabulary
        assert(mtd.data_name_short != "other");

        return mtd;
    }

    int XML_File::get_spectrum( // this obviously only extracts data that is in profile mode.
        std::vector<float> *const spectrum_mz,
        std::vector<float> *const spectrum_int,
        size_t index)
    {
        assert(spectrum_mz->empty() && spectrum_int->empty());
        assert(!this->defective);

        if (linknodes->size() == 0)
        {
            fprintf(stderr, "Error: no spectra found for index %zu\n", index);
            return 1;
        }

        const pugi::xml_node &spectrum_node = (*linknodes)[index];

        pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");

        unsigned int number_traces = spectrum_node.attribute("defaultArrayLength").as_uint();

        auto dataArray = node_binary_list.children("binaryDataArray").begin();
        assert(dataArray != node_binary_list.children("binaryDataArray").end());

        std::vector<char> buffer;
        { // extract mz values
            pugi::xml_node node_binary = dataArray->child("binary");
            const char *encoded_string = node_binary.child_value();
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.empty())
            {
                fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 \n"
                                "correctly. Ensure the input file is not corrupted.\n",
                        index);
                return 2;
            }

            if (mtd_mz.compressed)
            {
                size_t expectedSize = decoded_string.size() * 6;
                buffer.resize(expectedSize);
                zng_uncompress((uint8_t *)buffer.data(), &expectedSize,
                               (uint8_t *)decoded_string.data(), decoded_string.size());
                // decompress_zlib(&decoded_string, &buffer);
                // check that less characters have been written than fit into the buffer
                assert(buffer.size() > expectedSize);
                buffer.resize(expectedSize);
                bytesToFloatVec(&buffer, mtd_mz.isDouble, spectrum_mz);
            }
            else
            {
                bytesToFloatVec(&decoded_string, mtd_mz.isDouble, spectrum_mz);
            }

            assert(spectrum_mz->size() == number_traces); // this happens if an index is tried which does not exist in the data
        }

        dataArray++; // array pointer is incremented since both spectra are stored at the same node

        { // extract intensity values
            pugi::xml_node node_binary = dataArray->child("binary");
            std::string encoded_string = node_binary.child_value();
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.empty())
            {
                fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 "
                                "correctly. Ensure the input file is not corrupted.\n",
                        index);
                return 2;
            }

            if (mtd_intensity.compressed)
            {
                size_t expectedSize = decoded_string.size() * 6;
                buffer.resize(expectedSize);
                zng_uncompress((uint8_t *)buffer.data(), &expectedSize,
                               (uint8_t *)decoded_string.data(), decoded_string.size());
                // decompress_zlib(&decoded_string, &buffer);
                // check that less characters have been written than fit into the buffer
                assert(buffer.size() > expectedSize);
                buffer.resize(expectedSize);
                bytesToFloatVec(&buffer, mtd_intensity.isDouble, spectrum_int);
            }
            else
            {
                bytesToFloatVec(&decoded_string, mtd_intensity.isDouble, spectrum_int);
            }

            assert(spectrum_int->size() == number_traces); // this happens if an index is tried which does not exist in the data
        }
        return 0;
    };

    static float extract_scan_RT(const pugi::xml_node &spec)
    {
        pugi::xml_node rt_node = spec.child("scanList").child("scan").find_child_by_attribute("cvParam", "name", "scan start time");

        float rt_val = rt_node.attribute("value").as_float();
        const char *rt_unit = rt_node.attribute("unitName").as_string();
        bool unit_secs = strcmp(rt_unit, "second") == 0; // strcmp returns 0 for equal strings
        if (!unit_secs)
        {
            assert(strcmp(rt_unit, "minute") == 0);
        }

        return unit_secs ? rt_val : rt_val * 60;
    };

    void XML_File::get_spectra_RT(const std::vector<unsigned int> *indices, std::vector<float> *const RTs)
    {
        const size_t idxSize = indices->size();
        assert(!this->defective);
        assert(idxSize > 0);

        RTs->resize(idxSize);

        for (size_t i = 0; i < idxSize; ++i)
        {
            size_t idx = indices->at(i);
            pugi::xml_node spec = (*linknodes)[idx];
            float RT = extract_scan_RT(spec);
            RTs->at(i) = RT;
        }
    };

    Polarities XML_File::get_polarity_mode()
    {
        assert(linknodes->size() > 1);
        size_t count = linknodes->size();

        bool positive = false;
        bool negative = false;
        for (size_t i = 0; i < count; ++i)
        {
            pugi::xml_node spec = (*linknodes)[i];
            if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000130") != nullptr)
            {
                positive = true;
            }
            else
            {
                assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000129"));
                negative = true;
            }

            if (positive && negative)
            {
                return Polarities::mixed;
            }
        }
        return positive ? Polarities::positive : Polarities::negative;
    };

    std::vector<unsigned int> XML_File::filter_spectra(
        bool ms1, bool polarity, bool centroided)
    {
        // return a vector of all indices that are relevant to the query. Properties are checked in order of regularity.
        assert(!this->defective);
        assert(this->number_spectra > 0);
        std::vector<unsigned int> indices;
        indices.reserve(this->number_spectra);

        for (unsigned int i = 0; i < this->number_spectra; i++)
        {
            pugi::xml_node spec = (*linknodes)[i];

            bool isCentroid = spec.find_child_by_attribute("cvParam", "accession", "MS:1000127") != nullptr;
            if (isCentroid != centroided)
                continue; // this does not allow for processing of partially centroided data

            bool polarityPos = spec.find_child_by_attribute("cvParam", "accession", "MS:1000130") != nullptr;
            if (polarityPos != polarity)
                continue;

            int level = spec.find_child_by_attribute("cvParam", "name", "ms level").attribute("value").as_int();
            bool isMS1 = 1 == level;
            if (isMS1 != ms1)
                continue; // only ms1 or msn data can be retrieved at once.

            indices.push_back(i);
        }
        indices.shrink_to_fit();
        return indices;
    }

    bool XML_File::isCentroided_fun()
    {
        size_t centroided = 0;
        size_t profile = 0;

        for (size_t i = 0; i < this->number_spectra; ++i)
        {
            pugi::xml_node spec = (*linknodes)[i];

            int level = spec.find_child_by_attribute("cvParam", "name", "ms level").attribute("value").as_int();
            bool isMS1 = 1 == level;
            if (!isMS1)
                continue;

            if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000128") != nullptr)
            {
                profile += 1;
                continue; // profile mode
            }
            centroided += 1;
        }

        if (centroided > 0)
        {
            assert(profile == 0);
            return true;
        }
        return false;
    }

    // Decodes a Base64 string into a string with binary data using the simdutf library subset chosen by '--with-base64'
    // (https://github.com/simdutf/simdutf/tree/master?tab=readme-ov-file#single-header-version-with-limited-features).
    std::vector<char> decode_base64(const std::string &encoded_string)
    {
        size_t length = encoded_string.size() / 4 * 3;
        std::vector<char> output(length);
        simdutf::result simd_res = simdutf::base64_to_binary(encoded_string.c_str(),
                                                             encoded_string.size(),
                                                             output.data());

        if (simd_res.error != 0) // [[unlikely]]
        {
            return {0}; // error message is handled one function above
        }
        output.resize(simd_res.count);
        return output;
    };

    std::vector<char> encode_base64_dbl(const double *input_dbl, const size_t in_len_dbl)
    {
        const size_t length = in_len_dbl * sizeof(double);
        const char *input = (const char *)input_dbl;
        std::vector<char> buffer(simdutf::base64_length_from_binary(length));
        size_t written = simdutf::binary_to_base64(input, length, buffer.data());
        assert(written == buffer.size());
        return buffer;
    }

    static void compress_zlib(const std::vector<char> *uncompressed_string, std::vector<char> *output_string)
    {
        // worst case: no compression (probably impossible)
        output_string->resize(uncompressed_string->size());
        size_t outSize = output_string->size();
        zng_compress((Bytef *)output_string->data(), &outSize,
                     (Bytef *)uncompressed_string->data(), outSize);
        assert(outSize != uncompressed_string->size());
        output_string->resize(outSize);
    };

} // namespace qAlgorithms