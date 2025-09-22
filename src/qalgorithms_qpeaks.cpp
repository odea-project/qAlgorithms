#include "qalgorithms_qpeaks.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h" // @todo remove coupling

#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm> // sorting
#include <stdint.h>  // max numeric vals

namespace qAlgorithms
{

    constexpr auto INV_ARRAY = initialize(); // this only works with constexpr square roots, which are part of C++26
#define GLOBAL_MAXSCALE_CENTROID 8           // @todo this is a critical part of the algorithm and should not be hard-coded

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
        std::vector<RegCoeffs> regressions = findCoefficients_new(&y_log, maxScale);
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

        std::vector<float> logIntensity(maxWindowSize, NAN);

        assert(GLOBAL_MAXSCALE_CENTROID <= MAXSCALE);

        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(treatedData->size() / 2); // probably too large, shouldn't matter
        for (size_t i = 0; i < treatedData->size(); i++)   // 185
        {
            const auto block = &((*treatedData)[i]);
            const size_t length = block->mz.size();
            assert(length == block->mz.size());
            assert(length > 4);

            // @todo adjust the scale dynamically based on the number of valid regressions found, early terminate after x iterations
            const size_t maxScale = std::min(size_t(GLOBAL_MAXSCALE_CENTROID), (length - 1) / 2); // length - 1 because the center point is not part of the span

            logIntensity.clear(); // this is now filled inside the function, the vector only reserves space

            runningRegression(&block->intensity, &logIntensity, &block->cumdf, &validRegressions, maxScale, length - 2);
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

        // assert(retPeaks->size() != 0); // at least one centroid per mass spectrum @todo not applicable to filtered regions
    }

#pragma endregion "find peaks"

#pragma region "running regression"
    size_t globalCount = 0;

    void runningRegression(
        const std::vector<float> *intensities,
        std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        std::vector<RegressionGauss> *validRegressions,
        const size_t maxScale,
        const size_t maxApexIdx)
    /* ### allocations ###
        regressions: size determined by function

       ### called functions ###
        findCoefficients
        mergeRegressionsOverScales
    */
    {
        assert(intensities_log->empty());
        assert(intensities_log->capacity() >= intensities->size());

        for (size_t i = 0; i < intensities->size(); i++)
        {
            intensities_log->push_back(std::log(intensities->at(i)));
        }

        assert(validRegressions->empty());

        // coefficients for single-b0 peaks, spans all regressions over a peak window
        const std::vector<RegCoeffs> regressions = findCoefficients_new(intensities_log, maxScale);

        const size_t numPoints = intensities->size();
        // all entries in coeff are sorted by scale in ascending order - this is not checked!

        std::vector<RegressionGauss> validRegsTmp; // temporary vector to store valid regressions
        validRegsTmp.reserve(numPoints);
        // the start index of the regression is the same as the index in beta. The end index is at 2*scale + index in beta.
        validRegsTmp.push_back(RegressionGauss{});

        size_t scale = 2;
        size_t idxStart = 0;

        std::vector<int> failures;
        failures.reserve(regressions.size());
        std::vector<int> x0s;
        x0s.reserve(regressions.size());

        {
            // FILE *f = fopen("failedRegs.csv", "w");
            // if (f == NULL)
            // {
            //     printf("Error opening file!\n");
            //     exit(1);
            // }

            // fprintf(f, "ID, x0, b0, b1, b2, b3, fail\n");

            // fclose(f);
        }

        for (size_t range = 0; range < regressions.size(); range++)
        {
            validRegsTmp.back().coeffs = regressions[range];
            // the total span of the regression may not exceed the number of points
            assert(idxStart + 2 * scale < numPoints);

            int failpoint = makeValidRegression(degreesOfFreedom_cum,
                                                intensities,
                                                intensities_log,
                                                &validRegsTmp.back(),
                                                idxStart,
                                                scale,
                                                maxApexIdx);

            failures.push_back(failpoint);
            x0s.push_back(idxStart + scale);

            if (validRegsTmp.back().isValid)
            {
                validRegsTmp.push_back(RegressionGauss{});
            }

            idxStart++;
            // for every set of scales, execute the validation + in-scale merge operation
            // early termination needed if maxscale is reached, since here idxStart is 1 and the compared value 0
            if ((idxStart == numPoints - 2 * scale)) //|| (currentScale == maxScale))
            {
                // remove the last regression, since it is always empty
                validRegsTmp.pop_back();
                // no valid peaks if the size of validRegsTemp is 0
                if (validRegsTmp.size() == 1)
                {
                    // only one valid peak, no fitering necessary
                    validRegressions->push_back(std::move(validRegsTmp[0]));
                }
                else if (validRegsTmp.size() > 1)
                {
                    findBestScales(validRegressions, &validRegsTmp, intensities, degreesOfFreedom_cum);
                }

                // reset loop
                scale++;
                idxStart = 0;
                validRegsTmp.clear();
                validRegsTmp.push_back(RegressionGauss{});
            }
        }

        // there can be 0, 1 or more than one regressions in validRegressions
        mergeRegressionsOverScales(validRegressions, intensities);

        {
            // FILE *f = fopen("failedRegs.csv", "a");
            // if (f == NULL)
            // {
            //     printf("Error opening file!\n");
            //     exit(1);
            // }

            // for (size_t i = 0; i < regressions.size(); i++)
            // {
            //     auto r = regressions.at(i);
            //     fprintf(f, "%zu, %d, %f, %f, %f, %f, %d\n", globalCount, x0s[i], r.b0, r.b1, r.b2, r.b3, failures[i]);
            // }

            // fclose(f);
        }
        globalCount += 1;
    }

    std::vector<RegCoeffs> restoreShape(
        const std::vector<unsigned int> *scaleCount,
        std::vector<double> *const beta_0,
        std::vector<double> *const beta_1,
        std::vector<double> *const beta_2,
        std::vector<double> *const beta_3,
        const size_t numPoints)
    /* ### allocations ###
        results: size known at function call, 5 * f32 * length

       ### called functions ###
        none
    */
    {
        // this function is required since during convolution, the result array is constructed in this order:
        /*
            0
            1   2
            3   4   5
            6   7   8   9
            10  11  12
            13  14
            15
        */
        // for the following tests, we need it to follow a column-first order. Since the outermost scale contains two
        // entries if the checked block has an even number of points, we must retrace our steps from the convolution

        std::vector<RegCoeffs> results(beta_0->size(), {0, 0, 0, 0}); // checks for coefficients != 0 exist in the rest of the code
        size_t counter = 0;
        size_t maxIdx = numPoints - 5;
        assert(maxIdx == scaleCount->size() - 1);
        // outer loop: index at scale = 2
        for (size_t idx = 0; idx <= maxIdx; idx++)
        { // inner loop: scales > 2 for all points at that scale position

            size_t inner = scaleCount->at(idx);
            size_t offset = 0;
            for (size_t scale_i = 2; scale_i <= inner; scale_i++)
            {
                RegCoeffs current = {
                    beta_0->at(counter),
                    beta_1->at(counter),
                    beta_2->at(counter),
                    beta_3->at(counter),
                };

                results[idx + offset] = current;

                // offset = offset - scale at iteration 0 + the number of points at that scale + 1
                offset += -1 + numPoints - 2 * scale_i;
                counter++;
            }
        }
        return results;
    }

