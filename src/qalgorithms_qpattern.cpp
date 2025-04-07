// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_global_vars.h"

#include "./../external/CDFlib/cdflib.hpp"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
#include <vector>
#include <iostream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

int VALLEYS_1 = 0;
int VALLEYS_other = 0;

namespace qAlgorithms
{
    void findComponents(const std::vector<FeaturePeak> *peaks, const std::vector<EIC> *bins)
    {
        // peaks must be sorted here since they arrive out of order

        std::vector<GroupLims> limits = preGroup(peaks);
        std::vector<ComponentGroup> components;
        components.reserve(limits.size() / 2);
        for (size_t i = 0; i < limits.size(); i++)
        {
            // only test if the matrix thing works for now
            ComponentGroup newComponent;
            size_t groupsize = limits[i].end - limits[i].start + 1;
            std::cout << groupsize << ", ";
            // @todo skip for groups with size 1?
            if (groupsize == 1)
            {
                if ((peaks->at(limits[i].end).coefficients.b2 > 0) || (peaks->at(limits[i].end).coefficients.b3 > 0))
                {
                    VALLEYS_1++;
                }

                continue;
            }
            for (size_t j = limits[i].start; j < limits[i].end + 1; j++)
            {
                if ((peaks->at(j).coefficients.b2 > 0) || (peaks->at(j).coefficients.b3 > 0))
                {
                    VALLEYS_other++;
                }
            }

            if (groupsize > 10) // this is only to speed up testing - @todo remove!
            {
                continue;
            }
            newComponent.features.reserve(groupsize);
            unsigned int maxScan = 0;
            unsigned int minScan = 4294967295; // max value of unsigned int
            for (size_t j = limits[i].start; j < limits[i].end + 1; j++)
            {
                auto test = &peaks->at(j);
                auto movedTest = moveAndScaleReg(test);
                const qAlgorithms::EIC *bin = &bins->at(test->idxBin);
                calcRSS(&movedTest, bin);
                auto binRTs = (*bins)[test->idxBin].rententionTimes;
                auto scans = (*bins)[test->idxBin].scanNumbers;
                movedTest.binIdxStart = test->idxPeakStart;
                movedTest.binIdxEnd = test->idxPeakEnd;
                movedTest.limit_L = binRTs[test->idxPeakStart];
                movedTest.limit_R = binRTs[test->idxPeakEnd];
                maxScan = std::max(maxScan, scans[test->idxPeakEnd]); // @todo scans should be their own type, same with indices
                minScan = std::min(minScan, scans[test->idxPeakStart]);
                newComponent.features.push_back(movedTest);
            }

            std::vector<ReducedEIC> eics;
            for (size_t j = 0; j < groupsize; j++)
            {
                const auto feature = newComponent.features[j];
                const qAlgorithms::EIC *bin = &bins->at(feature.binID);
                eics.push_back(harmoniseEICs(bin, minScan, maxScan, feature.binIdxStart, feature.binIdxEnd));
                assert(eics[j].intensity.size() == eics[0].intensity.size());
            }

            newComponent.calcScores();
            // produce a subset of bins with uniform RT axis for this component

            // auto test = getCompareOrder(&newComponent); // not the best way to go about this

            // first, calculate the pairwise RSS in a sparse matrix. The RSS is set to INFINITY if it is worse
            // than the sum of both individual RSS values.
            // size: (x^2 -x) / 2 ; access: x * smaller Idx + larger Idx
            std::vector<float> pairRSS(groupsize * groupsize - groupsize, -1);
            // multi match all pairs
            for (size_t idx_S = 0; idx_S < groupsize - 1; idx_S++)
            {
                for (size_t idx_L = idx_S + 1; idx_L < groupsize; idx_L++)
                {
                    size_t access = idx_S * groupsize + idx_L; // position in pairRSS
                }
            }
            multiFit(&eics, peaks);

            // if two features are not improved by being combined individually, they may never be part of the same group

            // assign features to groups by forming the best-fit pairs

            // at this point, the group must be subdivided such that the total RSS is minimal
            // special case for groupsize 2 might be a good idea, since it seems to occur very often with the
            // current pre-gruping strategy
        }
        std::cout << std::endl;
        std::cout << "1: " << VALLEYS_1 << " ; other: " << VALLEYS_other << "\n"; // at least for one dataset, features with a valley point are much more likely
        // to be groups of size 1 than to be included in larger groups (ca. twice as likely)
    }

