#include "StreamCraft_mzml.hpp"
#include <omp.h>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <filesystem>

StreamCraft::MZML::MZML(const std::filesystem::path &file) // @todo move to std::filesystem
{

    // file_path = file;

    // file_dir = file.substr(0, file.find_last_of("/\\") + 1);

    // if (file_dir.back() == '/')
    //     file_dir.pop_back();

    // file_name = file.substr(file.find_last_of("/\\") + 1);

    // file_extension = file_name.substr(file_name.find_last_of(".") + 1);

    // file_name = file_name.substr(0, file_name.find_last_of("."));

    // const char *path = file.;
    pugi::xml_document doc; // @todo why does this need to persist beyond the constructor? check access to class member variable

    loading_result = mzml_base_document.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);
    auto loading2 = doc.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

    assert(loading2.status == loading_result.status);

    if (loading_result)
    {
        mzml_root_node = mzml_base_document.document_element();

        if (!mzml_root_node)
        {
            std::cerr << "Root element is empty!" << std::endl; // @todo change this function to something that can return null if it fails
        }
        else
        {
            format = mzml_root_node.name();

            if ("indexedmzML" == format)
            {
                format = "mzML";
            }
            else
            {
                std::cerr << "Root element must be indexedmzML!" << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "mzML file could not be opened!" << std::endl
                  << loading_result.description() << std::endl;
    }

    if (format == "mzML")
    {
        mzml_root_node = mzml_root_node.first_child();
    }

    name = mzml_root_node.name();

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    std::vector<int> sp0 = {0};

    first_spectra_headers = extract_spectra_headers(sp0);

    number_spectra = spec_list.attribute("count").as_int();

    if (number_spectra > 0)
    {
        extract_spectra_binary_metadata(spec_list.first_child());
    }

    pugi::xml_node chrom_list = xps_run.node().child("chromatogramList");

    number_chromatograms = chrom_list.attribute("count").as_int();
};

std::vector<std::vector<double>> StreamCraft::MZML::get_spectrum(int index)
{
    std::vector<std::vector<double>> spectrum;
    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return spectrum;
    }

    const pugi::xml_node &spectrum_node = spectra_nodes[index];
    spectrum = extract_spectrum(spectrum_node);

    return spectrum;
}

// void StreamCraft::MZML::print()
// {
//     std::cout << name << std::endl;
//     std::cout << std::endl;
//     std::cout << " File:                      " << file_path << std::endl;
//     std::cout << std::endl;
//     std::cout << " Number of spectra:         " << number_spectra << std::endl;
//     std::cout << " Spectra mode (first):      " << first_spectra_headers.mode[0] << std::endl;
//     std::cout << " Number of binnary arrays:  " << number_spectra_binary_arrays << std::endl;
//     std::cout << " Name of binary arrays:     ";
//     if (number_spectra_binary_arrays > 0)
//     {
//         for (int i = 0; i < number_spectra_binary_arrays; ++i)
//         {
//             std::cout << spectra_binary_metadata[i].data_name_short;
//             if (i < (number_spectra_binary_arrays - 1))
//             {
//                 std::cout << ", ";
//             }
//         }
//     }
//     std::cout << std::endl;
//     std::cout << std::endl;
//     std::cout << " Number of chromatograms:   " << number_chromatograms << std::endl;
//     std::cout << std::endl;
// };

// void StreamCraft::MZML::print_spectra_binary_metadata()
// {
//     if (number_spectra_binary_arrays > 0)
//     {
//         for (int i = 0; i < number_spectra_binary_arrays; ++i)
//         {
//             spectra_binary_metadata[i].print();
//         }
//     }
// };

std::vector<pugi::xml_node> StreamCraft::MZML::link_vector_spectra_nodes() // @todo this does not need to be called more than once
{

    std::vector<pugi::xml_node> spectra;

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    if (spec_list)
    {
        for (pugi::xml_node child = spec_list.first_child(); child; child = child.next_sibling())
        {
            spectra.push_back(child);
        }
    }
    else
    {
        std::cerr << "Spectra list not found in the mzML file!" << std::endl;
    }

    assert(spectra.size() > 0);
    return spectra;
};

int StreamCraft::MZML::extract_spec_index(const pugi::xml_node &spec)
{
    return spec.attribute("index").as_int();
};

// std::string StreamCraft::MZML::extract_spec_id(const pugi::xml_node &spec)
// {
//     return spec.attribute("id").as_string();
// };

int StreamCraft::MZML::extract_spec_scan(const pugi::xml_node &spec)
{
    std::string spec_id = spec.attribute("id").as_string();
    std::size_t poslastEqual = spec_id.rfind('=');
    return std::stoi(spec_id.substr(poslastEqual + 1));
};

