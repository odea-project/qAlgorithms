#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatypes.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#include <vector>
#include <array>

namespace qAlgorithms
{
    std::array<float, 3> interpolateQadratic(float interpolate, const float *x, const float *y);

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
     * @param dataPoints : {x, y, df, DQSC, DQSB, scanNumber}
     * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
     */
    treatedData pretreatData(std::vector<dataPoint> &dataPoints,
                             std::vector<unsigned int> &binIdx,
                             float expectedDifference,
                             const bool updateExpectedDifference = true);

    treatedData pretreatDataCentroids(std::vector<dataPoint> &dataPoints, float expectedDifference);

    treatedData pretreatDataFeatures(std::vector<dataPoint> &dataPoints,
                                     std::vector<unsigned int> &binIdx,
                                     float expectedDifference,
                                     const bool updateExpectedDifference = true);

    float calcRTDiff(std::vector<double> &retention_times);

    std::vector<dataPoint> mzmlToDataPoint(sc::MZML &data, const int index);

    std::vector<dataPoint> qbinToDataPoint(EIC &eic);

    /**
     * @brief Read 3D tensor data from a StreamCraft mzML object
     * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores
     * it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file").
     *
     * @param data : StreamCraft mzML object
     * @param ms1only : boolean to indicate if only MS1 spectra should be read
     * @param start_index : index of the first spectrum to be read
     * @return std::vector<std::vector<DataType::Peak>> : list of peaks
     */
    std::vector<std::vector<CentroidPeak>> findCentroids_MZML(
        sc::MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool ms1only = true,
        const std::string polarity = "positive",
        const int start_index = 0);

    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        float rt_diff);
}

#endif