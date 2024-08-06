#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

// internal
// #include "../include/qalgorithms_datatype_mass_spectrum.h"
#include "../include/qalgorithms_datatype_peak.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#include <vector>
#include <memory>

namespace q
{
    namespace MeasurementData
    {
        // using MS = std::vector<std::unique_ptr<DataType::MassSpectrum>> *; // pointer to a vector of mass spectra
        // using varDataType = std::variant<MS>;                              // add more data types if needed
        /**
         * @brief A class to store measurement data
         * @details The MeasurementData class is a virtual class used to store measurement data. Based on the type of measurement data, the MeasurementData class can be subclassed to store different types of measurement data.
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
                float mz;

                dataPoint(float x, float y, bool df, float dqsCentroid, float dqsBinning, int scanNumber, float mz)
                    : x(x), y(y), df(df), dqsCentroid(dqsCentroid), dqsBinning(dqsBinning), scanNumber(scanNumber), mz(mz) {}
            };

        public:
            // variables
            struct treatedData
            {
                std::vector<dataPoint> dataPoints;
                std::vector<int> separators;

                void addDataPoint(float x, float y, bool df, float dqsCentroid, float dqsBinning, int scanNumber, float mz)
                {
                    dataPoints.emplace_back(x, y, df, dqsCentroid, dqsBinning, scanNumber, mz);
                }

                void addSeparator(int index)
                {
                    if (index < static_cast<int>(dataPoints.size()))
                    {
                        separators.push_back(index);
                    }
                }
            };
                // methods
                // virtual void readCSV(std::string filename, int rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataType::DataField> variableTypes) = 0;

                std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
                transfereCentroids(
                    sc::MZML &data,
                    std::vector<int> &indices,
                    std::vector<double> &retention_times,
                    const int start_index);

                double
                calcExpectedDiff(std::vector<double> &data);

                /**
                 * @brief Inter/extrapolate gaps in data and define separation markers for data blocks.
                 *
                 * @param dataPoints : {x, y, df, dqsCentroid, dqsBinning, scanNumber}
                 *
                 * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
                 */
                treatedData
                pretreatData(std::vector<dataPoint> &dataPoints,
                             float expectedDifference,
                             const bool updateExpectedDifference = true);
            };
        } // namespace MeasurmentData
    }
#endif // QALGORITHMS_MEASUREMENT_DATA_H
