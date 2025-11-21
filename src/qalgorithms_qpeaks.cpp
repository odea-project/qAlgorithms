#include "qalgorithms_qpeaks.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h" // @todo remove coupling

#include <cassert>
#include <cmath>
#include <vector>
// #include <algorithm> // sorting
// #include <stdint.h> // max numeric vals

namespace qAlgorithms
{

    constexpr auto INV_ARRAY = initialize(); // this only works with constexpr square roots, which are part of C++26

    size_t hardFilter(std::vector<double> *mz, std::vector<double> *intensity, double minMZ, double maxMZ)
    {
        std::vector<double> mz_new;
        std::vector<double> int_new;

        size_t pos = 0;
        while (pos < mz->size() && mz->at(pos) < minMZ)
            pos += 1;

        while (pos < mz->size() && mz->at(pos) < maxMZ)
        {
            mz_new.push_back(mz->at(pos));
            int_new.push_back(intensity->at(pos));
            pos += 1;
        }

        *mz = mz_new;
        *intensity = int_new;

        return pos;
    }

    std::vector<double> predictedInt(const RegCoeffs *reg, const int startIdx, const int endIdx)
    {
        assert(startIdx < 0);
        assert(endIdx > 0);

        std::vector<double> res;
        res.reserve(endIdx - startIdx + 1);

        for (int idx = startIdx; idx <= endIdx; idx++)
        {
            res.push_back(regExpAt(reg, idx));
        }

        return res;
    }

    int qpeaks_find(
        // SOME_IMPLEMENTATION_OF_LINEAR_ALLOCATOR_HERE
        const std::vector<float> *y_values,
        const std::vector<float> *x_values,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const size_t maxScale_in,
        std::vector<RegressionGauss> *detectedPeaks)
    {
        // control input for nullpointers, mismatching x and y, and fitting maxScale
        if (y_values == nullptr || x_values == nullptr || detectedPeaks == nullptr)
        {
            return -1;
        }
        if (y_values->size() != x_values->size() || y_values->size() != degreesOfFreedom_cum->size())
        {
            return -2;
        }
        if (maxScale_in < 2)
        {
            return -3;
        }

        const size_t length = y_values->size();
        const size_t halfLength = (length - 1) / 2;
        const size_t maxScale = maxScale_in > halfLength ? halfLength : maxScale_in;

        std::vector<float> y_log;
        y_log.reserve(length);
        for (float y : *y_values)
        {
            y_log.push_back(std::log(y));
        }

        // basic structure: find cofficients, reduce them to valid regressions, resolve contradictions,

        // coefficients for single-b0 peaks, spans all regressions over a peak window
        std::vector<RegCoeffs> regressions;
        findCoefficients(&y_log, maxScale, &regressions);
        assert(!regressions.empty());

        // filter the produced coefficients so that only correct ones remain (separate function?)
        for (size_t reg = 0; reg < regressions.size(); reg++)
        {
            RegCoeffs *current = &(regressions[reg]);
            // we can only process regressions with at least one
            bool regOK = ((current->b2 < 0) || (current->b3 < 0)) && (current->b1 != 0);

            current->b0 = regOK ? current->b0 : 0;
        }

        //
        return -42;
    }

#pragma region "find peaks"
    void findCentroidPeaks(std::vector<CentroidPeak> *retPeaks, // results are appended to this vector
                           const std::vector<ProfileBlock> *treatedData,
                           const size_t scanNumber,
                           const size_t accessor,
                           const size_t maxWindowSize)
    /* ### allocations ###
        logIntensity: maxWindowSize * f32
        validRegressions: UNKNOWN * RegressionGauss (large!)

       ### called functions ###
        runningRegression
        createCentroidPeaks
    */
    {
        assert(!treatedData->empty());

        std::vector<float> logIntensity(maxWindowSize + 2 * GLOBAL_MAXSCALE_CENTROID, NAN);

        assert(GLOBAL_MAXSCALE_CENTROID <= MAXSCALE);

        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(treatedData->size() / 2); // probably too large, shouldn't matter
        for (size_t i = 0; i < treatedData->size(); i++)   // 185
        {
            const ProfileBlock *const block = treatedData->data() + i;

            assert(block->intensity.size() == block->mz.size());

            // this is now filled inside the function, the vector only reserves space. We do not
            // perform this step in the function so that it is explicitly empty. This should be
            // replaced by a non-malloc calling scratch space eventually
            logIntensity.clear();
            runningRegression(&block->intensity, &logIntensity, &block->cumdf, &validRegressions, GLOBAL_MAXSCALE_CENTROID);

            volatile bool purge = false;
            if (purge)
                validRegressions.clear();

            if (validRegressions.empty())
            {
                continue; // no valid peaks
            }
            createCentroidPeaks(retPeaks, &validRegressions, block, scanNumber, accessor);
            validRegressions.clear();
        }
        // if (retPeaks->size() == 0)
        // {
        //     // debug: print relevant data to file
        //     FILE *f = fopen("errorspec.csv", "w");
        //     if (f == NULL)
        //     {
        //         printf("Error opening file!\n");
        //         exit(1);
        //     }

        //     fprintf(f, "ID, mz, int, df\n");
        //     for (size_t i = 0; i < treatedData->size(); i++)
        //     {
        //         auto mz = treatedData->at(i).mz;
        //         auto intensity = treatedData->at(i).intensity;
        //         auto df = treatedData->at(i).cumdf;
        //         assert(mz.size() == intensity.size());
        //         for (size_t j = 0; j < mz.size(); j++)
        //         {
        //             fprintf(f, "%zu, %f, %f, %u\n", i, mz[j], intensity[j], df[j]);
        //         }
        //     }

        //     fclose(f);
        //     exit(1);
        // }
    }

#pragma endregion "find peaks"

#pragma region "running regression"

    volatile bool debug = false;
    std::vector<int> failCodes;

    size_t validateRegressions(
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const std::vector<RegCoeffs> *coefficients,
        std::vector<RegressionGauss> *validRegressions)
    {
        size_t validCount = 0;
        for (size_t i = 0; i < coefficients->size(); i++)
        {

            RegressionGauss reg;
            reg.coeffs = coefficients->at(i);
            int failpoint = makeValidRegression(degreesOfFreedom_cum,
                                                intensities,
                                                intensities_log,
                                                &reg);
            failCodes.push_back(failpoint);

            validCount += failpoint == 0 ? 1 : 0;
            if (reg.isValid)
            {
                validRegressions->push_back(reg);
            }
        }
        assert(validCount == validRegressions->size());

        failCodes.push_back(-1);

        return validCount;
    }

#ifdef _RM
    void extrapolate_old(std::vector<float> *intensities_log, const size_t fillCount)
    {
        { // extrapolate left using the first point and dividing by two fillcount times
            double baseInt = intensities_log->at(fillCount);
            for (size_t i = fillCount - 1; i < fillCount; i--)
            {
                baseInt /= 2;
                intensities_log->at(i) = baseInt;
            }
            assert(intensities_log->front() != 0);
        }
        { // extrapolate right using the last point and dividing by two fillcount times
            size_t endpos = intensities_log->size() - fillCount - 1;
            double baseInt = intensities_log->at(endpos);
            for (size_t i = endpos + 1; i < intensities_log->size(); i++)
            {
                baseInt /= 2;
                intensities_log->at(i) = baseInt;
            }
            assert(intensities_log->front() != 0);
        }
    }

    void extrapolateLogInt(std::vector<float> *intensities_log, const size_t fillCount)
    {
        assert(intensities_log->front() == 0);
        assert(intensities_log->back() == 0);
        const size_t len = intensities_log->size();

        { // extrapolate the left using the first point, the first local maximum and the point in the middle of both
            double x_leftmost = fillCount;
            double y_leftmost = intensities_log->at(x_leftmost);
            double x_firstMax = x_leftmost + 2; // there must be at least one point here for this to work
            double y_firstMax = 0;
            for (; x_firstMax < len - fillCount; x_firstMax++)
            {
                double y_tmp = intensities_log->at(x_firstMax);
                if (y_tmp < y_firstMax)
                    break;

                y_firstMax = y_tmp;
            }
            assert(y_firstMax > 0);
            // for a non-even number of points, prefer the one closer to the first apex
            double x_middle = trunc((x_leftmost + x_firstMax + 1) / 2);
            double y_middle = intensities_log->at(x_middle);

            // extrapolation on the left
            double b0, b1, b2;
            coeffsQuadratic(x_leftmost, x_middle, x_firstMax,
                            y_leftmost, y_middle, y_firstMax,
                            &b0, &b1, &b2);
            for (size_t x = 0; x < fillCount; x++)
            {
                intensities_log->at(x) = quadraticAt(b0, b1, b2, x);
            }
        }
        { // extrapolate the right using the last point, the last local maximum and the point in the middle of both
            double x_rightmost = len - fillCount - 1;
            double y_rightmost = intensities_log->at(x_rightmost);
            double x_lastMax = x_rightmost - 2; // there must be at least one point here for this to work
            double y_lastMax = 0;
            for (; x_lastMax > fillCount; x_lastMax--)
            {
                double y_tmp = intensities_log->at(x_lastMax);
                if (y_tmp < y_lastMax)
                    break;

                y_lastMax = y_tmp;
            }
            assert(y_lastMax > 0);
            // for a non-even number of points, prefer the one closer to the last apex
            double x_middle = trunc((x_rightmost + x_lastMax) / 2);
            double y_middle = intensities_log->at(x_middle);

            // extrapolation on the right
            double b0, b1, b2;
            coeffsQuadratic(x_lastMax, x_middle, x_rightmost,
                            y_lastMax, y_middle, y_rightmost,
                            &b0, &b1, &b2);
            for (size_t x = len - fillCount; x < len; x++)
            {
                intensities_log->at(x) = quadraticAt(b0, b1, b2, x);
            }
        }
    }
#endif

