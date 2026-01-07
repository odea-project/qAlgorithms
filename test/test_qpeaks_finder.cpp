#include "common_test_utils.hpp"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_datatypes.h"

using namespace qAlgorithms;

int simulate_profile(
    const RegCoeffs *coeff,
    std::vector<float> *simulated)
{
    assert(coeff->x0 > 1);

    double x = -coeff->x0;
    for (size_t i = 0; i < coeff->x0; i++)
    {
        simulated->at(i) = regExpAt(coeff, x);
        x += 1;
    }
    for (size_t i = coeff->x0; i < simulated->size(); i++)
    {
        simulated->at(i) = regExpAt(coeff, x);
        x += 1;
    }
    return 0;
}

int main()
{
    RegCoeffs coeff;
    coeff.b0 = 10;
    coeff.b1 = -0.4;
    coeff.b2 = -0.1;
    coeff.b3 = -0.2;
    coeff.x0 = 5;
    std::vector<float> simulated;
    simulated.resize(11);
    std::vector<float> simulated_log;
    std::vector<unsigned int> df;

    simulate_profile(&coeff, &simulated);
    for (size_t i = 0; i < simulated.size(); i++)
    {
        simulated_log.push_back(log(simulated[i]));
        df.push_back(i + 1);
    };

    std::vector<RegressionGauss> validRegs;
    runningRegression(simulated.data(), &simulated_log, &df, &validRegs, simulated.size(), 5);
    assert(validRegs.size() == 1, "incorrect number of regressions found");

    return 0;
}