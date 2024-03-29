// test_qPeak_class.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_refmatrix.h"
#include "../include/qalgorithms_matrix.h"
#include "../include/qalgorithms_peak.h"

// external
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>

int main()
{
  // define input filename
  // std::string filename_input = "test/data/test_lcms_data.csv";
  std::string filename_input = "test/data/test_lcms_data2.csv";
  std::cout << "create LCMSData object....";
  q::LCMSData lcmsData;
  std::cout << "done\n";
  std::cout << "read csv....";
  // lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',', {q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME, q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY});
  lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',', {q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY, q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME});
  std::cout << "done\n";

  
  lcmsData.zeroFilling();
  lcmsData.cutData();
  lcmsData.filterSmallDataSets();
  lcmsData.interpolateData();

  lcmsData.info();

  // qPEAKS
  q::varDataType dataObject = &(lcmsData.data);
  q::qPeaks qpeaks(dataObject);
  // Capture the start time
  auto start = std::chrono::high_resolution_clock::now();

  qpeaks.findPeaks(dataObject);

  // Capture the end time
  auto end = std::chrono::high_resolution_clock::now();
  // Calculate the duration
  std::chrono::duration<double> duration = end - start;
  std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
  
  /*
  // export the data to a CSV file
  // define the filename
  std::string filename_output = "test/data/test_lcms_data_out.csv";
  // check if the file exists
  if (std::ifstream file(filename_output); file)
  {
    // remove the file
    std::remove(filename_output.c_str());
  }
  lcmsData.writeCSV(filename_output);

  // Call the Python script to plot the data
  // std::string pythonPath = "C:\\Python312\\python.exe";
  std::string pythonPath = "C:\\Users\\gerri\\AppData\\Local\\Programs\\Python\\Python312\\python.exe";
  std::string command = pythonPath + " test/test_qPeak_visualize_profiles.py" + " " + filename_input + " " + filename_output;
  std::system(command.c_str());
  */

  return 0;
}
