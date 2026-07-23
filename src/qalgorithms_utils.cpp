#include "qalgorithms_utils.h"
#include <cstddef>
#include <cstdint>
#define _USE_MATH_DEFINES
#include "cephes.h"
#include <cassert>
#include <math.h>

#include <unordered_map>

namespace qAlgorithms
{
    bool t_test_welch(const double mean_1, const double sd_1, const double count_1,
                      const double mean_2, const double sd_2, const double count_2,
                      const float alpha)
    {
        assert(abs(alpha - 0.05) < 0.0001); // @todo this is suboptimal, we should return the p-value
        // source: https://doi.org/10.1093/beheco/ark016
        // H0: mean_1 is equal to mean_2

        double t = (mean_1 - mean_2) / sqrt(sd_1 / count_1 + sd_2 / count_2);
        // the degrees of freedom are a non-integer value and rounded down before comparison
        double u = (sd_2 * sd_2) / (sd_1 * sd_1);
        double n1 = count_1 * count_1 * (count_1 - 1);
        double n2 = count_2 * count_2 * (count_2 - 1);
        size_t df = size_t((1 / count_1 + u / count_2) * (1 / count_1 + u / count_2) / (1 / n1 + u * u / n2));

        double t_refval = T_VALUES[df]; // @todo support more alphas than 0.05 - maybe return the p-value instead? Also support one / two sided
        bool reject_H0 = t > t_refval;
        return reject_H0;
    }

    bool F_test_regs(const double RSS_complex, const double RSS_simple,
                     const size_t params_complex, const size_t params_simple,
                     const size_t n, const double alpha)
    {
        const double fval = F_value(RSS_complex, RSS_simple, params_complex, params_simple, n);
        const double F_refval = F_stat(alpha, params_complex, params_simple, n);
        const bool f_ok = fval > F_refval; // reject H0, complex regression is significantly better than simple model
        return f_ok;
    }

    static size_t hashm(uint32_t a, uint32_t b)
    {
        size_t a2 = (size_t)a;
        a2 = a2 << __INT_WIDTH__;
        return a2 | (size_t)b;
    }
    // this global hashmap is used to avoid recalculating the f value every time - better, thread-safe solution possible? @todo
    static std::unordered_map<size_t, double> global_fhash_5perc;

    double F_stat(double alpha, size_t params_complex, size_t params_simple, size_t numPoints)
    {
        assert(params_complex > params_simple);
        assert(alpha > 0);
        assert(alpha < 1);

        int dfn_int = params_complex - params_simple;
        int dfd_int = numPoints - params_complex;
        size_t key = hashm(dfn_int, dfd_int);
        (alpha = 0.05); // @todo generalise for all alpha
        {
            double Fhash = global_fhash_5perc[key];
            if (Fhash != 0) // this serves as a check for the key existing
                return Fhash;
        }

        double F = cephes::F_density(dfn_int, dfd_int, alpha);

#if false
        double F_old = 0; // return value
        {
            double q = alpha;
            double dfn = double(params_complex - params_simple); // numerator degrees of freedom
            double dfd = double(numPoints - params_complex);     // denominator degrees of freedom
            double p = 1 - alpha;                                // area of the covered distribution
            int which = 2;
            double bound = 0;
            int status = 1;
            cdff(&which, &p, &q, &F_old, &dfn, &dfd, &status, &bound); // library function, see https://people.math.sc.edu/Burkardt/cpp_src/cdflib/cdflib.html
            assert(status == 0);
        }
        assert(float(F) == float(F_old));
#endif

        global_fhash_5perc[key] = F;

        return F;
    }

    double F_value(const double RSS_complex, const double RSS_simple,
                   const double params_complex, const double params_simple,
                   const size_t n)
    {
        // Calculate F value of two models by their residual sum of squares (RSS) and number of regression
        // parameters (params). H0 is the model being compared against. n is the number of real points
        // both regressions are applied to. Refer to https://en.wikipedia.org/wiki/F-test#Regression_problems
        assert(params_complex > params_simple);
        assert(RSS_complex > 0);
        assert(RSS_simple > 0);
        assert(n > 1);
        double RSS_ratio = (RSS_simple - RSS_complex) / RSS_complex;
        double params_ratio = ((double)n - params_complex) / (params_complex - params_simple);
        return RSS_ratio * params_ratio;
    }

