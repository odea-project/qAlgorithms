#ifndef _QALGORITHMS_GLOBALS_INCLUDED
#define _QALGORITHMS_GLOBALS_INCLUDED

#include <array>

namespace qAlgorithms
{
    extern float PPM_PRECENTROIDED;
    extern float MZ_ABSOLUTE_PRECENTROIDED;
    extern const double TOLERANCE_BINNING;
    extern const int GLOBAL_MAXSCALE;
    extern const std::array<float, 384> INV_ARRAY; // Make const
    extern const float T_VALUES[256];
    extern const float CHI_SQUARES[256];
    extern const double SQRTPI_2;
    extern bool PRINT_PPM_WARNING;
    extern const double OS_CRIT_A; // this value is determined empirically (see https://github.com/GeRe87/OS_critVal)
    extern const double OS_CRIT_B; // this value is determined empirically (see https://github.com/GeRe87/OS_critVal)

    const std::array<float, 384> initialize(); // Declare initialize function
}

#endif