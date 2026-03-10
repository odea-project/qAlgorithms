#include "qalgorithms_qpeaks.h"
#include "qalgorithms_datatypes.h"
#include "math.h"
#include "string"

#include "common_test_utils.hpp"

using namespace qAlgorithms;

double peakVal_gauss(double x, double apex, double height, double sdev)
{
    return height * exp(-(x - apex) * (x - apex) / (2 * sdev * sdev));
}

double area_gauss(double height, double sdev)
{
    return height * sdev * sqrt(2 * M_PI);
}

double fwhm_gauss(double sdev)
{
    // 0.5 * height = height * exp(-(x-apex)^2 / (2 * sdev^2))
    // 0.5 = exp(-(x-apex)^2 / (2 * sdev^2))
    // ln(0.5) * (2 * sdev^2) = -(x-apex)^2
    // sqrt(-ln(0.5) * 2 * sdev^2) = x - apex
    // x = apex + sqrt(-ln(0.5) * 2 * sdev^2)

    // since the gaussian peak is symmetrical, we only need the distance from the apex * 2
    // this means that the apex is not required for knowing the FWHM
    return 2 * sqrt(-log(0.5) * 2 * sdev * sdev);
}

// note on simulating points: it is not possible to just generate arbitrary points
// since the core assumption of qPeaks is that all points are evenly spaced. Furthermore,
// it only makes sense to characterise the algorithm with data where the full peak is captured.
// To make cases where two peaks are in one system or where noise is added easier, all
// peak functions are additive. It is the responsibility of the user to zero them before first use.
void simulate_gauss(
    const std::vector<float> *xvals,
    double apex, double height, double sdev,
    std::vector<float> *simulated)
{
    assert(!simulated->empty(), "wrong usage of test function\n");
    assert(xvals->size() == simulated->size(), "wrong usage of test function\n");

    for (size_t i = 0; i < simulated->size(); i++)
    {
        double x = xvals->at(i);
        // @todo add the option to randomise here
        simulated->at(i) += peakVal_gauss(x, apex, height, sdev);
    }
}

double peakVal_EMG(double x, double apex, double height, double sdev, double tau)
{
    // implements the exponentially modified gaussian fit as a generative model

    // naive model: https://doi.org/10.1021/ac970481d, Eq. B1
    // y(x) = A/t * exp(s^2 / 2t^2 + (x_0 - x) / s)    *    (1 - erf( (x - x_0) / (sqrt(2) * s) - s / (sqrt(2) * t) ) )
    // A = area, t = tau (tailing), s = sdev, x_0 = apex
    // problem: the first term gets very large while the second term gets very small

    // alternative equation: https://doi.org/10.1002/cem.1343 Eq. 6
    // z = 1/sqrt(2) * ( (x_0 - x) / s + s / t )
    // y(x) = h * exp( -(x_0 - x)^2 / (2 * s^2) ) * (s/t) * sqrt(pi/2) * exp(z^2) * erfc(z)

    double z = (1 / sqrt(2)) * ((apex - x) / sdev + sdev / tau);
    double a = height * exp(-(apex - x) * (apex - x) / (2 * sdev * sdev)) * (sdev / tau) * sqrt(M_PI_2);
    double y = a * exp(z * z) * erfc(z);
    if (!(y < INFINITY))
        return -1;
    return y;
}

