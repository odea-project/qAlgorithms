#include "qalgorithms_read_file.h"
#include "qalgorithms_datatypes.h"

#include "../external/pugixml/pugixml.hpp"
#include "../external/simdutf/simdutf.h" // use a fast base64 decode function that makes proper use of SIMD

// #include <filesystem>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

//  problems linking against zlib-ng with mingw under windows, use this bypass until a proper solution is required
#if __has_include(<zlib-ng.h>)
    #include <zlib-ng.h>
#else
    #include <zlib.h>
inline int32_t zng_uncompress(uint8_t *dest, size_t *destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf destLen2 = *destLen;
    uint32_t z_ret = uncompress(dest, &destLen2, source, sourceLen);
    *destLen = destLen2;
    return z_ret;
}
inline size_t zng_compressBound(size_t sourceLen)
{
    return compressBound(sourceLen);
}
inline int32_t zng_compress(uint8_t *dest, size_t *destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf destLen2 = *destLen;
    uint32_t z_ret = compress(dest, &destLen2, source, sourceLen);
    *destLen = destLen2;
    return z_ret;
}
#endif
// since we care mostly about speed, we want to use the generally faster zlib-ng for decompression.
// However, we cannot be sure that it exists for a given system. Therefore, it is only included if
// it can be installed for the host

namespace qAlgorithms
{
    size_t bytesToFloatVec(const std::vector<char> *bytes, const bool isDouble,
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

    static bool isCentroided_fun(const XML_File *file);

    static Polarities get_polarity_mode(const XML_File *file)
    {
        const size_t count = file->linknodes->size();
        assert(count > 1);

        Polarities polarity_prev = spectrum_polarity(file, 0);
        for (size_t specNum = 1; specNum < count; ++specNum)
        {
            Polarities polarity = spectrum_polarity(file, specNum);
            if (polarity != polarity_prev)
                return Polarities::mixed;
        }
        return polarity_prev;
    };

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
            (void)fprintf(stderr, "Error: .mzML file could not be opened. Error description:\n%s\n",
                          loading_result.description());
            defective = true;
            return;
        }

        // traverse the xml tree while ensuring that failure occurs at the first possible moment
        assert(std::strcmp("indexedmzML", mzml_root_node.name()) == 0);
        mzml_root_node = mzml_root_node.first_child();
        assert(std::strcmp("mzML", mzml_root_node.name()) == 0);

        // qAlgorithms is not designed to process data which consists of more than one sample
        volatile bool sampleListExists = !mzml_root_node.child("sampleList").empty();
        volatile bool sourceFileListExists = !mzml_root_node.child("fileDescription").child("sourceFileList").empty();
        if (sampleListExists)
        {
            if (mzml_root_node.child("sampleList").attribute("count").value()[0] != '1')
            {
                // assert();
                //@todo check for expected conversion tool use
            }
        }
        if (sourceFileListExists)
        {
            //@todo see above
        }
        if (!(sampleListExists || sourceFileListExists))
        {
            defective = true;
            (void)fprintf(stderr, "Error: the supplied mzML file contains more than one sample.\n"
                                  "This probably means that a file containing intermediate (aggregated) results is\n"
                                  "supplied instead of raw data. If the file is correct, inspect the conversion pipeline.\n");
            return;
        }

        assert(mzml_root_node.child("run"));

        pugi::xml_node spec_list = mzml_root_node.child("run").child("spectrumList");
        assert(spec_list);

        number_spectra = spec_list.attribute("count").as_uint();

        if (number_spectra == 0)
        {
            defective = true;
            (void)fprintf(stderr, "Error: the supplied mzML file does not contain any spectra.\n");
            return;
        }

        linknodes = new std::vector<pugi::xml_node>(number_spectra);
        linknodes->clear();
        for (pugi::xml_node child = spec_list.first_child(); (bool)child; child = child.next_sibling())
        {
            linknodes->push_back(child);
        }
        assert(linknodes->size() == number_spectra);

