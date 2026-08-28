#ifndef QALGORITHMS_LOGGING_H
#define QALGORITHMS_LOGGING_H

#include "qalgorithms_datatypes.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace qAlgorithms
{
    struct QPeaks_log_mapping
    {
        size_t length;
        size_t maxscale;
        size_t resultSize;
        float *intensities = nullptr;
        float *x_axis = nullptr;
        float *intensities_log = nullptr;
        uint16_t *df = nullptr;
        RegressionGauss *result = nullptr;
        std::vector<char> internal_arrays;
    };

    // despite the name, this function records all data needed to fully replicate a failed run of
    // qpeaks including the relevant
    void log_qpeaks(const float *intensities,
                    const float *x_axis,
                    const float *intensities_log,
                    const uint16_t *const df,
                    const size_t length,
                    const size_t maxscale,
                    const std::vector<RegressionGauss> *result);

    QPeaks_log_mapping read_log_qpeaks(const std::vector<char> *internal_arrays);
} // namespace qAlgorithms
#endif