int StreamCraft::MZML::extract_spec_array_length(const pugi::xml_node &spec)
{
    return spec.attribute("defaultArrayLength").as_int();
};

int StreamCraft::MZML::extract_spec_level(const pugi::xml_node &spec)
{
    pugi::xml_node level_node = spec.find_child_by_attribute("cvParam", "name", "ms level");
    return level_node.attribute("value").as_int();
};

std::string StreamCraft::MZML::extract_spec_mode(const pugi::xml_node &spec)
{
    pugi::xml_node centroid_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000127");
    pugi::xml_node profile_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000128");
    // @todo what if both applies?
    if (centroid_node)
    {
        return "centroid";
    }
    else if (profile_node)
    {
        return "profile";
    }
    else
    {
        return "";
    }
};

std::string StreamCraft::MZML::extract_spec_polarity(const pugi::xml_node &spec)
{
    pugi::xml_node pol_pos_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000130");
    pugi::xml_node pol_neg_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000129");
    if (pol_pos_node)
    {
        return "positive";
    }
    else if (pol_neg_node)
    {
        return "negative";
    }
    else
    {
        assert(false);
    }
};

double StreamCraft::MZML::extract_spec_lowmz(const pugi::xml_node &spec)
{
    pugi::xml_node lowmz_node = spec.find_child_by_attribute("cvParam", "name", "lowest observed m/z");
    return lowmz_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_spec_highmz(const pugi::xml_node &spec)
{
    pugi::xml_node highmz_node = spec.find_child_by_attribute("cvParam", "name", "highest observed m/z");
    return highmz_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_spec_bpmz(const pugi::xml_node &spec)
{
    pugi::xml_node bpmz_node = spec.find_child_by_attribute("cvParam", "name", "base peak m/z");
    return bpmz_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_spec_bpint(const pugi::xml_node &spec)
{
    pugi::xml_node bpint_node = spec.find_child_by_attribute("cvParam", "name", "base peak intensity");
    return bpint_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_spec_tic(const pugi::xml_node &spec)
{
    pugi::xml_node tic_node = spec.find_child_by_attribute("cvParam", "name", "total ion current");
    return tic_node.attribute("value").as_double();
};

std::string StreamCraft::MZML::extract_spec_title(const pugi::xml_node &spec)
{
    pugi::xml_node title_node = spec.find_child_by_attribute("cvParam", "name", "spectrum title");
    return title_node.attribute("value").as_string();
};

double StreamCraft::MZML::extract_scan_rt(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node rt_node = node_scan.find_child_by_attribute("cvParam", "name", "scan start time");
    std::string rt_unit = rt_node.attribute("unitName").as_string();
    double rt_val = rt_node.attribute("value").as_double();
    if (rt_unit == "minute")
        rt_val = rt_val * 60;
    return rt_val;
};

double StreamCraft::MZML::extract_scan_drift(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node drift_node = node_scan.find_child_by_attribute("cvParam", "name", "ion mobility drift time");
    return drift_node.attribute("value").as_double();
};

std::string StreamCraft::MZML::extract_scan_filter_string(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node filter_string_node = node_scan.find_child_by_attribute("cvParam", "name", "filter string");
    return filter_string_node.attribute("value").as_string();
};

int StreamCraft::MZML::extract_scan_config(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node config_node = node_scan.find_child_by_attribute("cvParam", "name", "preset scan configuration");
    return config_node.attribute("value").as_int();
};

double StreamCraft::MZML::extract_scan_injection_ion_time(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node iit_node = node_scan.find_child_by_attribute("cvParam", "name", "ion injection time");
    return iit_node.attribute("value").as_double();
};

int StreamCraft::MZML::extract_precursor_scan(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    std::string pre_scan_str = precursor.attribute("spectrumRef").as_string();
    if (pre_scan_str != "")
    {
        std::size_t poslastEqual = pre_scan_str.rfind('=');
        return std::stoi(pre_scan_str.substr(poslastEqual + 1));
    }
    else
    {
        return 0;
    }
};

double StreamCraft::MZML::extract_window_mz(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node isolation = precursor.child("isolationWindow");
    pugi::xml_node window_mz_node = isolation.find_child_by_attribute("cvParam", "name", "isolation window target m/z");
    return window_mz_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_window_mzlow(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node isolation = precursor.child("isolationWindow");
    pugi::xml_node window_mzlow_node = isolation.find_child_by_attribute("cvParam", "name", "isolation window lower offset");
    return window_mzlow_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_window_mzhigh(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node isolation = precursor.child("isolationWindow");
    pugi::xml_node window_mzhigh_node = isolation.find_child_by_attribute("cvParam", "name", "isolation window upper offset");
    return window_mzhigh_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_ion_mz(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node selected_ion = precursor.child("selectedIonList").first_child();
    pugi::xml_node ion_mz_node = selected_ion.find_child_by_attribute("cvParam", "name", "selected ion m/z");
    return ion_mz_node.attribute("value").as_double();
};

double StreamCraft::MZML::extract_ion_intensity(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node selected_ion = precursor.child("selectedIonList").first_child();
    pugi::xml_node ion_intensity_node = selected_ion.find_child_by_attribute("cvParam", "name", "peak intensity");
    return ion_intensity_node.attribute("value").as_double();
};

int StreamCraft::MZML::extract_ion_charge(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node selected_ion = precursor.child("selectedIonList").first_child();
    pugi::xml_node ion_charge_node = selected_ion.find_child_by_attribute("cvParam", "name", "charge state");
    return ion_charge_node.attribute("value").as_int();
};

std::string StreamCraft::MZML::extract_activation_type(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node activation = precursor.child("activation");
    if (activation)
    {
        pugi::xml_node activation_type_node = activation.first_child();
        return activation_type_node.name();
    }
    else
    {
        return "";
    }
};

double StreamCraft::MZML::extract_activation_ce(const pugi::xml_node &spec)
{
    pugi::xml_node precursor = spec.child("precursorList").child("precursor");
    pugi::xml_node activation = precursor.child("activation");
    if (activation)
    {
        pugi::xml_node activation_ce_node = activation.find_child_by_attribute("cvParam", "name", "collision energy");
        return activation_ce_node.attribute("value").as_double();
    }
    else
    {
        return 0;
    }
};

StreamCraft::MS_SPECTRA_HEADERS StreamCraft::MZML::extract_spectra_headers(const std::vector<int> &idxs)
{

    StreamCraft::MS_SPECTRA_HEADERS headers;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    // spectra_nodes = this->spectra;

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "No indices given!" << std::endl;
        return headers;
    }

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return headers;
    }

    headers.resize_all(n);

    // # pragma omp parallel for
    for (int i = 0; i < n; i++)
    {

        const int index = idxs[i];

        const pugi::xml_node &spec = spectra_nodes[index];

        headers.index[i] = extract_spec_index(spec);
        headers.scan[i] = extract_spec_scan(spec);
        headers.array_length[i] = extract_spec_array_length(spec);
        headers.level[i] = extract_spec_level(spec);
        headers.mode[i] = extract_spec_mode(spec);
        headers.polarity[i] = extract_spec_polarity(spec);
        headers.lowmz[i] = extract_spec_lowmz(spec);
        headers.highmz[i] = extract_spec_highmz(spec);
        headers.bpmz[i] = extract_spec_bpmz(spec);
        headers.bpint[i] = extract_spec_bpint(spec);
        headers.tic[i] = extract_spec_tic(spec);
        headers.title[i] = extract_spec_title(spec);
        headers.rt[i] = extract_scan_rt(spec);
        headers.drift[i] = extract_scan_drift(spec);
        headers.filter_string[i] = extract_scan_filter_string(spec);
        headers.config[i] = extract_scan_config(spec);
        headers.injection_ion_time[i] = extract_scan_injection_ion_time(spec);

        pugi::xml_node precursor = spec.child("precursorList").child("precursor");

        if (precursor)
        {
            headers.precursor_scan[i] = extract_precursor_scan(spec);
            headers.window_mz[i] = extract_window_mz(spec);
            headers.window_mzlow[i] = extract_window_mzlow(spec);
            headers.window_mzhigh[i] = extract_window_mzhigh(spec);

            pugi::xml_node selected_ion = precursor.child("selectedIonList").first_child();

            if (selected_ion)
            {
                headers.precursor_mz[i] = extract_ion_mz(spec);
                headers.precursor_intensity[i] = extract_ion_intensity(spec);
                headers.precursor_charge[i] = extract_ion_charge(spec);
            }

            pugi::xml_node activation = precursor.child("activation");

            if (activation)
            {
                headers.activation_type[i] = extract_activation_type(spec);
                headers.activation_ce[i] = extract_activation_ce(spec);
            }
        }
    } // end of i loop

    return headers;
}

StreamCraft::MZML_BINARY_METADATA StreamCraft::MZML::extract_binary_metadata(const pugi::xml_node &bin)
{

    MZML_BINARY_METADATA mtd;

    pugi::xml_node node_integer_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000519");

    pugi::xml_node node_float_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000521");

    pugi::xml_node node_integer_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000522");

    pugi::xml_node node_float_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000523");

    if (node_float_64)
    {
        // mtd.precision_name = node_float_64.attribute("name").as_string();
        // mtd.precision_accession = node_float_64.attribute("accession").as_string();
        // mtd.precision_type = "float";
        mtd.precision_int = 64;
    }
    else if (node_float_32)
    {
        // mtd.precision_name = node_float_32.attribute("name").as_string();
        // mtd.precision_accession = node_float_32.attribute("accession").as_string();
        // mtd.precision_type = "float";
        mtd.precision_int = 32;
    }
    else if (node_integer_64)
    {
        // mtd.precision_name = node_integer_64.attribute("name").as_string();
        // mtd.precision_accession = node_integer_64.attribute("accession").as_string();
        // mtd.precision_type = "integer";
        mtd.precision_int = 64;
    }
    else if (node_integer_32)
    {
        // mtd.precision_name = node_integer_32.attribute("name").as_string();
        // mtd.precision_accession = node_integer_32.attribute("accession").as_string();
        // mtd.precision_type = "integer";
        mtd.precision_int = 32;
    }
    else
    {
        assert(false);
        // @todo fix exceptions
        // throw("Encoding precision with accession MS:1000521, MS:1000522 or MS:1000523 not found!");
    }

    pugi::xml_node node_comp = bin.find_child_by_attribute("cvParam", "accession", "MS:1000574");

    if (node_comp)
    {
        mtd.compression = node_comp.attribute("name").as_string();

        if (mtd.compression == "zlib" || mtd.compression == "zlib compression")
        {
            mtd.compressed = true;
        }
        else
        {
            mtd.compressed = false;
        }
    }

    bool has_bin_data_type = false;

    for (size_t i = 0; 1 < possible_accessions_binary_data.size(); ++i)
    {
        pugi::xml_node node_data_type = bin.find_child_by_attribute("cvParam", "accession", possible_accessions_binary_data[i].c_str());

        if (node_data_type)
        {

            has_bin_data_type = true;

            // mtd.data_name = node_data_type.attribute("name").as_string();

            // mtd.data_accession = node_data_type.attribute("accession").as_string();

            mtd.data_value = node_data_type.attribute("value").as_string();

            // mtd.data_unit = node_data_type.attribute("unitName").as_string();

            mtd.data_name_short = possible_short_name_binary_data[i];

            break;
        }
    }

    assert(has_bin_data_type);
    // throw("Encoded data type could not be found matching the mzML official vocabolary!");

    if (mtd.data_name_short == "other") // this should be separated into a check for null / termination condition @todo
    {
        mtd.data_name_short = mtd.data_value;
    }

    return mtd;
}

void StreamCraft::MZML::extract_spectra_binary_metadata(const pugi::xml_node &first_node)
{

    pugi::xml_node binary_list = first_node.child("binaryDataArrayList");

    int counter = 0;

    for (const pugi::xml_node &bin : binary_list.children("binaryDataArray"))
    {

        MZML_BINARY_METADATA mtd = extract_binary_metadata(bin);

        if (mtd.data_name_short == "")
        {
            mtd.data_name_short = "val_" + std::to_string(counter);
        }

        mtd.index = counter;

        spectra_binary_metadata.push_back(mtd);

        counter++;
    }

    number_spectra_binary_arrays = counter;
}

std::vector<std::vector<double>> StreamCraft::MZML::extract_spectrum(const pugi::xml_node &spectrum_node)
{

    std::vector<std::vector<double>> spectrum;

    int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

    pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");

    int number_bins = node_binary_list.attribute("count").as_int();

    assert(number_spectra_binary_arrays == number_bins);

    // @todo fix exceptions
    // throw("Binary array length does not match binary array length of first spectrum!");

    spectrum.resize(number_bins);

    int counter = 0;

    for (auto i = node_binary_list.children("binaryDataArray").begin(); i != node_binary_list.children("binaryDataArray").end(); ++i)
    {

        const pugi::xml_node &bin = *i;

        pugi::xml_node node_binary = bin.child("binary");

        std::string encoded_string = node_binary.child_value();

        std::string decoded_string = StreamCraft::decode_base64(encoded_string);

        if (spectra_binary_metadata[counter].compressed)
        {
            decoded_string = StreamCraft::decompress_zlib(decoded_string);
        }

        spectrum[counter] = StreamCraft::decode_little_endian(decoded_string, spectra_binary_metadata[counter].precision_int / 8);

        int bin_array_size = spectrum[counter].size();

        if (bin_array_size != number_traces)
        {
            // this happens if an index is tried which does not exist in the data
            std::cerr << "Number of traces in binary array does not match the value of the spectrum header!\n";
            return std::vector<std::vector<double>>{0};
        }

        if (spectra_binary_metadata[counter].data_name_short == "time")
        {
            pugi::xml_node node_unit = bin.find_child_by_attribute("cvParam", "unitCvRef", "UO");
            std::string unit = node_unit.attribute("unitName").as_string();

            if (unit == "minute")
            {
                for (double &j : spectrum[counter])
                {
                    j *= 60;
                }
            }
        }

        counter++;
    }

    return spectrum;
};

std::vector<std::vector<std::vector<double>>> StreamCraft::MZML::extract_spectra(const std::vector<int> &idxs)
{

    std::vector<std::vector<std::vector<double>>> sp;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "No indices given!" << std::endl;
        return sp;
    }

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return sp;
    }

    sp.resize(n);

    // # pragma omp parallel for
    for (int i = 0; i < n; i++)
    {

        const int index = idxs[i];

        const pugi::xml_node &spectrum_node = spectra_nodes[index];

        sp[i] = extract_spectrum(spectrum_node);
    }

    return sp;
}

std::vector<pugi::xml_node> StreamCraft::MZML::link_vector_chrom_nodes()
{

    std::vector<pugi::xml_node> chrom_nodes;

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node chrom_list = xps_run.node().child("chromatogramList");

    if (chrom_list)
    {
        for (pugi::xml_node child = chrom_list.first_child(); child; child = child.next_sibling())
        {
            chrom_nodes.push_back(child);
        }
    }
    else
    {
        std::cerr << "Chromatogram list not found in the mzML file!" << std::endl;
    }

    return chrom_nodes;
};

StreamCraft::MS_CHROMATOGRAMS_HEADERS StreamCraft::MZML::extract_chrom_headers(const std::vector<int> &idxs)
{

    StreamCraft::MS_CHROMATOGRAMS_HEADERS headers;

    std::vector<pugi::xml_node> chrom_nodes = link_vector_chrom_nodes();

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "No indices given!" << std::endl;
        return headers;
    }

    if (chrom_nodes.size() == 0)
    {
        std::cerr << "No chromatograms found!" << std::endl;
        return headers;
    }

    headers.resize_all(n);

    for (int i = 0; i < n; i++)
    {

        const int index = idxs[i];

        const pugi::xml_node &chrom = chrom_nodes[index];

        headers.id[i] = chrom.attribute("index").as_int();

        std::string id = chrom.attribute("id").as_string();

        headers.id[i] = id;

        headers.array_length[i] = chrom.attribute("defaultArrayLength").as_int();

        pugi::xml_node pol_pos_node = chrom.find_child_by_attribute("cvParam", "accession", "MS:1000130");
        pugi::xml_node pol_neg_node = chrom.find_child_by_attribute("cvParam", "accession", "MS:1000129");

        if (pol_pos_node)
        {
            headers.polarity[i] = "positive";
        }
        else if (pol_neg_node)
        {
            headers.polarity[i] = "negative";
        }
        else
        {
            headers.polarity[i] = "";
        }

        pugi::xml_node precursor = chrom.child("precursor");

        if (precursor != NULL)
        {

            pugi::xml_node isolation = precursor.child("isolationWindow");

            pugi::xml_node pre_mz_node = isolation.find_child_by_attribute("cvParam", "name", "isolation window target m/z");

            headers.precursor_mz[i] = pre_mz_node.attribute("value").as_double();

            pugi::xml_node activation = precursor.child("activation");

            if (activation)
            {

                pugi::xml_node activation_type_node = activation.first_child();
                headers.activation_type[i] = activation_type_node.name();

                pugi::xml_node activation_ce_node = activation.find_child_by_attribute("cvParam", "name", "collision energy");
                headers.activation_ce[i] = activation_ce_node.attribute("value").as_double();
            }
        }

        pugi::xml_node product = chrom.child("product");

        if (product)
        {

            pugi::xml_node isolation = product.child("isolationWindow");

            pugi::xml_node pro_mz_node = isolation.find_child_by_attribute("cvParam", "name", "isolation window target m/z");
            headers.product_mz[i] = pro_mz_node.attribute("value").as_double();
        }
    } // end of i loop

    return headers;
}

std::vector<std::vector<double>> StreamCraft::MZML::extract_chromatogram(const pugi::xml_node &chrom_node)
{

    std::vector<std::vector<double>> chrom;

    int number_traces = chrom_node.attribute("defaultArrayLength").as_int();

    pugi::xml_node node_binary_list = chrom_node.child("binaryDataArrayList");

    int number_bins = node_binary_list.attribute("count").as_int();

    chrom.resize(number_bins);

    int counter = 0;

    for (auto i = node_binary_list.children("binaryDataArray").begin(); i != node_binary_list.children("binaryDataArray").end(); ++i)
    {

        const pugi::xml_node &bin = *i;

        MZML_BINARY_METADATA mtd = extract_binary_metadata(bin);

        pugi::xml_node node_binary = bin.child("binary");

        std::string encoded_string = node_binary.child_value();

        std::string decoded_string = StreamCraft::decode_base64(encoded_string);

        if (mtd.compressed)
        {
            decoded_string = StreamCraft::decompress_zlib(decoded_string);
        }

        chrom[counter] = StreamCraft::decode_little_endian(decoded_string, mtd.precision_int / 8);

        int bin_array_size = chrom[counter].size();

        if (bin_array_size != number_traces)
        {
            std::cerr << "Number of traces in binary array does not match the value of the chromatogram header!\n";
            return std::vector<std::vector<double>>{0};
        }

        if (mtd.data_name_short == "time")
        {
            pugi::xml_node node_unit = bin.find_child_by_attribute("cvParam", "unitCvRef", "UO");
            std::string unit = node_unit.attribute("unitName").as_string();

            if (unit == "minute")
            {
                for (double &j : chrom[counter])
                {
                    j *= 60;
                }
            }
        }

        counter++;
    }
    assert(counter > 0);

    for (int i = 1; i < counter; i++)
    {
        assert(chrom[0].size() == chrom[i].size());
    }

    return chrom;
}

std::vector<std::vector<std::vector<double>>> StreamCraft::MZML::extract_chromatograms(const std::vector<int> &idxs)
{

    std::vector<std::vector<std::vector<double>>> chr;

    std::vector<pugi::xml_node> chrom_nodes = link_vector_chrom_nodes();

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "Warning: No indices given!" << std::endl;
        return chr;
    }

    if (chrom_nodes.size() == 0)
    {
        std::cerr << "No chromatograms found!" << std::endl;
        return chr;
    }

    chr.resize(n);

    // # pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        const int index = idxs[i];

        const pugi::xml_node &chrom_node = chrom_nodes[index];

        chr[i] = extract_chromatogram(chrom_node);
    }

    return chr;
}

