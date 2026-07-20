#include "qalgorithms_qpeaks.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h"
#include "qalgorithms_utils.h"

#include "libcerf_reduced.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define _USE_MATH_DEFINES // relevant for windows to have math constants
#include <math.h>
#include <vector>

#include <algorithm> // sorting @todo get rid of this

namespace qAlgorithms
{
    static void retransformPeaks(
        const std::vector<RegressionGauss> *peaks,
        const float *x_values,
        const size_t numPeaks,
        std::vector<PeakFit> *result)
    {
        assert(numPeaks == peaks->size());
        // retransforming is probably best if the range of x values is as small as necessary
        for (size_t i = 0; i < numPeaks; i++)
        {
            const RegressionGauss *regression = peaks->data() + i;
            assert(regression->isValid);
            const RegCoeffs coeff = regression->coeffs;
            double b23 = coeff.b1 < 0 ? coeff.b2 : coeff.b3;

            double apex_raw = -coeff.b1 / (b23 * 2);
            double apex = apex_raw + double(coeff.x0);
            assert(abs(apex - regression->apex_position) < 10e-6); // this can differ up to 5 * 10e-7 from floating point error

            PeakFit peak;

            // @todo ensure that this is a good way to estimate delta_x for real data
            // we could also use a strategy such as taking the distance closest to the apex.
            // Also check if the chosen delta_x is acceptable.
            size_t leftOfApex = size_t(apex);
            float delta_x = x_values[leftOfApex + 1] - x_values[leftOfApex];

            // control against all distances in the peak, test if they are equidistant enough
            // the test below shows that delta x must be considered on a per-peak basis

#if 0
            double delta_min = delta_x;
            double delta_max = delta_x;
            double delta_mean = 0;
            for (size_t pos = regression->startIdx; pos < regression->regSpan.endIdx; pos++)
            {
                double d = x_values[pos + 1] - x_values[pos];
                delta_min = min(d, delta_min);
                delta_max = max(d, delta_max);
                delta_mean += d;
                printf("%f, ", d);
            }
            delta_mean /= regression->regSpan.length - 1;
            printf(" | mean: %f\n", delta_mean);
#endif

            // position is determined relative to the point left of the apex
            float apexFraction = delta_x * float(apex - trunc(apex));
            peak.position = x_values[leftOfApex] + apexFraction;
            peak.uncert_position = regression->uncert_position * delta_x;

            // left and right limits
            peak.limit_L = x_values[regression->startIdx];
            peak.limit_R = x_values[regression->regSpan.endIdx];

            peak.startIdx = regression->startIdx;
            peak.length = regression->length;

            // peak height = regression at apex position before transformation
            peak.height = (float)exp(regAt(&coeff, apex_raw));
            peak.uncert_height = regression->uncert_height * peak.height;

            peak.area = regression->area;
            peak.uncert_area = regression->uncert_area * (float)exp(coeff.b0); // @todo this is incorrect, since we have two uncertainty bounds
            assert(peak.area > 1);

            // the empirical peak width is generally estimated at half maximum. Our peak
            // model only has a standard deviation for the apex peak
            peak.fwhm = (float)fullWidthHalfMax(&coeff, peak.height, delta_x);

            peak.dqs = erfc(regression->uncert_area / regression->area);
            peak.jaccard = regression->jaccard;

            peak.coeffs = regression->coeffs;

            result->push_back(peak);
        }
    }

    static void regression_on_continuum(
        const float *intensities,
        const float *x_axis,
        const float *intensities_log,
        const uint16_t *const df,
        const size_t length,
        const size_t maxscale,
        std::vector<PeakFit> *result);

    const double minIntensity_global = exp(1);

    int qpeaks_find(
        // SOME_IMPLEMENTATION_OF_LINEAR_ALLOCATOR_HERE
        const float *intensity_base,
        const float *x_values,
        const uint16_t *DF_cum,
        const size_t length,
        size_t maxscale,
        std::vector<PeakFit> *result)
    {
        // control input for nullpointers, mismatching x and y, and fitting maxscale
        if (intensity_base == nullptr || x_values == nullptr || result == nullptr)
        {
            return -1;
        }
        if (length < MINLENGTH)
        {
            return -2;
        }
        if (maxscale < GLOBAL_MINSCALE)
        {
            return -3;
        }
        if (maxscale > QALGORITHMS_MAXSCALE_PRECOMPILED)
        {
            return -4;
        }

        // @todo the assumtion that all values of x are equidistant is conrolled. If gaps are
        // found, the missing values are interpolated assuming an exponential rate of change.
        // this should happen before calling this function (?)

        // core operation: identify best-fit regressions for the input data

        /*
        The fitting routine assumes that all present peaks have a modified gaussian base function.
        This means that no baseline exists. Baseline substraction, if appropriate, has to be performed
        before calling the qpeaks_find.
        */
        std::vector<float> intensity_base_log(length + 1);

        for (size_t i = 0; i < length; i++)
        {
            intensity_base_log[i] = log(intensity_base[i]);
        }

        // The minimum intensity is "> e ", because this way we know that all log intensities
        // treated in the program will be > 1. This reduces the risk of high floating point
        // errors when multiplying a number << 1 with one >> 1.

        size_t rangeStart = 0;
        for (size_t pointIdx = 0; pointIdx < length; pointIdx++)
        {
            // expand the region while y values are large enough
            if ((intensity_base[pointIdx] > minIntensity_global) && (pointIdx != length - 1))
                continue;

            // since the current point is out-of-range at this point, there is no +1 for the range
            const size_t newLen = pointIdx - rangeStart;
            if (newLen >= MINLENGTH)
            {
                const size_t newMaxscale = min(maxscale, (newLen - 1) / 2);
                const float *intensities = intensity_base + rangeStart;
                const float *intensities_log = intensity_base_log.data() + rangeStart;
                const float *x_axis = x_values + rangeStart;
                const uint16_t *df = DF_cum == nullptr ? nullptr : DF_cum + rangeStart;

                assert(intensities_log[newLen - 1] > 1);

                regression_on_continuum(intensities,
                                        x_axis,
                                        intensities_log,
                                        df,
                                        newLen,
                                        newMaxscale,
                                        result);
            }
            rangeStart = pointIdx + 1;
        }

        return (int)result->size();
    }

#pragma region "running regression"

    static invalid validRegWidth(const RegCoeffs *coeffs, Range_i *range);

    static size_t validateRegressions( // @todo should centroids and features have to adhere to the same quality standards?
        const float *intensities,
        const float *intensities_log,
        const float *x_axis,
        const uint16_t *const degreesOfFreedom_cum,
        const std::vector<RegCoeffs> *coefficients,
        const size_t length,
        std::vector<RegressionGauss> *validRegressions)
    {
        size_t validCount = 0;
        std::vector<float> predict(length, 0);
        for (size_t i = 0; i < coefficients->size(); i++)
        {
            Range_i range;
            // sets the range and checks for validity @todo not applicable for dual peak systems
            invalid failpoint = validRegWidth(&(coefficients->at(i)), &range);
            if (failpoint != ok)
                continue;

            size_t df_sum = sumOfCumulative(degreesOfFreedom_cum, range.startIdx, range.length);
            if (df_sum < MINLENGTH)
                continue;

            df_sum -= 4; // four coefficients, adjust for components

            RegressionGauss reg;
            reg.coeffs = coefficients->at(i);
            reg.regSpan = range;
            reg.startIdx = range.startIdx;
            reg.length = range.length;

            /*
                Adjustment of b0 coefficient:
                When working with log-transformed data, the coefficients are suboptimal for the exponential case.
                Since we must work with a log system to perform a linear regression, there is a bias in the
                results which is somewhat corrected here. While correction could occur before validation, the
                initial tests filter out a lot of bad regressions which reduces processing time. The tests are
                presumed to be better when using the transformed coefficients in terms of applicability of the results.
                This function also modifies the "predict" vector supplied as its argument!
            */
            correctB0(intensities, &range, predict.data(), &reg.coeffs);

            failpoint = calcRegressionProperties(
                intensities,
                intensities_log,
                x_axis,
                predict.data(),
                df_sum,
                length,
                &reg);

            validCount += failpoint == ok ? 1 : 0;
            if (failpoint == ok)
            {
                validRegressions->push_back(reg);
            }
        }
        assert(validCount == validRegressions->size());

        return validCount;
    }

#pragma endregion "running regression"

#pragma region "Conflict Elimination"

    static double calcMSE_exp(const RegCoeffs *coeff,
                              const float *observed,
                              const Range_i *regSpan,
                              const double df);

    static size_t groupRegsByApex(const std::vector<RegressionGauss> *validRegressions, int16_t apexGroups[]);

    static bool groupApexIsStable(
        const std::vector<RegressionGauss> *validRegressions,
        const int16_t apexGroups[],
        const int16_t groupNum);

    static size_t selectFromGroup(
        const std::vector<RegressionGauss> *validRegressions,
        const float *intensities,
        const size_t length,
        const int16_t apexGroups[],
        const int16_t groupNum);

    // ----------- old functions -------------//

    static void mergeRegsInScale(
        const float *intensities,
        const uint16_t *const df_cum,
        std::vector<RegressionGauss> *validRegsTmp,
        std::vector<RegressionGauss> *validRegressions);

    static void mergeRegressionsOverScales(
        std::vector<RegressionGauss> *validRegressions,
        const float *intensities);

    // -------------------------------------- //

