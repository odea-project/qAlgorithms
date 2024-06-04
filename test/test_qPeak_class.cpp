// test_qPeak_class.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_qpeaks.h"
// #include "../include/qalgorithms_refmatrix.h"
// #include "../include/qalgorithms_matrix.h"
// #include "../include/qalgorithms_datatype_peak.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>
int main()
{
  // define input filename
  // std::string filename_input = "test/data/test_lcms_data.csv";
  std::string filename_input = "test/data/test_lcms_data2.csv";
  // std::string filename_input = "test/data/test_ftir.csv";
  // std::string filename_input = "test/data/test_chrom.csv";
  // std::string filename_input = "test/data/test_orbitrap.csv";
  // std::string filename_input = "test/data/example_profile.mzML";

  std::cout << "create LCMSData object....";
  q::LCMSData lcmsData;
  std::cout << "done\n";
  std::cout << "read file....";

  lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',', {q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY, q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME});
  // sc::MZML data(filename_input);
  // lcmsData.readStreamCraftMZML(data);
  std::cout << "done\n";
  lcmsData.info();

  std::cout << "zero filling....";
  lcmsData.zeroFilling();
  std::cout << "done\n";
  std::cout << "cut data....";
  lcmsData.cutData();
  std::cout << "done\n";
  std::cout << "filter small data sets....";
  lcmsData.filterSmallDataSets();
  std::cout << "done\n";
  std::cout << "interpolate data....";
  lcmsData.interpolateData();
  std::cout << "done\n";

  lcmsData.info();

  // qPEAKS
  std::cout << "create qPeaks object....";
  q::varDataType dataObject = &(lcmsData.data);
  q::qPeaks qpeaks(dataObject);
  std::cout << "done\n";
  std::cout << "find peaks....";

  // Capture the start time
  auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks = qpeaks.findPeaks(dataObject);

  // Capture the end time
  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "done in : ";
  // Calculate the duration
  std::chrono::duration<double> duration = end - start;
  std::cout << duration.count() << " seconds" << std::endl;
  std::cout << "Peaks found: ";
  size_t count = 0;
  for (size_t i = 0; i < peaks.size(); i++)
  {
    count += peaks[i].size();
  }
  std::cout << count << std::endl;
  // for (int i = 0; i < peaks.size(); i++)
  // {
  //   for (int j = 0; j < peaks[i].size(); j++)
  //   {
  //     std::cout << "SampleID: " << peaks[i][j]->sampleID << " Position: " << peaks[i][j]->position << " Height: " << peaks[i][j]->height << " DQS: " << peaks[i][j]->dqsPeak << " x0: " << peaks[i][j]->x0 << std::endl;
  //   }
  // }

  // write peaks to file
  std::cout << "write peaks to file....";
  std::string filename_output = "test/data/test_peaks_out.csv";
  qpeaks.peakListToCSV(peaks, filename_output);
  std::cout << "done\n";

  // plot peaks
  std::cout << "plot peaks....";
  qpeaks.plotPeaksToPython(
      filename_input,  // path to the original data
      filename_output, // path to the peak list
      true,            // plot fits as well; if set to false, only original peaks and vertical lines for peak positions are plotted
      false);          // plot the peaks as a scatter plot; if set to true, sampleID vs. position is plotted
  std::cout << "done\n";

  return 0;
}