void StreamCraft::MZML::write_spectra(
    const std::vector<std::vector<std::vector<double>>> &spectra,
    const std::vector<std::string> &names, MS_SPECTRA_MODE mode, bool compress, bool save, std::string save_suffix)
{

    if (spectra.size() == 0)
    {
        std::cerr << "No spectra to write!" << std::endl;
        return;
    }

    if (spectra[0].size() != names.size())
    {
        std::cerr << "Number of spectra and names do not match!" << std::endl;
        return;
    }

    std::string search_run = "//run";

    pugi::xml_node run_node = mzml_root_node.select_node(search_run.c_str()).node();

    pugi::xml_node spec_list_node = run_node.child("spectrumList");

    std::vector<pugi::xml_node> spectra_nodes;

    if (spec_list_node)
    {

        for (pugi::xml_node child = spec_list_node.first_child(); child; child = child.next_sibling())
        {
            spectra_nodes.push_back(child);
        }

        if (spectra_nodes.size() != spectra.size())
        {
            std::cerr << "Number of spectra in the file and the number of spectra to write do not match!" << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "Spectrum list not found in the mzML file!" << std::endl;
        return;
    }

    int number_spectra = spectra.size();

    std::string number_spectra_str = std::to_string(number_spectra);

    spec_list_node.attribute("count").set_value(number_spectra_str.c_str());

    for (size_t i = 0; i < spectra.size(); i++)
    {

        pugi::xml_node spec = spectra_nodes[i];

        const std::vector<double> &mz = spectra[i][0];

        const std::vector<double> &intensity = spectra[i][1];

        spec.attribute("defaultArrayLength").set_value(spectra[i][0].size());

        if (mode == MS_SPECTRA_MODE::CENTROID)
        {
            pugi::xml_node node_mode = spec.find_child_by_attribute("cvParam", "accession", "MS:1000128");

            if (node_mode)
            {
                node_mode.attribute("accession").set_value("MS:1000127");
                node_mode.attribute("name").set_value("centroid spectrum");
            }
        }

        pugi::xml_node low_mz_node = spec.find_child_by_attribute("cvParam", "name", "lowest observed m/z");

        double low_mz = *std::min_element(mz.begin(), mz.end());

        low_mz_node.attribute("value").set_value(low_mz);

        pugi::xml_node high_mz_node = spec.find_child_by_attribute("cvParam", "name", "highest observed m/z");

        double high_mz = *std::max_element(mz.begin(), mz.end());

        high_mz_node.attribute("value").set_value(high_mz);

        pugi::xml_node bp_mz_node = spec.find_child_by_attribute("cvParam", "name", "base peak m/z");

        double bp_mz = mz[std::distance(intensity.begin(), std::max_element(intensity.begin(), intensity.end()))];

        bp_mz_node.attribute("value").set_value(bp_mz);

        pugi::xml_node bp_int_node = spec.find_child_by_attribute("cvParam", "name", "base peak intensity");

        double bp_int = *std::max_element(intensity.begin(), intensity.end());

        bp_int_node.attribute("value").set_value(bp_int);

        pugi::xml_node tic_node = spec.find_child_by_attribute("cvParam", "name", "total ion current");

        double tic = std::accumulate(intensity.begin(), intensity.end(), 0.0);

        tic_node.attribute("value").set_value(tic);

        pugi::xml_node bin_array_list = spec.child("binaryDataArrayList");

        bin_array_list.remove_children();

        bin_array_list.attribute("count").set_value(spectra[i].size());

        for (size_t j = 0; j < spectra[i].size(); j++)
        {

            const std::vector<double> &x = spectra[i][j];

            std::string x_enc = StreamCraft::encode_little_endian(x, 8);

            if (compress)
                x_enc = StreamCraft::compress_zlib(x_enc);

            x_enc = StreamCraft::encode_base64(x_enc);

            pugi::xml_node bin_array = bin_array_list.append_child("binaryDataArray");

            bin_array.append_attribute("encodedLength") = x_enc.size();

            pugi::xml_node bin = bin_array.append_child("cvParam");

            bin.append_attribute("cvRef") = "MS";

            bin.append_attribute("accession") = "MS:1000523";

            bin.append_attribute("name") = "64-bit float";

            bin.append_attribute("value") = "";

            bin = bin_array.append_child("cvParam");

            if (compress)
            {
                bin.append_attribute("cvRef") = "MS";
                bin.append_attribute("accession") = "MS:1000574";
                bin.append_attribute("name") = "zlib compression";
                bin.append_attribute("value") = "";
            }
            else
            {
                bin.append_attribute("cvRef") = "MS";
                bin.append_attribute("accession") = "MS:1000576";
                bin.append_attribute("name") = "no compression";
                bin.append_attribute("value") = "";
            }

            bin = bin_array.append_child("cvParam");

            bin.append_attribute("cvRef") = "MS";

            if (j == 0)
            {
                bin.append_attribute("accession") = "MS:1000514";
                bin.append_attribute("name") = "m/z array";
                bin.append_attribute("value") = "";
                bin.append_attribute("unitCvRef") = "MS";
                bin.append_attribute("unitAccession") = "MS:1000040";
                bin.append_attribute("unitName") = "m/z";
            }
            else if (j == 1)
            {
                bin.append_attribute("accession") = "MS:1000515";
                bin.append_attribute("name") = "intensity array";
                bin.append_attribute("value") = "";
                bin.append_attribute("unitCvRef") = "MS";
                bin.append_attribute("unitAccession") = "MS:1000131";
                bin.append_attribute("unitName") = "number of detector counts";
            }
            else
            {
                bin.append_attribute("accession") = "MS:1000786";
                bin.append_attribute("name") = "non-standard data array";
                bin.append_attribute("value") = names[j].c_str();
            }

            pugi::xml_node bin_data = bin_array.append_child("binary");

            bin_data.append_child(pugi::node_pcdata).set_value(x_enc.c_str());
        }
    }

    // if (save)
    // {

    //     if (save_suffix == "")
    //         save_suffix = "_modified";

    //     std::string new_file_path = file_dir + "/" + file_name + save_suffix + "." + file_extension;

    //     if (new_file_path == file_path)
    //     {
    //         std::cerr << "The new file path is the same as the original file path!" << std::endl;
    //         return;
    //     }

    //     if (std::filesystem::exists(new_file_path))
    //         std::filesystem::remove(new_file_path);

    //     if (!doc.save_file(new_file_path.c_str()))
    //         std::cerr << "Error saving the file!" << std::endl;
    // }
};

std::vector<int> StreamCraft::MZML::get_spectra_index(std::vector<int> indices)
{

    std::vector<int> idxs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return idxs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int index = extract_spec_index(spec);
        idxs.push_back(index);
    }

    return idxs;
};