    static void regression_on_continuum(
        const float *intensities,
        const float *x_axis,
        const float *intensities_log,
        const uint16_t *const df,
        const size_t length,
        const size_t maxscale,
        std::vector<PeakFit> *result)
    {
        // coefficients for single-b0 peaks, spans all regressions over a peak window
        // all entries in coeff are sorted by scale and position in ascending order - this is not checked!
        std::vector<RegCoeffs> coefficients;
        findCoefficients(intensities_log, length, maxscale, &coefficients);

        std::vector<RegressionGauss> validRegsTmp; // all independently valid regressions regressions
        validRegsTmp.reserve(coefficients.size() / 2);
        size_t validCount = validateRegressions(intensities,
                                                intensities_log,
                                                x_axis,
                                                df,
                                                &coefficients,
                                                length,
                                                &validRegsTmp);
        if (validCount == 0)
            return;

        int16_t apexGroups[max_apex_per_group];
        size_t apexcount = groupRegsByApex(&validRegsTmp, apexGroups);

        size_t chosenOne = 0;
        if (validCount > apexcount)
        {
            chosenOne = selectFromGroup(&validRegsTmp, intensities, length, apexGroups, 0);
            bool stableApex = groupApexIsStable(&validRegsTmp, apexGroups, 0);
            assert(stableApex);
        }
        double chosenB1 = validRegsTmp.at(chosenOne).coeffs.b1;

        std::vector<RegressionGauss> validRegressions;

        mergeRegsInScale(
            intensities,
            df,
            &validRegsTmp,
            &validRegressions);

        // there can be 0, 1 or more than one regressions in validRegressions
        mergeRegressionsOverScales(&validRegressions, intensities);

        assert(validRegressions.size() == apexcount);
        bool b1_match = false;
        for (size_t reg = 0; reg < validRegressions.size(); reg++)
        {
            if (validRegressions.at(reg).coeffs.b1 == chosenB1)
                b1_match = true;
        }
        if (!b1_match && validRegressions.size() == 1)
        {
            printf("observed | old prediction | new prediction\n");
            // assert(validRegressions.size() == 1);
            RegressionGauss *reg_old = validRegressions.data();
            RegressionGauss *reg_new = validRegsTmp.data() + chosenOne;
            for (size_t i = 0; i < length; i++)
            {
                double x_old = (double)i - (double)reg_old->coeffs.x0;
                double x_new = (double)i - (double)reg_new->coeffs.x0;
                printf("%f, %f, %f\n", intensities[i],
                       exp(regAt(&reg_old->coeffs, x_old)),
                       exp(regAt(&reg_new->coeffs, x_new)));
            }
            printf("Old range: %u to %u\n", reg_old->startIdx, reg_old->startIdx + reg_old->length - 1);
            printf("New range: %u to %u\n\n", reg_new->startIdx, reg_new->startIdx + reg_new->length - 1);
            // exit(1);
        }

        retransformPeaks(&validRegressions,
                         x_axis,
                         validRegressions.size(),
                         result);
    }

    static size_t groupRegsByApex(const std::vector<RegressionGauss> *validRegressions, int16_t apexGroups[])
    {
        const size_t length = validRegressions->size();

        assert(length < max_apex_per_group);

        if (length < 2)
            return length;

        // there must be a distance of at least this many points between two apexes
        // for them to be considered distinct. We use int here so that an apex at
        // x < min_apex_dist will not be a problem later
        const double min_apex_dist_d = 2 * GLOBAL_MINSCALE;

        // keep a record of which apex group a given peak belongs to. We assume that
        // 256 far exceeds the number of possible groups even for large inputs
        for (size_t i = 0; i < max_apex_per_group; i++)
            apexGroups[i] = -1;

        // assign apex groups
        size_t assignments = 0;
        uint16_t currentGroup = 0;

        size_t next_unassigned = 0;
#define reg validRegressions->at(next_unassigned)

        double currentApex = reg.apex_position;
        double apexLeftLim = currentApex - min_apex_dist_d;
        double apexRightLim = currentApex + min_apex_dist_d;
        size_t outerStart = reg.startIdx; // @todo check if signed type is necessary, this should always be positive
        size_t outerLength = reg.length;
        apexGroups[next_unassigned] = currentGroup;

        // the assignments counter is incremented for every point that was assigned an apex group
        while (assignments < length)
        {
            // the loop must iterate until we are certain that no further change will occur
            bool hasChanged = true;
            while (hasChanged)
            {
                hasChanged = false;
                // first pass: Assign regressions and update limits accordingly
                for (size_t p = next_unassigned + 1; p < length; p++)
                {
                    if (apexGroups[p] != -1)
                        continue;

                    double secondApex = validRegressions->at(p).apex_position;
                    size_t innerStart = validRegressions->at(p).startIdx;
                    size_t innerLength = validRegressions->at(p).length;
                    // reasoning: while a distance of four points is the logically mandated distance, this is
                    // preconditioned on both regions having any overlap. Here, one point matching in fit
                    // region is not considered an overlap. To this end, we check that the defined region
                    // for the tested regression overlaps to either side with the covered region so far.
                    // By testing for the inner being enclosed or the outer being enclosed, we also test
                    // for either region being fully enclosed in the other
                    bool addToGroup = ((secondApex > apexLeftLim) && (secondApex < apexRightLim)) &&
                                      (((innerStart <= outerStart) && (outerStart < innerStart + innerLength)) || // L_i ... L_o ... R_i ... R_o
                                       ((outerStart <= innerStart) && (innerStart < outerStart + outerLength)));  // L_o ... L_i ... R_o ... R_i
                    if (addToGroup)
                    {
                        apexGroups[p] = currentGroup;
                        apexLeftLim = min(apexLeftLim, secondApex - min_apex_dist_d);
                        apexRightLim = max(apexRightLim, secondApex + min_apex_dist_d);
                        hasChanged = true;
                        assignments += 1;
                    }
                }
            }

            // iterate through the data until the next unassigned value is found
            for (; next_unassigned < length; next_unassigned++)
            {
                if (apexGroups[next_unassigned] == -1)
                    break;
            }
            // break must be placed here to avoid bad array access
            if (next_unassigned == length)
                break;

            assert(currentGroup < length);
            currentGroup += 1;
            // @todo this is duplicated from function initialisation, macro?
            currentApex = reg.apex_position;
            apexLeftLim = currentApex - min_apex_dist_d;
            apexRightLim = currentApex + min_apex_dist_d;
            outerStart = reg.startIdx;
            outerLength = reg.length;
            apexGroups[next_unassigned] = currentGroup;
        }
#undef reg
        return currentGroup + 1;
    }

    // function that asserts that all regressions within one grouping describe only one apex
    static bool groupApexIsStable(
        const std::vector<RegressionGauss> *validRegressions,
        const int16_t apexGroups[],
        const int16_t groupNum)
    {
        const size_t regCount = validRegressions->size();
        assert(regCount > 1);

        double apex_lim_L = INFINITY;
        double apex_lim_R = 0;

        for (size_t i = 0; i < regCount; i++)
        {
            if (apexGroups[i] != groupNum)
                continue;
            const RegressionGauss *reg = validRegressions->data() + i;
            apex_lim_L = min(apex_lim_L, reg->apex_position);
            apex_lim_R = max(apex_lim_R, reg->apex_position);
        }
        assert(apex_lim_L <= apex_lim_R);

        // if there is a gap >1, it is unlikely that the group contains only
        // well-formed regressions which describe only one peak
        return apex_lim_R - apex_lim_L < 1;
    }

    // return the index of the regression that was found to be the best group representative
    static size_t selectFromGroup(
        const std::vector<RegressionGauss> *validRegressions,
        const float *intensities,
        const size_t length,
        const int16_t apexGroups[],
        const int16_t groupNum)
    {
        assert(groupNum > -1);
        size_t regCount = validRegressions->size();
        assert(regCount > 1);

        // 1) iterate over the groups to find the relevant region over which to compare
        // the regressions
        uint16_t lim_L = length;
        uint16_t lim_R = 0;
        for (size_t i = 0; i < regCount; i++)
        {
            if (apexGroups[i] == groupNum)
            {
                const RegressionGauss *reg = validRegressions->data() + i;
                lim_L = min(reg->startIdx, lim_L);
                uint16_t lim_R_reg = reg->length + reg->startIdx - 1;
                lim_R = max(lim_R, lim_R_reg);
            }
        }
        assert(lim_R > 0);

        // calculate SSE in order
        // We use the SSE instead of the MSE since n is identical for this
        // comparison
        double bestSSE = INFINITY;
        size_t bestRegIdx = regCount;

        for (size_t i = 0; i < regCount; i++)
        {
            if (apexGroups[i] != groupNum)
                continue;

            const RegressionGauss *reg = validRegressions->data() + i;

            double currentSSE = 0;
            for (size_t xval = lim_L; xval < (size_t)lim_R + 1; xval++)
            {
                double observed = intensities[xval];
                double x = double(xval) - (double)reg->coeffs.x0;
                double predict = exp(regAt(&reg->coeffs, x));
                double diff = observed - predict;
                currentSSE += diff * diff;

                // early termination of mse calculation should save some time on average, not measured!
                if (currentSSE > bestSSE)
                    break;
            }
            if (currentSSE < bestSSE)
            {
                bestSSE = currentSSE;
                bestRegIdx = i;
            }
        }
        assert(bestRegIdx < regCount);
        return bestRegIdx;
    }

    // --------------- old functions ---------------- //
    void mergeRegsInScale(
        const float *intensities,
        const uint16_t *const df_cum,
        std::vector<RegressionGauss> *validRegsTmp,
        std::vector<RegressionGauss> *validRegressions)
    {
        std::vector<RegressionGauss> validRegsAtScale;
        size_t currentScale = GLOBAL_MINSCALE;
        RegressionGauss tmp;
        tmp.coeffs.scale = 0;
        validRegsTmp->push_back(tmp); // doing this avoids a second check for the last scale group @todo still very hacky
        RegressionGauss *currentReg = validRegsTmp->data();

        while (currentReg->coeffs.scale != 0)
        {
        repeat:
            if (currentReg->coeffs.scale == currentScale)
            {
                validRegsAtScale.push_back(*currentReg);
                currentReg += 1;
                goto repeat;
            }

            // nothing happens if the per-scale vector is empty
            if (validRegsAtScale.size() == 1)
            {
                // only one valid regression at scale
                validRegressions->push_back(validRegsAtScale.front());
            }
            else if (validRegsAtScale.size() > 1)
            {
                // resolve conflicting regressions
                findBestScales(validRegressions, &validRegsAtScale, intensities, df_cum);
            }
            // regression is not incremented because a toggle was triggered
            currentScale += 1;
            validRegsAtScale.clear();
        }
        validRegsTmp->pop_back();
    }

    static uint32_t findBestRegression(
        const float *intensities,
        const std::vector<RegressionGauss> *regressions,
        const uint16_t *const degreesOfFreedom_cum,
        const Range_i *regSpan);

