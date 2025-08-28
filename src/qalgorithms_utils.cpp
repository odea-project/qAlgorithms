#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include <cstdint> // uint64_t
#include <cmath>   // std::abs()
#include <cassert>
#include "../external/CDFlib/cdflib.hpp"

namespace qAlgorithms
{
    // @todo separate out the library functions into something better readable with less flexible error checking
    double cdflib_F_stat(double alpha, size_t params_complex, size_t params_simple, size_t numPoints)
    {
        // wrapper around the cdflib library function cdff with the correct presets
        // for calculating the area of F from 0 to 1 - alpha.
        assert(params_complex > params_simple);
        assert(alpha > 0);
        assert(alpha < 1);

        double F = 0; // return value

        int which = 2;

        double p = 1 - alpha; // area of the covered distribution
        double q = alpha;
        double dfn = double(params_complex - params_simple); // numerator degrees of freedom
        double dfd = double(numPoints - params_complex);     // denominator degrees of freedom

        int status = 1;   // result invalid if this is not 0
        double bound = 0; // allows recovery from non-0 status @todo

        cdff(&which, &p, &q, &F, &dfn, &dfd, &status, &bound); // library function, see https://people.math.sc.edu/Burkardt/cpp_src/cdflib/cdflib.html
        assert(status == 0);

        return F;
    }

    double f_value(const double RSS_model, const double RSS_H0,
                   const size_t params_model, const size_t params_H0,
                   const size_t n)
    {
        // Calculate F value of two models by their residual sum of squares (RSS) and number of regression
        // parameters (params). H0 is the model being compared against. n is the number of real points
        // both regressions are applied to. Refer to https://en.wikipedia.org/wiki/F-test#Regression_problems
        assert(params_model < params_H0);
        assert(RSS_model > 0);
        assert(RSS_H0 > 0);
        double RSS_ratio = (RSS_H0 - RSS_model) / RSS_model;
        double params_ratio = double(n - params_model) / double(params_model - params_H0);
        return RSS_ratio * params_ratio;
    }

    double exp_approx_d(const double x)
    {
        // assert(x > 0); // @todo this is specified in the header but not respected throughout the code
        // assert(x < 26);
        constexpr double LOG2E = 1.44269504088896340736;
        constexpr double OFFSET = 1022.9329329329329;
        constexpr uint64_t EXP_OFFSET = 1LL << 52;
        union
        {
            uint64_t i;
            double d;
        } v = {(uint64_t)((x * LOG2E + OFFSET) * EXP_OFFSET)};
        return v.d;
    }

    float erf_approx_f(const float x)
    {
        float sign = x < 0 ? -1.0f : 1.0f; // get sign as the approximation is only valid for positive x
        float t = std::abs(x);             // get the absolute value of x
        constexpr float a1 = 0.278393f;    // empirically determined
        constexpr float a2 = 0.230389f;    // empirically determined
        constexpr float a3 = 0.000972f;    // empirically determined
        constexpr float a4 = 0.078108f;    // empirically determined
        t = 1.0f + t * (a1 + t * (a2 + t * (a3 + t * a4)));
        t = t * t * t * t;               // t^4
        return sign * (1.0f - 1.0f / t); // return the final approximation
    }

    double dawson5(double x)
    {
        double y, p, q;
        y = x * x;
        p = 1.0 + y * (0.1049934947 + y * (0.0424060604 + y * (0.0072644182 + y * (0.0005064034 + y * (0.0001789971)))));
        q = 1.0 + y * (0.7715471019 + y * (0.2909738639 + y * (0.0694555761 + y * (0.0140005442 + y * (0.0008327945 + 2 * 0.0001789971 * y)))));
        return x * (p / q);
    }

    double experfc(double x, double sign) // @todo this was updated in dev_G - understand, change and sync it
    {
        constexpr double a = 0.978795604954049; // empirically determined
        constexpr double b = 1.25731022692317;  // empirically determined
        double t = -x * x;
        return SQRTPI_2 * exp_approx_d(t) + sign * a * x * exp_approx_d(t * b); // @todo t is always negative, which goes against the definition given in exp_approx_d
    }

    double erfi(const double x)
    {
        /* This function uses the Dawson Integral, i.e.
        erfi(x) = 2 * Dawson * exp(x^2) / sqrt(pi)

        The Dawson Integral is calculated by Stan Sykora's rational function approximations. URL : http://www.ebyte.it/library/codesnippets/DawsonIntegralApproximations.html (Dawson5())
        */
        // calculate the Dawson Integral:
        double y, p, q;
        y = x * x;
        p = 1.0 + y * (0.1049934947 + y * (0.0424060604 + y * (0.0072644182 + y * (0.0005064034 + y * (0.0001789971)))));
        q = 1.0 + y * (0.7715471019 + y * (0.2909738639 + y * (0.0694555761 + y * (0.0140005442 + y * (0.0008327945 + 2 * 0.0001789971 * y)))));
        double D = x * (p / q);

        return D * exp_approx_d(x * x);
    }

    // critical order space of two normally distributed populations
    double binningCritVal(unsigned int n, double stdDev)
    {
        return (OS_CRIT_A + (OS_CRIT_B / std::sqrt(std::log(n + 1)))) * stdDev;
    }

    double *maxVal(double *const arrayStart, const size_t length)
    {
        assert(length > 0);
        double *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret > *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }
    const double *maxVal(const double *const arrayStart, const size_t length)
    {
        assert(length > 0);
        const double *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret > *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }

    double meanOfCumulative(double *const cumArray, const size_t startIdx, const size_t endIdx)
    {
        assert(startIdx <= endIdx);
        double subtractor = startIdx == 0 ? 0 : cumArray[startIdx - 1]; // account for index 0
        double totalSum = cumArray[endIdx] - subtractor;
        return totalSum / (endIdx - startIdx + 1);
    }
    double meanOfCumulative(const double *const cumArray, const size_t startIdx, const size_t endIdx)
    {
        assert(startIdx <= endIdx);
        double subtractor = startIdx == 0 ? 0 : cumArray[startIdx - 1]; // account for index 0
        double totalSum = cumArray[endIdx] - subtractor;
        return totalSum / (endIdx - startIdx + 1);
    }

}