// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_global_vars.h"
// temporary dependencies
#include "./../external/CDFlib/cdflib.hpp"

#include <cmath>   // pow and log
#include <numeric> // iota and accumulate
#include <vector>
#include <iostream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

size_t VALLEYS_1 = 0;
size_t VALLEYS_other = 0;

namespace qAlgorithms
{
    size_t realRegressions = 0;
    size_t failRegressions = 0;
    size_t ERRORCOUNTER = 0;

    // this module-local variable is used to prevent negative intensities from occurring
    float lowestAreaLog = 0;

    void findComponents_new(
        // note: both features and bins contain a "componentID" field that is 0 by default.
        // the componentisation function updates these fields in addition to returning the component regressions
        std::vector<FeaturePeak> *features,
        std::vector<EIC> *bins)
    {
        // sort features by RT

        // find pre-groups by checking if apexes are further apart than the last smallest distance

        // for every pre group, construct an exclusion matrix
        // -- ensure that the apex position is respected
        // -- ? (efficiency) Do a second pass and find potential subgroups from exclusion info
        // -- do the pairwise comparision
        // -- -- maximum scale: determined by maximum present scale in features (+1 for saftey?)
        // -- ? some additional prefilter based on exclusions
        // -- add features to groups in order of match score
        // produce group information based on matrix shape

        // for every group, calculate shape score
    }

    // note: ranges are only returned for two or more points
    std::vector<Range_i> preGroup_new(const std::vector<FeaturePeak> *features)
    {
        // assumes RT-sorted vector as input
        std::vector<Range_i> ret;

#define position(x) features->at(x).retentionTime
#define uncert(x) features->at(x).RT_Uncertainty

        struct Point
        {
            size_t index = 0;
            double position = 0;
            double uncert = 0;

            void upd(size_t idx, double pos, double un)
            {
                index = idx;
                position = pos;
                uncert = un;
            }
            double rb()
            {
                return position + uncert;
            }
        };

        // the group has the following relevant points:
        // 1) the point which determines the right limit of the range by its position
        // 2) the point which determines the right limit of the range by outermost point
        //
        // the grouping can only consider a point if it is within the range given by these two values.
        // If the points are identical, all comparisons are trivial.
        // If the outer limit is caused by a different point, it is necessary to check in reverse order
        // (until point 2 is reached or the difference of the compared point is exceeded)
        // all points that are elements of the group.
        //
        // we can only be sure that a group is complete when the current position exceeds the limit
        // as set by point 2. This is because any peak could just be very small and be placed
        // inbetween two points with a very large span. Also note that the minimum reasonable uncertainty
        // is one point to each side of the apex.

        size_t groupStart = 0;

        Point point1;
        point1.position = position(0);
        point1.uncert = uncert(0);
        Point point2 = point1;

        for (size_t i = 1; i < features->size(); i++)
        {
            double pos_n = position(i);
            double uncert_n = uncert(i);
            if (pos_n > point2.rb())
            {
                // the group is complete
                point1.upd(i, pos_n, uncert_n);
                point2 = point1;
            }
            else if (pos_n - uncert_n > point1.position)
            {
                bool continueGroup = false;
                for (size_t j = i + 1; j < features->size(); j++)
                {
                    pos_n = position(j);
                    uncert_n = uncert(j);

                    if (pos_n > point2.rb())
                        break;

                    if (pos_n - uncert_n > point1.position)
                        continue;

                    point1.upd(j, pos_n, uncert_n);
                    if (pos_n + uncert_n > point2.rb())
                        point2.upd(j, pos_n, uncert_n);
                    continueGroup = true;
                    i = j;
                }

                if (!continueGroup)
                {
                    // the group is complete
                    point1.upd(i, pos_n, uncert_n);
                    point2 = point1;
                }
            }
            else if (point1.index == point2.index)
            {
                // only check if the point could be added once
                double minUN = point1.uncert < uncert_n ? point1.uncert : uncert_n;
                double diff = pos_n - point1.position;
                if (diff < minUN)
                {
                    // the point belongs into the group
                    point1.upd(i, pos_n, uncert_n);
                    if (pos_n + uncert_n > point2.rb())
                        point2.upd(i, pos_n, uncert_n);
                }
                else
                {
                    // the group is complete
                    point1.upd(i, pos_n, uncert_n);
                    point2 = point1;
                }
            }
            else
            {
                // there are two limits relevant to the computation, and the point could be within the
                // relevant region.
                bool addToGroup = false;

                // check if another point exists which could be within the relevant distance
                for (size_t check = point1.index; check >= point2.index; check--)
                {
                    double pos_c = position(check);
                    double uncert_c = uncert(check);
                    double minUN = uncert_c < uncert_n ? uncert_c : uncert_n;
                    double diff = pos_n - pos_c;
                    if (diff < minUN)
                    {
                        addToGroup = true;
                        break;
                    }
                }

                if (addToGroup)
                {
                    // the point belongs into the group
                    point1.upd(i, pos_n, uncert_n);
                    if (pos_n + uncert_n > point2.rb())
                        point2.upd(i, pos_n, uncert_n);
                }
            }
        }

        // @todo close last group

        return ret;

#undef position
#undef uncert
    }

    size_t ExcludeMatrix::indexOf(size_t first, size_t second)
    {
        // the structure of the matrix is triangular, so only half of it has to be stored.
        // points cannot sensibly be compared with themselves
        assert(first != second);
        size_t smaller = first < second ? first : second;
        size_t bigger = first > second ? first : second;
        return bigger * (bigger - 1) / 2 + smaller;
    }
    void ExcludeMatrix::invalidate(size_t first, size_t second)
    {
        size_t access = indexOf(first, second);
        storage[access] = 0;
    }
    bool ExcludeMatrix::isInvalid(size_t first, size_t second)
    {
        if (first == second)
            return true;
        size_t access = indexOf(first, second);
        return storage[access] == 0;
    }
    double *ExcludeMatrix::at(size_t first, size_t second)
    {
        size_t access = indexOf(first, second);
        return storage.data() + access;
    }
    void ExcludeMatrix::fill(size_t n)
    {
        featureCount = n;
        size_t maxcount = n * (n - 1) / 2 + n;
        storage.resize(maxcount);
        for (size_t i = 0; i < maxcount; i++)
            storage[i] = NAN;
    }
    size_t ExcludeMatrix::countMatches(size_t row)
    {
        size_t matches = 0;
        for (size_t i = 0; i < featureCount; i++)
        {
            if (!isInvalid(i, row))
                matches += 1;
        }
        return matches;
    }

    void pairwiseMatch(const Range_i *region,
                       const std::vector<FeaturePeak> *features,
                       const std::vector<EIC> *eics,
                       ExcludeMatrix *excludeMatrix)
    {
        size_t nonExcludes = 0;
// in a first pass, establish if the two points fit in terms of position
#define position(x) features->at(x).retentionTime
#define uncert(x) features->at(x).RT_Uncertainty
        for (size_t i = region->startIdx; i < region->endIdx; i++)
        {
            for (size_t j = i + 1; j < region->endIdx + 1; j++)
            {
                double pos_i = position(i);
                double uncert_i = uncert(i);
                double pos_j = position(j);
                double uncert_j = uncert(j);
                double minUncert = uncert_i < uncert_j ? uncert_i : uncert_j;
                double posDiff = pos_j - pos_i;
                if (posDiff > minUncert)
                    excludeMatrix->invalidate(i, j);
            }
        }
#undef position
#undef uncert

        // next, establish that the profile in a pairwise match is OK.
        for (size_t i = region->startIdx; i < region->endIdx; i++)
        {
            for (size_t j = i + 1; j < region->endIdx + 1; j++)
            {
                if (excludeMatrix->isInvalid(i, j))
                    continue;
                // do the regression test up to the largest scale applicable to both points
                const FeaturePeak *feature_I = features->data() + i;
                const FeaturePeak *feature_J = features->data() + j;
                const EIC *eic_I = eics->data() + i;
                const EIC *eic_J = eics->data() + j;
                const double msePair = evalPair(feature_I, eic_I, feature_J, eic_J);
                *excludeMatrix->at(i, j) = msePair;
                nonExcludes += msePair > 0 ? 1 : 0;
            }
        }
        // @todo consider including the number of not-invalid comparisons as return value
    }

    double evalPair(const FeaturePeak *feat_A, const EIC *eic_A,
                    const FeaturePeak *feat_B, const EIC *eic_B)
    {
        // this is a special case of the normal multiregression with only two b0 values.
        const FeaturePeak *feats[2] = {feat_A, feat_B};
        const size_t featCount = 2;
        const EIC *eics[2] = {eic_A, eic_B};

        // the range is the scan region within which regressions of size maxscale will be searched for
        Range_i range;
        size_t maxscale;
        size_t minscale;
        scanRegion(feats, featCount, &range, &maxscale, &minscale);

        // construct the sum vector and individual vector of log intensities.
        // this requires the maximum regression window to be known ahead of time.
        // @todo performing the regression in log space intensifies the baseline significantly when
        // summing up everything, consider a means of documenting the how much for every signal

        auto regTarget = logVectors_multireg(eics, 2, &range);

        // perform the multireg
        auto regs = findCoefficients_multi_new(&regTarget.logInt_single, &regTarget.logInt_sum, featCount, maxscale);

        // validate regressions
        size_t pointsPerScale = rangeLen(&range) - 2 * maxscale;
        size_t startidx = minscale * pointsPerScale;
        double lowestMSE = INFINITY;
        size_t bestRegIdx = 0;
        std::vector<MultiRegression> validRegs;
        for (size_t i = startidx; i < regs.size(); i++)
        {
            // modulus(position / first index at which scale == 2) + scale at which multireg starts
            size_t scale = (i % (maxscale - 1)) + 2;
            // minscale is imposed so that regressions are not narrowed too much compared to the input regressions
            auto coeff = regs[i];

            // validation

            // mse calculation and comparison
            // Note: Since we know that at most one regression can be applied to the region, we only accept
            // an answer with one single valid regression, that being the best by mse in exponential form.
        }
    }

    double compareMulti(const FeaturePeak **featArray, const EIC **eics, size_t numFeats)
    {
        // same as above, just less explicit - merge functions eventually? @todo
    }