    std::vector<RegCoeffs> findCoefficients_new(
        const std::vector<float> *intensity_log,
        const size_t maxScale) // maximum scale that will be checked. Should generally be limited by peakFrame
    /* ### allocations ###
       coeffs: allocation size known at function call
    */
    {
        assert(maxScale > 1);
        assert(maxScale <= MAXSCALE);
        const unsigned int minScale = 2;
        assert(minScale <= maxScale);

        const size_t regsPerScale = intensity_log->size() - 2 * maxScale;
        const size_t totalScales = maxScale - minScale + 1;
        const size_t iterationCount = regsPerScale * totalScales;

        std::vector<RegCoeffs> coeffs(iterationCount, {NAN, NAN, NAN, NAN});

        // the first n points of every region are extrapolated, where n is the maximum scale -2
        const size_t limit = intensity_log->size() - maxScale;

        size_t scale5Count = 0;
        for (size_t center = maxScale; center < limit; center++)
        {
            const float *cen = intensity_log->data() + center; // this is initially the third real point

            // move along the intensity_log (outer loop)
            // calculate the convolution with the kernel of the lowest scale (= 2), i.e. xT * intensity_log[i:i+4]
            // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
            // b0 is 1, 1, 1, 1, 1
            double product_sum_b0 = *(cen - 2) + *(cen - 1) + *(cen) + *(cen + 1) + *(cen + 2);
            // b1 is -2, -1, 0, 1, 2
            double product_sum_b1 = -2 * *(cen - 2) + -1 * *(cen - 1) + 1 * *(cen + 1) + 2 * *(cen + 2);
            // b2 is 4, 1, 0, 0, 0
            double product_sum_b2 = 4 * *(cen - 2) + 1 * *(cen - 1);
            // b3 is 0, 0, 0, 1, 4
            double product_sum_b3 = 1 * *(cen + 1) + 4 * *(cen + 2);
            {
                const double inv_A = INV_ARRAY[minScale * 6 + 0];
                const double inv_B = INV_ARRAY[minScale * 6 + 1];
                const double inv_C = INV_ARRAY[minScale * 6 + 2];
                const double inv_D = INV_ARRAY[minScale * 6 + 3];
                const double inv_E = INV_ARRAY[minScale * 6 + 4];
                const double inv_F = INV_ARRAY[minScale * 6 + 5];

                // this line is: a*t_i + b * sum(t without i)
                // inv_array starts at scale = 2
                coeffs[scale5Count].b0 = inv_A * product_sum_b0 + inv_B * (product_sum_b2 + product_sum_b3);
                coeffs[scale5Count].b1 = inv_C * product_sum_b1 + inv_D * (product_sum_b2 - product_sum_b3);
                coeffs[scale5Count].b2 = inv_B * product_sum_b0 + inv_D * product_sum_b1 + inv_E * product_sum_b2 + inv_F * product_sum_b3;
                coeffs[scale5Count].b3 = inv_B * product_sum_b0 - inv_D * product_sum_b1 + inv_F * product_sum_b2 + inv_E * product_sum_b3;
            }

            scale5Count += 1;
            for (size_t scale = minScale + 1; scale < maxScale + 1; scale++)
            {
                // expand the kernel to the left and right of the intensity_log.
                // b0 is expanded by the two outer points * 1
                product_sum_b0 += *(cen - scale) + *(cen + scale);
                // b1 is expanded by the points * scale, negative to the left
                product_sum_b1 += -double(scale) * *(cen - scale) + double(scale) * *(cen + scale);
                // b2 and b3 are expanded by scale^2 the outermost point to the left or right
                double scale_sqr = double(scale * scale);
                product_sum_b2 += scale_sqr * *(cen - scale);
                product_sum_b3 += scale_sqr * *(cen + scale);

                const double inv_A = INV_ARRAY[scale * 6 + 0];
                const double inv_B = INV_ARRAY[scale * 6 + 1];
                const double inv_C = INV_ARRAY[scale * 6 + 2];
                const double inv_D = INV_ARRAY[scale * 6 + 3];
                const double inv_E = INV_ARRAY[scale * 6 + 4];
                const double inv_F = INV_ARRAY[scale * 6 + 5];

                const double inv_B_b0 = inv_B * product_sum_b0;
                const double inv_D_b1 = inv_D * product_sum_b1;

                const size_t access = regsPerScale * (scale - minScale);
                coeffs[access].b0 = inv_A * product_sum_b0 + inv_B * (product_sum_b2 + product_sum_b3);
                coeffs[access].b1 = inv_C * product_sum_b1 + inv_D * (product_sum_b2 - product_sum_b3);
                coeffs[access].b2 = inv_B_b0 + inv_D_b1 + inv_E * product_sum_b2 + inv_F * product_sum_b3;
                coeffs[access].b3 = inv_B_b0 - inv_D_b1 + inv_F * product_sum_b2 + inv_E * product_sum_b3;
            }
        }

        return coeffs;
    }

