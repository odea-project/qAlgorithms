// rcpp_ms_centroid_spectra_qCentroid.cpp

#include "../include/qalgorithms.h"
#include "../include/qalgorithms_utils.h"
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
  // Requesting string input with a default value for filename
  std::cout << "Enter a file name (default is '../test/test_orbitrap.csv'): ";
  std::string userInputStr;
  std::getline(std::cin, userInputStr); // Using getline to read a full line (including spaces)

  if (userInputStr.empty())
  {
    userInputStr = "test/test_orbitrap.csv"; // Assigning default value
  }

  // Requesting integer input for defining the scale limit for the peak model
  std::cout << "Enter a the Maximum Scale as Integer (default is 5): ";
  std::string lineInput;
  std::getline(std::cin, lineInput);
  std::istringstream iss(lineInput);
  int userInputInt;
  if (!(iss >> userInputInt))
  {
    userInputInt = 5; // Assigning default value if conversion fails
  }

  // Requesting integer input for defining the running Mode
  std::cout << "Choose the Running Mode: " << std::endl;
  std::cout << "0: Debugging" << std::endl;
  std::cout << "1: Silent Mode (default)" << std::endl;
  std::cout << "2: Progressbar Mode   : ";
  std::string lineInputMode;
  std::getline(std::cin, lineInputMode);
  std::istringstream issMode(lineInputMode);
  int userInputIntMode;
  if (!(issMode >> userInputIntMode))
  {
    userInputIntMode = 1; // Assigning default value if conversion fails
  }

  // LOAD SOME TEST DATA
  std::ifstream file(userInputStr);
  // std::ifstream file("test/test_tof.csv");
  std::string line;
  std::vector<std::vector<double>> matrix;

  while (std::getline(file, line))
  {
    std::stringstream ss(line);
    std::string cell;
    std::vector<double> row;

    while (std::getline(ss, cell, ','))
    {
      row.push_back(std::stod(cell));
    }

    matrix.push_back(row);
  }
  q::Matrix xyData(matrix.size(), 3);
  for (int i = 0; i < matrix.size(); i++)
  {
    xyData(i, 0) = matrix[i][0];
    xyData(i, 1) = matrix[i][1];
    xyData(i, 2) = matrix[i][2];
  }

  // Initialize tValues for statistical Tests
  q::tValues tVal;

  /* Initialize Peakmodel with scales from 2 to 5. A scale is defined as a window of datapoints from -n to n, i.e., a scale of 2 means [-2, -1, 0, 1, 2]. */
  q::Peakmodel model(userInputInt);
  switch (userInputIntMode)
  {
  case 0:
    model.setMode(q::Mode::DEBUGGING);
    break;

  case 2:
    model.setMode(q::Mode::PROGRESS);
    break;

  default:
    break;
  }

  // add tValues to the Peak model
  model.loadTValues(tVal);

  // add measurement data to the Peak model
  model.addMultipleMeasurements(xyData);

  // Capture the start time
  auto start = std::chrono::high_resolution_clock::now();
  model.findPeaks();

  // Capture the end time
  auto finish = std::chrono::high_resolution_clock::now();

  // Calculate and print the elapsed time
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";

  /* print some Peak (the 1st Peak will be printed) */
  // model[0].print();

  /* create a vector of a certain peak property */
  // std::vector<double> test = model.getPeakProperties(q::Peakproperties::PEAKID);

  /* get raw pre-processed raw data (gapfilled, zero-interpolated) */
  // q::Matrix DATA = model.getData(1,0);

  /* Export pre-processed raw data */
  // std::ofstream outputfile2("data.csv", std::ios::app);
  //   if (!outputfile2.is_open()) {
  //       std::cerr << "Error while opening CSV-File." << std::endl;
  //   }
  //   for (size_t i = 0; i < DATA.numRows();i++) {
  //     outputfile2 << DATA(i,0) << ", " << DATA(i,1) << std::endl;
  //   }

  /* EXPORT RESULTS */
  //   // Open CSV-file
  //   std::ofstream outputfile("results.csv", std::ios::app);
  //   if (!outputfile.is_open()) {
  //       std::cerr << "Error while opening CSV-File." << std::endl;
  //   }
  //   std::vector<double> peak_id = model.getPeakProperties(q::Peakproperties::PEAKID);
  //   std::vector<double> smpl_id = model.getPeakProperties(q::Peakproperties::SMPLID);
  //   std::vector<double> position = model.getPeakProperties(q::Peakproperties::POSITION);
  //   std::vector<double> height = model.getPeakProperties(q::Peakproperties::HEIGHT);
  //   std::vector<double> dqs = model.getPeakProperties(q::Peakproperties::DQS);
  //   for (size_t i = 0; i < peak_id.size(); i++) {
  //       outputfile << peak_id[i] << ","
  //            << smpl_id[i] << ","
  //            << position[i] << ","
  //            << height[i] << ","
  //            << dqs[i] << std::endl;
  //   }
  return 0;
}
