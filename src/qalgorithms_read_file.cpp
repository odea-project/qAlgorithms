#include "qalgorithms_read_file.h"
#include "qalgorithms_datatypes.h"

#include "../external/pugixml/pugixml.h"
#include "../external/simdutf/simdutf.h" // use a fast base64 decode function that makes proper use of SIMD

// #include <filesystem>
#include <cassert>
#include <string>
#include <vector>
#include <zlib.h>

namespace qAlgorithms
{
    int bytesToFloatVec(const std::vector<char> *bytes, const bool isDouble,
                        std::vector<float> *result)
    {
        // cast the byte array resulting from zlib decompression to a float array
        const size_t fsize = sizeof(float);
        const size_t dsize = sizeof(double);
        assert(bytes->size() % (isDouble ? dsize : fsize) == 0);
        int lengthDecoded = bytes->size() / (isDouble ? dsize : fsize);

        result->reserve(lengthDecoded);

// for only this block, ignore the alignment change. It is intended behavouir.
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
        if (isDouble)
        {
            const double *dbl = (const double *)bytes->data();
            for (int i = 0; i < lengthDecoded; ++i)
            {
                result->push_back((float)dbl[i]); // result must be cast to float individually
            }
        }
        else
        {
            // encountered one relevant file "in the wild", but this is still rare enough to warrant a warning
            // @todo replace with memcpy
            const float *flt = (const float *)bytes->data();
            for (int i = 0; i < lengthDecoded; ++i)
            {
                result->push_back(flt[i]);
            }
        }
#pragma clang diagnostic pop
        return lengthDecoded;
    };

    // Decompresses a string using the zlib library (https://zlib.net/).
    void decompress_zlib(const std::vector<char> *compressed_string, std::vector<char> *output_string)
    {
        // max expected compression is factor 6
        output_string->resize(compressed_string->size() * 6);
        // zlib struct
        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;
        // setup input and output
        infstream.avail_in = compressed_string->size();         // size of input
        infstream.next_in = (Bytef *)compressed_string->data(); // input char array
        infstream.avail_out = output_string->size();            // maximum size of output
        infstream.next_out = (Bytef *)output_string->data();    // output char array

        // the actual DE-compression work.
        inflateInit(&infstream);
        auto ret_1 = inflate(&infstream, Z_NO_FLUSH);
        assert(ret_1 == 1);
        auto ret_2 = inflateEnd(&infstream);
        assert(ret_2 == 0);
        // since resize does not deallocate, this just ensures we can use the vector without making compromises later
        output_string->resize(infstream.total_out);
    };

    // Decodes a Base64 string into a string with binary data using the simdutf library subset chosen by '--with-base64'
    // (https://github.com/simdutf/simdutf/tree/master?tab=readme-ov-file#single-header-version-with-limited-features).
    std::vector<char> decode_base64(const std::string &encoded_string)
    {
        size_t length = encoded_string.size() / 4 * 3;
        std::vector<char> output(length);
        auto simd_res = simdutf::base64_to_binary(encoded_string.c_str(), encoded_string.size(), output.data());

        if (simd_res.error != 0) // [[unlikely]]
        {
            return {0}; // error message is handled one function above
        }
        output.resize(simd_res.count);
        return output;
    };

    XML_File::XML_File(const path_char *file, const SourceFileType type)
    {
        filetype = type;
        assert(filetype == mzML); // @todo support mzXML and other xml based formats

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

        number_spectra = spec_list.attribute("count").as_int();

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
        for (pugi::xml_node child = spec_list.first_child(); child; child = child.next_sibling())
        {
            linknodes->push_back(child);
        }
        assert(linknodes->size() == number_spectra);

        isCentroided = isCentroided_fun();

        polarityMode = get_polarity_mode();
    };

    void XML_File::free_linknodes()
    {
        if (linknodes != nullptr)
            delete linknodes;
        defective = true;
    };

    BinaryMetadata XML_File::extract_binary_metadata(const pugi::xml_node &bin)
    {
        // extract type of number representation in binary data
        bool type_double = false, type_float = false, type_int32 = false, type_int64 = false;

        for (pugi::xml_node cvParam = bin.child("cvParam"); cvParam; cvParam = cvParam.next_sibling("cvParam"))
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
        assert(has_bin_data_type); // Encoded data type could not be found matching the mzML official vocabulary

        if (mtd.data_name_short == "other") // this should be separated into a check for null / termination condition @todo
        {
            mtd.data_name_short += ": " + tmp;
        }

        return mtd;
    }

