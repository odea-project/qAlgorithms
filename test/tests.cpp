#include "qalgorithms_datatypes.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_utils.h"

#include "CDFlib/cdflib.hpp"
#include "pugixml-1.14/src/pugixml.h"
#include "shared/common_test_utils.hpp"

#include <cstdio> // printing

int main()
{
    using namespace qAlgorithms;
    // check if a basic regression succeeds
    {
        std::vector<float> logInts = {6.40492535, 7.95729923, 8.44852829, 8.27999401, 7.23839712};
        size_t scale = 2;
        std::vector<qAlgorithms::RegCoeffs> reg;
        findCoefficients(&logInts, scale, &reg);
        auto c = reg.front();
        // note that the regressions seems to be a numerically unstable process
        assert(roundTo_d(c.b0, 4) == roundTo_d(8.5012159, 4), "b0 is incorrect!");
        assert(roundTo_d(c.b1, 4) == roundTo_d(0.1143269, 4), "b1 is incorrect!");
        assert(roundTo_d(c.b2, 4) == roundTo_d(-0.4647133, 4), "b2 is incorrect!");
        assert(roundTo_d(c.b3, 4) == roundTo_d(-0.3706720, 4), "b3 is incorrect!");
        printf("simple regression gives correct results to four digits\n");
    }

    // check if a difficult centroid is identified correctly
    {
        std::vector<float> intensity = {32, 475, 711, 472, 207, 132, 57, 14};
        std::vector<float> mz = {205.120056, 205.125031, 205.130005, 205.134979, 205.139954, 205.144928, 205.149902, 205.154877};
        std::vector<unsigned int> df = {1, 2, 3, 4, 5, 6, 7, 8};
        ProfileBlock block = {
            intensity.data(),
            mz.data(),
            329,
            8};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxScale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            block.intensity,
            &logIntensity,
            &df,
            &validRegressions,
            block.length,
            maxScale);
        assert(validRegressions.size() == 1, "Failed to find representative centroid peak 1");
    }
    {
        std::vector<float> intensity = {1308.43738, 8625.85156, 29830.793, 55325.1523, 79094, 68921.2969, 41931.5977, 17812.5977, 5546.17139, 1343.45618};
        std::vector<float> mz = {};
        std::vector<unsigned int> df = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        ProfileBlock block = {
            intensity.data(),
            mz.data(),
            0,
            intensity.size()};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxScale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            block.intensity,
            &logIntensity,
            &df,
            &validRegressions,
            block.length,
            maxScale);
        assert(validRegressions.size() == 1, "Failed to find representative centroid peak 2");
    }
    {
        std::vector<float> intensity = {2841.0835, 6436.08984, 14330.1494, 26352.5996, 29525.8105, 20583.4023, 13286.2842, 23209.8262, 53549.1562, 85892.2109, 92988.0469, 61581.0977, 29610.7812, 4705.82324};
        std::vector<float> mz = {};
        std::vector<unsigned int> df = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        ProfileBlock block = {
            intensity.data(),
            mz.data(),
            0,
            intensity.size()};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxScale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            block.intensity,
            &logIntensity,
            &df,
            &validRegressions,
            block.length,
            maxScale);
        assert(validRegressions.size() == 2, "Failed to find representative centroid peaks in pair system 1");
    }
    {
        std::vector<float> intensity = {882.300964, 3681.37842, 10451.9082, 18054.3828, 24405.959, 20502.377, 12295.1006, 4777.3667, 985.008301, 258.505646, 1578.77429, 4576.50537, 7863.09326, 8823.82227, 6399.12842, 3314.36108, 837.389709};
        std::vector<float> mz = {};
        std::vector<unsigned int> df = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        ProfileBlock block = {
            intensity.data(),
            mz.data(),
            0,
            intensity.size()};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxScale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            block.intensity,
            &logIntensity,
            &df,
            &validRegressions,
            block.length,
            maxScale);
        assert(validRegressions.size() == 2, "Failed to find representative centroid peaks in pair system 2");
    }

    // does the RT conversion struct work correctly?

    std::vector<float>
        RTs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 27, 28, 29, 30};
    std::vector<float> correctRTs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    verify(correctRTs.size() == size_t(RTs.back()));
    RT_Converter test = interpolateScanNumbers(&RTs);

    for (size_t i = 0; i < RTs.size(); i++)
    {
        float correctRT = RTs[i];
        size_t convertIdx = test.indexOfOriginalInInterpolated[i];
        float assumedRT = test.groups[convertIdx].trueRT;
        assert(correctRT == assumedRT, "RTs cannot be restored from the original index");
    }

    assert(test.groups.size() == 30, "Incorrect number of retention times");
    printf("RTs are correctly interpolated\n");

    // test: execute binning function on five identical centroids with increasing scan numbers
    // @todo the test is wrong
    if (false)
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
            inputCens[i].mz = randRange_d(99.998, 100.002, 1);
            convertRT[i] = i;
        }

        vecLen -= 1;

        // expected output: one EIC with five points
        std::vector<qAlgorithms::EIC> testEIC = performQbinning_old(&inputCens, &test);
        volatile qAlgorithms::EIC copy = testEIC[0];
        assert(testEIC.size() != 0, "No EIC constructed");
        assert(testEIC.size() < 2, "More than one EIC found");
        assert(testEIC[0].mz.size() > vecLen - 1 + 4, "Some points were excluded from the EIC");
        assert(testEIC[0].mz.size() < vecLen + 1 + 4, "Centroids added to EIC");
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
        assert(testEIC.size() != 0, "No EIC constructed");
        assert(testEIC.size() < 2, "More than one EIC found");
        // correct length is 9 since two points are added to each side. This is very confusing, change it @todo
        assert(testEIC[0].mz.size() > 8, "Some points were excluded from the EIC");
        assert(testEIC[0].mz.size() < 10, "Centroids added to EIC");
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
        assert(testEIC.size() != 0, "No EIC constructed");
        assert(testEIC.size() == 2, "Incorrect number of EICs");
        assert(testEIC[1].mz.size() == 10, "Incorrect EIC size");
        printf("EIC processing with two EICs and noise inbetween works\n");
    }

    //@todo add a test for correct peak identification using a subset of real data (centroids and features)

    return 0;
}
