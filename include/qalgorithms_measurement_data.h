#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h"

#include <vector>
#include <array>

namespace qAlgorithms
{
    struct RT_Converter
    {
        // find the spectrum index after interpolation from MS1 spectrum number and vice versa
        std::vector<unsigned int> countToInterp;
        std::vector<unsigned int> interpToCount;
    };

    RT_Converter interpolateScanNumbers(const std::vector<float> *retentionTimes);

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

    inline float calcRTDiff(const std::vector<double> *retention_times);

    std::vector<CentroidPeak> findCentroids(
        XML_File &data,
        const std::vector<pugi::xml_node> *spectra_nodes_ex,
        const bool polarity,
        const bool ms1only = true);

    std::vector<FeaturePeak> findFeatures(std::vector<EIC> &data,
                                          std::vector<unsigned int> *backConvert,
                                          std::vector<float> *RT);
}

#endif