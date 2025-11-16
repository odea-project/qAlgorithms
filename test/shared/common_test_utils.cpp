#include "CDFlib/cdflib.hpp"
#include "pugixml-1.14/src/pugixml.h"
#include "common_test_utils.hpp"

#include <cmath>
#include <stdint.h> // printing

float ran0(long *idum)
{
    // function taken from numerical recepies, second edition, page 279
#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

    long k;
    float ans;
    *idum ^= MASK;
    k = (*idum) / IQ;
    *idum = IA * (*idum - k * IQ) - IR * k;
    if (*idum < 0)
        *idum += IM;
    ans = AM * (*idum);
    *idum ^= MASK;
    return ans;
}

// produce a random double value in the range (lower, upper). If no seed is supplied, the seed is also random
double randRange_d(double lower, double upper, long seed)
{
    verify(lower < upper);
    long randint = seed;
    if (seed == 0)
        randint = nanoseconds();
    double randDouble = ran0(&randint); // random number between 0 and 1
    double span = upper - lower;

    double res = lower + span * randDouble;
    verify(res > lower);
    verify(res < upper);
    return res;
}

// round a number x to a given number of digits
double roundTo_d(double x, size_t digits)
{
    if (digits == 0)
        return 0;
    // this is a naive implementation and overall not robust enough to use uncritically with very large numbers
    const size_t maxPrec = 17;
    verify(digits < maxPrec);

    static const double powers[maxPrec]{1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
                                        1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16};

    double pow = powers[digits];

    verify(double(INT64_MAX) / pow > x); // prevent overflow
    verify(double(INT64_MIN) / pow < x); // prevent underflow

    int sign = std::signbit(x) ? -1 : 1;

    int rounded = int(x * pow + 0.5 * sign); // @todo this does not incorporate the even-odd switch for a remainder of 1/2, but does that matter here?

    return (double(rounded) / pow);
}

void printVec_f(const std::vector<float> *vec, const char *vecName)
{
    verify(!vec->empty());
    printf("Vector %s:  %f", vecName, (*vec)[0]);
    for (size_t i = 1; i < vec->size(); i++)
    {
        printf(", %f", (*vec)[i]);
    }
    printf("  | length: %zu\n", vec->size());
}
