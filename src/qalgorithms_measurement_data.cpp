// qalgorithm_measurement_data.cpp

// internal
#include "../include/qalgorithms_measurement_data.h"

namespace q
{
  void MeasurementData::zeroFilling(data &data, int k = 8)
  {
    // calculate the differences
    double expectedDifference = 0.0;
    std::vector<double> differences;
    // iterate over data, which is a variant of different data types and a vector of unique pointers to data points structures
    std::visit([&](auto &&arg)
               {
        double previousX = arg[0]->x();
        for (int i = 1; i < arg.size(); i++)
        {
          double difference = arg[i]->x() - previousX;
          differences.push_back(difference);
          previousX = arg[i]->x();
        } 

    // median of differences
    std::sort(differences.begin(), differences.end());
    if (differences.size() % 2 == 0)
    {
      expectedDifference = (differences[differences.size() / 2 - 1] + differences[differences.size() / 2]) / 2;
    }
    else
    {
      expectedDifference = differences[differences.size() / 2];
    }

    /* analyze the data for gaps, i.e., differences > 1.75 * expectedDifference, and fill the gaps by adding new data points at the end of the vector
    */
    // size of the data vector, needed for the loop to avoid looping over the new data points
    int vector_size = differences.size()+1;
    
    for (int i = 1; i < vector_size; i++)
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
          for (int j = gapSize; j >= 1; j--)
          {
            arg.push_back(std::make_unique<arg[0]->DataPoint>(0.0, arg[i - 1]->x() + j * expectedDifference, 0));
            // dataPoints.push_back(std::make_unique<DataType::MassSpectrum::DataPoint>(0.0, dataPoints[i - 1]->x() + j * expectedDifference, 0));
          }
        }
      }
    }
    },data);
  }

        // for (int i = 1; i < xData.size(); i++)
        // {
        //   double difference = xData[i] - xData[i - 1];
        //   /* We will always add new data points in the beginning of the data set. */
        //   if (i == 1)
        //   {
        //     int gapSize = (int)k / 2;
        //     for (int j = gapSize; j >= 1; j--)
        //     { // fill the gap backwards
        //       xData.insert(xData.begin(), xData[0] - expectedDifference);
        //       yData.insert(yData.begin(), 0.0);
        //     }
        //     // add a separator
        //     xData.insert(xData.begin(), -1.0);
        //     yData.insert(yData.begin(), -1.0);
        //     i += gapSize + 1; // Update the index to the next data point
        //   }
        //   else
        //   {
        //     if (difference > 1.75 * expectedDifference)
        //     {
        //       int gapSize = (int)(difference / expectedDifference - 1);
        //       /* Check if the gapsSize is larger than k, as this is the maximum gap size. If the gap size is less or equal to k, we add gapSize new data points to the data set between the two data points that show the gap, i.e. (i-1 and i). The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values.
        //        */
        //       if (gapSize <= k)
        //       {
        //         for (int j = gapSize; j >= 1; j--)
        //         { // fill the gap backwards
        //           xData.insert(xData.begin() + i, xData[i - 1] + j * expectedDifference);
        //           yData.insert(yData.begin() + i, 0.0);
        //         }
        //       }
        //       else
        //       {
        //         /* If the gapSize is larger than k, we limit the gap size to k and add k/2 new data points close to the (i-1) data point and k/2 new data points close to the (i) data point. The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values. Moreover, we add a separator for later splitting the data set into smaller data sets by adding an additional zero in between where the intensity and x-axis are set to -1.0. This is done to indicate that the data set should be split at this point.
        //          */
        //         gapSize = k;
        //         // add k/2 new data points close to the (i) data point
        //         for (int j = 1; j <= k / 2; j++)
        //         { // fill the gap forwards
        //           xData.insert(xData.begin() + i, xData[i] - expectedDifference);
        //           yData.insert(yData.begin() + i, 0.0);
        //         }
        //         // add a separator
        //         xData.insert(xData.begin() + i, -1.0);
        //         yData.insert(yData.begin() + i, -1.0);
        //         // add k/2 new data points close to the (i-1) data point
        //         for (int j = k / 2; j >= 1; j--)
        //         { // fill the gap backwards
        //           xData.insert(xData.begin() + i, xData[i - 1] + j * expectedDifference);
        //           yData.insert(yData.begin() + i, 0.0);
        //         }
        //       }
        //       i += gapSize; // Update the index to the next data point
        //     }
        //     else
        //     {
        //       // update the expected difference
        //       expectedDifference = (expectedDifference + difference) / 2;
        //     }
        //   }
        // }
        // /* We will always add new data points in the end of the data set. */
        // int gapSize = (int)k / 2;
        // for (int j = 1; j <= gapSize; j++)
        // { // fill the gap forwards
        //   xData.push_back(xData[xData.size() - 1] + expectedDifference);
        //   yData.push_back(0.0);
        // }
      }

      std::vector<size_t> MeasurementData::cutData(
          std::vector<double> & xData,
          std::vector<double> & yData) const
      {
        // find the separator
        std::vector<size_t> separators;
        for (int i = 0; i < xData.size(); i++)
        {
          if (xData[i] == -1.0 && yData[i] == -1.0)
          {
            separators.push_back(i);
          }
        }
        return separators;
      }

      void MeasurementData::interpolateData(
          std::vector<double> & xData,
          std::vector<double> & yData)
      {
        /* Handle the beginning of the data. Hereby, the data always starts with n zeros. Therefore we need to find the first three data points that are not zero.*/
        int i = 0;
        int I = 0;
        int flag = 0;
        std::vector<double> xDataTemp;
        std::vector<double> yDataTemp;
        while (flag < 3)
        {
          if (i >= xData.size())
          {
            std::cerr << "Error: The data set is too small to interpolate." << std::endl;
            break;
          }
          if (yData[i] > 0.0)
          {
            xDataTemp.push_back(xData[i]);
            yDataTemp.push_back(log(yData[i])); // we use log space for interpolation
            if (flag == 0)
            {
              I = i;
            }
            flag++;
          }
          i++;
        }
        // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
        Matrix B = linreg(xDataTemp, yDataTemp, 2);
        // calculate the interpolated y-axis values
        i = 0;
        while (yData[i] == 0.0)
        {
          yData[i] = exp(B(0, 0) + B(1, 0) * xData[i] + B(2, 0) * xData[i] * xData[i]);
          i++;
        }

        /* Handle the end of the data. Hereby, the data always ends with n zeros. Therefore we need to find the last three data points that are not zero.*/
        int j = xData.size() - 1;
        int J = 0;
        flag = 0;
        xDataTemp.clear();
        yDataTemp.clear();
        while (flag < 3)
        {
          if (j < 0)
          {
            std::cerr << "Error: The data set is too small to interpolate." << std::endl;
            break;
          }
          if (yData[j] > 0.0)
          {
            xDataTemp.push_back(xData[j]);
            yDataTemp.push_back(log(yData[j])); // we use log space for interpolation
            if (flag == 0)
            {
              J = j;
            }
            flag++;
          }
          j--;
        }
        // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
        B = linreg(xDataTemp, yDataTemp, 2);
        // calculate the interpolated y-axis values
        j = xData.size() - 1;
        while (yData[j] == 0.0)
        {
          yData[j] = exp(B(0, 0) + B(1, 0) * xData[j] + B(2, 0) * xData[j] * xData[j]);
          j--;
        }

        /* Handle the middle of the data. Hereby, we use a quadratic interpolation to fill the gaps between the data points. */
        // iterate over the data points from I to J
        for (int i = I; i <= J; i++)
        {
          if (yData[i] == 0.0)
          {
            // now, we asume the last value before the zero appears at index "i-1" and we need to find the next data point "j" that is not zero to define the range for the interpolation
            int j = i + 1;
            while (yData[j] == 0.0)
            {
              if (j >= J)
              {
                break;
              }
              j++;
            }
            // now we have the range from "i-1" to "j" for the interpolation and we need to find the next two data points that are not zero on each side of the range to use them for the interpolation as reference points
            xDataTemp.clear();
            yDataTemp.clear();
            int k = i - 1; // first reference point to the left
            int l = j;     // first reference point to the right

            // side: left
            // find the next two data points that are not zero
            int flag = 0;
            while (flag < 2)
            {
              if (k < I)
              {
                break;
              }
              if (yData[k] > 0.0)
              {
                xDataTemp.push_back(xData[k]);
                yDataTemp.push_back(log(yData[k])); // we use log space for interpolation
                flag++;
              }
              k--;
            }
            // side: right
            // find the next two data points that are not zero
            flag = 0;
            while (flag < 2)
            {
              if (l > J)
              {
                break;
              }
              if (yData[l] > 0.0)
              {
                xDataTemp.push_back(xData[l]);
                yDataTemp.push_back(log(yData[l])); // we use log space for interpolation
                flag++;
              }
              l++;
            }
            // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
            B = linreg(xDataTemp, yDataTemp, 2);
            // calculate the interpolated y-axis values
            for (int n = i; n < j; n++)
            {
              yData[n] = exp(B(0, 0) + B(1, 0) * xData[n] + B(2, 0) * xData[n] * xData[n]);
            }
          }
        }
      }

    } // namespace q