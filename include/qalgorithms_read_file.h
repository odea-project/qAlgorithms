#ifndef QALGORITHMS_READ_FILE_H
#define QALGORITHMS_READ_FILE_H

#include "qalgorithms_datatypes.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#define PUGIXML_HEADER_ONLY
#include "../external/pugixml/pugixml.hpp"

namespace qAlgorithms
{
    enum SourceFileType // NOLINT (performance-enum-size)
    {
        unknown_filetype,
        mzML,
        mzxML,
        hdf5,
        json,
    };

    struct SpectrumData // this information is required by qAlgorithms to function
    {
        size_t spectrum_index = 0;     // start at 1
        size_t spectrum_numPoints = 0; // profile points or centroids in this spectrum
        uint32_t mode = 0;             // 1 = profile, 2 = centroid
        uint32_t MS_level = 0;
        bool polarity = false; // 0 = negative, 1 = positive
    };

    // struct mzML_schema
    // {
    //     const char accession[11];
    //     const char short_name[35];
    // };

    // const mzML_schema mzML_fields[] =
    //     {
    //         {"MS:1000514", "mz"},
    //         {"MS:1000515", "intensity"},
    //         {"MS:1000516", "charge"},
    //         {"MS:1000517", "sn"},
    //         {"MS:1000595", "time"},
    //         {"MS:1000617", "wavelength"},
    //         {"MS:1000786", "other"},
    //         {"MS:1000820", "flowrate"},
    //         {"MS:1000821", "pressure"},
    //         {"MS:1000822", "temperature"},
    //         {"MS:1002478", "mean_charge"},
    //         {"MS:1002529", "resolution"},
    //         {"MS:1002530", "baseline"},
    //         {"MS:1002742", "noise"},
    //         {"MS:1002743", "sampled_noise_mz"},
    //         {"MS:1002744", "sampled_noise_intensity"},
    //         {"MS:1002745", "sampled_noise_baseline"},
    //         {"MS:1002893", "ion_mobility"},
    //         {"MS:1003143", "mass"},
    //         {"MS:1003157", "quadrupole_position_lower_bound_mz"},
    //         {"MS:1003158", "quadrupole_position_upper_bound_mz"}};

    /* ### WARNING: THIS CONSTRUCTOR ALLOCATES A SEPARATE ARRAY ### */
    struct XML_File
    {
        // @todo change this to a generalised XML document interface for mass spec data

        pugi::xml_document mzml_base_document;

        pugi::xml_parse_result loading_result;

        std::vector<pugi::xml_node> *linknodes = nullptr; // allocation during initialisation, has to be deallocated manually with the supplied function

        pugi::xml_node mzml_root_node;

        SourceFileType filetype = unknown_filetype;

        uint32_t number_spectra = 0;

        Polarities polarityMode = Polarities::unknown_polarity;

        bool defective = false;

        bool isCentroided = false;

        bool zlib_compression = true; // per standard, only zlib is possible (note: i have never seen an uncompressed file)

        bool precision_f64 = false;

        XML_File(const path_char *filepath, SourceFileType type);

        // copy constructor or assignment could lead to double-free / accessing a dead pointer
        XML_File(XML_File &) = delete;
        XML_File(XML_File &&) = delete;
        XML_File &operator=(XML_File &&) = delete;

        // return all indices of spectra that match the required criteria

        void free_linknodes();
    };

    std::vector<uint32_t> filter_spectra(const XML_File *file,
                                         const bool ms1,
                                         const Polarities polarity,
                                         const bool profile_mode);

    void get_spectra_RT(const XML_File *data,
                        const std::vector<uint32_t> *indices,
                        std::vector<float> *const RTs);

    int32_t get_spectrum(const XML_File *file, // this only extracts data that is in profile mode.
                         std::vector<float> *const spectrum_mz,
                         std::vector<float> *const spectrum_int,
                         size_t index);

    bool spectrum_is_compressed(const XML_File *file, const size_t specNum);

    bool spectrum_is_float64(const XML_File *file, const size_t specNum);

    bool spectrum_is_profile(const XML_File *file, const size_t specNum);

    int32_t spectrum_ms_level(const XML_File *file, const size_t specNum);

    Polarities spectrum_polarity(const XML_File *file, const size_t specNum);

    // functions exposed for testing
    std::vector<char> decode_base64(const std::string &encoded_string);

    std::vector<char> encode_base64_dbl(const double *input_dbl, const size_t in_len_dbl);

    void compress_zlib(const std::vector<char> *uncompressed_string, std::vector<char> *output_string);

    void compress_and_encode(const uint8_t *input_arr,
                             const size_t length,
                             std::vector<char> *output_string);
}; // namespace qAlgorithms

#endif // QALGORITHMS_READ_FILE_H