// header to export specific functions relevant to qAlgorithms from the liberfc source code
// this is done to avoid having required dependencies, especially since we generally do not
// operate in the domain of complex numbers

// remove the type for complex numbers, it is not needed
#define _cerf_cmplx CMPLX

double erfi(double x); // special case for real x