    void scanRegion(const FeaturePeak **featArray, const size_t length, Range_i *scanRange, size_t *maxscale, size_t *minscale)
    {
        size_t maxLowerScan = -1;
        size_t minUpperScan = 0;
        *maxscale = 0;
        *minscale = INFINITY;

        for (size_t i = 0; i < length; i++)
        {
            size_t lower = (*featArray)[i].scanPeakStart;
            size_t upper = (*featArray)[i].scanPeakEnd;
            size_t scale = (*featArray)[i].scale;

            maxLowerScan = lower > maxLowerScan ? lower : maxLowerScan;
            minUpperScan = upper < minUpperScan ? upper : minUpperScan;
            *maxscale = scale > *maxscale ? scale : *maxscale;
            *minscale = scale < *minscale ? scale : *minscale;
        }
        *maxscale += 1; // safety margain, can this be reasoned about?

        assert(maxLowerScan < minUpperScan);
        assert(maxLowerScan > *maxscale);
        // in order for the range to account for the existing regressions, at least the
        // largest observed scale must be possible for the multiregression
        *scanRange = {maxLowerScan - *maxscale, minUpperScan + *maxscale};
    }

    MergeVectors logVectors_multireg(const EIC **eics, const size_t length, const Range_i *scanRange)
    {
        // note: scan range describes the region in which the apex of the regression can be. Since there is no need
        // to check for larger peaks than we previously found, the maxscale is limited to the maximum scale of compared
        // features (+ the number of minimal degrees of freedom to account for potenial merges of two features distorted
        // into different directions) This adjustment is made when calling this function. All this function does is
        // extract a region of data that goes from scanrange.start - maxscale to scanrange.end + maxscale

        size_t span = rangeLen(scanRange);
        assert(span >= 5); // despite having more degrees of freedom, we still need five points for the individual peaks to make sense

        std::vector<float> logInt_sum(span, 0);
        std::vector<float> logInt_single(span * length, NAN);
        std::vector<size_t> df_sum(span, 0);
        std::vector<size_t> df_single(span * length, 0);

        for (size_t elem = 0; elem < length; elem++)
        {
            const EIC *currentEIC = eics[elem];
            size_t eicSize = currentEIC->mz.size();

            // first, identify the start and end of the region in the given EIC.
            size_t currentIdx = 0;
            size_t endIdx = 0;
            size_t startIdx = 0;
            // All ends of EICs have already been extrapolated. The halving process is fairly
            // aggessive, so there should be no need to add additional complexity to the system
            // through some further extrapolation. All EICs are also fully interpolated, so no
            // logic is needed for that either.
            // As such, this function really just copies data from the individual EICs into the
            // less cluttered container struct

            for (; currentIdx < eicSize; currentIdx++)
            {
                size_t scan = currentEIC->scanNumbers[currentIdx];
                if (scan >= scanRange->startIdx)
                    break;
            }
            // @todo solve case of the regression extending past the border
            // first relevant degree of freedom required to avoid re-addition
            size_t df_start = currentEIC->df[currentIdx];
            df_start -= currentIdx == 0 ? 0 : currentEIC->df[currentIdx - 1];
            for (; currentIdx < eicSize; currentIdx++)
            {
                size_t scan = currentEIC->scanNumbers[currentIdx];
                if (scan > scanRange->endIdx)
                    break;

                double logint = log(currentEIC->ints_area[currentIdx]);
                size_t df = currentEIC->df[currentIdx] - df_start;

                size_t access_sum = scan - scanRange->startIdx;
                logInt_sum[access_sum] += logint;
                df_sum[access_sum] += df;

                size_t access_single = scan - scanRange->startIdx + span * elem;
                logInt_single[access_single] = logint;
                df_single[access_single] = df;
            }
        }
        // the degrees of freedom should be cummulative

        MergeVectors res;
        res.numFeats = length;
        res.lengthSingle = span;
        res.df_single = df_single;
        res.df_sum = df_sum;
        res.logInt_single = logInt_single;
        res.logInt_sum = logInt_sum;

        return res;
    }

