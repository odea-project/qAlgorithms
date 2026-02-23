#include "qalgorithms_utils.h"
#include "common_test_utils.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdint>
#include "../../external/fastexp.h"
#include <chrono>

using namespace qAlgorithms;

void test_rounding()
{ // rounding is functional
    verify(roundTo_d(145.136578244512, 3) == 145.137);
    verify(roundTo_d(-145.136578244512, 3) == -145.137);
}

void test_min_max()
{ // min and max functions - two numbers
    verify(min(size_t(0), INT64_MAX) == 0);
    verify(max(size_t(0), INT64_MAX) == INT64_MAX);
    verify(min(-40, -10) == -40);
    verify(max(-1, 0) == 0);
    verify(min(FLT_MAX, -FLT_MAX) == -FLT_MAX);
    verify(max(FLT_MAX, -FLT_MAX) == FLT_MAX);
}
void test_array_min_max()
{ // min and max - array
    const size_t len = 5;
    double test_d[len] = {0, 100, 200, 300, FLT_MAX};
    double *min_d = minVal(test_d, len);
    double *max_d = maxVal(test_d, len);
    verify(*min_d == 0);
    verify(*max_d == FLT_MAX);
}
void test_standard_deviation()
{ // standard deviation - true values generated with wolfram alpha
    double numbers[10] = {1, 2, 3, 4, 5, 5, 5, 6, 3, 2};
    double sd = sdev(numbers, 10);
    assert(roundTo_d(sd, 14) == roundTo_d(1.64654520469713, 14), "Standard deviation calculated wrong");

    double numbers_L[10] = {300.021736801890, 299.997556193020, 299.996439273950, 300.003719530080, 299.993646472210, 299.990848954870, 299.989294329370, 299.998503324230, 300.008035381130, 300.004645938850};
    sd = sdev(numbers_L, 10);
    assert(roundTo_d(sd, 14) == roundTo_d(0.009603979272, 14), "Standard deviation inaccurate at larger numbers");
}
void test_linear_solve() // @todo not sure about the name here
{                        // exact solutuion to linear equation
    double true_b0 = 2;
    double true_b1 = 3;
    double true_b2 = 1.5;

    double x1 = 1;
    double y1 = true_b0 + true_b1 * x1 + true_b2 * x1 * x1;
    double y1_test = quadraticAt(true_b0, true_b1, true_b2, x1);
    verify(y1_test == y1);
    double x2 = 2;
    double y2 = true_b0 + true_b1 * x2 + true_b2 * x2 * x2;
    double x3 = 3;
    double y3 = true_b0 + true_b1 * x3 + true_b2 * x3 * x3;

    double b0 = 0, b1 = 0, b2 = 0;
    coeffsQuadratic(x1, x2, x3,
                    y1, y2, y3,
                    &b0, &b1, &b2);
    assert(b0 == true_b0, "b0:%f should have been %f\n", b0, true_b0);
    assert(b1 == true_b1, "b1:%f should have been %f\n", b1, true_b1);
    assert(b2 == true_b2, "b2:%f should have been %f\n", b2, true_b2);
}

void test_exp_approx()
{
    size_t len = 1000000;
    std::vector<float> numbers(len, 0);
    std::vector<float> expLibc(len, 0);
    std::vector<float> approx1(len, 0);
    std::vector<float> approx2(len, 0);
    std::vector<float> approx3(len, 0);
    for (size_t i = 0; i < len; i++)
    {
        numbers[i] = double(rand() % 5000) / 100;
    }
    auto timeStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < len; i++)
    {
        expLibc[i] = exp(numbers[i]);
    }
    auto timeEnd = std::chrono::high_resolution_clock::now();
    float timePassed_exp = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    timeStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < len; i++)
    {
        approx1[i] = exp_approx_d(numbers[i]);
    }
    timeEnd = std::chrono::high_resolution_clock::now();
    float timePassed_app1 = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    timeStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < len; i++)
    {
        approx2[i] = fast_exp_1(numbers[i]);
    }
    timeEnd = std::chrono::high_resolution_clock::now();
    float timePassed_app2 = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    timeStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < len; i++)
    {
        approx3[i] = fast_exp_2(numbers[i]);
    }
    timeEnd = std::chrono::high_resolution_clock::now();
    float timePassed_app3 = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    double meanDiff_1 = 0;
    double meanDiff_2 = 0;
    double meanDiff_3 = 0;

    for (size_t i = 0; i < len; i++)
    {
        meanDiff_1 += abs(approx1[i] - expLibc[i]);
        meanDiff_2 += abs(approx2[i] - expLibc[i]);
        meanDiff_3 += abs(approx3[i] - expLibc[i]);
    }
    meanDiff_1 /= len;
    meanDiff_2 /= len;
    meanDiff_3 /= len;

    printf("### exp approximation timing results:\n Base time: %f s\n Approx 1: %f s, mean diff = %f\n Approx 2: %f s, mean diff = %f\n Approx 3: %f s, mean diff = %f\n",
           timePassed_exp, timePassed_app1, meanDiff_1, timePassed_app2, meanDiff_2, timePassed_app3, meanDiff_3);
}

void test_quadraticSolve()
{
    // solve a x^2 + b x + c = 0 for x
    double a = -3, b = 2, c = 1;
    double x_L_true = -1.0 / 3.0;
    double x_R_true = 1;

    double x_l, x_r;
    solveQuadratic(a, b, c, &x_l, &x_r);

    double diff_l = abs(x_l - x_L_true);
    assert(diff_l < DBL_EPSILON, "x_l differs too strongly (%f)", diff_l);

    double diff_r = abs(x_r - x_R_true);
    assert(diff_r < DBL_EPSILON, "x_l differs too strongly (%f)", diff_r);
}

void test_normalDist(int x)
{
    printf("r = c(");
    for (int i = 0; i < x - 1; i++)
    {
        printf("%f,", gauss_rand(0, 1));
    }
    printf("%f)", gauss_rand(0, 1));
}

int main()
{
    srand(1234);
    // This could be automated using compile time execution
    test_rounding();
    test_min_max();
    test_array_min_max();
    test_standard_deviation();
    test_linear_solve();
    // test_exp_approx();
    test_quadraticSolve();
    // test_normalDist(50000);
}