    std::vector<RegCoeffs> findCoefficients(
        const std::vector<float> *intensity_log,
        const size_t maxScale) // maximum scale that will be checked. Should generally be limited by peakFrame
    /* ### allocations ###
        maxInnerLoop: size known at function call
        beta_0 etc.: size calculated in function *4 * f32

       ### called functions ###
        std::accumulate
        restoreShape
    */
    {
        assert(maxScale > 1);
        assert(maxScale <= MAXSCALE);
        const unsigned int minScale = 2;
        assert(minScale <= maxScale);
        const size_t steps = intensity_log->size() - 2 * minScale; // iteration number at scale 2

        // the vector starts and ends with minScale and increases by one towards the middle until it reaches the maxScale value
        std::vector<unsigned int> maxInnerLoop(steps, maxScale); // @todo the vector is not necessary, get rid of it
        size_t numScales = maxScale - minScale;
        {
            for (size_t i = 0; i < numScales; i++)
            {
                maxInnerLoop[i] = i + minScale;
            }
            size_t startIdx = maxInnerLoop.size() - numScales;
            unsigned int currentVal = maxScale - 1;
            for (size_t i = startIdx; i < maxInnerLoop.size(); i++)
            {
                maxInnerLoop[i] = currentVal;
                currentVal -= 1;
            }
            assert(currentVal + 1 == minScale); // last iteration substracts one
        }

        // the number of "missing" regressions covering each index is -1 -2 -3 etc., expressed by the triangular number
        // triangular(n) = n * (n + 1) / 2. the / 2 is omitted, since we multiply by 2 anyway. n is the iteration count from above.
        // since minScale is included in numScales, (minScale - 1) * steps is subtracted
        size_t iterationCount = steps * maxScale - numScales * (numScales + 1) - (minScale - 1) * steps;

        // these arrays contain all coefficients for every loop iteration @todo arena allocator here
        std::vector<double> beta_0(iterationCount, NAN);
        std::vector<double> beta_1(iterationCount, NAN);
        std::vector<double> beta_2(iterationCount, NAN);
        std::vector<double> beta_3(iterationCount, NAN);

        // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
        // The first n entries are contained in the b0 vector, one for each peak the regression is performed
        // over.
        double tmp_product_sum_b0 = 0;
        double tmp_product_sum_b1 = 0;
        double tmp_product_sum_b2 = 0;
        double tmp_product_sum_b3 = 0;

        // debug variables
        // size_t bothPos = 0;
        // size_t eitherPos = 0;
        // size_t bothNeg = 0;

        size_t k = 0;
        for (size_t i = 0; i < steps; i++)
        {
            // move along the intensity_log (outer loop)
            // calculate the convolution with the kernel of the lowest scale (= 2), i.e. xT * intensity_log[i:i+4]
            // tmp_product_sum_b0_vec = intensity_log[i:i+5].sum(axis=0) // numPeaks rows of xT * intensity_log[i:i+4]

            tmp_product_sum_b0 = intensity_log->at(i) + intensity_log->at(i + 1) +
                                 intensity_log->at(i + 2) + intensity_log->at(i + 3) +
                                 intensity_log->at(i + 4); // b0 = 1 for all elements
            tmp_product_sum_b1 = 2 * (intensity_log->at(i + 4) - intensity_log->at(i)) +
                                 intensity_log->at(i + 3) - intensity_log->at(i + 1);
            tmp_product_sum_b2 = 4 * intensity_log->at(i) + intensity_log->at(i + 1);
            tmp_product_sum_b3 = 4 * intensity_log->at(i + 4) + intensity_log->at(i + 3);

            // use [12 + ...] since the array is constructed for the accession arry[scale * 6 + (0:5)]
            const double S2_A = INV_ARRAY[12 + 0];
            const double S2_B = INV_ARRAY[12 + 1];
            const double S2_C = INV_ARRAY[12 + 2];
            const double S2_D = INV_ARRAY[12 + 3];
            const double S2_E = INV_ARRAY[12 + 4];
            const double S2_F = INV_ARRAY[12 + 5];

            // this line is: a*t_i + b * sum(t without i)
            // inv_array starts at scale = 2
            beta_0[k] = S2_A * tmp_product_sum_b0 + S2_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
            beta_1[k] = S2_C * tmp_product_sum_b1 + S2_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
            beta_2[k] = S2_B * tmp_product_sum_b0 + S2_D * tmp_product_sum_b1 + S2_E * tmp_product_sum_b2 + S2_F * tmp_product_sum_b3;
            beta_3[k] = S2_B * tmp_product_sum_b0 - S2_D * tmp_product_sum_b1 + S2_F * tmp_product_sum_b2 + S2_E * tmp_product_sum_b3;

            assert(!isnan(beta_0[k]));
            assert(!isnan(beta_1[k]));
            assert(!isnan(beta_2[k]));
            assert(!isnan(beta_3[k]));

            k += 1;       // update index for the productsums array
            size_t u = 1; // u is the expansion increment
            for (size_t scale = 3; scale < maxInnerLoop[i] + 1; scale++)
            { // minimum scale is 2. so we start with scale + 1 = 3 in the inner loop
                double scale_sqr = double(scale * scale);
                // expand the kernel to the left and right of the intensity_log.
                tmp_product_sum_b0 += intensity_log->at(i - u) + intensity_log->at(i + 4 + u);
                tmp_product_sum_b1 += scale * (intensity_log->at(i + 4 + u) - intensity_log->at(i - u));
                tmp_product_sum_b2 += scale_sqr * intensity_log->at(i - u);
                tmp_product_sum_b3 += scale_sqr * intensity_log->at(i + 4 + u);

                const double inv_A = INV_ARRAY[12 + u * 6 + 0];
                const double inv_B = INV_ARRAY[12 + u * 6 + 1];
                const double inv_C = INV_ARRAY[12 + u * 6 + 2];
                const double inv_D = INV_ARRAY[12 + u * 6 + 3];
                const double inv_E = INV_ARRAY[12 + u * 6 + 4];
                const double inv_F = INV_ARRAY[12 + u * 6 + 5];

                const double inv_B_b0 = inv_B * tmp_product_sum_b0;
                const double inv_D_b1 = inv_D * tmp_product_sum_b1;

                beta_0[k] = inv_A * tmp_product_sum_b0 + inv_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
                beta_1[k] = inv_C * tmp_product_sum_b1 + inv_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
                beta_2[k] = inv_B_b0 + inv_D_b1 + inv_E * tmp_product_sum_b2 + inv_F * tmp_product_sum_b3;
                beta_3[k] = inv_B_b0 - inv_D_b1 + inv_F * tmp_product_sum_b2 + inv_E * tmp_product_sum_b3;

                // assert(!isnan(beta_0[k]));
                // assert(!isnan(beta_1[k]));
                // assert(!isnan(beta_2[k]));
                // assert(!isnan(beta_3[k]));

                // bothNeg += (beta_2[k] < 0) && (beta_3[k] < 0);
                // bothPos += (beta_2[k] >= 0) && (beta_3[k] >= 0);
                // eitherPos += (beta_2[k] < 0) xor (beta_3[k] < 0);

                u += 1; // update expansion increment

                k += 1; // update index for the productsums array
            }
        }

        assert(beta_0.size() == beta_1.size());

        // @todo how does the restructure in dev_G work?
        auto coeffs = restoreShape(&maxInnerLoop,
                                   &beta_0, &beta_1, &beta_2, &beta_3,
                                   intensity_log->size());

        //@todo could the ratio of positives / negatives serve as a criterion for data quality in that region?
        // printf("Both positive: %zu, one negative: %zu, both negative: %zu\n", bothPos, eitherPos, bothNeg);

        return coeffs;
    }

#pragma endregion "running regression"

#pragma region "validate Regression"

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

    void updateRegRange(RegressionGauss *mutateReg, const size_t idxStart, const size_t scale, const float valleyPos);

    int makeValidRegression( // returns the number of the failed test
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const size_t maxApexPos)
    /* ### allocations ###
        selectLog: size calculated in function, max size known
        predictLog: s.o.

       ### called functions ###
        calcApexAndValleyPos_old
        abs
        std::min
        calcDF_cum (2x)
        apexToEdgeRatio
        calcRSS_reg
        isValidQuadraticTerm
        isValidPeakArea
        isValidPeakHeight
        calcSSE_chisqared
        smearingCorrection
        calcUncertaintyPos
    */
    {
        assert(!mutateReg->isValid);
        assert(scale > 1);
        assert(idxStart + 4 < degreesOfFreedom_cum->size());
        assert(mutateReg->coeffs.b0 < 100 && mutateReg->coeffs.b0 > -100);
        assert(mutateReg->coeffs.b1 < 100 && mutateReg->coeffs.b1 > -100);
        assert(mutateReg->coeffs.b2 < 100 && mutateReg->coeffs.b2 > -100);
        assert(mutateReg->coeffs.b3 < 100 && mutateReg->coeffs.b3 > -100);

        // for a regression to be valid, at least one coefficient must be < 0
        if (mutateReg->coeffs.b2 >= 0 && mutateReg->coeffs.b3 >= 0)
        {
            return 1;
        }

        // the model parameters can only be calculated if the coefficients are significantly different from 0
        // significance is determined as the precision of float as the intermediate storage representation here
        // if (std::abs(mutateReg->coeffs.b2) < 10e-8 || std::abs(mutateReg->coeffs.b3) < 10e-8)
        // {
        //     return 1;
        // }

        /*
          Apex and Valley Position Filter:
          This block of code implements the apex and valley position filter.
          It calculates the apex and valley positions based on the coefficients
          matrix B.
        */
        double valley_position = 0;
        // bool first = calcApexAndValleyPos_old(mutateReg, scale, &valley_position);
        int failure = calcApexAndValleyPos_new(mutateReg, scale, &valley_position);

        // assert(first == (failure == 0)); // both succeed

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

        updateRegRange(mutateReg, idxStart, scale, valley_position);

        assert(mutateReg->regSpan.endIdx < intensities->size());
        const size_t idx_x0 = idxStart + scale;

        /*
            Note: left and right limit are not the limits of the regression, but of the window the regression applies in.
            When multiple regressions are combined, the window limits are combined by maximum.
        */
        if (idx_x0 - mutateReg->regSpan.startIdx < 2 || (mutateReg->regSpan.endIdx - idx_x0 < 2))
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
        size_t idxApex = (size_t)mutateReg->apex_position + idx_x0;
        float apexToEdge = apexToEdgeRatio(mutateReg->regSpan, idxApex, intensities);
        if (!(apexToEdge > 2))
        {
            return 6; // invalid apex to edge ratio
        }

        double RSS_reg = calcRSS_reg(&mutateReg->coeffs, intensities_log,
                                     mutateReg->regSpan, idx_x0);

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

        double mse = RSS_reg / double(df_sum - 4); // mean squared error with respect to the degrees of freedom - @todo is the -4 correct?

        if (!isValidQuadraticTerm(mutateReg->coeffs, scale, mse, df_sum))
        {
            return 8; // statistical insignificance of the quadratic term
        }
        if (!isValidPeakArea(&mutateReg->coeffs, mse, scale, df_sum))
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
        calcPeakHeightUncert(mutateReg, mse, scale);
        if (1 / mutateReg->uncertainty_height <= T_VALUES[df_sum - 5]) // statistical significance of the peak height
        {
            return 10;
        }
        // at this point without height, i.e., to get the real uncertainty
        // multiply with height later. This is done to avoid exp function at this point
        if (!isValidPeakHeight(mse, scale, mutateReg->apex_position, valley_position, df_sum, apexToEdge))
        {
            return 11; // statistical insignificance of the height
        }

        /*
          Area Filter:
          This block of code implements the area filter. It calculates the Jacobian
          matrix for the peak area based on the coefficients matrix B. Then it calculates
          the uncertainty of the peak area based on the Jacobian matrix.
          NOTE: this function does not consider b0: i.e. to get the real uncertainty and
          area multiply both with Exp(b0) later. This is done to avoid exp function at this point
        */
        // it might be preferential to combine both functions again or store the common matrix somewhere
        calcPeakAreaUncert(mutateReg, mse, scale);

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
        float chiSquare = calcSSE_chisqared(mutateReg->coeffs, intensities, mutateReg->regSpan, idx_x0);
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

        mutateReg->uncertainty_pos = calcUncertaintyPos(mse, mutateReg->coeffs, mutateReg->apex_position, scale);
        mutateReg->df = df_sum - 4; // @todo add explanation for -4
        mutateReg->apex_position += idxStart + scale;

        // @todo this should be part of a more structured test
        if (mutateReg->apex_position < 2 || mutateReg->apex_position > maxApexPos)
        { // this situation implies that only one half of the peak has the minimum data points for a gaussian
            return 14;
        }

        mutateReg->scale = scale;
        mutateReg->index_x0 = idx_x0;
        mutateReg->mse = mse; // the quadratic mse is used for the weighted mean of the coefficients later
        mutateReg->isValid = true;
        return 0;
    }

