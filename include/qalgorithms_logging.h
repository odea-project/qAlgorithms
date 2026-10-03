#ifndef QALGORITHMS_LOGGING_H
#define QALGORITHMS_LOGGING_H

#include "qalgorithms_datatypes.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace qAlgorithms
{
    // this modifies a local static variable to switch off the logging separately
    // from the user option that just omits errors and fails silently. It is intended
    // only for use with the replay feature during debugging.
    void setReplay(bool on);

#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
    struct QPeaks_log_mapping
    {
        size_t length = 0;
        size_t maxscale = 0;
        size_t resultSize = 0;
        size_t intensities_offset = 0;
        float *get_intensities_ptr(void)
        {
            return (float *)(internal_arrays.data() + intensities_offset);
        }
        size_t x_axis_offset = 0;
        float *get_x_axis_ptr(void)
        {
            return (float *)(internal_arrays.data() + x_axis_offset);
        }
        size_t intensities_log_offset = 0;
        float *get_intensities_log_ptr(void)
        {
            return (float *)(internal_arrays.data() + intensities_log_offset);
        }
        size_t df_offset = 0;
        uint16_t *get_df_ptr(void)
        {
            if (df_offset == 0)
                return nullptr;
            return (uint16_t *)(internal_arrays.data() + df_offset);
        }
        size_t returns_offset = 0;
        RegressionGauss *get_returns_ptr(void)
        {
            if (resultSize == 0)
                return nullptr;
            return (RegressionGauss *)(internal_arrays.data() + returns_offset);
        }

        // changes to this vector will invalidate one or all pointes returned from the above functions
        std::vector<char> internal_arrays;
    };
#pragma clang diagnostic pop

    // despite the name, this function records all data needed to fully replicate a failed run of
    // qpeaks including the relevant
    bool log_qpeaks(const float *intensities,
                    const float *x_axis,
                    const float *intensities_log,
                    const uint16_t *const df,
                    const size_t length,
                    const size_t maxscale,
                    const std::vector<RegressionGauss> *result);

    QPeaks_log_mapping read_log_qpeaks(const char *compressed_data, const size_t in_length);
} // namespace qAlgorithms
#endif