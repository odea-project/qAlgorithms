#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

// internal
#include "qalgorithms_datatype_mass_spectrum.h"
#include "qalgorithms_matrix.h"
#include "qalgorithms_utils.h"

// external
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <fstream> // for debugging
#include <iomanip>

namespace q
{
    namespace MeasurementData
    {
        using MS = std::vector<std::unique_ptr<DataType::MassSpectrum>> *; // pointer to a vector of mass spectra
        using varDataType = std::variant<MS>;                              // add more data types if needed
        /**
         * @brief A class to store measurement data
         * @details The MeasurementData class is a virtual class used to store measurement data. Based on the type of measurement data, the MeasurementData class can be subclassed to store different types of measurement data.
         */
        class MeasurementData
        {
        public:
            // destructor
            virtual ~MeasurementData(){};

            // methods
            virtual void readCSV(std::string filename, int rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataType::DataField> variableTypes) = 0;

            /**
             * @brief Identify and fill gaps in the data
             * @details The zeroFilling method identifies and fills gaps in the data. The method uses difference between two neighboring data points to identify gaps. If the difference is 1.75 times greater than expected, then the method fills the gap with zero values for y-axis and inter/extrapolated values for x-axis values. For the expected difference, the method the difference of the last two data points that not show a gap. However, the first expected difference is set to the median of the differences of the total data points. However, the maximum gap size is set to "k/2" per side, i.e., "k" in total, where there is a gap leftover between the fourth and fifth data points.
             * @param dataVec A vector of variant data types
             * @param k The maximum gap size
             */
            void
            zeroFilling(varDataType &dataVec, int k);

            int 
            zeroFilling_vec(
                std::vector<std::vector<double>> &data,
                double expectedDifference);

            double
            calcExpectedDiff(std::vector<double> &data);

            void
            isZeroFillingNeeded(
                std::vector<double> &data,
                bool &needsZeroFilling);

            /**
             * @brief Cut the data into smaller data sets
             * @details The cutData method cuts the data into smaller data sets. The method uses the separator value to split the data into smaller data sets. The separator value is set to -1.0 for the x-axis and -1.0 for the y-axis. For each cut, the method creates a new data subset and stores it in the data vector using a sub-dataset ID as the secondary key.
             * @param dataVec A variant data type
             * @param maxKey The maximum key value in the current data vector. This value is used to create a new key for the new data subset.
             */
            void
            cutData(
                varDataType &dataVec,
                size_t &maxKey);

            /**
             * @brief Filter small data sets
             * @details The filterSmallDataSets method filters small data sets. The method removes data sets with less than 5 data points. This is due to the regression analysis that includes 4 coefficients and therefore requires at least 5 data points.
             * @param dataVec A variant data type
             */
            void filterSmallDataSets(varDataType &dataVec);

            /**
             * @brief Interpolate y-axis values
             * @details The interpolateData method interpolates the y-axis values. The method uses quadratic interpolation to interpolate the y-axis values in the log space.
             * @param dataVec A variant data type
             */
            void interpolateData(varDataType &dataVec);

            /**
             * @brief Gaussian extrapolate y-axis values
             * while data[1] will look like: 0 0 0 0 y1 y2 y3...yi 0 0 0 0 0 0 0 0 yj yk yl...yn 0 0 0 0 0 0 0 0... and so on. 
             * The idea is to extrapolate the zeros using parabola extrapolation of the blocks of non-zero values. 
             * For parabola, first, last and maximum values are considered. 
             * We use the log data for the extrapolation and back-transform the extrapolated values to linear space. 
             * This will lead to gaussian extrapolation of the zeros.
             * @param data is structured as follows:
             * data[0] = x-axis values; data[1] = y-axis values; data[2] = df values
             */
            void 
            extrapolateData_vec(
                std::vector<std::vector<double>> &data,
                std::vector<double>::iterator *separators);

            int
            intra_extrapolateData_vec(
                std::vector<std::vector<double>> &data,
                std::vector<std::vector<double>::iterator> &separators);

            // debugging
            // virtual void print() = 0;
        };
    } // namespace MeasurmentData
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H
