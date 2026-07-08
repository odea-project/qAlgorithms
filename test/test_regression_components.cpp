#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvariadic-macro-arguments-omitted"

#include "qalgorithms_datatypes.h"
#include "qalgorithms_qpeaks.h"

#include "CDFlib/cdflib.hpp"
#include "pugixml/pugixml.hpp"
#include "shared/common_test_utils.hpp"

#include <cstdio> // printing

int main()
{
    using namespace qAlgorithms;
    // check if a basic regression succeeds
    {
        const float logInts[] = {6.40492535, 7.95729923, 8.44852829, 8.27999401, 7.23839712};
        const size_t len = sizeof(logInts) / sizeof(float);
        size_t scale = 2;
        std::vector<qAlgorithms::RegCoeffs> reg;
        findCoefficients(logInts, len, scale, &reg);
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
        std::vector<RegressionGauss> validRegressions;
        const size_t maxscale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            intensity.data(),
            &logIntensity,
            nullptr,
            intensity.size(),
            maxscale,
            &validRegressions);
        assert(validRegressions.size() == 1, "Failed to find representative centroid peak 1");
    }
    {
        std::vector<float> intensity = {1308.43738, 8625.85156, 29830.793, 55325.1523, 79094, 68921.2969, 41931.5977, 17812.5977, 5546.17139, 1343.45618};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxscale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            intensity.data(),
            &logIntensity,
            nullptr,
            intensity.size(),
            maxscale,
            &validRegressions);
        assert(validRegressions.size() == 1, "Failed to find representative centroid peak 2");
    }
    {
        std::vector<float> intensity = {2841.0835, 6436.08984, 14330.1494, 26352.5996, 29525.8105, 20583.4023, 13286.2842, 23209.8262, 53549.1562, 85892.2109, 92988.0469, 61581.0977, 29610.7812, 4705.82324};
        std::vector<float> mz = {};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxscale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            intensity.data(),
            &logIntensity,
            nullptr,
            intensity.size(),
            maxscale,
            &validRegressions);
        assert(validRegressions.size() == 2, "Failed to find representative centroid peaks in pair system 1");
    }
    {
        std::vector<float> intensity = {882.300964, 3681.37842, 10451.9082, 18054.3828, 24405.959, 20502.377, 12295.1006, 4777.3667, 985.008301, 258.505646, 1578.77429, 4576.50537, 7863.09326, 8823.82227, 6399.12842, 3314.36108, 837.389709};
        std::vector<RegressionGauss> validRegressions;
        const size_t maxscale = 8; // @todo not bound to centroid maxscale
        std::vector<float> logIntensity(25, NAN);
        logIntensity.clear();
        runningRegression(
            intensity.data(),
            &logIntensity,
            nullptr,
            intensity.size(),
            maxscale,
            &validRegressions);
        assert(validRegressions.size() == 2, "Failed to find representative centroid peaks in pair system 2");
    }

    // @todo test binning

    // @todo add a test for correct peak identification using a subset of real data (centroids and features)

    return 0;
}
