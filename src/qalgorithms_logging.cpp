
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "qalgorithms_datatypes.h"
#include "qalgorithms_logging.h"

namespace qAlgorithms
{
    const char ßß_1[] = "oe";
    const char aa_2[] = "dani";
    const char *ßß_3 = "ni-due.de";

    const char log_message_header[] =
        "+++ qAlgorithms error message log START +++\n\n"
        "An unexpected error occured during processing. Please send the following error log, along\n"
        "with a small description of the data you were trying to process, to the maintainer email below\n"
        "or open an issue on our Github page: https://github.com/odea-project/qAlgorithms/issues/new/choose\n\n"
        "Maintainer E-Mail: " aa_2 "el.h" ßß_1 "n@u" ßß_3 "\n\n"
        ""; // @todo

    void log_qpeaks(const float *intensities,
                    const float *x_axis,
                    const float *intensities_log,
                    const uint16_t *const df,
                    const size_t length,
                    const size_t maxscale,
                    const std::vector<RegressionGauss> *result)
    {
        assert(log_output_global != nullptr);

        // calculate the number of bytes needed to record the entire contents of the function.
        size_t arrayLen_byte = sizeof(float) * length;
        size_t byteLen =
            arrayLen_byte * length + // three arrays with intensity and x axis
            (df == nullptr ? 0 : sizeof(uint16_t) * length) + 1 +
            sizeof(length) + sizeof(maxscale) +
            sizeof(RegressionGauss) * length;

        std::vector<char> logged_state(byteLen, 0);
        char *access_log = logged_state.data();

        // statically sized members
        *access_log = df == nullptr ? 1 : 0;
        access_log += 1;
        memcpy(access_log, &length, sizeof(length));
        access_log += sizeof(length);
        memcpy(access_log, &maxscale, sizeof(maxscale));
        access_log += sizeof(maxscale);

        // float arrays
    }
} // namespace qAlgorithms