#ifndef QALGORITHMS_READ_FILE_H
#define QALGORITHMS_READ_FILE_H

#include <vector>
#include <string>
#include <filesystem>
#include "qalgorithms_datatypes.h"

#define PUGIXML_HEADER_ONLY
#include "../external/pugixml-1.14/src/pugixml.h"

namespace qAlgorithms
{
    struct BinaryMetadata // @todo there is no need for a file-specific metadata object
    {
        std::string data_name_short;
        int index;
        bool compressed;
        bool isDouble;
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

    enum Filetype
    {
        mzML,
        mzXML,
        other
    };

    struct XML_Attribute
    {
        char *name;
        char *attr_name;
        char *attr_value;
    };

    class XML_File // @todo this is just a complicated way of having a filetype specific accession struct and a generalised container
    {
        // @todo change this to a generalised XML class which includes cases for mzML, mzXML and others
    private:
        std::vector<BinaryMetadata> spectra_binary_metadata;

        BinaryMetadata extract_binary_metadata(const pugi::xml_node &bin);

        double extract_scan_RT(const pugi::xml_node &spec);

        pugi::xml_document mzml_base_document;

        pugi::xml_parse_result loading_result;

        // pugi::xml_node mzml_root_node;

        // std::vector<pugi::xml_node> spectra; // making this a class member makes the code unbelieveably slow

    public:
        pugi::xml_node mzml_root_node;

        Filetype filetype = other;

        unsigned int number_spectra;

        unsigned int number_spectra_binary_arrays;

        bool defective = false;

        XML_File(const std::filesystem::path &file);

        void get_spectrum(
            const std::vector<pugi::xml_node> *spectra_nodes_ex,
            std::vector<double> *const spectrum_mz,
            std::vector<double> *const spectrum_RT,
            size_t index);

        std::vector<unsigned int> get_spectra_index(const std::vector<unsigned int> *indices,
                                                    const std::vector<pugi::xml_node> *spectra_nodes_ex);
        std::vector<int> get_spectra_level(const std::vector<unsigned int> *indices,
                                           const std::vector<pugi::xml_node> *spectra_nodes_ex);
        std::vector<bool> get_spectra_mode(const std::vector<pugi::xml_node> *spectra_nodes_ex);
        std::vector<bool> get_spectra_polarity(const std::vector<unsigned int> *indices,
                                               const std::vector<pugi::xml_node> *spectra_nodes_ex);

        // return all indices of spectra that match the required criteria
        std::vector<unsigned int> filter_spectra(
            const std::vector<pugi::xml_node> *spectra_nodes_ex,
            bool ms1, bool polarity, bool centroided); // @todo this is only useable to select MS1 or MS2

        Polarities get_polarity_mode(const size_t count,
                                     const std::vector<pugi::xml_node> *spectra_nodes_ex);
        std::vector<float> get_spectra_RT(const std::vector<unsigned int> *indices,
                                          const std::vector<pugi::xml_node> *spectra_nodes_ex);
    };
    const std::vector<pugi::xml_node> link_vector_spectra_nodes(pugi::xml_node mzml_root_node);
};

#endif // QALGORITHMS_READ_FILE_H