// rcpp_ms_centroid_spectra_qCentroid.cpp

#include "../include/qalgorithms.h"
#include "../include/qalgorithms_utils.h"
#include <vector>
#include <cmath>
#include <chrono>

int main()
{
  // LOAD SOME TEST DATA
  std::ifstream file("test/test_orbitrap.csv");
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
  q::Peakmodel model(5);

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

  // print some Peak (the 500th Peak will be printed)
  model[500].print();

  return 0;
}