    void findBestScales(std::vector<RegressionGauss> *validRegressions,
                        std::vector<RegressionGauss> *validRegsTmp,
                        const float *intensities,
                        const uint16_t *const degreesOfFreedom_cum)
    {
        /*
            Grouping:
            This block of code implements the grouping. It groups the valid peaks based
            on the apex positions. Peaks are defined as similar, i.e., members of the
            same group, if they fullfill at least one of the following conditions:
            - The difference between two peak apexes is less than 4. (Nyquist Shannon
            Sampling Theorem, separation of two maxima)
            - At least one apex of a pair of peaks is within the window of the other peak.
            (Overlap of two maxima)
        */
        std::vector<Range_i> groups;
        groups.reserve(validRegsTmp->size());

        size_t prev_i = 0;

        for (size_t i = 0; i < validRegsTmp->size() - 1; i++)
        {
            // check if the difference between two peak apexes is less than 4 (Nyquist Shannon
            // Sampling Theorem, separation of two maxima), or if the apex of a peak is within
            // the window of the other peak (Overlap of two maxima)
            RegressionGauss *reg1 = &(validRegsTmp->at(i));
            RegressionGauss *reg2 = &(validRegsTmp->at(i + 1));

            if (std::abs(reg1->apex_position - reg2->apex_position) < 4)
                continue;

            if (reg1->apex_position > reg2->startIdx)
                continue;

            if (reg1->regSpan.endIdx > reg2->apex_position)
                continue;

            // the two regressions differ, i.e. create a new group
            groups.push_back({prev_i, i, i - prev_i + 1});
            prev_i = i + 1;
        }
        groups.push_back({prev_i,
                          validRegsTmp->size() - 1,
                          validRegsTmp->size() - prev_i}); // last group ends with index of the last element

        /*
          Survival of the Fittest Filter:
          This block of code implements the survival of the fittest filter. It selects the peak with
          the lowest mean squared error (MSE) as the representative of the group. If the group contains
          only one peak, the peak is directly pushed to the valid regressions. If the group contains
          multiple peaks, the peak with the lowest MSE is selected as the representative of the group
          and pushed to the valid regressions.
        */
        for (size_t groupIdx = 0; groupIdx < groups.size(); groupIdx++)
        {
            if (groups[groupIdx].startIdx == groups[groupIdx].endIdx)
            { // already isolated peak => push to valid regressions
                size_t regIdx = groups[groupIdx].startIdx;
                auto onlyReg = validRegsTmp->at(regIdx);
                assert(onlyReg.isValid);
                validRegressions->push_back(onlyReg);
            }
            else
            { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
                uint32_t bestRegIdx = findBestRegression(intensities, validRegsTmp, degreesOfFreedom_cum,
                                                         groups.data() + groupIdx);

                RegressionGauss bestReg = validRegsTmp->at(bestRegIdx);
                // bestReg.mse = bestRegIdx.mse;
                assert(bestReg.isValid);
                validRegressions->push_back(bestReg);
            }
        }
    }

    uint32_t findBestRegression(
        const float *intensities,
        const std::vector<RegressionGauss> *regressions,
        const uint16_t *const degreesOfFreedom_cum,
        const Range_i *regSpan)
    {
        double best_mse = INFINITY;
        unsigned int bestRegIdx = 0;

        // identify left (smallest) and right (largest) limit of the grouped regression windows
        uint16_t left_limit = UINT16_MAX;
        size_t right_limit = 0;
        for (size_t i = regSpan->startIdx; i < regSpan->endIdx + 1; i++)
        {
            const RegressionGauss *reg = regressions->data() + i;
            left_limit = min(left_limit, reg->startIdx);
            right_limit = max(right_limit, reg->regSpan.endIdx);
        }

        Range_i newRange = {left_limit, right_limit, right_limit - left_limit + 1};
        double df_sum = sumOfCumulative(degreesOfFreedom_cum, left_limit, newRange.length);
        df_sum -= 4; // four coefficients
        assert(df_sum > 0);

        for (size_t i = regSpan->startIdx; i < regSpan->endIdx + 1; i++)
        {
            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            const RegressionGauss *reg = regressions->data() + i;
            Range_i range = {left_limit, right_limit, right_limit - left_limit + 1};
            double mse = calcMSE_exp(&reg->coeffs,
                                     intensities,
                                     &range,
                                     df_sum);
            if (mse < best_mse)
            {
                best_mse = mse;
                bestRegIdx = i;
            }
        }
        assert(regressions->at(bestRegIdx).isValid);
        return {bestRegIdx};
    }

    void mergeRegressionsOverScales(
        std::vector<RegressionGauss> *validRegressions,
        const float *intensities)
    {
        if (validRegressions->size() < 2)
        {
            return;
        }

        /*
          Grouping Over Scales:
          This block of code implements the grouping over scales. It groups the valid
          peaks based on the apex positions. Peaks are defined as similar, i.e.,
          members of the same group, if they fullfill at least one of the following conditions:
          - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
          - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
        */

        // iterate over the validRegressions vector
        RegressionGauss *firstReg = validRegressions->data();
        for (size_t i = 0; i < validRegressions->size(); i++)
        {
            RegressionGauss *activeReg = firstReg + i;
            assert(activeReg->isValid);
            double MSE_group = 0;
            int DF_group = 0;
            // only calculate required MSEs since this is one of the performance-critical steps
            std::vector<double> exponentialMSE(validRegressions->size(), 0);
            std::vector<unsigned int> validRegressionsInGroup; // vector of indices to validRegressions
            validRegressionsInGroup.reserve(64);
            size_t competitors = 0; // a competitor is a mutually exclusive alternative regression

            Range_i sharedRegion = activeReg->regSpan;

            for (size_t j = 0; j < i; j++)
            {

                RegressionGauss *secondReg = firstReg + j;
                if (secondReg == activeReg)
                {
                    continue; // this should not have been permitted in the first place
                }
                // there was a subtle bug here where the MSE was not calculated for the shared region,
                // resulting in a selection bias for narrow regression windows
                sharedRegion.startIdx = min(activeReg->startIdx, secondReg->startIdx);
                sharedRegion.endIdx = max(activeReg->regSpan.endIdx, secondReg->regSpan.endIdx);
                sharedRegion.length = sharedRegion.endIdx - sharedRegion.startIdx + 1;

                if (!secondReg->isValid) // check is needed because regressions are set to invalid in the outer loop
                    continue;

                if (activeReg->apex_position < secondReg->startIdx)
                    continue;

                if (activeReg->apex_position > secondReg->regSpan.endIdx)
                    continue;

                if (secondReg->apex_position < activeReg->startIdx)
                    continue;

                if (secondReg->apex_position > activeReg->regSpan.endIdx)
                    continue;

                if (exponentialMSE[j] == 0.0)
                {
                    exponentialMSE[j] = calcMSE_exp(
                        &secondReg->coeffs,
                        intensities,
                        &sharedRegion,
                        secondReg->df); // @todo this is wrong!
                }
                DF_group += secondReg->df;                      // add the degree of freedom
                MSE_group += exponentialMSE[j] * secondReg->df; // add the sum of squared errors
                // add the iterator of the ref peak to a vector of iterators
                validRegressionsInGroup.push_back(j);
                competitors += secondReg->numCompetitors + 1; // a regression can have beaten a previous one

            } // after this loop, validRegressionsInGroup contains all regressions that are still valid and contend with the regression at position i

            if (validRegressionsInGroup.empty()) // no competing regressions exist
            {
                assert(DF_group < 1);
                continue;
            }

            MSE_group /= DF_group;

            if (exponentialMSE[i] == 0.0)
            { // calculate the mse of the current peak
                exponentialMSE[i] = calcMSE_exp(
                    &activeReg->coeffs,
                    intensities,
                    &activeReg->regSpan,
                    activeReg->df);
            }
            if (exponentialMSE[i] <= MSE_group)
            {
                // Set isValid to false for the candidates from the group
                for (size_t it_ref_peak : validRegressionsInGroup)
                {
                    firstReg[it_ref_peak].isValid = false;
                }
                // only advance competitor count if regression is actually better
                activeReg->numCompetitors = competitors;
            }
            else
            { // Set isValid to false for the current peak
                activeReg->isValid = false;
            }
        }

        // remove invalid regressions
        size_t accessID = 0;
        for (size_t i = 0; i < validRegressions->size(); i++)
        {
            if (validRegressions->at(i).isValid)
            {
                validRegressions->at(accessID) = validRegressions->at(i);
                accessID += 1;
            }
        }
        validRegressions->resize(accessID);
    }

#pragma endregion "eliminate conflicting regs"

