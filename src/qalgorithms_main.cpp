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

// console output
#define PRINT_NO_CENTROIDS               \
  SetConsoleTextAttribute(hConsole, 12); \
  std::cout << "no centroids found\n";   \
  SetConsoleTextAttribute(hConsole, 15);

// console output
#define PRINT_POLARITY(polarity)                   \
  SetConsoleTextAttribute(hConsole, 11);           \
  std::cout << "\npolarity: " << polarity << "\n"; \
  SetConsoleTextAttribute(hConsole, 15);

// console output
#define PRINT_FILE(filename, i, i_total)                                          \
  SetConsoleTextAttribute(hConsole, 6);                                           \
  std::cout << "\n[" << i << " / " << i_total << "]\nfile: " << filename << "\n"; \
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
  // check if the input file is a txt file
  std::vector<std::string> fileNames;
  if (filename_input.find(".txt") != std::string::npos)
  {
    const std::string endOfFile = "END_OF_FILE";
    std::ifstream file(filename_input);
    if (!file.is_open())
    {
      std::cerr << "Error: could not open file " << filename_input << std::endl;
      return 1;
    }
    std::string line;
    while (std::getline(file, line))
    {
      if (line.find(endOfFile) != std::string::npos)
      {
        break;
      }
      if (line.find(".mzML") != std::string::npos)
      {
        fileNames.push_back(line);
      }
    }
    file.close(); // close file
    if (fileNames.size() == 0)
    {
      std::cout << "Error: no mzML files found in the txt file" << std::endl;
      return 0;
    }
  }
  else
  {
    fileNames.push_back(filename_input);
  }
  int i = 1;
  int i_total = fileNames.size();
  for (auto fileName : fileNames)
  {
    // check if the input file a mzML file
    if (fileName.find(".mzML") != std::string::npos)
    {
      PRINT_FILE(fileName, i, i_total)
      std::vector<std::string> polarities = {"positive", "negative"};
      for (auto polarity : polarities)
      {
        PRINT_POLARITY(polarity)
        std::cout << "read file.........................";
        sc::MZML data(fileName); // create mzML object
        PRINT_DONE
        std::cout << "find centroids....................";
        q::Algorithms::qPeaks qpeaks;              // create qPeaks object
        q::MeasurementData::TensorData tensorData; // create tensorData object
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> centroids =
            tensorData.findCentroids_MZML(qpeaks, data, true, polarity, 10); // read mzML file and find centroids via qPeaks
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        if (centroids.size() == 0)
        {
          std::cout << "no centroids found" << std::endl;
          continue;
        }
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

        std::cout << "prepare data for qBinning.........";
        q::Algorithms::qBinning::CentroidedData testdata = qpeaks.passToBinning(centroids, centroids.size());
        PRINT_DONE
        std::cout << "perform qBinning..................";
        std::string summary_output_location = "summary_output_location";
        start = std::chrono::high_resolution_clock::now();
        std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(testdata, summary_output_location, 6, true, false);
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
        // use file input name to create output file name with polarity_features.csv extension
        std::string output_filename = fileName.substr(0, fileName.find_last_of(".")) + "_" + polarity + "_features.csv";
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
      } // for polarities
    } // if mzML
    i++;
  } // for fileNames
  return 0;
}
