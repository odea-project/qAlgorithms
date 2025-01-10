#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatypes.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#include <vector>

namespace qAlgorithms
{
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

    std::vector<std::vector<CentroidPeak>> transferCentroids( // @todo merge with findPeaks_mzml
        sc::MZML &data,
        std::vector<int> &indices,
        std::vector<double> &retention_times,
        const int start_index,
        double PPMerror);

    double
    calcExpectedDiff(std::vector<double> &data);

    /**
     * @brief Inter/extrapolate gaps in data and define separation markers for data blocks.
     * @param dataPoints : {x, y, df, dqsCentroid, dqsBinning, scanNumber}
     * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
     */
    treatedData pretreatData(std::vector<dataPoint> &dataPoints,
                             std::vector<unsigned int> &binIdx,
                             float expectedDifference,
                             const bool updateExpectedDifference = true);

    /**
     * @brief A class to store 3D tensor measurement data, e.g., LC-MS data
     * @details The TensorData class is a subclass of the MeasurementData class used to store 3D tensor measurement data, e.g., LC-MS data.
     *
     * @param data A 3D tensor of LC-MS data /ref DataType::LC_MS
     */

    float calcRTDiff(std::vector<double> &retention_times);

    std::vector<dataPoint> mzmlToDataPoint(sc::MZML &data, const int index);

    std::vector<dataPoint> qbinToDataPoint(EIC &eic);

    // methods
    /**
     * @brief Read 3D tensor data from a StreamCraft mzML object
     * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file").
     *
     * @param data : StreamCraft mzML object
     * @param ms1only : boolean to indicate if only MS1 spectra should be read
     * @param start_index : index of the first spectrum to be read
     * @return std::vector<std::vector<DataType::Peak>> : list of peaks
     */
    std::vector<std::vector<CentroidPeak>> findCentroids_MZML(
        sc::MZML &data,
        std::vector<unsigned int> &addEmpty,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool ms1only = true,
        const std::string polarity = "positive",
        const int start_index = 0);

    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        float rt_diff);
}

#endif // QALGORITHMS_MEASUREMENT_DATA_H