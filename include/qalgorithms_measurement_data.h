#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "../include/qalgorithms_datatypes.h"
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

#include <vector>

namespace qAlgorithms
{
    struct dataPoint // @todo remove this (again)
    {
        float x;
        float y;
        bool df;
        float dqsCentroid;
        float dqsBinning;
        int scanNumber;
        float mz;
    };

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
     *
     * @param dataPoints : {x, y, df, dqsCentroid, dqsBinning, scanNumber}
     *
     * @return std::vector<std::vector<dataPoint>::iterator> : separation markers for data blocks
     */
    treatedData pretreatData(std::vector<dataPoint> &dataPoints,
                             std::vector<unsigned int> &binIdx,
                             float expectedDifference,
                             const bool updateExpectedDifference = true);
}

#endif // QALGORITHMS_MEASUREMENT_DATA_H