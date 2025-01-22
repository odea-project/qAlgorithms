// qalgorithms_utils.h
#ifndef QALGORITHMS_UTILS_H // Include guarde to prevent double inclusion
#define QALGORITHMS_UTILS_H

// external
#include <vector>
#include <string>
#include <immintrin.h>

namespace qAlgorithms
{
    /**
     * @brief Fast exponential approximation base on Bit Manipulation.
     * @details This function is a fast approximation of the exponential
     * function. It is based on the bit manipulation of the double value.
     * The function is not as accurate as the standard exponential function
     * but is much faster. It is based on the following repository:
     * https://github.com/edrosten/fasterexp
     *
     * @param x : value between 0 and 26
     * @return double
     */
    double exp_approx_d(const double x);

    /**
     * @brief Fast approximation of the error function erf(x) using Abramowitz and Stegun approach.
     *
     * @param x
     * @return float
     */
    float erf_approx_f(const float x);

    /**
     * @brief Fast calculation of the Dawson integral by courtesy of Extra Byte, www.ebyte.it.
     * @details The Dawson integral is defined as: F(x) = exp(-x^2) * erfi(x)
     * * sqrt(pi)/2. For more details on this approximation, see www.dx.doi.org/10.3247/SL4Soft12.001
     *
     * @param x
     * @return double
     */
    double dawson5(double x);

    /**
     * @brief Fast calculation of exp(-x^2) * erfc(x) * sqrt(pi)/2.
     * @details This function uses an empirical approximation : exp(-x^2)
     * * erfc(x) * sqrt(pi)/2 ~ exp(-x^2) * sqrt(pi)/2 +- a * x * exp(-b * x^2).
     * The +- depends if exp(-x^2) * (1+-erf(x)) is used.
     *
     * @param x
     * @param sign : 1.0 or -1.0
     * @return double
     */
    double experfc(double x, double sign = -1.0);

    double erfi(const double x);
}
#endif // QALGORITHMS_UTILS_H