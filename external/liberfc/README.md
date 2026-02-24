# libcerf

This is the home page of **libcerf**, a self-contained numeric library that provides an efficient and accurate implementation of complex error functions, along with Dawson, Faddeeva, and Voigt functions.

# User Documentation

## Synopsis

In the following, `complex` stands for either the C data type `double complex` from <complex.h> or the C++ data type `std::complex<double>` from <complex>.

  * complex [cerf](http://apps.jcns.fz-juelich.de/man/cerf.html) (complex): The complex error function erf(z).
  * complex [cerfc](http://apps.jcns.fz-juelich.de/man/cerf.html) (complex): The complex complementary error function erfc(z) = 1 - erf(z).
  * complex [cerfcx](http://apps.jcns.fz-juelich.de/man/erfcx.html) (complex z): The underflow-compensating function erfcx(z) = exp(z^2) erfc(z).
  * double [erfcx](http://apps.jcns.fz-juelich.de/man/erfcx.html) (double x): The same for real x.
  * complex [cerfi](http://apps.jcns.fz-juelich.de/man/erfi.html) (complex z): The imaginary error function erfi(z) = -i erf(iz).
  * double [erfi](http://apps.jcns.fz-juelich.de/man/erfi.html) (double x): The same for real x.
  * complex [w_of_z](http://apps.jcns.fz-juelich.de/man/w_of_z.html) (complex z): Faddeeva's scaled complex error function w(z) = exp(-z^2) erfc(-iz).
  * double [im_w_of_x](http://apps.jcns.fz-juelich.de/man/w_of_z.html) (double x): The same for real x, returning the purely imaginary result as a real number.
  * complex [cdawson](http://apps.jcns.fz-juelich.de/man/dawson.html) (complex z): Dawson's integral D(z) = sqrt(pi)/2 * exp(-z^2) * erfi(z).
  * double [dawson](http://apps.jcns.fz-juelich.de/man/dawson.html) (double x): The same for real x.
  * double [voigt](http://apps.jcns.fz-juelich.de/man/voigt.html) (double x, double sigma, double gamma): The convolution of a Gaussian and a Lorentzian.
  * double [voigt_hwhm](http://apps.jcns.fz-juelich.de/man/voigt_hwhm.html) (double sigma, double gamma): The half width at half maximum of the Voigt profile.

## Accuracy

By construction, it is expected that the relative accuracy is generally better than 1E-13. This has been confirmed by comparison with high-precision Maple computations and with a *long double* computation using Fourier transform representation and double-exponential transform.

Accuracy of the real functions erfcx and im_w_of_x has been greatly improved in release 2.5, it is now 1E-15 or better.

## Copyright and Citation

Copyright (C) [Steven G. Johnson](http:*math.mit.edu/~stevenj), Massachusetts Institute of Technology, 2012; [Joachim Wuttke](http:*www.fz-juelich.de/SharedDocs/Personen/JCNS/EN/Wuttke_J.html), Forschungszentrum Jülich, 2013.

License: [MIT License](http://opensource.org/licenses/MIT)

When using libcerf in scientific work, please cite as follows:
  * S. G. Johnson, J. Wuttke: libcerf, numeric library for complex error functions, version [...], http://apps.jcns.fz-juelich.de/libcerf
If appropriate cite also the authors of language bindings or binary packages.

Please send bug reports to the authors, or submit them through the Gitlab issue tracker.

## Further references

Most function evaluations in this library rely on Faddeeva's function w(z).

This function has been reimplemented from scratch by [Steven G. Johnson](http://math.mit.edu/~stevenj);
project web site http://ab-initio.mit.edu/Faddeeva. The implementation partly relies on algorithms from the following publications:
  * Walter Gautschi, *Efficient computation of the complex error function,* SIAM J. Numer. Anal. 7, 187 (1970).
  * G. P. M. Poppe and C. M. J. Wijers, *More efficient computation of the complex error function,* ACM Trans. Math. Soft. 16, 38 (1990).
  * Mofreh R. Zaghloul and Ahmed N. Ali, *Algorithm 916: Computing the Faddeyeva and Voigt Functions,* ACM Trans. Math. Soft. 38, 15 (2011).

# Installation

## From source

Build&install are based on CMake. Out-of-source build is enforced.
After unpacking the source, go to the source directory and do:

  mkdir build
  cd build
  cmake .. [CERF_C=OFF] [CERF_CPP=OFF]
  make
  ctest
  make install

This process builds two libraries that represent the application binary interface (ABI) for
the programming languages C and C++:
* libcerf (C ABI),
* libcerfcpp (C++ ABI).
Two disable one of them, use one of the `cmake` options indicated above.

Support for complex numbers by Microsoft Visual C is so deficient that
compilation of libcerf as C is not possible.
Therefore, under Visual Studio the option `CERF_C=OFF` is obligatory.

## Binary packages

  * Linux:
    * [Debian package](https://buildd.debian.org/status/package.php?p=bornagain) by Eugen Wintersberger
    * [Gentoo package](http://packages.gentoo.org/package/sci-libs/libcerf) by Christoph Junghans
    * [rpm package](https://build.opensuse.org/package/show/science/libcerf) by Christoph Junghans
  * OS X:
    * [Homebrew/homebrew-science/libcerf.rb](https://formulae.brew.sh/formula/libcerf), by Roman Garnett
    * [MacPorts::libcerf](http://www.macports.org/ports.php?by=name&substr=libcerf), by Mojca Miklavec
  * Cross platform:
    * [vcpkg CMake packager](https://github.com/microsoft/vcpkg/tree/master/ports/libcerf), by Alexander Täschner

## Language bindings

For use with other programming languages, libcerf should be either linked directly, or provided with a trivial wrapper. Such language bindings are added to the libcerf package as contributed by their authors.

The following bindings are available:
  * **fortran**, by Antonio Cervellino (Paul Scherrer Institut)

Further contributions will be highly welcome.

Please report bugs to the package maintainer.
