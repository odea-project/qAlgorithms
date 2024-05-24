#include "StreamCraft_mzxml.hpp"
#include <omp.h>
#include <filesystem>
#include <cstring>
#include <algorithm>

sc::mzxml::MZXML::MZXML(const std::string& file) {

  file_path = file;

  file_dir = file.substr(0, file.find_last_of("/\\") + 1);

  if (file_dir.back() == '/') file_dir.pop_back();

  file_name = file.substr(file.find_last_of("/\\") + 1);
  
  file_extension = file_name.substr(file_name.find_last_of(".") + 1);
  
  file_name = file_name.substr(0, file_name.find_last_of("."));

  const char* path = file.c_str();

  loading_result = doc.load_file(path, pugi::parse_default | pugi::parse_declaration | pugi::parse_pi);

  if (loading_result) {
    root = doc.document_element();

    if (!root) {
      std::cerr << "Root element is empty!" << std::endl;

    } else {
      format = root.name();

      if ("mzXML" == format) {
        format = "mzXML";

      } else {
        std::cerr << "Root element must be mzXML!" << std::endl;
      }
    }

  } else {
    std::cerr << "mzXML file could not be opened!" << std::endl << loading_result.description() << std::endl;
  }

  name = root.name();

  pugi::xml_node msrun = root.child("msRun");

  number_spectra = msrun.attribute("scanCount").as_int();

  pugi::xml_node first_node = msrun.child("scan");

  if (number_spectra > 0) {

    if (first_node) extract_binary_metadata(first_node);

    std::vector<int> sp0 = {0};
    first_spectra_headers = extract_spectra_headers(sp0);
  }
};

void sc::mzxml::MZXML::extract_binary_metadata(const pugi::xml_node& first_node) {

  binary_metadata.precision = first_node.child("peaks").attribute("precision").as_int();

  std::string compression = first_node.child("peaks").attribute("compressionType").as_string();

  binary_metadata.compression = compression;

  if (compression == "zlib" || compression == "zlib compression") {
    binary_metadata.compressed = true;

  } else {
    binary_metadata.compressed = false;
  }

  std::string byte_order = first_node.child("peaks").attribute("byteOrder").as_string();

  if (byte_order == "network") {
    binary_metadata.byte_order = "big_endian";

  } else {
    binary_metadata.byte_order = "little_endian";
  }
};

void sc::mzxml::MZXML::print() {
  std::cout << name << std::endl;
  std::cout << std::endl;
  std::cout << " File:                      " << file_path << std::endl;
  std::cout << std::endl;
  std::cout << " Number of spectra:         " << number_spectra << std::endl;
  // std::cout << " Spectra mode (first):      " << first_spectra_headers.spec_mode[0] << std::endl;
  std::cout << std::endl;
};

std::vector<pugi::xml_node> sc::mzxml::MZXML::link_vector_spectra_nodes() {

  std::vector<pugi::xml_node> spectra;

  pugi::xml_node msrun = root.child("msRun");

  if (msrun) {
    for (pugi::xml_node child = msrun.child("scan"); child; child = child.next_sibling()) {
      spectra.push_back(child);
    }

  } else {
    std::cerr << "msRun not found in the mzXML file!" << std::endl;
  }
  
  return spectra;
};

int sc::mzxml::MZXML::extract_spec_index(const pugi::xml_node& spec) {
  return spec.attribute("num").as_int();
};

std::string sc::mzxml::MZXML::extract_spec_id(const pugi::xml_node& spec) {
  return spec.attribute("num").as_string();
};

int sc::mzxml::MZXML::extract_spec_scan(const pugi::xml_node& spec) {
  return spec.attribute("num").as_int();
};

int sc::mzxml::MZXML::extract_spec_array_length(const pugi::xml_node& spec) {
  return spec.attribute("peaksCount").as_int();
};

int sc::mzxml::MZXML::extract_spec_level(const pugi::xml_node& spec) {
  return spec.attribute("msLevel").as_int();
};

std::string sc::mzxml::MZXML::extract_spec_mode(const pugi::xml_node& spec) {
  int centroided = spec.attribute("centroided").as_int();
  if (centroided == 1) {
    return "centroid";
  } else if (centroided == 0) {
     return "profile";
  } else {
    return "";
  }
};

std::string sc::mzxml::MZXML::extract_spec_polarity(const pugi::xml_node& spec) {
  std::string pol_sign = spec.attribute("polarity").as_string();
  if (pol_sign == "+") {
     return "positive";
  } else if (pol_sign == "-") {
    return "negative";
  } else {
    return "";
  }
};

double sc::mzxml::MZXML::extract_spec_lowmz(const pugi::xml_node& spec) {
  return spec.attribute("lowMz").as_double();
};

double sc::mzxml::MZXML::extract_spec_highmz(const pugi::xml_node& spec) {
  return spec.attribute("highMz").as_double();
};

double sc::mzxml::MZXML::extract_spec_bpmz(const pugi::xml_node& spec) {
  return spec.attribute("basePeakMz").as_double();
};

double sc::mzxml::MZXML::extract_spec_bpint(const pugi::xml_node& spec) {
  return spec.attribute("basePeakIntensity").as_double();
};

