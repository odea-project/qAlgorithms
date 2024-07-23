// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_measurement_data_tensor.h"
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_ascii_logo.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>

// console output
#define PRINT_DONE                       \
  SetConsoleTextAttribute(hConsole, 10); \
  std::cout << "done\n";                 \
  SetConsoleTextAttribute(hConsole, 15);

// console output
#define PRINT_DONE_no_n                  \
  SetConsoleTextAttribute(hConsole, 10); \
  std::cout << "done";                   \
  SetConsoleTextAttribute(hConsole, 15);

int main()
{
  // print logo
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  q::logo::print_qpeaks();
  // initialize qPeaks static variables and lcmsData object
  q::Algorithms::qPeaks::initialize();

  // userinput for filename
  std::string filename_input;
  std::cout << "Enter the filename (*.mzML): ";
  std::cin >> filename_input;
  // check if file exists; if not, repeat the input until a valid file is entered
  while (!std::ifstream(filename_input))
  {
    std::cout << "Error: file not found" << std::endl;
    std::cout << "Enter the filename (*.mzML): ";
    std::cin >> filename_input;
  }

  std::cout << "read file.........................";

  // check if the input file a mzML file
  if (filename_input.find(".mzML") != std::string::npos)
  {
    sc::MZML data(filename_input); // create mzML object
    PRINT_DONE
    std::cout << "find centroids....................";
    q::Algorithms::qPeaks qpeaks;              // create qPeaks object
    q::MeasurementData::TensorData tensorData; // create tensorData object
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> centroids =
        tensorData.findCentroids_MZML(qpeaks, data, true, 10); // read mzML file and find centroids via qPeaks
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    PRINT_DONE_no_n
            std::cout
        << " in " << duration.count() << " s";
    size_t num_centroids = 0;
    for (size_t i = 0; i < centroids.size(); ++i)
    {
      for (size_t j = 0; j < centroids[i].size(); ++j)
      {
        num_centroids++;
      }
    }
    std::cout << " (" << num_centroids << " centroids)" << std::endl;
    PRINT_DONE

    std::cout << "prepare data for qBinning.........";
    q::Algorithms::qBinning::CentroidedData testdata = qpeaks.passToBinning(centroids, centroids.size());
    PRINT_DONE
    std::cout << "perform qBinning..................";
    std::string summary_output_location = "summary_output_location";
    start = std::chrono::high_resolution_clock::now();
    std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(testdata, summary_output_location, 6, true);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    PRINT_DONE_no_n
            std::cout
        << " in " << duration.count() << " s";
    std::cout << " (" << binnedData.size() << " EICs)" << std::endl;

    std::cout << "find chromatographic peaks........";
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks =
        tensorData.findPeaks_QBIN(qpeaks, binnedData);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    PRINT_DONE_no_n
            std::cout
        << " in " << duration.count() << " s";
    size_t num_peaks = 0;
    for (size_t i = 0; i < peaks.size(); ++i)
    {
      for (size_t j = 0; j < peaks[i].size(); ++j)
      {
        num_peaks++;
      }
    }
    std::cout << " (" << num_peaks << " peaks)" << std::endl;

    // write peaks to csv file
    std::cout << "write peaks to file...............";
    std::string output_filename = "output.csv";
    std::ofstream output_file(output_filename);
    output_file << "mz,rt,int,mzUncertainty,rtUncertainty,intUncertainty,dqs_cen,dqs_bin,dqs_peak\n";
    for (size_t i = 0; i < peaks.size(); ++i)
    {
      for (size_t j = 0; j < peaks[i].size(); ++j)
      {
        output_file << peaks[i][j]->mz << "," << peaks[i][j]->retentionTime << "," << peaks[i][j]->area << ","
                    << peaks[i][j]->mzUncertainty << "," << peaks[i][j]->retentionTimeUncertainty << "," << peaks[i][j]->areaUncertainty << ","
                    << peaks[i][j]->dqsCen << "," << peaks[i][j]->dqsBin << "," << peaks[i][j]->dqsPeak << "\n";
      }
    }
    output_file.close();
    PRINT_DONE
  }

  return 0;
}