    void runningRegression(
        const std::vector<float> *intensities,
        std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        std::vector<RegressionGauss> *validRegressions,
        const size_t maxScale)
    /* ### allocations ###
        regressions: size determined by function

       ### called functions ###
        mergeRegressionsOverScales
    */
    {
        const size_t length = intensities->size();

#ifdef _RM
        volatile bool extrapolate = false;
        if (extrapolate)
        { // prepare the log intensity vector, including extrapolation
            assert(intensities_log->empty());
            size_t fillCount = maxScale - 2; // -2 since maxscale - minscale (== 2) is used in regression
            intensities_log->reserve(length + 2 * fillCount);
            for (size_t i = 0; i < fillCount; i++)
            {
                intensities_log->push_back(0);
            }
            for (size_t i = 0; i < length; i++)
            {
                intensities_log->push_back(log(intensities->at(i)));
            }
            for (size_t i = 0; i < fillCount; i++)
            {
                intensities_log->push_back(0);
            }

            extrapolateLogInt(intensities_log, fillCount);
            volatile bool swapInOld = false;
            if (swapInOld)
                extrapolate_old(intensities_log, fillCount);
        }
        else
#endif
        {
            intensities_log->reserve(length);
            for (size_t i = 0; i < length; i++)
            {
                intensities_log->push_back(log(intensities->at(i)));
            }
        }

        assert(validRegressions->empty());

        // coefficients for single-b0 peaks, spans all regressions over a peak window
        // all entries in coeff are sorted by scale and position in ascending order - this is not checked!
        std::vector<RegCoeffs> coefficients;
        findCoefficients(intensities_log, maxScale, &coefficients);

        std::vector<int> failures;
        failures.reserve(coefficients.size());
        std::vector<int> x0s;
        x0s.reserve(coefficients.size());

        volatile bool printDebug = false;
        if (printDebug)
        {
            FILE *f = fopen("failedRegs.csv", "w");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(f, "ID, intensity\n");
            for (float i : *intensities)
            {
                fprintf(f, "-1, %f\n", i);
            }
            size_t i = 0;
            size_t numCenters = length - 4;
            for (RegCoeffs reg : coefficients)
            {
                // index rotates as points are added by scale
                int x0 = i % numCenters + 2;
                int startIdx = -x0;
                int endIdx = length - x0 - 1;
                auto points = predictedInt(&reg, startIdx, endIdx);

                for (size_t access = 0; access < length; access++)
                {
                    fprintf(f, "%zu, %f\n", i, points[access]);
                }

                i += 1;
            }

            fclose(f);
        }

        std::vector<RegressionGauss> validRegsTmp; // all independently valid regressions regressions
        validRegsTmp.reserve(coefficients.size() / 2);
        size_t validCount = validateRegressions(intensities,
                                                intensities_log,
                                                degreesOfFreedom_cum,
                                                &coefficients,
                                                &validRegsTmp);

        // @todo find out how impactful the adjusted region where a regression holds true is
        // potentially refactor the elimination such that it is one fucntion to handle within
        // and between regs through invalidation only

        std::vector<RegressionGauss> validRegsAtScale;
        size_t currentScale = 2;
        validRegsTmp.push_back({0});
        validRegsTmp.back().coeffs.scale = 0;
        RegressionGauss *currentReg = validRegsTmp.data();

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
                findBestScales(validRegressions, &validRegsAtScale, intensities, degreesOfFreedom_cum);
            }
            // regression is not incremented because a toggle was triggered
            currentScale += 1;
            validRegsAtScale.clear();
        }

        // there can be 0, 1 or more than one regressions in validRegressions
        mergeRegressionsOverScales(validRegressions, intensities);
    }

    void findCoefficients(
        const std::vector<float> *intensity_log,
        size_t maxScale,
        std::vector<RegCoeffs> *coeffs)
    /* ### allocations ###
       coeffs: allocation size known at function call
    */
    {
        assert(maxScale >= GLOBAL_MINSCALE);
        assert(maxScale <= MAXSCALE);

        const size_t length = intensity_log->size();
        assert(length > 4);
        maxScale = min(maxScale, (length - 1) / 2);

        {
            size_t totalRegs = 0;
            for (size_t scale = 2; scale <= maxScale; scale++)
            {
                totalRegs += length - 2 * scale;
            }
            coeffs->resize(totalRegs);
        }

        // the first and last MINSCALE elements of the data do not need to be checked for x0, since they are invalid by definition
        const size_t limit = length - GLOBAL_MINSCALE;
        for (size_t x0 = GLOBAL_MINSCALE; x0 < limit; x0++)
        {
            const float *cen = intensity_log->data() + x0; // this is initially the third real point

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

            // var for access in inner loop
            size_t offset_prev = 0;
            for (size_t scale = GLOBAL_MINSCALE; scale <= maxScale; scale++)
            {
                if (x0 + scale == length)
                    break;

                // expand the kernel to the left and right of the intensity_log.
                // b0 is expanded by the two outer points * 1
                product_sum_b0 += cen[-scale] + cen[scale];
                // b1 is expanded by the points * scale, negative to the left
                product_sum_b1 += -double(scale) * cen[-scale] + double(scale) * cen[scale];
                // b2 and b3 are expanded by scale^2 the outermost point to the left or right
                double scale_sqr = double(scale * scale);
                product_sum_b2 += scale_sqr * cen[-scale];
                product_sum_b3 += scale_sqr * cen[scale];

                const double *inv = INV_ARRAY.data() + scale * 6;
                const double inv_A = inv[0];
                const double inv_B = inv[1];
                const double inv_C = inv[2];
                const double inv_D = inv[3];
                const double inv_E = inv[4];
                const double inv_F = inv[5];

                const double inv_B_b0 = inv_B * product_sum_b0;
                const double inv_D_b1 = inv_D * product_sum_b1;

                // access is determined by scale and x0.
                // scale 2: idx is x0 - scale
                // scale 3: length - ((scale - 1) * 2) + x0 - scale
                // scale 4: 2 * length - (scale - 1) * 2 - (scale - 2) * 2 + x0 - scale
                //          2 * length - scale * 4 + 6 + x0 - scale
                const size_t offset_front = x0 - scale;
                const size_t access = offset_front + offset_prev;
#define current (*coeffs)[access]
                current.b0 = inv_A * product_sum_b0 + inv_B * (product_sum_b2 + product_sum_b3);
                current.b1 = inv_C * product_sum_b1 + inv_D * (product_sum_b2 - product_sum_b3);
                current.b2 = inv_B_b0 + inv_D_b1 + inv_E * product_sum_b2 + inv_F * product_sum_b3;
                current.b3 = inv_B_b0 - inv_D_b1 + inv_F * product_sum_b2 + inv_E * product_sum_b3;
                current.scale = scale;
                current.x0 = x0;
#undef current

                // next scale would access front of vector
                if (offset_front == 0)
                    break;

                offset_prev += length - 2 * scale;
            }
        }

        return;
    }

#ifdef _RM
    RegCoeffs hardRegression(std::vector<float> *intensity_log, const size_t startIdx, const size_t endIdx)
    {
        // 1) find the position 0 - this should generally be the maximum of the region
        float *intensityP = intensity_log->data() + startIdx;
        size_t length = endIdx - startIdx + 1;
        float *maxInt = maxVal(intensityP, length);
        size_t x0 = maxInt - intensity_log->data();

        // 2) design matrix covering the entire data region - assumes good cutoffs
        /*todo: matrixtype*/ double *designMat = 0;
        // length times 1
        // from -x0 to length - x0: i++
        // while i < x0: mat[i]^2
        // start at i = x0, until i = length

        return {0};
    }
#endif

#pragma endregion "running regression"

