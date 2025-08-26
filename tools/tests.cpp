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
    // function taken from numerical recepies
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

int nanorand(void)
{
    struct timespec p[1];
    clock_gettime(CLOCK_MONOTONIC, p);
    return p->tv_nsec % 1000;
}

double randRange_d(double lower, double upper)
{
    assert(lower < upper);
    long randint = nanorand();
    double randDouble = ran0(&randint); // random number between 0 and 1
    double span = upper - lower;

    double res = lower + span * randDouble;
    assert(res > lower);
    assert(res < upper);
    return res;
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
            inputCens[i].mz = randRange_d(99.998, 100.002);
            convertRT[i] = i;
        }

        vecLen -= 1;

        // expected output: one EIC with five points
        std::vector<qAlgorithms::EIC> testEIC = performQbinning(&inputCens, &convertRT);
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() < 2, "More than one EIC found");
        qass(testEIC[0].mz.size() > vecLen - 1, "Some points were excluded from the EIC");
        qass(testEIC.size() < vecLen + 1, "Centroids added to EIC");
        printf("EIC processing with one EIC and no interference works\n");

        convertRT.push_back(vecLen);
        inputCens.push_back(centroid);
        inputCens.back().mz = 200;
        inputCens.back().mzUncertainty = 4 * 10e-6;
        inputCens.back().number_MS1 = vecLen;

        vecLen++;
        // expected output: one EIC with five points and one trailing centroid
        testEIC = performQbinning(&inputCens, &convertRT);
        qass(testEIC.size() != 0, "No EIC constructed");
        qass(testEIC.size() < 2, "More than one EIC found");
        qass(testEIC[0].mz.size() > vecLen - 1, "Some points were excluded from the EIC");
        qass(testEIC.size() < vecLen + 1, "Centroids added to EIC");
    }

    return 0;
}