double fwhm_EMG(double sdev, double tau)
{
    // z = 1/sqrt(2) * ( (x_0 - x) / s + s / t )
    // since FWHM is position-independent, we can set x_0 = 0
    // z = 1/sqrt(2) * (-x / s + s / t)
    // z = x * -(1/(sqrt(2) * s)) + (1/sqrt(2) * s / t)
    // z_1 = -(1/(sqrt(2) * s)), z_2 = (1/sqrt(2) * s / t)
    // z == x * z_1 + z_2

    // c_1 = (s/t) * sqrt(pi/2), c_2 = -1 / (2 * s^2)
    // 0.5 * h = h * exp( x^2 * c_2 ) * c_1 * exp(z^2) * erfc(z)
    // 0 = x^2 * c_2 + log(c_1) + z^2 + log(erfc(z)) - log(0.5)
    // log(erfc(z)) = log(1 - erf(z)) = log(1) / log(erf(z)) == 0

    // z^2 = x^2 * z_1^2 + 2 * x * z_1 * z_2 + z_2^2
    // c_3 = log(c_1) - log(0.5) + z_2^2
    // 0 = x^2 * c_2 + x^2 * z_1^2 + 2 * x * z_1 * z_2 + c_3
    // z_3 = 2 * z_1 * z_2 ; c_4 = c_2 + z_1^2

    // 0 = c_4 * x^2 + z_3 * x + c_3

    // however, c_4 == 0 (error in calculation?)
    // -> only one intersect with height?

    // The final form is just a quadratic equation!
    // since the function is asymmetric, we require both values for x

    double c_2 = -1 / (2 * sdev * sdev);
    double z_1 = -(1 / (sqrt(2) * sdev));
    double c_4 = c_2 + z_1 * z_1;

    double z_2 = (1 / sqrt(2) * sdev / tau);
    double z_3 = 2 * z_1 * z_2;

    double c_1 = (sdev / tau) * sqrt(M_PI_2);
    double c_3 = log(c_1) - log(0.5) + z_2 * z_2;

    double x1, x2;
    solveQuadratic(c_4, z_3, c_3, &x1, &x2); // @todo this will fail

    return x2 - x1;
}

void simulate_EMG(
    double x_start, double x_step,
    double apex, double height, double sdev, double tau,
    std::vector<float> *xvals, std::vector<float> *yvals)
{
    assert(yvals->empty(), "wrong usage of test function\n");
    assert(xvals->empty(), "wrong usage of test function\n");

    double x = x_start;
    bool peakDone = false;
    for (size_t i = 0; i < 1000000; i++)
    {
        double y = peakVal_EMG(x, apex, height, sdev, tau);
        if (y > 0.01)
        {
            yvals->push_back(y);
            xvals->push_back(x);
            peakDone = true;
        }
        else if (peakDone)
            break;
        x += x_step;
    }
}

void control_sim_gauss()
{
    // generate data using a standard gaussian on an equidistant x axis
    float x_start = 100;
    float x_step = 1.1;
    float apex = 115;
    size_t length = 30;
    double sdev = 2.5;
    double height = 1000;

    double fwhm = fwhm_gauss(sdev);
    double area = area_gauss(height, sdev);

    std::vector<float> xvals(length, 0);
    std::vector<float> yvals(length, 0);
    for (size_t i = 0; i < length; i++)
    {
        xvals[i] = x_start + x_step * i;
    }

    simulate_gauss(&xvals, apex, height, sdev, &yvals);

    std::vector<PeakFit> ret;
    qpeaks_find(yvals.data(), xvals.data(), nullptr, length, 8, &ret);

    assert(ret.size() != 0, "Peak not found\n");
    assert(ret.size() == 1, "Too many peaks found\n");

    PeakFit reg = ret.front();

    float apex_p = reg.position;
    float height_p = reg.height;
    float fwhm_p = reg.fwhm;
    // float area_p = reg.area; // wrong result

    float area_e = area_empiric(&xvals, &yvals);
    RegCoeffs c = reg.coeffs;
    float area_c = peakArea(c.b0, c.b1, c.b2, c.b3, x_step);

    assert(abs(apex - apex_p) < reg.position_uncert, "inaccurate position\n");
    assert(abs(height - height_p) < reg.height_uncert, "inaccurate height\n");
    assert(abs(fwhm - fwhm_p) < 10e-4, "inaccurate width\n");
    assert(abs(area - area_c) < 0.01, "inaccurate area (%f vs. %f), empiric %f\n", area, area_c, area_e);
}

struct ErrorEMG
{
    float r_tau;
    float d_area_rel, d_area_abs;
    float r_sdev, d_fwhm_rel, d_fwhm_abs;
    float r_apex, d_apex_abs;
    float r_height, d_height_rel, d_height_abs;
    float dqs, jaccard;
    bool negativeB23 = true;
};