        zlib_compression = spectrum_is_compressed(this, 0);

        precision_f64 = spectrum_is_float64(this, 0);

        isCentroided = isCentroided_fun(this);

        polarityMode = get_polarity_mode(this);
        assert(polarityMode != Polarities::unknown_polarity);
    };

    void XML_File::free_linknodes()
    {
        delete linknodes;
        defective = true;
    };

    int32_t get_spectrum(const XML_File *file, // this only extracts data that is in profile mode.
                         std::vector<float> *const spectrum_mz,
                         std::vector<float> *const spectrum_int,
                         size_t index)
    {
        // @todo the entire function needs a rework

        assert(spectrum_mz->empty() && spectrum_int->empty());
        assert(!file->defective);

        if (file->linknodes->size() == 0)
        {
            (void)fprintf(stderr, "Error: no spectra found for index %zu\n", index);
            return 1;
        }

        const pugi::xml_node *spectrum_node = file->linknodes->data() + index;

        pugi::xml_node node_binary_list = spectrum_node->child("binaryDataArrayList");

        size_t number_traces = spectrum_node->attribute("defaultArrayLength").as_uint();

        pugi::xml_named_node_iterator dataArray = node_binary_list.children("binaryDataArray").begin();
        assert(dataArray != node_binary_list.children("binaryDataArray").end());

        std::vector<char> buffer;
        { // extract mz values
            pugi::xml_node node_binary = dataArray->child("binary");
            const char *encoded_string = node_binary.child_value();
            std::vector<char> decoded_string = decode_base64(encoded_string);

            // error handling
            if (decoded_string.empty())
            {
                (void)fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 \n"
                                      "correctly. Ensure the input file is not corrupted.\n",
                              index);
                return 2;
            }

            if (file->zlib_compression)
            {
                size_t expectedSize = decoded_string.size() * 6;
                buffer.resize(expectedSize);
                zng_uncompress((uint8_t *)buffer.data(), &expectedSize,
                               (uint8_t *)decoded_string.data(), decoded_string.size());
                // decompress_zlib(&decoded_string, &buffer);
                // check that less characters have been written than fit into the buffer
                assert(buffer.size() > expectedSize);
                buffer.resize(expectedSize);
                bytesToFloatVec(&buffer, file->precision_f64, spectrum_mz);
            }
            else
            {
                bytesToFloatVec(&decoded_string, file->precision_f64, spectrum_mz);
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
                (void)fprintf(stderr, "Error: spectrum %zu could not be decoded as base64 "
                                      "correctly. Ensure the input file is not corrupted.\n",
                              index);
                return 2;
            }

            if (file->zlib_compression)
            {
                size_t expectedSize = decoded_string.size() * 6;
                buffer.resize(expectedSize);
                zng_uncompress((uint8_t *)buffer.data(), &expectedSize,
                               (uint8_t *)decoded_string.data(), decoded_string.size());
                // decompress_zlib(&decoded_string, &buffer);
                // check that less characters have been written than fit into the buffer
                assert(buffer.size() > expectedSize);
                buffer.resize(expectedSize);
                bytesToFloatVec(&buffer, file->precision_f64, spectrum_int);
            }
            else
            {
                bytesToFloatVec(&decoded_string, file->precision_f64, spectrum_int);
            }

            assert(spectrum_int->size() == number_traces); // this happens if an index is tried which does not exist in the data
        }
        return 0;
    };

    static float extract_scan_RT(const pugi::xml_node *spec)
    {
        pugi::xml_node rt_node = spec->child("scanList").child("scan").find_child_by_attribute("cvParam", "name", "scan start time");

        float rt_val = rt_node.attribute("value").as_float();
        const char *rt_unit = rt_node.attribute("unitName").as_string();
        bool unit_secs = strcmp(rt_unit, "second") == 0; // strcmp returns 0 for equal strings
        if (!unit_secs)
        {
            assert(strcmp(rt_unit, "minute") == 0);
        }

        return unit_secs ? rt_val : rt_val * 60;
    };

    void get_spectra_RT(const XML_File *data, const std::vector<uint32_t> *indices, std::vector<float> *const RTs)
    {
        const size_t idxSize = indices->size();
        assert(!data->defective);
        assert(idxSize > 0);

        RTs->resize(idxSize);

        for (size_t i = 0; i < idxSize; ++i)
        {
            size_t idx = indices->at(i);
            const pugi::xml_node *spec = data->linknodes->data() + idx;
            float RT = extract_scan_RT(spec);
            RTs->at(i) = RT;
        }
    };

    std::vector<uint32_t> filter_spectra(const XML_File *file,
                                         const bool ms1,
                                         const Polarities polarity,
                                         const bool profile_mode)
    {
        // return a vector of all indices that are relevant to the query. Properties are checked in order of regularity.
        assert(!file->defective);
        const size_t numSpectra = file->number_spectra;
        assert(numSpectra > 0);
        std::vector<uint32_t> indices;
        indices.reserve(numSpectra);

        for (uint32_t specNum = 0; specNum < numSpectra; specNum++)
        {
            bool isProfile = spectrum_is_profile(file, specNum);
            if (isProfile != profile_mode) // this does not allow for processing of partially centroided data - change?
                continue;

            Polarities polarity_spec = spectrum_polarity(file, specNum);
            if (polarity_spec != polarity)
                continue;

            uint32_t level = spectrum_ms_level(file, specNum);
            bool isMS1 = 1 == level;
            if (isMS1 != ms1)
                continue; // only ms1 or msn data can be retrieved at once.

            indices.push_back(specNum);
        }
        indices.shrink_to_fit();
        return indices;
    }

    static bool isCentroided_fun(const XML_File *file)
    {
        size_t centroided = 0;
        size_t profile = 0;

        for (size_t specNum = 0; specNum < file->number_spectra; ++specNum)
        {
            uint32_t level = spectrum_ms_level(file, specNum);
            if (level != 1)
                continue;

            if (spectrum_is_profile(file, specNum))
            {
                profile += 1;
            }
            else
            {
                centroided += 1;
            }
        }

        if (centroided > 0)
        {
            assert(profile == 0);
            return true;
        }
        return false;
    }

    bool spectrum_is_compressed(const XML_File *file, const size_t specNum)
    {
        assert(specNum < file->number_spectra);
        const pugi::xml_node *spec = file->linknodes->data() + specNum;
        // Per standard, the compression should be specified at the binaryDataArrayList level.
        // Going by actual output files, this is not the case.
        const pugi::xml_node binaryDataArrayNode = spec->child("binaryDataArrayList")
                                                       .child("binaryDataArray");

        bool isCompressed = !binaryDataArrayNode.find_child_by_attribute("cvParam",
                                                                         "accession",
                                                                         "MS:1000574")
                                 .empty();
        if (isCompressed)
            return true;

        bool isNotCompressed = !binaryDataArrayNode.find_child_by_attribute("cvParam",
                                                                            "accession",
                                                                            "MS:1000576")
                                    .empty();
        assert(isNotCompressed);
        return false;
    }

    bool spectrum_is_float64(const XML_File *file, const size_t specNum)
    {
        assert(specNum < file->number_spectra);
        const pugi::xml_node *spec = file->linknodes->data() + specNum;
        const pugi::xml_node binaryDataArrayNode = spec->child("binaryDataArrayList")
                                                       .child("binaryDataArray");

        bool isDouble = !binaryDataArrayNode.find_child_by_attribute("cvParam",
                                                                     "accession",
                                                                     "MS:1000523")
                             .empty();
        if (isDouble)
            return true;

        bool isFloat = !binaryDataArrayNode.find_child_by_attribute("cvParam",
                                                                    "accession",
                                                                    "MS:1000521")
                            .empty();
        assert(isFloat);
        return false;
    }

    bool spectrum_is_profile(const XML_File *file, const size_t specNum)
    {
        assert(specNum < file->number_spectra);
        const pugi::xml_node *spec = file->linknodes->data() + specNum;
        // values taken from https://peptideatlas.org/tmp/mzML1.1.0.html
        bool isProfile = !spec->find_child_by_attribute("cvParam",
                                                        "accession",
                                                        "MS:1000128")
                              .empty();
        return isProfile;
    }

    int32_t spectrum_ms_level(const XML_File *file, const size_t specNum)
    {
        assert(specNum < file->number_spectra);
        const pugi::xml_node *spec = file->linknodes->data() + specNum;
        // values taken from https://peptideatlas.org/tmp/mzML1.1.0.html
        int32_t ms_lvl = spec->find_child_by_attribute("cvParam",
                                                       "name",
                                                       "ms level")
                             .attribute("value")
                             .as_int();
        return ms_lvl;
    }

    Polarities spectrum_polarity(const XML_File *file, const size_t specNum)
    {
        assert(specNum < file->number_spectra);
        const pugi::xml_node *spec = file->linknodes->data() + specNum;

        bool positive_scan = !spec->find_child_by_attribute("cvParam",
                                                            "accession",
                                                            "MS:1000130")
                                  .empty();
        if (positive_scan)
            return Polarities::positive;

        bool negative_scan = !spec->find_child_by_attribute("cvParam",
                                                            "accession",
                                                            "MS:1000129")
                                  .empty();
        if (negative_scan)
            return Polarities::negative;

        return Polarities::unknown_polarity;
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

    void compress_zlib(const std::vector<char> *uncompressed_string, std::vector<char> *output_string)
    {
        // worst case: no compression (probably impossible)
        output_string->resize(uncompressed_string->size());
        size_t outSize = output_string->size();
        zng_compress((Bytef *)output_string->data(), &outSize,
                     (Bytef *)uncompressed_string->data(), outSize);
        assert(outSize != uncompressed_string->size());
        output_string->resize(outSize);
    };

    void compress_and_encode_dbl(std::vector<double> *input_dbl, std::vector<char> *output_string)
    {
        // cast double array to char array for transform
        const size_t length = input_dbl->size() * sizeof(double);
        const char *input = (const char *)input_dbl->data();

        // compress using zlib-ng
        std::vector<char> buffer(zng_compressBound(length));
        size_t outSize = length;
        zng_compress((Bytef *)buffer.data(), &outSize,
                     (Bytef *)input, length);
        assert(outSize <= buffer.size());

        // encode as base64
        size_t base64_size = simdutf::base64_length_from_binary(outSize);
        output_string->resize(base64_size);
        const size_t written = simdutf::binary_to_base64(buffer.data(), outSize, output_string->data());
        assert(written == base64_size);
    }

    template <typename T>
    void compress_and_encode(const std::vector<T> *input, std::vector<char> *output_string)
    {
        static_assert(std::is_trivially_copyable_v<T>);

        // cast double array to char array for transform
        const size_t length = input->size() * sizeof(T);
        const char *input_arr = (const char *)input->data();

        // compress using zlib-ng
        const size_t compressionBound = zng_compressBound(length);
        std::vector<char> buffer(compressionBound);
        size_t outSize = compressionBound;
        zng_compress((Bytef *)buffer.data(), &outSize,
                     (Bytef *)input_arr, length);
        assert(outSize <= buffer.size());

        // encode as base64
        size_t base64_size = simdutf::base64_length_from_binary(outSize);
        output_string->resize(base64_size);
        const size_t written = simdutf::binary_to_base64(buffer.data(), outSize, output_string->data());
        assert(written == base64_size);
    }

} // namespace qAlgorithms