#pragma region "validate Regression"

    size_t invalidateSuboptimal_inScale(
        const std::vector<float> *intensities,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        std::vector<RegressionGauss> *validRegressions)
    {
        // goal: for every regression in the current scale, select the one with the
        // best MSE and then mark all other ones as invalid.

        assert(!validRegressions->empty());
        assert(!intensities->empty());
        assert(validRegressions->back().coeffs.scale < intensities->size() / 2);

        size_t totalValid = 0;

        RegressionGauss *conflictReg = nullptr;
        size_t currentScale = 2;

        for (size_t i = 0; i < validRegressions->size(); i++)
        {
            RegressionGauss *reg = validRegressions->data() + i;
            if (!reg->isValid)
                continue;

            // the first round of comparisons is only concerned with peaks within one scale
            // @todo is this the correct approach?
            if (reg->coeffs.scale != currentScale)
            {
                // reset the comparison
                conflictReg = nullptr;
                assert(currentScale + 1 == reg->coeffs.scale);
                currentScale += 1;
            }

            // no conflict -> this one needs to be tested against
            if (conflictReg == nullptr)
            {
                conflictReg = reg;
                continue;
            }

            // check if the regression is in range. A regression is in range if
            // the difference between apexes is smaller than 4 or the scale,
            // whichever one is larger
            double critDiff = max(double(currentScale), 4.0);
            // current position should alwasys be greater than old
            double apexDiff = reg->apex_position - conflictReg->apex_position;
            assert(apexDiff > 0);

            if (apexDiff > critDiff)
            {
                // no conflict, test other reg
                conflictReg = reg;
                totalValid += 1;
                continue;
            }

            // expand the range until no overlap between regressions exists @todo should this be updated in the chosen regression?
            Range_i limits = conflictReg->regSpan;

            size_t lookAhead = i;
            for (; lookAhead < validRegressions->size(); lookAhead++)
            {
                // check how far the region can expand before it no longer makes sense to comapare regressions
                RegressionGauss *forward = validRegressions->data() + lookAhead;

                if (forward->coeffs.scale != currentScale)
                    break;
                if (!forward->isValid)
                    continue;
                if (forward->apex_position > float(limits.endIdx))
                    break;

                assert(limits.endIdx <= forward->regSpan.endIdx);
                limits.endIdx = forward->regSpan.endIdx;
            }

            // within this range, find the best mse. conflictReg is always the
            // better of the two.
            size_t df_sum = calcDF_cum(degreesOfFreedom_cum, limits);
            double bestMSE = calcSSE_exp(&conflictReg->coeffs,
                                         intensities,
                                         &limits);
            bestMSE /= (df_sum - 4);

            for (; i < lookAhead; i++) // at least one conflict needs to be resolved if the loop gets this far
            {
                reg = validRegressions->data() + i;

                double mse = calcSSE_exp(&reg->coeffs,
                                         intensities,
                                         &limits);
                mse /= (df_sum - 4);

                bool replace = mse < bestMSE;
                if (replace)
                {
                    conflictReg->isValid = false;
                    conflictReg = reg;
                    bestMSE = mse;
                }
            }

            // at this point, only one regression in the conflict window remains
            conflictReg = nullptr;
            totalValid += 1;
        }

        return totalValid;
    }

    void findBestScales(std::vector<RegressionGauss> *validRegressions,
                        std::vector<RegressionGauss> *validRegsTmp,
                        const std::vector<float> *intensities,
                        const std::vector<unsigned int> *degreesOfFreedom_cum)
    /* ### allocations ###
        startEndGroups: known at function call

       ### called functions ###
        std::abs
        findBestRegression
    */
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
        // @todo could this part be combined with merge over scales?
        // vector with the access pattern [2*i] for start and [2*i + 1] for end point of a regression group
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

            if (reg1->apex_position > reg2->regSpan.startIdx)
                continue;

            if (reg1->regSpan.endIdx > reg2->apex_position)
                continue;

            // the two regressions differ, i.e. create a new group
            groups.push_back({prev_i, i});
            prev_i = i + 1;
        }
        groups.push_back({prev_i, validRegsTmp->size() - 1}); // last group ends with index of the last element

        /*
          Survival of the Fittest Filter:
          This block of code implements the survival of the fittest filter. It selects the peak with
          the lowest mean squared error (MSE) as the representative of the group. If the group contains
          only one peak, the peak is directly pushed to the valid regressions. If the group contains
          multiple peaks, the peak with the lowest MSE is selected as the representative of the group
          and pushed to the valid regressions.
        */
        // @todo use a "ridges" approach here (gaussian mixture model)
        for (size_t groupIdx = 0; groupIdx < groups.size(); groupIdx += 2)
        {
            if (groups[groupIdx].startIdx == groups[groupIdx].endIdx)
            { // already isolated peak => push to valid regressions
                size_t regIdx = groups[groupIdx].startIdx;
                auto onlyReg = validRegsTmp->at(regIdx);
                validRegressions->push_back(onlyReg);
            }
            else
            { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
                RegPair bestRegIdx = findBestRegression(intensities, validRegsTmp, degreesOfFreedom_cum,
                                                        groups[groupIdx]);

                RegressionGauss bestReg = validRegsTmp->at(bestRegIdx.idx);
                bestReg.mse = bestRegIdx.mse;
                validRegressions->push_back(bestReg);
            }
        }
    }

    void updateRegRange(RegressionGauss *mutateReg, const double valleyPos);

    double correctLogBias(const std::vector<float> *intensities, const RegCoeffs *coeff)
    {
        // problem: after the log transform, regression residuals are not directly transferable to
        // the retransformed model. The error Err of the log model is: y_log = b0 + b1 * x + b23 * x^2 + Err
        // (b23 depending on the regression half). After the exponential retransform, the error term is exp(Err)
        // refer to: https://doi.org/10.1002/etc.5620120618
        // as follows from eq. 9, the error for the coefficients is log(sum(exp(residual_(0:n))) / n)

        // this function calculates the error term for a given set of regression coefficients

        double expError = 0;
        int start = coeff->x0 - coeff->scale;
        int end = coeff->x0 + coeff->scale;
        for (int i = start; i <= end; i++)
        {
            double xval = i - coeff->x0;
            double reg = regExpAt(coeff, xval);
            double real = intensities->at(i);
            double residual = real - reg;
            expError = exp(residual);
        }
        expError /= intensities->size();
        double bias = log(expError);

        return bias;
    }

    int makeValidRegression( // returns the number of the failed test
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        RegressionGauss *mutateReg)
    /* ### allocations ###
        selectLog: size calculated in function, max size known
        predictLog: s.o.
    */
    {
        assert(!mutateReg->isValid);
        const size_t scale = mutateReg->coeffs.scale;
        assert(scale > 1);
        size_t length = intensities_log->size();
        assert(mutateReg->coeffs.x0 + scale < length);

        // for a regression to be valid, at least one coefficient must be < 0
        if (mutateReg->coeffs.b2 >= 0 && mutateReg->coeffs.b3 >= 0)
        {
            return 1;
        }

        /*
          Apex and Valley Position Filter:
          This block of code implements the apex and valley position filter.
          It calculates the apex and valley positions based on the coefficients
          matrix B.
        */
        double valley_position = 0;
        int failure = calcApexAndValleyPos(mutateReg, &valley_position);
        if (failure != 0) // something went wrong
        {
            return 2; // invalid apex and valley positions
        }
        /*
          Area Pre-Filter:
          This test is used to check if the later-used arguments for exp and erf
          functions are within the range for which both are defined, i.e., |x^2| < 25. x is in this case
          -apex_position * b1 / 2 and -valley_position * b1 / 2.
          The test is moved here instead of isValidPeakArea to save computation time @todo is this measureable?
        */
        if (mutateReg->apex_position * mutateReg->coeffs.b1 > 50 || valley_position * mutateReg->coeffs.b1 < -50)
        {
            return 3; // invalid area pre-filter
        }

        updateRegRange(mutateReg, valley_position);
        assert(mutateReg->regSpan.endIdx < length);
        if (mutateReg->coeffs.x0 - mutateReg->regSpan.startIdx < 2 ||
            (mutateReg->regSpan.endIdx - mutateReg->coeffs.x0 < 2))
        {
            // only one half of the regression applies to the data, since the
            // degrees of freedom for the "squished" half results in an invalid regression
            return 4;
        }

        size_t df_sum = calcDF_cum(degreesOfFreedom_cum, mutateReg->regSpan);
        if (df_sum < 5)
        {
            // degree of freedom less than 5; i.e., less than 5 measured data points.
            // Four points or less are not enough to fit a regression with four coefficients
            return 5;
        }

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates
          the ratio of the apex signal to the edge signal and ensures that the
          ratio is greater than 2. This is a pre-filter for later
          signal-to-noise ratio checkups. apexToEdge is also required in isValidPeakHeight further down
        */

        float apexToEdge = apexToEdgeRatio(mutateReg, intensities);
        if (!(apexToEdge > 2))
        {
            return 6; // invalid apex to edge ratio
        }

        double RSS_reg = calcRSS_reg(mutateReg, intensities_log); // @todo we should use the exponential for this

        /*
        competing regressions filter:
        If the real distribution of points could also be described as a continuum (i.e. only b0 is relevant),
        the regression does not describe a peak. This is done through a nested F-test against a constant that
        is the mean of all predicted values. @todo test this function
        */
        bool f_ok = f_testRegression(intensities, RSS_reg, &mutateReg->regSpan);
        if (!f_ok)
        {
            return 7; // H0 holds, the two distributions are not noticeably different
        }

        // mean squared error with respect to the degrees of freedom - @todo is the -4 correct?
        // the quadratic mse is used for the weighted mean of the coefficients later
        mutateReg->mse = RSS_reg / double(df_sum - 4);

        if (!isValidQuadraticTerm(mutateReg, df_sum))
        {
            return 8; // statistical insignificance of the quadratic term
        }
        if (!isValidPeakArea(mutateReg, df_sum))
        {
            return 9; // statistical insignificance of the area
        }

        /*
          Height Filter:
          This block of code implements the height filter. It calculates the height
          of the peak based on the coefficients matrix B. Then it calculates the
          uncertainty of the height based on the Jacobian matrix and the variance-covariance
          matrix of the coefficients.
        */
        calcPeakHeightUncert(mutateReg);
        if (1 / mutateReg->uncertainty_height <= T_VALUES[df_sum - 5]) // statistical significance of the peak height
        {
            return 10;
        }
        // at this point without height, i.e., to get the real uncertainty
        // multiply with height later. This is done to avoid exp function at this point
        // if (!isValidPeakHeight(mutateReg, valley_position, df_sum, apexToEdge))
        // {
        //     return 11; // statistical insignificance of the height
        // }

        /*
          Area Filter:
          This block of code implements the area filter. It calculates the Jacobian
          matrix for the peak area based on the coefficients matrix B. Then it calculates
          the uncertainty of the peak area based on the Jacobian matrix.
          NOTE: this function does not consider b0: i.e. to get the real uncertainty and
          area multiply both with Exp(b0) later. This is done to avoid exp function at this point
        */
        // it might be preferential to combine both functions again or store the common matrix somewhere
        calcPeakAreaUncert(mutateReg);
        if (mutateReg->area / mutateReg->uncertainty_area <= T_VALUES[df_sum - 5])
        {
            return 12; // statistical insignificance of the area
        }

        /*
          Chi-Square Filter:
          This block of code implements the chi-square filter. It calculates the chi-square
          value based on the weighted chi squared sum of expected and measured y values in
          the exponential domain. If the chi-square value is less than the corresponding
          value in the CHI_SQUARES, the regression is invalid.
        */
        float chiSquare = calcSSE_chisqared(mutateReg, intensities);
        if (chiSquare < CHI_SQUARES[df_sum - 5])
        {
            return 13; // statistical insignificance of the chi-square value
        }

        /*
          Smearing Correction:
          The coefficient beta_0 is corrected by the smearing approach from Naihua Duan.
          The new cofficient is then b0* = b0 + logC, where C is the correction factor.
          first: logC; second: variance of logC

          NOTE: Results can be distorted for the worse when applying smearing correction
        */
        // auto smearing = smearingCorrection(&predictLog, &selectLog, scale);
        // mutateReg->coeffs.b0 += smearing.log_C; // b0* = b0 + logC
        // @todo: implement smearing.second for the uncertainty of b0

        calcUncertaintyPos(mutateReg);
        mutateReg->df = df_sum - 4; // @todo add explanation for -4
        mutateReg->apex_position += mutateReg->coeffs.x0;

        // @todo is it actually a problem if the apex is within a two-point region?
        // if (mutateReg->apex_position < 2 || mutateReg->apex_position > maxApexPos)
        // { // this situation implies that only one half of the peak has the minimum data points for a gaussian
        //     return 14;
        // }

        mutateReg->isValid = true;
        return 0;
    }

    void updateRegRange(RegressionGauss *mutateReg, const double valleyPos)
    {
        const size_t scale = mutateReg->coeffs.scale;
        const size_t idxCenter = mutateReg->coeffs.x0;
        size_t rangeStart = idxCenter - scale;
        size_t rangeEnd = idxCenter + scale;

        mutateReg->regSpan = {rangeStart, rangeEnd};
        if (valleyPos == 0) // no valley point exists
            return;

        // set start or end to the valley point if it is within the regression span
        size_t absValley = size_t(abs(valleyPos));
        bool valleyLeft = valleyPos < 0;
        bool updateVal = absValley < scale;
        if (!updateVal)
            return;

        if (valleyLeft)
        {
            mutateReg->regSpan.startIdx = idxCenter - absValley;
        }
        else
        {
            mutateReg->regSpan.endIdx = idxCenter + absValley;
        }
        assert(mutateReg->regSpan.endIdx > mutateReg->regSpan.startIdx);
    }

