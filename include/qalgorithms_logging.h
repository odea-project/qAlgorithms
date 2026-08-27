#ifndef QALGORITHMS_LOGGING_H
#define QALGORITHMS_LOGGING_H

#include "qalgorithms_datatypes.h"
#include <cstdint>
#include <vector>

namespace qAlgorithms
{
    // despite the name, this function records all data needed to fully replicate a failed run of
    // qpeaks including the relevant
    void log_qpeaks(const float *intensities,
                    const float *x_axis,
                    const float *intensities_log,
                    const uint16_t *const df,
                    const size_t length,
                    const size_t maxscale,
                    const std::vector<RegressionGauss> *result);

} // namespace qAlgorithms
#endif