    std::vector<MultiRegression> findComponents(
        std::vector<FeaturePeak> *peaks, // the peaks are updated as part of componentisation
        std::vector<EIC> *bins,
        const std::vector<float> *convertRT,
        float lowestArea,
        size_t *featuresInComponents)
    {
        // std::cout << "pregroup,pairs,excludes,totalComps\n";

        assert(peaks->begin()->componentID == 0);
        assert(*featuresInComponents == 0);
        lowestAreaLog = log(lowestArea);

        size_t globalCompID = 1; // this is the component ID later used on a feature level. 0 means not part of a component

#pragma region "Pre-Group"
        std::vector<GroupLims> limits = preGroup(peaks);
        std::vector<MultiRegression> finalComponents;

        for (size_t groupIdx = 0; groupIdx < limits.size(); groupIdx++)
        {
            // @todo factor the loop internals into at least one function
            PreGrouping pregroup;
            size_t groupsize = limits[groupIdx].end - limits[groupIdx].start + 1;

            if (groupsize == 1)
            {
                if ((peaks->at(limits[groupIdx].end).coefficients.b2 > 0) ||
                    (peaks->at(limits[groupIdx].end).coefficients.b3 > 0))
                {
                    VALLEYS_1++;
                }
                // std::cout << 0 << ", " << 0 << "\n";
                continue;
            }
            for (size_t j = limits[groupIdx].start; j < limits[groupIdx].end + 1; j++)
            {
                if ((peaks->at(j).coefficients.b2 > 0) || (peaks->at(j).coefficients.b3 > 0))
                {
                    VALLEYS_other++;
                }
            }
            assert(VALLEYS_other < peaks->size());

            // every pre-group describes a varying-size region of data in the entire mass spectrum.
            // this loop determines the range (in scans) that is relevant to this grouping
            pregroup.features.reserve(groupsize);
            pregroup.EICs.reserve(groupsize);
            size_t maxScan = 0;
            size_t minScan = 4294967295; // max value of size_t
            for (size_t j = limits[groupIdx].start; j < limits[groupIdx].end + 1; j++)
            {
                FeaturePeak *test = &(peaks->at(j));
                assert(test->scanPeakEnd - test->scanPeakStart >= 4);
                // auto binRTs = (*bins)[test->idxBin].rententionTimes;
                auto scans = (*bins)[test->idxBin].scanNumbers;
                maxScan = std::max(maxScan, size_t(test->scanPeakEnd)); // @todo scans should be their own type, same with indices
                minScan = std::min(minScan, size_t(test->scanPeakStart));

                assert(maxScan - minScan >= 4);
                if (test->scanPeakStart + test->idxCenter_offset >= test->scanPeakEnd - 1)
                {
                    ERRORCOUNTER++; // @todo this is something that should be prevented during feature construction, why isn't it?
                    groupsize--;
                    continue;
                }
                pregroup.features.push_back(test);
                pregroup.EICs.push_back(&(bins->at(test->idxBin)));
            }
            assert(maxScan < convertRT->size());
            if (groupsize < 2)
            {
                continue;
            }

            // create a vector of unified RTs for interpolation in the harmonised EICs
            // this uses the same scan -> time conversion as the feature construction
            // assert(minScan != 0);
            std::vector<float> unifiedRT(maxScan - minScan + 1, 0);
            for (size_t i = 0; i < unifiedRT.size(); i++)
            {
                unifiedRT[i] = convertRT->at(minScan + i);
            }

            std::vector<ReducedEIC> eics; // @todo the maximum size of a reduced EIC is the global maxscale + 1!
            for (size_t j = 0; j < groupsize; j++)
            {
                const FeaturePeak *feature = pregroup.features[j];
                const EIC *bin = &bins->at(feature->idxBin);
                assert(bin == pregroup.EICs[j]); // @todo clean this up
                eics.push_back(harmoniseEIC(feature, bin, &unifiedRT, minScan, maxScan));
                eics.back().feature_ID = j;
                assert(eics[j].intensity.size() == eics[0].intensity.size());
            }
            // At this stage, the EICs are in the correct shape for performing a multi-regression.
            // To make comparisons faster, the RSS is calculated per feature at every point in the block.
            // This means that the total RSS for a given sub-block is always calculated as the sum of RSS
            // in that region. It is important to note that the sub-block changes with the considered members,
            // as it only consists of the smallest region that contains all real points of the underlying features.
            // The RSS for all individual features over the current range has been calculated as part of harmoniseEIC().
#pragma endregion "Pre-Group"

#pragma region "Compare Pairs"
            // first, calculate the pairwise RSS in a sparse matrix. The RSS is set to INFINITY if it is worse
            // than the sum of both individual RSS values.
            // size: (x^2 -x) / 2 ; access: smaller Idx + (larger Idx * (larger Idx - 1)) / 2
            std::vector<RSS_pair> pairs((groupsize * groupsize - groupsize) / 2);

            for (size_t idx_S = 0; idx_S < groupsize - 1; idx_S++)
            {
                auto EIC_A = eics[idx_S];
                for (size_t idx_L = idx_S + 1; idx_L < groupsize; idx_L++)
                {
                    size_t access = idx_S + (idx_L * (idx_L - 1)) / 2; // index of the half matrix where the pair sits
                    pairs[access].idx_S = idx_S;
                    pairs[access].idx_L = idx_L;

                    auto EIC_B = eics[idx_L];
                    {
                        // before performing the computationally expensive check by regression, we can exclude features
                        // that don't overlap. This will massively reduce the time spent on large pregroups
                        const float uncert_A = peaks->at(EIC_A.feature_ID).RT_Uncertainty;
                        const float apex_A = peaks->at(EIC_A.feature_ID).retentionTime;
                        const float uncert_B = peaks->at(EIC_B.feature_ID).RT_Uncertainty;
                        const float apex_B = peaks->at(EIC_B.feature_ID).retentionTime;

                        if (std::abs(apex_A - apex_B) > (uncert_A + uncert_B) / 2)
                        {
                            pairs[access].RSS = INFINITY;
                            failRegressions++;
                            continue;
                        }
                    }

                    // merge the EICs that are relevant to both
                    static const std::vector<size_t> select{0, 1}; // smallest RT is always left
                    size_t idxStart = std::min(EIC_A.featLim_L, EIC_B.featLim_L);
                    size_t idxEnd = std::max(EIC_A.featLim_R, EIC_B.featLim_R);
                    assert(idxEnd > idxStart);
                    std::vector<ReducedEIC> eics_pair{EIC_A, EIC_B};
                    auto mergedEIC = mergeEICs(&eics_pair, &select, idxStart, idxEnd);
                    auto regression = runningRegression_multi(&mergedEIC, &eics_pair, &select, idxStart, idxEnd, 2);

                    if (regression.b0_vec[0] == 0)
                    {
                        pairs[access].RSS = INFINITY;
                        failRegressions++;
                    }
                    else
                    {
                        realRegressions++;

                        assert(regression.scale <= MAXSCALE);

                        pairs[access].regression = regression; // this introduces some redundancy
                        pairs[access].idxStart = regression.idxStart;
                        pairs[access].idxEnd = regression.idxEnd;

                        pairs[access].cumRSS = regression.cum_RSS;
                        // return infinity if the regression does not work
                        pairs[access].RSS = simpleRSS(&regression.cum_RSS, &EIC_A.RSS_cum, &EIC_B.RSS_cum,
                                                      idxStart, idxEnd, 2, 8);
                    }
                }
            }
            // pairRSS serves as an exclusion matrix and priorisation tool. The component assignment is handled through
            // a group vector and stored assignment information
#pragma endregion "Compare Pairs"

#pragma region "Iterative Assign"
            int componentGroup = 0;
            std::vector<int> assignment(groupsize, -1); // -1 == unassigned
            std::vector<CompAssignment> components;     // index-based access of component RSS

            // all pairs are iterated through in ascending order of RSS (best -> worst)
            std::sort(pairs.begin(), pairs.end(), [](const RSS_pair lhs, const RSS_pair rhs)
                      { return lhs.RSS < rhs.RSS; });

            // size_t excludes = 0;
            size_t finalCompares = 0; // todo: this is often 0 or 1, check if it is too strict a criterium

            for (size_t i = 0; i < pairs.size(); i++) // @todo length of pairs = the number of RSS - infinity
            {
                auto p = pairs[i];
                if (p.RSS == INFINITY) // the pair cannot form a component by itself, so these features may never be assigned to the same component
                {
                    // excludes = pairs.size() - i;
                    break;
                }

                int *ass_L = &assignment[p.idx_L];
                int *ass_S = &assignment[p.idx_S];

                if (*ass_L == *ass_S)
                {
                    // if the component ID is not -1, both are assigned to the same component -> do nothing
                    if (*ass_L == -1)
                    {
                        finalCompares++;
                        // form a new component from the two features
                        *ass_L = componentGroup;
                        *ass_S = componentGroup;
                        CompAssignment insert{
                            p.regression,
                            p.cumRSS, // @todo this is a major design flaw (also see above)
                            p.idxStart,
                            p.idxEnd,
                            2, // number of peaks in this component
                            p.RSS,
                            componentGroup};
                        components.push_back(insert);
                        componentGroup++;
                    }
                }
                else
                { // @todo this can be condensed by merging the selection assignment and other stuff
                    finalCompares++;
                    if ((*ass_L == -1) || (*ass_S == -1))
                    {
                        // one feature is assigned, the other is not
                        bool singleLarge = *ass_L == -1;
                        int *unAss = singleLarge ? ass_L : ass_S;
                        // check if the unassigned feature can be assigned to a component.
                        // this will not lead to the creation of a new component!

                        // 1) create a selection vector that holds the feature IDs for merge
                        int existingComponent = singleLarge ? *ass_S : *ass_L;      // component ID of the assigned feature
                        size_t unassignedFeature = singleLarge ? p.idx_L : p.idx_S; // index of the unassigned feature
                        std::vector<size_t> selection(1, unassignedFeature);
                        for (size_t idx = 0; idx < groupsize; idx++)
                        {
                            if (assignment[idx] == existingComponent)
                            {
                                selection.push_back(idx); // @todo this is pretty inefficient
                            }
                        }
                        // this is necessary because otherwise, the values of b0 cannot be assigned to members. This way,
                        // they are always sorted by RT of the original feature
                        std::sort(selection.begin(), selection.end());

                        size_t idxStart = std::min(components[existingComponent].limit_L, eics[unassignedFeature].featLim_L);
                        size_t idxEnd = std::max(components[existingComponent].limit_R, eics[unassignedFeature].featLim_R);
                        assert(idxEnd > idxStart);
                        // 2) perform the multi-regression over the combined EIC for the selection
                        auto mergedEIC = mergeEICs(&eics, &selection, idxStart, idxEnd);
                        const size_t n = components[existingComponent].numPeaks;
                        if (n + 1 >= 32) [[unlikely]]
                        {
                            // @todo this is a hard limit due to the max amount of b0 coeffs we can store. It is only ever triggered by the pump error dataset
                            // note: the error only ever occurred with the data measured at the moment the pump broke
                            std::cout << "Warning: the number of component members exceeds the maximum number of features (32).\n";
                            ERRORCOUNTER++;
                            continue;
                        }
                        auto regression = runningRegression_multi(&mergedEIC, &eics, &selection,
                                                                  idxStart, idxEnd, n + 1);

                        // 3) check if there is a better RSS when combining the regressions, then merge if yes
                        // This is already handled in the simpleRSS function, so we just check for infinity (= no merge)
                        float newRSS = simpleRSS(&regression.cum_RSS,
                                                 &eics[unassignedFeature].RSS_cum,
                                                 &components[existingComponent].cumRSS,
                                                 idxStart, idxEnd, n + 1, n + 3 + 4);

                        if (newRSS < INFINITY)
                        {
                            // do nothing if the single feature doesn't fit
                            components[existingComponent].regression = regression;
                            components[existingComponent].cumRSS = regression.cum_RSS;
                            components[existingComponent].numPeaks++;
                            components[existingComponent].limit_L = regression.idxStart;
                            components[existingComponent].limit_R = regression.idxEnd;
                            components[existingComponent].RSS = newRSS;
                            *unAss = components[existingComponent].component;
                        }
                    }
                    else
                    {
                        // both features are assigned to different components
                        // the only relevant check here is for the combination of both into one new component
                        assert((*ass_L != -1) && (*ass_S != -1));
                        assert(*ass_L != *ass_S);

                        const size_t n = components[*ass_L].numPeaks + components[*ass_S].numPeaks;
                        if (n >= 32) [[unlikely]]
                        {
                            // @todo this is a hard limit due to the max amount of b0 coeffs we can store. It is only ever triggered by the pump error dataset
                            std::cout << "Warning: the number of component members exceeds the maximum number of features (32)\n";
                            ERRORCOUNTER++;
                            // note: occurs during pump error and frequently with SFC data
                            continue;
                        }

                        size_t idxStart = std::min(components[*ass_L].limit_L, eics[*ass_S].featLim_L);
                        size_t idxEnd = std::max(components[*ass_L].limit_R, eics[*ass_S].featLim_R);
                        assert(idxEnd > idxStart);

                        // add all feature IDs from both components
                        std::vector<size_t> selection;
                        for (size_t idx = 0; idx < groupsize; idx++)
                        {
                            if ((assignment[idx] == *ass_L) || (assignment[idx] == *ass_S))
                            {
                                selection.push_back(idx); // @todo this is pretty inefficient
                            }
                        }
                        // 2) perform the multi-regression over the combined EIC for the selection
                        // start and end indices need to be adjusted so that
                        auto mergedEIC = mergeEICs(&eics, &selection, idxStart, idxEnd);
                        auto regression = runningRegression_multi(&mergedEIC, &eics, &selection,
                                                                  idxStart, idxEnd, n);

                        // 3) check if there is a better RSS when combining the regressions, then merge if yes
                        // This is already handled in the simpleRSS function, so we just check for infinity (= no merge)
                        float newRSS = simpleRSS(&regression.cum_RSS,
                                                 &components[*ass_L].cumRSS,
                                                 &components[*ass_S].cumRSS,
                                                 idxStart, idxEnd, n, n + 6);

                        if (newRSS < INFINITY)
                        {
                            // do nothing if the two components cannot be merged
                            // always prefer the smaller indexed feature when assigning components.
                            // this does not have an effect on any comparisons, so not a relevant implementation detail
                            components[*ass_S].regression = regression;
                            components[*ass_S].cumRSS = regression.cum_RSS;
                            components[*ass_S].numPeaks = n;
                            components[*ass_S].limit_L = regression.idxStart;
                            components[*ass_S].limit_R = regression.idxEnd;
                            components[*ass_S].RSS = newRSS;
                            // invalidate the merged component
                            int removedID = *ass_L;
                            size_t counter = 0;
                            for (size_t idx = 0; idx < groupsize; idx++)
                            {
                                if ((assignment[idx] == removedID))
                                {
                                    counter++;
                                    assignment[idx] = *ass_S;
                                }
                            }
                            assert(counter == components[removedID].numPeaks);
                            components[removedID].numPeaks = 0;
                            components[removedID].RSS = INFINITY;
                            assert(*ass_L == *ass_S);
                        }
                        // @todo the regression goes out of scope here, add function-level storage for coefficients
                    }
                }
            } // outer if statement

            // std::cout << groupsize << "," << pairs.size() << "," << excludes << "," << finalCompares << "\n";

            assert(componentGroup > -1);
            assert((size_t(componentGroup)) <= groupsize);
#pragma endregion "Iterative Assign"

#pragma region "cleanup"
            // At this stage, some components could be invalid. Additionally, the component ID is local to the inner loop.
            // To produce user-facing data, the component ID is moved to a global level and added to the features themselves

            for (size_t comp = 0; comp < components.size(); comp++)
            {
                if (components[comp].numPeaks == 0)
                {
                    continue;
                }
                int compID = components[comp].component;
                std::vector<size_t> selection; // used for tanimoto later on
                for (size_t feat = 0; feat < groupsize; feat++)
                {
                    if (assignment[feat] == compID)
                    {
                        // update the feature that was passed by reference
                        pregroup.features[feat]->componentID = globalCompID;
                        pregroup.EICs[feat]->componentID = globalCompID;
                        selection.push_back(feat);
                        *featuresInComponents += 1;
                    }
                }
                finalComponents.push_back(components[comp].regression);
                // the tanimoto-score is calculated using the uniformly scaled intensity vectors of all data points in the region

                // this is just for debugging, remove later @todo
                for (size_t i = 0; i < selection.size(); i++)
                {
                    size_t dfCount = 0;
                    for (size_t idx = finalComponents.back().idxStart; idx < finalComponents.back().idxEnd + 1; idx++)
                    {
                        dfCount += eics[selection[i]].df[idx] ? 1 : 0;
                    }
                    assert(dfCount > 4);
                }

                finalComponents.back().DQS = tanimotoScore(&eics, &selection,
                                                           finalComponents.back().idxStart,
                                                           finalComponents.back().idxEnd);

                if (finalComponents.back().DQS == -1)
                {
                    // globalCompID--; // @todo decide if this should happen
                    // finalComponents.pop_back();
                    std::cerr << "Warning: in group " << groupIdx << ", the EICs associated with component "
                              << globalCompID << " are identical\n";
                }
                globalCompID++;
            }

#pragma endregion "cleanup"
        }
        // std::cout << std::endl;
        // std::cout << "1: " << VALLEYS_1 << " ; other: " << VALLEYS_other << "\n"; // at least for one dataset, features with a valley point are much more likely
        // // to be groups of size 1 than to be included in larger groups (ca. twice as likely)
        // std::cout << "fails: " << failRegressions << ", real ones: " << realRegressions << ", Errors: " << ERRORCOUNTER << "\n";
        failRegressions = 0;
        realRegressions = 0;
        VALLEYS_1 = 0;
        VALLEYS_other = 0;
        ERRORCOUNTER = 0;

        return finalComponents;
    }