#pragma endregion "validate Regression"

    void mergeRegressionsOverScales(std::vector<RegressionGauss> *validRegressions,
                                    const std::vector<float> *intensities)
    /* ### allocations ###
        exponentialMSE: known at function call
        validRegressionsInGroup: size unknown

       ### called functions ###
        calcSSE_exp
        shrink_to_fit
    */
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
        for (size_t i = 0; i < validRegressions->size(); i++)
        {
            RegressionGauss *activeReg = &(*validRegressions)[i];
            assert(activeReg->isValid);
            double MSE_group = 0;
            int DF_group = 0;
            // only calculate required MSEs since this is one of the performance-critical steps
            std::vector<float> exponentialMSE(validRegressions->size(), 0);
            std::vector<unsigned int> validRegressionsInGroup; // vector of indices to validRegressions
            validRegressionsInGroup.reserve(64);
            size_t competitors = 0; // a competitor is a mutually exclusive alternative regression

            // iterate over the validRegressions vector till the new peak
            // first iteration always false
            for (size_t j = 0; j < i; j++)
            {
                RegressionGauss *secondReg = &(*validRegressions)[j];
                if (!secondReg->isValid) // check is needed because regressions are set to invalid in the outer loop
                    continue;

                if (activeReg->apex_position < secondReg->regSpan.startIdx)
                    continue;

                if (activeReg->apex_position > secondReg->regSpan.endIdx)
                    continue;

                if (secondReg->apex_position < activeReg->regSpan.startIdx)
                    continue;

                if (secondReg->apex_position > activeReg->regSpan.endIdx)
                    continue;

                // if ( // check for the overlap of the peaks
                //     (
                //         secondReg->apex_position > activeReg->left_limit &&   // ref peak matches the left limit
                //         secondReg->apex_position < activeReg->right_limit) || // ref peak matches the right limit
                //     (
                //         activeReg->apex_position > secondReg.left_limit && // new peak matches the left limit
                //         activeReg->apex_position < secondReg.right_limit)) // new peak matches the right limit

                if (exponentialMSE[j] == 0.0)
                { // calculate the mse of the reference peak
                    exponentialMSE[j] = calcSSE_exp(
                        &secondReg->coeffs,
                        intensities,
                        &secondReg->regSpan);
                    exponentialMSE[j] /= secondReg->df;
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
                exponentialMSE[i] = calcSSE_exp(
                    &activeReg->coeffs,
                    intensities,
                    &activeReg->regSpan);
                exponentialMSE[i] /= activeReg->df;
            }
            if (exponentialMSE[i] < MSE_group)
            {
                // Set isValid to false for the candidates from the group
                for (size_t it_ref_peak : validRegressionsInGroup)
                {
                    (*validRegressions)[it_ref_peak].isValid = false;
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
        validRegressions->shrink_to_fit();
    }
#pragma endregion "validate regression"

#pragma region "create peaks"

    MeanVar weightedMeanAndVariance_EIC(const std::vector<float> *weight,
                                        const std::vector<float> *values,
                                        const Range_i regSpan)
    /* ### allocations ###
        none!

       ### called functions ###
        std::sqrt
    */
    {
        // weighted mean using intensity as weighting factor and left_limit right_limit as range
        size_t realPoints = 0;
        double mean_weights = 0;
        double sum_weighted_x = 0; // sum of values * weight
        double sum_weight = 0;
        for (size_t j = regSpan.startIdx; j <= regSpan.endIdx; j++)
        {
            int interpolated = (*values)[j] == 0 ? 0 : 1; // this is used instead of a continue so this can be vectorised. Skips a loop if value was interpolated
            mean_weights += (*weight)[j] * interpolated;
            sum_weighted_x += (*values)[j] * (*weight)[j] * interpolated;
            sum_weight += (*weight)[j] * interpolated;
            realPoints += 1 * interpolated; // interpolated points do not count!
        }
        mean_weights /= realPoints;
        sum_weighted_x /= mean_weights;
        sum_weight /= mean_weights;

        double weighted_mean = sum_weighted_x / sum_weight;
        double sum_Qxxw = 0.0; // sum of (values - mean)^2 * weight
        for (size_t j = regSpan.startIdx; j <= regSpan.endIdx; j++)
        {
            double difference = (*values)[j] - weighted_mean;
            double interpolated = (*values)[j] == 0 ? 0 : 1; // see above, add 0 if value is not real
            sum_Qxxw += interpolated * difference * difference * (*weight)[j];
        }
        float uncertaintiy = std::sqrt(sum_Qxxw / sum_weight / realPoints);
        return {float(weighted_mean), float(uncertaintiy)};
    };

    void createCentroidPeaks(
        std::vector<CentroidPeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const ProfileBlock *block, // @todo replace with mz and trace range
        const size_t scanNumber,
        const size_t accessor)
    /* ### allocations ###
        none, if peaks has the correct number of elements reserved

       ### called functions ###
        exp_approx_d
        erf_approx_f
        std::floor
    */
    {
        assert(!validRegressionsVec->empty());
        // iterate over the validRegressions vector
        for (size_t i = 0; i < validRegressionsVec->size(); i++)
        {
            const RegressionGauss *regression = validRegressionsVec->data() + i;
            assert(regression->isValid);
            CentroidPeak peak;
            peak.number_MS1 = scanNumber;
            // add height
            RegCoeffs coeff = regression->coeffs;
            peak.height = exp_approx_d(coeff.b0 + (regression->apex_position - coeff.x0) * coeff.b1 * 0.5);
            // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.heightUncertainty = regression->uncertainty_height * peak.height;

            size_t offset = (size_t)std::floor(regression->apex_position);
            double mz0 = block->mz[offset];
            double delta_mz = block->mz[offset + 1] - mz0;

            // add scaled area
            double exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression->area * exp_b0 * delta_mz;
            peak.areaUncertainty = regression->uncertainty_area * exp_b0 * delta_mz;

            // add scaled apex position (mz)
            peak.mz = mz0 + delta_mz * (regression->apex_position - std::floor(regression->apex_position));
            peak.mzUncertainty = regression->uncertainty_pos * delta_mz * T_VALUES[regression->df + 1] * sqrt(1 + 1 / (regression->df + 4));

            // quality params
            peak.DQSC = 1 - erf_approx_f(regression->uncertainty_area / regression->area);
            peak.df = regression->df;

            peak.numCompetitors = regression->numCompetitors;
            peak.scale = regression->coeffs.scale;

            // traceability information
            peak.trace.access = accessor;
            peak.trace.start = block->startPos;
            peak.trace.end = block->endPos;

            peaks->push_back(std::move(peak));
        }
    }

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const RT_Converter *convertRT,
        const std::vector<float> *RTs) // @todo this should be handled correctly through the converter
    /* ### allocations ###
        none!

       ### called functions ###
        floor
        erf_approx_d
        erf_approx_f
    */
    {
        assert(!validRegressionsVec->empty());
        assert(peaks->empty());

        for (size_t i = 0; i < validRegressionsVec->size(); i++)
        {
            const RegressionGauss *regression = validRegressionsVec->data() + i;
            assert(regression->isValid);
            size_t maxIdx = convertRT->groups.back().interpolatedIndex;
            assert(regression->regSpan.endIdx <= maxIdx);

            FeaturePeak peak;
            // add height
            RegCoeffs coeff = regression->coeffs;
            // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.height = exp_approx_d(coeff.b0 + (regression->apex_position - coeff.x0) * coeff.b1 * 0.5);
            peak.heightUncertainty = regression->uncertainty_height * peak.height;

            // calculate the apex position in RT
            size_t idx_leftOfApex = (size_t)regression->apex_position;
            size_t idx_leftOfApex_absolute = convertRT->groups[idx_leftOfApex].interpolatedIndex;

            assert(idx_leftOfApex != 0); // at least two points to the left if apex is > 1
            size_t idx_rightOfApex_absolute = idx_leftOfApex_absolute + 1;

            if (idx_rightOfApex_absolute > convertRT->groups.size() - 1)
            {
                continue;
            }

            assert(idx_rightOfApex_absolute < convertRT->groups.size() - 1); // at least two points to the right
            // float rt_leftOfApex = convertRT->groups[idx_leftOfApex_absolute].trueRT;
            float rt_leftOfApex_true = RTs->at(idx_leftOfApex);
            // float rt_rightOfApex = convertRT->groups[idx_rightOfApex_absolute].trueRT;
            float rt_rightOfApex_true = RTs->at(idx_leftOfApex + 1);
            assert(rt_leftOfApex_true < rt_rightOfApex_true);
            float delta_rt = rt_rightOfApex_true - rt_leftOfApex_true;
            float rt_fraction = (regression->apex_position - floor(regression->apex_position));
            assert(rt_fraction >= 0);
            assert(rt_fraction < 1);
            float rt_apex = rt_leftOfApex_true + delta_rt * rt_fraction;
            peak.retentionTime = rt_apex;
            peak.RT_Uncertainty = regression->uncertainty_pos * delta_rt;

            // add area
            float exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression->area * exp_b0 * delta_rt;
            peak.areaUncertainty = regression->uncertainty_area * exp_b0 * delta_rt;

            // mz, mzUncertainty, mean DQSC and meanDQSF are all calculated in after this function is called in measurement_data
            peak.DQSF = 1 - erf_approx_f(regression->uncertainty_area / regression->area);

            assert(regression->regSpan.endIdx - coeff.x0 > 1);
            peak.idxPeakStart = regression->regSpan.startIdx;
            peak.idxPeakEnd = regression->regSpan.endIdx;
            peak.idxCenter_offset = coeff.x0 - regression->regSpan.startIdx;
            assert(peak.idxPeakEnd > peak.idxPeakStart);
            assert(peak.idxPeakEnd > peak.idxCenter_offset);
            assert(peak.idxPeakEnd - peak.idxPeakStart >= 4); // at least five points

            // params needed to merge two peaks
            bool apexLeft = regression->apex_position < coeff.x0;
            assert(apexLeft == (coeff.b1 <= 0));

            coeff.b1 /= delta_rt;
            coeff.b2 /= delta_rt * delta_rt;
            coeff.b3 /= delta_rt * delta_rt;
            peak.coefficients = coeff;
            peak.mse_base = regression->mse;

            peak.scale = regression->coeffs.scale;
            peak.interpolationCount = rangeLen(&regression->regSpan) - regression->df - 4; // -4 since the degrees of freedom are reduced by 1 per coefficient
            peak.competitorCount = regression->numCompetitors;

            peaks->push_back(std::move(peak));
        }
    }
#pragma endregion "create peaks"

#pragma region calcSSE

    double calcRSS_reg(const RegressionGauss *mutateReg, const std::vector<float> *y_start)
    // no allocations
    {
        const size_t idxCenter = mutateReg->coeffs.x0;
        double RSS = 0.0;
        for (size_t idx = mutateReg->regSpan.startIdx; idx < mutateReg->regSpan.endIdx + 1; idx++)
        {
            double new_x = double(idx) - double(idxCenter);
            double y_predict = regAt(&mutateReg->coeffs, new_x);
            double difference = y_start->at(idx) - y_predict;

            RSS += difference * difference;
        }
        return RSS;
    }

    double calcRSS_H0_cf1(const std::vector<float> *observed, const Range_i *range)
    {
        // this function calculates the RSS for H0: y = b0 (a constant value)

        assert(range->endIdx < observed->size());

        double mean = 0;
        for (size_t i = range->startIdx; i <= range->endIdx; i++)
        {
            mean += (*observed)[i];
        }
        mean /= rangeLen(range);

        double RSS = 0;
        for (size_t i = range->startIdx; i <= range->endIdx; i++)
        {
            double difference = (*observed)[i] - mean;
            RSS += difference * difference;
        }

        return RSS;
    }

    double calcRSS_H0_cf2(const std::vector<float> *observed, const Range_i *range)
    {
        // this function calculates the RSS for H0: y = b0 + x * b1 (no weights)

        assert(range->endIdx < observed->size());

        double slope, intercept;
        size_t length = rangeLen(range);
        const float *start = observed->data() + range->startIdx;
        linReg_intx(start, length, &slope, &intercept);

        double RSS = 0;
        size_t x = 0;
        for (size_t i = range->startIdx; i <= range->endIdx; i++)
        {
            double difference = (*observed)[i] - (intercept - slope * x);
            RSS += difference * difference;
            x += 1;
        }
        assert(x == length);

        return RSS;
    }

    bool f_testRegression(const std::vector<float> *observed, double RSS_reg, const Range_i *range)
    {
        // during the tests, the RSS for the regression has already been calculated in calcRSS_reg
        assert(RSS_reg > 0);
        const size_t length = rangeLen(range);
        double RSS_H0;
        bool f_ok;

        RSS_H0 = calcRSS_H0_cf1(observed, range); // y = b
        f_ok = F_test_regs(RSS_reg, RSS_H0, 4, 1, length, 0.05);
        if (!f_ok)
            return false;

        RSS_H0 = calcRSS_H0_cf2(observed, range); // y = b0 + b1 * x
        f_ok = F_test_regs(RSS_reg, RSS_H0, 4, 1, length, 0.05);
        if (!f_ok)
            return false; // reject H0, significant difference from

        // no alternatives were accepted
        return true;
    }

    double calcSSE_exp(const RegCoeffs *coeff, const std::vector<float> *y_start, const Range_i *regSpan)
    { // @todo this does not account for asymmetric RT distances, will that be a problem?
        double idxCenter = double(coeff->x0);
        double result = 0.0;
        for (size_t iSegment = regSpan->startIdx; iSegment < regSpan->endIdx + 1; iSegment++)
        {
            double new_x = double(iSegment) - idxCenter;
            double y_predict = regExpAt(coeff, new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff;
        }
        return result;
    }

    double calcSSE_chisqared(const RegressionGauss *mutateReg, const std::vector<float> *y_start)
    {
        double result = 0.0;
        const double idxCenter = mutateReg->coeffs.x0;
        for (size_t iSegment = mutateReg->regSpan.startIdx; iSegment < mutateReg->regSpan.endIdx + 1; iSegment++)
        {
            double new_x = double(iSegment) - idxCenter;
            double y_predict = regExpAt(&mutateReg->coeffs, new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff / y_predict; // this part is different from the above function, do not try to merge them!
        }
        return result;
    }

#pragma endregion calcSSE

#pragma region "smearing correction"

    /// @brief Updates the intercept (b0) for log-linear regression models after back-transformation,
    ///        by calculating the optimal scaling correction. Returns the new b0 (to be used in exp(new_b0 + ...)).
    ///        This replaces the old b0 in your model for unbiased prediction after exp-transformation.
    /// @param yLogHatInWindow Vector of predicted log(y) (including b0!).
    /// @param yInWindow       Vector of original (not log-transformed) y values.
    /// @param b0              The old intercept (from log regression).
    /// @return                The updated b0 (corrected).

    float updateB0Scaling( // @todo how and why does this work?
        const std::vector<float> &yLogHatInWindow,
        const std::vector<float> &yInWindow,
        const float b0)
    {
        assert(yLogHatInWindow.size() == yInWindow.size());

        size_t n = yInWindow.size();

        double sum_y_yhat = 0.0f;
        double sum_yhat2 = 0.0f;

        // Use yLogHat[i] - b0 to get the prediction without intercept
        for (size_t i = 0; i < n; ++i)
        {
            double yhat_wo_b0 = exp_approx_d(yLogHatInWindow[i] - b0);
            sum_y_yhat += yInWindow[i] * yhat_wo_b0;
            sum_yhat2 += yhat_wo_b0 * yhat_wo_b0;
        }
        return std::log(sum_y_yhat / sum_yhat2);
    }

    // @todo implement this
    CorrectionFactors smearingCorrection(
        const std::vector<float> *predictLog,
        const std::vector<float> *selectLog)
    /* ### allocations ###
        none!

       ### called functions ###
        exp_approx_d
        std::log
    */
    {
        assert(predictLog->size() == selectLog->size());
        // This function calculates the smearing correction factor logC and the
        // corresponding variance var(logC) which will be used to correct the
        // beta_0 coefficient of the regression. The idea is to correct the
        // underestimation bias from the log transformation when reconstructing the
        // original data. The smearing correction is based on the following formula:
        // y = exp(log(y)) * C = exp(log(y) + logC) = exp(b0 + logC + b1 * x + ...)
        // where C is the smearing correction factor and logC is the log of C. The
        // new beta_0 is then calculated as b0 + logC. The variance of logC is also
        // calculated to account for the uncertainty in the smearing correction.
        // For Var(logC), we consider:
        // Var(logC) = Var(C) / C^2
        //
        // reference:
        // Duan, N. (1983). Smearing Estimate: A Nonparametric Retransformation Method.
        // Journal of the American Statistical Association, 78(383), 605–610.
        // https://doi.org/10.1080/01621459.1983.10478017

        size_t n = predictLog->size();

        double sumR = 0.0;  // sum of exp(ε̂_i), i.e., exponential of the residuals
        double sumR2 = 0.0; // sum of (exp(ε̂_i))^2, i.e., square of the exponential of the residuals
        // loop to calculate sum of exp(ε̂_i) and sum of (exp(ε̂_i))^2
        for (size_t i = 0; i < n; ++i)
        {
            // ri = exp(ε̂_i) = exp(log y_i - log ŷ_i) residuals
            double ri = exp_approx_d(selectLog->at(i) - predictLog->at(i));
            sumR += ri;
            sumR2 += ri * ri;
        }

        // calculate the smearing correction factor C and its variance varC
        double C = sumR / double(n);                                   // C = mean(exp(ε̂_i)) = mean(r_i)
        double s2 = (sumR2 - sumR * sumR / double(n)) / double(n - 1); // s² = Var(r_i)
        double varC = s2 / double(n);                                  // Var(C) = s²/n
        double varLogC = varC / (C * C);                               // Var(log C) via Delta-Method

        return {std::log(C), varLogC};
    }
#pragma endregion "smearing correction"

    RegPair findBestRegression(
        const std::vector<float> *intensities,
        const std::vector<RegressionGauss> *regressions,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const Range_i regSpan)
    {
        double best_mse = INFINITY;
        unsigned int bestRegIdx = 0;

        // identify left (smallest) and right (largest) limit of the grouped regression windows
        size_t left_limit = -1;
        size_t right_limit = 0;
        for (size_t i = regSpan.startIdx; i < regSpan.endIdx + 1; i++)
        {

            const RegressionGauss *reg = &(*regressions)[i];
            left_limit = min(left_limit, reg->regSpan.startIdx);
            right_limit = max(right_limit, reg->regSpan.endIdx);
        }
        // the new df_sum is only needed since the function limits are adjusted above, correct that?
        size_t df_sum = calcDF_cum(degreesOfFreedom_cum, regSpan);

        for (size_t i = regSpan.startIdx; i < regSpan.endIdx + 1; i++)
        {
            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            const RegressionGauss *reg = &(*regressions)[i];
            const Range_i range = {left_limit, right_limit};
            double mse = calcSSE_exp(&reg->coeffs,
                                     intensities,
                                     &range);
            mse /= (df_sum - 4);
            // assert(mse == reg->mse); // @todo harmonise this eventually

            if (mse < best_mse)
            {
                best_mse = mse;
                bestRegIdx = i;
            }
        }
        return {bestRegIdx, float(best_mse)};
    }

    inline size_t calcDF_cum(
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const Range_i regSpan)
    {
        assert(regSpan.endIdx < degreesOfFreedom_cum->size());
        unsigned int substract = regSpan.startIdx == 0 ? 0 : degreesOfFreedom_cum->at(regSpan.startIdx - 1);
        size_t df = degreesOfFreedom_cum->at(regSpan.endIdx) - substract;
        return df;
    }

    int calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        double *valley_position)
    {
        const bool valley_left = mutateReg->coeffs.b2 >= 0;
        const bool valley_right = mutateReg->coeffs.b3 >= 0;
        const bool apexLeft = mutateReg->coeffs.b1 < 0;

        // the apex cannot be on the same side as the valley point
        if (valley_left && apexLeft)
            return 1;
        if (valley_right && (!apexLeft))
            return 1;

        // position maximum / minimum of b2 or b3. This is just the frst derivative of the peak half equation (b0 + b1 x + b23 x^2)
        double position_b2 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b2);
        double position_b3 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b3);

        // the apex must be at least minscale different from the valley, if it exists
        if (valley_left || valley_right)
        {
            if ((position_b3 - position_b2) < GLOBAL_MINSCALE)
                return 2;

            *valley_position = apexLeft ? position_b3 : position_b2;
        }
        mutateReg->apex_position = apexLeft ? position_b2 : position_b3;

        // if this difference from 0 (rounded down) is exceeded by the apex position,
        // there are not enough points to validate the peak half left
        // example: at a scale of 2, the apex position must always be smaller than 1
        double maxApexDist = double(mutateReg->coeffs.scale - GLOBAL_MINSCALE + 1);
        if (abs(mutateReg->apex_position) > maxApexDist)
            return 3;

        return 0;
    }

    double apexToEdgeRatio(const RegressionGauss *mutateReg, const std::vector<float> *intensities)
    {
        // is the apex at least twice as large as the outermost point?
        // assumption: outermost point is already near base level
        const size_t idxApex = size_t(mutateReg->apex_position) + mutateReg->coeffs.x0;
        assert(idxApex < intensities->size());

        double left = intensities->at(mutateReg->regSpan.startIdx);
        double right = intensities->at(mutateReg->regSpan.endIdx);
        double minIntensity = min(left, right);
        double apex = intensities->at(idxApex);
        return apex / minIntensity;
    }

    bool isValidQuadraticTerm(const RegressionGauss *mutateReg, const size_t df_sum)
    {
        assert(mutateReg->mse > 0);
        // inverseMatrix_2_2 is at position 4 of initialize()
        size_t access = mutateReg->coeffs.scale * 6 + 4;
        double divisor = std::sqrt(INV_ARRAY[access] * mutateReg->mse);
        double abs2 = std::abs(mutateReg->coeffs.b2);
        double abs3 = std::abs(mutateReg->coeffs.b3);
        double tValue = abs2 > abs3 ? abs2 : abs3;
        return tValue > T_VALUES[df_sum - 5] * divisor; // statistical significance of the quadratic term
        // note that the tvalue would have to be divided by the divisor, but this is not always compiled to a multiplication
    }

    void calcPeakHeightUncert(RegressionGauss *mutateReg)
    {
        double Jacobian_height[4]{1, 0, 0, 0};         // Jacobian matrix for the height
        Jacobian_height[1] = mutateReg->apex_position; // apex_position * height;
        if (mutateReg->apex_position < 0)
        {
            Jacobian_height[2] = mutateReg->apex_position * mutateReg->apex_position; // apex_position * Jacobian_height[1];
            // Jacobian_height[3] = 0;
        }
        else
        {
            // Jacobian_height[2] = 0;
            Jacobian_height[3] = mutateReg->apex_position * mutateReg->apex_position; // apex_position * Jacobian_height[1];
        }
        // at this point without height, i.e., to get the real uncertainty
        // multiply with height later. This is done to avoid exp function at this point
        mutateReg->uncertainty_height = calcUncertainty(Jacobian_height, mutateReg->coeffs.scale, mutateReg->mse);
        return;
    }

    bool isValidPeakHeight(
        const RegressionGauss *mutateReg,
        const double valley_position,
        const size_t df_sum,
        const double apexToEdge)
    {
        // check if the peak height is significantly greater than edge signal @todo this should be solved by the f test
        double apex = mutateReg->apex_position;
        assert(apex != 0);
        const bool apexLeft = apex < 0;
        // determine left or right limit, based on apex position
        const double scale_d = double(mutateReg->coeffs.scale);
        const double altValley = scale_d * (apexLeft ? -1 : 1);
        const double valley = valley_position == 0 ? altValley : valley_position;
        // construct matrix
        const double j1 = apex - valley;
        const double j23 = apex * apex - valley * valley;
        const double j2 = apexLeft ? j23 : 0;
        const double j3 = apexLeft ? 0 : j23;
        const double jacobianHeight[4]{0, j1, j2, j3};

        float uncertainty_apexToEdge = calcUncertainty(jacobianHeight, mutateReg->coeffs.scale, mutateReg->mse);

        // @todo why -2? Just to account for position?
        bool peakHeightSignificant = (apexToEdge - 2) > T_VALUES[df_sum - 5] * (apexToEdge * uncertainty_apexToEdge);
        return peakHeightSignificant;
    }

    void calcPeakAreaUncert(RegressionGauss *mutateReg)
    {
        double b1 = mutateReg->coeffs.b1;
        double b2 = mutateReg->coeffs.b2;
        double b3 = mutateReg->coeffs.b3;
        double _SQRTB2 = 1 / std::sqrt(std::abs(b2));
        double _SQRTB3 = 1 / std::sqrt(std::abs(b3));
        double B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
        double B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
        double B1_2_B2 = b1 / 2 / b2;
        double B1_2_B3 = b1 / 2 / b3;

        // error calculation uses imaginary part if coefficient is > 0
        double err_L = (b2 < 0)
                           ? experfc(B1_2_SQRTB2, -1.0) // 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
                           : dawson5(B1_2_SQRTB2);      // erfi(b1 / 2 / SQRTB2);        // peak with valley point;

        double err_R = (b3 < 0)
                           ? experfc(B1_2_SQRTB3, 1.0) // 1 + std::erf(b1 / 2 / SQRTB3) // ordinary peak
                           : dawson5(-B1_2_SQRTB3);    // -erfi(b1 / 2 / SQRTB3);       // peak with valley point ;

        // calculate the Jacobian matrix terms
        double J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
        double J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
        double J_2_common_L = B1_2_B2 / b1;
        double J_2_common_R = B1_2_B3 / b1;
        double J_1_L = J_1_common_L * err_L;
        double J_1_R = J_1_common_R * err_R;
        double J_2_L = J_2_common_L - J_1_L * B1_2_B2;
        double J_2_R = -J_2_common_R - J_1_R * B1_2_B3;

        double J[4]; // Jacobian matrix
        J[0] = J_1_R + J_1_L;
        J[1] = J_2_R + J_2_L;
        J[2] = -B1_2_B2 * (J_2_L + J_1_L / b1);
        J[3] = -B1_2_B3 * (J_2_R + J_1_R / b1);

        // at this point the area is without exp(b0), i.e., to get the real area multiply with exp(b0) later. This is done to avoid exp function at this point
        mutateReg->area = J[0];
        mutateReg->uncertainty_area = calcUncertainty(J, mutateReg->coeffs.scale, mutateReg->mse);

        return;
    }

    bool isValidPeakArea(const RegressionGauss *mutateReg, const size_t df_sum)
    // no allocations
    {
        double doubleScale = double(mutateReg->coeffs.scale);
        double b1 = mutateReg->coeffs.b1;
        double b2 = mutateReg->coeffs.b2;
        double b3 = mutateReg->coeffs.b3;

        double _SQRTB2 = 1 / std::sqrt(std::abs(b2));
        double _SQRTB3 = 1 / std::sqrt(std::abs(b3));
        double B1_2_B2 = b1 / 2 / b2;
        double B1_2_B3 = b1 / 2 / b3;

        double err_L_covered = 0;
        double x_left = -doubleScale; // @todo we use the last point before the valley as limit later, keep this in?
        {
            double B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
            double EXP_B12 = exp_approx_d(-b1 * B1_2_B2 / 2);

            bool valley = b2 > 0;
            if (valley)
            {
                // valley point on the left side of the apex
                // error = erfi(b1 / 2 / SQRTB2)
                double err_L = dawson5(B1_2_SQRTB2);
                // check if the valley point is inside the window for the regression and consider it if necessary
                if (-B1_2_B2 < -doubleScale)
                {
                    // valley point is outside the window, use scale as limit
                    err_L_covered = err_L - erfi((b1 - 2 * b2 * doubleScale) / 2 * _SQRTB2) * EXP_B12;
                }
                else
                {
                    // valley point is inside the window, use valley point as limit
                    err_L_covered = err_L;
                    x_left = -B1_2_B2;
                }
            }
            else
            {
                // no valley point
                // error = 1 - std::erf(b1 / 2 / SQRTB2)
                double err_L = experfc(B1_2_SQRTB2, -1.0);
                // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
                // std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
                err_L_covered = erf_approx_f((b1 - 2 * b2 * doubleScale) / 2 * _SQRTB2) * EXP_B12 * SQRTPI_2 + err_L - SQRTPI_2 * EXP_B12;
            }
        }

        double err_R_covered = 0;
        double x_right = doubleScale; // right limit due to the window @todo this is already adjusted earlier
        {
            double B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
            double EXP_B13 = exp_approx_d(-b1 * B1_2_B3 / 2);

            bool valley = b3 > 0;
            if (valley)
            {
                // valley point is on the right side of the apex
                // error = - erfi(b1 / 2 / SQRTB3)
                double err_R = dawson5(-B1_2_SQRTB3);
                if (-B1_2_B3 > doubleScale)
                {
                    // valley point is outside the window, use scale as limit
                    err_R_covered = erfi((b1 + 2 * b3 * doubleScale) / 2 * _SQRTB3) * EXP_B13 + err_R;
                }
                else
                {
                    // valley point is inside the window, use valley point as limit
                    err_R_covered = err_R;
                    x_right = -B1_2_B3;
                }
            }
            else
            {
                // no valley point
                // error = 1 + std::erf(b1 / 2 / SQRTB3)
                double err_R = experfc(B1_2_SQRTB3, 1.0);
                // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
                // err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
                err_R_covered = err_R - SQRTPI_2 * EXP_B13 - erf_approx_f((b1 + 2 * b3 * doubleScale) / 2 * _SQRTB3) * SQRTPI_2 * EXP_B13;
            }
        }
        assert(x_left < x_right);

        double J_covered[4]; // Jacobian matrix for the covered peak area
        {
            // b0 not used on purpose, error is height independent
            const double y_left = exp_approx_d(b1 * x_left + b2 * x_left * x_left);
            const double y_right = exp_approx_d(b1 * x_right + b3 * x_right * x_right);
            const double dX = x_right - x_left;

            // calculate the Jacobian matrix terms
            const double J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
            const double J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
            const double J_2_common_L = B1_2_B2 / b1;
            const double J_2_common_R = B1_2_B3 / b1;

            // calculate the trapzoid correction terms for the jacobian matrix
            double trpzd_b0 = (y_right + y_left) * dX / 2;
            double trpzd_b1 = (x_right * y_right + x_left * y_left) * dX / 2;
            double trpzd_b2 = (x_left * x_left * y_left) * dX / 2;
            double trpzd_b3 = (x_right * x_right * y_right) * dX / 2;

            const double J_1_L_covered = J_1_common_L * err_L_covered;
            const double J_1_R_covered = J_1_common_R * err_R_covered;
            const double J_2_L_covered = J_2_common_L - J_1_L_covered * B1_2_B2;
            const double J_2_R_covered = -J_2_common_R - J_1_R_covered * B1_2_B3;

            J_covered[0] = J_1_R_covered + J_1_L_covered - trpzd_b0;
            J_covered[1] = J_2_R_covered + J_2_L_covered - trpzd_b1;
            J_covered[2] = -B1_2_B2 * (J_2_L_covered + J_1_L_covered / b1) - trpzd_b2;
            J_covered[3] = -B1_2_B3 * (J_2_R_covered + J_1_R_covered / b1) - trpzd_b3;
        }
        if (J_covered[0] < 0)
            return false;

        float area_uncertainty_covered = calcUncertainty(J_covered, mutateReg->coeffs.scale, mutateReg->mse);

        // J[0] / uncertainty > Tval
        bool J_is_significant = J_covered[0] > T_VALUES[df_sum - 5] * area_uncertainty_covered;

        return J_is_significant;
    }
#pragma endregion isValidPeakArea

    void calcUncertaintyPos(RegressionGauss *mutateReg)
    {
        double _b1 = 1 / mutateReg->coeffs.b1;
        double _b2 = 1 / mutateReg->coeffs.b2;
        double _b3 = 1 / mutateReg->coeffs.b3;
        double J[4]; // Jacobian matrix
        J[0] = 0.f;
        J[1] = mutateReg->apex_position * _b1;
        if (mutateReg->apex_position < 0)
        {
            J[2] = -mutateReg->apex_position * _b2;
            J[3] = 0;
        }
        else
        {
            J[2] = 0;
            J[3] = -mutateReg->apex_position * _b3;
        }

        mutateReg->uncertainty_pos = calcUncertainty(J, mutateReg->coeffs.scale, mutateReg->mse);
        return;
    }

#pragma region "convolve regression"

    double calcUncertainty(const double J[4], const size_t scale, const double mse)
    {
        assert(mse > 0);
        // Calculate the Matrix Product of J * Xinv * J^T for uncertainty calculation
        const double *inv = INV_ARRAY.data() + scale * 6;
        double vecMatrxTranspose = J[0] * J[0] * inv[0] +
                                   J[1] * J[1] * inv[2] +
                                   (J[2] * J[2] + J[3] * J[3]) * inv[4] +
                                   2 * (J[2] * J[3] * inv[5] +
                                        J[0] * (J[1] + J[3]) * inv[1] +
                                        J[1] * (J[2] - J[3]) * inv[3]);
        double uncertainty = std::sqrt(mse * vecMatrxTranspose);
        return uncertainty;
    }
#pragma endregion "convolve regression"

#pragma region "Feature Detection"

    double medianVec(const std::vector<float> *vec) // @todo this should be the mode, not the median
    {
        std::vector<float> tmpDiffs = *vec;
        std::sort(tmpDiffs.begin(), tmpDiffs.end());

        size_t size = tmpDiffs.size();
        assert(size > 0);
        if (size % 2 == 0)
        {
            return (tmpDiffs[size / 2 - 1] + tmpDiffs[size / 2]) / 2;
        }
        else
        {
            return tmpDiffs[size / 2];
        }
    }

    RT_Converter interpolateScanNumbers(const std::vector<float> *retentionTimes)
    /* ### allocations ###
        diffs: known at time of function call
        forwardConv: s.o.
        backwardConv: size calculated in function

       ### called functions ###
        medianVec allocates a vector the size of diffs
    */
    {
        // This function interpolates the existing RTs of MS1 spectra and produces a vector that contains,
        // for the index of every real MS1 spectrum, the scan number with interpolations for that spectrum.
        // Since we work with an integer scale, interpolations are rounded at 0.5.
        // It is assumed that the RT vector is sorted.

        assert(!retentionTimes->empty());
        std::vector<float> diffs(retentionTimes->size() - 1, 0);
        for (size_t i = 0; i < diffs.size(); i++)
        {
            diffs[i] = retentionTimes->at(i + 1) - retentionTimes->at(i);
        }
        assert(!diffs.empty());

        float expectedDiff = medianVec(&diffs);
        // if this is not given, there are severe distortions at some point in the data. We accept one non-compliance,
        // at the first scan in the spectrum only, which is generally one of the first recorded scans
        // incorrectness here could be due to a vendor-specific instrument checkup procedure, which has been observed once at least
        // assert(tmpDiffs[1] > expectedDiff / 2);

        size_t scanCount = retentionTimes->size();
        std::vector<RT_Grouping> totalRTs;
        totalRTs.reserve(scanCount * 2);
        std::vector<size_t> idxToGrouping(scanCount, -1);

        float critDiff = expectedDiff * 1.5; // if the difference is 1.5 times greater than the critDiff, there should be at least one interpolation

        for (size_t i = 0; i < diffs.size(); i++)
        {
            size_t interpScan = totalRTs.size();
            totalRTs.push_back({i, interpScan, (*retentionTimes)[i], false});
            idxToGrouping[i] = interpScan;

            if (diffs[i] >= critDiff)
            {
                // interpolate at least one point
                size_t numInterpolations = size_t(diffs[i] / expectedDiff + 0.5 - FLT_EPSILON); // + 0.5 since value is truncated (round up)
                assert(numInterpolations > 1);

                float RTstep = diffs[i] / (numInterpolations);
                for (size_t j = 1; j < numInterpolations; j++) // +1 since the span is between two points
                {
                    interpScan = totalRTs.size();
                    float newRT = (*retentionTimes)[i] + RTstep * j;
                    totalRTs.push_back({size_t(-1), interpScan, newRT, true});
                }
            }
        }
        // add in the last remaining point
        size_t lastScan = totalRTs.size();
        idxToGrouping.back() = lastScan;
        totalRTs.push_back({diffs.size(), lastScan, retentionTimes->back(), false});

        return {totalRTs, idxToGrouping};
    }

    void fillPeakVals(EIC *eic, FeaturePeak *currentPeak)
    {
        currentPeak->scanPeakStart = eic->scanNumbers.front();
        currentPeak->scanPeakEnd = eic->scanNumbers.back();

        // the correct limits in the non-interpolated EIC need to be determined. They are already included
        // in the cumulative degrees of freedom, but since there, df 0 is outside the EIC, we need to
        // use the index df[limit] - 1 into the original, non-interpolated vector

        unsigned int limit_L = eic->df[currentPeak->idxPeakStart];
        limit_L = std::min(limit_L, limit_L - 1); // uint underflows, so no issues.
        unsigned int limit_R = eic->df[currentPeak->idxPeakEnd] - 1;
        assert(limit_L < limit_R);

        currentPeak->idxBinStart = limit_L;
        currentPeak->idxBinEnd = limit_R;

        Range_i regSpan = {limit_L, limit_R};

        auto tmp = weightedMeanAndVariance_EIC(&eic->ints_area, &eic->mz, regSpan);
        currentPeak->mz = tmp.mean;
        currentPeak->mzUncertainty = tmp.var;
        currentPeak->DQSC = weightedMeanAndVariance_EIC(&eic->ints_area, &eic->DQSC, regSpan)
                                .mean;
        currentPeak->DQSB = weightedMeanAndVariance_EIC(&eic->ints_area, &eic->DQSB, regSpan)
                                .mean;
    }

    std::vector<FeaturePeak> findFeatures(std::vector<EIC> &EICs,
                                          const RT_Converter *convertRT)
    /* ### allocations ###
        peaks
        tmpPeaks
        validRegressions

       ### called functions ###

    */
    {
        // @todo this is not a universal limit and only chosen for computational speed at the moment
        // with an estimated scan difference of 0.6 s this means the maximum peak width is 61 * 0.6 = 36.6 s
        static const size_t GLOBAL_MAXSCALE_FEATURES = 30;
        assert(GLOBAL_MAXSCALE_FEATURES <= MAXSCALE);

        std::vector<FeaturePeak> peaks;    // return vector for feature list
        peaks.reserve(EICs.size() / 4);    // should be enough to fit all features without reallocation
        std::vector<FeaturePeak> tmpPeaks; // add features to this before pasting into FL

        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(512);

        std::vector<float> logIntensity;

        for (size_t i = 0; i < EICs.size(); ++i)
        {
            auto currentEIC = EICs[i];
            if (currentEIC.scanNumbers.size() < 5)
            {
                continue; // skip due to lack of data, i.e., degrees of freedom will be zero
            }

            validRegressions.clear();
            size_t length = currentEIC.df.size();
            assert(length > 4); // data must contain at least five points

            logIntensity.resize(length);
            logIntensity.clear();

            size_t maxScale = std::min(GLOBAL_MAXSCALE_FEATURES, (length - 1) / 2);

            runningRegression(&currentEIC.ints_area,
                              &logIntensity,
                              &currentEIC.df,
                              &validRegressions,
                              maxScale);

            if (!validRegressions.empty())
            {
                createFeaturePeaks(&tmpPeaks, &validRegressions, convertRT, &currentEIC.RT);

                for (auto peak : tmpPeaks)
                {
                    assert(peak.retentionTime > currentEIC.RT.front());
                    assert(peak.retentionTime < currentEIC.RT.back());
                }
            }
            // @todo extract the peak construction here and possibly extract findFeatures into a generic function

            if (tmpPeaks.empty())
            {
                continue;
            }
            for (size_t j = 0; j < tmpPeaks.size(); j++)
            {
                FeaturePeak currentPeak = tmpPeaks[j];

                // the correct limits in the non-interpolated EIC need to be determined. They are already included
                // in the cumulative degrees of freedom, but since there, df 0 is outside the EIC, we need to
                // use the index df[limit] - 1 into the original, non-interpolated vector

                currentPeak.idxBin = i;

                fillPeakVals(&currentEIC, &currentPeak);
                assert(currentPeak.scanPeakEnd < convertRT->groups.size());

                peaks.push_back(currentPeak);
            }

            tmpPeaks.clear();
        }
        // peaks are sorted here so they can be treated as const throughout the rest of the program
        std::sort(peaks.begin(), peaks.end(), [](FeaturePeak lhs, FeaturePeak rhs)
                  { return lhs.retentionTime < rhs.retentionTime; });
        return peaks;
    }

#pragma endregion "Feature Detection"

#pragma region "find centroids"

    inline float calcRTDiff(const std::vector<double> *retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times->size(); ++i)
        {
            sum += retention_times->at(i) - retention_times->at(i - 1);
        }
        return sum / (retention_times->size() - 1);
    }

    int findCentroids(XML_File &data, // @todo get rid of the direct coupling to pugixml
                      const std::vector<unsigned int> *selectedIndices,
                      std::vector<CentroidPeak> *centroids)
    /* ### allocations ###
        spectrum_mz: size unknown at time of function call
        spectrum_int: same as spectrum_mz
        groupedData: size unknown

       ### called functions ###
        data.get_spectrum
        pretreatDataCentroids
        findCentroidPeaks

    */
    {
        const size_t countSelected = selectedIndices->size();

        std::vector<double> spectrum_mz(1000);
        std::vector<double> spectrum_int(1000);

        std::vector<ProfileBlock> groupedData(500);
        for (size_t i = 0; i < countSelected; ++i)
        {
            // avoid needless allocation / deallocation of otherwise scope-local vectors
            spectrum_mz.clear();
            spectrum_int.clear();
            groupedData.clear();

            // i = 0;

            size_t ID_spectrum = selectedIndices->at(i);
            data.get_spectrum(&spectrum_mz, &spectrum_int, ID_spectrum);

#ifdef _RM
            // ### this is for development only, highly inefficient at scale! ###
            hardFilter(&spectrum_mz, &spectrum_int, 247.1, 247.3);
            if (spectrum_mz.empty())
                continue;

            volatile bool printThis = false;
            if (printThis)
            {
                FILE *f = fopen("./spectrum1.csv", "w");
                fprintf(f, "mz,int\n");
                for (size_t j = 0; j < spectrum_mz.size(); j++)
                {
                    fprintf(f, "%f,%.1f\n", spectrum_mz[j], spectrum_int[j]);
                }
            }

            volatile bool debug = false;
            if (debug)
                centroids->clear();
#endif
            // @todo every group is just an index range into three same-length vectors if we do not
            // perform interpolation. The "ProfileBlock" struct is unnecessary.
            size_t maxWindowSize = pretreatDataCentroids(&groupedData, &spectrum_mz, &spectrum_int);
            if (maxWindowSize == 0) // this is also relevant to filtering, add a warning if no filter?
                continue;

            findCentroidPeaks(centroids, &groupedData, i, ID_spectrum, maxWindowSize);

            if (debug)
            {
                // write a file with the processed spectrum and the assumed intensity
                // profile of the finalised centroids
                std::vector<double> predictedInt(spectrum_mz.size(), 0);
                for (size_t db = 0; db < centroids->size(); db++)
                {
                    CentroidPeak cen = centroids->at(db);
                    size_t idxStart = cen.trace.start;
                    size_t idxEnd = cen.trace.end;
                    assert(predictedInt[idxStart] == 0);
                    assert(predictedInt[idxEnd] == 0);
                    // new requirement: Fuction that evaluates centroid on real mz values
                }
            }
        }
        for (unsigned int i = 0; i < centroids->size(); i++)
        {
            centroids->at(i).ID = i;
        }

        return centroids->size();
    }

    size_t pretreatDataCentroids(
        std::vector<ProfileBlock> *groupedData,
        const std::vector<double> *spectrum_mz,
        const std::vector<double> *spectrum_int)
    {
        assert(groupedData->empty());
        assert(spectrum_int->size() == spectrum_mz->size());
        // this function walks through both spectra and fills a vector of blocks
        // every block is a region which is searched for centroids. Further, the
        // edges of the block are interpolated such that the third real point (which
        // is the start of valid regressions) can be occupied with a symmetric regression
        // of the maximum scale (currently 8)
        // the function returns the largest created block.

        const float minIntensity = 10; // @todo bad solution?

        std::vector<Range_i> ranges;
        ranges.reserve(64);
        size_t rangeStart = 0;
        size_t maxRangeSpan = 0;

        // fill the vec of ranges by iterating over mz and rt. all ranges are >5 points
        for (size_t pos = 0; pos < spectrum_int->size(); pos++)
        {
            if (spectrum_int->at(pos) < minIntensity) // this should generally be the same as == 0
            {
                size_t span = pos - rangeStart;
                if (span >= 5)
                {
                    // at least five points, add block
                    // pos - 1 is used since pos is 0
                    ranges.push_back({rangeStart, pos - 1});
                    assert(rangeLen(&ranges.back()) > 4);
                    maxRangeSpan = maxRangeSpan > span ? maxRangeSpan : span;
                }
                rangeStart = pos + 1;
            }
        }

        { // add last block if it is still open and correct
            size_t span = spectrum_int->size() - rangeStart;
            if (span >= 5)
            {
                ranges.push_back({rangeStart, spectrum_int->size() - 1});
                maxRangeSpan = maxRangeSpan > span ? maxRangeSpan : span;
            }
        }

        assert(!ranges.empty());
        assert(maxRangeSpan > 4);
        groupedData->reserve(ranges.size());

        const static size_t maxExtra = GLOBAL_MAXSCALE_CENTROID - 2; // at least two real points per half
        maxRangeSpan += 2 * maxExtra;
        std::vector<float> insert_ms(maxRangeSpan, NAN);
        std::vector<float> insert_int(maxRangeSpan, NAN);
        std::vector<unsigned int> cumdf(maxRangeSpan, NAN);

        for (size_t i = 0; i < ranges.size(); i++)
        {
            insert_ms.clear();
            insert_int.clear();
            cumdf.clear();

            const size_t idxL = ranges[i].startIdx;
            const size_t idxR = ranges[i].endIdx;

            // copy real values
            size_t newdf = 0;
            for (size_t M = idxL; M <= idxR; M++)
            {
                insert_ms.push_back(spectrum_mz->at(M));
                insert_int.push_back(spectrum_int->at(M));
                newdf += 1;
                cumdf.push_back(newdf);
            }

            ProfileBlock b = {insert_int,
                              insert_ms,
                              cumdf,
                              idxL, idxR};
            groupedData->push_back(b);
        }
        return maxRangeSpan;
    }

    double regAt(const RegCoeffs *coeff, const double x)
    {
        double b23 = x < 0 ? coeff->b2 : coeff->b3;
        return coeff->b0 + (coeff->b1 + x * b23) * x;
    }

    double regExpAt(const RegCoeffs *coeff, const double x)
    {
        double b23 = x < 0 ? coeff->b2 : coeff->b3;
        return exp_approx_d(coeff->b0 + (coeff->b1 + x * b23) * x);
    }
}