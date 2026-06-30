#pragma GCC diagnostic ignored "-Wfloat-conversion"

#include "common_test_utils.hpp"
#include "qalgorithms_input_output.h"
#include "qalgorithms_read_file.h"

#include <cfloat>
#include <cstddef>
#include <vector>

using namespace qAlgorithms;

void test_base64_decode()
{
    const double arr_D[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                              0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};
    const float arr_F[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                             0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};
    // string encoded using R (binary)
    const char encoded[] = "jiZEAKNgwT+WGDInAAsGQGOGDPv0h/s/KLQiljFalj/72H+cKHH6P7SXNGkTauQ//qRKReO8oz/uG/9JXpvRP2MAwvHVYPs/ugWHusGFwz8=";

    std::vector<char> decoded_string = decode_base64(encoded);
    std::vector<float> result;
    bytesToFloatVec(&decoded_string, true, &result);
    assert(result.size() == 10);
    for (size_t i = 0; i < 10; i++)
    {
        assert(flt_equal(arr_F[i], result[i], FLT_EPSILON));
    }
}

int main()
{
    test_base64_decode();
    return 0;
}