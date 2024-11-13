#ifndef _QALGORITHMS_GLOBALS_INCLUDED
#define _QALGORITHMS_GLOBALS_INCLUDED

namespace qAlgorithms
{
    extern float PPM_PRECENTROIDED; // cannot be const, since it is used to communicate the ppm selection
    extern const double TOLERANCE_BINNING;
    extern const int GLOBAL_MAXSCALE;
    extern std::array<float, 384> INV_ARRAY; // @todo make const
    extern const float T_VALUES[256];
    extern const float CHI_SQUARES[256];
    extern const double SQRTPI_2;
}

#endif