std::vector<int> StreamCraft::MZML::get_spectra_scan_number(std::vector<int> indices)
{

    std::vector<int> scans;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return scans;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int scan = extract_spec_scan(spec);
        scans.push_back(scan);
    }

    return scans;
};

std::vector<int> StreamCraft::MZML::get_spectra_array_length(std::vector<int> indices)
{

    std::vector<int> lengths;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return lengths;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int length = extract_spec_array_length(spec);
        lengths.push_back(length);
    }

    return lengths;
};

std::vector<int> StreamCraft::MZML::get_spectra_level(std::vector<int> indices)
{

    std::vector<int> levels;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return levels;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int level = extract_spec_level(spec);
        levels.push_back(level);
    }

    return levels;
};

std::vector<std::string> StreamCraft::MZML::get_spectra_mode(std::vector<int> indices) // centroid or profile
{

    std::vector<std::string> modes;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return modes;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        std::string mode = extract_spec_mode(spec);
        modes.push_back(mode);
    }

    return modes;
};

std::vector<std::string> StreamCraft::MZML::get_spectra_polarity(std::vector<int> indices)
{

    std::vector<std::string> polarities;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return polarities;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        std::string polarity = extract_spec_polarity(spec);
        polarities.push_back(polarity);
    }

    return polarities;
};

