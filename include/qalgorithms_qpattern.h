#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatypes.h"
// #include <map>

#include <vector>

namespace qAlgorithms
{
    // This function modifies the coefficients of a regression so that it describes a peak with the apex at
    // x = RT of the feature and the height 1. Features are manipulated this way to allow a shape comparison
    MovedRegression moveAndScaleReg(FeaturePeak *feature);

    // calculate the shape tanimoto / jaccard index of two moved regressions. The score is weighted slightly.
    // Calculation is done by dividing the area covered by the overlap both regression curves by the area covered
    // by both regressions independently
    // The antiderivative of our regression curve involves exp(6000) and greater numbers, so we use
    // trapezoid integration instead.
    float calcTanimoto(MovedRegression *feature_A, MovedRegression *feature_B);
}

#endif