    MergedEIC mergeEICs(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd)
    {
        // the values in selection must be unique! This isn't tested here @todo
        assert(selection->size() <= eics->size());
        assert(idxEnd > idxStart);
        size_t eicSize = eics->front().intensity.size();
        assert(idxEnd < eicSize);

        // {
        //     // this section ensures that no more tha the outermost two points on each end have no degrees of freedom
        //     // @todo a fundamental error with the way regression limits are passed into functions exists here
        //     size_t firstRealValFront = idxEnd;
        //     size_t lastRealValEnd = idxStart;
        //     for (size_t i = 0; i < selection->size(); i++)
        //     {
        //         const ReducedEIC *subEIC = &(eics->at(selection->at(i)));
        //         assert(subEIC->intensity.size() == eicSize);
        //         for (size_t idx = idxStart; idx < idxEnd; idx++)
        //         {
        //             if (subEIC->df[idx])
        //             {
        //                 firstRealValFront = firstRealValFront > idx ? idx : firstRealValFront;
        //             }
        //             break;
        //         }
        //         for (size_t idx = idxEnd; idx > idxStart; idx--)
        //         {
        //             if (subEIC->df[idx])
        //             {
        //                 lastRealValEnd = lastRealValEnd < idx ? idx : lastRealValEnd;
        //             }
        //             break;
        //         }
        //     }
        //     idxStart = std::max(idxStart, firstRealValFront);
        //     idxEnd = std::min(idxEnd, lastRealValEnd);
        // }

        size_t span = idxEnd - idxStart + 1;
        MergedEIC result;
        result.intensity.reserve(span);
        result.intensity_log.reserve(span);
        result.RSS_cum.reserve(span); // remember to overwrite this! It is no longer accurate after the regression completes
        result.df.reserve(span);
        result.DF_cum = std::vector<int>(span, 0);
        for (size_t i = 0; i < selection->size(); i++)
        {
            const ReducedEIC *subEIC = &(eics->at(selection->at(i)));
            assert(subEIC->intensity.size() == eicSize);
            for (size_t idx = idxStart; idx <= idxEnd; idx++)
            {
                result.intensity.push_back(subEIC->intensity[idx]);
                result.intensity_log.push_back(subEIC->intensity_log[idx]);
                result.df.push_back(subEIC->df[idx]);
                result.DF_cum[idx - idxStart]++;
            }
        }
        assert(result.DF_cum.front() != 0);
        assert(result.DF_cum.back() != 0);
        result.numPeaks = selection->size();
        result.peakFrame = span;
        result.groupIdxStart = idxStart;
        result.minScan = eics->front().minScan + idxStart;
        return result;
    }

    void makeValidRegression_multi(
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const std::vector<size_t> *cum_DF, // DF is cumulative over all combined harmonised EICs (between 0 and n DF per RT)
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log)
    {
        assert(scale > 1);
        assert(idxStart + 4 < cum_DF->size());
        assert(cum_DF->front() == 0);
        assert(cum_DF->size() == intensities->size() + 1);

        // there must be at least five real points of each feature within the region
        // of the regression

        /*
          Apex and Valley Position Filter:
          This block of code implements the apex and valley position filter.
          It calculates the apex and valley positions based on the coefficients
          matrix B. If the apex is outside the data range, the loop continues
          to the next iteration. If the apex and valley positions are too close
          to each other, the loop continues to the next iteration.
        */
        double valley_position = 0;
        // if (!calcApexAndValleyPos_old(mutateReg, scale, &valley_position))
        // {
        //     return; // invalid apex and valley positions
        // }
        /*
          Area Pre-Filter:
          This test is used to check if the later-used arguments for exp and erf
          functions are within the valid range, i.e., |x^2| < 25. If the test fails,
          the loop continues to the next iteration. @todo why 25?
          x is in this case -apex_position * b1 / 2 and -valley_position * b1 / 2.
        */
        if (mutateReg->apex_position * mutateReg->coeffs.b1 > 50 || valley_position * mutateReg->coeffs.b1 < -50)
        {
            return; // invalid area pre-filter
        }

        if (valley_position == 0)
        {
            // // no valley point exists
            // mutateReg->left_limit = idxStart;
            // mutateReg->right_limit = idxStart + 2 * scale;
        }
        else if (valley_position < 0)
        {
            size_t substractor = static_cast<size_t>(abs(valley_position));
            // mutateReg->left_limit = substractor < scale ? idxStart + scale - substractor : idxStart; // std::max(i, static_cast<int>(valley_position) + i + scale);
            // mutateReg->right_limit = idxStart + 2 * scale;
        }
        else
        {
            // mutateReg->left_limit = idxStart;
            // mutateReg->right_limit = std::min(idxStart + 2 * scale, static_cast<int>(valley_position) + idxStart + scale);
        }
        // assert(mutateReg->right_limit > mutateReg->left_limit);
        // assert(mutateReg->right_limit < intensities->size());
        // @todo this should be passed as a parameter for the sake of clarity
        const size_t idx_x0 = idxStart + scale;
        /*
            Note: left and right limit are not the limits of the regression, but of the window the regression applies in.
            When multiple regressions are combined, the window limits are combined by maximum.
        */

        // if (idx_x0 - mutateReg->left_limit < 2 || (mutateReg->right_limit - idx_x0 < 2))
        {
            // only one half of the regression applies to the data
            return;
        }

        // since the region within which the regression is formed has no preceding limit checks,
        // this step ensures that a regression is only valid if it does not require more than
        // three fully interpolated points to either side. Otherwise, a frequently occurring problem
        // is regressions preferring the baseline due to perfectly predictable intensities.
        // the number three is chosen since this is the maximum gap size in a bin. If the degrees
        // of freedom three points after the regression limit do not change, the regression has
        // a guaranteed incorrect range. Similarly, the degrees of freedom three points from the end of
        // the regression must differ from the last df value.
        // if (cum_DF->at(mutateReg->left_limit) == cum_DF->at(mutateReg->left_limit + 3))
        // {
        //     return;
        // }
        // if (cum_DF->at(mutateReg->right_limit) == cum_DF->at(mutateReg->right_limit - 3))
        // {
        //     return;
        // }

        // the degrees of freedom must exceed five for a valid fit. In the multi-model, this is
        // almost always true (change to 5 * num of peaks?)
        // the index in DF is the index of the feature limits + 1. Since cum_DF starts at 0, only the
        // right limit is increased by one for the DF check
        // size_t df_sum = cum_DF->at(mutateReg->right_limit + 1) - cum_DF->at(mutateReg->left_limit);
        // if (df_sum < 5)
        // {
        //     return;
        // }

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates
          the ratio of the apex signal to the edge signal and ensures that the
          ratio is greater than 2. This is a pre-filter for later
          signal-to-noise ratio checkups. apexToEdge is also required in isValidPeakHeight further down
        */
        size_t idxApex = (size_t)std::round(mutateReg->apex_position) + idx_x0;
        // float apexToEdge = apexToEdgeRatio(mutateReg->left_limit, idxApex, mutateReg->right_limit, intensities);
        // this check has been removed because it didn't account for adequate signal in parallel traces
        // if (!(apexToEdge > 2))
        // {
        //     return; // invalid apex to edge ratio
        // }

        /*
          Quadratic Term Filter:
          This block of code implements the quadratic term filter. It calculates
          the mean squared error (MSE) between the predicted and actual values.
          Then it calculates the t-value for the quadratic term. If the t-value
          is less than the corresponding value in the T_VALUES, the quadratic
          term is considered statistically insignificant, and the loop continues
          to the next iteration.
        */

        // std::vector<float> selectLog; // both vetors are used to transfer relevant values to the F test later
        // std::vector<float> predictLog;
        // selectLog.reserve(mutateReg->right_limit - mutateReg->left_limit + 1);
        // predictLog.reserve(mutateReg->right_limit - mutateReg->left_limit + 1);
        // float mse = calcRSS_reg(mutateReg->coeffs, intensities_log,
        //                          mutateReg->left_limit, mutateReg->right_limit, idx_x0);

        /*
        competing regressions filter:
        If the real distribution of points could also be described as a continuum (i.e. only b0 is relevant),
        the regression does not describe a peak. This is done through a nested F-test against a constant that
        is the mean of all predicted values. @todo this is not working correctly!
        */
        // float regression_Fval = calcRegressionFvalue(&selectLog, &predictLog, mse, mutateReg->coeffs.b0);
        // if (regression_Fval < F_VALUES[selectLog.size()]) // - 5 since the minimum is five degrees of freedom
        // {
        //     // H0 holds, the two distributions are not noticeably different
        //     return;
        // }
        // mse is only the correct mean square error after this division
        // mse /= (df_sum - 4);

        // if (!isValidQuadraticTerm(mutateReg->coeffs, scale, mse, df_sum))
        // {
        //     return; // statistical insignificance of the quadratic term
        // }
        // if (!isValidPeakArea(mutateReg->coeffs, mse, scale, df_sum))
        // {
        //     return; // statistical insignificance of the area
        // }
        /*
          Height Filter:
          This block of code implements the height filter. It calculates the height
          of the peak based on the coefficients matrix B. Then it calculates the
          uncertainty of the height based on the Jacobian matrix and the variance-covariance
          matrix of the coefficients. If the height is statistically insignificant,
          the loop continues to the next iteration.
        */

        // calcPeakHeightUncert(mutateReg, mse, scale);
        // if (1 / mutateReg->uncertainty_height <= T_VALUES[df_sum - 5]) // statistical significance of the peak height
        // {
        //     return;
        // }
        // at this point without height, i.e., to get the real uncertainty
        // multiply with height later. This is done to avoid exp function at this point
        // if (!isValidPeakHeight(mse, scale, mutateReg->apex_position, valley_position, df_sum, apexToEdge))
        {
            return; // statistical insignificance of the height
        }

        /*
          Area Filter:
          This block of code implements the area filter. It calculates the Jacobian
          matrix for the peak area based on the coefficients matrix B. Then it calculates
          the uncertainty of the peak area based on the Jacobian matrix. If the peak
          area is statistically insignificant, the loop continues to the next iteration.
          NOTE: this function does not consider b0: i.e. to get the real uncertainty and
          area multiply both with Exp(b0) later. This is done to avoid exp function at this point
        */
        // it might be preferential to combine both functions again or store the common matrix somewhere
        // calcPeakAreaUncert(mutateReg, mse, scale);

        // if (mutateReg->area / mutateReg->uncertainty_area <= T_VALUES[df_sum - 5])
        {
            return; // statistical insignificance of the area
        }

        /*
          Chi-Square Filter:
          This block of code implements the chi-square filter. It calculates the chi-square
          value based on the weighted chi squared sum of expected and measured y values in
          the exponential domain. If the chi-square value is less than the corresponding
          value in the CHI_SQUARES, the regression is invalid.
        */
        // float chiSquare = calcSSE_chisqared(mutateReg->coeffs, intensities, mutateReg->left_limit, mutateReg->right_limit, idx_x0);
        // if (chiSquare < CHI_SQUARES[df_sum - 5])
        {
            return; // statistical insignificance of the chi-square value
        }

        // mutateReg->uncertainty_pos = calcUncertaintyPos(mse, mutateReg->coeffs, mutateReg->apex_position, scale);
        // mutateReg->df = df_sum - 4; // @todo add explanation for -4
        mutateReg->apex_position += float(idx_x0);
        mutateReg->coeffs.scale = scale;
        // mutateReg->idxCenter = int(idx_x0);
        // mutateReg->mse = mse; // the quadratic mse is used for the weighted mean of the coefficients later
        mutateReg->isValid = true;
        return;
    }