std::vector<double> StreamCraft::MZML::get_spectra_lowmz(std::vector<int> indices)
{

    std::vector<double> lowmzs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return lowmzs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double lowmz = extract_spec_lowmz(spec);
        lowmzs.push_back(lowmz);
    }

    return lowmzs;
};

std::vector<double> StreamCraft::MZML::get_spectra_highmz(std::vector<int> indices)
{

    std::vector<double> highmzs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return highmzs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double highmz = extract_spec_highmz(spec);
        highmzs.push_back(highmz);
    }

    return highmzs;
};

std::vector<double> StreamCraft::MZML::get_spectra_bpmz(std::vector<int> indices)
{

    std::vector<double> bpmzs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return bpmzs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double bpmz = extract_spec_bpmz(spec);
        bpmzs.push_back(bpmz);
    }

    return bpmzs;
};

std::vector<double> StreamCraft::MZML::get_spectra_bpint(std::vector<int> indices)
{

    std::vector<double> bpints;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return bpints;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double bpint = extract_spec_bpint(spec);
        bpints.push_back(bpint);
    }

    return bpints;
};

std::vector<double> StreamCraft::MZML::get_spectra_tic(std::vector<int> indices)
{

    std::vector<double> tics;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return tics;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double tic = extract_spec_tic(spec);
        tics.push_back(tic);
    }

    return tics;
};

