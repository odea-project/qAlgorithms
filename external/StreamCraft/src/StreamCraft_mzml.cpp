#include "StreamCraft_utils.hpp"

#include "StreamCraft_mzml.hpp"
#include <iostream>
#include <filesystem>

StreamCraft::MZML::MZML(const std::filesystem::path &file)
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

    number_spectra = spec_list.attribute("count").as_int();

    if (number_spectra > 0)
    {
        auto first_node = spec_list.first_child();
        pugi::xml_node binary_list = first_node.child("binaryDataArrayList");

        size_t counter = 0;

        for (const pugi::xml_node &bin : binary_list.children("binaryDataArray"))
        {
            MZML_BINARY_METADATA mtd = extract_binary_metadata(bin);

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

double StreamCraft::MZML::extract_scan_RT(const pugi::xml_node &spec)
{
    pugi::xml_node node_scan = spec.child("scanList").child("scan");
    pugi::xml_node rt_node = node_scan.find_child_by_attribute("cvParam", "name", "scan start time");
    const std::string rt_unit = rt_node.attribute("unitName").as_string();
    double rt_val = rt_node.attribute("value").as_double();
    if (rt_unit == "second")
    {
        return rt_val;
    }
    if (rt_unit == "minute")
    {
        return rt_val * 60;
    }
    assert(false);
};

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

std::vector<bool> StreamCraft::MZML::get_spectra_polarity(const std::vector<unsigned int> *indices)
{
    assert(indices->size() > 0);
    std::vector<bool> polarities;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        int idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000130"))
        {
            polarities.push_back(true);
        }
        else
        {
            assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000129")); // @todo a single check should be enough
            polarities.push_back(false);
        }
    }

    return polarities;
};

std::vector<pugi::xml_node> StreamCraft::MZML::link_vector_spectra_nodes() // @todo this does not need to be called more than once
{
    std::vector<pugi::xml_node> spectra;

    std::string search_run = "//run";

    pugi::xpath_node xps_run = mzml_root_node.select_node(search_run.c_str());

    pugi::xml_node spec_list = xps_run.node().child("spectrumList");

    assert(spec_list);

    for (pugi::xml_node child = spec_list.first_child(); child; child = child.next_sibling())
    {
        spectra.push_back(child);
    }

    assert(spectra.size() > 0);
    return spectra;
};

std::vector<std::vector<double>> StreamCraft::MZML::extract_spectrum(const pugi::xml_node &spectrum_node)
{

    pugi::xml_node node_binary_list = spectrum_node.child("binaryDataArrayList");
    unsigned int number_bins = node_binary_list.attribute("count").as_int();
    assert(number_spectra_binary_arrays == number_bins);

    std::vector<std::vector<double>> spectrum;
    unsigned int number_traces = spectrum_node.attribute("defaultArrayLength").as_int();

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
            decoded_string = StreamCraft::decompress_zlib(decoded_string); // @todo is there a case where this doesn't apply for all spectra?
        }

        spectrum[counter] = StreamCraft::decode_little_endian(decoded_string, spectra_binary_metadata[counter].precision_int / 8);

        assert(spectrum[counter].size() == number_traces); // this happens if an index is tried which does not exist in the data

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

std::vector<bool> StreamCraft::MZML::get_spectra_mode(const std::vector<unsigned int> *indices) // centroid or profile
{
    assert(indices->size() > 0);
    std::vector<bool> modes;

    std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

    for (size_t i = 0; i < indices->size(); ++i)
    {
        size_t idx = (*indices)[i];
        pugi::xml_node spec = spectra_nodes[idx];
        if (spec.find_child_by_attribute("cvParam", "accession", "MS:1000128"))
        {
            modes.push_back(true);
        }
        else
        {
            assert(spec.find_child_by_attribute("cvParam", "accession", "MS:1000127")); // @todo is there any case where the mode is neither profile nor centroid?
            modes.push_back(false);
        }
    }
    return modes;
};