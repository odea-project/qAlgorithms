#ifndef QALGORITHMS_READ_FILE_H
#define QALGORITHMS_READ_FILE_H

#include <vector>
#include <string>
#include "qalgorithms_datatypes.h"

#define PUGIXML_HEADER_ONLY
#include "../external/pugixml-1.14/src/pugixml.h"

namespace qAlgorithms
{
    // @todo split off into "read file" and "read mzml", where the mzml reader just prepares an intermediate struct, processing of which is handled separately

#if defined(_WIN32)
    typedef wchar_t path_char;
#elif
    typedef char path_char;
#endif

    struct BinaryMetadata // @todo there is no need for a file-specific metadata object
    {
        std::string data_name_short;
        int index;
        bool compressed;
        bool isDouble;
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
        bool polarity; // 0 = negative, 1 = positive
    };

    static std::vector<std::string> possible_accessions_binary_data_mzML = {
        "MS:1000514", "MS:1000515", "MS:1000516", "MS:1000517",
        "MS:1000595", "MS:1000617", "MS:1000786", "MS:1000820",
        "MS:1000821", "MS:1000822", "MS:1002478", "MS:1002529",
        "MS:1002530", "MS:1002742", "MS:1002743", "MS:1002744",
        "MS:1002745", "MS:1002893", "MS:1003143", "MS:1003157",
        "MS:1003158"};

    static std::vector<std::string> possible_short_name_binary_data_mzML = {
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
        std::vector<BinaryMetadata> spectra_binary_metadata;

        BinaryMetadata extract_binary_metadata(const pugi::xml_node &bin);

        pugi::xml_document mzml_base_document;

        pugi::xml_parse_result loading_result;

        // pugi::xml_node mzml_root_node;

        const std::vector<pugi::xml_node> *linknodes; // allocation during initialisation, has to be deallocated manually with the supplied function

        bool isCentroided_fun();

        Polarities get_polarity_mode();

    public:
        pugi::xml_node mzml_root_node;

        SourceFileType filetype = unknown_filetype;

        unsigned int number_spectra;

        unsigned int number_spectra_binary_arrays; // @todo remove

        Polarities polarityMode;

        bool defective = false;

        bool isCentroided = false;

        XML_File(const path_char *fileconst, SourceFileType type);

        void get_spectrum(
            std::vector<float> *const spectrum_mz,
            std::vector<float> *const spectrum_int,
            size_t index);

        // return all indices of spectra that match the required criteria
        std::vector<unsigned int> filter_spectra(const bool ms1, bool polarity, bool centroided); // @todo this is only useable to select MS1 or MS2

        std::vector<float> get_spectra_RT(const std::vector<unsigned int> *indices);

        void free_linknodes();
    };
};

#endif // QALGORITHMS_READ_FILE_H