#include "qalgorithms_qpeaks.h"
#include "qalgorithms_datatypes.h"

#include "common_test_utils.hpp"

using namespace qAlgorithms;

double peakVal_gauss(double x, double apex, double height, double sdev)
{
    return height * exp(-(x - apex) * (x - apex) / (2 * sdev * sdev));
}

// note on simulating points: it is not possible to just generate arbitrary points
// since the core assumption of qPeaks is that all points are evenly spaced. Furthermore,
// it only makes sense to characterise the algorithm with data where the full peak is captured.
// To make cases where two peaks are in one system or where noise is added easier, all
// peak functions are additive. It is the responsibility of the user to zero them before first use.
void simulate_gauss(
    double start_x, double step,
    double apex, double height, double sdev,
    std::vector<float> *simulated)
{
    assert(!simulated->empty(), "wrong usage of test function\n");
    assert(apex + sdev < start_x + step * simulated->size(), "wrong usage of test function\n");

    for (size_t i = 0; i < simulated->size(); i++)
    {
        double x = start_x + i * step;
        simulated->at(i) += peakVal_gauss(x, apex, height, sdev);
    }
}

void control_sim()
{
    // the challenge here is that we do a fit only in y, so the x axis needs to be fitted in backwards.
}

int simulate_profile(
    const RegCoeffs *coeff,
    std::vector<float> *simulated,
    std::vector<float> *simulated_log,
    std::vector<unsigned int> *df)
{
    assert(coeff->x0 > 1, "error");

    double x = -double(coeff->x0);
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
    simulated_log->clear();
    df->clear();
    for (size_t i = 0; i < simulated->size(); i++)
    {
        simulated_log->push_back(log(simulated->at(i)));
        df->push_back(i + 1);
    };
    return 0;
}

void test_singlePeak()
{
    RegCoeffs coeff;
    coeff.b0 = 12;
    coeff.b1 = 0.32;
    coeff.b2 = -0.13;
    coeff.b3 = -0.22;
    coeff.x0 = 5;
    std::vector<float> simulated;
    simulated.resize(11);
    std::vector<float> simulated_log;
    std::vector<unsigned int> df;

    simulate_profile(&coeff, &simulated, &simulated_log, &df);

    std::vector<RegressionGauss> validRegs;
    runningRegression(simulated.data(), &simulated_log, &df, &validRegs, simulated.size(), 5);

    assert(validRegs.size() == 1, "incorrect number of regressions found\n");
    double diff_b0 = abs(coeff.b0 - validRegs.front().coeffs.b0);
    double diff_b1 = abs(coeff.b1 - validRegs.front().coeffs.b1);
    double diff_b2 = abs(coeff.b2 - validRegs.front().coeffs.b2);
    double diff_b3 = abs(coeff.b3 - validRegs.front().coeffs.b3);
    printf("b0: %f, b1: %f, b2: %f, b3: %f\n", diff_b0, diff_b1, diff_b2, diff_b3);

    assert(diff_b0 < abs(coeff.b0) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b1 < abs(coeff.b1) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b2 < abs(coeff.b2) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b3 < abs(coeff.b3) * 0.05, "> 5%% Error in b0 estimate\n");
}

int main()
{
    test_singlePeak();

    return 0;
}