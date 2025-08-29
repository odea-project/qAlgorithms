#include "../include/qalgorithms_datatypes.h"
#include "../include/qalgorithms_global_vars.h"
#include "../include/qalgorithms_input_output.h"
#include "../include/qalgorithms_qbin.h"
#include "../include/qalgorithms_qpattern.h"
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_read_file.h"
#include "../include/qalgorithms_utils.h"

#include "../external/CDFlib/cdflib.hpp"
#include "../external/pugixml-1.14/src/pugixml.h"
#include "../external/simdutf/simdutf.h"

#include <cstdio> // printing

void qassert(bool condition, size_t line, const char *message)
{
    if (condition)
        return;

    printf("Error in line %zu: %s\n", line, message);
    exit(1);
}
#define qass(boolean, message) qassert(boolean, __LINE__, message)

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

int nanoseconds(void)
{
    struct timespec p[1];
    clock_gettime(CLOCK_MONOTONIC, p);
    return p->tv_nsec % 1000;
}

// produce a random double value in the range (lower, upper). If no seed is supplied, the seed is also random
double randRange_d(double lower, double upper, long seed = 0)
{
    assert(lower < upper);
    long randint = seed;
    if (seed == 0)
        randint = nanoseconds();
    double randDouble = ran0(&randint); // random number between 0 and 1
    double span = upper - lower;

    double res = lower + span * randDouble;
    assert(res > lower);
    assert(res < upper);
    return res;
}

// round a number x to a given number of digits
double roundTo_d(double x, size_t digits)
{
    if (digits == 0)
        return 0;
    // this is a naive implementation and overall not robust enough to use uncritically with very large numbers
    const size_t maxPrec = 17;
    assert(digits < maxPrec);

    static const double powers[maxPrec]{1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
                                        1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16};

    double pow = powers[digits];

    assert(double(INT64_MAX) / pow > x); // prevent overflow
    assert(double(INT64_MIN) / pow < x); // prevent underflow

    int rounded = int(x * pow + 0.5); // @todo this does not incorporate the even-odd switch for a remainder of 1/2, but does that matter here?

    return (double(rounded) / pow);
}

int main()
{
    using namespace qAlgorithms;

    // test: execute binning function on five identical centroids with increasing scan numbers
    {
        CentroidPeak centroid = {0};
        centroid.mz = 100;
        centroid.mzUncertainty = 2 * 10e-6;

        size_t vecLen = 6;
        std::vector<CentroidPeak> inputCens(vecLen, centroid);
        // the first centroid is a dummy value
        inputCens[0].mz = 0;

        std::vector<unsigned int> convertRT(vecLen, 0);
        for (size_t i = 1; i < vecLen; i++)
        {
            inputCens[i].number_MS1 = i;
            inputCens[i].ID = i;
            inputCens[i].mz = randRange_d(99.998, 100.002);
            convertRT[i] = i;
        }

        vecLen -= 1;

        // expected output: one EIC with five points
        std::vector<qAlgorithms::EIC> testEIC = performQbinning_old(&inputCens, &convertRT);
        volatile qAlgorithms::EIC copy = testEIC[0];
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() < 2, "More than one EIC found");
        qass(testEIC[0].mz.size() > vecLen - 1 + 4, "Some points were excluded from the EIC");
        qass(testEIC[0].mz.size() < vecLen + 1 + 4, "Centroids added to EIC");
        printf("EIC processing with one EIC and no interference works\n");

        convertRT.push_back(vecLen + 1);
        inputCens.push_back(centroid);
        inputCens.back().mz = 200;
        inputCens.back().mzUncertainty = 4 * 10e-6;
        inputCens.back().number_MS1 = vecLen;
        inputCens.back().ID = vecLen + 1;

        vecLen++;
        // expected output: one EIC with five points and one trailing centroid
        testEIC = performQbinning_old(&inputCens, &convertRT);
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() < 2, "More than one EIC found");
        // correct length is 9 since two points are added to each side. This is very confusing, change it @todo
        qass(testEIC[0].mz.size() > 8, "Some points were excluded from the EIC");
        qass(testEIC[0].mz.size() < 10, "Centroids added to EIC");
        printf("EIC processing with one EIC and one noise point past the end works\n");

        // test for two bins separated in mz and RT with some noise and the second bin having no other points
        vecLen += 6;
        for (size_t i = 0; i < 6; i++)
        {
            CentroidPeak centroid2 = {0};
            centroid2.mz = 400;
            centroid2.mzUncertainty = 5 * 10e-6;
            centroid2.number_MS1 = i;
            centroid2.ID = vecLen + i;
            inputCens.push_back(centroid2);
            size_t prevRT = convertRT.back();
            convertRT.push_back(prevRT + 1);
        }

        for (size_t RT : convertRT)
            printf("%zu ", RT);

        testEIC = performQbinning_old(&inputCens, &convertRT);
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() == 2, "Incorrect number of EICs");
        qass(testEIC[1].mz.size() == 10, "Incorrect EIC size");
        printf("EIC processing with two EICs and noise inbetween works\n");
    }

    //@todo add a test for correct peak identification using a subset of real data (centroids and features)

    return 0;
}
