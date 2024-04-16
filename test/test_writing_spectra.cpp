#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#define STREAMCRAT_HEADER_ONLY


int main() {

  std::string file = "./example_profile.mzML";

  sc::MZML z(file);

  std::vector<int> idx(10);
  std::iota(idx.begin(), idx.end(), 0);

  std::vector<std::vector<std::vector<double>>> spectra;
  // spectra list
      // spectrum
        // m/z, intensity, ...
        
  spectra = z.get_spectra();
  
  std::cout << "Number of extracted spectra: " << spectra.size() << std::endl;

  // Add two extra binnary arays to each spectrum
  int number_traces = 0;
  for (size_t i = 0; i < spectra.size(); i++) {
    for (size_t j = 0; j < spectra[i].size(); j++) {
      spectra[i][j].resize(10);
    }
    number_traces = number_traces + spectra[i][0].size();
    spectra[i].push_back(spectra[i][0]);
    spectra[i].push_back(spectra[i][0]);
  }

  std::cout << "Number of traces: " << number_traces << std::endl;
  std::cout << "Number of traces in 1st spectrum: " << spectra[0][0].size() << std::endl;
  std::cout << "Number of binary entries: " << spectra[0].size() << std::endl;

  std::vector<std::string> bin_names = {"mz", "intensity", "x1", "x2"};

  z.write_spectra(spectra, bin_names, sc::MS_SPECTRA_MODE::CENTROID, true, true, "_cent");

  std::cout << std::endl;

  return 0;
}
