// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpattern.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_global_vars.h"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
// #include <string>
#include <vector>
#include <iostream>
// #include <fstream>
// #include <sstream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

namespace qAlgorithms
{
    MovedRegression moveAndScaleReg(FeaturePeak *feature)
    {
        float b0 = feature->coefficients.b0;
        float b1 = feature->coefficients.b1;
        float b2 = feature->coefficients.b2;
        float b3 = feature->coefficients.b3;

        const bool apexLeft = feature->apexLeft;
        float b23 = apexLeft ? b2 : b3;
        float apexDist = -b1 / (2 * b23);
        float height = feature->height;
        b0 += log(1) - height; // scale to height 1
        // move regression to RT of feature
        float offset = feature->retentionTime - apexDist;
        MovedRegression movedReg;
        movedReg.origin = feature;
        movedReg.binID = 0;   // @todo
        movedReg.limit_L = 0; // @todo
        movedReg.limit_R = 0; // @todo
        movedReg.b0_L = b0 - b1 * offset + b2 * offset * offset;
        movedReg.b0_R = b0 - b1 * offset + b3 * offset * offset;
        movedReg.b1_L = b1 - 2 * b2 * offset;
        movedReg.b1_R = b1 - 2 * b3 * offset;
        movedReg.b2 = b2;
        movedReg.b3 = b3;
        movedReg.RT_switch = apexLeft ? movedReg.b1_L : movedReg.b1_R / (2 * b23) - apexDist;
        return movedReg;
    }

    // small functions for calcTanimoto
    inline std::vector<float> areas(std::vector<float> *borders, MovedRegression *feature)
    {
        std::vector<float> areas;
        areas.reserve(borders->size() - 1);
        std::vector<float> heights;
        heights.reserve(borders->size());
        { // calcualte the relevant heights for both peak halves
            size_t i = 0;
            for (; i < borders->size(); i++)
            {
                float x = (*borders)[i];
                if (x > feature->RT_switch)
                {
                    break;
                }
                heights.push_back(exp(feature->b0_L + feature->b1_L * x + feature->b2 * x * x));
            }
            for (; i < borders->size(); i++)
            {
                float x = (*borders)[i];
                heights.push_back(exp(feature->b0_R + feature->b1_R * x + feature->b3 * x * x));
            }
        }
        for (size_t i = 1; i < borders->size(); i++)
        {
            areas.push_back((heights[i] + heights[i - 1]) / 2 *
                            ((*borders)[i] - (*borders)[i - 1]));
        }
        return areas;
    }

    float calcTanimoto(MovedRegression *feature_A, MovedRegression *feature_B)
    {
        assert(feature_A->RT_switch != feature_B->RT_switch);
        MovedRegression *feature_L = feature_A->RT_switch < feature_B->RT_switch ? feature_A : feature_B;
        MovedRegression *feature_R = feature_A->RT_switch > feature_B->RT_switch ? feature_A : feature_B;
        // naming: first  L / R : feature_L or _R; second L / R : left or right half of the regression
        float b0_L_L = feature_L->b0_L;
        float b0_L_R = feature_L->b0_R;
        float b0_R_L = feature_R->b0_L;
        float b0_R_R = feature_R->b0_R;
        float b1_L_L = feature_L->b1_L;
        float b1_L_R = feature_L->b1_R;
        float b1_R_L = feature_R->b1_L;
        float b1_R_R = feature_R->b1_R;
        float b2_L = feature_L->b2;
        float b2_R = feature_R->b2;
        float b3_L = feature_L->b3;
        float b3_R = feature_R->b3;
        float switch_L = feature_L->RT_switch;
        float switch_R = feature_R->RT_switch;
        float limit_L = std::max(feature_L->limit_L, feature_R->limit_L);
        float limit_R = std::max(feature_L->limit_R, feature_R->limit_R);

        // find intersects of both regressions
        std::vector<float> intersects; // @todo this can have a fixed size
        intersects.push_back(limit_L);

        // term under the root separate to check for 0 == ((b1A - b1B)^2 - 4* (b23A - b23B) * (b0A - b0B))
        { // both left halves of the regressions
            float root_LL = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) - 4 * (b2_L - b2_R) * (b0_L_L - b0_R_L);
            if (root_LL > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) / (2 * (b2_L - b2_R)) + root_LL / (2 * (b2_L - b2_R));
                if (intersect_pos > limit_L && intersect_pos < switch_L)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) / (2 * (b2_L - b2_R)) - root_LL / (2 * (b2_L - b2_R));
                if (intersect_neg > limit_L && intersect_neg < switch_L)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }

        intersects.push_back(switch_L);
        { // in the middle part, take the right half of feature_L and the left half of feature_R
            float root_LR = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) - 4 * (b3_L - b2_R) * (b0_L_R - b0_R_L);
            if (root_LR > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) / (2 * (b3_L - b2_R)) + root_LR / (2 * (b3_L - b2_R));
                if (intersect_pos > switch_L && intersect_pos < switch_R)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) / (2 * (b3_L - b2_R)) - root_LR / (2 * (b3_L - b2_R));
                if (intersect_neg > switch_L && intersect_neg < switch_R)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }

        intersects.push_back(switch_R);
        {
            float root_RR = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) - 4 * (b3_L - b3_R) * (b0_L_L - b0_R_L);
            if (root_RR > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) / (2 * (b3_L - b3_R)) + root_RR / (2 * (b3_L - b3_R));
                if (intersect_pos > switch_R && intersect_pos < limit_R)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) / (2 * (b3_L - b3_R)) - root_RR / (2 * (b3_L - b3_R));
                if (intersect_neg > switch_R && intersect_neg < limit_R)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }
        intersects.push_back(limit_R);
        std::sort(intersects.begin(), intersects.end());
        // subdivide the regions further in order for the score to be accurate
        size_t subdivisions = 10;
        size_t e = intersects.size();
        for (size_t i = 1; i < e; i++)
        {
            float distance = intersects[i] - intersects[i - 1];
            for (size_t i = 1; i < subdivisions; i++)
            {
                float step = distance * i;
                intersects.push_back(intersects[i - 1] + step);
            }
        }
        std::sort(intersects.begin(), intersects.end());
        // trapezoid integral for both features independently
        std::vector<float> areas_L = areas(&intersects, feature_L);
        std::vector<float> areas_R = areas(&intersects, feature_R);

        // scale by the maximum of either L or R at any given point
        std::vector<float> weights;
        weights.reserve(intersects.size() - 1);
        size_t position = 1;
        for (; position < weights.size(); position++)
        {
            // both left halves
            if (weights[position] > switch_L)
            {
                break;
            }
            float xval = (weights[position] + weights[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_L + b1_L_L * xval + b2_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < weights.size(); position++)
        {
            // middle region
            if (weights[position] > switch_R)
            {
                break;
            }
            float xval = (weights[position] + weights[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < weights.size(); position++)
        {
            // both right halves
            float xval = (weights[position] + weights[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_R + b1_R_R * xval + b3_R * xval * xval)));
        }
        // multiply every area with the weight, then sum up and divide for the coefficent
        assert(areas_L.size() == weights.size());
        float AuB = 0; // area only covered by either A or B
        float AnB = 0; // area covered by both A and B
        for (size_t i = 0; i < weights.size(); i++)
        {
            areas_L[i] *= weights[i];
            areas_R[i] *= weights[i];
            AnB += std::min(areas_L[i], areas_R[i]);
            AuB += abs(areas_L[i] - areas_R[i]);
        }
        return AuB / AnB;
    }
}
