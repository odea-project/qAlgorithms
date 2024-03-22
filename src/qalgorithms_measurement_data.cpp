// qalgorithm_measurement_data.cpp

// internal
#include "../include/qalgorithms_measurement_data.h"

namespace q
{
  void MeasurementData::zeroFilling(varDataType &dataMap, int k)
  {
    std::visit([k](auto &&arg) 
    {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>>*>)
      {
        // iterate over the map of varDataType objects
        for (auto& pair : *arg)
        {
          // de-reference the unique pointer of the object
          auto &dataObj = *(pair.second.get());
          // initialize the expected difference
          double expectedDifference = 0.0;
          // initialize the vector of differences
          std::vector<double> differences;
          // iterate over data point vector, which is a vector of unique pointers to data points structures
          auto& data = dataObj.dataPoints;
        
          double previousX = data[0]->x();
          for (int i = 1; i < data.size(); i++)
          {
            double difference = data[i]->x() - previousX;
            differences.push_back(difference);
            previousX = data[i]->x();
          }

          // median of differences
          // copy the differences vector and sort it
          std::vector<double> differences_copy = differences;
          std::sort(differences_copy.begin(), differences_copy.end());
          int n = differences_copy.size();
          if (n % 2 == 0)
          {
            expectedDifference = (differences_copy[n / 2 - 1] + differences_copy[n / 2]) / 2;
          }
          else
          {
            expectedDifference = differences_copy[n / 2];
          }
          // delete the copy of the differences vector
          std::vector<double>().swap(differences_copy);

          /* analyze the data for gaps, i.e., differences > 1.75 * expectedDifference, and fill the gaps by adding new data points at the end of the vector
          */
          n++; // adjust n to fit with the size of the data vector
          size_t counter = k/2; // counter for the number of added data points to store separator positions. it is initialized with k/2 to address that later k/2 data points will be added at the front of the vector
          std::vector<size_t> separators; 
          for (int i = 1; i < n; i++)
          {
            // consider the difference between two neighboring data points from differences vector and compare it with 1.75 * expectedDifference
            if (differences[i - 1] > 1.75 * expectedDifference)
            {
              // calculate the gap size
              int gapSize = (int)(differences[i - 1] / expectedDifference - 1);
              // check if the gapSize is larger than k, as this is the maximum gap size
              if (gapSize <= k)
              {
                // add gapSize new data points at the end of the vector
                for (int j = 1; j <= gapSize; j++)
                {
                  // define the new pointer for new data point and add it to the data vector using the addDataPoint method from the respecting class
                  dataObj.addDataPoint(0.0, data[i-1]->x() + j * expectedDifference, 0);
                }
                counter += gapSize;
              }
              else
              {
                // limit the gap size to k and add k/2 new data points close to the (i-1) data point and k/2 new data points close to the (i) data point
                gapSize = k;
                // add k/2 new data points close to the (i) and (i-1) data point
                for (int j = 1; j <= k / 2; j++)
                {
                  // define the new pointer for new data point and add it to the data vector
                  dataObj.addDataPoint(0.0, data[i]->x() - j * expectedDifference, 0);
                  dataObj.addDataPoint(0.0, data[i - 1]->x() + j * expectedDifference, 0);
                }
                counter += k/2;
                // add the separator to the vector
                separators.push_back(counter);
                counter += k/2;
              }
            }
            counter++;
          }
          // extrapolate the data by adding k/2 new data points at the end of the vector
          int gapSize = (int)k / 2;
          for (int j = 1; j <= gapSize; j++)
          {
            // define the new pointer for new data point and add it to the data vector
            dataObj.addDataPoint(0.0, data[n - 1]->x() + j * expectedDifference, 0);
            dataObj.addDataPoint(0.0, data[0]->x() - j * expectedDifference, 0);
          }
          // flip separators vector 
          std::reverse(separators.begin(), separators.end());
          // cumulative difference for separators
          for (int i = 0; i < separators.size(); i++)
          {
            if (i == 0)
            {
              separators[i] = dataObj.dataPoints.size() - separators[i] - 1;
            }
            else
            {
              separators[i] = (dataObj.dataPoints.size() - separators[i] - 1 - separators[i - 1]);
            }
          }
          // add the separators to the Object's cuttingPoints vector, which is a vector of unique pointers to size_t objects
          for (int i = 0; i < separators.size(); i++)
          {
            dataObj.cuttingPoints.push_back(std::make_unique<size_t>(separators[i]));
          }
          // sort the data points by x-axis values
          dataObj.sortDataPoints();
        } // end of for loop
      } // end of if statement
    }, dataMap); // end of visit
  } // end of zeroFilling

  void MeasurementData::cutData(varDataType &dataMap) {
    std::visit([](auto &&arg) 
    {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>>*>)
      {
        // iterate over the map of varDataType objects
        for (auto& pair : *arg)
        {
          // de-reference the unique pointer of the object
          auto &dataObj = *(pair.second.get());
          auto& cuttingPoints = dataObj.cuttingPoints;
            // iterate over the cutting points vector
          for (int i = 0; i < cuttingPoints.size(); i++)
          {
            
          }
        } // end of for loop
      } // end of if statement
    }, dataMap); // end of visit
  } // end of cutData


  // std::vector<size_t> MeasurementData::cutData(
  //     std::vector<double> &xData,
  //     std::vector<double> &yData) const
  // {
  //   // find the separator
  //   std::vector<size_t> separators;
  //   for (int i = 0; i < xData.size(); i++)
  //   {
  //     if (xData[i] == -1.0 && yData[i] == -1.0)
  //     {
  //       separators.push_back(i);
  //     }
  //   }
  //   return separators;
  // }

  // void MeasurementData::interpolateData(
  //     std::vector<double> &xData,
  //     std::vector<double> &yData)
  // {
  //   /* Handle the beginning of the data. Hereby, the data always starts with n zeros. Therefore we need to find the first three data points that are not zero.*/
  //   int i = 0;
  //   int I = 0;
  //   int flag = 0;
  //   std::vector<double> xDataTemp;
  //   std::vector<double> yDataTemp;
  //   while (flag < 3)
  //   {
  //     if (i >= xData.size())
  //     {
  //       std::cerr << "Error: The data set is too small to interpolate." << std::endl;
  //       break;
  //     }
  //     if (yData[i] > 0.0)
  //     {
  //       xDataTemp.push_back(xData[i]);
  //       yDataTemp.push_back(log(yData[i])); // we use log space for interpolation
  //       if (flag == 0)
  //       {
  //         I = i;
  //       }
  //       flag++;
  //     }
  //     i++;
  //   }
  //   // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
  //   Matrix B = linreg(xDataTemp, yDataTemp, 2);
  //   // calculate the interpolated y-axis values
  //   i = 0;
  //   while (yData[i] == 0.0)
  //   {
  //     yData[i] = exp(B(0, 0) + B(1, 0) * xData[i] + B(2, 0) * xData[i] * xData[i]);
  //     i++;
  //   }

  // /* Handle the end of the data. Hereby, the data always ends with n zeros. Therefore we need to find the last three data points that are not zero.*/
  // int j = xData.size() - 1;
  // int J = 0;
  // flag = 0;
  // xDataTemp.clear();
  // yDataTemp.clear();
  // while (flag < 3)
  // {
  //   if (j < 0)
  //   {
  //     std::cerr << "Error: The data set is too small to interpolate." << std::endl;
  //     break;
  //   }
  //   if (yData[j] > 0.0)
  //   {
  //     xDataTemp.push_back(xData[j]);
  //     yDataTemp.push_back(log(yData[j])); // we use log space for interpolation
  //     if (flag == 0)
  //     {
  //       J = j;
  //     }
  //     flag++;
  //   }
  //   j--;
  // }
  // // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
  // B = linreg(xDataTemp, yDataTemp, 2);
  // // calculate the interpolated y-axis values
  // j = xData.size() - 1;
  // while (yData[j] == 0.0)
  // {
  //   yData[j] = exp(B(0, 0) + B(1, 0) * xData[j] + B(2, 0) * xData[j] * xData[j]);
  //   j--;
  // }

  // /* Handle the middle of the data. Hereby, we use a quadratic interpolation to fill the gaps between the data points. */
  // // iterate over the data points from I to J
  // for (int i = I; i <= J; i++)
  // {
  //   if (yData[i] == 0.0)
  //   {
  //     // now, we asume the last value before the zero appears at index "i-1" and we need to find the next data point "j" that is not zero to define the range for the interpolation
  //     int j = i + 1;
  //     while (yData[j] == 0.0)
  //     {
  //       if (j >= J)
  //       {
  //         break;
  //       }
  //       j++;
  //     }
  //     // now we have the range from "i-1" to "j" for the interpolation and we need to find the next two data points that are not zero on each side of the range to use them for the interpolation as reference points
  //     xDataTemp.clear();
  //     yDataTemp.clear();
  //     int k = i - 1; // first reference point to the left
  //     int l = j;     // first reference point to the right

  //     // side: left
  //     // find the next two data points that are not zero
  //     int flag = 0;
  //     while (flag < 2)
  //     {
  //       if (k < I)
  //       {
  //         break;
  //       }
  //       if (yData[k] > 0.0)
  //       {
  //         xDataTemp.push_back(xData[k]);
  //         yDataTemp.push_back(log(yData[k])); // we use log space for interpolation
  //         flag++;
  //       }
  //       k--;
  //     }
  //     // side: right
  //     // find the next two data points that are not zero
  //     flag = 0;
  //     while (flag < 2)
  //     {
  //       if (l > J)
  //       {
  //         break;
  //       }
  //       if (yData[l] > 0.0)
  //       {
  //         xDataTemp.push_back(xData[l]);
  //         yDataTemp.push_back(log(yData[l])); // we use log space for interpolation
  //         flag++;
  //       }
  //       l++;
  //     }
  //     // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
  //     B = linreg(xDataTemp, yDataTemp, 2);
  //     // calculate the interpolated y-axis values
  //     for (int n = i; n < j; n++)
  //     {
  //       yData[n] = exp(B(0, 0) + B(1, 0) * xData[n] + B(2, 0) * xData[n] * xData[n]);
  //     }
  //   }
  // }
  // }

} // namespace q