    void findCoefficients(
        const float *intensity_log,
        const size_t length,
        size_t maxscale,
        std::vector<RegCoeffs> *coeffs)
    {
        assert(maxscale >= GLOBAL_MINSCALE);
        assert(maxscale <= QALGORITHMS_MAXSCALE_PRECOMPILED);

        assert(length > 4);
        maxscale = min(maxscale, (length - 1) / 2);

        // for every checked scale, there are length - 2 scale regressions performed
        // totalRegs = sum_i from minscale to maxscale (length - 2 * scale_i)
        // scalediff = maxscale - minscale + 1
        // totalRegs = scalediff * length - 2 * (2 + 3 + 4 + ... + maxscale)
        // the last bit is the (triangle number of maxscale) - (miscale - 1)
        // totalRegs = scalediff * length - 2 * (maxscale * (maxscale + 1) / 2 - minscale + 1)
        // totalRegs = scalediff * length - maxscale * (maxscale + 1) + 2 * minscale - 2)
        const size_t totalRegs = (maxscale - GLOBAL_MINSCALE + 1) * length -
                                 maxscale * (maxscale + 1) +
                                 2 * GLOBAL_MINSCALE - 2;
        coeffs->resize(totalRegs);

        // the first and last MINSCALE elements of the data do not need to be checked for x0, since they are invalid by definition
        const size_t limit = length - GLOBAL_MINSCALE;
        for (size_t x0 = GLOBAL_MINSCALE; x0 < limit; x0++)
        {
            const float *cen = intensity_log + x0; // this is initially the third real point

            // calculate the convolution with the kernel of the lowest scale - 1 (= 1), i.e. xT * intensity_log[cen - 1 : cen + 1]
            // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
            // they are set to scale = 1 so the first value written is at scale = 2
            // b0 is 1, 1, 1,
            double product_sum_b0 = cen[-1] + cen[0] + cen[1];
            // b1 is -1, 0, 1
            double product_sum_b1 = -cen[-1] + cen[1];
            // b2 is 1, 0, 0
            double product_sum_b2 = cen[-1];
            // b3 is 0, 0, 1
            double product_sum_b3 = cen[1];

            size_t maxscale_absolute = 0;
            { // the largest valid scale depends on x0
                size_t maxscale_left = x0;
                size_t maxscale_right = length - x0 - 1;
                size_t maxscale_limits = min(maxscale_left, maxscale_right);
                maxscale_absolute = min(maxscale_limits, maxscale);
            }

            // var for access in inner loop
            size_t offset_prev = 0;
            for (size_t scale = GLOBAL_MINSCALE; scale <= maxscale_absolute; scale++)
            {
                // random access is difficult to vectorise
                double leftVal = cen[-scale];
                double rightVal = cen[scale];

                // expand the kernel to the left and right of the intensity_log.
                // b0 is expanded by the two outer points * 1
                product_sum_b0 += leftVal + rightVal;
                // b1 is expanded by the points * scale, negative to the left
                // product_sum_b1 += -double(scale) * leftVal + double(scale) * rightVal;
                product_sum_b1 += double(scale) * (rightVal - leftVal);
                // b2 and b3 are expanded by scale^2 the outermost point to the left or right
                double scale_sqr = double(scale * scale);
                product_sum_b2 += scale_sqr * leftVal;
                product_sum_b3 += scale_sqr * rightVal;

                const MatInverse inv = qalgo_matInverse[scale];

                const double inv_B_b0 = inv.B * product_sum_b0;
                const double inv_D_b1 = inv.D * product_sum_b1;

                // access is determined by scale and x0.
                // scale 2: idx is x0 - scale
                // scale 3: length - ((scale - 1) * 2) + x0 - scale
                // scale 4: 2 * length - (scale - 1) * 2 - (scale - 2) * 2 + x0 - scale
                //          2 * length - scale * 4 + 6 + x0 - scale
                const size_t offset_front = x0 - scale;
                const size_t offset = offset_front + offset_prev;
                assert(offset < totalRegs);
                RegCoeffs *current = coeffs->data() + offset;
                current->b0 = inv.A * product_sum_b0 + inv.B * (product_sum_b2 + product_sum_b3);
                current->b1 = inv.C * product_sum_b1 + inv.D * (product_sum_b2 - product_sum_b3);
                current->b2 = inv_B_b0 + inv_D_b1 + inv.E * product_sum_b2 + inv.F * product_sum_b3;
                current->b3 = inv_B_b0 - inv_D_b1 + inv.F * product_sum_b2 + inv.E * product_sum_b3;
                current->scale = scale;
                current->x0 = x0;
                // next scale would access front of vector
                offset_prev += length - 2 * scale;
            }
        }
    }

#pragma endregion "running regression"

#pragma region "validate Regression"

    double correctB0(const float *const intensities,
                     const Range_i *range,
                     float *predicted,
                     RegCoeffs *coeff) // @todo also correct the error matrix, assess degree of correction applied
    {
        // problem: after the log transform, regression residuals are not directly transferable to
        // the retransformed model. This is corrected by adjusting b0 so that the MSE in the
        // exponential case is minimal. We perform another regression taking e^(x b1 + x^2 b23) as a constant
        // and adjusting e^b0 so that (intensities - e^b0 * constant)^2 is minimal.
        // This is the same as scaling the regression by a constant c.

        // The (XtX)^-1 term collapses into a scalar, 1 / sum(predict^2). Xt * y is also scalar, sum(predict * intensities).
        // The corrective factor is then sum(predict * intensities) / sum(predict^2).

        // predict intensity only within range to prevent unnecessary exp operations.
        // prediction is incomplete, so it has to be multiplied with exp(b0)

        // uncertainties of this prediction are derived from the general equation for every coefficient.
        // the derivative is the following general form where bx' depends on the derived coefficient (1 for b0, x for b1, x^2 for b23)
        // b0' + ln(sum(exp(b0 + b1 x + b23 x^2) * y_i)) * sum(exp(b0 + b1 x + b23 x^2) * y_i * bx')
        //     - ln(sum(exp(b0 + b1 x + b23 x^2)^2)) * sum(exp(b0 + b1 x + b23 x^2)^2 * bx'^2)
        // note that the value of bx' depends on x and cannot be removed from the sum. Precalculation is also not
        // possible due to the value of y being required. This is not a large performance concern since we can combine it
        // with the calculations that are already necessary for obtaining the modified b0.

        // double b0_old = coeff->b0;

        const size_t start = range->startIdx;
        const size_t end = range->endIdx + 1;
        assert(start < end);
        double x0 = double(coeff->x0);
        for (size_t i = start; i < end; i++)
        {
            double x = double(i) - x0;
            predicted[i] = (float)regExp_fac(coeff, x);
        }

        double sum_predictSq = 0;
        double sum_predictReal = 0;
        double b0_exp = exp(coeff->b0);

        // Regression correction is only calculated from the range in which the regression is relevant initially.
        for (size_t i = start; i < end; i++)
        {
            double pred = predicted[i] * b0_exp;
            sum_predictSq += pred * pred;
            sum_predictReal += pred * intensities[i];
        }
        double correction = sum_predictReal / sum_predictSq;

        // exp(a) * exp(b) == exp(a + b), so b0 + log(correction) is the same as predict * correction
        coeff->b0 += log(correction);
        // assert(abs(coeff->b0 - b0_old) < 0.001);

        // adjust the now incorrect values for predict. Remember that the previous prediciton was incomplete!
        float factor = (float)exp(coeff->b0);
        for (size_t i = start; i < end; i++)
        {
            predicted[i] *= factor;
        }

        return factor;
    }

    invalid validRegWidth(const RegCoeffs *coeffs, Range_i *range)
    {
        // test regression validity without depending on b0 or the degrees of freedom
        const bool valley_left = coeffs->b2 >= 0;
        const bool valley_right = coeffs->b3 >= 0;
        const bool apexLeft = coeffs->b1 < 0;

        // the peak must have a maximum
        bool noApex = (valley_left && apexLeft) ||
                      (valley_right && (!apexLeft));
        if (noApex)
            return no_apex;

        // position maximum / minimum of b2 or b3. This is just the frst derivative of the peak half equation
        // (d of y = b0 + b1 x + b23 x^2 => y b1 + 2 * b23 x) solved for y = 0
        double position_b2 = -coeffs->b1 / (2 * coeffs->b2);
        double position_b3 = -coeffs->b1 / (2 * coeffs->b3);
        double x0d = double(coeffs->x0);

        double apexd = apexLeft ? position_b2 : position_b3;

        // there must be at least two full points to either side of the apex. The truncated apex is
        // the number of points next to it towards the center
        bool edge_too_far = 2 + (uint16_t)abs(apexd) > coeffs->scale;
        if (edge_too_far)
            return invalid_apex; // apex outside of regression window

        size_t apex = size_t(x0d + apexd);
        size_t lim_l = coeffs->x0 - coeffs->scale;
        size_t lim_r = coeffs->x0 + coeffs->scale;
        *range = {lim_l, lim_r, lim_r - lim_l + 1};

        if (!(valley_left || valley_right))
            return ok; // all ok by definition

        // it is already established that the apex side of the regression is acceptable.
        // now, we only need to check that the valley is at least two points away from the apex
        // since only the difference is relevant, this is independent of the side on which the apex is
        assert(position_b2 < position_b3);
        if (position_b3 - position_b2 <= 2)
            return invalid_apex;

        if (valley_left)
        {
            size_t spanLeft = size_t(-position_b2);
            size_t altLim = coeffs->x0 < spanLeft ? 0 : coeffs->x0 - spanLeft;
            lim_l = max(lim_l, altLim);
            if (coeffs->x0 - lim_l < 2)
                return invalid_apex;
        }
        else // if (valley_right)
        {
            lim_r = min(lim_r, coeffs->x0 + size_t(position_b3));
            if (lim_r - coeffs->x0 < 2)
                return invalid_apex;
        }
        assert(lim_l <= apex);
        assert(lim_r >= apex);
        assert(lim_l < coeffs->x0);
        assert(lim_r > coeffs->x0);

        *range = {lim_l, lim_r, lim_r - lim_l + 1};
        return ok;
    }

    /// @brief calculate the residual sum of squares for the log regression / data
    /// @param mutateReg relevant regression
    /// @param observed log data
    /// @return RSS value
    static double calcRSS_log(const RegressionGauss *mutateReg, const float *observed);

    /// @brief performs two F-tests against the log data. First H0 is the mean, second y = mx + b
    /// @param observed log data (or normal data, depends on the use case)
    /// @param RSS_reg previously calculated residual sum of squares of the complex model. Hard assumpion of four coefficients.
    /// @param range range of the regression.
    /// @return true: Regression is significant; false: Regression is not better than either alternative.
    static bool f_testRegression(const float *observed, double RSS_reg, const Range_i *range);

    static double calcSSE_chisqared(const Range_i *regSpan,
                                    const float *observed,
                                    const float *predict);