    void vecSum(std::vector<float> *base, const std::vector<float> *add)
    {
        assert(base->size() == add->size());
        for (size_t i = 0; i < base->size(); i++)
        {
            base->at(i) += add->at(i);
        }
    }

    size_t calcDF_single( // @todo change this so that cumulative degrees of freedom are used
        const std::vector<bool> *degreesOfFreedom,
        size_t left_limit,
        size_t right_limit)
    {
        size_t DF = 0;
        for (size_t i = left_limit; i < right_limit + 1; i++)
        {
            if ((*degreesOfFreedom)[i])
            {
                ++DF;
            }
        }
        return DF;
    }

    MultiRegression runningRegression_multi( // add function that combines multiplr eics and updates the peak count
        const MergedEIC *eic,
        const std::vector<ReducedEIC> *eics,
        const std::vector<size_t> *selection,
        const size_t idxStart,
        const size_t idxEnd,
        // const size_t maxScale,
        const size_t numPeaks)
    // const size_t peakFrame)
    {
        assert(numPeaks < 32);
        size_t peakFrame = (idxEnd - idxStart + 1);
        size_t maxScale = (peakFrame - 1) / 2 > MAXSCALE ? MAXSCALE : (peakFrame - 1) / 2;

        assert(selection->size() == eic->numPeaks);
        assert(eic->numPeaks == numPeaks);
        // regressions for every possible scale and window position
        std::vector<MultiRegression> regressions = findCoefficients_multi(&(eic->intensity_log), maxScale, numPeaks, peakFrame);
        // bookkeeping: which regressions are viable?
        std::vector<bool> regressionOK(regressions.size(), false);
        // used to decide on the best overall regression
        std::vector<double> sum_MSE(regressions.size(), 0);

        // for every multi-regression, reconstruct the normal regression and validate. If any derived regression
        // is invalid, the multi-regression is also invalid.
        // we know that the correct interpretation of the component always involves only one regression.
        // since the merge over scales is required to isolate multiple peaks within the same block from
        // each other, it would always apply to the special case of all regressions being in the same window here.
        // this allows us to simplify the program such that the best regression regardless of scale is chosen.
        // the validateRegression function does not account for that special case, but only the validation step
        // is actually required here. as such, this code has been adapted from the implementation in qPeaks.

        // the validation function expects the intensity and degree of freedom data to arrive as a vector
        // for every singular peak.
        std::vector<std::vector<bool>> DF_vecs(numPeaks);
        std::vector<std::vector<float>> intensity_vecs(numPeaks);
        std::vector<std::vector<float>> logInt_vecs(numPeaks);

        std::vector<size_t> DF_cum(peakFrame + 1, 0);
        for (size_t i = 0; i < eic->df.size(); i++)
        {
            size_t access = i % peakFrame;
            DF_cum[access + 1] += eic->df[i];
        }
        // cumulative dfs start at 0 to avoid conditional later
        for (size_t i = 1; i < DF_cum.size(); i++)
        {
            DF_cum[i] += DF_cum[i - 1];
        }

        for (size_t i = 0; i < numPeaks; i++)
        {
            size_t idxBegin = i * peakFrame;
            size_t idxEnd_inner = (i + 1) * peakFrame;
            logInt_vecs[i] = std::vector<float>(eic->intensity_log.begin() + idxBegin, eic->intensity_log.begin() + idxEnd_inner);
            intensity_vecs[i] = std::vector<float>(eic->intensity.begin() + idxBegin, eic->intensity.begin() + idxEnd_inner);
            DF_vecs[i] = std::vector<bool>(eic->df.begin() + idxBegin, eic->df.begin() + idxEnd_inner); // @todo this does not serve a purpose
        }

        for (size_t multiReg = 0; multiReg < regressions.size(); multiReg++)
        {
            // create the set of individual regressions (only one b0 each) and provide the individual
            // vectors for EIC data, then test the regression for validity
            auto reg = regressions[multiReg];
            size_t idxRegStart = reg.idxStart;
            size_t scale = reg.scale;
            assert(scale <= maxScale);

            RegCoeffs coeff{0, reg.b1, reg.b2, reg.b3};
            if ((coeff.b1 == 0) || (coeff.b2 == 0) || (coeff.b3 == 0))
            {
                regressionOK[multiReg] = false;
                continue;
            }
            // limits are adjusted when assigning validity
            regressions[multiReg].idxStart = idxStart;
            regressions[multiReg].idxEnd = idxEnd;
            regressions[multiReg].idx_x0 = idxRegStart + scale; // this is always in relation to the complete regression window

            for (size_t i = 0; i < numPeaks; i++)
            {
                coeff.b0 = reg.b0_vec[i];
                RegressionGauss testCase;
                testCase.coeffs = coeff; // @todo do this during initialisation

                size_t df = calcDF_single(&(DF_vecs[i]), idxRegStart, 2 * scale + idxRegStart); // @todo switch to cumulative array
                if (df < 5)
                {
                    regressionOK[multiReg] = false;
                    break;
                    // continue;
                }
                makeValidRegression_multi(&testCase, idxRegStart, scale, &DF_cum, &(intensity_vecs[i]), &(logInt_vecs[i]));
                // @todo adjust the regression limits if positive coefficients are relevant
                if (!testCase.isValid)
                {
                    regressionOK[multiReg] = false;
                    break;
                    // continue;
                }

                // check degrees of fredom again with updated limits
                // df = calcDF_single(&(eics->at(selection->at(i)).df), testCase.left_limit, testCase.right_limit);
                if (df < 5)
                {
                    regressionOK[multiReg] = false;
                    break;
                }
                assert(regressions[multiReg].scale <= MAXSCALE);

                // regressions[multiReg].idxStart = std::max(size_t(testCase.left_limit), regressions[multiReg].idxStart);
                // regressions[multiReg].idxEnd = std::min(size_t(testCase.right_limit), regressions[multiReg].idxEnd);

                auto selDF = eics->at(selection->at(i)).df;
                int countHits = 0;
                for (size_t idx = regressions[multiReg].idxStart; idx < regressions[multiReg].idxEnd + 1; idx++)
                {
                    countHits += selDF[idx] ? 1 : 0;
                }
                if (countHits < 5)
                {
                    regressionOK[multiReg] = false;
                    break;
                }

                // now that only regressions which are logically sensible are present, pick the best one. This uses the sum
                // of all mse values for the given regression, similar to qPeaks. Different from the previous validation,
                // the RSS is calculated cumulatively over the entire possible region
                // double mse = calcSSE_exp(coeff,
                //                          &(intensity_vecs[i]),
                //                          0,
                //                          intensity_vecs[i].size() - 1,
                //                          testCase.idxCenter);
                // sum_MSE[multiReg] += mse / (testCase.df - numPeaks - 3);

                regressionOK[multiReg] = true;
            }
        }
        // decide on the best regression @todo do some statistics regarding how different mse values actually are, then remove
        // the storage vector and replace it with a current minimum + index tracker
        double min_MSE = INFINITY;
        int minIdx = -1;
        for (size_t j = 0; j < regressions.size(); j++)
        {
            if (regressionOK[j] && min_MSE > sum_MSE[j])
            {
                bool regOK = true;
                for (size_t i = 0; i < selection->size(); i++)
                {
                    size_t dfCount = 0;
                    std::vector<bool> df = eics->at(selection->at(i)).df;
                    for (size_t idx = regressions[i].idxStart; idx < regressions[i].idxEnd + 1; idx++)
                    {
                        dfCount += df[idx] ? 1 : 0;
                    }
                    if (dfCount < 5)
                    {
                        regOK = false;
                        break;
                    }
                }
                if (regOK)
                {
                    min_MSE = sum_MSE[j];
                    minIdx = j;
                }
            }
        }

        if (minIdx == -1)
        {
            // scale is 0 by default, use that to check
            MultiRegression a;
            a.numPeaks = 0;
            a.b0_vec[0] = 0;
            return a; // this will lead to problems further down @todo
        }
        // calculate the cumulative RSS for the entire block @todo this could lead to very large values and problems, make a test
        auto bestReg = regressions[minIdx];

        assert(bestReg.scale <= MAXSCALE);

        for (size_t i = 0; i < selection->size(); i++)
        {
            size_t dfCount = 0;
            std::vector<bool> df = eics->at(selection->at(i)).df;
            for (size_t idx = bestReg.idxStart; idx < bestReg.idxEnd + 1; idx++)
            {
                dfCount += df[idx] ? 1 : 0;
            }
            assert(dfCount > 4);
        }

        bestReg.cum_RSS = std::vector<float>(eics->front().intensity.size(), 0); // @todo this is ugly

        for (size_t i = 0; i < numPeaks; i++)
        {
            RegCoeffs coeff{bestReg.b0_vec[i], bestReg.b1, bestReg.b2, bestReg.b3};
            auto intensity = eics->at(selection->at(i)).intensity;
            std::vector<float> cumRSS_local = cumulativeRSS(&intensity, &coeff, bestReg.idx_x0);
            // the RSS for one mass trace is not useful for comparing multi-regressions, which is why the sum
            // over all different regressions is used. Sum of cumsums == cumsum of the sums
            vecSum(&bestReg.cum_RSS, &cumRSS_local);
        }
        assert(bestReg.idxEnd != bestReg.idxStart);
        bestReg.scanStart = eic->minScan + bestReg.idxStart;
        bestReg.scanEnd = eic->minScan + bestReg.idxEnd;

        // @todo remove diagnostics, add conditions: at least one DF per side of x0, at least five df in total per feature
        for (size_t i = 0; i < selection->size(); i++)
        {
            auto selDF = eics->at(selection->at(i)).df;
            int countHits = 0;
            for (size_t idx = bestReg.idxStart; idx < bestReg.idxEnd + 1; idx++)
            {
                countHits += selDF[idx] ? 1 : 0;
            }
            assert(countHits > 4);
        }

        return bestReg;
    }

