/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File w_of_z.c:
 *   Computation of Faddeeva's complex scaled error function,
 *      w(z) = exp(-z^2) * erfc(-i*z),
 *   nameless function (7.1.3) of Abramowitz&Stegun (1964),
 *   also known as the plasma dispersion function.
 *
 *   This implementation uses a combination of different algorithms.
 *   See man 3 w_of_z for references.
 *
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013 Forschungszentrum Jülich GmbH
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
 *   ../CHANGELOG
 *
 * Man page:
 *   w_of_z(3)
 */

/*
  Computes various error functions (erf, erfc, erfi, erfcx),
  including the Dawson integral, in the complex plane, based
  on algorithms for the computation of the Faddeeva function
  w(z) = exp(-z^2) * erfc(-i*z).
  Given w(z), the error functions are mostly straightforward
  to compute, except for certain regions where we have to
  switch to Taylor expansions to avoid cancellation errors
  [e.g. near the origin for erf(z)].

*/

#include "cerf.h"
#include "defs.h" // defines _cerf_cmplx, NaN, C, cexp, ...
#include <float.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#ifdef CERF_INTROSPECT
EXPORT int cerf_algorithm;
EXPORT int cerf_nofterms;
#endif

#include "w_taylor_tiles.c" // -> static const int Tiles[]
#include "w_taylor_coeffs.c"  // -> static const int NTay; static const double TaylorCoeffs[]

/******************************************************************************/
/*  w_of_z, Faddeeva's scaled complex error function                          */
/******************************************************************************/