    // @todo rework: Currently, we are trying to determine the validity of a given regression
    // using multiple tests. This is wrong, since this way we get uneven p-values of the final
    // outcome (refer to bonferroni correction). Instead, a correct version of this function
    // should only perform one test (if any) and resolve regressions primarily via decision
    // rules founded in logical necessities for the peak model
    invalid calcRegressionProperties( // returns the number of the failed test
        const float *intensities,
        const float *intensities_log,
        const float *x_axis,
        const float *predict,
        const size_t df_sum,
        const size_t length,
        RegressionGauss *mutateReg)
    {
        assert(!mutateReg->isValid);
        const size_t scale = mutateReg->coeffs.scale;
        const RegCoeffs *coeffs = &mutateReg->coeffs;
        const Range_i regSpan = mutateReg->regSpan;

        assert(scale > 1);
        assert(coeffs->x0 + scale < length);

        int failstates = 0;

        // this is the error term for the corrected regression. Of the original 4 x 4 matrix,
        // only the first row is needed
        // double errorMat[4] = {0};

        // @todo new error correction here. Previous covariance matrix was mse (log) * (XtX)^-1,
        // multiply with matrix U, where first four terms are partial derivative of equation in
        // correctB0 by original coefficients

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates
          the ratio of the apex signal to the edge signal and ensures that the
          ratio is greater than 2. This is a pre-filter for later
          signal-to-noise ratio checkups.
          @todo this is not a relevant test
        */
        // double apexToEdge = apexToEdgeRatio(mutateReg, intensities);
        // if (apexToEdge < 2)
        // {
        //     failstates += 1;
        //     // return invalid_apexToEdge; // invalid apex to edge ratio // b0 independent
        // }

        // @todo differentiate between tests performed in log and exp domain better
        // everything involving the RSS is dependent on b0!
        double RSS_log = calcRSS_log(mutateReg, intensities_log);
        assert(RSS_log > 0);
        // double RSS_exp = calcRSS(predict->data(), intensities, &regSpan);
        // assert(RSS_exp > 0);
        // double mse_exp = RSS_exp / double(df_sum);
        double mse_log = RSS_log / double(df_sum);

        /*
        We use an F-test instead of a normal height threshold. If the peak shape is a good enough
        fit, even very low intensity signals should be accounted for. The test here has a hard-coded
        alpha of 0.05
        */
        bool f_ok = f_testRegression(intensities_log, RSS_log, &regSpan);
        if (!f_ok)
        {
            failstates += 2;
            // return f_test_fail; // H0 holds, the two distributions are not noticeably different
        }

        // uncertainty calculation and t-tests against peak properties

        double position = -coeffs->b1 / 2 / (coeffs->b1 < 0 ? coeffs->b2 : coeffs->b3);
        float uncert_position = (float)peakPositionUncert(coeffs, mse_log);
        mutateReg->uncert_position = uncert_position;
        float uncert_height = (float)peakHeightUncert(coeffs, mse_log);
        mutateReg->uncert_height = uncert_height;

        size_t leftOfApex = (size_t)position;
        float delta_x = x_axis[leftOfApex + 1] - x_axis[leftOfApex];
        double uncertainty = -1;
        mutateReg->area = (float)peakArea(coeffs, delta_x, mse_log, &uncertainty);
        mutateReg->uncert_area = (float)uncertainty;

        if (mutateReg->area <= minIntensity_global)
            return invalid_height;

        // this is also nonsensical since we care about the validity of the exponential
        // area only.
        // if (mutateReg->area / mutateReg->uncert_area <= T_VALUES[df_sum])
        // {
        //     failstates += 32;
        //     // return invalid_area; // statistical insignificance of the area
        // }

        /*
          Chi-Square Filter:
          This block of code implements the chi-square filter. It calculates the chi-square
          value based on the weighted chi squared sum of expected and measured y values in
          the exponential domain. If the chi-square value is less than the corresponding
          value in the CHI_SQUARES, the regression is invalid. @todo why?
        */
        assert(mutateReg->regSpan.length > 0);
        double chiSquare = calcSSE_chisqared(&mutateReg->regSpan, intensities, predict);
        if (chiSquare > CHI_SQUARES[df_sum])
        {
            failstates += 64;
            // return invalid_chisq; // statistical insignificance of the chi-square value
        }

        mutateReg->df = df_sum;
        mutateReg->apex_position = (float)coeffs->x0 + (float)peakPosition(coeffs);
        mutateReg->jaccard = (float)calcJaccardIdx(intensities, predict, length);

        if (failstates != 0)
            return invalid::invalid_apex;

        mutateReg->isValid = true;
        return ok;
    }

#pragma region calcSSE

    double calcRSS_log(const RegressionGauss *mutateReg, const float *observed)
    {
        double RSS = 0;
        const size_t start = mutateReg->startIdx;
        const float *obs = observed + start;
        double x = double(start) - double(mutateReg->coeffs.x0);
        const size_t len = mutateReg->regSpan.length;
        for (size_t i = 0; i < len; i++)
        {
            double pred = regAt(&mutateReg->coeffs, x);
            double difference = obs[i] - pred;
            x += 1.0;
            RSS += difference * difference;
        }
        assert(RSS > 0);
        return RSS;
    }

    double calcMSE_exp(const RegCoeffs *coeff,
                       const float *observed,
                       const Range_i *regSpan,
                       const double df)
    {
        double mse = 0;
        double x = double(regSpan->startIdx) - double(coeff->x0);
        const float *obs = observed + regSpan->startIdx;
        for (size_t i = 0; i < regSpan->length; i++)
        {
            double pred = exp(regAt(coeff, x));
            double diff = obs[i] - pred;
            mse += diff * diff;
            x += 1.0;
        }
        mse /= df;
        assert(mse > 0);
        return mse;
    }

    double calcSSE_chisqared(const Range_i *regSpan,
                             const float *observed,
                             const float *predict)
    {
        const float *obs = observed + regSpan->startIdx;
        const float *pred = predict + regSpan->startIdx;
        double result = 0.0;
        for (size_t i = 0; i < regSpan->length; i++)
        {
            double diff = obs[i] - pred[i];
            result += diff * diff / pred[i];
        }
        return result;
    }

    static double calcRSS_H0_cf1(const float *observed, const Range_i *range)
    {
        // this function calculates the RSS for H0: y = b0 (a constant value)
        double mean = 0;
        const float *obs = observed + range->startIdx;
        for (size_t i = 0; i < range->length; i++)
        {
            mean += obs[i];
        }
        mean /= range->length;

        double RSS = 0;
        for (size_t i = 0; i < range->length; i++)
        {
            double difference = obs[i] - mean;
            RSS += difference * difference;
        }
        assert(RSS > 0);
        return RSS;
    }

    static double calcRSS_H0_cf2(const float *observed, const Range_i *range)
    {
        // this function calculates the RSS for H0: y = b0 + x * b1 (no weights)

        double slope = NAN;
        double intercept = NAN;
        size_t length = range->length;
        assert(length > 0);
        const float *obs = observed + range->startIdx;
        linReg_intx(obs, length, &slope, &intercept);

        double RSS = 0;
        double x = 0;
        for (size_t i = 0; i < length; i++)
        {
            double difference = obs[i] - (intercept - slope * x);
            RSS += difference * difference;
            x += 1;
        }
        assert(RSS > 0);

        return RSS;
    }

    bool f_testRegression(const float *observed, double RSS_reg, const Range_i *range)
    {
        // during the tests, the RSS for the regression has already been calculated in calcRSS_log
        assert(RSS_reg > 0);
        const size_t length = range->length;
        double RSS_H0 = INFINITY;
        bool f_ok = false;

        RSS_H0 = calcRSS_H0_cf1(observed, range); // y = b
        f_ok = F_test_regs(RSS_reg, RSS_H0, 4, 1, length, 0.05);
        if (!f_ok)
            return false;

        RSS_H0 = calcRSS_H0_cf2(observed, range); // y = b0 + b1 * x
        f_ok = F_test_regs(RSS_reg, RSS_H0, 4, 1, length, 0.05);

        return f_ok;
    }

#pragma endregion calcSSE

    // double apexToEdgeRatio(const RegressionGauss *mutateReg, const float *intensities)
    // {
    //     // is the apex at least twice as large as the outermost point?
    //     // assumption: outermost point is already near base level
    //     const size_t idxApex = size_t(mutateReg->apex_position) + mutateReg->coeffs.x0;

    //     double left = intensities[mutateReg->startIdx];
    //     double right = intensities[mutateReg->regSpan.endIdx];
    //     double minIntensity = min(left, right);
    //     double apex = intensities[idxApex]; // @todo since this is not the actual apex height, it might be a bad idea to use it
    //     return apex / minIntensity;
    // }

    bool isValidQuadraticTerm(const RegCoeffs *coeffs, const double mse, const size_t df_sum)
    {
        // @todo explain
        // checks if both quadratic terms are significant - should we only check the apex coeff?
        assert(mse > 0);
        const double inv_E = qalgo_matInverse[coeffs->scale].E;
        double divisor = sqrt(inv_E * mse);
        double abs2 = abs(coeffs->b2);
        double abs3 = abs(coeffs->b3);
        double tValue = max(abs2, abs3);
        return tValue / divisor > T_VALUES[df_sum] * divisor; // statistical significance of the quadratic term
    }

#pragma region "Feature Detection"

    double medianVec(const std::vector<float> *vec)
    {
        std::vector<float> tmpDiffs = *vec;
        std::sort(tmpDiffs.begin(), tmpDiffs.end());

        size_t size = tmpDiffs.size();
        assert(size > 0);
        double diff = (size % 2 == 0)
                          ? (tmpDiffs[size / 2 - 1] + tmpDiffs[size / 2]) / 2
                          : tmpDiffs[size / 2];
        return diff;
    }

    static float weightedMeanAndVariance_EIC(const float *weight,
                                             const float *values,
                                             const size_t length,
                                             float *variance)
    {
        assert(weight[0] > 0);
        assert(values[0] > 0);
        assert(weight[length - 1] > 0);
        assert(values[length - 1] > 0);

        // weighted mean using intensity as weighting factor and left_limit right_limit as range
        size_t realPoints = 0;
        double mean_weights = 0;
        double sum_weighted_x = 0; // sum of values * weight
        double sum_weight = 0;
        for (size_t j = 0; j < length; j++)
        {
            // multiplication with one or zero is used instead of a continue so this can be vectorised.
            // @todo use another method of omitting interpolations
            unsigned int interpolated = values[j] == 0 ? 0 : 1;
            mean_weights += weight[j] * interpolated;
            sum_weighted_x += values[j] * weight[j] * interpolated;
            sum_weight += weight[j] * interpolated;
            realPoints += 1 * interpolated; // interpolated points do not count!
        }
        double dpoints = (double)realPoints;
        mean_weights /= dpoints;
        sum_weighted_x /= mean_weights;
        sum_weight /= mean_weights;

        double weighted_mean = sum_weighted_x / sum_weight;

        // only calculate the mean if the variance is not required
        if (variance == nullptr)
            return (float)weighted_mean;

        double sum_Qxxw = 0.0; // sum of (values - mean)^2 * weight
        for (size_t j = 0; j < length; j++)
        {
            double difference = values[j] - weighted_mean;
            double interpolated = values[j] == 0 ? 0 : 1; // @todo see above, add 0 if value is not real
            sum_Qxxw += interpolated * difference * difference * weight[j];
        }

        *variance = (float)sqrt(sum_Qxxw / sum_weight / dpoints);
        return (float)weighted_mean;
    };

