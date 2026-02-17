#include "CDFlib/cdflib.hpp"
#include "pugixml-1.14/src/pugixml.h"
#include "common_test_utils.hpp"
#include "qalgorithms_qpeaks.h"

#include <cmath>
#include <stdint.h> // printing

using namespace qAlgorithms;

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

/*
Random numbers following a gaussian distribution
Code taken from: https://c-faq.com/lib/gaussian.html (3.)
Method sourced from: https://www.jstor.org/stable/2027592
*/
double gauss_rand(const double mean, const double sdev)
{
    assert(sdev > DBL_EPSILON, "sdev < 0 (%f)", sdev);

    static double V1, V2, S;
    static int phase = 0;
    double X;

    if (phase == 0)
    {
        do
        {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while (S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    }
    else
    {
        X = V2 * sqrt(-2 * log(S) / S);
    }

    phase = 1 - phase;

    return X * sdev + mean;
}

#if 0
/* Generate random numbers with mean mean, standard deviation std_dev, */
/* with a Normal (Gaussian) distribution.                              */
/* mean is any real number, std_dev > 0 (or machine epsilon).          */
/* The formula used is the inverse of the density function of the      */
/* Normal distribution:                                                */
/* x = mean +/- std_dev * sqrt((-2.0) * log(y)), 0 < y <= 1            */
/* The client should call srand(int) to initialize the seed,           */
/* before any calls to gauss_rand().                                   */
/* L.T., May 3, 1998, University of Toronto                            */
/* Source: https://c-faq.com/lib/gaussrand.luben.html (modified)       */
/* does not work (tested: 200 values generated with seed 1234)         */

double gauss_rand(const double mean, const double sdev)
{
    /* std_dev must be greater than 0.0 (or machine epsilon) */
    if (!(sdev > DBL_EPSILON))
        return INFINITY; // chosen since this will hopefully crash the test

    double y = (double)rand() / (RAND_MAX + 1.0); /* 0.0 <= y < 1.0 */
    bool bin = (y < 0.5) ? false : true;
    y = fabs(y - 1.0); /* 0.0 < y <= 1.0 */
    y = sdev * sqrt((-2.0) * log(y));
    y = bin ? (mean + y) : (mean - y);

    return y;
}
#endif