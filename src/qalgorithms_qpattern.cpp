// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpattern.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_global_vars.h"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
#include <vector>
#include <iostream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

namespace qAlgorithms
{
    void findComponents(const std::vector<FeaturePeak> *peaks, const std::vector<EIC> *bins)
    {
        std::vector<GroupLims> limits = preGroup(peaks);
        for (size_t i = 0; i < peaks->size() - 10; i++)
        {
            // only test if the matrix thing works for now
            ComponentGroup newComponent;
            for (size_t j = 0; j < 10; j++)
            {
                auto test = &((*peaks)[i + j]);
                auto movedTest = moveAndScaleReg(test);
                auto binRTs = (*bins)[test->idxBin].rententionTimes;
                movedTest.limit_L = binRTs[test->idxPeakStart];
                movedTest.limit_R = binRTs[test->idxPeakEnd];
                newComponent.features.push_back(movedTest);
            }
            newComponent.calcScores();
            auto test = getCompareOrder(&newComponent);
        }
    }

    float ComponentGroup::score(size_t idx1, size_t idx2)
    { // @todo we could use a more complex structure here to save space, but with how small these groups should be it is probably pointless
        size_t size = shapeScores.size();
        assert(idx1 < size && idx2 < size);

        if (idx1 == idx2)
        {
            return 1;
        }
        size_t idx = size * idx1 + idx2;
        return shapeScores[idx];
    }

