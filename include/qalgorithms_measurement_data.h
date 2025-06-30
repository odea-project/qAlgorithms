#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h"

#include <vector>
#include <array>

namespace qAlgorithms
{
    inline std::array<double, 3> interpolateQuadratic(float interpolate, const float *x, const float *y);

    std::vector<std::vector<CentroidPeak>> transferCentroids(
        MZML &data,
        std::vector<int> &indices,
        std::vector<double> &retention_times,
        const int start_index,
        double PPMerror);

    /**
     * @brief Inter/extrapolate gaps in data and define separation markers for data blocks.
     * @param dataPoints : {x, y, df, DQSC, DQSB, scanNumber}
     * @return std::vector<std::vector<DataPoint_deprecated>::iterator> : separation markers for data blocks
     */
    TreatedData pretreatEIC(EIC &dataPoints,
                            // std::vector<unsigned int> &binIdx,
                            float expectedDifference,
                            size_t maxScan);

    struct Block
    {
        size_t start;
        size_t end;
    };

    void binProfileSpec(std::vector<Block> *result,
                        const std::vector<double> *diffs,
                        // const std::vector<unsigned int> *diffOrder,
                        const std::vector<double> *cumDiffs, // indices into cumDiffs must be right-shifted by one!
                        // size_t previousDiffPos,              // skip this many points in the diffOrder vector
                        size_t start, size_t end);

    std::vector<ProfileBlock> pretreatDataCentroids(
        const std::vector<double> *spectrum_mz,
        const std::vector<double> *spectrum_int);

    void extrapolateEIC(const std::vector<size_t> scanNums, std::vector<float> *intensity);

    inline float calcRTDiff(const std::vector<double> *retention_times);

    std::vector<CentroidPeak> findCentroids(
        MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool polarity,
        const bool ms1only = true);

    std::vector<FeaturePeak> findPeaks_QBIN(std::vector<EIC> &data, float rt_diff, size_t maxScan);
}

#endif