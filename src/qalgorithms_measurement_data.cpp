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
                 if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>> *>)
                 {
                   // iterate over the map of varDataType datatype objects
                   for (auto &pair : *arg)
                   {
                     // de-reference the unique pointer of the object
                     auto &dataObj = *(pair.second.get());
                     // initialize the expected difference
                     double expectedDifference = 0.0;
                     // initialize the vector of differences
                     std::vector<double> differences;
                     // iterate over data point vector, which is a vector of unique pointers to data points structures
                     auto &data = dataObj.dataPoints;

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
                     // cumulative difference for separators // HERE IS SOME ERROR
                     for (int i = 0; i < separators.size() - 1; i++)
                     {
                       separators[i] = separators[i] - separators[i + 1];
                     }
                     // delete the last element of the separators vector
                     separators.pop_back();
                     // add the separators to the Object's cuttingPoints vector, which is a vector of unique pointers to size_t objects
                     for (int i = 0; i < separators.size(); i++)
                     {
                       dataObj.cuttingPoints.push_back(std::make_unique<size_t>(separators[i]));
                     }
                     // sort the data points by x-axis values
                     dataObj.sortDataPoints();
                   } // end of for loop
                 }   // end of if statement
               },
               dataMap); // end of visit
  }                      // end of zeroFilling

  void MeasurementData::cutData(varDataType &dataMap, int &maxKey)
  {
    std::visit([&maxKey](auto &&arg)
               {
                 using T = std::decay_t<decltype(arg)>;
                 if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>> *>)
                 {
                   std::vector<int> keys;
                   for (const auto &pair : *arg)
                   {
                     keys.push_back(pair.first);
                   }
                   // iterate over the map of varDataType objects
                   for (const auto &key : keys)
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
                       // create a new object from the parent object and add it to the arg map as pointer considering maximum key value + 1
                       // update the maxKey
                       maxKey++;
                       arg->emplace(maxKey, std::make_unique<ObjectType>(dataObj, k));
                     }
                     // delete the cutting points vector
                     std::vector<std::unique_ptr<size_t>>().swap(cuttingPoints);
                   } // end of for loop
                 }   // end of if statement
               },
               dataMap); // end of visit
  }                      // end of cutData

  void MeasurementData::filterSmallDataSets(varDataType &dataMap)
  {
    std::visit([](auto &&arg)
               {
                 using T = std::decay_t<decltype(arg)>;
                 if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>> *>)
                 {
                   // iterate over the map of varDataType objects
                   for (auto &pair : *arg)
                   {
                     // de-reference the unique pointer of the datatype object
                     auto &dataObj = *(pair.second.get());
                     // check if dataObj is child object (only children can be deleted)

                     auto &dataPoints = dataObj.dataPoints;
                     // sum the df values
                     double sum = 0.0;
                     for (int i = 0; i < dataPoints.size(); i++)
                     {
                       sum += dataPoints[i]->df;
                     }
                     // check if df is too small
                     if (sum < 5)
                     {

                       if (dataObj.isParent.first == false)
                       {
                         // delete the data set
                         arg->erase(pair.first);
                       }
                       else
                       {
                         // clean dataPoints vector
                         dataPoints.clear();
                       }
                     }
                   }
                 } // end of if statement
               },
               dataMap); // end of visit
  }                      // end of filterSmallDataSets

  void MeasurementData::interpolateData(varDataType &dataMap)
  {
    std::visit([](auto &&arg)
               {
                 using T = std::decay_t<decltype(arg)>;
                 if constexpr (std::is_same_v<T, std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>> *>)
                 {
                   // iterate over the map of varDataType objects
                   for (auto &pair : *arg)
                   {
                     // de-reference the unique pointer of the datatype object
                     auto &dataObj = *(pair.second.get());
                     auto &dataPoints = dataObj.dataPoints;
                     // check if dataPoints is empty
                     if (dataPoints.size() > 0)
                     {
                       // ALL THE DATA SETS CONSIDERED HAVE AT LEAST 4 DATA POINTS WITH INTENSITY > 0 (due to filterSmallDataSets)
                       // extract the largest y value, which is used for thershoulding the extrapolations, i.e. MAX < 3 * max(y)
                       double MAX = 0.0;
                       for (int i = 0; i < dataPoints.size(); i++)
                       {
                         if (dataPoints[i]->y() > MAX)
                         {
                           MAX = dataPoints[i]->y();
                         }
                       }

                       /* Handle the beginning of the data. Hereby, the data always starts with n zeros. Therefore we need to find the first four data points that are not zero.*/
                       int i = 0;
                       int I = 0;
                       int flag = 0;
                       std::vector<double> xDataTemp;
                       std::vector<double> yDataTemp;
                       while (flag < 4)
                       {
                         if (i >= dataPoints.size())
                         {
                           std::cerr << "Error: The data set is too small to interpolate." << std::endl;
                           break;
                         }
                         if (dataPoints[i]->y() > 0.0)
                         {
                           xDataTemp.push_back(dataPoints[i]->x() - dataPoints[0]->x());
                           yDataTemp.push_back(log(dataPoints[i]->y())); // we use log space for interpolation
                           if (flag == 0)
                           {
                             I = i; // store the index of the first data point that is not zero
                           }
                           flag++;
                         }
                         i++;
                       }
                       // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
                       Matrix B = linreg(xDataTemp, yDataTemp, 2);
                       // check if B(2, 0) is positive -> the regression is not valid and has to be repeated with p = 1
                       if (B(2, 0) > 0)
                       {
                         B = linreg(xDataTemp, yDataTemp, 1);
                       }
                       // calculate the interpolated y-axis values
                       i = 0;
                       while (dataPoints[i]->y() == 0.0)
                       {
                         dataPoints[i]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) * (dataPoints[i]->x() - dataPoints[0]->x())));
                         dataPoints[i]->df = 0;

                         // check if the interpolated value is larger than 3 * MAX
                         if (dataPoints[i]->y() > 3 * MAX)
                         {
                           dataPoints[i]->setY(3 * MAX);
                         }
                         i++;
                       }

                       /* Handle the end of the data. Hereby, the data always ends with n zeros. Therefore we need to find the last four data points that are not zero.*/
                       int j = dataPoints.size() - 1;
                       int J = 0;
                       flag = 0;
                       xDataTemp.clear();
                       yDataTemp.clear();
                       while (flag < 4)
                       {
                         if (j < 0)
                         {
                           std::cerr << "Error: The data set is too small to interpolate." << std::endl;
                           break;
                         }
                         if (dataPoints[j]->y() > 0.0)
                         {
                           xDataTemp.push_back(dataPoints[j]->x() - dataPoints[0]->x());
                           yDataTemp.push_back(log(dataPoints[j]->y())); // we use log space for interpolation
                           if (flag == 0)
                           {
                             J = j; // store the index of the last data point that is not zero
                           }
                           flag++;
                         }
                         j--;
                       }
                       // calculate the coefficients b0, b1, and b2 for the quadratic interpolation
                       B = linreg(xDataTemp, yDataTemp, 2);
                       // check if B(2, 0) is positive -> the regression is not valid and has to be repeated with p = 1
                       if (B(2, 0) > 0)
                       {
                         B = linreg(xDataTemp, yDataTemp, 1);
                       }
                       // calculate the interpolated y-axis values
                       j = dataPoints.size() - 1;
                       while (dataPoints[j]->y() == 0.0)
                       {
                         dataPoints[j]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[j]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[j]->x() - dataPoints[0]->x()) * (dataPoints[j]->x() - dataPoints[0]->x())));
                         dataPoints[j]->df = 0;

                         // check if the interpolated value is larger than 3 * MAX
                         if (dataPoints[j]->y() > 3 * MAX)
                         {
                           dataPoints[j]->setY(3 * MAX);
                         }
                         j--;
                       }

                       /* Handle the middle of the data. Hereby, we use a quadratic interpolation to fill the gaps between the data points. */
                       // iterate over the data points from I to J
                       for (int i = I; i <= J; i++)
                       {
                         if (dataPoints[i]->y() == 0.0)
                         {
                           // now, we asume the last value before the zero appears at index "i-1" and we need to find the next data point "j" that is not zero to define the range for the interpolation
                           int j = i + 1;
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
                          //  // check if when p = 2 and B(2, 0) is positive -> the regression is not valid and has to be repeated with p = 1
                          //  if (B(2, 0) > 0)
                          //  {
                          //    B = linreg(xDataTemp, yDataTemp, 1);
                          //  }
                           // calculate the interpolated y-axis values
                           for (int n = i; n < j; n++)
                           {
                             dataPoints[n]->setY(exp(B(0, 0) + B(1, 0) * (dataPoints[n]->x() - dataPoints[0]->x()) + B(2, 0) * (dataPoints[n]->x() - dataPoints[0]->x()) * (dataPoints[n]->x() - dataPoints[0]->x())));
                             dataPoints[n]->df = 0;
                             // check if the interpolated value is nan
                             if (std::isnan(dataPoints[n]->y()))
                             {
                               std::cout << "NAN in the middle of the data: ";
                               // print xDataTemp and yDataTemp
                               for (int k = 0; k < xDataTemp.size(); k++)
                               {
                                 std::cout << xDataTemp[k] << " ";
                               }
                               std::cout << " || ";
                               for (int k = 0; k < yDataTemp.size(); k++)
                               {
                                 std::cout << yDataTemp[k] << " ";
                               }
                               std::cout << std::endl;
                             }
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
                 } // end of if statement
               },
               dataMap); // end of visit
  }                      // end of interpolateData

} // namespace q