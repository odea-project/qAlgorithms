/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File cerf.h:
 *   Declare exported functions.
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
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 *
 * Revision history:
 *   ../CHANGELOG
 *
 * Man pages:
 *   w_of_z(3), dawson(3), voigt(3), cerf(3), erfcx(3), erfi(3)
 */


#ifndef __CERF_H
#  define __CERF_H

#  ifdef __cplusplus
#    include <complex>
     typedef std::complex<double> _cerf_cmplx;
#  else
#    include <complex.h>
#    if _WIN32
#      error Compilation under C not supported by MSVC (arithmetic operators missing)
#    endif
     typedef double complex _cerf_cmplx;
#  endif

#if _WIN32
#  define EXPORT __declspec(dllexport)
#  define IMPORT __declspec(dllimport)
#else
#  define EXPORT
#  define IMPORT
#endif

// compute w(z) = exp(-z^2) erfc(-iz), Faddeeva's scaled complex error function
EXPORT _cerf_cmplx w_of_z(_cerf_cmplx z);

EXPORT double im_w_of_x(double x); // special case Im[w(x)] of real x
EXPORT double re_w_of_z(double x, double y);
EXPORT double im_w_of_z(double x, double y);

// compute erf(z), the error function of complex arguments
EXPORT _cerf_cmplx cerf(_cerf_cmplx z);

// compute erfc(z) = 1 - erf(z), the complementary error function
EXPORT _cerf_cmplx cerfc(_cerf_cmplx z);

// compute erfcx(z) = exp(z^2) erfc(z), an underflow-compensated version of erfc
EXPORT _cerf_cmplx cerfcx(_cerf_cmplx z);
EXPORT double erfcx(double x); // special case for real x

// compute erfi(z) = -i erf(iz), the imaginary error function
EXPORT _cerf_cmplx cerfi(_cerf_cmplx z);
EXPORT double erfi (double x); // special case for real x

// compute dawson(z) = sqrt(pi)/2 * exp(-z^2) * erfi(z), Dawson's integral
EXPORT _cerf_cmplx cdawson(_cerf_cmplx z);
EXPORT double dawson(double x); // special case for real x

// compute voigt(x,...), the convolution of a Gaussian and a Lorentzian
EXPORT double voigt(double x, double sigma, double gamma);
// compute the full width at half maximum of the Voigt function
EXPORT double voigt_hwhm(double sigma, double gamma);

#endif /* __CERF_H__ */