    static FeaturePeak peakToFeat(const PeakFit *peak, const EIC *eic, unsigned int eic_ID)
    {
        const float *area_arr = eic->ints_area.data() + peak->startIdx;
        const float *mz_arr = eic->mz.data() + peak->startIdx;
        const float *dqsc_arr = eic->DQSC.data() + peak->startIdx;
        const float *rt_arr = eic->RT.data() + peak->startIdx;

        float DQSB = -1; // = weightedMeanAndVariance_EIC(area_arr, dqsb_arr, peak->length, nullptr);
        float DQSC = weightedMeanAndVariance_EIC(area_arr, dqsc_arr, peak->length, nullptr);
        assert(DQSC > 0);
        assert(DQSC <= 1);

        float mz_uncert = 0;
        float mz = weightedMeanAndVariance_EIC(area_arr, mz_arr, peak->length, &mz_uncert);
        assert(mz > 10);
        assert(mz_uncert > 0);

        return {
            peak->coeffs,
            peak->height,
            peak->area,
            peak->uncert_height,
            peak->uncert_area,
            peak->dqs,
            DQSB,
            DQSC,
            peak->position,
            mz,
            peak->uncert_position,
            mz_uncert,
            eic_ID,
            (unsigned int)peak->startIdx,
            (unsigned int)peak->length,
            rt_arr[0],
            rt_arr[peak->length - 1]};
    }

    int findFeatures(const std::vector<EIC> *EICs,
                     const std::vector<float> *convertRT, // correct RT corresponding to every scan number
                     std::vector<FeaturePeak> *res)
    {
        std::vector<PeakFit> peaks;

        // the only relevant change here is that the start point of the x axis (RT) depends on the first scan in an EIC
        // it is already assured that a bin contains continuous data
        size_t eicCount = EICs->size();
        for (size_t eic_ID = 0; eic_ID < eicCount; eic_ID++)
        {
            const EIC *bin = EICs->data() + eic_ID;
            const float *rt = convertRT->data() + bin->scanNumbers[0];
            size_t binLen = bin->df.size();
            qpeaks_find(bin->ints_area.data(),
                        rt,
                        bin->df.data(),
                        binLen, 30, // @todo dynamic maxscale
                        &peaks);
            for (size_t peak = 0; peak < peaks.size(); peak++)
            {
                FeaturePeak feat = peakToFeat(&peaks[peak], bin, eic_ID);
                res->push_back(feat);
            }
            peaks.clear();
        }

        // peaks are sorted here so they can be treated as const throughout the rest of the program
        std::sort(res->begin(), res->end(), [](FeaturePeak lhs, FeaturePeak rhs)
                  { return lhs.retentionTime < rhs.retentionTime; });

        return res->size();
    }

#pragma endregion "Feature Detection"

#pragma region "find centroids"

    static CentroidPeak peakToCen(const PeakFit *peak, size_t id, size_t specNum)
    {
        CentroidPeak cen = {0};

        cen.area = peak->area;
        cen.areaUncertainty = peak->uncert_area;
        cen.DQSC = peak->dqs;
        cen.height = peak->height;
        cen.heightUncertainty = peak->uncert_height;
        cen.ID = id;
        cen.mz = peak->position;
        cen.mzUncertainty = peak->uncert_position;
        cen.number_MS1 = specNum;
        cen.scale = peak->coeffs.scale;
        cen.width = peak->fwhm;

        return cen;
    }

    // @todo find a better way of determining the smallest possible upper scale
    static const size_t maxscale_cen = 10;

    int findCentroids(const XML_File *data,
                      const std::vector<unsigned int> *selectedIndices,
                      std::vector<CentroidPeak> *centroids)
    {
        assert(!data->defective);
        assert(centroids->empty());

        std::vector<float> spectrum_mz;
        spectrum_mz.reserve(1000);
        std::vector<float> spectrum_int;
        spectrum_int.reserve(1000);
        std::vector<PeakFit> ret;

        for (size_t specNum = 0; specNum < selectedIndices->size(); specNum++)
        {
            int ok = get_spectrum(data,
                                  &spectrum_mz,
                                  &spectrum_int,
                                  selectedIndices->at(specNum));
            assert(ok == 0);

            const size_t peaksFound = qpeaks_find(spectrum_int.data(),
                                                  spectrum_mz.data(),
                                                  nullptr,
                                                  spectrum_int.size(),
                                                  maxscale_cen,
                                                  &ret);

            for (size_t p = 0; p < peaksFound; p++)
            {
                PeakFit *peak = ret.data() + p;
                size_t id = centroids->size();
                centroids->push_back(peakToCen(peak, id, specNum));
            }

            spectrum_mz.clear();
            spectrum_int.clear();
            ret.clear();
        }
        return centroids->size();
    }

    void centroids_to_mzml(const path_char *pathTarget, const path_char *pathSource)
    {
        assert(std::strcmp(pathTarget, pathSource) != 0);

        // only work on the copy
        XML_File source_file(pathSource, mzML);
        const size_t numSpecs = source_file.number_spectra;

        std::vector<float> spectrum_mz;
        std::vector<float> spectrum_int;
        std::vector<PeakFit> result;
        std::vector<double> res_spectrum_mz;
        std::vector<double> res_spectrum_int;
        std::vector<char> char_spectrum_mz;
        std::vector<char> char_spectrum_int;

        for (size_t i = 0; i < numSpecs; i++)
        {
            // 1) obtain next spectrum that is MS level 1 and not centroided
            pugi::xml_node *spec = source_file.linknodes->data() + i;

            bool isCentroid = spec->find_child_by_attribute("cvParam", "accession", "MS:1000127") != nullptr;
            if (isCentroid)
                continue; // this does not allow for processing of partially centroided data

            int ms_level = spec->find_child_by_attribute("cvParam", "name", "ms level").attribute("value").as_int();
            if (ms_level != 1)
                continue; // centroiding only makes sense for ms level 1

            get_spectrum(&source_file, &spectrum_mz, &spectrum_int, i);

            // 3) extract spectum data, process with qpeaks_find
            int numPeaks = qpeaks_find(spectrum_int.data(),
                                       spectrum_mz.data(),
                                       nullptr,
                                       spectrum_mz.size(),
                                       40,
                                       &result);
            assert(numPeaks > 0);
            const size_t numPeaks_u = (size_t)numPeaks;

            // 4) write centroid m/z and intensity into two float arrays
            res_spectrum_int.resize(numPeaks_u);
            res_spectrum_mz.resize(numPeaks_u);
            for (size_t peak = 0; peak < numPeaks_u; peak++)
            {
                res_spectrum_int[peak] = result[peak].area;
                res_spectrum_mz[peak] = result[peak].position;
            }

            // 5) compress arrays to char *
            compress_and_encode(&res_spectrum_int, &char_spectrum_int);
            compress_and_encode(&res_spectrum_mz, &char_spectrum_mz);

            // 6) set spectrum mode to centroided. This means replacing the attribute name and value
            spec->find_child_by_attribute("cvParam", "name", "profile spectrum").set_value("centroid spectrum");
            spec->find_child_by_attribute("cvParam", "accession", "MS:1000128").set_value("MS:1000127");

            // 7) overwrite intensity and m/z for spectrum, also set length of array
            pugi::xml_named_node_iterator dataArray = spec->child("binaryDataArrayList").children("binaryDataArray").begin();
            dataArray->attribute("encodedLength").set_value(char_spectrum_mz.size());
            dataArray->child("binary").set_value(char_spectrum_mz.data());
            dataArray->next_sibling();
            dataArray->attribute("encodedLength").set_value(char_spectrum_int.size());
            dataArray->child("binary").set_value(char_spectrum_int.data());

            // reset storage vectors to avoid in-loop allocation
            spectrum_mz.clear();
            spectrum_int.clear();
            result.clear();
            res_spectrum_mz.clear();
            res_spectrum_int.clear();
            char_spectrum_mz.clear();
            char_spectrum_int.clear();
        }
        source_file.mzml_base_document.save_file(pathTarget);
        source_file.free_linknodes();
    }

#pragma region "Helper math"

    inline double regAt(const RegCoeffs *coeff, const double x)
    {
        const double b23 = x < 0 ? coeff->b2 : coeff->b3;
        return coeff->b0 + (coeff->b1 + x * b23) * x;
    }

    double regExp_fac(const RegCoeffs *coeff, const double x)
    {
        double b23 = x < 0 ? coeff->b2 : coeff->b3;
        return exp((coeff->b1 + x * b23) * x);
    }

    double fullWidthHalfMax(const RegCoeffs *coeff, const double height, const double delta_x)
    {
        // solve height / 2 = exp(b0 + x b1 + x^2 b2)
        double y = log(height / 2);
        // use quadratic formula for a x^2 + b x + c = 0
        double a_l = coeff->b2;
        double a_r = coeff->b3;
        double b = coeff->b1;
        double c = coeff->b0 - y;

        double x_l = NAN;
        double x_r = NAN;
        double dummy = NAN;
        solveQuadratic(a_l, b, c, &x_l, &dummy);
        solveQuadratic(a_r, b, c, &dummy, &x_r);

        // special case: valley is such that one half of the peak is not defined at half maximum

        if (x_l == INFINITY)
        {
            assert(a_l > 0);
            x_l = -b / (2 * a_l);
        }
        if (x_r == INFINITY)
        {
            assert(a_r > 0);
            x_r = -b / (2 * a_r);
        }

        assert(x_l < x_r);

        return (x_r - x_l) * delta_x;
    }

