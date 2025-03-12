#ifndef STREAMCRAFT_MZML_HPP
#define STREAMCRAFT_MZML_HPP

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <filesystem>

#define PUGIXML_HEADER_ONLY

#ifndef PUGIXML_PATH
#define PUGIXML_PATH "../../pugixml-1.14/src/pugixml.h"
#endif

#include PUGIXML_PATH

#include "StreamCraft_utils.hpp"

namespace StreamCraft
{
    // enum SpectrumMode
    // {
    //     centroid;
    //     profile;
    // };

    class MZML_BINARY_METADATA // @todo there is no need for a file-specific metadata object
    {

    public:
        int index;
        // std::string precision_name;
        // std::string precision_accession;
        int precision_int;
        // std::string precision_type;
        std::string compression;
        bool compressed;
        // std::string data_name;
        // std::string data_accession;
        std::string data_value;
        // std::string data_unit;
        std::string data_name_short;
    };

    struct SpectrumData // this information is required by qAlgorithms to function
    {
        size_t spectrum_index = 0;     // start at 1
        size_t spectrum_numPoints = 0; // profile points or centroids in this spectrum
        int mode = 0;                  // 1 = profile, 2 = centroid
        int MS_level = 0;
        bool polarity; // 0 = negative, 1 = positive
    };

    class MZML // @todo this is just a complicated way of having a filetype specific accession struct and a generalised container
    {
    private:
        std::vector<MZML_BINARY_METADATA> spectra_binary_metadata;

        std::vector<std::string> possible_accessions_binary_data = {
            "MS:1000514", "MS:1000515", "MS:1000516", "MS:1000517",
            "MS:1000595", "MS:1000617", "MS:1000786", "MS:1000820",
            "MS:1000821", "MS:1000822", "MS:1002478", "MS:1002529",
            "MS:1002530", "MS:1002742", "MS:1002743", "MS:1002744",
            "MS:1002745", "MS:1002893", "MS:1003143", "MS:1003157",
            "MS:1003158"};

        std::vector<std::string> possible_short_name_binary_data = {
            "mz", "intensity", "charge", "sn",
            "time", "wavelength", "other", "flowrate",
            "pressure", "temperature", "mean_charge", "resolution",
            "baseline", "noise", "sampled_noise_mz", "sampled_noise_intensity",
            "sampled_noise_baseline", "ion_mobility", "mass", "quadrupole_position_lower_bound_mz",
            "quadrupole_position_upper_bound_mz"};

        std::vector<pugi::xml_node> link_vector_spectra_nodes();

        MZML_BINARY_METADATA extract_binary_metadata(const pugi::xml_node &bin);

        bool extract_spec_mode(const pugi::xml_node &spec);
        bool extract_spec_polarity(const pugi::xml_node &spec); // @todo make bool

        double extract_scan_rt(const pugi::xml_node &spec);

        std::vector<std::vector<double>> extract_spectrum(const pugi::xml_node &spectrum_node);
        std::vector<std::vector<std::vector<double>>> extract_spectra(const std::vector<int> &idxs);

    public:
        std::filesystem::path path;

        pugi::xml_document mzml_base_document;

        pugi::xml_parse_result loading_result;

        pugi::xml_node mzml_root_node;

        // std::vector<pugi::xml_node> spectra; // making this a class member makes the code unbelieveably slow

        int number_spectra;

        int number_chromatograms;

        int number_spectra_binary_arrays;

        // MS_SPECTRA_HEADERS first_spectra_headers;

        MZML(const std::filesystem::path &file);

        std::vector<std::vector<double>> get_spectrum(int index); // this is the actually important function

        std::vector<int> get_spectra_index(std::vector<int> indices = {});
        std::vector<int> get_spectra_array_length(std::vector<int> indices = {});
        std::vector<int> get_spectra_level(std::vector<int> indices = {});
        std::vector<bool> get_spectra_mode(std::vector<int> indices = {});
        std::vector<bool> get_spectra_polarity(std::vector<int> indices = {});
        std::vector<double> get_spectra_rt(std::vector<int> indices = {});
    }; // class MZML
}; // namespace StreamCraft

#endif // STREAMCRAFT_MZML_HPP