std::vector<double> StreamCraft::MZML::get_spectra_rt(std::vector<int> indices)
{

    std::vector<double> rts;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return rts;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double rt = extract_scan_rt(spec);
        rts.push_back(rt);
    }

    return rts;
};

std::vector<double> StreamCraft::MZML::get_spectra_drift(std::vector<int> indices)
{

    std::vector<double> drifts;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return drifts;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double drift = extract_scan_drift(spec);
        drifts.push_back(drift);
    }

    return drifts;
};

std::vector<int> StreamCraft::MZML::get_spectra_precursor_scan(std::vector<int> indices)
{

    std::vector<int> prec_scans;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return prec_scans;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int prec_scan = extract_precursor_scan(spec);
        prec_scans.push_back(prec_scan);
    }

    return prec_scans;
};

std::vector<double> StreamCraft::MZML::get_spectra_precursor_mz(std::vector<int> indices)
{

    std::vector<double> prec_mzs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return prec_mzs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double prec_mz = extract_ion_mz(spec);
        prec_mzs.push_back(prec_mz);
    }

    return prec_mzs;
};

std::vector<double> StreamCraft::MZML::get_spectra_precursor_window_mz(std::vector<int> indices)
{

    std::vector<double> prec_w_mzs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return prec_w_mzs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double prec_w_mz = extract_window_mz(spec);
        prec_w_mzs.push_back(prec_w_mz);
    }

    return prec_w_mzs;
};