    double matProductReg(const double J[4], const size_t scale)
    {
        // Calculate the Matrix Product of J * Xinv * J^T for uncertainty calculation,
        // Where J is the jacobian matrix of the regression property A for which the
        // uncertainty should be calculated (J = [d A / d b0, d A / d b1, d A / d b2, d A / d b3])

        ///                   | A  0  B  B |    | J0 |
        ///                   | 0  C  D -D |    | J1 |
        /// | J0 J1 J2 J3 | x | B  D  E  F | x  | J2 |
        ///                   | B -D  F  E |    | J3 |
        ///
        ///                                                                    | J0 |
        /// |   J0 A + J1 0;   J0 0 + J1 C;   J0 B + J1 D;   J0 B - J1 D |     | J1 |
        /// | + J2 B + J3 B; + J2 D - J3 D; + J2 E + J3 F; + J2 F + J3 E |  x  | J2 |
        ///                                                                    | J3 |
        /// The product is a scalar
        ///
        ///   J0^2 A + J0 J2 B + J0 J3 B
        /// + J1^2 C + J1 J2 D - J1 J3 D
        /// + J0 J2 B + J1 J2 D + J2^2 E + J2 J3 F
        /// + J0 J3 B - J1 J3 D + J2 J3 F + J3^2 E
        ///
        /// simplify
        ///
        ///   J0^2 A + J1^2 C + (J2^2 + J3^2) E
        /// + J0 B * (J2 + J3) + J1 D * (J2 - J3)
        /// + J0 B * (J2 + J3) + J1 D * (J2 - J3)
        /// + 2 * J2 J3 F
        ///
        ///   J0^2 A + J1^2 C + (J2^2 + J3^2) E
        /// + 2 * J0 B * (J2 + J3) + 2 * J1 D * (J2 - J3) + 2 * J2 J3 F
        ///
        /// = J0^2 A + J1^2 C + (J2^2 + J3^2) E + 2 * (J0 B * (J2 + J3) + J1 D * (J2 - J3) + J2 J3 F )

        assert(scale > 1);
        const MatInverse inv = qalgo_matInverse[scale];
        double vecMatrxTranspose = J[0] * J[0] * inv.A +
                                   J[1] * J[1] * inv.C +
                                   (J[2] * J[2] + J[3] * J[3]) * inv.E +
                                   2 * (J[0] * inv.B * (J[2] + J[3]) + J[1] * inv.D * (J[2] - J[3]) + J[2] * J[3] * inv.F);
        return vecMatrxTranspose;
    }

    double peakArea(const RegCoeffs *c, const double delta_x, const double mse, double *uncert)
    {
        // base function: integral of e^(b0 + b1 x + b2 x^2) dx =
        // [ sqrt(pi) * e^( b0 - b1^2/(4 b2) ) * erfi( (b1 + 2 b2 x) / (2 sqrt(b2)) ) ] / (2 sqrt(b2))   // source: wolfram alpha
        // erfi(x) = i * erf(i * x) * -1
        // under the condition b2 < 0: sqrt(b2) = i * sqrt(-b2), where sqrt(-b2) is a real number
        // [(real part) * -1 * i * erf( i * (real) / (i * 2 * sqrt(-b2)) ) ] / (i * 2 * sqrt(-b2))
        // i within and outside of the error function cancel each other out: i / i = 1
        // erf(0) = 0 ; erf(-inf) = -1 ; erf(inf) = 1
        // F(-inf) = [ (...) * -1 * erf( (b1 + 2 b2 * -inf) / (> 0) ) ] / (...)
        // when evaluating F towards infinity, b2 or b3 always have a negative sign
        // b2 * -inf = +inf ; b2 * +inf = -inf
        // F(-inf) = [ sqrt(pi) * e^( b0 - b1^2/(4 b2) ) * -1 * +1 ] / (2 sqrt(-b2))
        // F(+inf) = [ sqrt(pi) * e^( b0 - b1^2/(4 b2) ) * -1 * -1 ] / (2 sqrt(-b2))
        // F(0)    = [ sqrt(pi) * e^( b0 - b1^2/(4 b2) ) * -1 * erf( b1 / (2 sqrt(-b2)) ) ] / (2 sqrt(-b2))
        // if b2 or b3 are positive, erfi has to be used. The positive part of the function replaces F(+-inf)
        // for reasons of numerical stability, the constant scaling factor    b0_exp * sqrt(pi) / 2    is factored out.
        // the final equation is composed as follows:
        // A = b0_exp * sqrt(pi) / 2 * (F(0)_b2 - F(-inf)_b2 + F(inf)_b3 - F(0)_b3)
        // for every peak half, F can be written as exp_b23 * erf_b23 / sqrt_b23, where only erf_b23 changes. As such:
        // A = b0_exp * sqrt(pi) / 2 * (exp_b2 / sqrt_b2 * (erf_b2_0 - erf_b2_-inf) + exp_b3 / sqrt_b3 * (erf_b3_inf - erf_b3_0))
        // apply erf(-x) = -erf(x); erfc(-x) = erf(x) + 1:
        // left:  erf_b2_0 - erf_b2_-inf =    -erf(b1 / (2 sqrt(-b2)) + 1 = erfc( b1 / sqrt(-b2))
        // right: erf_b3_inf - erf_b3_0  = 1 + erf(b1 / (2 sqrt(-b3))     = erfc(-b1 / sqrt(-b3))
        // A = b0_exp * sqrt(pi) / 2 * (
        //      e^(-b1^2/(4 b2)) * erfc( b1 / (2 sqrt(-b2)) ) / sqrt(-b2) +
        //      e^(-b1^2/(4 b3)) * erfc(-b1 / (2 sqrt(-b3)) ) / sqrt(-b3)
        // )
        // erfcx(x) = e^(x^2) * erfc(x); (b1 / (2 sqrt(-b2)))^2 = -b1^2 / (4 b2); (-b1 / (2 sqrt(-b2)))^2 = -b1^2 / (4 b2)
        // A = b0_exp * sqrt(pi) / 2 * (
        //      erfcx( b1 / (2 sqrt(-b2)) ) / sqrt(-b2) +
        //      erfcx(-b1 / (2 sqrt(-b3)) ) / sqrt(-b3)
        // )
        // in implementation, sqrt(pi) / 2 is multiplied with each half individually so that the case
        // of a positive coefficient is handled cleanly.

        // if there is a valley point, the antiderivative evaluated at the valley is always zero. This
        // is because erfi(b1 + 2 b2 x) resolves to erfi(b1 + 2 b2 * (-b1 / (2 b2))) == 0
        // (the valley point is always at -b1 / (2 b2), refer to apex position calculation).
        // The area of the half is then:
        // A_L = e^(b0) * sqrt(pi) / 2 * e^(-b1^2/(4 b2)) * erfi(b1 / (2 sqrt(b2)))  / sqrt(b2)
        // This suffers from the same algorithmic instability as the positive case.
        // Here, we can use Dawson's integral D(x) = 1/2 sqrt(pi) * e^(-x^2) * erfi(x)
        // A_L = e^(b0) * D(b1 / (2 sqrt(b2))) / sqrt(b2)
        // The same transformation applies to b3. Depending on left / right valley position,
        // we calculate the partial area as A_L - 0 or 0 - A_R

        const double b0 = c->b0;
        const double b1 = c->b1;
        const double b2 = c->b2;
        const double b3 = c->b3;
        const double sqrt_pi_2 = 1.7724538509055158819 / 2; // sqrt(M_PI);

        bool b2_neg = b2 < 0;
        bool b3_neg = b3 < 0;
        assert(b2_neg || b3_neg);
        // the apex cannot be left if b2 is positive, same for b3
        // (NOT b2_neg) NAND (b1 < 0)
        // NOT ((NOT b2_neg) AND (b1 < 0))
        // b2_neg OR (NOT b1 < 0) // b1 != 0
        // b2_neg OR b1 > 0
        assert(b2_neg || b1 > 0);
        assert(b3_neg || b1 < 0);
        assert(delta_x > 0);

        double sqrt_b2 = sqrt(abs(b2));
        double sqrt_b3 = sqrt(abs(b3));
        double b0_exp = exp(b0);

        double area_L = -1;
        if (b2_neg)
        {
            area_L = libcerf::erfcx(b1 / sqrt_b2 / 2) / sqrt_b2 * sqrt_pi_2;
        }
        else
        {
            area_L = libcerf::dawson(b1 / sqrt_b2 / 2) / sqrt_b2;
        }
        assert(area_L > 0);

        double area_R = -1;
        if (b3_neg)
        {
            area_R = libcerf::erfcx(-b1 / sqrt_b3 / 2) / sqrt_b3 * sqrt_pi_2;
        }
        else
        {
            area_R = -libcerf::dawson(b1 / sqrt_b3 / 2) / sqrt_b3;
        }
        assert(area_R > 0);

        double area_F = (area_L + area_R) * b0_exp;

        if (mse <= 0)
        {
            // it should be possible to calculate the area without supplying a mse
            assert(uncert == nullptr);
            return area_F * delta_x;
        }

        {
            // We cannot directly calculate the uncertainty from the exponential form (true peak area).
            // This is because the mse required for the uncertainty is only defined as relating to the
            // coefficients in the log-transformed space.
            // Instead, since the coefficients apply to the logarithmic transform, we calculate the
            // uncertainty of the logarithm of the area as defined above.
            //     A  = e^b0 *  (area_L + area_R) * deleta_x
            // log(A) = b0 + log(area_L + area_R) + deleta_x
            // The uncertainty is calculated using the Jacobian J, which contains the per-coefficient
            // partial derivatives. delta_x is just scaling and never relevant to uncertainty.
            // It is obvious that the derivative by b0 is 1.
            double J[4];
            J[0] = 1;

            // b1:
            // The derivative of log(x) is x' / x. Apply the chain rule and we obtain:
            // log(A)'_b1 = (area_L' + area_R') / (area_L + area_R)
            // For negative b2, the area is calculated as:
            // area_L  = erfcx(b1 / (2 sqrt(b2))) * sqrt(pi) / (2 sqrt(b2))
            // area_L' = -H_(-2)(b1 / (2 sqrt(b2))) * 1/b2
            // Where H_(-2) is the hermite polynomial evaluated at -2. The following transformation applies:
            // H_(-2)(x) = 1/2 - sqrt(pi)/2 * x * e^(x^2) * erfc(x) = 1/2 - sqrt(pi)/2 * x * erfcx(x)
            // (see https://math.stackexchange.com/questions/2238662/do-hermite-polynomials-exist-for-negative-integers)
            // replace H in the above equation:
            // area_L' = 1/(2 b2) - sqrt(pi)/2 * b1 / (2 sqrt(b2) * erfcx(b1 / (2 sqrt(b2)) * 1/b2
            // notice that we can extract the definition of the area from the derivative and obtain:
            // area_L' = (area_L * b1 - 1) / (2 b2)
            // For b2 > 0, area_L = D(b1 / 2 sqrt(b2)) / sqrt(b2), where D is dawson's integral.
            // D(a x)' = a - 2 a^2 x D(a x)
            // area_L' = 1 / (2 sqrt(b2)) * 1 / sqrt(b2) -
            //           2 ( 1 / (2 sqrt(b2)) )^2 *
            //           b1 * D(b1 / 2 sqrt(b2)) / sqrt(b2)
            // area_L' = 1 / (2 b2) -
            //           1 / (2 b2) *
            //           b1 * D(b1 / 2 sqrt(b2)) / sqrt(b2)
            // factor out the area itself and  1 / (2 b2):
            // area_L' = 1 / (2 b2) * (1 - b1 * area_L)
            // We see that the first derivative of area_L is a scaling of itself that is fully
            // independent of the sign of b2.

            // If we work with b3, we use erfcx(-x). This is equivalent to substituting b1 with -b1 and multiplying with -1
            // Consequently, we can back-substitute b1 = -b1 into the final result:
            // area_R' = 1 / (2 b3) * (1 + b1 * area_R) * -1
            // For positive values of b3, divide the full term by  -1 / sqrt(b3)  instead of +1:
            // area_R' = 1 / (2 sqrt(b3)) *  -1 / sqrt(b3) -
            //           2 ( 1 / (2 sqrt(b3)) )^2 * b1 *
            //           -D(b1 / 2 sqrt(b3)) / sqrt(b3)
            // area_R' = -1 / (2 b3) - 1 / (2 b3) * b1 * area_R
            // area_R' = -1 / (2 b3) * (1 + b1 * area_R)
            // Here, too, the two terms converge to the same scaling of the initial area

            double area_L_db1 = (1 - b1 * area_L) / (2 * b2);
            double area_R_db1 = -(1 + b1 * area_R) / (2 * b3);
            assert(area_L_db1 != NAN);
            assert(area_R_db1 != NAN);
            J[1] = (area_L_db1 + area_R_db1) / (area_L + area_R);

            // for the partial derivative by b2 and b3 for negative coefficents, there is a dependence on the
            // hermite polynomials H_(-2)(x) and H_(-1)(x):
            // area_L  = erfcx(a / (2 sqrt(x))) / sqrt(x) * sqrt(pi)/2 ; get first derivative with wolfram alpha
            // area_L' = ( a H_(-2)(a/(2 sqrt(x))) ) / (2 x^2) - ( H_(-1)(a/(2 sqrt(x))) ) / (2 x^(3/2))
            // area_L' = b1 * H_(-2)( b1 / (2 sqrt(b2)) ) / (2 b2^2) -
            //           H_(-1)( b1 / (2 sqrt(b2)) ) / (2 b2 * sqrt(b2))
            // multiply the second term with (2 sqrt(pi)) / (2 sqrt(pi)) to convert H_(-1)(x) to erfcx(x) * sqrt(pi)/2
            // area_L' = b1 * H_(-2)( b1 / (2 sqrt(b2)) ) / (2 b2^2) -
            //           erfcx( b1 / (2 sqrt(b2)) ) / (2 b2 * sqrt(b2)) * sqrt(pi)/2
            // simplyfy by applying the definition of the area to the second term
            // area_L' = b1 / (2 b2^2) * H_(-2)( b1 / (2 sqrt(b2)) ) -
            //           area_L / (2 b2)
            // apply the identity of H_(-2)(x) with x = b1 / (2 sqrt(b2)), see above
            // area_L' = b1 / (2 b2^2) * ( 1/2 - sqrt(pi)/2 * b1 / (2 sqrt(b2)) * erfcx(b1 / (2 sqrt(b2))) ) -
            //           area_L / (2 b2)
            // area_L' = b1 / (4 b2^2) -
            //           b1 / (2 b2^2) * sqrt(pi)/2 * b1 / (2 sqrt(b2)) * erfcx(b1 / (2 sqrt(b2))) -
            //           area_L / (2 b2)
            // extract the area from the second term
            // area_L' = b1 / (4 b2^2) -
            //           b1 / (2 b2^2) * b1 / 2 * area_L -
            //           area_L / (2 b2)
            // area_L' = b1 / (4 b2^2) -
            //           b1^2 / (4 b2^2) * area_L -
            //           area_L / (2 b2) * (2 b2) / (2 b2)
            // area_L' = b1 / (4 b2^2) -
            //           area_L * b1^2 / (4 b2^2) -
            //           area_L * (2 b2) / (4 b2^2)
            // area_L' = ( b1 - area_L * b1^2 - area_L * (2 b2) ) / (4 b2^2)
            // area_L' = ( b1 - area_L * (b1^2 + 2 b2) ) / (4 b2^2)

            // For positive b2 (using wolfram alpha):
            // area_L = D(a / (2 sqrt(x))) / sqrt(x); a = b2; x = b2
            // area_L' = ( (a^2 - 2 x) D(a/(2 sqrt(x))) )/(4 x^(5/2)) - a/(4 x^2)
            // area_L' = ((b1^2 - 2 b2) * area_L) / (4 b2^2) - b1 / (4 b2^2)
            // area_L' = ( area_L * (b1^2 - 2 b2) - b1 ) / (4 b2^2)

            // unlike for b1, the derivative changes for positive or negative b2.
            // This is not just a sign flip, notice that here we use a difference instead
            // of a sum. This discrepancy originates in the fact that we initially took
            // the absolute value of b2 for the negative case, but not the positive one.
            // @todo is it correct to not write "sqrt(-b2)" for the uncertainty?

            // for negative b3, set b1 = -b1:
            // area_R' = ( - b1 - area_R * (b1^2 + 2 b3) ) / (4 b3^2)
            // area_R' =  -( b1 + area_R * (b1^2 + 2 b3) ) / (4 b3^2)
            // for positive b3, multiply the whole term with -1:
            // area_R' = - ( area_R * (b1^2 - 2 b3) - b1 ) / (4 b3^2)
            // area_R' =   ( b1 - area_R * (b1^2 - 2 b3) ) / (4 b3^2)

            // @todo results are not fully symmetric - is there an error here?

            if (b2_neg)
            {
                J[2] = (b1 - area_L * (b1 * b1 + 2 * b2)) / (4 * b2 * b2);
            }
            else
            {
                J[2] = (area_L * (b1 * b1 - 2 * b2) - b1) / (4 * b2 * b2);
            }

            if (b3_neg)
            {
                J[3] = -(b1 + area_R * (b1 * b1 + 2 * b3)) / (4 * b3 * b3);
            }
            else
            {
                J[3] = (b1 - area_R * (b1 * b1 - 2 * b3)) / (4 * b3 * b3);
            }

            double u = matProductReg(J, c->scale);
            assert(u > 0);
            *uncert = sqrt(u * mse); // this is the uncertainty of the logarithmic area.
        }

        // scale by delta_x, the assumed difference between two neighbouring points in x.
        // this is done explicitly in this function since it is assumed when using the precalculated inverse.
        *uncert *= delta_x;
        return area_F * delta_x;
    }