    void linReg_intx(const float *yvals,
                     const size_t length,
                     double *slope, double *intercept)
    {
        assert(length > 2); // regression through two points is nonsensical;

        double s = 0, s_x = 0, s_y = 0, s_xx = 0, s_xy = 0; // accumulators for the different sums

        s = double(length);
        for (size_t x = 0; x < length; x++)
        {
            double y = yvals[x];

            s_x += double(x);
            s_y += y;
            s_xx += double(x * x);
            s_xy += double(x) * y;
        }

        double delta = 1 / (s * s_xx - s_x * s_x);
        *intercept = (s_xx * s_y - s_x * s_xy) * delta;
        *slope = (s * s_xy - s_x * s_y) * delta;
    }

    void weightedLinReg(const double *xvals,
                        const double *yvals,
                        const double *variance,
                        const size_t length,
                        double *slope, double *intercept)
    {
        /*
            The weighted linear regression as discussed in chapter 15.2 of numerical recepies (ISBN 0-521-43108-5).
            Variable names are taken from the text, not the included code example. The control for significance
            using the gamma function is skipped since there should always be a significant correlation. This could be
            replaced by a check against the mse when just using the mean.
        */
        assert(length > 2); // regression through two points is nonsensical;

        double s = 0, s_x = 0, s_y = 0, s_xx = 0, s_xy = 0; // accumulators for the different sums

        if (variance == nullptr)
        {
            s = double(length);
            for (size_t i = 0; i < length; i++)
            {
                double y = yvals[i];
                double x = xvals[i];

                s_x += x;
                s_y += y;
                s_xx += x * x;
                s_xy += x * y;
            }
        }
        else
        {
            for (size_t i = 0; i < length; i++)
            {
                double inverse = 1 / (variance[i] * variance[i]);

                double y = yvals[i];
                double x = xvals[i];

                s += inverse;
                s_x += x * inverse;
                s_y += y * inverse;
                s_xx += x * x * inverse;
                s_xy += x * y * inverse;
            }
        }

        // now, the two linear systems a * s + b * s_x = s_y and a * s_x + b * s_xx = s_xy can be solved for a and b
        // a is the inclination and b the intercept

        double delta = 1 / (s * s_xx - s_x * s_x);
        *intercept = (s_xx * s_y - s_x * s_xy) * delta;
        *slope = (s * s_xy - s_x * s_y) * delta;

        // calculate the uncertainties of a and b @todo
    }

    void coeffsQuadratic(const double x1, const double x2, const double x3,
                         const double y1, const double y2, const double y3,
                         double *b0, double *b1, double *b2)
    {
        // y1 = b2 x1^2 + b1 * x1 + b0 etc.
        const double div_x_23 = 1 / (x2 - x3);
        const double x22 = x2 * x2;
        const double x33 = x3 * x3;
        *b2 = ((y1 - y2) - (y2 - y3) * (x1 - x2) * div_x_23) / ((x1 * x1 - x22) - (x22 - x33) * (x1 - x2) * div_x_23);
        *b1 = (y2 - y3 + *b2 * (x33 - x22)) * div_x_23;
        *b0 = y3 - x33 * *b2 - x3 * *b1;
    }

    double quadraticAt(const double b0, const double b1, const double b2,
                       const double x)
    {
        return b0 + (b1 + x * b2) * x;
    }

    int solveQuadratic(const double a, const double b, const double c,
                       double *x1, double *x2)
    {
        // reference: https://www.av8n.com/physics/quadratic-formula.htm
        double sign = b <= 0 ? -1 : 1;
        double root = b * b - 4 * a * c;
        bool invalid = root <= 0;
        if (invalid)
        {
            *x1 = INFINITY;
            *x2 = INFINITY;
            return 1;
        }
        double x_big = (-b - sign * sqrt(root)) / (2 * a);
        double x_sml = c / (a * x_big);

        *x1 = min(x_sml, x_big);
        *x2 = max(x_sml, x_big);

        return 0;
    }

    // critical order space of two normally distributed populations
    double binningCritVal(const size_t n, const double stdDev)
    {
        // these values are determined empirically (see https://github.com/GeRe87/OS_critVal)
        const double OS_CRIT_A = 0.1443340625173891;
        const double OS_CRIT_B = 3.2412322699344687;
        return (OS_CRIT_A + (OS_CRIT_B / sqrt(log1p(n)))) * stdDev; // log1p(x) = log(x + 1)
    }

