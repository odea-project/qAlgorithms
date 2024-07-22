// qalgorithm_measurement_data.cpp

// internal
#include "../include/qalgorithms_measurement_data.h"

namespace q
{
  namespace MeasurementData
  {
    void MeasurementData::zeroFilling(varDataType &dataVec, int k)
    {
      std::visit([k](auto &&arg)
                 {
                   // iterate over the vector of varDataType datatype objects
                   for (auto &pair : *arg)
                   {
                     // de-reference the unique pointer of the object
                     auto &dataObj = *(pair.get());
                     // initialize the expected difference
                     double expectedDifference = 0.0;
                     // initialize the vector of differences
                     std::vector<double> differences;
                     // iterate over data point vector, which is a vector of unique pointers to data points structures
                     auto &data = dataObj.dataPoints;

                     double previousX = data[0]->x();
                     for (size_t i = 1; i < data.size(); i++)
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
                     n++;                    // adjust n to fit with the size of the data vector
                     size_t counter = k / 2; // counter for the number of added data points to store separator positions. it is initialized with k/2 to address that later k/2 data points will be added at the front of the vector
                     std::vector<size_t> separators;
                     for (int i = 1; i < n; i++)
                     {
                       // consider the difference between two neighboring data points from differences vector and compare it with 1.75 * expectedDifference
                       if (differences[i - 1] > 1.75 * expectedDifference)
                       {
                         // calculate the gap size; need to be rounded to the nearest integer
                         int gapSize = std::round(differences[i - 1] / expectedDifference - 1);
                         // check if the gapSize is larger than k, as this is the maximum gap size
                         if (gapSize <= k)
                         {
                           // add gapSize new data points at the end of the vector
                           for (int j = 1; j <= gapSize; j++)
                           {
                             // define the new pointer for new data point and add it to the data vector using the addDataPoint method from the respecting class
                             dataObj.addDataPoint(0.0, data[i - 1]->x() + j * expectedDifference, 0);
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
                           counter += k / 2;
                           // add the separator to the vector
                           separators.push_back(counter);
                           counter += k / 2;
                         }
                       }
                       else
                       {
                         // update expectedDifference
                         expectedDifference = expectedDifference * .5 + differences[i - 1] * .5;
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
                     // add the last index of data vector to the separators vector
                     separators.push_back(dataObj.dataPoints.size() - 1);
                     // flip separators vector
                     std::reverse(separators.begin(), separators.end());
                     // cumulative difference for separators
                     for (size_t i = 0; i < separators.size() - 1; i++)
                     {
                       separators[i] = separators[i] - separators[i + 1];
                     }
                     // delete the last element of the separators vector
                     separators.pop_back();
                     // add the separators to the Object's cuttingPoints vector, which is a vector of unique pointers to size_t objects
                     for (size_t i = 0; i < separators.size(); i++)
                     {
                       dataObj.cuttingPoints.push_back(std::make_unique<size_t>(separators[i]));
                     }
                     // sort the data points by x-axis values
                     dataObj.sortDataPoints();
                   } // end of for loop
                 },
                 dataVec); // end of visit
    } // end of zeroFilling

    int
    MeasurementData::zeroFilling_vec(
        std::vector<std::vector<double>> &data,
        double expectedDifference,
        const bool updateExpectedDifference)
    {
      const int numPoints = data[0].size();       // number of data points
      std::vector<int> sorted_indexes(numPoints); // vector to store the sorted indexes of the data points
      int counter = 4;                            // number of added zeros to store separator positions. it is initialized with 4 to address that later 4 data points will be added at the front of the vector
      int numZeros = 0;                           // number of zeros
      int num_subsets = 0;                        // number of subsets
      bool containsDublicates = false;            // flag to indicate if the data contains dublicates
      const size_t size_data = data.size();

      // lamda function to add new data points at the end of the vector
      auto addDataPoints = [&data, &counter, &numZeros, &expectedDifference, &sorted_indexes, size_data](int gapSize, int i, int k, int index_offset, int loop_index, int sign, bool interpolate) -> void
      {
        if (interpolate)
        {
          // calculate the interpolated y-axis values using log interpolation
          const double diff = std::pow(data[1][i + 1] / data[1][i], 1.0 / (gapSize + 1));
          // const double differece_y_per_gapsize = (data[1][i + 1] - data[1][i]) / (gapSize + 1);

          for (int j = 0; j < gapSize; j++)
          {
            data[0].push_back(data[0][i] + loop_index * expectedDifference * sign);
            data[1].push_back(data[1][i] * std::pow(diff, j + 1));
            for (size_t l = 2; l < size_data; l++)
            {
              data[l].push_back(0.0);
            }
            sorted_indexes.push_back(sorted_indexes[k] + j + index_offset);
            loop_index += sign;
          }
        }
        else
        {
          for (int j = 0; j < gapSize; j++)
          {
            data[0].push_back(data[0][i] + loop_index * expectedDifference * sign);
            for (size_t l = 1; l < size_data; l++)
            {
              data[l].push_back(0.0);
            }
            sorted_indexes.push_back(sorted_indexes[k] + j + index_offset);
            loop_index += sign;
          }
        }
        counter += gapSize;
        numZeros += gapSize;
      };

      // analyze data for gaps, i.e., differences > 1.75 * expectedDifference, and fill the gaps by adding new data points at the end of the vector
      for (int i = 0; i < numPoints - 1; i++)
      {
        sorted_indexes[i] = counter;
        // consider the difference between two neighboring data points from differences vector and compare it with 1.75 * expectedDifference
        float difference = data[0][i + 1] - data[0][i];
        if (difference > 1.75f * expectedDifference)
        {
          // calculate the gap size; need to be rounded to the nearest integer
          int gapSize = static_cast<int>(difference / expectedDifference - 1);
          // check if the gapSize is larger than k, as this is the maximum gap size
          if (gapSize <= 8)
          {                                              // SMALL GAP
            addDataPoints(gapSize, i, i, 1, 1, 1, true); // add gapSize new data points at the end of the vector
          }
          else
          { // LARGE GAP
            // limit the gap size to 8 and add 4 new data points close to the (i) and (i+1) data point
            gapSize = 8;
            addDataPoints(4, i, i, 1, 1, 1, false);      // add 4 new data points close to the (i) data point
            addDataPoints(4, i + 1, i, 5, 4, -1, false); // add 4 new data points close to the (i+1) data point
            num_subsets++;
          }
        }
        else
        {
          // check for dublicates, i.e., difference < .1 * expectedDifference
          if (difference < .1 * expectedDifference)
          {
            containsDublicates = true;
            if (data[1][i] > data[1][i + 1])
            {
              data[2][i + 1] = -255.0; // mark the data point for deletion
            }
            else
            {
              data[2][i] = -255.0; // mark the data point for deletion
            }
          }
          else
          {
            // update expectedDifference
            if (updateExpectedDifference)
            {
              expectedDifference = (expectedDifference + data[0][i + 1] - data[0][i]) * .5;
            }
          }
        }
        counter++;
      }
      sorted_indexes[numPoints - 1] = counter;
      // extrapolate the data by adding 4 new data points at the end of the vector
      addDataPoints(4, 0, 0, 1, 4, -1, false); // extrapolation to the left
      // correct the sorted indexes for the first 4 data points to 0, 1, 2, 3
      for (int i = 0; i < 4; i++)
      {
        sorted_indexes[sorted_indexes.size() - 4 + i] = i;
      }
      addDataPoints(4, numPoints - 1, numPoints - 1, 1, 1, 1, false); // extrapolation to the right

      for (int i = 0; i < static_cast<int>(sorted_indexes.size()); ++i)
      {
        while (sorted_indexes[i] != i)
        {
          for (size_t j = 0; j < size_data; j++)
          {
            std::swap(data[j][i], data[j][sorted_indexes[i]]);
          }
          std::swap(sorted_indexes[i], sorted_indexes[sorted_indexes[i]]);
        }
      }
      if (containsDublicates)
      {
        for (int i = 0; i < numPoints; i++)
        {
          if (data[2][i] == -255.0)
          {
            for (size_t j = 0; j < size_data; j++)
            {
              data[j][i] = -255.0;
            }
          }
        }
        auto removeCondition = [](double value)
        { return value == -255.0; };
        for (size_t j = 0; j < size_data; j++)
        {
          data[j].erase(std::remove_if(data[j].begin(), data[j].end(), removeCondition), data[j].end());
        }
      }

      return num_subsets + 1;
    } // end of zeroFilling_vec

    double
    MeasurementData::calcExpectedDiff(std::vector<double> &data)
    {
      const int numPoints = data.size(); // number of data points
      double expectedDifference = 0.0;
      if (numPoints > 128)
      { // static approach: mean of the 8 lowest distances
        alignas(64) double differences[128];
        for (int i = 0; i < 128 - 1; i++)
        {
          differences[i] = data[i + 1] - data[i];
        }
        std::sort(differences, differences + 128);
        for (int i = 0; i < 8; i++)
        {
          expectedDifference += differences[i];
        }
        expectedDifference /= 8;
      }
      else
      { // dynamic approach
        alignas(64) double *differences = new double[numPoints - 1];
        for (int i = 0; i < numPoints - 1; i++)
        {
          differences[i] = data[i + 1] - data[i];
        }
        std::sort(differences, differences + numPoints - 1);
        for (int i = 0; i < std::min(8, numPoints - 1); i++)
        {
          expectedDifference += differences[i];
        }
        expectedDifference /= std::min(8, numPoints - 1);
        delete[] differences;
      }
      return expectedDifference;
    }

    void
    MeasurementData::isZeroFillingNeeded(
        std::vector<double> &data,
        bool &needsZeroFilling)
    {
      int numOfBlocks = 0;
      const int zeros_in_a_row = 8; // typical value for Orbitrap data
      for (auto it = data.begin(); it != data.end(); ++it)
      {
        int counter = 0;
        while (*it == 0.0)
        {
          counter++;
          it++;
          if (it == data.end())
          {
            it--;
            break;
          }
        }
        if (counter == zeros_in_a_row)
        {
          numOfBlocks++;
        }
      }
      if (numOfBlocks > 3)
      {
        needsZeroFilling = false;
      }
    }

    void MeasurementData::cutData(varDataType &dataVec, size_t &maxKey)
    {
      std::visit([&maxKey](auto &&arg)
                 {
                   std::vector<size_t> keys;
                   for (size_t i = 0; i <= maxKey; i++)
                   {
                     keys.push_back(i);
                   }

                   // iterate over the vector of varDataType objects
                   for (const size_t &key : keys)
                   {
                     // de-reference the unique pointer of the datatype object
                     auto &dataObj = *((*arg)[key].get());
                     auto &cuttingPoints = dataObj.cuttingPoints;
                     // iterate over the cutting points vector
                     for (auto &uniquePtr : cuttingPoints)
                     {
                       auto &k = *(uniquePtr.get());
                       // create a new object from the parent object
                       // get the Object Type from T
                       using ObjectType = std::decay_t<decltype(dataObj)>;
                       // create a new object from the parent object and add it to the arg vector as pointer considering maximum key value + 1
                       // update the maxKey
                       maxKey++;
                       arg->push_back(std::make_unique<ObjectType>(dataObj, k));
                     }
                     // delete the cutting points vector
                     std::vector<std::unique_ptr<size_t>>().swap(cuttingPoints);
                   } // end of for loop
                 },
                 dataVec); // end of visit
    } // end of cutData

    void
    MeasurementData::cutData_vec_orbitrap(
        std::vector<std::vector<double>> &data,
        double expectedDifference,
        std::vector<std::vector<double>::iterator> &separators)
    {
      auto it_x = data[0].begin() + 3;
      auto it_y = data[1].begin() + 3;
      for (; it_x != data[0].end() - 5; ++it_x, ++it_y)
      {
        if (*it_y != 0.0 || *(it_y + 1) != 0.0 || *(it_y - 3) != 0 || *(it_y + 4) != 0)
        { // skip if y is not like [0 0 0 0 0 0 0 0]
          continue;
        }

        if ((*it_x - *(it_x - 1)) > (*(it_x + 1) - *it_x) || (*(it_x + 1) - *it_x) < (*(it_x + 2) - *(it_x + 1)))
        { // skip if the largest gap is not in the middle of the 8 zeros
          continue;
        }

        if (*(it_x + 1) - *it_x > 10 * expectedDifference)
        {
          separators.push_back(it_x);
        }
        // else
        // {
        //   expectedDifference = (expectedDifference + *(it_x + 1) - *it_x) * .5;
        // }
      } // end of for loop
      separators.push_back(data[0].end() - 1);
    } // end of cutData_vec

    void MeasurementData::filterSmallDataSets(varDataType &dataVec)
    {
      std::visit([](auto &&arg)
                 {
                  // delete all children < 5
                  arg->erase(std::remove_if(arg->begin(), arg->end(),
                    [](const auto& pair) {
                      auto &dataObj = *(pair.get());
                      auto &dataPoints = dataObj.dataPoints;
                      double sum = 0.0;
                      for (size_t i = 0; i < dataPoints.size(); i++)
                      {
                        sum += dataPoints[i]->df;
                      }
                      return sum < 5 && dataObj.isParent.first == false;
                    }), arg->end());

                   // clean all parents < 5
                   for (auto &pair : *arg)
                   {
                     auto &dataObj = *(pair.get());
                     auto &dataPoints = dataObj.dataPoints;
                     double sum = 0.0;
                     for (size_t i = 0; i < dataPoints.size(); i++)
                     {
                       sum += dataPoints[i]->df;
                     }
                     // check if df is too small
                     if (sum < 5)
                     {

                       if (dataObj.isParent.first == true)
                       {
                         // clean dataPoints vector
                         dataPoints.clear();
                       }
                     }
                   } },
                 dataVec); // end of visit
    } // end of filterSmallDataSets

    void MeasurementData::interpolateData(varDataType &dataVec)
    {
      std::visit([](auto &&arg)
                 {
                   std::vector<int> keysToDelete;
                   // iterate over the vector of varDataType objects
                   for (auto it = arg->begin(); it != arg->end(); ++it)
                   {
                     // de-reference the unique pointer of the datatype object
                     auto &dataObj = *(it->get());
                     auto &dataPoints = dataObj.dataPoints;
                     // check if dataPoints is empty
                     if (dataPoints.size() > 0)
                     {
                       // ALL THE DATA SETS CONSIDERED HAVE AT LEAST 4 DATA POINTS WITH INTENSITY > 0 (due to filterSmallDataSets)
                       // extract the largest y value, which is used for thershoulding the extrapolations, i.e. MAX < 3 * max(y)
                       double MAX = 0.0;
                       for (size_t i = 0; i < dataPoints.size(); i++)
                       {
                         if (dataPoints[i]->y() > MAX)
                         {
                           MAX = dataPoints[i]->y();
                         }
                       }

                       /* Data points at the edges will be extrapolated. Therefore, first and last non-zero data points and maximum data point are log-transformed and considered for parabola definition. Edge data points are then replaced by the extrapolated values from parabola and back transformed into linear space using exp function.*/
                       // find the first and last data points in dataPoints->y() that are not zero and store them in xDataTemp and yDataTemp
                       std::vector<double> xDataTemp;
                       std::vector<double> yDataTemp;
                       size_t I = 0;
                       size_t J = 0;
                       // iterate over the data points to find the first data point that is not zero
                       for (size_t i = 0; i < dataPoints.size(); i++)
                       {
                         if (dataPoints[i]->y() > 0.0)
                         {
                           I = i;
                           xDataTemp.push_back(dataPoints[i]->x() - dataPoints[0]->x());
                           yDataTemp.push_back(log(dataPoints[i]->y())); // we use log space for extrapolation
                           break;
                         }
                       }
                       // iterate over the data points to find the last data point that is not zero
                       for (size_t j = dataPoints.size() - 1; j >= 0; j--)
                       {
                         if (dataPoints[j]->y() > 0.0)
                         {
                           J = j;
                           xDataTemp.push_back(dataPoints[j]->x() - dataPoints[0]->x());
                           yDataTemp.push_back(log(dataPoints[j]->y())); // we use log space for extrapolation
                           break;
                         }
                       }
                       // find the maximum data point
                       double max = 0.0;
                       size_t maxIndex = 0;
                       for (size_t k = I; k < J; k++)
                       {
                         if (dataPoints[k]->y() > max)
                         {
                           max = dataPoints[k]->y();
                           maxIndex = k;
                         }
                       }
                       xDataTemp.push_back(dataPoints[maxIndex]->x() - dataPoints[0]->x());
                       yDataTemp.push_back(log(dataPoints[maxIndex]->y())); // we use log space for extrapolation

                       /* For later peak detection, it has to be ensured that the maximum of y values is not the first or last non-zero data point.*/
                       if (maxIndex == I || maxIndex == J)
                       {
                         // delete /clear the arg vector entry
                         if (dataObj.isParent.first == false)
                         {
                           // Store the key of the element to delete
                           int indexToDelete = std::distance(arg->begin(), it);
                           keysToDelete.push_back(indexToDelete);
                         }
                         else
                         {
                           // clean dataPoints vector
                           dataPoints.clear();
                         }
                         continue;
                       }

                       // calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
                       q::Matrices::Matrix B = linreg(xDataTemp, yDataTemp, 2);
                       // extrapolate the y-axis values for i=0 to I-1 and j=J+1 to dataPoints.size()-1
                       for (size_t i = 0; i < I; i++)
                       {
                         dataPoints[i]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) * (dataPoints[i]->x() - dataPoints[0]->x())));
                         dataPoints[i]->df = 0;
                         // check if the extrapolated value is larger than 3 * MAX
                         if (dataPoints[i]->y() > 3 * MAX)
                         {
                           dataPoints[i]->setY(3 * MAX);
                         }
                       }
                       for (size_t j = J + 1; j < dataPoints.size(); j++)
                       {
                         dataPoints[j]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[j]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[j]->x() - dataPoints[0]->x()) * (dataPoints[j]->x() - dataPoints[0]->x())));
                         dataPoints[j]->df = 0;
                         // check if the extrapolated value is larger than 3 * MAX
                         if (dataPoints[j]->y() > 3 * MAX)
                         {
                           dataPoints[j]->setY(3 * MAX);
                         }
                       }

                       /* Handle the middle of the data. Hereby, we use a quadratic interpolation to fill the gaps between the data points. */
                       // iterate over the data points from I to J
                       for (size_t i = I; i <= J; i++)
                       {
                         if (dataPoints[i]->y() == 0.0)
                         {
                           // now, we asume the last value before the zero appears at index "i-1" and we need to find the next data point "j" that is not zero to define the range for the interpolation
                           size_t j = i + 1;
                           while (dataPoints[j]->y() == 0.0)
                           {
                             if (j >= J)
                             {
                               break;
                             }
                             j++;
                           }
                           // now we have the range from "i" to "j-1" for the interpolation and we need to find the next two data points that are not zero on each side of the range to use them for the interpolation as reference points
                           xDataTemp.clear();
                           yDataTemp.clear();
                           size_t k = i - 1; // first reference point to the left
                           size_t l = j;     // first reference point to the right

                           // side: left
                           // find the next two data points that are not zero
                           int flag = 0;
                           while (flag < 2)
                           {
                             if (k < I)
                             {
                               break;
                             }
                             if (dataPoints[k]->y() > 0.0)
                             {
                               xDataTemp.push_back(dataPoints[k]->x() - dataPoints[0]->x());
                               yDataTemp.push_back(log(dataPoints[k]->y())); // we use log space for interpolation
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
                             if (dataPoints[l]->y() > 0.0)
                             {
                               xDataTemp.push_back(dataPoints[l]->x() - dataPoints[0]->x());
                               yDataTemp.push_back(log(dataPoints[l]->y())); // we use log space for interpolation
                               flag++;
                             }
                             l++;
                           }
                           // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
                           B = linreg(xDataTemp, yDataTemp, 2);
                           // calculate the interpolated y-axis values
                           for (size_t n = i; n < j; n++)
                           {
                             dataPoints[n]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[n]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[n]->x() - dataPoints[0]->x()) * (dataPoints[n]->x() - dataPoints[0]->x())));
                             dataPoints[n]->df = 0;
                             // check if the interpolated value is larger than 3 * MAX
                             if (dataPoints[n]->y() > 3 * MAX)
                             {
                               dataPoints[n]->setY(3 * MAX);
                             }
                           }
                         }
                       }
                     }
                   }
                   // Delete the elements after the iteration
                   arg->erase(std::remove_if(arg->begin(), arg->end(),
                      [&keysToDelete, &arg](const auto& item) {
                          // Check if the current index is in keysToDelete
                          return std::find(keysToDelete.begin(), keysToDelete.end(), &item - &arg->front()) != keysToDelete.end();
                      }), arg->end()); },
                 dataVec); // end of visit
    } // end of interpolateData

    void
    MeasurementData::extrapolateData_vec(
        std::vector<std::vector<double>> &data,
        std::vector<std::vector<double>::iterator> &separators)
    {
      // lambda function to calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
      auto calculateCoefficients = [](const double *x, const double *y, double &b0, double &b1, double &b2)
      {
        double x1 = x[0], y1 = y[0];
        double x2 = x[1], y2 = y[1];
        double x3 = x[2], y3 = y[2];

        double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);

        double a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
        double b = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
        double c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

        b0 = c;
        b1 = b;
        b2 = a;
      };

      // define iterators for the data vector
      auto block_start_y = data[1].begin() + 4; // start of the block
      auto block_end_y = data[1].begin() + 4;   // end of the block
      auto block_max_y = data[1].begin() + 4;   // maximum of the block
      auto block_start_x = data[0].begin() + 4; // start of the block
      auto block_end_x = data[0].begin() + 4;   // end of the block
      auto block_max_x = data[0].begin() + 4;   // maximum of the block
      const auto end = data[1].end() - 5;       // last non-zero value
      int block_counter = 0;                    // number of the current block
      while (true)
      {
        // adjust iterators to the next block
        while (*block_start_y == 0.0)
        {
          block_start_y++;
          block_end_y++;
          block_max_y++;
          block_start_x++;
          block_end_x++;
          block_max_x++;
          if (block_end_y == data[1].end())
          {
            return;
          }
        }

        // find the maximum and end of the block
        while (*block_end_y != 0.0)
        {
          if (*block_end_y > *block_max_y)
          {
            block_max_y = block_end_y;
            block_max_x = block_end_x;
          }
          block_end_y++;
          block_end_x++;
        }
        // set separator
        separators[block_counter] = block_end_x + 3;
        block_counter++;
        // check if the block is valid
        if ((block_max_y == block_start_y) || (block_max_y == block_end_y))
        {
          // check if the last block is reached
          if (block_end_y - 1 == end)
          {
            return;
          }
          // jump to the next block
          block_start_y = block_end_y + 8;
          block_end_y = block_start_y;
          block_max_y = block_start_y;
          block_start_x = block_end_x + 8;
          block_end_x = block_start_x;
          block_max_x = block_start_x;
          continue;
        }

        // calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
        double x[3] = {0.0, *block_max_x - *block_start_x, *(block_end_x - 1) - *block_start_x};
        double y[3] = {std::log(*block_start_y), std::log(*block_max_y), std::log(*(block_end_y - 1))};
        double b0, b1, b2;
        calculateCoefficients(x, y, b0, b1, b2);
        // extrapolate the zeros
        auto it_y = block_start_y - 4;
        auto it_x = block_start_x - 4;
        for (int i = 0; i < 4; i++)
        {
          const double x = *it_x - *block_start_x;
          *it_y = exp_approx_d(b0 + x * (b1 + x * b2));
          it_y++;
          it_x++;
        }
        it_y = block_end_y;
        it_x = block_end_x;
        for (int i = 0; i < 4; i++)
        {
          const double x = *it_x - *block_start_x;
          *it_y = exp_approx_d(b0 + x * (b1 + x * b2));
          it_y++;
          it_x++;
        }

        if (block_end_y - 1 == end)
        {
          return;
        }
        // jump to the next block
        block_start_y = block_end_y + 8;
        block_end_y = block_start_y;
        block_max_y = block_start_y;
        block_start_x = block_end_x + 8;
        block_end_x = block_start_x;
        block_max_x = block_start_x;
      } // end of while loop
    } // end of extrapolateData_vec

    void
    MeasurementData::interpolateData_vec_orbitrap(
        std::vector<std::vector<double>> &data,
        std::vector<std::vector<double>::iterator> &separators)
    {
      // define iterators for the data vector
      auto it_x = data[0].begin() + 8;       // x-axis iterator
      auto it_y = data[1].begin() + 8;       // y-axis iterator
      auto it_df = data[2].begin() + 8;      // df iterator
      auto it_sep = separators.begin() + 1;  // separator iterator; start from the second separator
      const auto end = separators.end() - 1; // end of the separators vector; the last separator is not considered
      for (; it_sep != end; ++it_sep, it_x += 8, it_y += 8, it_df += 8)
      { // iterate over the blocks
        for (; it_x != *it_sep - 3; ++it_x, ++it_y, ++it_df)
        { // iterate in the block
          if (*it_y == 0.0)
          { // count number of zeros in a row
            int counter = 0;
            while (*it_y == 0.0)
            {
              *it_df = 0.0;
              it_df++;
              it_x++;
              it_y++;
              counter++;
              if (it_x == *it_sep - 3)
              {
                break;
              }
            } // end of while loop
            // log interpolate between it_y - counter and it_y
            double diff = 0.0;
            if (*(it_y - counter - 1) != 0.0)
            {                                                                      // ordinary case is considered
              diff = std::pow(*it_y / *(it_y - counter - 1), 1.0 / (counter + 1)); // nth root of the y values ratio
              if (diff == 0.0)
              { // there is no value to the right of the gap
                diff = 0.5;
              }
              for (auto it = it_y - counter; it != it_y; ++it)
              {
                *it = *(it - 1) * diff;
              }
            }
            else
            { // there is no value to the left of the gap
              for (auto it = it_y - 1; it != it_y - counter - 1; --it)
              {
                *it = *(it + 1) * .5; // half of the value to the right
              }
            } // end of if-else statement
          } // end of if (*it_y == 0.0)
          if (it_x == *it_sep - 3)
          {
            break;
          }
        } // end of for loop (block)
      } // end of for loop (separators)
    } // end of interpolateData_vec

    void
    MeasurementData::extrapolateData_vec_orbitrap(
        std::vector<std::vector<double>> &data,
        std::vector<std::vector<double>::iterator> &separators)
    {
      // lambda function to calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
      auto calculateCoefficients = [](const double *x, const double *y, double &b0, double &b1, double &b2)
      {
        double x1 = x[0], y1 = y[0];
        double x2 = x[1], y2 = y[1];
        double x3 = x[2], y3 = y[2];

        double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);

        double a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
        double b = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
        double c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

        b0 = c;
        b1 = b;
        b2 = a;
      };

      // define iterators for the data vector
      auto block_start_y = data[1].begin() + 8; // start of the block
      auto block_end_y = data[1].begin() + 8;   // end of the block
      auto block_max_y = data[1].begin() + 8;   // maximum of the block
      auto block_start_x = data[0].begin() + 8; // start of the block
      auto block_end_x = data[0].begin() + 8;   // end of the block
      auto block_max_x = data[0].begin() + 8;   // maximum of the block
      auto block_end_df = data[2].begin() + 8;  // df of the block
      auto it_df = data[2].begin() + 4;         // df of the block
      auto it_x = data[0].begin() + 4;          // x-axis iterator
      auto it_y = data[1].begin() + 4;          // y-axis iterator
      for (auto it_sep = separators.begin() + 1; it_sep != separators.end() - 1; ++it_sep)
      { // iterate over the blocks
        while (block_end_x != *it_sep - 3)
        { // find maximum of the block
          if (*block_end_y > *block_max_y)
          {
            block_max_y = block_end_y;
            block_max_x = block_end_x;
          }
          block_end_y++;
          block_end_x++;
          block_end_df++;
        }
        // calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
        double x[3] = {0.0, *block_max_x - *block_start_x, *(block_end_x - 1) - *block_start_x};
        double y[3] = {std::log(*block_start_y), std::log(*block_max_y), std::log(*(block_end_y - 1))};
        double b0, b1, b2;
        calculateCoefficients(x, y, b0, b1, b2);
        // extrapolate the zeros
        for (; it_y != block_start_y; ++it_x, ++it_y, ++it_df)
        { // extrapolate to the left
          const double x = *it_x - *block_start_x;
          *it_y = exp_approx_d(b0 + x * (b1 + x * b2));
          *it_df = 0.0;
        }
        // update the iterators
        it_x = block_end_x;
        it_y = block_end_y;
        it_df = block_end_df;
        for (; it_x != *it_sep + 1; ++it_x, ++it_y, ++it_df)
        {
          // extrapolate to the right
          const double x = *it_x - *block_start_x;
          *it_y = exp_approx_d(b0 + x * (b1 + x * b2));
          *it_df = 0.0;
        }
        // update the iterators
        block_start_y = block_end_y + 8;
        block_end_y = block_start_y;
        block_max_y = block_start_y;
        block_start_x = block_end_x + 8;
        block_end_x = block_start_x;
        block_max_x = block_start_x;
        block_end_df = block_end_df + 8;
      } // end of for loop (separators)
    } // end of extrapolateData_vec
  } // namespace MeasurementData
} // namespace q