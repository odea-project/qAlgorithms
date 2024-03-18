// qalgorithm_measurement_data.cpp

// internal
#include "../include/qalgorithms_measurement_data.h"

namespace q {
  void MeasurementData::zeroFilling(
    std::vector<double>& xData, 
    std::vector<double>& yData, 
    int k) {
    // calculate the differences
    double expectedDifference = 0.0;
    std::vector<double> differences;
    for (int i = 1; i < xData.size(); i++) {
        differences.push_back(xData[i] - xData[i-1]);
    }
    // median of differences
    std::sort(differences.begin(), differences.end());
    if (differences.size() % 2 == 0) {
        expectedDifference = (differences[differences.size()/2 - 1] + differences[differences.size()/2]) / 2;
    } else {
        expectedDifference = differences[differences.size()/2];
    }

    // fill the gaps
    for (int i = 1; i < xData.size(); i++) {
      double difference = xData[i] - xData[i-1];
      /* We will always add new data points in the beginning of the data set. */
      if (i == 1) {
        int gapSize = (int) k/2;
        for (int j = gapSize; j >= 1; j--) { // fill the gap backwards
          xData.insert(xData.begin(), xData[0] - expectedDifference);
          yData.insert(yData.begin(), 0.0);
        }
        // add a separator
        xData.insert(xData.begin(), -1.0);
        yData.insert(yData.begin(), -1.0);
        i += gapSize+1; // Update the index to the next data point
      } else {
        if (difference > 1.75 * expectedDifference) {
          int gapSize = (int) (difference / expectedDifference - 1);
          /* Check if the gapsSize is larger than k, as this is the maximum gap size. If the gap size is less or equal to k, we add gapSize new data points to the data set between the two data points that show the gap, i.e. (i-1 and i). The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values.
          */
          if (gapSize <= k) {
            for (int j = gapSize; j >= 1; j--) { // fill the gap backwards
              xData.insert(xData.begin() + i, xData[i-1] + j * expectedDifference);
              yData.insert(yData.begin() + i, 0.0);
            }
          } else {
            /* If the gapSize is larger than k, we limit the gap size to k and add k/2 new data points close to the (i-1) data point and k/2 new data points close to the (i) data point. The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values. Moreover, we add a separator for later splitting the data set into smaller data sets by adding an additional zero in between where the intensity and x-axis are set to -1.0. This is done to indicate that the data set should be split at this point.  
            */
            gapSize = k;
            // add k/2 new data points close to the (i) data point
            for (int j = 1; j <= k/2; j++) { // fill the gap forwards
              xData.insert(xData.begin() + i, xData[i] -  expectedDifference);
              yData.insert(yData.begin() + i, 0.0);
            }
            // add a separator
            xData.insert(xData.begin() + i, -1.0);
            yData.insert(yData.begin() + i, -1.0);
            // add k/2 new data points close to the (i-1) data point 
            for (int j = k/2; j >= 1; j--) { // fill the gap backwards
              xData.insert(xData.begin() + i, xData[i-1] + j * expectedDifference);
              yData.insert(yData.begin() + i, 0.0);
            }     
          }
          i += gapSize; // Update the index to the next data point
        }  else {
          // update the expected difference
          expectedDifference = (expectedDifference + difference) / 2;
        } 
      }   
    }
    /* We will always add new data points in the end of the data set. */
    int gapSize = (int) k/2;
    for (int j = 1; j <= gapSize; j++) { // fill the gap forwards
      xData.push_back(xData[xData.size()-1] + expectedDifference);
      yData.push_back(0.0);
    }
  }

  std::vector<size_t> MeasurementData::cutData(std::vector<double>& xData, std::vector<double>& yData) const {
    // find the separator
    std::vector<size_t> separators;
    for (int i = 0; i < xData.size(); i++) {
      if (xData[i] == -1.0 && yData[i] == -1.0) {
        separators.push_back(i);
      }
    }
    return separators;
  }
} // namespace q