// void control_sim_EMG(float x_start, float x_step, float y_min, size_t n_samples, ErrorEMG *in_out)
void control_sim_EMG(float x_start, float x_step, ErrorEMG *in_out)
{
    // generate data using an exponentially modified gaussian on an equidistant x axis

    float apex = in_out->r_apex;
    double sdev = in_out->r_sdev;
    double height = in_out->r_height;
    double tau = in_out->r_tau;

    std::vector<float> xvals;
    std::vector<float> yvals;

    simulate_EMG(x_start, x_step, apex, height, sdev, tau, &xvals, &yvals);
    x_start = xvals.front();
    float x_end = xvals.back();
    size_t length = xvals.size();
    in_out->jaccard = 0;

    if (length < 5)
    {
        in_out->dqs = -2;
        return;
    }

    // take n equidistant samples from the generated distribution, starting at y_min.
    size_t idx_start = 0;
    for (idx_start; idx_start < length; idx_start++)
    {
        // @todo
    }

    std::vector<PeakFit> ret;
    qpeaks_find(yvals.data(), xvals.data(), nullptr, length, 20, &ret);

    // printf("    Observed values:\n");
    // for (size_t i = 0; i < length; i++)
    // {
    //     printf("%f,", yvals[i]);
    // }
    // printf("\n");

    // if (ret.size() == 1)
    // {
    //     print_regFit(&ret.front().coeffs, &xvals, x_step);
    // }
    // else
    // {
    //     assert(ret.empty(), "error in test design!\n");
    //     printf("    No prediction possible, regression unfit\n####\n");
    // }

    xvals.clear();
    yvals.clear();
    simulate_EMG(x_start, x_step / 500, apex, height, sdev, tau, &xvals, &yvals);
    double fwhm_e = fwhm_empiric(&xvals, &yvals);
    double area_e = area_empiric(&xvals, &yvals);
    double apex_e = position_empiric(&xvals, &yvals);
    double height_e = *maxVal(yvals.data(), yvals.size());

    in_out->r_height = height_e;
    in_out->r_apex = apex_e;

    // assert(ret.size() != 0, "Peak not found\n");
    // this check is removed since some configurations of parameters result in no peaks
    if (ret.empty())
    {
        in_out->dqs = -1;
        return;
    }

    assert(ret.size() == 1, "Too many peaks found\n");

    PeakFit reg = ret.front();

    float apex_p = reg.position;
    float height_p = reg.height;
    float fwhm_p = reg.fwhm;
    // float area_p = reg.area;

    RegCoeffs c = reg.coeffs;

    in_out->negativeB23 = (c.b2 < 0) && (c.b3 < 0);

    float area_c = peakArea(c.b0, c.b1, c.b2, c.b3, x_step);

    // empiric estimation of peak parameters

    // assert(abs(apex_e - apex_p) < 0.01, "inaccurate position (%f vs. %f)\n", apex_e, apex_p);
    // assert(abs(height_e - height_p) < height_e * 0.01, "inaccurate height (%f vs. %f)\n", height_e, height_p);
    // assert(abs(fwhm_e - fwhm_p) < fwhm_e * 0.05, "inaccurate width (%f vs. %f)\n", fwhm_e, fwhm_p);
    // assert(abs(area_e - area_p) < reg.area_uncert, "inaccurate area (%f vs. %f)\n", area_e, area_p);

    in_out->d_area_rel = abs(area_e - area_c) / area_c;
    in_out->d_area_abs = abs(area_e - area_c);
    in_out->d_fwhm_rel = abs(fwhm_e - fwhm_p) / fwhm_p;
    in_out->d_fwhm_abs = abs(fwhm_e - fwhm_p);
    in_out->d_apex_abs = abs(apex_e - apex_p);
    in_out->d_height_rel = abs(height_e - height_p) / height;
    in_out->d_height_abs = abs(height_e - height_p);
    in_out->dqs = reg.dqs;
    in_out->jaccard = reg.jaccard;
}

void survey_EMG()
{
    // print results of fitting various EMG simulations to a file
    std::vector<ErrorEMG> testCases;
    float x_start = 10;
    float x_step = 1;
    ErrorEMG test = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0};
    test.r_apex = 20;

    std::string container = "height,sdev,tau,position,d_area_rel,d_area_abs,d_fwhm_rel,d_fwhm_abs,d_apex_abs,d_height_rel,d_height_abs,dqs,jaccard,closedPeak\n";

    float heights[5] = {100, 1000, 2500, 5000, 10000};

    for (size_t i = 0; i < 5; i++)
    {
        for (float sd = 0.7; sd < 3.1; sd += 0.1)
        {
            for (float tau = 0.1; tau < 2.1; tau += 0.1)
            {
                test.r_height = heights[i];
                test.r_sdev = sd;
                test.r_tau = tau;
                control_sim_EMG(x_start, x_step, &test);
                testCases.push_back(test);
                char buffer[500];
                sprintf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%c\n", test.r_height, test.r_sdev, test.r_tau, test.r_apex, test.d_area_rel, test.d_area_abs,
                        test.d_fwhm_rel, test.d_fwhm_abs, test.d_apex_abs, test.d_height_rel, test.d_height_abs, test.dqs, test.jaccard, test.negativeB23 ? 'T' : 'F');
                container += buffer;
                test.d_height_rel = -1;
                test.d_height_abs = -1;
                test.d_apex_abs = -1;
                test.d_area_rel = -1;
                test.d_area_abs = -1;
                test.d_fwhm_rel = -1;
                test.d_fwhm_abs = -1;
                test.dqs = -1;
            }
        }
    }
    FILE *f = fopen("./peaktest.csv", "w");
    fprintf(f, "%s", container.c_str());
    fclose(f);
}