    void ComponentGroup::calcScores()
    {
        shapeScores.reserve(features.size() * features.size());
        for (size_t i = 0; i < features.size(); i++)
        {
            for (size_t j = 0; j < features.size(); j++)
            {
                float tanimoto = calcTanimoto_reg(&features[i], &features[j]);
                shapeScores.push_back(tanimoto);
            }
        }
    }

    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks)
    {
        // base assumption: for all peaks that constitute a component, the two apexes
        // are no further apart than the smallest uncertainty of the pair
        // This assumes that peaks is sorted by retention time!
        std::vector<GroupLims> initialGroups;
        initialGroups.push_back({0, 0});
        for (size_t i = 1; i < peaks->size(); i++)
        {
            float apex_L = (*peaks)[i - 1].retentionTime;
            float apex_R = (*peaks)[i].retentionTime;
            float uncert = std::min((*peaks)[i - 1].retentionTimeUncertainty, (*peaks)[i].retentionTimeUncertainty);
            if (apex_R - apex_L > uncert)
            {
                initialGroups.back().end = i - 1;
                initialGroups.push_back({i, 0});
            }
        }
        initialGroups.back().end = peaks->size() - 1;
        return initialGroups;
    }

    MovedRegression moveAndScaleReg(const FeaturePeak *feature)
    {
        float b0 = feature->coefficients.b0;
        float b1 = feature->coefficients.b1;
        float b2 = feature->coefficients.b2;
        float b3 = feature->coefficients.b3;

        const bool apexLeft = feature->apexLeft;
        float b23 = apexLeft ? b2 : b3;
        float apexDist = -b1 / (2 * b23);
        float height = b0 + b1 * apexDist + b23 * apexDist * apexDist;
        const float scaleTo = 1;
        b0 += log(scaleTo) - height; // scale to height 1. The log is 0 for case height = 1, but we might have to adjust later
        // move regression to RT of feature
        float offset = feature->retentionTime - apexDist;
        MovedRegression movedReg;
        movedReg.origin = feature;
        movedReg.binID = feature->idxBin;
        movedReg.limit_L = 0; // @todo
        movedReg.limit_R = 0; // @todo
        movedReg.b0_L = b0 - b1 * offset + b2 * offset * offset;
        movedReg.b0_R = b0 - b1 * offset + b3 * offset * offset;
        movedReg.b1_L = b1 - 2 * b2 * offset;
        movedReg.b1_R = b1 - 2 * b3 * offset;
        movedReg.b2 = b2;
        movedReg.b3 = b3;
        movedReg.RT_switch = (apexLeft ? movedReg.b1_L : movedReg.b1_R) / (-2 * b23); // -1 would be in front of the b1, switched for readability
        return movedReg;
    }

    // small functions for calcTanimoto_reg
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
            auto meanHeight = (heights[i] + heights[i - 1]) / 2;
            auto dist = (*borders)[i] - (*borders)[i - 1];
            auto area = meanHeight * dist;
            assert(area > 0);
            areas.push_back(area);
        }
        return areas;
    }

    float calcTanimoto_reg(MovedRegression *feature_A, MovedRegression *feature_B)
    {
        // assert(feature_A->RT_switch != feature_B->RT_switch); // @todo turn on once we need a more elegant access pattern
        MovedRegression *feature_L = feature_A->RT_switch < feature_B->RT_switch ? feature_A : feature_B;
        MovedRegression *feature_R = feature_A->RT_switch > feature_B->RT_switch ? feature_A : feature_B;
        if (feature_L->limit_R < feature_R->limit_L) [[unlikely]]
        {
            // the features have no overlap, exclude this case for better correctness checing in the function
            return 0;
        }

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

        // this is a stopgap solution for the case of a positive quadratic exponent when the regression window extends past the local minimum
        // use signbit of -b2 / -b3 to still select the max if both are positive (signbit returns true for set)
        if ((b2_L > 0) | (b2_R > 0))
        {
            limit_L = std::max(std::signbit(-b2_L) * feature_L->limit_L, std::signbit(-b2_R) * feature_R->limit_L);
        }
        if ((b3_L > 0) | (b3_R > 0))
        {
            limit_R = std::max(std::signbit(-b2_R) * feature_L->limit_R, std::signbit(-b3_R) * feature_R->limit_R);
        }

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

        // remove duplicate entries from vector
        std::sort(intersects.begin(), intersects.end());
        const auto &it = std::unique(intersects.begin(), intersects.end());
        intersects.resize(std::distance(intersects.begin(), it));

        // subdivide the regions further in order for the score to be accurate
        size_t subdivisions = 10;
        size_t e = intersects.size();
        for (size_t i = 1; i < e; i++)
        {
            float distance = intersects[i] - intersects[i - 1];
            for (size_t j = 1; j < subdivisions; j++)
            {
                float step = distance * j / subdivisions;
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
        for (; position < intersects.size(); position++)
        {
            // both left halves
            if (intersects[position] > switch_L)
            {
                break;
            }
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_L + b1_L_L * xval + b2_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < intersects.size(); position++)
        {
            // middle region
            if (intersects[position] > switch_R)
            {
                break;
            }
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < intersects.size(); position++)
        {
            // both right halves
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_R + b1_R_R * xval + b3_R * xval * xval)));
        }
        // multiply every area with the weight, then sum up and divide for the coefficent
        // @todo check for squre sum score
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
        auto score = AuB / AnB;
        assert(0 <= score && score <= 1);
        return score;
    }

    std::pair<unsigned int, unsigned int> featNums(size_t idx, const ComponentGroup *group)
    {
        // the original two indices of a pair:
        // idx = size * idx1 + idx2
        unsigned int idx2 = idx % group->features.size();
        unsigned int idx1 = (idx - idx2) / group->features.size();
        return std::make_pair(idx1, idx2);
    }

    std::vector<size_t> getCompareOrder(const ComponentGroup *group)
    {
        assert(group->shapeScores.size() > 0);
        std::vector<size_t> indices(group->shapeScores.size());
        std::iota(indices.begin(), indices.end(), 0);
        {
            std::sort(indices.begin(), indices.end(),
                      [group](size_t lhs, size_t rhs)
                      { return (group->shapeScores)[lhs] > (group->shapeScores)[rhs]; });
            // remove indices where the score is smaller than sensible
            float lowestScore = 0.01;
            size_t breakpoint = 0;
            for (; group->shapeScores[indices[breakpoint]] > lowestScore; breakpoint++)
                ;
            indices.resize(breakpoint + 1);
        }

        // vector to count how many other regressions the one at this index has been combined with
        std::vector<unsigned int> taken(group->features.size(), 0);

        for (size_t i = 0; i < indices.size() / 2; i++)
        {
            auto pair = featNums(indices[2 * i], group); // at this point, two neighbours always have the same score.
            bool left_is_left = group->features[pair.first].origin->retentionTime < group->features[pair.second].origin->retentionTime;
            if (!left_is_left)
            {
                std::swap(pair.first, pair.second);
            }
            // @todo do the regression comparison here
            bool mergeSuccess = true;
            if (mergeSuccess)
            {
                taken[pair.first]++;
                taken[pair.second]++;
            }
        }

        return indices;
    }
}