    double peakPositionUncert(const RegCoeffs *c, const double mse)
    {
        // peak position: -b1 / (2 b23)
        // Jacobian (derivatives by b0 to b3) is 0 for b0 and b2 or b3, depending on which peak half contains the apex
        double J[4] = {0, 0, 0, 0};
        bool apex_left = c->b1 < 0;
        double b23 = apex_left ? c->b2 : c->b3;
        J[1] = -1 / (2 * b23); // dx d b1
        int idx = apex_left ? 2 : 3;
        J[idx] = c->b1 / (2 * b23 * b23); // -b1 * -1

        double u = matProductReg(J, c->scale);
        assert(u > 0);
        double uncert = sqrt(u * mse);
        return uncert;
    }

    double peakPosition(const RegCoeffs *c)
    {
        double b23 = c->b1 < 0 ? c->b2 : c->b3;
        return -c->b1 / (2 * b23);
    }

    double peakHeightUncert(const RegCoeffs *c, const double mse)
    {
        // height: exp( b0 + b1 * -b1 / (2 b23) + b23 * (-b1 / (2 b23))^2 )
        // = exp( b0 + (-b1^2 / (2 b23)) + (b23 * b1^2 / (4 b23^2)) )
        // = exp( b0 + (-b1^2 / (2 b23)) * (2 / 2) + b1^2 / (4 b23) )
        // = exp( b0 - b1^2 / (4 b23) )
        // We only deal with the log form here: b0 - b1^2 / (4 b23)

        bool apex_left = c->b1 < 0;
        double b23 = apex_left ? c->b2 : c->b3;
        double b1_sq = c->b1 * c->b1;
        // double height = exp(c->b0 - b1_sq / (4 * b23));

        // uncertainty: calculate uncertainty for the logarithmic case, then transform back
        // note: this leads to two values for uncertainty
        double deriv_b0 = -b1_sq / (4 * b23);
        double deriv_b1 = c->b0 - c->b1 / (2 * b23); // b0 + apex position
        double deriv_b23 = c->b0 + b1_sq / (4 * b23 * b23);
        // assign jacobian
        double J[4] = {deriv_b0, deriv_b1, 0, 0};
        J[apex_left ? 2 : 3] = deriv_b23;

        // double J_exp[4] = {height, -c->b1 * height / (2 * b23), 0, 0};
        // J_exp[apex_left ? 2 : 3] = b1_sq * height / (4 * b23 * b23);
        // double u_exp = matProductReg(J_exp, c->scale);
        // double test_u = sqrt(u_exp * mse_exp); // both u_exp and the MSE are too large. The combination via square root is also wrong (?)

        double u = matProductReg(J, c->scale);
        assert(u > 0);
        double uncert = sqrt(u * mse);
        // assert(uncert == test_u); // calculating the error for the exponential case directly leads to wrong results
        return uncert;
    }

} // namespace qAlgorithms