    constexpr auto INV_ARRAY = initialize();

    std::vector<RegCoeffs> findCoefficients_multi_new(
        const std::vector<float> *intensity_log_b0,
        const std::vector<float> *intensity_log_sum, // note: consider not just summing up the extrapolated values
        const size_t numPeaks,
        const size_t maxScale) // This must be the same maxscale as the one used during extrapolation!
    /* ### allocations ###
       coeffs: allocation size known at function call
    */
    {
        assert(maxScale >= GLOBAL_MINSCALE);
        assert(maxScale <= MAXSCALE);

        const size_t length = intensity_log_sum->size();
        assert(length > 2 * maxScale); // at least one valid point

        const size_t regsPerScale = length - 2 * maxScale; // number of points with at least two real neighbours
        const size_t totalScales = maxScale - GLOBAL_MINSCALE + 1;
        const size_t iterationCount = regsPerScale * totalScales;

        std::vector<RegCoeffs> coeffs(iterationCount, {NAN, NAN, NAN, NAN});
        std::vector<float[MAX_COMPONENT_SIZE]> b0s(iterationCount);

        // the first and last n points of every region are extrapolated, where n is the maximum scale -2.
        // as such, the last point at which a regression is possible is maxscale points from the back.
        const size_t limit = length - maxScale;

        size_t scale5Count = 0;
        for (size_t center = maxScale; center < limit; center++)
        {
            const float *cen_sum = intensity_log_sum->data() + center; // this is initially the third real point
            double product_sum_b0 = cen_sum[-1] + cen_sum[0] + cen_sum[1];
            double product_sum_b1 = -cen_sum[-1] + cen_sum[1];
            double product_sum_b2 = cen_sum[-1];
            double product_sum_b3 = cen_sum[1];

            double single_sums_b0[MAX_COMPONENT_SIZE] = {0};
            for (size_t i = 0; i < numPeaks; i++)
            {
                const float *cen_inner = intensity_log_b0->data() + i * length + center;
                single_sums_b0[i] = cen_inner[-1] + cen_inner[0] + cen_inner[1];
            }

            // it is not possible to choose a different minscale since that would break the iterative sum
            for (size_t scale = GLOBAL_MINSCALE; scale <= maxScale; scale++)
            {
                product_sum_b0 += cen_sum[-scale] + cen_sum[scale];
                product_sum_b1 += -double(scale) * cen_sum[-scale] + double(scale) * cen_sum[scale];
                double scale_sqr = double(scale * scale);
                product_sum_b2 += scale_sqr * cen_sum[-scale];
                product_sum_b3 += scale_sqr * cen_sum[scale];

                for (size_t i = 0; i < numPeaks; i++)
                {
                    const float *cen_inner = intensity_log_b0->data() + i * length + center;
                    single_sums_b0[i] += cen_inner[-scale] + cen_inner[scale];
                }

                const double *inv = INV_ARRAY.data() + scale * 6;
                const double inv_A1 = inv[0] - 2 * INV_ARRAY[1] / numPeaks;
                // A2 is only defined for numPeaks > 1, set it to 0 to avoid conditional in loops
                const double inv_A2 = -2 * inv[1] / numPeaks;
                const double inv_B = inv[1] / numPeaks;
                const double inv_C = inv[2] / numPeaks;
                const double inv_D = inv[3] / numPeaks;
                const double inv_E = inv[4] / numPeaks;
                const double inv_F = inv[5] / numPeaks;

                const double inv_B_b0 = inv_B * product_sum_b0;
                const double inv_D_b1 = inv_D * product_sum_b1;

                const size_t access = regsPerScale * (scale - GLOBAL_MINSCALE) + scale5Count;
                // coeffs[access].b0 = inv_A * product_sum_b0 + inv_B * (product_sum_b2 + product_sum_b3);
                coeffs[access].b1 = inv_C * product_sum_b1 + inv_D * (product_sum_b2 - product_sum_b3);
                coeffs[access].b2 = inv_B_b0 + inv_D_b1 + inv_E * product_sum_b2 + inv_F * product_sum_b3;
                coeffs[access].b3 = inv_B_b0 - inv_D_b1 + inv_F * product_sum_b2 + inv_E * product_sum_b3;

                for (size_t i = 0; i < numPeaks; i++)
                {
                    b0s[access][i] = inv_A2 * product_sum_b0 + (inv_A1 - inv_A2) * single_sums_b0[i] +
                                     inv_B * (product_sum_b2 + product_sum_b3);
                }
            }
            scale5Count += 1;
        }

        // @todo also return b0s
        return coeffs;
    }

