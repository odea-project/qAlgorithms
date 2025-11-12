#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include <cstdint> // uint64_t
#include <cmath>   // std::abs()
#include <cassert>
#include "../external/CDFlib/cdflib.hpp"

namespace qAlgorithms
{

    void beta_inc_2(double *a, double *b, double *x, double *y, double *w,
                    double *w1)

    //****************************************************************************80
    //
    //  Purpose:
    //
    //    BETA_INC evaluates the incomplete beta function IX(A,B).
    //
    //  Author:
    //
    //    Alfred H Morris, Jr,
    //    Naval Surface Weapons Center,
    //    Dahlgren, Virginia.
    //
    //  Parameters:
    //
    //    Input, double *A, *B, the parameters of the function.
    //    A and B should be nonnegative.
    //
    //    Input, double *X, *Y.  X is the argument of the
    //    function, and should satisy 0 <= X <= 1.  Y should equal 1 - X.
    //
    //    Output, double *W, *W1, the values of IX(A,B) and
    //    1-IX(A,B).
    //
    {
        assert(*a > 0);
        assert(*b > 0);
        assert((0 < *x) && (*x < 1));
        assert((0 < *y) && (*y < 1));
        *w = *w1 = 0.0e0;

        static double a0, b0, eps, lambda, t, x0, y0, z;
        static int ierr1, n;
        static double T2, T3, T4, T5;
        //
        //  EPS IS A MACHINE DEPENDENT CONSTANT. EPS IS THE SMALLEST
        //  NUMBER FOR WHICH 1.0 + EPS .GT. 1.0
        //
        static int K1 = 1;
        eps = dpmpar(&K1);

        eps = max(eps, 1.e-15);
        if (max(*a, *b) < 1.e-3 * eps)
        {
            //
            //  PROCEDURE FOR A AND B .LT. 1.E-3*EPS
            //
            *w = *b / (*a + *b);
            *w1 = *a / (*a + *b);
            return;
        }
        bool ind = false;
        a0 = *a;
        b0 = *b;
        x0 = *x;
        y0 = *y;
        if (min(a0, b0) > 1.0e0)
        {
            //
            //  PROCEDURE FOR A0 .GT. 1 AND B0 .GT. 1
            //
            if (*a > *b)
            {
                lambda = (*a + *b) * *y - *b;
            }
            else
            {
                lambda = *a - (*a + *b) * *x;
            }
            if (lambda < 0.0e0)
            {
                ind = true;
                a0 = *b;
                b0 = *a;
                x0 = *y;
                y0 = *x;
                lambda = fabs(lambda);
            }
            if (b0 < 40.0e0 && b0 * x0 <= 0.7e0)
            {
                *w = beta_pser(&a0, &b0, &x0, &eps);
                *w1 = 0.5e0 + (0.5e0 - *w);
                if (ind)
                {
                    t = *w;
                    *w = *w1;
                    *w1 = t;
                }
                return;
            }
            if (b0 < 40.0e0)
            {
                goto S160;
            }
            if (a0 > b0)
            {
                if ((lambda > 0.03 * b0) || (b0 <= 100))
                {
                    goto S130;
                }

                T5 = 100.0e0 * eps;
                *w = beta_asym(&a0, &b0, &lambda, &T5);
                *w1 = 0.5e0 + (0.5e0 - *w);
                if (ind)
                {
                    t = *w;
                    *w = *w1;
                    *w1 = t;
                }
                return;
            }
            if ((a0 <= 100) || (lambda > 0.03e0 * a0))
            {
                goto S130;
            }

            T5 = 100.0e0 * eps;
            *w = beta_asym(&a0, &b0, &lambda, &T5);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        //
        //  PROCEDURE FOR A0 .LE. 1 OR B0 .LE. 1
        //
        if (*x > 0.5e0)
        {
            ind = true;
            a0 = *b;
            b0 = *a;
            x0 = *y;
            y0 = *x;
        }
        if (b0 < min(eps, eps * a0))
        {
            //
            //  EVALUATION OF THE APPROPRIATE ALGORITHM
            //
            *w = fpser(&a0, &b0, &x0, &eps);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        if (a0 < min(eps, eps * b0) && b0 * x0 <= 1)
        {
            *w1 = apser(&a0, &b0, &x0, &eps);
            *w = 0.5e0 + (0.5e0 - *w1);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        if (max(a0, b0) <= 1)
        {
            if ((a0 >= min(0.2, b0)) || (pow(x0, a0) <= 0.9))
            {
                *w = beta_pser(&a0, &b0, &x0, &eps);
                *w1 = 1 - *w;
                if (ind)
                {
                    t = *w;
                    *w = *w1;
                    *w1 = t;
                }
                return;
            }
            if (x0 >= 0.3e0)
            {
                *w1 = beta_pser(&b0, &a0, &y0, &eps);
                *w = 0.5e0 + (0.5e0 - *w1);
                if (ind)
                {
                    t = *w;
                    *w = *w1;
                    *w1 = t;
                }
                return;
            }
            n = 20;
            goto S140;
        }

        if (b0 <= 1.0e0)
        {
            *w = beta_pser(&a0, &b0, &x0, &eps);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        if (x0 >= 0.3e0)
        {
            *w1 = beta_pser(&b0, &a0, &y0, &eps);
            *w = 0.5e0 + (0.5e0 - *w1);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }

        if ((pow(x0 * b0, a0) <= 0.7e0) && x0 < 0.1)
        {
            *w = beta_pser(&a0, &b0, &x0, &eps);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        if (b0 > 15.0e0)
        {
            T3 = 15.0e0 * eps;
            beta_grat(&b0, &a0, &y0, &x0, w1, &T3, &ierr1);
            *w = 0.5e0 + (0.5e0 - *w1);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        n = 20;
        goto S140;

    S130:
        T2 = 15.0e0 * eps;
        *w = beta_frac(&a0, &b0, &x0, &y0, &lambda, &T2);
        *w1 = 0.5e0 + (0.5e0 - *w);
        if (ind)
        {
            t = *w;
            *w = *w1;
            *w1 = t;
        }
        return;
    S140:
        *w1 = beta_up(&b0, &a0, &y0, &x0, &n, &eps);
        b0 = b0 + (double)n;

    S160:
        n = (int)b0;
        b0 -= (double)n;
        if (b0 == 0.0e0)
        {
            n -= 1;
            b0 = 1.0e0;
        }
        *w = beta_up(&b0, &a0, &y0, &x0, &n, &eps);
        if (x0 <= 0.7e0)
        {
            *w = *w + beta_pser(&a0, &b0, &x0, &eps);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        if (a0 > 15.0e0)
        {
            T4 = 15.0e0 * eps;
            beta_grat(&a0, &b0, &x0, &y0, w, &T4, &ierr1);
            *w1 = 0.5e0 + (0.5e0 - *w);
            if (ind)
            {
                t = *w;
                *w = *w1;
                *w1 = t;
            }
            return;
        }
        n = 20;
        *w = *w + beta_up(&a0, &b0, &x0, &y0, &n, &eps);
        a0 = a0 + (double)n;
        return;

    flipIfInd:
    }

    void cumf_2(const double f, const double dfn, const double dfd,
                double *cum, double *ccum)

    //  Purpose:
    //
    //    CUMF evaluates the cumulative F distribution.
    //
    //  Discussion:
    //
    //    CUMF computes the integral from 0 to F of the F density with DFN
    //    numerator and DFD denominator degrees of freedom.
    //
    //  Reference:
    //
    //    Milton Abramowitz and Irene Stegun,
    //    Handbook of Mathematical Functions
    //    1966, Formula 26.5.28.
    //
    //  Parameters:
    //
    //    Input, double *F, the upper limit of integration.
    //
    //    Input, double *DFN, *DFD, the number of degrees of
    //    freedom for the numerator and denominator.
    //
    //    Output, double *CUM, *CCUM, the value of the F CDF and
    //    the complementary F CDF.
    //
    {
        assert(f > 0);

        //
        //     XX is such that the incomplete beta with parameters
        //     DFD/2 and DFN/2 evaluated at XX is 1 - CUM or CCUM
        //     YY is 1 - XX
        //     Calculate the smaller of XX and YY accurately
        //
        double prod = dfn * f;
        double dsum = dfd + prod;
        double xx = dfd / dsum;
        double yy;

        if (xx > 0.5)
        {
            yy = prod / dsum;
            xx = 1 - yy;
        }
        else
        {
            yy = 1 - xx;
        }

        int ierr;
        double T1 = dfd / 2;
        double T2 = dfn / 2;

        double T1a = dfd / 2;
        double T2a = dfn / 2;
        double xxa = xx;
        double yya = yy;

        double ccuma = 0;
        double cuma = 0;

        beta_inc(&T1, &T2, &xx, &yy, ccum, cum, &ierr);

        beta_inc_2(&T1a, &T2a, &xxa, &yya, &ccuma, &cuma);

        assert(T1 == T1a);
        assert(T2 == T2a);
        assert(xx == xxa);
        assert(yy == yya);
        assert(*ccum == ccuma);
        assert(*cum == cuma);

        return;
    }

    /*
        Calculates the F-value for a given set of alpha, the numerator
        degreees of freedom dfn and the denominator degrees of
        freedom dfd.

        returns an error code:
        0: Successful execution
        1: Search resulted in a value < 0
        2: Search exceeded the limit of e^300

        Function is adapted from CDFF, mostly removal of gotos and
        unnecessary functionality.
    */
    int calc_fval(double alpha, double dfn, double dfd,
                  double *f)
    {
        assert(alpha > 0 && alpha < 1);
        assert(dfn > 0);
        assert(dfd > 0);

        { // @todo find out if this call is actually necessary. Documentation implies it is, but this also works
            static double T3 = 1.0e300;
            static double T6 = (1.0e-50);
            static double T7 = (1.0e-8);
            static double K2 = 0.0e0;
            static double K4 = 0.5e0;
            static double K5 = 5.0e0;
            dstinv(&K2, &T3, &K4, &K4, &K5, &T6, &T7);
        }

        static double fx, cum, ccum;
        static unsigned long qhi, qleft;
        int status = 0;
        *f = 5.0e0;
        dinvr(&status, f, &fx, &qleft, &qhi);

        double p = 1 - alpha;

        while (status == 1)
        {
            cumf(f, &dfn, &dfd, &cum, &ccum);

            // select minimum of p or q
            fx = p <= alpha ? cum - p : ccum - alpha;

            dinvr(&status, f, &fx, &qleft, &qhi);
        }

        if (status != -1)
            return 0;

        if (qleft)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

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

        double f = 0;
        calc_fval(q, dfn, dfd, &f); // @todo does not work for all input params
        assert(float(f) == float(F));

        return F;
    }

    double f_value(const double RSS_model, const double RSS_H0,
                   const size_t params_model, const size_t params_H0,
                   const size_t n)
    {
        // Calculate F value of two models by their residual sum of squares (RSS) and number of regression
        // parameters (params). H0 is the model being compared against. n is the number of real points
        // both regressions are applied to. Refer to https://en.wikipedia.org/wiki/F-test#Regression_problems
        assert(params_model > params_H0);
        assert(RSS_model > 0);
        assert(RSS_H0 > 0);
        double RSS_ratio = (RSS_H0 - RSS_model) / RSS_model;
        double params_ratio = double(n - params_model) / double(params_model - params_H0);
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

    size_t min(size_t a, size_t b)
    {
        return a < b ? a : b;
    }
    size_t max(size_t a, size_t b)
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

    double sdev(double *const array, const size_t n)
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
}