#include "qalgorithms_utils.h"
#include "common_test_utils.hpp"
#include <cmath>
#include <cstdint>

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
        verify(min(MAXFLOAT, -MAXFLOAT) == -MAXFLOAT);
        verify(max(MAXFLOAT, -MAXFLOAT) == MAXFLOAT);
    }
void test_array_min_max()
    { // min and max - array
        const size_t len = 5;
        double test_d[len] = {0, 100, 200, 300, MAXFLOAT};
        double *min_d = minVal(test_d, len);
        double *max_d = maxVal(test_d, len);
        verify(*min_d == 0);
        verify(*max_d == MAXFLOAT);
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
void test_linear_solve() // TODO not sure about the name here
    { // exact solutuion to linear equation
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
        assert(b0 == true_b0, "b0:%f should have been %f\n",b0,true_b0);
        assert(b1 == true_b1, "b1:%f should have been %f\n",b1,true_b1);
        assert(b2 == true_b2, "b2:%f should have been %f\n",b2,true_b2);
    }


int main() {
	// This could be automated using compile time execution 
	test_rounding();
	test_min_max();
	test_array_min_max();
	test_standard_deviation();
	test_linear_solve();
}
