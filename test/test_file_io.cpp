#pragma GCC diagnostic ignored "-Wfloat-conversion"

#include "qalgorithms_input_output.h"
#include "qalgorithms_read_file.h"

#include <cfloat>
#include <cstddef>
#include <cstring>
#include <vector>

#include "common_test_utils.hpp"

using namespace qAlgorithms;

static void test_base64_decode()
{
    const float arr_F[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                             0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};
    // reference string encoded using python script (see end of file @todo)
    const char encoded[] = "jiZEAKNgwT+WGDInAAsGQGOGDPv0h/s/KLQiljFalj/72H+cKHH6P7SXNGkTauQ//qRKReO8oz/uG/9JXpvRP2MAwvHVYPs/ugWHusGFwz8=";

    std::vector<char> decoded_string = decode_base64(encoded);
    std::vector<float> result;
    bytesToFloatVec(&decoded_string, true, &result);
    assert(result.size() == 10, "Incorrect number of Elements decoded\n");
    for (size_t i = 0; i < 10; i++)
    {
        assert(flt_equal(arr_F[i], result[i], FLT_EPSILON),
               "Element %d was decoded incorrectly (difference: %f)",
               i, abs(arr_F[i] - result[i]));
    }

    // also check if encoding works
    std::string encode = encode_base64_flt(arr_F, 10);
    assert(std::strcmp(encode.c_str(), encoded) == 0, "Encoding of array (float) failed\n");
}

int main()
{
    test_base64_decode();
    return 0;
}