    void updateRegRange(RegressionGauss *mutateReg, const size_t idxStart, const size_t scale, const float valleyPos)
    {
        size_t rangeEnd = idxStart + 2 * scale;
        if (valleyPos == 0) [[likely]]
        {
            // no valley point exists
            mutateReg->regSpan = {idxStart, rangeEnd};
            return;
        }
        size_t absValley = size_t(valleyPos);
        assert(absValley >= 2);

        bool valleyLeft = valleyPos < 0;
        if (valleyLeft)
        {
            size_t newStart = absValley < scale ? idxStart + scale - absValley : idxStart;
            mutateReg->regSpan = {newStart, rangeEnd};
        }
        else
        {
            size_t newEnd = std::min(rangeEnd, absValley + idxStart + scale);
            mutateReg->regSpan = {idxStart, newEnd};
        }
        assert(mutateReg->regSpan.endIdx - mutateReg->regSpan.startIdx > 3);
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
                        secondReg->coeffs,
                        intensities,
                        secondReg->regSpan,
                        secondReg->index_x0);
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
                    activeReg->coeffs,
                    intensities,
                    activeReg->regSpan,
                    activeReg->index_x0);
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
        const ProfileBlock *block,
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
            RegressionGauss regression = (*validRegressionsVec)[i];
            assert(regression.isValid);
            CentroidPeak peak;
            peak.number_MS1 = scanNumber;
            // add height
            RegCoeffs coeff = regression.coeffs;
            peak.height = exp_approx_d(coeff.b0 + (regression.apex_position - regression.index_x0) * coeff.b1 * 0.5);
            // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.heightUncertainty = regression.uncertainty_height * peak.height;

            size_t offset = (size_t)std::floor(regression.apex_position);
            double mz0 = block->mz[offset];
            double delta_mz = block->mz[offset + 1] - mz0;

            // add scaled area
            double exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression.area * exp_b0 * delta_mz;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0 * delta_mz;

            // add scaled apex position (mz)
            peak.mz = mz0 + delta_mz * (regression.apex_position - std::floor(regression.apex_position));
            peak.mzUncertainty = regression.uncertainty_pos * delta_mz * T_VALUES[regression.df + 1] * sqrt(1 + 1 / (regression.df + 4));

            // quality params
            peak.DQSC = 1 - erf_approx_f(regression.uncertainty_area / regression.area);
            peak.df = regression.df;

            peak.numCompetitors = regression.numCompetitors;
            peak.scale = regression.scale;

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
            RegressionGauss regression = (*validRegressionsVec)[i];
            assert(regression.isValid);

            FeaturePeak peak;
            // add height
            RegCoeffs coeff = regression.coeffs;
            // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.height = exp_approx_d(coeff.b0 + (regression.apex_position - regression.index_x0) * coeff.b1 * 0.5);
            peak.heightUncertainty = regression.uncertainty_height * peak.height;

            // calculate the apex position in RT
            size_t idx_leftOfApex = (size_t)regression.apex_position;
            size_t idx_leftOfApex_absolute = convertRT->indexOfOriginalInInterpolated[idx_leftOfApex];

            // @todo fix peak detection
            if (idx_leftOfApex < 2)
            {
                continue;
            }

            assert(idx_leftOfApex > 1); // at least two points to the left
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
            float rt_fraction = (regression.apex_position - floor(regression.apex_position));
            assert(rt_fraction >= 0);
            assert(rt_fraction < 1);
            float rt_apex = rt_leftOfApex_true + delta_rt * rt_fraction;
            peak.retentionTime = rt_apex;
            peak.RT_Uncertainty = regression.uncertainty_pos * delta_rt;

            // add area
            float exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression.area * exp_b0 * delta_rt;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0 * delta_rt;

            // mz, mzUncertainty, mean DQSC and meanDQSF are all calculated in after this function is called in measurement_data
            peak.DQSF = 1 - erf_approx_f(regression.uncertainty_area / regression.area);

            assert(regression.regSpan.endIdx - regression.index_x0 > 1);
            peak.idxPeakStart = regression.regSpan.startIdx;
            peak.idxPeakEnd = regression.regSpan.endIdx;
            peak.index_x0_offset = regression.index_x0 - regression.regSpan.startIdx;
            assert(peak.idxPeakEnd > peak.idxPeakStart);
            assert(peak.idxPeakEnd > peak.index_x0_offset);
            assert(peak.idxPeakEnd - peak.idxPeakStart >= 4); // at least five points

            // params needed to merge two peaks
            bool apexLeft = regression.apex_position < regression.index_x0;
            assert(apexLeft == (coeff.b1 <= 0));

            coeff.b1 /= delta_rt;
            coeff.b2 /= delta_rt * delta_rt;
            coeff.b3 /= delta_rt * delta_rt;
            peak.coefficients = coeff;
            peak.mse_base = regression.mse;

            peak.scale = regression.scale;
            peak.interpolationCount = rangeLen(&regression.regSpan) - regression.df - 4; // -4 since the degrees of freedom are reduced by 1 per coefficient
            peak.competitorCount = regression.numCompetitors;

            peaks->push_back(std::move(peak));
        }
    }
#pragma endregion "create peaks"

