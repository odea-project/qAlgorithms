/*
 * File width.c:
 *   Compute voigt_hwhm, half width at half maximum of the Voigt profile,
 *   using power series (Wuttke, J. Numer. Anal. Approx. Theo. 54, 345-356 (2025))
 *   and hard-coded Chebyshev approximations.

 *   Modified from code originally written for gnuplot.
 *
 * Copyright:
 *   Joachim Wuttke 2021, 2025
 *
 * License:
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifdef __cplusplus
#include <cassert>
#include <cmath>
using std::isnan;
#else
#include <assert.h>
#include <math.h>
#endif
#include "cerf.h"
#include "defs.h" // defines frexp2

#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131E-16
#endif

#include "auto_cheb_voigt_hwhm.c"

//! Returns polynomial approximation to f(x), using auto-tabulated expansion coefficients.
//! Code taken from https://jugit.fz-juelich.de/mlz/ppapp.

static double chebApproximant(double x)
{
    // Application-specific constants:
    static const int loff = (ppapp_j0 + 1) * (1 << ppapp_M) + ppapp_l0; // precomputed offset

    // For given x, obtain mantissa xm and exponent je:
    int je;                           // will be set in next line
    const double xm = frexp2(x, &je); // sets xm and je

    // Integer arithmetics to obtain reduced coordinate t:
    const int ip = (int)((1 << (ppapp_M+1)) * xm);   // index in octave + 2^M
    const int lij = je * (1 << ppapp_M) + ip - loff; // index in lookup table
    const double t = (1 << (ppapp_M+2)) * xm - (1 + 2*ip);

    const double *const P = ppapp_Coeffs0 + lij*8;
    return ((((((P[0] * t
                 + P[1]) * t
                + P[2]) * t
               + P[3]) * t
              + P[4]) * t
             + P[5]) * t
            + P[6]) * t
        + P[7];
}

double voigt_hwhm(double sigma, double gamma)
{
    if (sigma==0 && gamma==0)
        return 0;
    if (isnan(sigma) || isnan(gamma))
        return NAN;

    /* Reduce order of magnitude to prevent overflow */
    double scale = 1.;
    double s = fabs(sigma);
    double g = fabs(gamma);
    while (s>0x1p+320 || g>0x1p+320) {
        scale *= 0x1p+80;
        s *= 0x1p-80;
        g *= 0x1p-80;
    }

    /* Increase order of magnitude to prevent underflow */
    while (s<0x1p-320 && g<0x1p-320) {
        scale *= 0x1p-80;
        s *= 0x1p+80;
        g *= 0x1p+80;
    }

    if (16*s <= g) { // near-Lorentz asymptotic series
        const double q = pow(s / g, 2);
        return ((((((((((((
                              +0x0.bd231325e1e888p34 ) * q //  11
                          -0x0.8eb6c90ddf5b00p30 ) * q //  10
                         +0x0.ed458b66488000p25 ) * q //   9
                        -0x0.db9670afd00000p21 ) * q //   8
                       +0x0.e5e11df0000000p17 ) * q //   7
                      -0x0.8a67a100000000p14 ) * q //   6
                     +0x0.c1d34000000000p10 ) * q //   5
                    -0x0.a2b40000000000p7  ) * q //   4
                   +0x0.b7000000000000p4  ) * q //   3
                  -0x0.a8000000000000p2  ) * q //   2
                 +0x0.c0000000000000p1  ) * q //   1
                +0x0.80000000000000p1  ) * g //   0
            * scale;
    }

    const double x = g / s;
    if (x < .25) { // near-Gaussian expansion
        return (((((((((((((((
                                 +0x0.ef1d351e9acd20p-30) * x   //  13
                             -0x0.9068391094ed20p-25) * x  //  12
                            -0x0.f427c8d4b85470p-25) * x  //  11
                           +0x0.b5b27cc9c83e90p-22) * x   //  10
                          +0x0.decbb1c3bbc110p-21) * x   //   9
                         -0x0.9db642914088d0p-19) * x  //   8
                        -0x0.c64ecc686be2a0p-17) * x  //   7
                       -0x0.ac725003142298p-19) * x  //   6
                      +0x0.ede970db9e7240p-13) * x   //   5
                     +0x0.c667a57f0ca408p-12) * x   //   4
                    -0x0.afd312902207a8p-8 ) * x  //   3
                   -0x0.d130cfb7698988p-8 ) * x  //   2
                  +0x0.c4ffa59799f5a0p-3 ) * x   //   1
                 +0x0.885502093bf608p0  ) * g   //   0
                +0x0.96b55f2257e218p1   *  s)    //  -1
            * scale;
    }

    return s * chebApproximant(x) * scale;
}
