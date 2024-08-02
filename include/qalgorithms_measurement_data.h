#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

// internal
#include "qalgorithms_datatype_mass_spectrum.h"
#include "qalgorithms_datatype_peak.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.h"

#include <vector>
#include <memory>

// up to date with commit 47da7e1

namespace q
{
    namespace MeasurementData
    {
        using MS = std::vector<std::unique_ptr<DataType::MassSpectrum>> *; // pointer to a vector of mass spectra @todo rename so the pointer is obvious
        using varDataType = std::variant<MS>;                              // add more data types if needed
        /**
         * @brief A class to store measurement data
         * @details The MeasurementData class is a virtual class used to store measurement data.
         * Based on the type of measurement data, the MeasurementData class can be
         * subclassed to store different types of measurement data.
         */
        class MeasurementData
        {
        protected:
            // variables
            struct dataPoint
            {
                float x;
                float y;
                bool df;
                float dqsCentroid;
                float dqsBinning;
                int scanNumber;
            };

        public:
            // variables
            struct treatedData
            {
                std::vector<dataPoint> dataPoints;
                std::vector<std::vector<dataPoint>::iterator> separators;
            };
            // destructor
            // virtual ~MeasurementData() {};

            std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
            transfereCentroids(
                sc::MZML &data,
                std::vector<int> &indices,
                std::vector<double> &retention_times,
                const int start_index);

            // methods
            // virtual bool readCSV(std::string filename, int rowStart, int rowEnd,
            //                      int colStart, int colEnd, char separator,
            //                      std::vector<DataType::DataField> variableTypes) = 0;

            /**
             * @brief Identify and fill gaps in the data
             * @details The zeroFilling method identifies and fills gaps in the data. The method uses difference between
             * two neighboring data points to identify gaps. If the difference is 1.75 times greater than
             * expected, then the method fills the gap with zero values for y-axis and inter/extrapolated
             * values for x-axis values. For the expected difference, the method the difference of the
             * last two data points that not show a gap. However, the first expected difference is set
             * to the median of the differences of the total data points. However, the maximum gap
             * size is set to "k/2" per side, i.e., "k" in total, where there is a gap leftover
             * between the fourth and fifth data points.
             * @param dataVec A vector of variant data types
             * @param k The maximum gap size
             */
            void zeroFilling(varDataType &dataVec, int k);

            int zeroFilling_blocksAndGaps(
                std::vector<std::vector<double>> &data,
                double expectedDifference,
                const bool updateExpectedDifference = true);

            int zeroFilling_blocksOnly(
                std::vector<std::vector<double>> &data,
                double expectedDifference,
                const bool updateExpectedDifference = true);

            double calcExpectedDiff(std::vector<double> &data);

            void isZeroFillingNeeded( // @todo why does this not just return a bool?
                std::vector<double> &data,
                bool &needsZeroFilling);

            /**
             * @brief Cut the data into smaller data sets
             * @details The cutData method cuts the data into smaller data sets. The method uses the separator value to split the data into smaller data sets. The separator value is set to -1.0 for the x-axis and -1.0 for the y-axis. For each cut, the method creates a new data subset and stores it in the data vector using a sub-dataset ID as the secondary key.
             * @param dataVec A variant data type
             * @param maxKey The maximum key value in the current data vector. This value is used to create a new key for the new data subset.
             */
            void cutData(varDataType &dataVec, size_t &maxKey);

            // @todo documentation
            void cutData_vec_orbitrap(
                std::vector<std::vector<double>> &data,
                double expectedDifference,
                std::vector<std::vector<double>::iterator> &separators);

            /**
             * @brief Filter small data sets
             * @details The filterSmallDataSets method filters small data sets. The method
             * removes data sets with less than 5 data points. This is due to the regression
             * analysis that includes 4 coefficients and therefore requires at least 5 data points.
             * @param dataVec A variant data type
             */
            void filterSmallDataSets(varDataType &dataVec);

            /**
             * @brief Interpolate y-axis values
             * @details The interpolateData method interpolates the y-axis values.
             * The method uses quadratic interpolation to interpolate the y-axis
             * values in the log space.
             * @param dataVec A variant data type
             */
            void interpolateData(varDataType &dataVec);

            /**
             * @brief Gaussian extrapolate missing y-axis values
             * @details data[1] will look like this:
             * 0 0 0 0 y1 y2 y3...yi 0 0 0 0 0 0 0 0 yj yk yl...yn 0 0 0 0 0 0 0 0... etc.
             * The idea is to extrapolate the zeros using parabola extrapolation of the blocks of non-zero values.
             * For parabola, first, last and maximum values are considered.
             * We use the log data for the extrapolation and back-transform the extrapolated values to linear space.
             * This will lead to gaussian extrapolation of the zeros.
             * @param data is structured as follows:
             * data[0] = x-axis values; data[1] = y-axis values; data[2] = df values
             */
            void extrapolateData_vec(
                std::vector<std::vector<double>> &data, // @todo make this a struct with named vars
                std::vector<std::vector<double>::iterator> &separators);

            // @todo documentation
            void interpolateData_vec_orbitrap(
                std::vector<std::vector<double>> &data,
                std::vector<std::vector<double>::iterator> &separators);

            void extrapolateData_vec_orbitrap(
                std::vector<std::vector<double>> &data,
                std::vector<std::vector<double>::iterator> &separators);

            /**
             * @brief Inter/extrapolate gaps in data and define separation markers for data blocks.
             *
             * @param dataPoints : {x, y, df, dqsCentroid, dqsBinning, scanNumber}
             *
             * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
             */
            treatedData
            pretreatData(std::vector<dataPoint> &dataPoints,
                         float expectedDifference);
        };
    } // namespace MeasurmentData
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H