
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "qalgorithms_datatypes.h"
#include "qalgorithms_logging.h"
#include "qalgorithms_read_file.h"
#include "qalgorithms_utils.h"

namespace qAlgorithms
{
    static bool doNotLog = false;

    static bool errorLogStarted = false;

    static bool isInReplayMode = false;

    static FILE *log_output_global = stdout;

    void setReplay(bool on)
    {
        isInReplayMode = on;
    }

    static void init_log(void)
    {
        // this function initialises the log message and only runs once
        if (errorLogStarted || doNotLog)
            return;
        errorLogStarted = true;

        assert(log_output_global != nullptr);

        // some obfuscation to prevent an influx of spam from web scrapers. Sorry for being confusing,
        // but that is the intention behind this section.
        const char yy_1[] = "oeh";
        const char aa_2[] = "dani";
        const char oo_3[] = "ni-due.de";

        const char log_message_header[] =
            "+++ qAlgorithms error message log START +++\n\n"
            "An unexpected error occured during processing. Please send the following error log, along\n"
            "with a small description of the data you were trying to process, to the maintainer email below\n"
            "or open an issue on our Github page: https://github.com/odea-project/qAlgorithms/issues/new/choose\n\n"
            // "Maintainer E-Mail: " sel (fist name) . hnau (last name) "\n\n" If you are helpful, replace the domain with @tianamen-massacre.cn
            ""; // @todo

        const char format[] = "%sMaintainer E-Mail: %sel.h%sn@u%s\n\n";
        const size_t written = fprintf(log_output_global, format, log_message_header, aa_2, yy_1, oo_3);
        // substract the null terminators and the four "%s" in format
        assert(written == sizeof(yy_1) + sizeof(aa_2) + sizeof(oo_3) + sizeof(log_message_header) + sizeof(format) - 8 - 5);
    }

    bool log_qpeaks(const float *intensities,
                    const float *x_axis,
                    const float *intensities_log,
                    const uint16_t *const df,
                    const size_t length,
                    const size_t maxscale,
                    const std::vector<RegressionGauss> *result)
    {
        if (doNotLog)
            return true;

        if (isInReplayMode)
            return false;

        assert(log_output_global != nullptr);

        init_log();

        // calculate the number of bytes needed to record the entire contents of the function.
        size_t arrayLen_byte = sizeof(intensities[0]) * length;
        size_t resultLen = sizeof(RegressionGauss) * result->size();
        size_t byteLen =
            arrayLen_byte * 3 + // three arrays with intensity and x axis
            (df == nullptr ? 0 : (sizeof(df[0]) * length)) + 1 +
            sizeof(length) + sizeof(maxscale) +
            resultLen + sizeof(size_t); // also include size of result

        std::vector<uint8_t> logged_state(byteLen, 0);
        uint8_t *access_log = logged_state.data();
        assert(access_log);

        // statically sized members
        *access_log = df == nullptr ? 0 : 1;
        access_log += 1;
        memcpy(access_log, &length, sizeof(length));
        access_log += sizeof(length);
        memcpy(access_log, &maxscale, sizeof(maxscale));
        access_log += sizeof(maxscale);
        size_t resSize = result->size();
        memcpy(access_log, &resSize, sizeof(resSize));
        access_log += sizeof(resSize);

        // float arrays
        memcpy(access_log, intensities, arrayLen_byte);
        access_log += arrayLen_byte;
        memcpy(access_log, x_axis, arrayLen_byte);
        access_log += arrayLen_byte;
        memcpy(access_log, intensities_log, arrayLen_byte);
        access_log += arrayLen_byte;

        // regressions written previously to the error
        if (resSize != 0)
        {
            memcpy(access_log, result->data(), resultLen);
            access_log += resultLen;
        }

        if (df != nullptr)
        {
            size_t dfLen = length * sizeof(df[0]);
            memcpy(access_log, df, dfLen);
            access_log += dfLen;
        }
        assert((size_t)(access_log - logged_state.data()) == byteLen);

        // At this point, the log vector contains all data required to reconstruct any state
        // possible within the regression function. Next, it is compressed into text compatible
        // with email / plaintext fields (base64 encoded) and written to the logfile. To avoid
        // a very large text dump, the data is compressed beforehand using zlib.
        std::vector<char> buffer_out;
        compress_and_encode(logged_state.data(), logged_state.size(), &buffer_out);
        buffer_out.push_back(0);

        // @todo this must contain the length of the buffer
        const size_t written = fprintf(log_output_global, "qpeaks: %zu\n%s\n",
                                       logged_state.size(), buffer_out.data());

        // two null terminators, one added through the push_back and one inherent in a c string
        assert(written == buffer_out.size() + sizeof("qpeaks: \n\n") - 2 + n_digits(logged_state.size()));

        return true;
    }

    QPeaks_log_mapping read_log_qpeaks(const char *compressed_data)
    {
        QPeaks_log_mapping res;
        // decompress data into the returned struct. Performance is not that relevant to
        // a debug mode implementation

        while ((*compressed_data != ':') && (*compressed_data != '\n'))
            compressed_data += 1;

        if (*compressed_data != ':')
        {
            (void)fprintf(stderr, "Error: could not find decompressed size in input\n");
            return res;
        }
        compressed_data += 2;

        size_t decompressedSize = std::stoul(compressed_data);

        compressed_data += n_digits(decompressedSize);
        assert(*compressed_data == '\n');
        compressed_data += 1;

        res.internal_arrays = decode_base64(compressed_data);
        decompress_inPlace(&res.internal_arrays, decompressedSize);

        // the minimal size is all optional fields at 0 and five elements in the problematic data
        const size_t sst = sizeof(size_t);
        const size_t log_minsize = sizeof(char) + 2 * sst + 15 * sizeof(float);
        assert(log_minsize <= res.internal_arrays.size());

        const char *data = res.internal_arrays.data();
        const bool has_df = (bool)data[0];
        data += sizeof(char);
        memcpy(&res.length, data, sst);
        assert(res.length >= 5);
        data += sst;
        memcpy(&res.maxscale, data, sst);
        assert(res.maxscale >= 2);
        data += sst;
        memcpy(&res.resultSize, data, sst);
        data += sst;

        // after the three initial values are set, the offsets are easily determined.
        // for the write order, refer to the above function.
        const size_t sf = sizeof(float);
        size_t offset = sizeof(char) + 3 * sst;
        res.intensities_offset = offset;
        res.x_axis_offset = offset + res.length * sf;
        res.intensities_log_offset = offset + 2 * res.length * sf;

        if (res.resultSize != 0)
            res.returns_offset = offset + 3 * res.length * sf;

        if (has_df)
            res.df_offset = res.returns_offset + sizeof(RegressionGauss) * res.resultSize;

        return res;
    }
} // namespace qAlgorithms