#include "StreamCraft_mzml.hpp"
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <filesystem>

StreamCraft::MZML::MZML(const std::filesystem::path &file) // @todo move to std::filesystem
{
    pugi::xml_document doc; // @todo why does this need to persist beyond the constructor? check access to class member variable

    loading_result = mzml_base_document.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);
    auto loading2 = doc.load_file(file.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

    assert(loading2.status == loading_result.status);

    if (loading_result)
    {
        mzml_root_node = mzml_base_document.document_element();
        assert(mzml_root_node);
    }
    else
    {
        std::cerr << "mzML file could not be opened!" << std::endl
                  << loading_result.description() << std::endl;
        return;
    }

    std::string format = mzml_root_node.name();
    assert(format == "indexedmzML");

    mzml_root_node = mzml_root_node.first_child();

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    std::vector<int> sp0 = {0};

    number_spectra = spec_list.attribute("count").as_int();

    if (number_spectra > 0)
    {
        auto first_node = spec_list.first_child();
        pugi::xml_node binary_list = first_node.child("binaryDataArrayList");

        size_t counter = 0;

        for (const pugi::xml_node &bin : binary_list.children("binaryDataArray"))
        {
            MZML_BINARY_METADATA mtd = extract_binary_metadata(bin);

            if (mtd.data_name_short == "") // @todo does this make any sense? If we do not know what a field is in advance, we cannot process it properly
            {
                mtd.data_name_short = "val_" + std::to_string(counter);
            }

            mtd.index = counter;

            spectra_binary_metadata.push_back(mtd);

            counter++;
        }

        number_spectra_binary_arrays = counter;
    }
};

StreamCraft::MZML_BINARY_METADATA StreamCraft::MZML::extract_binary_metadata(const pugi::xml_node &bin)
{
    MZML_BINARY_METADATA mtd;

    pugi::xml_node node_integer_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000519");
    pugi::xml_node node_float_32 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000521");
    pugi::xml_node node_integer_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000522");
    pugi::xml_node node_float_64 = bin.find_child_by_attribute("cvParam", "accession", "MS:1000523");

    if (node_float_64)
    {
        mtd.precision_int = 64;
    }
    else if (node_float_32)
    {
        mtd.precision_int = 32;
    }
    else if (node_integer_64)
    {
        mtd.precision_int = 64;
    }
    else if (node_integer_32)
    {
        mtd.precision_int = 32;
    }
    else
    {
        assert(false);
        // Encoding precision with accession MS:1000519, MS:1000521, MS:1000522 or MS:1000523 not found
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
            mtd.data_value = node_data_type.attribute("value").as_string();
            mtd.data_name_short = possible_short_name_binary_data[i];
            break;
        }
    }

    assert(has_bin_data_type);
    // throw("Encoded data type could not be found matching the mzML official vocabulary!");

    if (mtd.data_name_short == "other") // this should be separated into a check for null / termination condition @todo
    {
        mtd.data_name_short = mtd.data_value;
    }

    return mtd;
}

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

std::vector<double> StreamCraft::MZML::get_spectra_RT(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);

    std::vector<double> retention_times;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        double RT = extract_scan_RT(spec);
        retention_times.push_back(RT);
    }

    return retention_times;
};

bool StreamCraft::MZML::extract_spec_polarity(const pugi::xml_node &spec)
{
    pugi::xml_node pol_pos_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000130");
    pugi::xml_node pol_neg_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000129");
    if (pol_pos_node)
    {
        return true;
    }
    else if (pol_neg_node)
    {
        return false;
    }
    else
    {
        assert(false);
    }
};

std::vector<bool> StreamCraft::MZML::get_spectra_polarity(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<bool> polarities;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        int idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        bool polarity = extract_spec_polarity(spec);
        polarities.push_back(polarity);
    }

    return polarities;
};

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

double StreamCraft::MZML::extract_scan_RT(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node rt_node = node_scan.find_child_by_attribute("cvParam", "name", "scan start time");
    std::string rt_unit = rt_node.attribute("unitName").as_string();
    double rt_val = rt_node.attribute("value").as_double();
    if (rt_unit == "minute")
        rt_val = rt_val * 60;
    return rt_val;
};

std::vector<std::vector<double>> StreamCraft::MZML::extract_spectrum(const pugi::xml_node &spectrum_node)
{

    pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");
    unsigned int number_bins = node_binary_list.attribute("count").as_int();
    assert(number_spectra_binary_arrays == number_bins);

    std::vector<std::vector<double>> spectrum;
    int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

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
            else
            {
                assert(unit == "second");
            }
        }
        counter++;
    }
    return spectrum;
};

std::vector<std::vector<std::vector<double>>> StreamCraft::MZML::extract_spectra(const std::vector<int> &idxs)
{
    std::vector<std::vector<std::vector<double>>> all_spectra;
    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    int n = idxs.size();

    if (n == 0)
    {
        std::cerr << "No indices given!" << std::endl;
        return all_spectra;
    }

    if (spectra_nodes.size() == 0)
    {
        std::cerr << "No spectra found!" << std::endl;
        return all_spectra;
    }

    all_spectra.resize(n);

    // # pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        const int index = idxs[i];

        const pugi::xml_node &spectrum_node = spectra_nodes[index];

        all_spectra[i] = extract_spectrum(spectrum_node);
    }

    return all_spectra;
}

std::vector<size_t> StreamCraft::MZML::get_spectra_index(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<size_t> spec_indices;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        int idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        int index = spec.attribute("index").as_int();
        spec_indices.push_back(index);
    }

    return spec_indices;
};

std::vector<int> StreamCraft::MZML::get_spectra_level(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<int> levels;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        pugi::xml_node level_node = spec.find_child_by_attribute("cvParam", "name", "ms level");
        int level = level_node.attribute("value").as_int();
        levels.push_back(level);
    }

    return levels;
};

bool StreamCraft::MZML::extract_spec_mode(const pugi::xml_node &spec)
{
    pugi::xml_node centroid_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000127");
    pugi::xml_node profile_node = spec.find_child_by_attribute("cvParam", "accession", "MS:1000128");
    // @todo what if both applies?
    if (centroid_node)
    {
        return false;
    }
    else if (profile_node)
    {
        return true;
    }
    else
    {
        assert(false);
    }
};

std::vector<bool> StreamCraft::MZML::get_spectra_mode(const std::vector<unsigned int> *indices) // centroid or profile
{
    assert(indices->size() > 0);
    std::vector<bool> modes;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        bool mode = extract_spec_mode(spec);
        modes.push_back(mode);
    }
    return modes;
};