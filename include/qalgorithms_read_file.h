#ifndef QALGORITHMS_READ_FILE_H
#define QALGORITHMS_READ_FILE_H

#include "qalgorithms_datatypes.h"
#include <string>
#include <vector>

#define PUGIXML_HEADER_ONLY
#include "../external/pugixml/pugixml.hpp"

#if defined(_WIN32)
typedef wchar_t path_char;
#else
typedef char path_char;
#endif

namespace qAlgorithms
{
    struct BinaryMetadata // @todo there is no need for a file-specific metadata object. Is it possible for different spectra and properties to be compressed / uncompressed?
    {
        std::string data_name_short;
        bool compressed = false;
        bool isDouble = false;
    };

    enum SourceFileType
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
        int mode = 0;                  // 1 = profile, 2 = centroid
        int MS_level = 0;
        bool polarity = false; // 0 = negative, 1 = positive
    };

    const std::vector<std::string> possible_accessions_binary_data_mzML = {
        "MS:1000514", "MS:1000515", "MS:1000516", "MS:1000517",
        "MS:1000595", "MS:1000617", "MS:1000786", "MS:1000820",
        "MS:1000821", "MS:1000822", "MS:1002478", "MS:1002529",
        "MS:1002530", "MS:1002742", "MS:1002743", "MS:1002744",
        "MS:1002745", "MS:1002893", "MS:1003143", "MS:1003157",
        "MS:1003158"};

    const std::vector<std::string> possible_short_name_binary_data_mzML = {
        "mz", "intensity", "charge", "sn",
        "time", "wavelength", "other", "flowrate",
        "pressure", "temperature", "mean_charge", "resolution",
        "baseline", "noise", "sampled_noise_mz", "sampled_noise_intensity",
        "sampled_noise_baseline", "ion_mobility", "mass", "quadrupole_position_lower_bound_mz",
        "quadrupole_position_upper_bound_mz"};

    /* ### WARNING: THIS CONSTRUCTOR ALLOCATES A SEPARATE ARRAY ### */
    class XML_File
    {
        // @todo change this to a generalised XML document interface for mass spec data
      private:
        // std::vector<BinaryMetadata> spectra_binary_metadata;
        BinaryMetadata mtd_mz{}, mtd_intensity{};

        pugi::xml_document mzml_base_document;

        pugi::xml_parse_result loading_result;

        std::vector<pugi::xml_node> *linknodes = nullptr; // allocation during initialisation, has to be deallocated manually with the supplied function

        bool isCentroided_fun();

        Polarities get_polarity_mode();

      public:
        pugi::xml_node mzml_root_node;

        SourceFileType filetype = unknown_filetype;

        unsigned int number_spectra = 0;

        Polarities polarityMode = Polarities::unknown_polarity;

        bool defective = false;

        bool isCentroided = false;

        XML_File(const path_char *fileconst, SourceFileType type);

        int get_spectrum(
            std::vector<float> *const spectrum_mz,
            std::vector<float> *const spectrum_int,
            size_t index);

        // return all indices of spectra that match the required criteria
        std::vector<unsigned int> filter_spectra(const bool ms1, bool polarity, bool centroided); // @todo this is only useable to select MS1 or MS2

        void get_spectra_RT(const std::vector<unsigned int> *indices, std::vector<float> *const RTs);

        void free_linknodes();
    };

    // functions exposed for testing
    std::vector<char> decode_base64(const std::string &encoded_string);

    std::string encode_base64_flt(const float *input_flt, const size_t in_len_flt);

    int bytesToFloatVec(const std::vector<char> *bytes, const bool isDouble,
                        std::vector<float> *result);
}; // namespace qAlgorithms

#endif // QALGORITHMS_READ_FILE_H