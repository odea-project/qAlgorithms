#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatypes.h"
// #include <map>

#include <vector>

namespace qAlgorithms
{
    struct ComponentRegression
    {
        std::vector<size_t> members; // indices of the returned features
        RegCoeffs coeffs;
        size_t pointsToal;
    };

}

#endif