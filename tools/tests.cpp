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

    int sign = signbit(x) ? -1 : 1;

    int rounded = int(x * pow + 0.5 * sign); // @todo this does not incorporate the even-odd switch for a remainder of 1/2, but does that matter here?

    return (double(rounded) / pow);
}

void printVec_f(const std::vector<float> *vec, const char *vecName)
{
    assert(!vec->empty());
    printf("Vector %s:  %f", vecName, (*vec)[0]);
    for (size_t i = 1; i < vec->size(); i++)
    {
        printf(", %f", (*vec)[i]);
    }
    printf("  | length: %zu\n", vec->size());
}

int main()
{
    using namespace qAlgorithms;

    { // rounding is functional
        assert(roundTo_d(145.136578244512, 3) == 145.137);
        assert(roundTo_d(-145.136578244512, 3) == -145.137);
    }

    { // min and max functions - two numbers
        assert(min(size_t(0), INT64_MAX) == 0);
        assert(max(size_t(0), INT64_MAX) == INT64_MAX);
        assert(min(-40, -10) == -40);
        assert(max(-1, 0) == 0);
        assert(min(MAXFLOAT, -MAXFLOAT) == -MAXFLOAT);
        assert(max(MAXFLOAT, -MAXFLOAT) == MAXFLOAT);
    }

    { // min and max - array
        const size_t len = 5;
        double test_d[len] = {0, 100, 200, 300, MAXFLOAT};
        double *min_d = minVal(test_d, len);
        double *max_d = maxVal(test_d, len);
        assert(*min_d == 0);
        assert(*max_d == MAXFLOAT);
    }

    { // standard deviation - true values generated with wolfram alpha
        double numbers[10] = {1, 2, 3, 4, 5, 5, 5, 6, 3, 2};
        double sd = sdev(numbers, 10);
        qass(roundTo_d(sd, 14) == roundTo_d(1.64654520469713, 14), "Standard deviation calculated wrong");

        double numbers_L[10] = {300.021736801890, 299.997556193020, 299.996439273950, 300.003719530080, 299.993646472210, 299.990848954870, 299.989294329370, 299.998503324230, 300.008035381130, 300.004645938850};
        sd = sdev(numbers_L, 10);
        qass(roundTo_d(sd, 14) == roundTo_d(0.009603979272, 14), "Standard deviation inaccurate at larger numbers");
    }

    { // exact solutuion to linear equation
        double true_b0 = 2;
        double true_b1 = 3;
        double true_b2 = 1.5;

        double x1 = 1;
        double y1 = true_b0 + true_b1 * x1 + true_b2 * x1 * x1;
        double y1_test = quadraticAt(true_b0, true_b1, true_b2, x1);
        assert(y1_test == y1);
        double x2 = 2;
        double y2 = true_b0 + true_b1 * x2 + true_b2 * x2 * x2;
        double x3 = 3;
        double y3 = true_b0 + true_b1 * x3 + true_b2 * x3 * x3;

        double b0 = 0, b1 = 0, b2 = 0;
        coeffsQuadratic(x1, x2, x3,
                        y1, y2, y3,
                        &b0, &b1, &b2);
        qass(b0 == true_b0, "b0 differs\n");
        qass(b1 == true_b1, "b1 differs\n");
        qass(b2 == true_b2, "b2 differs\n");
    }

    // check if a basic regression succeeds
    {
        std::vector<float> logInts = {6.40492535, 7.95729923, 8.44852829, 8.27999401, 7.23839712};
        size_t scale = 2;
        std::vector<qAlgorithms::RegCoeffs> reg;
        findCoefficients(&logInts, scale, &reg);
        auto c = reg.front();
        // note that the regressions seems to be a numerically unstable process
        qass(roundTo_d(c.b0, 4) == roundTo_d(8.5012159, 4), "b0 is incorrect!");
        qass(roundTo_d(c.b1, 4) == roundTo_d(0.1143269, 4), "b1 is incorrect!");
        qass(roundTo_d(c.b2, 4) == roundTo_d(-0.4647133, 4), "b2 is incorrect!");
        qass(roundTo_d(c.b3, 4) == roundTo_d(-0.3706720, 4), "b3 is incorrect!");
        printf("simple regression gives correct results to four digits\n");
    }

    // check if a difficult centroid is identified correctly
    {
        ProfileBlock block = {
            {8, 16, 32, 475, 711, 472, 207, 57, 14, 9, 4, 2, 1},
            {205.110107, 205.115082, 205.120056, 205.125031, 205.130005, 205.134979, 205.139954, 205.144928, 205.149902, 205.154877, 205.159851, 205.164825, 205.16980, 205.174774},
            {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10},
            329,
            338};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxScale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(&block.intensity, &logIntensity, &block.cumdf, &validRegressions, maxScale);
    }

    // does the RT conversion struct work correctly?

    std::vector<float>
        RTs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 27, 28, 29, 30};
    std::vector<float> correctRTs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    assert(correctRTs.size() == size_t(RTs.back()));
    RT_Converter test = interpolateScanNumbers(&RTs);

    for (size_t i = 0; i < RTs.size(); i++)
    {
        float correctRT = RTs[i];
        size_t convertIdx = test.indexOfOriginalInInterpolated[i];
        float assumedRT = test.groups[convertIdx].trueRT;
        qass(correctRT == assumedRT, "RTs cannot be restored from the original index");
    }

    qass(test.groups.size() == 30, "Incorrect number of retention times");
    printf("RTs are correctly interpolated\n");

    // test: execute binning function on five identical centroids with increasing scan numbers
    // @todo the test is wrong
    {
        CentroidPeak centroid = {0};
        centroid.mz = 100;
        centroid.mzUncertainty = 2 * 10e-6;

        size_t vecLen = 6;
        std::vector<CentroidPeak> inputCens(vecLen, centroid);
        // the first centroid is a dummy value
        inputCens[0].mz = 0;

        std::vector<unsigned int> convertRT(vecLen, 0);
        for (size_t i = 0; i < vecLen; i++)
        {
            inputCens[i].number_MS1 = i;
            inputCens[i].ID = i;
            inputCens[i].mz = randRange_d(99.998, 100.002);
            convertRT[i] = i;
        }

        vecLen -= 1;

        // expected output: one EIC with five points
        std::vector<qAlgorithms::EIC> testEIC = performQbinning_old(&inputCens, &test);
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
        testEIC = performQbinning_old(&inputCens, &test);
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

        testEIC = performQbinning_old(&inputCens, &test);
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() == 2, "Incorrect number of EICs");
        qass(testEIC[1].mz.size() == 10, "Incorrect EIC size");
        printf("EIC processing with two EICs and noise inbetween works\n");
    }

    //@todo add a test for correct peak identification using a subset of real data (centroids and features)

    return 0;
}