    std::vector<MultiRegression> findCoefficients_multi( // @todo add option for a minimum scale
        const std::vector<float> *intensity_log,
        const size_t max_scale, // maximum scale that will be checked. Should generally be limited by peakFrame
        const size_t numPeaks,
        const size_t peakFrame) // how many points are covered per peak? For single-peak data, this is the length of intensity_log
    {
        const size_t minScale = 2;
        const size_t steps = peakFrame - 2 * minScale; // iteration number at scale 2

        std::vector<float> y_array_sum(peakFrame, 0);
        for (size_t i = 0; i < intensity_log->size(); i++)
        {
            size_t sumIdx = i % peakFrame;               // modulo means that i = peakframe -> sumIdx = 0
            y_array_sum[sumIdx] += intensity_log->at(i); // total intensity for the combined regression at each RT step
        }

        //   this vector is for the inner loop and looks like:
        //   [scale_min, scale_min +1 , .... scale_max, ... scale_min +1, scale_min]
        //   length of vector: num_steps
        // the vector starts and ends with minScale and increases by one towards the middle until it reaches the scale value
        std::vector<size_t> maxInnerLoop(steps, max_scale);
        for (size_t i = 0; i + 2 < max_scale; i++) // +2 since smallest scale is 2
        {
            // @todo somewhat inefficient, design better iteration scheme
            size_t newVal = i + 2;
            maxInnerLoop[i] = newVal;
            size_t backIdx = maxInnerLoop.size() - i - 1;
            maxInnerLoop[backIdx] = newVal;
        }

        size_t iterationCount = std::accumulate(maxInnerLoop.begin(), maxInnerLoop.end(), 0) - maxInnerLoop.size(); // no range check necessary since every entry > 1

        std::vector<float> emptyRSS(numPeaks, NAN);
        MultiRegression localEmpty = {{0}, emptyRSS, 0, 0, 0, 0, 0, 0, numPeaks, NAN, NAN, NAN, NAN};
        std::vector<MultiRegression> coeffs(iterationCount, localEmpty);

        // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
        // The first n entries are contained in the b0 vector, one for each peak the regression is performed over.
        double tmp_product_sum_b0[32]{NAN}; // one fully separate b0 per peak
        double tmp_product_sum_b1;
        double tmp_product_sum_b2;
        double tmp_product_sum_b3;

        double sum_tmp_product_sum_b0 = 0; // sum of all elements in the tmp_product_sum_b0 vector

        size_t k = 0;
        for (size_t i = 0; i < steps; i++)
        {

            // move along the intensity_log (outer loop)
            // calculate the convolution with the kernel of the lowest scale (= 2), i.e. xT * intensity_log[i:i+4]
            // tmp_product_sum_b0 = intensity_log[i:i+5].sum(axis=0) // numPeaks rows of xT * intensity_log[i:i+4]
            for (size_t b0_elems = 0; b0_elems < numPeaks; b0_elems++)
            {
                size_t x = b0_elems * peakFrame + i;
                double tmpval = intensity_log->at(x) + intensity_log->at(x + 1) +
                                intensity_log->at(x + 2) + intensity_log->at(x + 3) +
                                intensity_log->at(x + 4); // b0 = 1 for all elements
                tmp_product_sum_b0[b0_elems] = tmpval;
                sum_tmp_product_sum_b0 += tmpval;
            }
            // sum_tmp_product_sum_b0 = std::accumulate(tmp_product_sum_b0.begin(), tmp_product_sum_b0.end(), 0.0);

            tmp_product_sum_b1 = 2 * (y_array_sum[i + 4] - y_array_sum[i]) + y_array_sum[i + 3] - y_array_sum[i + 1];
            tmp_product_sum_b2 = 4 * y_array_sum[i] + y_array_sum[i + 1];
            tmp_product_sum_b3 = 4 * y_array_sum[i + 4] + y_array_sum[i + 3];

            // A1 is modified  by -2 * b / numPeaks and not just /numPeaks
            // use [12 + ...] since the array is constructed for the accession arry[scale * 6 + (0:5)]
            const double S2_A1 = INV_ARRAY[12 + 0] - 2 * INV_ARRAY[12 + 1] / numPeaks;
            // @todo replace the array with a struct and an accessor function
            const double S2_A2 = -2 * INV_ARRAY[12 + 1] / numPeaks;
            const double S2_B = INV_ARRAY[12 + 1] / numPeaks;
            const double S2_C = INV_ARRAY[12 + 2] / numPeaks;
            const double S2_D = INV_ARRAY[12 + 3] / numPeaks;
            const double S2_E = INV_ARRAY[12 + 4] / numPeaks;
            const double S2_F = INV_ARRAY[12 + 5] / numPeaks;

            // this line is: a*t_i + b * sum(t without i)
            // inv_array starts at scale = 2
            for (size_t peak = 0; peak < numPeaks; peak++)
            {
                coeffs[k].b0_vec[peak] = S2_A2 * sum_tmp_product_sum_b0 + (S2_A1 - S2_A2) * tmp_product_sum_b0[peak] +
                                         S2_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
            }

            coeffs[k].b1 = S2_C * tmp_product_sum_b1 + S2_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
            coeffs[k].b2 = S2_B * sum_tmp_product_sum_b0 + S2_D * tmp_product_sum_b1 + S2_E * tmp_product_sum_b2 + S2_F * tmp_product_sum_b3;
            coeffs[k].b3 = S2_B * sum_tmp_product_sum_b0 - S2_D * tmp_product_sum_b1 + S2_F * tmp_product_sum_b2 + S2_E * tmp_product_sum_b3;

            coeffs[k].scale = 2;
            coeffs[k].idxStart = i;

            k += 1;       // update index for the productsums array
            size_t u = 1; // u is the expansion increment
            for (size_t scale = 3; scale < maxInnerLoop[i] + 1; scale++)
            { // minimum scale is 2. so we start with scale + 1 = 3 in the inner loop
                coeffs[k].scale = scale;
                coeffs[k].idxStart = i - u;
                size_t scale_sqr = scale * scale;
                // expand the kernel to the left and right of the intensity_log.
                sum_tmp_product_sum_b0 = 0;
                for (size_t peak = 0; peak < numPeaks; peak++)
                {
                    tmp_product_sum_b0[peak] += intensity_log->at(i - u) + intensity_log->at(i + 4 + u);
                    sum_tmp_product_sum_b0 += tmp_product_sum_b0[peak];
                }

                tmp_product_sum_b1 += scale * (y_array_sum[i + 4 + u] - y_array_sum[i - u]);
                tmp_product_sum_b2 += scale_sqr * y_array_sum[i - u];
                tmp_product_sum_b3 += scale_sqr * y_array_sum[i + 4 + u];

                // A1 is not modified if numPeaks is 1. It is, however, modified by -2 * b / numPeaks and not just /numPeaks if numPeaks > 1
                const double inv_A1 = INV_ARRAY[12 + u * 6 + 0] - 2 * INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                // A2 is only defined for numPeaks > 1, set it to 0 to avoid conditional in loops
                const double inv_A2 = -2 * INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                const double inv_B = INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                const double inv_C = INV_ARRAY[12 + u * 6 + 2] / numPeaks;
                const double inv_D = INV_ARRAY[12 + u * 6 + 3] / numPeaks;
                const double inv_E = INV_ARRAY[12 + u * 6 + 4] / numPeaks;
                const double inv_F = INV_ARRAY[12 + u * 6 + 5] / numPeaks;

                for (size_t peak = 0; peak < numPeaks; peak++)
                {
                    coeffs[k].b0_vec[peak] = inv_A2 * sum_tmp_product_sum_b0 + (inv_A1 - inv_A2) * tmp_product_sum_b0[peak] +
                                             inv_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
                }

                coeffs[k].b1 = inv_C * tmp_product_sum_b1 + inv_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
                coeffs[k].b2 = inv_B * sum_tmp_product_sum_b0 + inv_D * tmp_product_sum_b1 + inv_E * tmp_product_sum_b2 + inv_F * tmp_product_sum_b3;
                coeffs[k].b3 = inv_B * sum_tmp_product_sum_b0 - inv_D * tmp_product_sum_b1 + inv_F * tmp_product_sum_b2 + inv_E * tmp_product_sum_b3;

                u += 1; // update expansion increment
                k += 1; // update index for the productsums array
            }
        }

        return coeffs;
    }

    // @todo this funtion should be generalised to qPeaks also
    double residual(const RegCoeffs *coeff, float realInt, size_t idxCenter, size_t index, bool left)
    {
        float b23 = left ? coeff->b2 : coeff->b3;
        float maxHeight = INFINITY;
        if (b23 > 0) [[unlikely]]
        {
            // use the other peak half for the maximum
            float maxPos = -coeff->b1 / (2 * b23);
            maxHeight = std::exp(coeff->b0 + (coeff->b1 + maxPos * b23) * maxPos); // correct for positive exponential
        }

        double new_x = double(index) - double(idxCenter);
        double y_base = std::exp(coeff->b0 + (coeff->b1 + b23 * new_x) * new_x);
        // in case a positive exponent half is in the new window, we need to prevent excessive distrotion from the exponential part
        y_base = y_base <= maxHeight ? y_base : maxHeight / (new_x + 1); // halve with the first step from idxCenter
        double RS = (y_base - realInt) * (y_base - realInt);             // residual square
        assert(RS < INFINITY);
        return (RS);
    }

    std::vector<float> cumulativeRSS(const std::vector<float> *intensities,
                                     const RegCoeffs *coeff,
                                     size_t idxCenter)
    {
        assert(idxCenter < intensities->size() - 1);
        std::vector<float> result(intensities->size(), 0);

        result[0] = residual(coeff, intensities->front(), idxCenter, 0, true) + 0;

        // left half - 1
        for (size_t i = 1; i < idxCenter; i++)
        {
            result[i] = residual(coeff, intensities->at(i), idxCenter, i, true) + result[i - 1];
        }
        // center point
        float diff = std::exp((coeff->b0)) - intensities->at(idxCenter); // this will never be infinity
        result[idxCenter] = diff * diff + result[idxCenter - 1];
        // right half
        for (size_t i = idxCenter + 1; i < intensities->size(); i++)
        {
            result[i] = residual(coeff, intensities->at(i), idxCenter, i, false) + result[i - 1];
        }
        assert(result.back() < INFINITY);
        return result;
    }

