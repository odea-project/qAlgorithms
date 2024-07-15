// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_ascii_logo.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>

int main()
{
  // print logo
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  q::logo::print_qpeaks();
  // initialize qPeaks static variables
  q::Algorithms::qPeaks::initialize();
  bool isCSV = false;

  // userinput for filename
  std::string filename_input;
  std::cout << "Enter the filename: ";
  std::cin >> filename_input;
  // check if file exists; if not, repeat the input until a valid file is entered
  while (!std::ifstream(filename_input))
  {
    std::cout << "Error: file not found" << std::endl;
    std::cout << "Enter the filename: ";
    std::cin >> filename_input;
  }

  std::cout << "create LCMSData object............";
  q::MeasurementData::LCMSData lcmsData;
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";
  SetConsoleTextAttribute(hConsole, 15);
  std::cout << "read file.........................";

  // check if the input file is a CSV file or a mzML file
  if (filename_input.find(".mzML") != std::string::npos)
  {
    sc::MZML data(filename_input);
    lcmsData.readStreamCraftMZML(data);
  }
  else if (filename_input.find(".csv") != std::string::npos)
  {
    lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',',
                     {q::DataType::DataField::MZ,
                      q::DataType::DataField::INTENSITY,
                      q::DataType::DataField::SCANNUMBER,
                      q::DataType::DataField::RETENTIONTIME});
    isCSV = true;
  }
  else
  {
    // return error not a valid file
    std::cout << "Error: not a valid file" << std::endl;
  }

  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";
  SetConsoleTextAttribute(hConsole, 15);
  lcmsData.info();

  std::cout << "zero filling......................";
  lcmsData.zeroFilling();
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";

  SetConsoleTextAttribute(hConsole, 15);
  std::cout << "cut data..........................";
  lcmsData.cutData();
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";

  SetConsoleTextAttribute(hConsole, 15);
  std::cout << "filter small data sets............";
  lcmsData.filterSmallDataSets();
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";

  SetConsoleTextAttribute(hConsole, 15);
  std::cout << "interpolate data..................";
  lcmsData.interpolateData();
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";

  SetConsoleTextAttribute(hConsole, 15);
  lcmsData.info();

  // qPEAKS
  std::cout << "create qPeaks object..............";
  q::MeasurementData::varDataType dataObject = &(lcmsData.data);
  q::Algorithms::qPeaks qpeaks;
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n";
  SetConsoleTextAttribute(hConsole, 15);
  std::cout << "find peaks........................";

  // Capture the start time
  auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks = qpeaks.findPeaks(dataObject);

  // Capture the end time
  auto end = std::chrono::high_resolution_clock::now();
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done";
  SetConsoleTextAttribute(hConsole, 15);
  std::cout << " in : ";
  // Calculate the duration
  std::chrono::duration<double> duration = end - start;
  std::cout << duration.count() << " seconds" << std::endl;
  std::cout << "Peaks found:                      ";
  size_t count = 0;
  for (size_t i = 0; i < peaks.size(); i++)
  {
    count += peaks[i].size();
  }
  std::cout << count << std::endl;

  // write peaks to file
  std::cout << "write peaks to file...............";
  std::string filename_output = filename_input;
  // remove the file extension
  filename_output = filename_output.substr(0, filename_output.find_last_of("."));
  filename_output += "_peaks.csv";
  qpeaks.peakListToCSV(peaks, filename_output);
  SetConsoleTextAttribute(hConsole, 10);
  std::cout << "done\n\n\n";
  SetConsoleTextAttribute(hConsole, 15);
  std::cout << peaks[0][0]->position << std::endl;
  // plot peaks
  if (isCSV)
  {
    // ask if the user wants to plot the peaks of the first dataset
    std::string plot;
    std::cout << "Do you want to plot the peaks? (y/n): ";
    std::cin >> plot;
    // check if the answer is valid
    while (plot != "y" && plot != "n")
    {
      std::cout << "Error: invalid input" << std::endl;
      std::cout << "Do you want to plot the peaks? (y/n): ";
      std::cin >> plot;
    }
    if (plot == "y")
    {
      std::cout << "plot peaks........................";
      qpeaks.plotPeaksToPython(
          filename_input,  // path to the original data
          filename_output, // path to the peak list
          true,            // plot fits as well; if set to false, only original peaks and vertical lines for peak positions are plotted
          false);          // plot the peaks as a scatter plot; if set to true, sampleID vs. position is plotted
      SetConsoleTextAttribute(hConsole, 10);
      std::cout << "done\n";
      SetConsoleTextAttribute(hConsole, 15);
    }
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cout << "Press any key to exit" << std::endl;
  std::cin.get();

  return 0;
}