    ReducedEIC harmoniseEICs(const EIC *bin, const size_t minScan, const size_t maxScan, const size_t minIdx, const size_t maxIdx)
    {
        assert(maxIdx < bin->ints_area.size());
        assert(minScan < bin->scanNumbers.back());
        assert(maxScan > bin->scanNumbers.front());
        // scan relates to the complete measurement and idx to the position within the bin
        int length = maxScan - minScan + 1;
        assert(bin->ints_area[minIdx] != 0);
        assert(bin->ints_area[maxIdx] != 0);

        assert(length > 4);
        ReducedEIC reduced{
            std::vector<float>(length, 0),
            std::vector<float>(length, 0),
            std::vector<float>(length, 0),
            std::vector<size_t>(length, 0)};

        std::iota(reduced.scanNo.begin(), reduced.scanNo.end(), minScan);
        for (size_t i = 0; i < bin->ints_area.size(); i++) // indices in relation to bin without interpolations
        {
            size_t scan = bin->scanNumbers[i];
            if ((scan < minScan) || (scan > maxScan)) // @todo switch to break condition
            {
                continue;
            }
            assert(scan >= minScan);
            size_t relIdx = scan - minScan;
            reduced.intensity[relIdx] = bin->ints_area[i];
            reduced.intensity_log[relIdx] = log(bin->ints_area[i]);
            reduced.RTs[relIdx] = bin->rententionTimes[i];
        }
        // due to the selected window, it is possible for the first and last point to be 0

        size_t nonZero = 0;
        for (size_t i = 0; i < reduced.intensity.size(); i++)
        {
            if (reduced.intensity[i] > 0)
            {
                nonZero++;
            }
        }
        assert(nonZero > 4);
        return reduced; // @todo can result in eics with only zeroes
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
            assert(apex_L <= apex_R);
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
                float x = borders->at(i);
                if (x > feature->RT_switch)
                {
                    break;
                }
                heights.push_back(exp(feature->b0_L + feature->b1_L * x + feature->b2 * x * x));
            }
            for (; i < borders->size(); i++)
            {
                float x = borders->at(i);
                heights.push_back(exp(feature->b0_R + feature->b1_R * x + feature->b3 * x * x));
            }
        }
        for (size_t i = 1; i < borders->size(); i++)
        {
            float meanHeight = (heights[i] + heights[i - 1]) / 2;
            float dist = borders->at(i) - borders->at(i - 1);
            // assert(dist > 0); // unsure why this fails, should be fine?
            float area = meanHeight * dist;
            // assert(area > 0); // evidently, this is not sure to be the case
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
        // assert(0 <= score && score <= 1);
        // @todo this won't work
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

    void calcRSS(MovedRegression *reg, const EIC *bin)
    {
        // mutates the RSS field in MovedRegression
#define PREDICT_L(x) (std::exp(reg->b0_L + x * reg->b1_L + x * x * reg->b2))
#define PREDICT_R(x) (std::exp(reg->b0_R + x * reg->b1_R + x * x * reg->b3))
        const auto RTs = bin->rententionTimes;
        const auto areas = bin->ints_area;
        size_t idx = 0;
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->limit_L)
            {
                break;
            }
        }
        float RSS = 0;
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->RT_switch)
            {
                break;
            }
            float diff = PREDICT_L(RTs[idx]) - areas[idx];
            RSS += diff * diff;
        }
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->limit_R)
            {
                break;
            }
            float diff = PREDICT_R(RTs[idx]) - areas[idx];
            RSS += diff * diff;
        }
        reg->RSS = RSS;
    }

    // MultiMatrix combinedMatrix(std::vector<std::vector<float>> *intensities)
    // {
    //     MultiMatrix matrix;
    //     matrix.n_cols = 3 + intensities->size(); // three for b1, b2, b3 and one b0 for every other member
    //     // omit the last value to ensure uneven number of points - this is a bad solution @todo
    //     for (size_t i = 0; i < intensities->size(); i++)
    //     {
    //         if (!(intensities[i].size() % 2))
    //         {
    //             intensities[i].pop_back();
    //         }
    //     }
    // }

    struct IntensityMatrix
    {
        std::vector<std::vector<float>> intensities;
        std::vector<size_t> scanNo;
    };

    IntensityMatrix combineIntensities(const ComponentGroup *group, const std::vector<EIC> *bins)
    {
        // combine all features in the group such that their intensities are all
        // accessible in a per-scan setting

        // get the lowest and highest scan number of all component candidates
        size_t lowerBound = INT64_MAX;
        size_t upperBound = 0;
        for (size_t i = 0; i < group->features.size(); i++)
        {
            size_t binID = group->features[i].binID;
            auto bin = (*bins)[binID];
            size_t lowEnd = bin.scanNumbers[group->features[i].origin->idxPeakStart];
            size_t highEnd = bin.scanNumbers[group->features[i].origin->idxPeakEnd];
            lowerBound = std::min(lowerBound, lowEnd);
            upperBound = std::max(upperBound, highEnd);
        }
        // this vector holds the scan numbers per regression
        size_t totalEntries = upperBound - lowerBound + 1;
        std::vector<size_t> scanNo(totalEntries);
        std::iota(scanNo.begin(), scanNo.end(), lowerBound);

        // add a vector of size = number of groups for every relevant scan
        std::vector<float> insert(group->features.size(), 0);
        std::vector<std::vector<float>> elements;
        elements.reserve(totalEntries);
        for (size_t i = 0; i < totalEntries; i++)
        {
            elements.push_back(insert);
        }

        // fill the sub-vectors with relevant intensities, intensity 0 being equivalent to "not found"
        for (size_t i = 0; i < group->features.size(); i++)
        {
            size_t binID = group->features[i].binID;
            auto bin = (*bins)[binID];
            auto scan = bin.scanNumbers;
            auto area = bin.ints_area;
            for (size_t j = 0; j < scan.size(); j++)
            {
                // iterate through the bin and add only those intensities with relevant scans
                if (scan[j] < lowerBound)
                {
                    continue;
                }
                if (scan[j] > upperBound)
                {
                    break;
                }
                size_t scanIdx = scan[j] - lowerBound;
                elements[scanIdx][i] = area[j];
            }
        }
    }

    std::vector<std::vector<float>> designMat(int scale, size_t k) // , const std::vector<size_t> *eliminate
    {
        // k is the number of features combined, left as k due to similar naming in adapted code
        // eliminate skips all rows whose indices are included.
        assert(scale > 1);
        assert(k > 1);
        size_t colLenght = 2 * scale + 1;
        size_t vecSize = colLenght * k;
        std::vector<std::vector<float>> resultMat;
        // add one vector b0 for every feature. b0 is 0 everywhere except for the region that contains the feature
        for (size_t i = 0; i < k; i++)
        {
            std::vector<float> b0(vecSize);
            for (size_t j = i; j < colLenght; j++)
            {
                b0[j] = 1;
            }
            resultMat.push_back(b0);
        }

        std::vector<float> b1(vecSize, 0);
        for (size_t i = 0; i < k; i++)
        {
            size_t offset = i * colLenght;
            std::iota(b1.begin() + offset, b1.begin() + offset + colLenght, -scale);
        }
        resultMat.push_back(b1);
        std::vector<float> b2(vecSize, 0);
        for (size_t j = 0; j < k; j++)
        {
            for (int i = 0; i < scale + 1; i++)
            {
                size_t offset = j * colLenght;
                b2[i + offset] = b1[i + offset] * b1[i + offset];
            }
        }
        std::vector<float> b3(vecSize, 0);
        for (size_t j = 0; j < k; j++)
        {
            for (size_t i = scale + 1; i < colLenght; i++)
            {
                size_t offset = j * (colLenght - 1);
                b3[i + offset] = b1[i + offset] * b1[i + offset];
            }
        }
        resultMat.push_back(b2);
        resultMat.push_back(b3);
        // @todo add a correctness check
        return resultMat;
    }

    // void singleFit(std::vector<float> *logIntensity) // this was only included for the residuals, replace
    // {
    //     // the model requires an uneven number of points, this is a suboptimal solution
    //     // int scale = bool(logIntensity->size() % 2) ? logIntensity->size() / 2 : logIntensity->size() / 2 - 1;

    //     if (!logIntensity->size() % 2)
    //     {
    //         logIntensity->resize(logIntensity->size() - 1);
    //     }
    //     int scale = logIntensity->size() / 2;
    //     // auto design = designMat(scale);
    // }

    void multiFit(const std::vector<ReducedEIC> *fitRegion, const std::vector<FeaturePeak> *peaks)
    {
        assert(fitRegion->size() == peaks->size());
        // fit one model with variable b0 over multiple traces
        // fitRegion contains all mass traces over which a fit should be performed, including zero padding
        // find the scale that fits the non-0 intensity region, -1 at the lowest intensity if an even number is found
        size_t k = fitRegion->size();
        size_t limit_L = 0;
        size_t limit_R = fitRegion->front().intensity.size();
        // to decide in the case of an uneven region, keep track of the total intensities at the two outer points
        float sumInts_L = -1;
        float sumInts_R = -1;
        for (size_t i = 0; i < fitRegion->front().intensity.size(); i++)
        {
            // left half
            float sumInts = 0;
            for (size_t j = 0; j < k; j++)
            {
                sumInts += fitRegion->at(j).intensity_log[i];
            }
            if (sumInts != 0)
            {
                sumInts_L = sumInts;
                limit_L = i;
                break;
            }
        }
        for (size_t i = fitRegion->front().intensity.size() - 1; i > limit_L; i--)
        {
            // left half
            float sumInts = 0;
            for (size_t j = 0; j < k; j++)
            {
                sumInts += fitRegion->at(j).intensity_log[i];
            }
            if (sumInts != 0)
            {
                sumInts_R = sumInts;
                limit_R = i;
                break;
            }
        }

        // if there is an even number of points, the difference of the limits is uneven
        if (((limit_R - limit_L) % 2) == 1)
        {
            // adjust the window while minimising the intensity loss
            if (sumInts_L > sumInts_R)
            {
                limit_R -= 1;
            }
            else
            {
                limit_L += 1;
            }
        }
        assert(limit_R - limit_L > 4);

        const size_t scale = (limit_R - limit_L + 1) / 2; // +1 since limits are indices
        // produce the intensity vector (without zeroes) while keeping track of which rows to interpolate
        std::vector<size_t> interpolate;
        std::vector<float> combined_logInt((2 * scale + 1) * k, 0);
        interpolate.reserve(combined_logInt.size() / 10);
        size_t logIdx = 0;
        for (size_t i = 0; i < k; i++)
        {
            ReducedEIC current = (*fitRegion)[i];
            auto coeffs = peaks->at(i).coefficients;
            for (size_t j = limit_L; j < limit_R + 1; j++)
            {
                combined_logInt[logIdx] = current.intensity_log[j];
                if (current.intensity_log[j] == 0)
                {
                    interpolate.push_back(logIdx);
                }
                logIdx++;
            }
        }
        // assert(combined_logInt.back() != 0); // this is possible, although hopefully unlikely
        auto matrix = designMat(scale, k);
        assert(matrix[0].size() == combined_logInt.size());
        // set all entries of the matrix were no intensity values exist to 0
        for (size_t i = 0; i < matrix.size(); i++)
        {
            for (size_t j = 0; j < interpolate.size(); j++)
            {
                matrix[i][j] = 0;
            }
        }
    }

    bool preferMerge(float rss_complex, float rss_simple, size_t n_complex, size_t p_complex, size_t p_simple)
    {
        float alpha = 0.05; // @todo is a set alpha reall the best possible solution?
        // problem: pre-calculation of all relevant f values could result in a very large array
        // possible max size of 20 seems reasonable, maximum observed is 6
        int which = 1; // select mode of library function and check computation result
        double p = 0;  // not required
        double q = 0;  // return value, equals p - 1
        double F = ((rss_simple - rss_complex) / float(p_complex - p_simple)) / (rss_complex / float(n_complex - p_complex));
        double dfn = p_complex - p_simple;  // numerator degrees of freedom
        double dfd = n_complex - p_complex; // denominator degrees of freedom
        int status = 1;                     // result invalid if this is not 0
        double bound = 0;                   // allows recovery from non-0 status

        cdff(&which, &p, &q, &F, &dfn, &dfd, &status, &bound); // library function, see https://people.math.sc.edu/Burkardt/cpp_src/cdflib/cdflib.html
        assert(status == 0);

        return q > alpha; // the merged model is not worse than the complex version, both can be merged
    }
}
