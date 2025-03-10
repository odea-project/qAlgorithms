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

        // void print()
        // {
        //     std::cout << std::endl;
        //     std::cout << "Index:                     " << index << std::endl;
        //     std::cout << "Precision original string: " << precision_name << std::endl;
        //     std::cout << "Precision accession:       " << precision_accession << std::endl;
        //     std::cout << "Precision:                 " << precision_int << std::endl;
        //     std::cout << "Precision type:            " << precision_type << std::endl;
        //     std::cout << "Compression:               " << compression << std::endl;
        //     std::cout << "Data name:                 " << data_name << std::endl;
        //     std::cout << "Data accession:            " << data_accession << std::endl;
        //     std::cout << "Data value:                " << data_value << std::endl;
        //     std::cout << "Data unit:                 " << data_unit << std::endl;
        //     std::cout << "Data short name:           " << data_name_short << std::endl;
        //     std::cout << std::endl;
        // };
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
        std::vector<pugi::xml_node> link_vector_chrom_nodes();

        void extract_spectra_binary_metadata(const pugi::xml_node &first_node);
        MZML_BINARY_METADATA extract_binary_metadata(const pugi::xml_node &bin);

        MS_SPECTRA_HEADERS extract_spectra_headers(const std::vector<int> &idxs);
        MS_CHROMATOGRAMS_HEADERS extract_chrom_headers(const std::vector<int> &idxs);

        std::string extract_spec_id(const pugi::xml_node &spec); // never used, @todo check others
        std::string extract_spec_mode(const pugi::xml_node &spec);
        std::string extract_spec_polarity(const pugi::xml_node &spec);
        std::string extract_spec_title(const pugi::xml_node &spec);

        std::string extract_scan_filter_string(const pugi::xml_node &spec);

        std::string extract_activation_type(const pugi::xml_node &spec);

        int extract_spec_scan(const pugi::xml_node &spec);
        int extract_spec_array_length(const pugi::xml_node &spec);
        int extract_spec_level(const pugi::xml_node &spec);
        int extract_spec_index(const pugi::xml_node &spec);

        int extract_scan_config(const pugi::xml_node &spec);

        int extract_ion_charge(const pugi::xml_node &spec);

        int extract_precursor_scan(const pugi::xml_node &spec);

        double extract_spec_lowmz(const pugi::xml_node &spec);
        double extract_spec_highmz(const pugi::xml_node &spec);
        double extract_spec_bpmz(const pugi::xml_node &spec);
        double extract_spec_bpint(const pugi::xml_node &spec);
        double extract_spec_tic(const pugi::xml_node &spec);

        double extract_scan_injection_ion_time(const pugi::xml_node &spec);
        double extract_scan_rt(const pugi::xml_node &spec);
        double extract_scan_drift(const pugi::xml_node &spec);

        double extract_window_mz(const pugi::xml_node &spec);
        double extract_window_mzlow(const pugi::xml_node &spec);
        double extract_window_mzhigh(const pugi::xml_node &spec);

        double extract_ion_mz(const pugi::xml_node &spec);
        double extract_ion_intensity(const pugi::xml_node &spec);

        double extract_activation_ce(const pugi::xml_node &spec);

        std::vector<std::vector<double>> extract_spectrum(const pugi::xml_node &spectrum_node);
        std::vector<std::vector<std::vector<double>>> extract_spectra(const std::vector<int> &idxs);

        std::vector<std::vector<double>> extract_chromatogram(const pugi::xml_node &chrom_node);
        std::vector<std::vector<std::vector<double>>> extract_chromatograms(const std::vector<int> &idxs);

    public:
        std::string file_path;

        std::string file_dir;

        std::string file_name;

        std::string file_extension;

        pugi::xml_document doc;

        pugi::xml_parse_result loading_result;

        pugi::xml_node root;

        std::string format;

        std::string name;

        int number_spectra;

        int number_chromatograms;

        int number_spectra_binary_arrays;

        MS_SPECTRA_HEADERS first_spectra_headers;

        MZML(const std::string &file);

        std::vector<std::string> get_spectra_binary_short_names();

        MS_SPECTRA_HEADERS get_spectra_headers(std::vector<int> indices = {});
        MS_CHROMATOGRAMS_HEADERS get_chromatograms_headers(std::vector<int> indices = {});
        std::vector<std::vector<std::vector<double>>> get_spectra(std::vector<int> indices = {});
        std::vector<std::vector<double>> get_spectrum(int index);
        std::vector<std::vector<std::vector<double>>> get_chromatograms(std::vector<int> indices = {});

        std::vector<int> get_spectra_index(std::vector<int> indices = {});
        std::vector<int> get_spectra_scan_number(std::vector<int> indices = {});
        std::vector<int> get_spectra_array_length(std::vector<int> indices = {});
        std::vector<int> get_spectra_level(std::vector<int> indices = {});
        std::vector<std::string> get_spectra_mode(std::vector<int> indices = {});
        std::vector<std::string> get_spectra_polarity(std::vector<int> indices = {});
        std::vector<double> get_spectra_lowmz(std::vector<int> indices = {});
        std::vector<double> get_spectra_highmz(std::vector<int> indices = {});
        std::vector<double> get_spectra_bpmz(std::vector<int> indices = {});
        std::vector<double> get_spectra_bpint(std::vector<int> indices = {});
        std::vector<double> get_spectra_tic(std::vector<int> indices = {});
        std::vector<double> get_spectra_rt(std::vector<int> indices = {});
        std::vector<double> get_spectra_drift(std::vector<int> indices = {});
        std::vector<int> get_spectra_precursor_scan(std::vector<int> indices = {});
        std::vector<double> get_spectra_precursor_mz(std::vector<int> indices = {});
        std::vector<double> get_spectra_precursor_window_mz(std::vector<int> indices = {});
        std::vector<double> get_spectra_precursor_window_mzlow(std::vector<int> indices = {});
        std::vector<double> get_spectra_precursor_window_mzhigh(std::vector<int> indices = {});
        std::vector<double> get_spectra_collision_energy(std::vector<int> indices = {});

        void print();
        // void print_spectra_binary_metadata();

        void write_spectra(const std::vector<std::vector<std::vector<double>>> &spectra,
                           const std::vector<std::string> &names, MS_SPECTRA_MODE mode, bool compress, bool save, std::string save_suffix);

    }; // class MZML

    void test_extract_spectra_mzml(const std::string &file);
    void test_extract_chromatograms_mzml(const std::string &file);
}; // namespace StreamCraft

#endif // STREAMCRAFT_MZML_HPP

// #if defined(STREAMCRAFT_HEADER_ONLY) && !defined(STREAMCRAFT_MZML_SOURCE)
// #	define STREAMCRAFT_MZML_SOURCE "StreamCraft_mzml.cpp"
// #	include STREAMCRAFT_MZML_SOURCE
// #endif