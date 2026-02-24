/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File erfcx.c:
 *   Compute erfcx(x) = exp(x^2) erfc(x) function, for real x,
 *   using a novel algorithm that is much faster than DERFC of SLATEC.
 *   This function is used in the computation of Faddeeva, Dawson, and
 *   other complex error functions.
 *
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013, 2025 Forschungszentrum Jülich GmbH
 *
 * Licence:
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
 * Authors:
 *   Steven G. Johnson, Massachusetts Institute of Technology, 2012
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013, 2025
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 *
 * Revision history:
 *   libcerf-2.5, April 2025:
 *      - For large |x|, continuous fractions replaced with asymptotic expansions.
 *      - Expanded the small |x| range where Taylor series is used.
 *      - Code for intermediate |x| entirely rewritten, using methods described in:
 *        Joachim Wuttke and Alexander Kleinsorge:
 *        "Code generation for piecewise Chebyshev approximation."
 *
 *   See also ../CHANGELOG
 *
 * Manual page:
 *   man 3 erfcx
 */

#include "cerf.h"
#include "defs.h" // defines frexp2
#include <math.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>

#include "auto_cheb_erfcx.c"

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
    const double *const Q = ppapp_Coeffs1 + lij*2;
    return ((((((((P[0] * t
                + P[1]) * t
               + P[2]) * t
              + P[3]) * t
             + P[4]) * t
            + P[5]) * t
           + P[6]) * t
          + P[7]) * t
         + Q[0]) * t
        + Q[1];
}

/******************************************************************************/
/*  Library function erfcx                                                    */
/******************************************************************************/

double erfcx(double x) {
    // Uses the following methods:
    // - asymptotic expansion for large positive x,
    // - Chebyshev polynomials for medium positive x,
    // - Taylor (Maclaurin) series for small |x|,
    // - 2*exp(x^2)-erfcx(-x) for medium negative x,
    // - Asymptote 2exp(x^2) for large negative x.

    double ax = fabs(x);

    if (ax < .125) {
        // Use Taylor expansion
        return ((((((((((((((+1.9841269841269841e-04) * x -
                            5.3440090793734269e-04) * x +
                           1.3888888888888889e-03) * x -
                          3.4736059015927274e-03) * x +
                         8.3333333333333332e-03) * x -
                        1.9104832458760001e-02) * x +
                       4.1666666666666664e-02) * x -
                      8.5971746064419999e-02) * x +
                     1.6666666666666666e-01) * x -
                    3.0090111122547003e-01) * x +
                   5.0000000000000000e-01) * x -
                  7.5225277806367508e-01) * x +
                 1.0000000000000000e+00) * x -
                1.1283791670955126e+00) * x +
            1.0000000000000000e+00;
    }

    if (x < 0) {
        if (x < -26.7)
            return HUGE_VAL;
        if (x < -6.1)
            return 2 * exp(x * x);
        return 2 * exp(x * x) - chebApproximant(-x);
    }

    if (x < 12)
        return chebApproximant(x);

    /* else */ {
        // Use asymptotic expansion
        //
        // Coefficient are a_0 = 1/sqrt(pi), a_N = (2N-1)!!/2^N/sqrt(pi).

        const double r = 1 / x;

        if (x < 150) {
            if (x < 23.2)
                return (((((((((((+3.6073371500083758e+05) * (r * r) -
                                 3.7971970000088164e+04) * (r * r) +
                                4.4672905882456671e+03) * (r * r) -
                               5.9563874509942218e+02) * (r * r) +
                              9.1636730015295726e+01) * (r * r) -
                             1.6661223639144676e+01) * (r * r) +
                            3.7024941420321507e+00) * (r * r) -
                           1.0578554691520430e+00) * (r * r) +
                          4.2314218766081724e-01) * (r * r) -
                         2.8209479177387814e-01) * (r * r) +
                        5.6418958354775628e-01) * r;
            return (((((((+9.1636730015295726e+01) * (r * r) -
                         1.6661223639144676e+01) * (r * r) +
                        3.7024941420321507e+00) * (r * r) -
                       1.0578554691520430e+00) * (r * r) +
                      4.2314218766081724e-01) * (r * r) -
                     2.8209479177387814e-01) * (r * r) +
                    5.6418958354775628e-01) * r;
        }
        if (x < 6.9e7)
            return ((((-1.0578554691520430e+00) * (r * r) +
                      4.2314218766081724e-01) * (r * r) -
                     2.8209479177387814e-01) * (r * r) +
                    5.6418958354775628e-01) * r;
        // 1-term expansion, important to avoid overflow
        return 0.56418958354775629 * r;
    }

} // erfcx
