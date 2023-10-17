// rcpp_ms_centroid_spectra_qCentroid.cpp

#include "../include/qalgorithms.h"
#include "../include/qalgorithms_utils.h"
#include <vector>
#include <cmath>
#include <chrono>


int main()
{
  // Requesting string input with a default value for filename
  std::cout << "Enter a file name (default is 'test/test_orbitrap.csv'): ";
  std::string userInputStr;
  std::getline(std::cin, userInputStr); // Using getline to read a full line (including spaces)

  if (userInputStr.empty()) {
      userInputStr = "test/test_orbitrap.csv";  // Assigning default value
  }

  // Requesting integer input for defining the scale limit for the peak model
  std::cout << "Enter a the Maximum Scale as Integer (default is 5): ";
  std::string lineInput;
  std::getline(std::cin, lineInput);
  std::istringstream iss(lineInput);
  int userInputInt;
  if (!(iss >> userInputInt)) {
        userInputInt = 5;  // Assigning default value if conversion fails
    }

  // LOAD SOME TEST DATA
  std::ifstream file(userInputStr);
  // std::ifstream file("test/test_tof.csv");
  std::string line;
  std::vector<std::vector<double>> matrix;

  while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string cell;
      std::vector<double> row;

      while (std::getline(ss, cell, ',')) {
          row.push_back(std::stod(cell));
      }

      matrix.push_back(row);
  }
  q::Matrix xyData(matrix.size(),3);
  for (int i = 0; i < matrix.size(); i++) {
    xyData(i,0) = matrix[i][0];
    xyData(i,1) = matrix[i][1];
    xyData(i,2) = matrix[i][2];
  }

  // Initialize tValues for statistical Tests
  q::tValues tVal;

  /* Initialize Peakmodel with scales from 2 to 5. A scale is defined as a window of datapoints from -n to n, i.e., a scale of 2 means [-2, -1, 0, 1, 2]. */
  q::Peakmodel model(userInputInt);

  // add tValues to the Peak model
  model.loadTValues(tVal);

  // add measurement data to the Peak model
  model.addMultipleMeasurements(xyData);
  
  // Capture the start time
  auto start = std::chrono::high_resolution_clock::now();
  model.runRegression();

  // Capture the end time
  auto finish = std::chrono::high_resolution_clock::now();

  // Calculate and print the elapsed time
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";

  // print some Peak (the 1st Peak will be printed)
  model[0].print();

  // create a vector of a certain peak property
  std::vector<double> test = model.getPeakProperties(q::Peakproperties::PEAKID);  

  /* Expected output for Orbitrap Test-Data:
  (0): 2484 --> 1212 --> 913 --> 165 --> 46
  (1): 128624 --> 55580 --> 33895 --> 5944 --> 1863
  (2): 65528 --> 28604 --> 18835 --> 3436 --> 1032
  (3): 78424 --> 33969 --> 22003 --> 3955 --> 1204
  Elapsed time: 1.34463 s
  Peak Properties:
  -----------------
  Peak ID: 0
  Sample ID: 0
  Position: 534.705
  Height: 4025.8
  Width: 0
  Area: 21261.3
  Position Uncertainty (sigma): 0.00107453
  Height Uncertainty (sigma): 406.339
  Width Uncertainty (sigma): 0
  Area Uncertainty (sigma): 6118.37
  DQS: 0.684031
  */

  return 0;
}