    size_t min(size_t a, size_t b)
    {
        return a < b ? a : b;
    }
    size_t max(size_t a, size_t b)
    {
        return a > b ? a : b;
    }
    uint16_t min(uint16_t a, uint16_t b)
    {
        return a < b ? a : b;
    }
    uint16_t max(uint16_t a, uint16_t b)
    {
        return a > b ? a : b;
    }
    int min(int a, int b)
    {
        return a < b ? a : b;
    }
    int max(int a, int b)
    {
        return a > b ? a : b;
    }
    double min(double a, double b)
    {
        return a < b ? a : b;
    }
    double max(double a, double b)
    {
        return a > b ? a : b;
    }

    float *minVal(float *const arrayStart, const size_t length)
    {
        assert(length > 0);
        float *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret < *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }
    const float *minVal(const float *const arrayStart, const size_t length)
    {
        assert(length > 0);
        const float *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret < *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }
    double *minVal(double *const arrayStart, const size_t length)
    {
        assert(length > 0);
        double *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret < *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }
    const double *minVal(const double *const arrayStart, const size_t length)
    {
        assert(length > 0);
        const double *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret < *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }

    float *maxVal(float *const arrayStart, const size_t length)
    {
        assert(length > 0);
        float *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret > *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
    }
    const float *maxVal(const float *const arrayStart, const size_t length)
    {
        assert(length > 0);
        const float *ret = arrayStart;
        for (size_t i = 1; i < length; i++) // no need to check the first element
        {
            ret = *ret > *(arrayStart + i) ? ret : arrayStart + i;
        }
        return ret;
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

    double meanOfCumulative(const double *const cumArray, const size_t startIdx, const size_t endIdx)
    {
        assert(startIdx <= endIdx);
        double subtractor = startIdx == 0 ? 0 : cumArray[startIdx - 1]; // account for index 0
        double totalSum = cumArray[endIdx] - subtractor;
        return totalSum / (endIdx - startIdx + 1);
    }

    unsigned int sumOfCumulative(const uint16_t *const cumArray, const size_t startIdx, const size_t length)
    {
        // if the cumulative array does not exist (== null), assume that
        // the toal df is the length
        if (cumArray == nullptr)
            return length;

        // it is assumed that the range does not violate array bounds
        unsigned int subtractor = startIdx == 0 ? 0 : cumArray[startIdx - 1];
        unsigned int totalSum = (cumArray[length + startIdx - 1] - subtractor);
        return totalSum;
    }

    double sdev(const double *const array, const size_t n)
    {
        assert(n > 2); // while standard deviation of two numbers is possible, it makes no sense
        double mean = 0;
        double sdev = 0;
        for (size_t i = 0; i < n; i++)
        {
            mean += array[i];
        }
        mean /= n;
        for (size_t i = 0; i < n; i++)
        {
            sdev += (array[i] - mean) * (array[i] - mean);
        }
        return sqrt(sdev / (n - 1));
    }

    double calcJaccardIdx(const float *const array1, const float *const array2, const size_t length)
    {
        // the jaccard index is defined as the intersection of two shapes divided by the union
        // The shapes are the two peak profiles here
        // We assume that both share the same x-axis, in which case every point can
        // be assumed to be at a distance of 1 to every other point.
        double intersect = 0;
        double union_val = 0;

        // calculate area as mean(array1[1], array[2]) * x etc. -> sum(array1) - array1[0] - array1[length - 1]
        for (size_t i = 0; i < length; i++)
        {
            double a1 = array1[i];
            double a2 = array2[i];
            intersect += min(a1, a2);
            union_val += max(a1, a2);
        }
        size_t end = length - 1;
        intersect -= (min(array1[0], array2[0]) + min(array1[end], array2[end])) / 2;
        union_val -= (max(array1[0], array2[0]) + max(array1[end], array2[end])) / 2;

        return intersect / union_val;
    }

    void clampSorted(const float *const array,
                     size_t *const startIdx,
                     size_t *const length,
                     const float lim_lower,
                     const float lim_upper)
    {
        assert(array != nullptr);
        assert(lim_lower < lim_upper);
        assert(array[*startIdx] < array[*length - 1]);

        *startIdx = 0;
        for (; *startIdx < *length; *startIdx += 1) // @todo use SIMD-friendly loop conditions
        {
            if (array[*startIdx] > lim_lower)
                break;
        }

        size_t newLength = *startIdx;
        for (; newLength < *length; newLength++)
        {
            if (array[newLength] > lim_upper)
                break;
        }
        newLength -= *startIdx;
        *length = newLength;
    }
} // namespace qAlgorithms