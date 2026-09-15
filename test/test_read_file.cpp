#include "qalgorithms_read_file.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#pragma GCC diagnostic ignored "-Wfloat-conversion"

#include "../src/qalgorithms_read_file.cpp" // NOLINT
#include "common_test_utils.hpp"

using namespace qAlgorithms;

static void test_spec_decode_nocomp_double()
{
    const double arr_D[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                              0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};
    // reference string encoded using python script (see end of file)
    const char encoded[] = "jiZEAKNgwT+WGDInAAsGQGOGDPv0h/s/KLQiljFalj/72H+cKHH6P7SXNGkTauQ//qRKReO8oz/uG/9JXpvRP2MAwvHVYPs/ugWHusGFwz8=";
    const size_t encode_len = 108; // 109 above includes null terminator

    std::vector<float> result;
    decodeSpectrum(encoded, &result, 10, false, true);

    // @todo also test for the other three combinations that are possible, as well as errors

    assert(result.size() == 10, "Incorrect number of Elements decoded\n", NULL);
    for (size_t i = 0; i < 10; i++)
    {
        assert(flt_equal(arr_D[i], result[i], FLT_EPSILON),
               "Element %d was decoded incorrectly (difference: %f)",
               i, abs(arr_D[i] - result[i]));
    }

    // also check if encoding works
    std::vector<char> encode = encode_base64_dbl(arr_D, 10);
    assert(encode.size() == encode_len, "Incorrect length of encoding", NULL);
    for (size_t i = 0; i < encode_len; i++)
    {
        assert(encode[i] == encoded[i], "Non-Matching encoding result", NULL);
    }
}

static void test_spec_decode_iscomp_double()
{
    const double arr_D[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                              0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};

    const uint8_t *arr_char = (uint8_t *)arr_D;
    const size_t arr_char_len = 10 * sizeof(double);
    std::vector<char> intermediate;

    compress_and_encode(arr_char, arr_char_len, &intermediate);

    std::vector<float> result;
    decodeSpectrum(intermediate.data(), &result, 10, true, true);

    assert(result.size() == 10, "Incorrect number of Elements decoded\n", NULL);
    for (size_t i = 0; i < 10; i++)
    {
        assert(flt_equal(arr_D[i], result[i], FLT_EPSILON),
               "Element %d was decoded incorrectly (difference: %f)",
               i, abs(arr_D[i] - result[i]));
    }
}

static void test_spec_decode_iscomp_float()
{
    const float arr_F[10] = {0.135761618738162, 2.75537138578063, 1.72069261613833, 0.0218284366498446, 1.65262662060337,
                             0.637948708991408, 0.0385504743656586, 0.275107929479815, 1.71114153324745, 0.152519432142112};

    const uint8_t *arr_char = (uint8_t *)arr_F;
    const size_t arr_char_len = 10 * sizeof(float);
    std::vector<char> intermediate;

    compress_and_encode(arr_char, arr_char_len, &intermediate);

    std::vector<float> result;
    decodeSpectrum(intermediate.data(), &result, 10, true, false);

    assert(result.size() == 10, "Incorrect number of Elements decoded\n", NULL);
    for (size_t i = 0; i < 10; i++)
    {
        assert(flt_equal(arr_F[i], result[i], FLT_EPSILON),
               "Element %d was decoded incorrectly (difference: %f)",
               i, abs(arr_F[i] - result[i]));
    }
}

int main()
{
    test_spec_decode_nocomp_double();
    test_spec_decode_iscomp_double();
    test_spec_decode_iscomp_float();
    return 0;
}

// python code used to generate test string (LLM code):

// import base64
// import struct

// doubles = [1.5, 2.7, 3.14, 4.0, 5.999]

// # Convert doubles to bytes
// # 'd' format character represents a double (8 bytes each)
// bytes_data = struct.pack(f'{len(doubles)}d', *doubles)

// # Encode to base64
// base64_string = base64.b64encode(bytes_data).decode('utf-8')

// print(base64_string)