    void XML_File::get_spectrum( // this obviously only extracts data that is in profile mode.
        std::vector<float> *const spectrum_mz,
        std::vector<float> *const spectrum_int,
        size_t index)
    {
        assert(spectrum_mz->empty() && spectrum_int->empty());
        assert(!this->defective);

        if (linknodes->size() == 0)
        {
            fprintf(stderr, "Error: no spectra found for index %zu\n", index);
            return;
        }

        const pugi::xml_node &spectrum_node = (*linknodes)[index];

        pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");

        unsigned int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

        auto dataArray = node_binary_list.children("binaryDataArray").begin();
        assert(dataArray != node_binary_list.children("binaryDataArray").end());

        std::vector<char> buffer;
        { // extract mz values
            pugi::xml_node node_binary = dataArray->child("binary");
            std::string encoded_string = node_binary.child_value();
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.empty())
            {
                fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 correctly. Ensure the input file is not corrupted.\n", index);
                return; // implement a defined way to skip the current file? @todo
            }

            if (mtd_mz.compressed)
            {
                buffer.clear();
                decompress_zlib(&decoded_string, &buffer);
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
                fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 correctly. Ensure the input file is not corrupted.\n", index);
                return; // implement a defined way to skip the current file? @todo
            }

            if (mtd_intensity.compressed)
            {
                buffer.clear();
                decompress_zlib(&decoded_string, &buffer);
                bytesToFloatVec(&buffer, mtd_intensity.isDouble, spectrum_int);
            }
            else
            {
                bytesToFloatVec(&decoded_string, mtd_intensity.isDouble, spectrum_int);
            }

            assert(spectrum_int->size() == number_traces); // this happens if an index is tried which does not exist in the data
            // assert(spectra_binary_metadata[1].data_name_short == "intensity");
        }
    };

    double extract_scan_RT(const pugi::xml_node &spec)
    {
        pugi::xml_node rt_node = spec.child("scanList").child("scan").find_child_by_attribute("cvParam", "name", "scan start time");

        double rt_val = rt_node.attribute("value").as_double();
        const char *rt_unit = rt_node.attribute("unitName").as_string();
        bool unit_secs = strcmp(rt_unit, "second") == 0; // strcmp returns 0 for equal strings
        if (!unit_secs)
        {
            assert(strcmp(rt_unit, "minute") == 0);
        }

        return unit_secs ? rt_val : rt_val * 60;
    };

    std::vector<float> XML_File::get_spectra_RT(const std::vector<unsigned int> *indices)
    {
        assert(indices->size() > 0);
        assert(!this->defective);

        std::vector<float> retention_times;

        for (size_t i = 0; i < indices->size(); ++i)
        {
            size_t idx = indices->at(i);
            pugi::xml_node spec = (*linknodes)[idx];
            double RT = extract_scan_RT(spec);
            retention_times.push_back(RT);
        }

        return retention_times; // 4800869
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

    std::vector<unsigned int> XML_File::filter_spectra(
        bool ms1, bool polarity, bool centroided)
    {
        // @todo this function should take a struct with the field names as input and switch it depending on format specifics

        // return a vector of all indices that are relevant to the query. Properties are checked in order of regularity.
        assert(!this->defective);
        assert(this->number_spectra > 0);
        std::vector<unsigned int> indices;
        indices.reserve(this->number_spectra);

        for (unsigned int i = 0; i < this->number_spectra; i++)
        {
            pugi::xml_node spec = (*linknodes)[i];

            bool isCentroid = spec.find_child_by_attribute("cvParam", "accession", "MS:1000127");
            if (isCentroid != centroided)
                continue; // this does not allow for processing of partially centroided data

            bool polarityPos = spec.find_child_by_attribute("cvParam", "accession", "MS:1000130");
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

            if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000128"))
            {
                profile += 1;
                continue; // profile mode
            }
            centroided += 1;
        }

        if (centroided == 0)
            return false;

        if (profile == 0)
            return true;

        if (profile / centroided < 2) // @todo this is a suboptimal solution
            return true;

        return false;
    }
} // namespace qAlgorithms