_cerf_cmplx w_of_z(_cerf_cmplx z) {

    const double x = creal(z);
    const double xa = fabs(x);
    const double y = cimag(z);
    const double ya = fabs(y);
    const double z2 = xa*xa + y*y;
    const double ispi = 0.5641895835477562869; // 1 / sqrt(pi)

// ------------------------------------------------------------------------------
// Case |y| << |x|
// ------------------------------------------------------------------------------

//   If |y| << |x|, we get |Re w| << |Im w|, which implies that an algorithm
//   can be very accurate in terms of the complex norm, with relative error
//   computed as |w - w_0| / |w_0|, and still Re w can be wrong by orders of
//   magnitude. Therefore this special case is treated beforehand.

//   We start from the real axis where w(x) = exp(-x^2) + i*im_w_of_x(x).
//   To compute w(x+iy) we assume that |y| is negligible when added to |x|.
//   We obtain Re w = exp(-x^2) + 2 y (x Im w(x) - 1 / sqrt(pi)).

    if (ya < 1e-9 * xa) {
	const double wi = im_w_of_x(x);
        const double e2 = xa > 27. ? 0. : exp(-xa*xa); // prevent underflow
	if (ya == 0)
	    return C(e2, wi); // also works for x=+-inf
	return C(e2 + y*2*(x*wi - ispi), wi - 2*x*y*e2);
    }

// ------------------------------------------------------------------------------
// Case |x| << |y|
// ------------------------------------------------------------------------------

//   If |x| << |y|, we get |Im w| << |Re w|, which implies that an algorithm
//   can be very accurate in terms of the complex norm, with relative error
//   computed as |w - w_0| / |w_0|, and still Im w can be wrong by orders of
//   magnitude. Therefore this special case is treated beforehand.

//   We start from the imaginary axis where w(iy) = erfcx(y).
//   To compute w(iy+x) we assume that |x| is negligible when added to |y|.
//   We obtain Im w = 2 x (1 / sqrt(pi) - y erfcx(y)).

    if (xa < 1e-9 * ya) {
	const double wr = erfcx(y);
	if (xa == 0)
	    return C(wr, 0); // also works for y=+inf
	return C(wr, x*(2*(ispi - y*wr)));
    }

// ------------------------------------------------------------------------------
// Case |z| -> 0: Maclaurin series
// ------------------------------------------------------------------------------

    if (z2 < .053) {
        if (z2 < .00689) {
            if (z2 < 4e-7) {
                return ((((
                              + C(+5.0000000000000000e-01, 0) ) * z // z^4
                          + C(0, -7.5225277806367508e-01) ) * z // z^3
                         + C(-1.0000000000000000e+00, 0) ) * z // z^2
                        + C(0, +1.1283791670955126e+00) ) * z // z^1
                    + 1.;
            }

            return (((((((((((((
                                   + C(0, +5.3440090793734269e-04) ) * z // z^13
                               + C(+1.3888888888888889e-03, 0) ) * z // z^12
                              + C(0, -3.4736059015927274e-03) ) * z // z^11
                             + C(-8.3333333333333332e-03, 0) ) * z // z^10
                            + C(0, +1.9104832458760001e-02) ) * z // z^9
                           + C(+4.1666666666666664e-02, 0) ) * z // z^8
                          + C(0, -8.5971746064419999e-02) ) * z // z^7
                         + C(-1.6666666666666666e-01, 0) ) * z // z^6
                        + C(0, +3.0090111122547003e-01) ) * z // z^5
                       + C(+5.0000000000000000e-01, 0) ) * z // z^4
                      + C(0, -7.5225277806367508e-01) ) * z // z^3
                     + C(-1.0000000000000000e+00, 0) ) * z // z^2
                    + C(0, +1.1283791670955126e+00) ) * z // z^1
                + 1.;
        }

	return (((((((((((((((((((
				   + C(0, -8.8239572002038009e-07) ) * z // z^19
				 + C(-2.7557319223985893e-06, 0) ) * z // z^18
				+ C(0, +8.3827593401936105e-06) ) * z // z^17
			       + C(+2.4801587301587302e-05, 0) ) * z // z^16
			      + C(0, -7.1253454391645692e-05) ) * z // z^15
			     + C(-1.9841269841269841e-04, 0) ) * z // z^14
			    + C(0, +5.3440090793734269e-04) ) * z // z^13
			   + C(+1.3888888888888889e-03, 0) ) * z // z^12
			  + C(0, -3.4736059015927274e-03) ) * z // z^11
			 + C(-8.3333333333333332e-03, 0) ) * z // z^10
			+ C(0, +1.9104832458760001e-02) ) * z // z^9
		       + C(+4.1666666666666664e-02, 0) ) * z // z^8
		      + C(0, -8.5971746064419999e-02) ) * z // z^7
		     + C(-1.6666666666666666e-01, 0) ) * z // z^6
		    + C(0, +3.0090111122547003e-01) ) * z // z^5
		   + C(+5.0000000000000000e-01, 0) ) * z // z^4
		  + C(0, -7.5225277806367508e-01) ) * z // z^3
		 + C(-1.0000000000000000e+00, 0) ) * z // z^2
		+ C(0, +1.1283791670955126e+00) ) * z // z^1
	    + 1.;
    }

// ------------------------------------------------------------------------------
// Case |z| -> infty: Asymptotic expansion
// ------------------------------------------------------------------------------

    if (z2 >= 49) {
	_cerf_cmplx ret = 0.; // return value
	const double xs = y < 0 ? -creal(z) : creal(z); // compute for -z if y < 0

	if (z2 > 4.6e16) {
	    // Scale to prevent overflow.
	    if (xa > ya) {
		const double yax = ya / xs;
		const double denom = 0.56418958354775629 / (xs + yax*ya);
		ret = C(denom*yax, denom);
	    } else if (isinf(ya)) {
		return ((isnan(xa) || y < 0) ? C(NaN, NaN) : C(0, 0));
	    } else {
		const double xya = xs / ya;
		const double denom = 0.56418958354775629 / (xya*xs + ya);
		ret = C(denom, denom*xya);
	    }

	} else {
            if (z2 > 120) {
		if (z2 > 28000) {
		    ret = ((((
				 + 1.0578554691520430e+00) / (z*z) // n=3
			     + 4.2314218766081724e-01) / (z*z) // n=2
			    + 2.8209479177387814e-01) / (z*z) // n=1
			   + 5.6418958354775628e-01) / C(ya,-xs); // n=0

		} else {
		    ret = ((((((((((((
					 + 3.7877040075087948e+06) / (z*z) // n=11
				     + 3.6073371500083758e+05) / (z*z) // n=10
				    + 3.7971970000088164e+04) / (z*z) // n=9
				   + 4.4672905882456671e+03) / (z*z) // n=8
				  + 5.9563874509942218e+02) / (z*z) // n=7
				 + 9.1636730015295726e+01) / (z*z) // n=6
				+ 1.6661223639144676e+01) / (z*z) // n=5
			       + 3.7024941420321507e+00) / (z*z) // n=4
			      + 1.0578554691520430e+00) / (z*z) // n=3
			     + 4.2314218766081724e-01) / (z*z) // n=2
			    + 2.8209479177387814e-01) / (z*z) // n=1
			   + 5.6418958354775628e-01) / C(ya,-xs); // n=0
		}

            } else {
                ret = (((((((((((((((((((((((
					      + 7.5846930433244938e+19) / (z*z) // n=22
					     + 3.5277642061974395e+18) / (z*z) // n=21
					   + 1.7208605883889949e+17) / (z*z) // n=20
					  + 8.8249260943025370e+15) / (z*z) // n=19
                                         + 4.7702303212446150e+14) / (z*z) // n=18
                                        + 2.7258458978540656e+13) / (z*z) // n=17
                                       + 1.6520278168812520e+12) / (z*z) // n=16
                                      + 1.0658243979879044e+11) / (z*z) // n=15
                                     + 7.3505130895717545e+09) / (z*z) // n=14
                                    + 5.4448245107938921e+08) / (z*z) // n=13
                                   + 4.3558596086351141e+07) / (z*z) // n=12
                                  + 3.7877040075087948e+06) / (z*z) // n=11
                                 + 3.6073371500083758e+05) / (z*z) // n=10
                                + 3.7971970000088164e+04) / (z*z) // n=9
                               + 4.4672905882456671e+03) / (z*z) // n=8
                              + 5.9563874509942218e+02) / (z*z) // n=7
                             + 9.1636730015295726e+01) / (z*z) // n=6
                            + 1.6661223639144676e+01) / (z*z) // n=5
                           + 3.7024941420321507e+00) / (z*z) // n=4
                          + 1.0578554691520430e+00) / (z*z) // n=3
                         + 4.2314218766081724e-01) / (z*z) // n=2
                        + 2.8209479177387814e-01) / (z*z) // n=1
                       + 5.6418958354775628e-01) / C(ya,-xs); // n=0
            }
        }
        if (y < 0) {
            // Use w(z) = 2.0*exp(-z*z) - w(-z),
            // but be careful of overflow in exp(-z*z) = exp(-(xs*xs-ya*ya) -2*i*xs*ya)
            return 2.0 * cexp(C((ya - xs) * (xs + ya), 2*xs*y)) - ret;
        } else
            return ret;
    }

// ------------------------------------------------------------------------------
// Taylor around some z
// ------------------------------------------------------------------------------

    if (isnan(xa))
	return C(xa, xa);

    if (isnan(y))
	return C(y, y);

    const int iTile = 2 * ((int)(inverseA*xa)*nXcover + (int)(inverseA*ya));
    const int kP = Tiles[iTile];
    assert(kP >= 0);
    const double* T = &TaylorCoeffs[2*(NTay+1)*kP];
    const _cerf_cmplx dz = C(xa - T[0], ya - T[1]);
    const int Nk = Tiles[iTile+1];
    _cerf_cmplx ret = C(T[2*Nk], T[2*Nk+1]);
    for (int k = Nk-1; k >= 1; --k)
	ret = ret * dz + C(T[2*k], T[2*k+1]);


    if (y < 0) {
	if (x < 0)
	    return 2.0 * cexp(C((y - x) * (x + y), -2*x*y)) - ret;
	return 2.0 * cexp(C((y - x) * (x + y), -2*x*y)) - C(creal(ret), -cimag(ret));
    }
    if (x < 0)
	ret = C(creal(ret), -cimag(ret));
    return ret;

} // w_of_z
