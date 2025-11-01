#ifndef QALGORITHMS_UTILS_H
#define QALGORITHMS_UTILS_H

#include <stddef.h>

// ### functions in this file should never call malloc! ###

namespace qAlgorithms
{
    /**
     * @brief Computes the F-statistic / F-Value for comparing a full model against a null model
     *
     * @param RSS_model     Residual sum of squares for the full (alternative) model
     * @param RSS_H0        Residual sum of squares under the null (reduced) model
     * @param params_model  Number of parameters in the full model
     * @param params_H0     Number of parameters in the null model
     * @param n             Number of real data points both models apply to
     * @return              The calculated F-statistic
     */
    double f_value(const double RSS_model, const double RSS_H0,
                   const size_t params_model, const size_t params_H0,
                   const size_t n);

    /**
     * @brief Computes the critical F‐statistic for a given alpha and model parameter counts using the cdff function from cdflib
     * @param alpha            Significance level (ex. 0.05 for 95% confidence)
     * @param params_complex   Number of parameters in the more complex model
     * @param params_simple    Number of parameters in the simpler (nested) model
     * @param numPoints        Total number of observations
     * @return The critical F value (quantile) such that P(F ≤ returned_value) = 1 – α,
     *         for an F distribution with numerator df = params_complex – params_simple
     *         and denominator df = numPoints – params_complex.
     */
    double cdflib_F_stat(double alpha, size_t params_complex, size_t params_simple, size_t numPoints);

    // exact solution for quadratic equation with three points and thing to get the y for a given x
    void coeffsQuadratic(const double x1, const double x2, const double x3,
                         const double y1, const double y2, const double y3,
                         double *b0, double *b1, double *b2);
    double quadraticAt(const double b0, const double b1, const double b2,
                       const double x);

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

    // @todo better documentation
    double binningCritVal(unsigned int n, double uncertainty); // critical value for deciding if a bin exists or not

    float *minVal(float *const arrayStart, const size_t length);
    const float *minVal(const float *const arrayStart, const size_t length);
    double *minVal(double *const arrayStart, const size_t length);
    const double *minVal(const double *const arrayStart, const size_t length);

    float *maxVal(float *const arrayStart, const size_t length);
    const float *maxVal(const float *const arrayStart, const size_t length);
    double *maxVal(double *const arrayStart, const size_t length);
    const double *maxVal(const double *const arrayStart, const size_t length);

    double meanOfCumulative(double *const cumArray, const size_t startIdx, const size_t endIdx);
    double meanOfCumulative(const double *const cumArray, const size_t startIdx, const size_t endIdx);

    double sdev(double *const array, const size_t n);
    double sdev(const double *const array, const size_t n);
}
#endif // QALGORITHMS_UTILS_H