std::vector<double> StreamCraft::MZML::get_spectra_precursor_window_mzlow(std::vector<int> indices)
{

    std::vector<double> prec_w_mzlows;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return prec_w_mzlows;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double prec_w_mzlow = extract_window_mzlow(spec);
        prec_w_mzlows.push_back(prec_w_mzlow);
    }

    return prec_w_mzlows;
};

std::vector<double> StreamCraft::MZML::get_spectra_precursor_window_mzhigh(std::vector<int> indices)
{

    std::vector<double> prec_w_mzhighs;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return prec_w_mzhighs;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double prec_w_mzhigh = extract_window_mzhigh(spec);
        prec_w_mzhighs.push_back(prec_w_mzhigh);
    }

    return prec_w_mzhighs;
};

std::vector<double> StreamCraft::MZML::get_spectra_collision_energy(std::vector<int> indices)
{

    std::vector<double> ces;

    if (number_spectra == 0)
    {
        std::cerr << "There are no spectra in the mzML file!" << std::endl;
        return ces;
    }

    if (indices.size() == 0)
    {
        indices.resize(number_spectra);
        std::iota(indices.begin(), indices.end(), 0);
    }

    int n = indices.size();

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double ce = extract_activation_ce(spec);
        ces.push_back(ce);
    }

    return ces;
};