int simulate_profile(
    const RegCoeffs *coeff,
    std::vector<float> *simulated,
    std::vector<float> *simulated_log)
{
    assert(coeff->x0 > 1, "error");

    double x = -double(coeff->x0);
    for (size_t i = 0; i < coeff->x0; i++)
    {
        simulated->at(i) = exp(regAt(coeff, x));
        x += 1;
    }
    for (size_t i = coeff->x0; i < simulated->size(); i++)
    {
        simulated->at(i) = exp(regAt(coeff, x));
        x += 1;
    }
    simulated_log->clear();
    for (size_t i = 0; i < simulated->size(); i++)
    {
        simulated_log->push_back(log(simulated->at(i)));
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

    simulate_profile(&coeff, &simulated, &simulated_log);

    std::vector<RegressionGauss> validRegs;
    runningRegression(simulated.data(), &simulated_log, nullptr, simulated.size(), 5, &validRegs);

    assert(validRegs.size() == 1, "incorrect number of regressions found\n");
    double diff_b0 = abs(coeff.b0 - validRegs.front().coeffs.b0);
    double diff_b1 = abs(coeff.b1 - validRegs.front().coeffs.b1);
    double diff_b2 = abs(coeff.b2 - validRegs.front().coeffs.b2);
    double diff_b3 = abs(coeff.b3 - validRegs.front().coeffs.b3);

    assert(diff_b0 < abs(coeff.b0) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b1 < abs(coeff.b1) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b2 < abs(coeff.b2) * 0.05, "> 5%% Error in b0 estimate\n");
    assert(diff_b3 < abs(coeff.b3) * 0.05, "> 5%% Error in b0 estimate\n");
}

int simulate_stepwise(
    const RegCoeffs *coeff,
    std::vector<float> *xvec,
    std::vector<float> *simulated,
    double delta_x)
{
    assert(coeff->x0 > 1, "error");

    double x = -delta_x * coeff->x0;
    for (size_t i = 0; i < coeff->x0; i++)
    {
        xvec->push_back(x);
        simulated->at(i) = exp(regAt(coeff, x));
        x += delta_x;
    }
    for (size_t i = coeff->x0; i < simulated->size(); i++)
    {
        xvec->push_back(x);
        simulated->at(i) = exp(regAt(coeff, x));
        x += delta_x;
    }
    return 0;
}

void test_areaPrediction()
{
    RegCoeffs coeff;
    coeff.b0 = 8;
    coeff.b1 = 0.32;
    coeff.b2 = -0.13;
    coeff.b3 = -0.22;
    coeff.x0 = 500;

    std::vector<float> x;
    std::vector<float> y;
    y.resize(1000);

    float x_step = 1;
    simulate_stepwise(&coeff, &x, &y, x_step * 0.05); // note: the sampling rate is not equal to the stride of x
    double area_e = area_empiric(&x, &y);
    double area_t = peakArea(coeff.b0, coeff.b1, coeff.b2, coeff.b3, x_step);

    // the area calculation is wrong!
    // RegressionGauss testreg;
    // testreg.coeffs = coeff;
    // calcPeakAreaUncert(&testreg);
    // double area_c = testreg.area * exp(coeff.b0);
    // assert(abs(area_e - area_c) < 0.01, "Incorrect area calculation\n");

    assert(abs(area_e - area_t) < 0.01, "Incorrect area calculation\n"); // expected area > 10^5
}

int main()
{
    // initialise seed for noise generating funtions
    srand(1234);

    test_singlePeak();
    test_areaPrediction();
    control_sim_gauss();
    survey_EMG();

    return 0;
}