    ReducedEIC harmoniseEIC(const FeaturePeak *feature,
                            const EIC *bin,
                            const std::vector<float> *RTs,
                            const size_t minScan, // minimum overall scan in the subgroup
                            const size_t maxScan) // maximum overall scan in the subgroup
    {
        size_t minIdx = feature->idxBinStart;
        size_t maxIdx = feature->idxBinEnd;
        assert(feature->scanPeakStart >= minScan);
        assert(maxIdx < bin->ints_area.size());
        assert(minScan < bin->scanNumbers.back());
        assert(maxScan > bin->scanNumbers.front());
        // this function creates harmonised EICs, meaning that it can be used for the running regression
        // within a component subgroup without further modification. The process is as follows:
        // 1) create a results vector for every element which is of a size known at the time of function call

        const size_t length = maxScan - minScan + 1;
        assert(length > 4);
        assert(RTs->size() == length);
        assert(bin->ints_area[minIdx] != 0);
        assert(bin->ints_area[maxIdx] != 0);

        // scan relates to the complete measurement and idx to the position within the bin

        // find idxCenter by finding the corresponding retention time
        // size_t featLim_L = bin->scanNumbers[minIdx] - minScan;
        // size_t featLim_R = bin->scanNumbers[maxIdx] - minScan;
        // assert(featLim_L < featLim_R);
        const size_t scanShift = feature->scanPeakStart - minScan; // offset of feature limits relating to the first element of the harmonised EIC
        size_t idxCenter = feature->idxCenter_offset + scanShift;  // absolute scan of x0 - begin of scan region
        assert(idxCenter < length - 2);

        ReducedEIC reduced{
            std::vector<float>(length, 0),   // intensity
            std::vector<float>(length, 0),   // log intensity
            std::vector<size_t>(length, 0),  // scan number
            std::vector<float>(length, 0),   // RSS_cum
            std::vector<bool>(length, true), // degrees of freedom
            // std::vector<int>(length, 0),
            0,               // feature ID is only initialised after function execution
            feature->idxBin, // bin ID
            minScan,

            scanShift, // these limits are relating to the fully interpolated EIC
            feature->scanPeakEnd - minScan,
            idxCenter,
            // these limits will lead to nonsense behaviour if not set
            length - 1,
            0};

        std::iota(reduced.scanNo.begin(), reduced.scanNo.end(), minScan);

        // 2) transfer all points that will not be interpolated.

        for (size_t i = 0; i < bin->ints_area.size(); i++) // indices in relation to bin without interpolations
        {
            size_t scan = bin->scanNumbers[i];
            if (scan < minScan)
            {
                continue;
            }
            else if (scan > maxScan)
            {
                break;
            }

            size_t relIdx = scan - minScan;
            assert(bin->ints_area[i] > 0);
            reduced.intensity[relIdx] = bin->ints_area[i];
            reduced.intensity_log[relIdx] = log(bin->ints_area[i]);
        }
        // add points from outside the window?

        // 3) interpolate all intensities at 0
        for (size_t i = 0; i < length; i++)
        {
            if (reduced.intensity_log[i] == 0)
            { // value needs to be interpolated
                float b23 = i < idxCenter ? feature->coefficients.b2 : feature->coefficients.b3;
                double xval = double(i) - double(idxCenter);
                float predictedInt = feature->coefficients.b0 + xval * feature->coefficients.b1 + xval * xval * b23;
                if (b23 > 0) [[unlikely]]
                {
                    // stopgap solution for positive coefficients, this is very likely a suboptimal idea @todo
                    if (i + minScan < feature->scanPeakStart)
                    {
                        predictedInt = reduced.intensity_log[reduced.featLim_L];
                    }
                    else if (i + minScan > feature->scanPeakEnd)
                    {
                        predictedInt = reduced.intensity_log[reduced.featLim_R];
                    }
                }
                // @todo this is a poor solution, but probably better than having a baseline at zero
                predictedInt = predictedInt > lowestAreaLog ? predictedInt : lowestAreaLog;

                reduced.intensity_log[i] = predictedInt;
                reduced.intensity[i] = std::exp(predictedInt);
                assert(reduced.intensity[i] > 0);
                reduced.df[i] = false;
            }
        }
        // 4) calculate the RSS for all transferred points against the moved regression and write them into
        //    the 0-filled vector for cumRSS. Then, take the cumsum over the vector.
        reduced.RSS_cum = cumulativeRSS(&reduced.intensity, &feature->coefficients, idxCenter);
        assert(reduced.RSS_cum.back() != INFINITY);

        // set the inclusion limits by finding the fifth point from either side that is not interpolated
        {
            size_t realCountL = 0;
            for (size_t i = 0; i < length; i++)
            {
                if (reduced.df[i])
                {
                    realCountL++;
                }
                if (realCountL == 5)
                {
                    reduced.minIncludedIndex = i;
                    break;
                }
            }
            size_t realCountR = 0;
            for (size_t i = length - 1; i > 0; i--)
            {
                if (reduced.df[i])
                {
                    realCountR++;
                }
                if (realCountR == 5)
                {
                    reduced.maxIncludedIndex = i;
                    break;
                }
            }
        }
        return reduced;
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
            assert((*peaks)[i].componentID == 0);
            float apex_L = (*peaks)[i - 1].retentionTime;
            float apex_R = (*peaks)[i].retentionTime;
            assert(apex_L <= apex_R);

            float uncert = ((*peaks)[i - 1].RT_Uncertainty + (*peaks)[i].RT_Uncertainty) / 2;
            if (apex_R - apex_L > uncert)
            {
                initialGroups.back().end = i - 1;
                initialGroups.push_back({i, 0});
            }
        }
        initialGroups.back().end = peaks->size() - 1;
        return initialGroups;
    }

    bool preferMerge(float rss_complex, float rss_simple, size_t n_total, size_t p_complex)
    {
        // @todo there is a failure here despite the simple
        assert(rss_complex < rss_simple);
        // @todo consider if this part of the code can be sped up by hashing the computations dependent on dfn and dfd
        float alpha = 0.05; // @todo is a set alpha really the best possible solution?
        // problem: pre-calculation of all relevant f values could result in a very large array
        // possible max size of 20 seems reasonable, maximum observed is 6
        int which = 1; // select mode of library function and check computation result
        double p = 0;  // not required
        double q = 0;  // return value, equals p - 1
        double F = ((rss_simple - rss_complex) / 3) / (rss_complex / float(n_total - p_complex));
        double dfn = 3;                   // numerator degrees of freedom is always 3, since the simple model has three coeffs less (simplification due to pairwise comparison)
        double dfd = n_total - p_complex; // denominator degrees of freedom
        int status = 1;                   // result invalid if this is not 0
        double bound = 0;                 // allows recovery from non-0 status

        // @todo replace with lookup table
        cdff(&which, &p, &q, &F, &dfn, &dfd, &status, &bound); // library function, see https://people.math.sc.edu/Burkardt/cpp_src/cdflib/cdflib.html
        assert(status == 0);

        return q > alpha; // the merged model is not worse than the complex version, both can be merged
    }

    float simpleRSS(std::vector<float> *RSS_simple_cum,
                    std::vector<float> *RSS_complex_cum_A,
                    std::vector<float> *RSS_complex_cum_B,
                    size_t idxStart,
                    size_t idxEnd,
                    size_t peakCount,
                    size_t p_complex)
    {
        if (RSS_simple_cum->empty())
        {
            return INFINITY; // @todo this is a bad idea
        }

        assert(RSS_simple_cum->size() == RSS_complex_cum_A->size());
        assert(RSS_simple_cum->size() == RSS_complex_cum_B->size());
        assert(idxStart < idxEnd);

        bool zero = idxStart == 0;

        float substract_S = zero ? 0 : RSS_simple_cum->at(idxStart - 1);
        float substract_cA = zero ? 0 : RSS_complex_cum_A->at(idxStart - 1);
        float substract_cB = zero ? 0 : RSS_complex_cum_B->at(idxStart - 1);

        float RSS_simple = RSS_simple_cum->at(idxEnd) - substract_S;
        float RSS_complex = RSS_complex_cum_A->at(idxEnd) - substract_cA +
                            RSS_complex_cum_B->at(idxEnd) - substract_cB;

        assert(RSS_complex < INFINITY); // @todo this is not a good solution to the positive coefficient problem

        // it is possible for the simple model to have a lower RSS since the feature
        // applies to a smaller area than it and has worse performance outside its original
        // region than the combined one. @todo consider if it makes sense to propose an alternative
        // set of simple models that all share the larger region

        if (RSS_simple <= RSS_complex) // there is one recorded case where this was equal
        {
            // the simple model is inherently better, no F-test needed
            // @todo make this the information criterion
            return RSS_simple;
        }
        // F-test
        size_t numPoints = (idxEnd - idxStart + 1) * peakCount;
        bool merge = preferMerge(RSS_complex, RSS_simple, numPoints, p_complex);
        if (merge)
        {
            return RSS_simple;
        }
        else
        {
            return INFINITY;
        }
    }

    // helper function: produce scaled intensity vector within the limits
    inline std::vector<float> euclidianNorm(const std::vector<float> *intensity,
                                            size_t idxStart,
                                            size_t idxEnd)
    {
        // the vector is normed by dividing every element by the length
        size_t numElements = idxEnd - idxStart + 1;

        float length = 0;
        for (size_t i = 0; i < numElements; i++)
        {
            float int_i = intensity->at(i + idxStart);
            assert(int_i > 0);
            length += int_i * int_i;
        }
        length = sqrt(length);

        std::vector<float> result(numElements, 0);
        for (size_t i = 0; i < numElements; i++)
        {
            result[i] = intensity->at(i + idxStart) / length;
        }
        return result; // @todo no reason to not mutate the reference / another reference
    }

    float tanimotoScore(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd)
    {
        // first, the interpolated vectors must be scaled for the tanimoto score to make sense.
        // to this end, the euclidean norms of the vectors are compared. The total score is the
        // combination of individual scores, but how?

        // new score: gini coeff of the standard deviation of normalised intensity at every point
        // This should be more sensitive to outliers than the tanimoto and less sensitive to the absolute error

        // https://arxiv.org/pdf/2411.07983 // this was decided against since it will not work for components with two elements

        // solution to the multi-reg problem: Always take minimum and maximum of all points at a given RT / scan number

        // 1) scale all regressions to vector length 1
        // this function discards all non-intensity information since no gaps exist in reduced EICs
        assert(idxEnd > idxStart);
        size_t numFeats = selection->size();
        assert(numFeats > 1);
        assert(numFeats <= eics->size());
        size_t length = idxEnd - idxStart + 1;

        std::vector<std::vector<float>> scaledIntensities(numFeats, std::vector<float>(length, 0));

        for (size_t i = 0; i < numFeats; i++)
        {
            size_t id = selection->at(i);
            scaledIntensities[i] = euclidianNorm(&(eics->at(id).intensity), idxStart, idxEnd);
        }

        // first attempt: use the square sums || comclusion: not really functional
        double minSQ = 0;
        double maxSQ = 0;
        double minXmax = 0;
        for (size_t element = 0; element < length; element++)
        {
            // min and max of the scaled intensities @todo cache inefficient, consider storing the data differently
            double min = INFINITY;
            double max = 0;
            for (size_t feat = 0; feat < numFeats; feat++)
            {
                double val = scaledIntensities[feat][element];
                min = min < val ? min : val;
                max = max > val ? max : val;
            }
            minSQ += min * min;
            maxSQ += max * max;
            minXmax += min * max;
        }

        // second attempt: use the unmodified area (harmonised scale means every x is 1, so the area is the mean of i and i+1.
        // since every i is accessed twice, the sum of all minima / maxima - the outermost two values is the final area)
        double area_min = 0;
        double area_max = 0;
        for (size_t element = 0; element < length; element++)
        {
            // min and max of the scaled intensities @todo cache inefficient, consider storing the data differently
            double min = INFINITY;
            double max = 0;
            for (size_t feat = 0; feat < numFeats; feat++)
            {
                double val = scaledIntensities[feat][element];
                min = min < val ? min : val;
                max = max > val ? max : val;
            }
            area_min += min;
            area_max += max;
        }
        {
            // outer vals
            double min_L = INFINITY;
            double max_L = 0;
            double min_R = INFINITY;
            double max_R = 0;
            for (size_t feat = 0; feat < numFeats; feat++)
            {
                double val_L = scaledIntensities[feat].front();
                double val_R = scaledIntensities[feat].back();
                min_L = min_L < val_L ? min_L : val_L;
                max_L = max_L > val_L ? max_L : val_L;
                min_R = min_R < val_R ? min_R : val_R;
                max_R = max_R > val_R ? max_R : val_R;
            }
            area_min += (min_L + min_R) / 2;
            area_max += (max_L + max_R) / 2;
        }

        double DQS_new = area_min / area_max;
        // assert(DQS_new < 1);
        if (DQS_new >= 1)
        {
            ERRORCOUNTER++;
            return -1;
        }

        // double DQS = minXmax / (minSQ + maxSQ - minXmax);
        // @todo control why extremely small values occur here
        // std::cout << DQS << ", " << DQS_new << "   ||   ";
        return DQS_new;
    }
}