#pragma region calcSSE

    double calcRSS_reg(const RegCoeffs *coeff,
                       const std::vector<float> *y_start,
                       const Range_i range,
                       const size_t index_x0)
    // no allocations
    {
        double RSS = 0.0;
        // left side
        for (size_t iSegment = range.startIdx; iSegment < index_x0; iSegment++)
        {
            double new_x = -1 * double(index_x0 - iSegment);
            double y_predict = regAt_L(coeff, new_x);
            double difference = y_start->at(iSegment) - y_predict;

            RSS += difference * difference;
        }
        // center point
        RSS += (y_start->at(index_x0) - coeff->b0) * (y_start->at(index_x0) - coeff->b0); // x = 0 -> (b0 - y)^2

        // right side
        for (size_t iSegment = index_x0 + 1; iSegment < range.endIdx + 1; iSegment++) // iSegment = 0 is center point calculated above
        {
            double new_x = double(iSegment - index_x0);
            double y_predict = regAt_R(coeff, new_x);
            double difference = y_start->at(iSegment) - y_predict;

            RSS += difference * difference;
        }
        return RSS;
    }

    double calcRSS_H0(const std::vector<float> *observed, const Range_i *range)
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

    bool f_testRegression(const std::vector<float> *observed, double RSS_reg, const Range_i *range)
    {
        // during the tests, the RSS for the regression has already been calculated in calcRSS_reg
        assert(RSS_reg > 0);

        double RSS_H0 = calcRSS_H0(observed, range);

        double fval = f_value(RSS_reg, RSS_H0, 4, 1, observed->size());

        const size_t length = rangeLen(range);
        double refval = F_VALUES[length]; // pre-calculated for alpha = 0.05

        return fval > refval; // reject H0, significant difference from y = b

        // @todo add a section to also test for y = mx + b
    }

    float calcSSE_exp(const RegCoeffs coeff, const std::vector<float> *y_start, const Range_i regSpan, size_t index_x0)
    /* ### allocations ###
        none!

       ### called functions ###
        exp_approx_d
    */
    { // @todo this does not account for asymmetric RT distances, will that be a problem?
        double result = 0.0;
        // left side
        for (size_t iSegment = regSpan.startIdx; iSegment < index_x0; iSegment++) // @todo factor this loop into a function
        {
            double new_x = double(iSegment) - double(index_x0); // always negative
            double y_predict = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff;
        }
        result += ((*y_start)[index_x0] - exp_approx_d(coeff.b0)) * ((*y_start)[index_x0] - exp_approx_d(coeff.b0)); // x = 0 -> (b0 - y)^2
        // right side
        for (size_t iSegment = index_x0 + 1; iSegment < regSpan.endIdx + 1; iSegment++) // start one past the center, include right limit index
        {
            double new_x = double(iSegment) - double(index_x0);                                // always positive
            double y_predict = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * new_x) * new_x); // b3 instead of b2
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff;
        }
        return result;
    }

    float calcSSE_chisqared(const RegCoeffs coeff, const std::vector<float> *y_start,
                            const Range_i regSpan, size_t index_x0)
    /* ### allocations ###
        none!

       ### called functions ###
        exp_approx_d
    */
    {
        double result = 0.0;
        // left side
        for (size_t iSegment = regSpan.startIdx; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_predict = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff / y_predict;
        }
        // center point, x = 0 -> (b0 - y)^2
        double exp_b0 = exp_approx_d(coeff.b0);
        result += (((*y_start)[index_x0] - exp_b0) * ((*y_start)[index_x0] - exp_b0)) / exp_b0;

        for (size_t iSegment = index_x0 + 1; iSegment < regSpan.endIdx + 1; iSegment++) // iSegment = 0 is center point (calculated above)
        {
            double y_predict = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * iSegment) * iSegment); // b3 instead of b2
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_predict) * (y_current - y_predict);
            result += newdiff / y_predict;
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
    /* ### allocations ###
        none!

       ### called functions ###
        std::min
        std::max
        calcDF_cum
        calcSSE_exp
    */
    {
        double best_mse = INFINITY;
        unsigned int bestRegIdx = 0;

        // identify left (smallest) and right (largest) limit of the grouped regression windows
        size_t left_limit = -1;
        size_t right_limit = 0;
        for (size_t i = regSpan.startIdx; i < regSpan.endIdx + 1; i++)
        {

            const RegressionGauss *reg = &(*regressions)[i];
            left_limit = std::min(left_limit, reg->regSpan.startIdx);
            right_limit = std::max(right_limit, reg->regSpan.endIdx);
        }
        // the new df_sum is only needed since the function limits are adjusted above, correct that?
        size_t df_sum = calcDF_cum(degreesOfFreedom_cum, regSpan);

        for (unsigned int i = regSpan.startIdx; i < regSpan.endIdx + 1; i++)
        {
            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            double mse = calcSSE_exp((*regressions)[i].coeffs,
                                     intensities,
                                     {left_limit, right_limit},
                                     (*regressions)[i].index_x0);
            mse /= (df_sum - 4);

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
        unsigned int substract = regSpan.startIdx == 0 ? 0 : degreesOfFreedom_cum->at(regSpan.startIdx - 1);
        size_t df = degreesOfFreedom_cum->at(regSpan.endIdx) - substract;
        assert(df <= degreesOfFreedom_cum->size()); // @todo not applicable for non-bool df vecs
        return df;
    }

    int calcApexAndValleyPos_new(
        RegressionGauss *mutateReg,
        const size_t scale,
        double *valley_position)
    {
        // assert(valley_position == 0); // @todo remove for final implementation?

        const bool valley_left = mutateReg->coeffs.b2 >= 0;
        const bool valley_right = mutateReg->coeffs.b3 >= 0;
        const bool apexLeft = mutateReg->coeffs.b1 < 0;

        // position maximum / minimum of b2 or b3. This is just the frst derivative of the peak half equation (b0 + b1 x + b23 x^2)
        float position_b2 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b2);
        float position_b3 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b3);
        // scale +1 / -1: prevent apex position to be at the edge of the data
        bool farOut_b2 = position_b2 < -float(scale) + 1;
        bool farOut_b3 = position_b3 > float(scale) - 1;

        // range check: There is at most one point in the left or right half of the peak
        if (valley_left && position_b2 > -2)
            return 1;

        if (valley_right && position_b3 < 2)
            return 1;

        if (apexLeft)
        {
            // if the apex is left, the left half cannot have a valley
            if (valley_left)
                return 2;

            // the apex position is not at a distance of at least two to the edge of the scale
            if (farOut_b2)
                return 3;

            // assert(mutateReg->apex_position == position_b2);
            mutateReg->apex_position = position_b2;

            if (valley_right)
                *valley_position = position_b3;
        }
        else
        {
            // if the apex is right, the right half cannot have a valley
            if (valley_right)
                return 4;

            // the apex position is not at a distance of at least two to the edge of the scale
            if (farOut_b3)
                return 5;

            // assert(mutateReg->apex_position == position_b3);
            mutateReg->apex_position = position_b3;

            if (valley_left)
                *valley_position = position_b2;
        }

        return 0;
    }

    bool calcApexAndValleyPos_old(
        RegressionGauss *mutateReg,
        const size_t scale,
        double *valley_position)
    /* ### allocations ###
        none!

       ### called functions ###
        none!
    */
    {
        // symmetric model should apply, this is not possible with the current peak model @todo
        // assert(mutateReg->coeffs.b1 != 0);
        // assert(mutateReg->coeffs.b2 != 0 && mutateReg->coeffs.b3 != 0);

        // calculate key by checking the signs of coeff
        const float floatScale = float(scale);
        int key = 0;
        bool b1_is = mutateReg->coeffs.b1 < 0;
        bool b2_is = mutateReg->coeffs.b2 < 0;
        bool b3_is = mutateReg->coeffs.b3 < 0;

        assert(b2_is || b3_is);

        const int key2 = b1_is + 3 * b2_is + 5 * b3_is;

        if (mutateReg->coeffs.b1 < 0)
        {
            key += 1;
        }
        if (mutateReg->coeffs.b2 < 0)
        {
            key += 3;
        }
        if (mutateReg->coeffs.b3 < 0)
        {
            key += 5;
        }
        assert(key == key2);

        // b1, b2, b3 are negative            ; 1 + 3 + 5 = 9
        // b1 is positive, b2, b3 are negative; 0 + 3 + 5 = 8
        // b1, b2 are negative, b3 is positive; 1 + 3 + 0 = 4
        // b1, b2 are positive, b3 is negative; 0 + 0 + 5 = 5

        // these two are the case where the apex position is also a valley, and as such invalid
        // b1, b3 are negative, b2 is positive; 1 + 0 + 5 = 6
        // b1, b3 are positive, b2 is negative; 0 + 3 + 0 = 3
        enum keyCase
        {
            apexLeft_valleyNone = 9,
            apexRight_valleyNone = 8,
            apexLeft_valleyRight = 4,
            apexRight_valleyLeft = 5
        };

        // position maximum / minimum of b2 or b3. This is just the frst derivative of the peak half equation (b0 + b1 x + b23 x^2)
        float position_2 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b2);
        float position_3 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b3);

        switch (key)
        {
        case apexLeft_valleyNone:
            mutateReg->apex_position = position_2; //-B1 / 2 / B2;  // is negative
            *valley_position = 0;                  // no valley point
            return position_2 > -floatScale + 1;   // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyNone:                 // Case 1b: apex right
            mutateReg->apex_position = position_3; //-B1 / 2 / B3;     // is positive
            *valley_position = 0;                  // no valley point
            return position_3 < floatScale - 1;    // scale -1: prevent apex position to be at the edge of the data

        case apexLeft_valleyRight:
            mutateReg->apex_position = position_2;                                      //-B1 / 2 / B2;      // is negative
            *valley_position = position_3;                                              //-B1 / 2 / B3;      // is positive
            return position_2 >= -floatScale + 1 && *valley_position - position_2 >= 2; // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyLeft:
            mutateReg->apex_position = position_3;                                     //-B1 / 2 / B3;       // is positive
            *valley_position = position_2;                                             //-B1 / 2 / B2;       // is negative
            return position_3 <= floatScale - 1 && position_3 - *valley_position >= 2; // scale -1: prevent apex position to be at the edge of the data

        default:
            return false; // invalid case
        }
    }

    double apexToEdgeRatio(
        const Range_i regSpan,
        const size_t idxApex,
        const std::vector<float> *intensities)
    {
        // is the apex at least twice as large as the outermost point?
        // assumption: outermost point is already near base level
        assert(idxApex < intensities->size());

        float apex = intensities->at(idxApex);
        float left = intensities->at(regSpan.startIdx);
        float right = intensities->at(regSpan.endIdx);
        return (left < right) ? (apex / left) : (apex / right);
    }

    bool isValidQuadraticTerm(
        const RegCoeffs coeff,
        const size_t scale,
        const double mse,
        const size_t df_sum)
    {
        float divisor = std::sqrt(INV_ARRAY[scale * 6 + 4] * mse); // inverseMatrix_2_2 is at position 4 of initialize()
        float tValue = std::max(                                   // t-value for the quadratic term
            std::abs(coeff.b2),                                    // t-value for the quadratic term left side of the peak
            std::abs(coeff.b3));                                   // t-value for the quadratic term right side of the peak
        return tValue > T_VALUES[df_sum - 5] * divisor;            // statistical significance of the quadratic term
        // note that the tvalue would have to be divided by the divisor, but this is not always compiled to a multiplication
    }

    void calcPeakHeightUncert(
        RegressionGauss *mutateReg,
        const double mse,
        const size_t scale)
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
        mutateReg->uncertainty_height = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale));
        return;
    }

    bool isValidPeakHeight(
        const double mse,
        const size_t scale,
        const double apex_position,
        const double valley_position,
        const size_t df_sum,
        const double apexToEdge)
    {
        // check if the peak height is significantly greater than edge signal
        double Jacobian_height[4]{0, 0, 0, 0};
        const bool apexLeft = apex_position < 0;
        const double altValley = double(scale) * (apexLeft ? -1 : 1);
        const double valley = valley_position == 0 ? altValley : valley_position;
        const double j1 = apex_position - valley;
        const double j23 = apex_position * apex_position - valley * valley;
        const double j2 = apexLeft ? j23 : 0;
        const double j3 = apexLeft ? 0 : j23;
        const double jacobianHeight[4]{0, j1, j2, j3};

        double valley_position2 = valley_position;

        if (apexLeft)
        {
            // float edge_position = (valley_position != 0) ? valley_position : static_cast<float>(-scale);
            if (valley_position == 0)
            {
                valley_position2 = static_cast<double>(scale) * -1;
            }

            Jacobian_height[1] = apex_position - valley_position2;
            Jacobian_height[2] = apex_position * apex_position - valley_position2 * valley_position2; // adjust for uncertainty calculation of apex to edge ratio
        }
        else
        {
            if (valley_position == 0)
            {
                valley_position2 = static_cast<double>(scale);
            }
            Jacobian_height[1] = apex_position - valley_position2;
            Jacobian_height[3] = apex_position * apex_position - valley_position2 * valley_position2; // adjust for uncertainty calculation of apex to edge ratio
        }

        assert(float(jacobianHeight[1]) == float(Jacobian_height[1]));
        assert(float(jacobianHeight[2]) == float(Jacobian_height[2]));
        assert(float(jacobianHeight[3]) == float(Jacobian_height[3]));

        float uncertainty_apexToEdge = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale));

        // @todo why -2? Just to account for position?
        bool peakHeightSignificant = (apexToEdge - 2) > T_VALUES[df_sum - 5] * (apexToEdge * uncertainty_apexToEdge);
        return peakHeightSignificant;
    }

    void calcPeakAreaUncert(RegressionGauss *mutateReg, const double mse, const size_t scale)
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

        double J[4]; // Jacobian matrix

        // here we have to check if there is a valley point or not // @todo this can be simplified
        double err_L =
            (b2 < 0)
                ? experfc(B1_2_SQRTB2, -1.0) // 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
                : dawson5(B1_2_SQRTB2);      // erfi(b1 / 2 / SQRTB2);        // peak with valley point;

        double err_R =
            (b3 < 0)
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

        J[0] = J_1_R + J_1_L;
        J[1] = J_2_R + J_2_L;
        J[2] = -B1_2_B2 * (J_2_L + J_1_L / b1);
        J[3] = -B1_2_B3 * (J_2_R + J_1_R / b1);

        // at this point the area is without exp(b0), i.e., to get the real area multiply with exp(b0) later. This is done to avoid exp function at this point
        mutateReg->area = J[0];
        mutateReg->uncertainty_area = std::sqrt(mse * multiplyVecMatrixVecTranspose(J, scale));

        return;
    }

    bool isValidPeakArea(
        const RegCoeffs *coeff,
        const double mse,
        const size_t scale,
        const size_t df_sum)
    // no allocations
    {
        double doubleScale = double(scale);
        // double b0 = coeff->b0;
        double b1 = coeff->b1;
        double b2 = coeff->b2;
        double b3 = coeff->b3;

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
        double J_covered[4]; // Jacobian matrix for the covered peak area
        {
            // calculate the y values at the left and right limits @todo error here?
            const double y_left = exp_approx_d(b1 * x_left + b2 * x_left * x_left);
            const double y_right = exp_approx_d(b1 * x_right + b3 * x_right * x_right);
            // const double y_left = regExpAt_L(coeff, x_left);
            // const double y_right = regExpAt_R(coeff, x_right);
            const double dX = x_right - x_left;
            assert(dX > 0);

            // calculate the Jacobian matrix terms
            const double J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
            const double J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
            const double J_2_common_L = B1_2_B2 / b1;
            const double J_2_common_R = B1_2_B3 / b1;

            // calculate the trapzoid correction terms for the jacobian matrix
            const double trpzd_b0 = (y_right + y_left) * dX / 2;
            const double trpzd_b1 = (x_right * y_right + x_left * y_left) * dX / 2;
            const double trpzd_b2 = (x_left * x_left * y_left) * dX / 2;
            const double trpzd_b3 = (x_right * x_right * y_right) * dX / 2;

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

        float area_uncertainty_covered = std::sqrt(mse * multiplyVecMatrixVecTranspose(J_covered, scale));

        // J[0] / uncertainty > Tval
        bool J_is_significant = J_covered[0] > T_VALUES[df_sum - 5] * area_uncertainty_covered;

        return J_is_significant;
    }
#pragma endregion isValidPeakArea

#pragma region "calcUncertaintyPosition"
    float calcUncertaintyPos(
        const double mse,
        const RegCoeffs coeff,
        const double apex_position,
        const size_t scale)
    {
        double _b1 = 1 / coeff.b1;
        double _b2 = 1 / coeff.b2;
        double _b3 = 1 / coeff.b3;
        double J[4]; // Jacobian matrix
        J[0] = 0.f;
        J[1] = apex_position * _b1;
        if (apex_position < 0)
        {
            J[2] = -apex_position * _b2;
            J[3] = 0;
        }
        else
        {
            J[2] = 0;
            J[3] = -apex_position * _b3;
        }
        return std::sqrt(mse * multiplyVecMatrixVecTranspose(J, scale));
    }
#pragma endregion "calcUncertaintyPosition"

#pragma region "convolve regression"

    inline double multiplyVecMatrixVecTranspose(const double vec[4], size_t scale)
    {
        scale *= 6;
        double result = vec[0] * vec[0] * INV_ARRAY[scale + 0] +
                        vec[1] * vec[1] * INV_ARRAY[scale + 2] +
                        (vec[2] * vec[2] + vec[3] * vec[3]) * INV_ARRAY[scale + 4] +
                        2 * (vec[2] * vec[3] * INV_ARRAY[scale + 5] +
                             vec[0] * (vec[1] + vec[3]) * INV_ARRAY[scale + 1] +
                             vec[1] * (vec[2] - vec[3]) * INV_ARRAY[scale + 3]);
        return result;
    }
#pragma endregion "convolve regression"

#pragma region "Feature Detection"

    double medianVec(const std::vector<float> *vec)
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

            const size_t maxApexIdx = convertRT->groups.size() - 2;

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
                              maxScale,
                              maxApexIdx);

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

                // @todo make sure these hold true somewhere else
                if (currentPeak.index_x0_offset < 2)
                {
                    assert(false);
                }
                if (currentPeak.idxPeakEnd - currentPeak.idxPeakStart - currentPeak.index_x0_offset < 2)
                {
                    assert(false);
                }
                if (currentPeak.idxPeakEnd - currentPeak.idxPeakStart + 2 < currentPeak.index_x0_offset)
                {
                    assert(false);
                }
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

    std::vector<CentroidPeak> findCentroids( // this function needs to be reworked further
        XML_File &data,                      // @todo get rid of the direct coupling to pugixml
        const std::vector<unsigned int> *selectedIndices)
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

        std::vector<CentroidPeak> centroids;
        centroids.reserve(countSelected * 100);

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

            // ### @todo this is for development only, highly inefficient at scale! ###
            // hardFilter(&spectrum_mz, &spectrum_int, 247.1, 247.3);
            if (spectrum_mz.empty())
                continue;

            centroids.clear();

            // @todo every group is just an index range into three same-length vectors if we do not
            // perform interpolation. Should interpolation be necessary (includes extrapolation),
            // add these values into the actual data(?)
            size_t maxWindowSize = pretreatDataCentroids(&groupedData, &spectrum_mz, &spectrum_int);
            if (maxWindowSize == 0) // this is also relevant to filtering, add a warning if no filter?
                continue;

            findCentroidPeaks(&centroids, &groupedData, i, ID_spectrum, maxWindowSize);
        }
        for (unsigned int i = 0; i < centroids.size(); i++)
        {
            centroids[i].ID = i;
        }

        return centroids; // @todo mutate centroids and return success / failure
    }

    void binProfileSpec(std::vector<Range_i> *result,
                        const std::vector<double> *diffs,
                        // const std::vector<unsigned int> *diffOrder,
                        const std::vector<double> *cumDiffs, // indices into cumDiffs must be right-shifted by one!
                                                             // size_t previousDiffPos,              // skip this many points in the diffOrder vector
                        const Range_i regSpan)
    /* ### allocations ###
        none!

       ### called functions ###
        binningCritVal
        std::max_element
        std::distance
        binProfileSpec

    */
    {
        // perform the recursive split introduced during binning to find gaps in mz
        // CritVal uses the standard deviation, which is estimated as the mean centroid error during binning.
        // here, we can use the real SD of the point-to-point differences.
        // @todo use the work done for the binning algorithm

        size_t start = regSpan.startIdx;
        size_t end = regSpan.endIdx;
        assert(start <= end); // <= since one point at the end could get removed, see below
        assert(!diffs->empty());
        assert(cumDiffs->size() == diffs->size() + 1);
        assert(cumDiffs->at(0) == 0); // this is important so there are no special cases when forming the difference

        size_t length = end - start + 1;
        if (length < 4) // one point less due to differences being used
        {
            return;
        }

        // we use the mean difference here since the differences should follow a flat distribution. The basic assumption
        // is that every point has an error of half the distance to its neighbours, since all points are evenly spaced.
        double meanDiff = (cumDiffs->at(end + 1) - cumDiffs->at(start)) / length;
        double critVal = binningCritVal(length, meanDiff / 2);

        // max of difference
        auto pmax = std::max_element(diffs->begin() + start, diffs->begin() + end + 1);
        double max = *pmax;
        size_t maxPos = std::distance(diffs->begin(), pmax);

        if (max < critVal)
        {
            // block is complete, add limits to result vector
            result->push_back({start, end + 1}); // end + 1 since difference has one point less
            return;
        }
        // recursive split at max - different calling convention since we work with differences
        if (maxPos != start)
        {
            binProfileSpec(result, diffs, cumDiffs, {start, maxPos - 1}); // when setting the block, 1 is added to end
        }
        if (maxPos != end)
        {
            binProfileSpec(result, diffs, cumDiffs, {maxPos + 1, end}); // one past the max to avoid large value
        }
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
                size_t span = pos - rangeStart + 1;
                if (span >= 5)
                {
                    // at least five points, add block
                    // pos - 1 is used since pos is 0
                    ranges.push_back({rangeStart, pos - 1});
                    maxRangeSpan = maxRangeSpan > span ? maxRangeSpan : span;
                }
                rangeStart = pos + 1;
            }
        }

        size_t span = spectrum_int->size() - rangeStart;
        if (span >= 5)
        {
            ranges.push_back({rangeStart, spectrum_int->size() - 1});
            maxRangeSpan = maxRangeSpan > span ? maxRangeSpan : span;
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
            // extrapolate every range. Intensities are extrapolated by halving, so the outermost real
            // intensity * 2^n is used for the nth point of the extrapolation. This allows us to use
            // bit shifts (+cast) to do the division as intensity / (1 << n)
            insert_ms.clear();
            insert_int.clear();
            cumdf.clear();

            const size_t idxL = ranges[i].startIdx;
            const size_t idxR = ranges[i].endIdx;

            // left extrapolate
            {
                const double mzDiff_L = spectrum_mz->at(idxL + 1) - spectrum_mz->at(idxL);
                const double leftInt = spectrum_int->at(idxL);
                assert(leftInt > 1);
                for (size_t L = maxExtra; L > 0; L--)
                {
                    double newMZ = spectrum_mz->at(idxL) - mzDiff_L * L;
                    insert_ms.push_back(newMZ);

                    int divisor = 1 << L;
                    double newInt = leftInt / double(divisor);
                    insert_int.push_back(newInt);

                    cumdf.push_back(0);
                }
            }

            // copy real values
            {
                for (size_t M = idxL; M <= idxR; M++)
                {
                    insert_ms.push_back(spectrum_mz->at(M));
                    insert_int.push_back(spectrum_int->at(M));
                    size_t newdf = cumdf.back() + 1;
                    cumdf.push_back(newdf);
                }
            }

            // right extrapolate
            {
                const double mzDiff_R = spectrum_mz->at(idxR) - spectrum_mz->at(idxR - 1);
                const double rightInt = spectrum_int->at(idxR);
                assert(rightInt > 1);
                size_t df = cumdf.back();
                for (size_t R = 1; R <= maxExtra; R++)
                {
                    double newMZ = spectrum_mz->at(idxR) - mzDiff_R * R;
                    insert_ms.push_back(newMZ);

                    int divisor = 1 << R;
                    double newInt = rightInt / double(divisor);
                    insert_int.push_back(newInt);

                    cumdf.push_back(df);
                }
            }
            ProfileBlock b = {insert_int,
                              insert_ms,
                              cumdf,
                              idxL, idxR};
            groupedData->push_back(b);
        }
        return maxRangeSpan;
    }

    inline ProfileBlock initBlock(size_t blocksize)
    {
        std::vector<float> mz_int(blocksize, 0);
        std::vector<unsigned int> cumdf(blocksize, 0);
        for (size_t i = 2; i < blocksize - 2; i++)
        {
            cumdf[i] = i - 1;
        }
        cumdf[blocksize - 1] = blocksize - 4;
        cumdf[blocksize - 2] = blocksize - 4;

        ProfileBlock ret = {mz_int,
                            mz_int,
                            cumdf,
                            0, 0};
        return ret;
    }

    void clearBlock(ProfileBlock *block, size_t blocksize)
    {
        assert(blocksize > 1);

        block->startPos = 0;
        block->endPos = 0;

        bool expand = block->cumdf.size() < blocksize;
        size_t addition = block->cumdf.size();

        block->mz.resize(blocksize);
        assert(block->mz.size() == blocksize);

        block->intensity.resize(blocksize);

        block->cumdf.resize(blocksize);
        assert(block->cumdf[1] == 0);
        if (expand)
        {
            for (size_t i = addition - 2; i < blocksize - 2; i++)
            {
                // starts at -2 since we used dummy values there
                block->cumdf[i] = block->cumdf[i - 1];
            }
        }
        block->cumdf[blocksize - 2] = blocksize - 4;
        block->cumdf[blocksize - 1] = blocksize - 4;
    }

    size_t pretreatDataCentroids_old(
        std::vector<ProfileBlock> *groupedData,
        const std::vector<double> *spectrum_mz,
        const std::vector<double> *spectrum_int)
    /* ### allocations ###
        intensities_profile: all known at function call
        mz_profile: s.o.
        idxConvert: s.o.

        diffs: size calculated in function
        cumDiffs: s.o.

        result: size unknown

       ### called functions ###
        binProfileSpec
        initBlock (inline)
    */
    {
        assert(groupedData->empty());
        std::vector<double> intensities_profile;
        std::vector<double> mz_profile;
        std::vector<unsigned int> idxConvert; // used to trace back processing steps to untreated data

        { // remove zeroes
            size_t size = spectrum_mz->size();
            assert(spectrum_int->size() == size);
            intensities_profile.reserve(size);
            mz_profile.reserve(size);
            idxConvert.reserve(size);

            // Depending on the vendor, a profile contains a lot of points with intensity 0.
            // These were added by the vendor software and must be removed prior to processing.
            for (unsigned int i = 0; i < size; ++i)
            {
                if (spectrum_int->at(i) == 0.0)
                {
                    continue; // skip values with no intensity @todo minimum intensity?
                }
                intensities_profile.push_back(spectrum_int->at(i));
                mz_profile.push_back(spectrum_mz->at(i));
                idxConvert.push_back(i);
            }
        }
        if (intensities_profile.empty())
            return 0;
        assert(!mz_profile.empty());

        std::vector<double> diffs;
        std::vector<double> cumDiffs;
        { // calculate mz differences
            diffs.reserve(mz_profile.size() - 1);
            cumDiffs.reserve(mz_profile.size());
            cumDiffs.push_back(0);
            double totalDiff = 0;

            for (size_t i = 1; i < mz_profile.size(); i++)
            {
                float diff = mz_profile[i] - mz_profile[i - 1];
                assert(diff != 0);
                diffs.push_back(diff);
                totalDiff += diff;
                cumDiffs.push_back(totalDiff);
            }
        }

        std::vector<Range_i> result; // result contains the start- and end indices of all relevant blocks in the data.
        result.reserve(128);

        double meanDiff = 0;
        for (size_t i = 0; i < diffs.size(); i++)
        {
            meanDiff += diffs[i];
        }
        meanDiff /= diffs.size();

        unsigned int knownStart = 0;
        unsigned int knownEnd = diffs.size() - 1;

        if (meanDiff > 0.1) [[likely]]
        {
            // in a real mass spectrum, this should always be a given. If not, the noise level is so
            // high that it might be impossible to find actually useful data here. Either way, since
            // we search for centroids in these regions, 0.1 is too large if anything. There will never
            // be a useable centroid with this large a m/z difference in a high res mass spectrum
            for (size_t i = 0; i < diffs.size(); i++)
            {
                if (diffs[i] > meanDiff) // @todo this does not work
                {
                    knownEnd = i - 1;
                    if (knownEnd > knownStart)
                    {
                        binProfileSpec(&result, &diffs, &cumDiffs, {knownStart, knownEnd});
                    }
                    knownStart = i + 1;
                }
            }
        }
        else
        {
            // this is expected for ToF data (?) @todo
            // fprintf(stderr, "Warning: a spectrum has no clear breaks.\n");
        }

        if (knownStart == 0 && knownEnd == diffs.size() - 1)
        {
            // fprintf(stderr, "Warning: a spectrum has no clear breaks.\n");
            binProfileSpec(&result, &diffs, &cumDiffs, {knownStart, knownEnd});
        }

        size_t maxEntry = 0; // this is the maximum size of all blocks

        // ProfileBlock entry = initBlock(64);

        // transfer the found groups into a representation accepted by the peak model fit
        for (size_t j = 0; j < result.size(); j++)
        {
            Range_i res = result[j];

            size_t entrySize = res.endIdx + 1 - res.startIdx + 4;
            maxEntry = maxEntry > entrySize ? maxEntry : entrySize;

            // clearBlock(&entry, entrySize); // @todo prevent a new block from being created every time
            ProfileBlock entry = initBlock(entrySize);

            for (size_t i = 0; i < entrySize - 4; i++)
            {
                size_t epos = i + res.startIdx; // element position
                size_t rpos = i + 2;            // result vector position
                entry.intensity[rpos] = intensities_profile[epos];
            }
            for (size_t i = 0; i < entrySize - 4; i++)
            {
                size_t epos = i + res.startIdx; // element position
                size_t rpos = i + 2;            // result vector position
                entry.mz[rpos] = mz_profile[epos];
            }

            // extrapolate two points to each size of the entry. The data has to be clamped to a minimum of 1 for log transform to work
            {
                entry.intensity[1] = entry.intensity[2] / 2;
                // entry.intensity[1] = entry.intensity[1] < 1 ? 1 : entry.intensity[1];
                entry.intensity[0] = entry.intensity[2] / 4;
                // entry.intensity[0] = entry.intensity[0] < 1 ? 1 : entry.intensity[0];

                size_t back1 = entrySize - 2;
                size_t back2 = entrySize - 1;
                entry.intensity[back1] = entry.intensity[entrySize - 3] / 2;
                // entry.intensity[back1] = entry.intensity[back1] < 1 ? 1 : entry.intensity[back1];
                entry.intensity[back2] = entry.intensity[entrySize - 3] / 4;
                // entry.intensity[back2] = entry.intensity[back2] < 1 ? 1 : entry.intensity[back2];

                double mzDiffFront = entry.mz[3] - entry.mz[2]; // @todo should there be two different distances?
                entry.mz[1] = entry.mz[2] - mzDiffFront;
                entry.mz[0] = entry.mz[2] - mzDiffFront * 2;

                double mzDiffBack = entry.mz[entrySize - 3] - entry.mz[entrySize - 4];
                entry.mz[entrySize - 2] = entry.mz[entrySize - 3] + mzDiffBack;
                entry.mz[entrySize - 1] = entry.mz[entrySize - 3] + mzDiffBack * 2;
            }

            // add traceability information from untreated spectrum
            entry.startPos = idxConvert[res.startIdx];
            entry.endPos = idxConvert[res.endIdx];

            groupedData->push_back(entry);
        }
        if (groupedData->empty())
            return 0;

        return maxEntry;
    }

    double regAt_L(const RegCoeffs *coeff, const double x)
    {
        return coeff->b0 + (coeff->b1 + x * coeff->b2) * x;
    }
    double regAt_R(const RegCoeffs *coeff, const double x)
    {
        return coeff->b0 + (coeff->b1 + x * coeff->b3) * x;
    }
    double regExpAt_L(const RegCoeffs *coeff, const double x)
    {
        return exp_approx_d(coeff->b0 + (coeff->b1 + x * coeff->b2) * x);
    }
    double regExpAt_R(const RegCoeffs *coeff, const double x)
    {
        return exp_approx_d(coeff->b0 + (coeff->b1 + x * coeff->b3) * x);
    }
}