double sc::mzxml::MZXML::extract_spec_tic(const pugi::xml_node& spec) {
  return spec.attribute("totIonCurrent").as_double();
};

double sc::mzxml::MZXML::extract_scan_rt(const pugi::xml_node& spec) {
  std::string rt = spec.attribute("retentionTime").as_string();
  double rt_n;
  std::sscanf(rt.c_str(), "%*[^0123456789]%lf", &rt_n);
  char last_char = '\0';
  std::sscanf(rt.c_str() + rt.size() - 1, "%c", &last_char);
  if (last_char != 'S') rt_n = rt_n * 60;
  return rt_n;
};

double sc::mzxml::MZXML::extract_ion_mz(const pugi::xml_node& spec) {
  pugi::xml_node precursor = spec.child("precursorMz");
  return precursor.text().as_double();
};

double sc::mzxml::MZXML::extract_activation_ce(const pugi::xml_node& spec) {
  return spec.attribute("collisionEnergy").as_double();
};

sc::MS_SPECTRA_HEADERS sc::mzxml::MZXML::extract_spectra_headers(const std::vector<int>& idxs) {

  MS_SPECTRA_HEADERS headers;

  std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

  int n = idxs.size();

  if (n == 0) {
    std::cerr << "No indices given!" << std::endl;
    return headers;
  }

  if (spectra_nodes.size() == 0) {
    std::cerr << "No spectra found!" << std::endl;
    return headers;
  }

  headers.resize_all(n);

  // #pragma omp parallel for
  for (int i = 0; i < n; i++) {

    const int& index = idxs[i];

    const pugi::xml_node& spec = spectra_nodes[index];

    headers.index[i] = extract_spec_index(spec);
    headers.id[i] = extract_spec_id(spec);
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
    headers.rt[i] = extract_scan_rt(spec);

    pugi::xml_node precursor = spec.child("precursorMz");

    if (precursor) {
      headers.precursor_mz[i] = extract_ion_mz(spec);
      headers.activation_ce[i] = extract_activation_ce(spec);
    }
  } // end of i loop

  return headers;
}

std::vector<std::vector<double>> sc::mzxml::MZXML::extract_spectrum(const pugi::xml_node& spectrum_node) {

  std::vector<std::vector<double>> spectrum(2);

  int number_traces = spectrum_node.attribute("peaksCount").as_int();

  if (number_traces == 0) return  spectrum;

  for (int i = 0; i < 2; i++) spectrum[i].resize(number_traces);

  const char* encoded_string = spectrum_node.child("peaks").child_value();

  std::string decoded_string = utils::decode_base64(encoded_string);

  if (binary_metadata.compressed) {
    decoded_string = utils::decompress_zlib(decoded_string);
  }

  std::vector<double> res(number_traces * 2);

  if (binary_metadata.byte_order == "big_endian") {
    res = utils::decode_big_endian(decoded_string, binary_metadata.precision / 8);

  } else if (binary_metadata.byte_order == "little_endian") {
    res = utils::decode_little_endian(decoded_string, binary_metadata.precision / 8);

  } else {
    std::cerr << "Byte order must be big_endian or little_endian!" << std::endl;
    return(spectrum);
  }

  for (int i = 0; i < number_traces; i++) {
    spectrum[0][i] = res[i * 2];
    spectrum[1][i] = res[i * 2 + 1];
  }

  return spectrum;
};

std::vector<std::vector<std::vector<double>>> sc::mzxml::MZXML::extract_spectra(const std::vector<int>& idxs) {

  std::vector<std::vector<std::vector<double>>> sp;

  std::vector<pugi::xml_node> spectra_nodes = link_vector_spectra_nodes();

  int n = idxs.size();

  if (n == 0) {
    std::cerr << "No indices given!" << std::endl;
    return sp;
  }

  if (spectra_nodes.size() == 0) {
    std::cerr << "No spectra found!" << std::endl;
    return sp;
  }

  sp.resize(n);

  // #pragma omp parallel for
  for (int i = 0; i < n; i++) {

    const int& index = idxs[i];

    const pugi::xml_node& spectrum_node = spectra_nodes[index];

    sp[i] = extract_spectrum(spectrum_node);
  }

  return sp;
}

sc::MS_SPECTRA_HEADERS sc::mzxml::MZXML::get_spectra_headers(std::vector<int> indices) {

  MS_SPECTRA_HEADERS hd;

  if (number_spectra == 0) {
    std::cerr << "There are no spectra in the mzXML file!" << std::endl;
    return hd;
  } 

  if (indices.size() == 0) {
    indices.resize(number_spectra);
    std::iota(indices.begin(), indices.end(), 0);
  }

  hd = extract_spectra_headers(indices);
  
  return hd;
};

std::vector<std::vector<std::vector<double>>> sc::mzxml::MZXML::get_spectra(std::vector<int> indices) {

  std::vector<std::vector<std::vector<double>>> sp;

  if (number_spectra == 0) {
    std::cerr << "There are no spectra in the mzXML file!" << std::endl;
    return sp;
  }

  if (indices.size() == 0) {
    indices.resize(number_spectra);
    std::iota(indices.begin(), indices.end(), 0);
  }

  sp = extract_spectra(indices);
  
  return sp;
};
