/* Library libcerf:
 *   compute complex error functions,
 *   along with Dawson, Faddeeva and Voigt functions
 *
 * File defs.h:
 *   Language-dependent includes.
 *
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013 Forschungszentrum Jülich GmbH
 *
 * Licence:
 *   MIT Licence.
 *   See ../COPYING
 *
 * Authors:
 *   Steven G. Johnson, Massachusetts Institute of Technology, 2012
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013, 2025
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 */

#define CERF_IEEE754
#ifdef CERF_IEEE754 // This flag can be set via CMake option -DCERF_IEEE754=ON

#if defined ENDIAN_IS_LITTLE
#elif defined(__cplusplus) && __cplusplus >= 202002L
#include <bit>
#define ENDIAN_IS_LITTLE (std::endian::native == std::endian::little)
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#define ENDIAN_IS_LITTLE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#elif defined(_WIN32)
#define ENDIAN_IS_LITTLE 1
#else
#error "Cannot determine endianness: provide -DENDIAN_IS_LITTLE=0|1 or switch off CERF_IEEE754"
#endif

//! Simpler replacement for frexp from math.h, assuming that 0 < value < inf.
//!
//! Adapted from https://github.com/dioptre/newos/blob/master/lib/libm/arch/sh4/frexp.c.
//! However, the mantissa must _not_ be broken into two variables to prevent errors
//! on architectures like MIPS that do not revert the byte order of simple types.
inline double frexp2(double value, int *eptr)
{
  union
  {
    double v;
    struct
    {
#if ENDIAN_IS_LITTLE
      unsigned long long mantissa : 52;
      unsigned long long exponent : 11;
      unsigned long long sign : 1;
#else
      unsigned long long sign : 1;
      unsigned long long exponent : 11;
      unsigned long long mantissa : 52;
#endif
    } s;
  } u;

  u.v = value;
  *eptr = u.s.exponent - 1022;
  u.s.exponent = 1022;
  return u.v;
}
#else
// Fall back to frexp from math.h. To be used for non-standard processor architectures
// for which our accelerated function frexp2 does not work.
#define frexp2 frexp
#endif
