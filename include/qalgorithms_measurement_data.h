#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatypes.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#include <vector>
#include <array>

namespace qAlgorithms
{
    inline std::array<double, 3> interpolateQuadratic(float interpolate, const float *x, const float *y);

    std::vector<std::vector<CentroidPeak>> transferCentroids( // @todo merge with findPeaks_mzml
        StreamCraft::MZML &data,
        std::vector<int> &indices,
        std::vector<double> &retention_times,
        const int start_index,
        double PPMerror);

    double calcExpectedDiff(const std::vector<std::vector<double>> *spectrum);

    /**
     * @brief Inter/extrapolate gaps in data and define separation markers for data blocks.
     * @param dataPoints : {x, y, df, DQSC, DQSB, scanNumber}
     * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
     */
    treatedData pretreatEIC(EIC &dataPoints,
                            std::vector<unsigned int> &binIdx,
                            float expectedDifference);

    std::vector<ProfileBlock> pretreatDataCentroids(const std::vector<std::vector<double>> *spectrum, float expectedDifference);

    void extrapolateEIC(const std::vector<size_t> scanNums, std::vector<float> *intensity);

    inline float calcRTDiff(const std::vector<double> *retention_times);

    std::vector<CentroidPeak> findCentroids_MZML(
        StreamCraft::MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool polarity,
        const bool ms1only = true);

    const std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        const std::vector<CentroidPeak> *centroids,
        float rt_diff);
}

#endif