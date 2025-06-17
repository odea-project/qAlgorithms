#include "qalgorithms_qpeaks.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_datatypes.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <vector>
#include <iostream>
#include <immintrin.h> // AVX
#include <numeric>     // accumulate, this is temporary @todo

namespace qAlgorithms
{

    constexpr auto INV_ARRAY = initialize(); // this only works with constexpr square roots, which are part of C++26

#pragma region "find peaks"
    void findCentroidPeaks(std::vector<CentroidPeak> *retPeaks, // results are appended to this vector
                           const std::vector<ProfileBlock> *treatedData,
                           const size_t scanNumber,
                           const size_t accessor)
    {
        assert(!treatedData->empty());
        assert(scanNumber != 0);
        size_t maxWindowSize = 0;
        for (size_t i = 0; i < treatedData->size(); i++)
        {
            size_t length = (*treatedData)[i].df.size();
            assert(length > 4); // data must contain at least five points
            maxWindowSize = maxWindowSize < length ? length : maxWindowSize;
        }
        assert(maxWindowSize > 0);
        std::vector<float> logIntensity(maxWindowSize, NAN);

        const size_t GLOBAL_MAXSCALE_CENTROID = 8; // @todo this is a critical part of the algorithm and should not be hard-coded
        assert(GLOBAL_MAXSCALE_CENTROID <= MAXSCALE);

        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(treatedData->size() / 2); // probably too large, shouldn't matter
        for (size_t i = 0; i < treatedData->size(); i++)
        {
            const auto block = (*treatedData)[i];
            const size_t length = block.df.size();
            assert(length > 4);

            logIntensity.resize(length); // this way, no new allocations will be made in the loop
            for (size_t blockPos = 0; blockPos < length; blockPos++)
            {
                logIntensity[blockPos] = std::log(block.intensity[blockPos]);
            }

            // @todo adjust the scale dynamically based on the number of valid regressions found, early terminate after x iterations
            const size_t maxScale = std::min(GLOBAL_MAXSCALE_CENTROID, size_t((length - 1) / 2)); // length - 1 because the center point is not part of the span

            runningRegression(&block.intensity, &logIntensity, &block.df, validRegressions, maxScale);
            if (validRegressions.empty())
            {
                continue; // no valid peaks
            }
            createCentroidPeaks(retPeaks, &validRegressions, &block, scanNumber, accessor);
            validRegressions.clear();
        }
    }

    void findFeatures(std::vector<FeaturePeak> &all_peaks,
                      treatedData &treatedData)
    {
        size_t length = treatedData.dataPoints.size();
        assert(length > 4); // data must contain at least five points

        std::vector<float> logIntensity(length, NAN);

        float *RT = new float[length];

        static const size_t GLOBAL_MAXSCALE_FEATURES = 30;
        // @todo this is not a universal limit and only chosen for computational speed at the moment
        // with an estimated scan difference of 0.6 s this means the maximum peak width is 61 * 0.6 = 36.6 s
        assert(GLOBAL_MAXSCALE_FEATURES <= MAXSCALE);

        std::vector<bool> degreesOfFreedom;
        degreesOfFreedom.reserve(length);
        std::vector<float> intensity;
        intensity.reserve(length);
        std::vector<float> RT_new;
        RT_new.reserve(length);
        for (size_t position = 0; position < length; position++)
        {
            size_t idx = position;
            intensity.push_back(treatedData.dataPoints[idx].y);
            RT[position] = treatedData.dataPoints[idx].x;
            RT_new.push_back(treatedData.dataPoints[idx].x);
            degreesOfFreedom.push_back(treatedData.dataPoints[idx].df);
        }

        logIntensity.resize(intensity.size());
        for (size_t blockPos = 0; blockPos < intensity.size(); blockPos++)
        {
            logIntensity[blockPos] = std::log(intensity[blockPos]);
        }

        std::vector<RegressionGauss> validRegressions;
        size_t maxScale = std::min(GLOBAL_MAXSCALE_FEATURES, size_t((length - 1) / 2));
        runningRegression(&intensity, &logIntensity, &degreesOfFreedom, validRegressions, maxScale);
        if (!validRegressions.empty())
        {
            createFeaturePeaks(&all_peaks, &validRegressions, &RT_new, RT);
            // there is no reason for this to be called here and not later @todo
        }
        delete[] RT;
    }
#pragma endregion "find peaks"

#pragma region "running regression"
    void runningRegression(
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        const std::vector<bool> *degreesOfFreedom,
        std::vector<RegressionGauss> &validRegressions,
        const size_t maxScale)
    {
        //@todo move more of the generic stuff into this function
        assert(validRegressions.empty());

        const std::vector<RegCoeffs> regressions = findCoefficients(intensities_log, maxScale);

        validateRegression(&regressions, intensities, intensities_log, degreesOfFreedom, maxScale, validRegressions);

        if (validRegressions.size() > 1) // @todo we can probably filter regressions based on MSE at this stage already
        {
            // number of competitors is intialised to 0, so no special case for size = 1 needed
            // there can be 0, 1 or more than one regressions in validRegressions
            validRegressions = mergeRegressionsOverScales(validRegressions, intensities);
        }
        return;
    }

    std::vector<RegCoeffs> restoreShape(const std::vector<RegCoeffs> *input,
                                        const std::vector<size_t> *scaleCount,
                                        const size_t numPoints,
                                        const size_t maxScale)
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

        std::vector<RegCoeffs> results(input->size(), {0, 0, 0, 0}); // checks for coefficients != 0 exist in the rest of the code
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
                auto current = input->at(counter);

                results[idx + offset] = current;

                // offset = offset - scale at iteration 0 + the number of points at that scale + 1
                offset += -1 + numPoints - 2 * scale_i;
                counter++;
            }
        }
        return results;
    }

    std::vector<RegCoeffs> findCoefficients(
        const std::vector<float> *intensity_log,
        const size_t max_scale) // maximum scale that will be checked. Should generally be limited by peakFrame
    {
        /*
  This function performs a convolution with the kernel: (xTx)^-1 xT and the data array: intensity_log.
  (xTx)^-1 is pre_calculated and stored in the inv_array and contains 7 unique values per scale.
  xT is the transpose of the design matrix X that looks like this:
  for scale = 2:
  xT = | 1  1  1  1  1 |    : all ones
       |-2 -1  0  1  2 |    : from -scale to scale
       | 4  1  0  0  0 |    : x^2 values for x < 0
       | 0  0  0  1  4 |    : x^2 values for x > 0

  It contains one additional row of all ones for every additional peak that is added into the model

  When adding multiple peaks to the regression model, we need to adjust the inverse values.
  This will change the number of unique values in the inv_values array from 6 to 7.
  Here we use the inv_array[1] position and shift all values from that point onwards to the right.
  example for num_peaks = 2:
  original matrix with the unique values [a, b, c, d, e, f] (six unique values)
  | a  0  b  b |
  | 0  c  d -d |
  | b  d  e  f |
  | b -d  f  e |

  new matrix with the unique values [A1, A2, B, C, D, E, F] (seven unique values)
  | A1  A2  0  B  B |
  | A2  A1  0  B  B |
  | 0   0   C  D -D |
  | B   B   D  E  F |
  | B   B  -D  F  E |

  for num_peaks = 3:
  new matrix with the unique values [A1, A2, B, C, D, E, F] (the same seven unique values)
  | A1  A2  A2  0  B  B |
  | A2  A1  A2  0  B  B |
  | A2  A2  A1  0  B  B |
  | 0   0   0   C  D -D |
  | B   B   B   D  E  F |
  | B   B   B  -D  F  E |

  Note that no more than seven different values are needed per scale.

  In general, we have two moving actions:
  1) step right through the intensity_log and calculate the convolution with the kernel
  2) expand the kernel to the left and right of the intensity_log (higher scale)

  The workflow is organized in nested loops:
  1) outer loop: move along the intensity_log AND calculate the convolution with the scale=2 kernel
  2) inner loop: expand the kernel to the left and right of the intensity_log (higher scale)

  The pattern used for both loops looks like:
  e.g. for n(y) = 16
  outer loop: i = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
  inner loop: range from:
   for i=0: 3 to 3 => i.e. loop is not executed
   for i=1: 3 to 4 => i.e. loop is executed once for scale=3
   for i=2: 3 to 5 => i.e. loop is executed twice for scale=3,4
   for i=3: 3 to 6 => i.e. loop is executed three times for scale=3,4,5
   for i=4: 3 to 7 => i.e. loop is executed four times for scale=3,4,5,6
   for i=5: 3 to 8 => i.e. loop is executed five times for scale=3,4,5,6,7
   for i=6: 3 to 8 => i.e. loop is executed five times for scale=3,4,5,6,7
   for i=7: 3 to 7 => i.e. loop is executed four times for scale=3,4,5,6
   for i=8: 3 to 6 => i.e. loop is executed three times for scale=3,4,5
   for i=9: 3 to 5 => i.e. loop is executed twice for scale=3,4
   for i=10: 3 to 4 => i.e. loop is executed once for scale=3
   for i=11: 3 to 3 => i.e. loop is not executed
   */
        assert(max_scale > 1);
        assert(max_scale <= MAXSCALE);
        const size_t minScale = 2;
        const size_t steps = intensity_log->size() - 2 * minScale; // iteration number at scale 2

        auto y_array_sum_rm = intensity_log; // sum of each index across multiple dimensions of intensity_log
        // only relevant for more than one peak

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

        // these arrays contain all coefficients for every loop iteration
        std::vector<double> beta_0(iterationCount, NAN);
        std::vector<double> beta_1(iterationCount, NAN);
        std::vector<double> beta_2(iterationCount, NAN);
        std::vector<double> beta_3(iterationCount, NAN);

        // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
        // The first n entries are contained in the b0 vector, one for each peak the regression is performed
        // over.
        double tmp_product_sum_b0;
        double tmp_product_sum_b1;
        double tmp_product_sum_b2;
        double tmp_product_sum_b3;

        size_t k = 0;
        for (size_t i = 0; i < steps; i++)
        {
            // move along the intensity_log (outer loop)
            // calculate the convolution with the kernel of the lowest scale (= 2), i.e. xT * intensity_log[i:i+4]
            // tmp_product_sum_b0_vec = intensity_log[i:i+5].sum(axis=0) // numPeaks rows of xT * intensity_log[i:i+4]

            tmp_product_sum_b0 = intensity_log->at(i) + intensity_log->at(i + 1) +
                                 intensity_log->at(i + 2) + intensity_log->at(i + 3) +
                                 intensity_log->at(i + 4); // b0 = 1 for all elements
            tmp_product_sum_b1 = 2 * (y_array_sum_rm->at(i + 4) - y_array_sum_rm->at(i)) +
                                 y_array_sum_rm->at(i + 3) - y_array_sum_rm->at(i + 1);
            tmp_product_sum_b2 = 4 * y_array_sum_rm->at(i) + y_array_sum_rm->at(i + 1);
            tmp_product_sum_b3 = 4 * y_array_sum_rm->at(i + 4) + y_array_sum_rm->at(i + 3);

            // use [12 + ...] since the array is constructed for the accession arry[scale * 6 + (0:5)]
            // @todo replace the array with a struct and an accessor function
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

            k += 1;       // update index for the productsums array
            size_t u = 1; // u is the expansion increment
            for (size_t scale = 3; scale < maxInnerLoop[i] + 1; scale++)
            { // minimum scale is 2. so we start with scale + 1 = 3 in the inner loop
                size_t scale_sqr = scale * scale;
                // expand the kernel to the left and right of the intensity_log.
                tmp_product_sum_b0 += intensity_log->at(i - u) + intensity_log->at(i + 4 + u);
                tmp_product_sum_b1 += scale * (y_array_sum_rm->at(i + 4 + u) - y_array_sum_rm->at(i - u));
                tmp_product_sum_b2 += scale_sqr * y_array_sum_rm->at(i - u);
                tmp_product_sum_b3 += scale_sqr * y_array_sum_rm->at(i + 4 + u);

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

                u += 1; // update expansion increment
                k += 1; // update index for the productsums array
            }
        }

        assert(beta_0.size() == beta_1.size());

        std::vector<RegCoeffs> coeffs;
        coeffs.reserve(beta_1.size());
        for (size_t i = 0; i < beta_1.size(); i++)
        {
            coeffs.push_back({float(beta_0[i]), float(beta_1[i]), float(beta_2[i]), float(beta_3[i])});
        }

        // @todo why not save the scale information as part of the coefficient struct and then use that for the whole merging?
        coeffs = restoreShape(&coeffs, &maxInnerLoop, intensity_log->size(), max_scale);

        return coeffs;
    }

#pragma endregion "running regression"

#pragma region "validate Regression"
    void validateRegression(
        const std::vector<RegCoeffs> *coeffs, // coefficients for single-b0 peaks, spans all regressions over a peak window
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        const std::vector<bool> *degreesOfFreedom,
        const size_t maxScale, // scale, i.e., the number of data points in a half window excluding the center point
        std::vector<RegressionGauss> &validRegressions)
    {
        const size_t numPoints = intensities->size();

        std::vector<RegressionGauss> validRegsTmp; // temporary vector to store valid regressions
        // the start index of the regression is the same as the index in beta. The end index is at 2*scale + index in beta.
        validRegsTmp.push_back(RegressionGauss{});

        int currentScale = 2;
        size_t idxStart = 0;
        bool stillValid = true;
        for (size_t range = 0; range < coeffs->size(); range++)
        {
            size_t df = calcDF(degreesOfFreedom, idxStart, 2 * currentScale + idxStart);

            if (df < 5) // @todo cumsum
            {
                stillValid = false;
            }
            auto coeff = coeffs->at(range);
            if ((coeff.b1 == 0.0f) | (coeff.b2 == 0.0f) | (coeff.b3 == 0.0f))
            {
                // None of these are a valid regression with the asymmetric model
                stillValid = false;
            }
            if (stillValid)
            {
                validRegsTmp.back().coeffs = coeff;
                // the total span of the regression may not exceed the number of points
                assert(idxStart + 2 * currentScale < numPoints);

                makeValidRegression(&validRegsTmp.back(), idxStart, currentScale, degreesOfFreedom, intensities, intensities_log);
                if (validRegsTmp.back().isValid)
                {
                    validRegsTmp.push_back(RegressionGauss{});
                }
            }
            stillValid = true;
            idxStart++;
            // for every set of scales, execute the validation + in-scale merge operation
            // early termination needed if maxscale is reached, since here idxStart is 1 and the compared value 0
            if ((idxStart == numPoints - 2 * currentScale)) //|| (currentScale == maxScale))
            {
                // no valid peaks if the size of validRegsTemp is 1
                if (validRegsTmp.size() == 2)
                {
                    // only one valid peak, no fitering necessary
                    validRegressions.push_back(std::move(validRegsTmp[0]));
                }
                else if (validRegsTmp.size() > 2)
                {
                    // @todo both of these blocks could be grouped into their own function here
                    // remove the last regression, since it is always empty
                    validRegsTmp.pop_back();
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
                    std::vector<int> startEndGroups;
                    startEndGroups.reserve(validRegsTmp.size());

                    size_t prev_i = 0;

                    for (size_t i = 0; i < validRegsTmp.size() - 1; i++)
                    {
                        // check if the difference between two peak apexes is less than 4 (Nyquist Shannon
                        // Sampling Theorem, separation of two maxima), or if the apex of a peak is within
                        // the window of the other peak (Overlap of two maxima)
                        if (std::abs(validRegsTmp[i].apex_position - validRegsTmp[i + 1].apex_position) > 4 &&
                            validRegsTmp[i].apex_position < validRegsTmp[i + 1].left_limit &&
                            validRegsTmp[i + 1].apex_position > validRegsTmp[i].right_limit)
                        {
                            // the two regressions differ, i.e. create a new group
                            startEndGroups.push_back(prev_i);
                            startEndGroups.push_back(i);
                            prev_i = i + 1;
                        }
                    }
                    startEndGroups.push_back(prev_i);
                    startEndGroups.push_back(validRegsTmp.size() - 1); // last group ends with index of the last element

                    /*
                      Survival of the Fittest Filter:
                      This block of code implements the survival of the fittest filter. It selects the peak with
                      the lowest mean squared error (MSE) as the representative of the group. If the group contains
                      only one peak, the peak is directly pushed to the valid regressions. If the group contains
                      multiple peaks, the peak with the lowest MSE is selected as the representative of the group
                      and pushed to the valid regressions.
                    */
                    // @todo use a "ridges" approach here (gaussian mixture model)
                    for (size_t groupIdx = 0; groupIdx < startEndGroups.size(); groupIdx += 2)
                    {
                        if (startEndGroups[groupIdx] == startEndGroups[groupIdx + 1])
                        { // already isolated peak => push to valid regressions
                            int regIdx = startEndGroups[groupIdx];
                            validRegressions.push_back(std::move(validRegsTmp[regIdx]));
                        }
                        else
                        { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
                            assert(startEndGroups[groupIdx] != startEndGroups[groupIdx + 1]);
                            auto bestRegIdx = findBestRegression(intensities, &validRegsTmp, degreesOfFreedom,
                                                                 startEndGroups[groupIdx], startEndGroups[groupIdx + 1]);

                            RegressionGauss bestReg = validRegsTmp[bestRegIdx.idx];
                            bestReg.mse = bestRegIdx.mse;
                            validRegressions.push_back(std::move(bestReg));
                        }
                    } // end for loop (group in vector of groups)
                }

                // reset loop
                currentScale++;
                idxStart = 0;
                validRegsTmp.clear();
                validRegsTmp.push_back(RegressionGauss{});
            }
        }
    }

    void makeValidRegression(
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const std::vector<bool> *degreesOfFreedom,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log)
    {
        assert(scale > 1);
        assert(idxStart + 4 < degreesOfFreedom->size());
        assert(mutateReg->coeffs.b0 < 100 && mutateReg->coeffs.b0 > -100);
        assert(mutateReg->coeffs.b1 < 100 && mutateReg->coeffs.b1 > -100);
        assert(mutateReg->coeffs.b2 < 100 && mutateReg->coeffs.b2 > -100);
        assert(mutateReg->coeffs.b3 < 100 && mutateReg->coeffs.b3 > -100);
        /*
          Apex and Valley Position Filter:
          This block of code implements the apex and valley position filter.
          It calculates the apex and valley positions based on the coefficients
          matrix B. If the apex is outside the data range, the loop continues
          to the next iteration. If the apex and valley positions are too close
          to each other, the loop continues to the next iteration.
        */
        float valley_position = 0;
        // no easy replace
        if (!calcApexAndValleyPos(mutateReg, scale, valley_position))
        {
            return; // invalid apex and valley positions
        }
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

        if (valley_position == 0) [[likely]]
        {
            // no valley point exists
            mutateReg->left_limit = idxStart;
            mutateReg->right_limit = idxStart + 2 * scale;
        }
        else if (valley_position < 0)
        {
            size_t substractor = static_cast<size_t>(abs(valley_position));
            mutateReg->left_limit = substractor < scale ? idxStart + scale - substractor : idxStart; // std::max(i, static_cast<int>(valley_position) + i + scale);
            mutateReg->right_limit = idxStart + 2 * scale;
        }
        else
        {
            mutateReg->left_limit = idxStart;
            mutateReg->right_limit = std::min(idxStart + 2 * scale, static_cast<int>(valley_position) + idxStart + scale);
        }
        assert(mutateReg->right_limit < intensities->size());
        const size_t idx_x0 = idxStart + scale;

        /*
            Note: left and right limit are not the limits of the regression, but of the window the regression applies in.
            When multiple regressions are combined, the window limits are combined by maximum.
        */
        if (idx_x0 - mutateReg->left_limit < 2 || (mutateReg->right_limit - idx_x0 < 2))
        {
            // only one half of the regression applies to the data, since the
            // degrees of freedom for the "squished" half results in an invalid regression
            return;
        }

        /*
          Degree of Freedom Filter:
          This block of code implements the degree of freedom filter. It calculates the
          degree of freedom based df vector. If the degree of freedom is less than 5,
          the loop continues to the next iteration. The value 5 is chosen as the
          minimum number of data points required to fit a quadratic regression model.
        */
        size_t df_sum = calcDF(degreesOfFreedom, mutateReg->left_limit, mutateReg->right_limit); // degrees of freedom considering the left and right limits @todo cumulative array
        if (df_sum < 5)
        {
            return; // degree of freedom less than 5; i.e., less then 5 measured data points
        }
        // assert(mutateReg->right_limit - mutateReg->left_limit > 4);

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates
          the ratio of the apex signal to the edge signal and ensures that the
          ratio is greater than 2. This is a pre-filter for later
          signal-to-noise ratio checkups. apexToEdge is also required in isValidPeakHeight further down
        */
        //         size_t idx_apex = (size_t)std::round(apex_position) + idx_x0;
        size_t idxApex = (size_t)std::round(mutateReg->apex_position) + idx_x0;
        float apexToEdge = apexToEdgeRatio(mutateReg->left_limit, idxApex, mutateReg->right_limit, intensities);
        if (!(apexToEdge > 2))
        {
            return; // invalid apex to edge ratio
        }

        /*
          Quadratic Term Filter:
          This block of code implements the quadratic term filter. It calculates
          the mean squared error (MSE) between the predicted and actual values.
          Then it calculates the t-value for the quadratic term. If the t-value
          is less than the corresponding value in the T_VALUES, the quadratic
          term is considered statistically insignificant, and the loop continues
          to the next iteration.
        */

        std::vector<float> selectLog; // both vetors are used to transfer relevant values to the F test later
        std::vector<float> predictLog;
        selectLog.reserve(mutateReg->right_limit - mutateReg->left_limit + 1);
        predictLog.reserve(mutateReg->right_limit - mutateReg->left_limit + 1);
        float mse = calcSSE_base(mutateReg->coeffs, intensities_log, &selectLog, &predictLog,
                                 mutateReg->left_limit, mutateReg->right_limit, idx_x0);

        /*
        competing regressions filter:
        If the real distribution of points could also be described as a continuum (i.e. only b0 is relevant),
        the regression does not describe a peak. This is done through a nested F-test against a constant that
        is the mean of all predicted values. @todo this is not working correctly!
        */
        float regression_Fval = calcRegressionFvalue(&selectLog, &predictLog, mse, mutateReg->coeffs.b0);
        if (regression_Fval < F_VALUES[selectLog.size()]) // - 5 since the minimum is five degrees of freedom
        {
            // H0 holds, the two distributions are not noticeably different
            return;
        }
        // mse is only the correct mean square error after this division
        mse /= (df_sum - 4);

        if (!isValidQuadraticTerm(mutateReg->coeffs, scale, mse, df_sum))
        {
            return; // statistical insignificance of the quadratic term
        }
        if (!isValidPeakArea(mutateReg->coeffs, mse, scale, df_sum))
        {
            return; // statistical insignificance of the area
        }
        /*
          Height Filter:
          This block of code implements the height filter. It calculates the height
          of the peak based on the coefficients matrix B. Then it calculates the
          uncertainty of the height based on the Jacobian matrix and the variance-covariance
          matrix of the coefficients. If the height is statistically insignificant,
          the loop continues to the next iteration.
        */

        calcPeakHeightUncert(mutateReg, mse, scale);                   // @todo independent of b0
        if (1 / mutateReg->uncertainty_height <= T_VALUES[df_sum - 5]) // statistical significance of the peak height
        {
            return;
        }
        // at this point without height, i.e., to get the real uncertainty
        // multiply with height later. This is done to avoid exp function at this point
        if (!isValidPeakHeight(mse, scale, mutateReg->apex_position, valley_position, df_sum, apexToEdge))
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
        calcPeakAreaUncert(mutateReg, mse, scale);

        if (mutateReg->area / mutateReg->uncertainty_area <= T_VALUES[df_sum - 5])
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
        float chiSquare = calcSSE_chisqared(mutateReg->coeffs, intensities, mutateReg->left_limit, mutateReg->right_limit, idx_x0);
        if (chiSquare < CHI_SQUARES[df_sum - 5])
        {
            return; // statistical insignificance of the chi-square value
        }

        /*
          Smearing Correction:
          The coefficient beta_0 is corrected by the smearing approach from Naihua Duan.
          The new cofficient is then b0* = b0 + logC, where C is the correction factor.
          first: logC; second: variance of logC
        */
        std::pair<float, float> smearing = smearingCorrection(&predictLog, &selectLog, scale);
        mutateReg->coeffs.b0 += smearing.first; // b0* = b0 + logC
        // @todo: implement smearing.second for the uncertainty of b0

        mutateReg->uncertainty_pos = calcUncertaintyPos(mse, mutateReg->coeffs, mutateReg->apex_position, scale);
        mutateReg->df = df_sum - 4; // @todo add explanation for -4
        mutateReg->apex_position += idxStart + scale;
        mutateReg->scale = scale;
        mutateReg->index_x0 = idx_x0;
        mutateReg->mse = mse; // the quadratic mse is used for the weighted mean of the coefficients later
        mutateReg->isValid = true;
        return;
    }
#pragma endregion "validate Regression"

    std::vector<RegressionGauss> mergeRegressionsOverScales(std::vector<RegressionGauss> validRegressions,
                                                            const std::vector<float> *intensities)
    {
        /*
          Grouping Over Scales:
          This block of code implements the grouping over scales. It groups the valid
          peaks based on the apex positions. Peaks are defined as similar, i.e.,
          members of the same group, if they fullfill at least one of the following conditions:
          - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
          - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
        */

        // iterate over the validRegressions vector
        for (size_t i = 0; i < validRegressions.size(); i++)
        {
            assert(validRegressions[i].isValid);
            const unsigned int left_limit = validRegressions[i].left_limit;   // left limit of the current peak regression window in the Y array
            const unsigned int right_limit = validRegressions[i].right_limit; // right limit of the current peak regression window in the Y array
            double MSE_group = 0;
            int DF_group = 0;
            // only calculate required MSEs since this is one of the performance-critical steps
            std::vector<float> exponentialMSE(validRegressions.size(), 0);
            std::vector<size_t> validRegressionsInGroup; // vector of indices to validRegressions
            size_t competitors = 0;                      // this variable keeps track of how many competitors a given regression has

            // iterate over the validRegressions vector till the new peak
            // first iteration always false
            for (size_t j = 0; j < i; j++)
            {
                if (validRegressions[j].isValid) // check is needed because regressions are set to invalid in the outer loop
                {
                    if ( // check for the overlap of the peaks
                        (
                            validRegressions[j].apex_position > left_limit &&   // ref peak matches the left limit
                            validRegressions[j].apex_position < right_limit) || // ref peak matches the right limit
                        (
                            validRegressions[i].apex_position > validRegressions[j].left_limit && // new peak matches the left limit
                            validRegressions[i].apex_position < validRegressions[j].right_limit)) // new peak matches the right limit
                    {
                        if (exponentialMSE[j] == 0.0)
                        { // calculate the mse of the reference peak
                            exponentialMSE[j] = calcSSE_exp(
                                validRegressions[j].coeffs,
                                intensities,
                                validRegressions[j].left_limit,
                                validRegressions[j].right_limit,
                                validRegressions[j].index_x0);
                            exponentialMSE[j] /= validRegressions[j].df;
                        }
                        DF_group += validRegressions[j].df;                      // add the degree of freedom
                        MSE_group += exponentialMSE[j] * validRegressions[j].df; // add the sum of squared errors
                        // add the iterator of the ref peak to a vector of iterators
                        validRegressionsInGroup.push_back(j);
                        competitors += validRegressions[j].numCompetitors + 1; // a regression can have beaten a previous one
                    }
                }
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
                    validRegressions[i].coeffs,
                    intensities,
                    left_limit,
                    right_limit,
                    validRegressions[i].index_x0);
                exponentialMSE[i] /= validRegressions[i].df;
            }
            if (exponentialMSE[i] < MSE_group)
            {
                // Set isValid to false for the candidates from the group
                for (size_t it_ref_peak : validRegressionsInGroup)
                {
                    validRegressions[it_ref_peak].isValid = false;
                }
                // only advance competitor count if regression is actually better
                validRegressions[i].numCompetitors = competitors;
            }
            else
            { // Set isValid to false for the current peak
                validRegressions[i].isValid = false;
            }
        } // end for loop, outer loop, it_current_peak
        std::vector<RegressionGauss> finalRegs;
        for (size_t i = 0; i < validRegressions.size(); i++)
        {
            if (validRegressions[i].isValid)
            {
                finalRegs.push_back(validRegressions[i]);
            }
        }
        return finalRegs;
    }
#pragma endregion "validate regression"

#pragma region "create peaks"

    MeanVar weightedMeanAndVariance(const float *values,
                                    const std::vector<float> *weight,
                                    size_t left_limit,
                                    size_t right_limit)
    {
        // weighted mean using intensity as weighting factor and left_limit right_limit as range
        size_t realPoints = right_limit - left_limit + 1;
        double mean_weights = 0.0;   // mean of weight
        double sum_weighted_x = 0.0; // sum of values * weight
        double sum_weight = 0.0;     // sum of weight
        for (size_t j = left_limit; j <= right_limit; j++)
        {
            if (values[j] != 0)
            {
                mean_weights += (*weight)[j];
                sum_weighted_x += values[j] * (*weight)[j];
                sum_weight += (*weight)[j];
            }
            else
            {
                realPoints--;
            }
        }
        mean_weights /= realPoints;
        sum_weighted_x /= mean_weights;
        sum_weight /= mean_weights;

        double weighted_mean = sum_weighted_x / sum_weight;
        double sum_Qxxw = 0.0; // sum of (values - mean)^2 * weight
        for (size_t j = left_limit; j <= right_limit; j++)
        {
            if (values[j] <= 0.f)
            {
                continue;
            }
            sum_Qxxw += (values[j] - weighted_mean) * (values[j] - weighted_mean) * (*weight)[j];
        }
        float uncertaintiy = std::sqrt(sum_Qxxw / sum_weight / realPoints);
        return {float(weighted_mean), float(uncertaintiy)};
    };

    MeanVar weightedMeanAndVariance_EIC(const std::vector<float> *weight,
                                        const std::vector<float> *values,
                                        size_t left_limit,
                                        size_t right_limit)
    {
        // weighted mean using intensity as weighting factor and left_limit right_limit as range
        size_t realPoints = right_limit - left_limit + 1;
        double mean_weights = 0.0;   // mean of weight
        double sum_weighted_x = 0.0; // sum of values * weight
        double sum_weight = 0.0;     // sum of weight
        for (size_t j = left_limit; j <= right_limit; j++)
        {
            mean_weights += (*weight)[j];
            sum_weighted_x += (*values)[j] * (*weight)[j];
            sum_weight += (*weight)[j];
        }
        mean_weights /= realPoints;
        sum_weighted_x /= mean_weights;
        sum_weight /= mean_weights;

        double weighted_mean = sum_weighted_x / sum_weight;
        double sum_Qxxw = 0.0; // sum of (values - mean)^2 * weight
        for (size_t j = left_limit; j <= right_limit; j++)
        {
            sum_Qxxw += ((*values)[j] - weighted_mean) * ((*values)[j] - weighted_mean) * (*weight)[j];
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
    {
        assert(!validRegressionsVec->empty());
        // iterate over the validRegressions vector
        for (size_t i = 0; i < validRegressionsVec->size(); i++)
        {
            RegressionGauss regression = (*validRegressionsVec)[i];
            assert(regression.isValid);
            CentroidPeak peak;
            peak.scanNumber = scanNumber;
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

            // peak.interpolations = regression.right_limit - regression.left_limit + 1 - regression.df - 4; // -4 since four coefficients take up degrees of freedom

            /// @todo consider adding these properties so we can trace back everything completely
            // peak.idxPeakStart = regression.left_limit;
            // peak.idxPeakEnd = regression.right_limit;
            peaks->push_back(std::move(peak));
        }
    }

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const std::vector<float> *RT,
        const float *rt_start)
    {
        assert(!validRegressionsVec->empty());
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

            // size_t apexIdx = (size_t)std::floor(regression.apex_position);
            const double rt_leftOfMax = *(rt_start + (int)std::floor(regression.apex_position)); // left of maximum
            const double delta_rt = *(rt_start + (int)std::floor(regression.apex_position) + 1) - rt_leftOfMax;
            assert(delta_rt > 0);
            const double apex_position = rt_leftOfMax + delta_rt * (regression.apex_position - std::floor(regression.apex_position));
            peak.retentionTime = apex_position;
            peak.retentionTimeUncertainty = regression.uncertainty_pos * delta_rt;
            assert(peak.retentionTimeUncertainty > 0);

            // add area
            float exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression.area * exp_b0 * delta_rt;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0 * delta_rt;

            // mz, mzUncertainty, mean DQSC and meanDQSF are all calculated in after this function is called in measurement_data
            peak.DQSF = 1 - erf_approx_f(regression.uncertainty_area / regression.area);

            assert(regression.right_limit - regression.index_x0 > 1);
            peak.idxPeakStart = regression.left_limit;
            peak.idxPeakEnd = regression.right_limit - 1;
            peak.index_x0_offset = regression.index_x0 - regression.left_limit;
            assert(peak.idxPeakEnd > peak.idxPeakStart);
            assert(peak.idxPeakEnd > peak.index_x0_offset);

            // params needed to merge two peaks
            peak.apexLeft = regression.apex_position < regression.index_x0;
            coeff.b1 /= delta_rt;
            coeff.b2 /= delta_rt * delta_rt;
            coeff.b3 /= delta_rt * delta_rt;
            peak.coefficients = coeff;
            peak.mse_base = regression.mse;

            peak.scale = regression.scale;
            peak.interpolationCount = regression.right_limit - regression.left_limit - regression.df - 4; // -4 since the degrees of freedom are reduced by 1 per coefficient
            peak.competitorCount = regression.numCompetitors;

            peaks->push_back(std::move(peak));
        }
    }
#pragma endregion "create peaks"

#pragma region calcSSE

    float calcSSE_base(const RegCoeffs coeff,
                       const std::vector<float> *y_start,
                       std::vector<float> *selectLog,
                       std::vector<float> *predictLog,
                       size_t limit_L,
                       size_t limit_R,
                       size_t index_x0)
    {
        // this function also populates some variables for the F test later in the validation
        double result = 0.0;
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x;
            double y_current = y_start->at(iSegment);
            selectLog->push_back(y_current);
            predictLog->push_back(y_base);
            double newdiff = (y_current - y_base) * (y_current - y_base);

            result += newdiff;
        }
        // center point
        result += (y_start->at(index_x0) - coeff.b0) * (y_start->at(index_x0) - coeff.b0); // x = 0 -> (b0 - y)^2
        selectLog->push_back(y_start->at(index_x0));
        predictLog->push_back(coeff.b0);

        // right side
        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // iSegment = 0 is center point calculated above
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = coeff.b0 + (coeff.b1 + coeff.b3 * new_x) * new_x; // b3 instead of b2
            double y_current = y_start->at(iSegment);
            selectLog->push_back(y_current);
            predictLog->push_back(y_base);
            double newdiff = (y_current - y_base) * (y_current - y_base);

            result += newdiff;
        }
        return result;
    }

    float calcRegressionFvalue(const std::vector<float> *selectLog, const std::vector<float> *predictLog, const float sse, const float b0)
    {
        // note that the mse must not be divided by df - 4 yet when this function is called

        // this function returns the F-value for significance of the regression. H0 is that only beta 0
        // influences the result, meaning that the points that qualified for a regression are of a constant intensity.
        // Compare https://link.springer.com/book/10.1007/978-3-662-67526-7 p. 501 ff.

        size_t length = predictLog->size();
        assert(selectLog->size() == length);
        assert(sse > 0);
        double sum = 0;
        for (size_t i = 0; i < length; i++)
        {
            sum += (*selectLog)[i];
        }
        sum /= length;
        // assert(abs(sum - b0) < sse / length); // misunderstanding
        double squareSumModel = 0;
        for (size_t i = 0; i < length; i++)
        {
            squareSumModel += ((*selectLog)[i] - sum) * ((*selectLog)[i] - sum);
        }
        // const double R_sqared = 1 - sse / squareSumModel;
        // assert(R_sqared > 0);
        // assert(R_sqared < 1);
        // n - p - 1 / p, where p is always 3 because we have a four-coefficient system.
        // df_sum is used since only non-interpolated points count towards the regression accuracy
        const double factor = double(length - 3 - 1) / double(length - 1);

        return squareSumModel / sse * factor;
    }

    float calcSSE_exp(const RegCoeffs coeff, const std::vector<float> *y_start, size_t limit_L, size_t limit_R, size_t index_x0)
    { // @todo this does not account for asymmetric RT distances, will that be a problem?
        double result = 0.0;
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++) // @todo factor this loop into a function
        {
            double new_x = double(iSegment) - double(index_x0); // always negative
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_base) * (y_current - y_base);
            result += newdiff;
        }
        result += ((*y_start)[index_x0] - exp_approx_d(coeff.b0)) * ((*y_start)[index_x0] - exp_approx_d(coeff.b0)); // x = 0 -> (b0 - y)^2
        // right side
        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // start one past the center, include right limit index
        {
            double new_x = double(iSegment) - double(index_x0);                             // always positive
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * new_x) * new_x); // b3 instead of b2
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_base) * (y_current - y_base);
            result += newdiff;
        }
        return result;
    }

    float calcSSE_chisqared(const RegCoeffs coeff, const std::vector<float> *y_start,
                            size_t limit_L, size_t limit_R, size_t index_x0)
    {
        double result = 0.0;
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_base) * (y_current - y_base);
            result += newdiff / y_base;
        }
        // center point, x = 0 -> (b0 - y)^2
        double exp_b0 = exp_approx_d(coeff.b0);
        result += (((*y_start)[index_x0] - exp_b0) * ((*y_start)[index_x0] - exp_b0)) / exp_b0;

        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // iSegment = 0 is center point (calculated above)
        {
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * iSegment) * iSegment); // b3 instead of b2
            double y_current = (*y_start)[iSegment];
            double newdiff = (y_current - y_base) * (y_current - y_base);
            result += newdiff / y_base;
        }
        return result;
    }

#pragma endregion calcSSE

#pragma region "smearing correction"
    std::pair<float, float> smearingCorrection(
        const std::vector<float> *predictLog,
        const std::vector<float> *selectLog,
        const size_t scale)
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

        return {float(std::log(C)), float(varLogC)};
    }
#pragma endregion "smearing correction"

    RegPair findBestRegression(
        const std::vector<float> *intensities,
        const std::vector<RegressionGauss> *regressions,
        const std::vector<bool> *degreesOfFreedom,
        const size_t startIdx,
        const size_t endIdx)
    {
        double best_mse = INFINITY;
        unsigned int bestRegIdx = 0;

        // identify left (smallest) and right (largest) limit of the grouped regression windows
        unsigned int left_limit = (*regressions)[startIdx].left_limit;
        unsigned int right_limit = (*regressions)[startIdx].right_limit;
        for (size_t i = startIdx + 1; i < endIdx + 1; i++)
        {
            left_limit = std::min(left_limit, (*regressions)[i].left_limit);
            right_limit = std::max(right_limit, (*regressions)[i].right_limit);
        }
        // the new df_sum is only needed since the function limits are adjusted above, correct that?
        const size_t df_sum = calcDF(degreesOfFreedom, left_limit, right_limit); // @todo make cumulative array

        for (unsigned int i = startIdx; i < endIdx + 1; i++)
        {
            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            double mse = calcSSE_exp((*regressions)[i].coeffs,
                                     intensities,
                                     left_limit,
                                     right_limit,
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

    size_t calcDF( // using unsigned int is multiple seconds faster than size_1 for ten files in a row - why? @todo
        const std::vector<bool> *degreesOfFreedom,
        unsigned int left_limit,
        unsigned int right_limit)
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

#pragma region calcApexAndValleyPos
    bool calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        const size_t scale,
        float &valley_position)
    {
        // symmetric model should apply, this is not possible with the current peak model @todo
        assert(mutateReg->coeffs.b1 != 0);
        assert(mutateReg->coeffs.b2 != 0 && mutateReg->coeffs.b3 != 0);
        // calculate key by checking the signs of coeff
        const float floatScale = float(scale);
        int key = 0;
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

        // b1, b2, b3 are negative            ; 1 + 3 + 5 = 9
        // b2, b3 are negative, b1 is positive; 0 + 3 + 5 = 8
        // b1, b2 are negative, b3 is positive; 1 + 3 + 0 = 4
        // b1, b2 are positive, b3 is negative; 0 + 0 + 5 = 5
        enum keyCase
        {
            apexLeft_valleyNone = 9,
            apexRight_valleyNone = 8,
            apexLeft_valleyRight = 4,
            apexRight_valleyLeft = 5
        };

        float position_2 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b2);
        float position_3 = -mutateReg->coeffs.b1 / (2 * mutateReg->coeffs.b3);

        switch (key)
        {
        case apexLeft_valleyNone:
            mutateReg->apex_position = position_2; //-B1 / 2 / B2;  // is negative
            valley_position = 0;                   // no valley point
            return position_2 > -floatScale + 1;   // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyNone:                 // Case 1b: apex right
            mutateReg->apex_position = position_3; //-B1 / 2 / B3;     // is positive
            valley_position = 0;                   // no valley point
            return position_3 < floatScale - 1;    // scale -1: prevent apex position to be at the edge of the data

        case apexLeft_valleyRight:
            mutateReg->apex_position = position_2;                                   //-B1 / 2 / B2;      // is negative
            valley_position = position_3;                                            //-B1 / 2 / B3;      // is positive
            return position_2 > -floatScale + 1 && valley_position - position_2 > 2; // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyLeft:
            mutateReg->apex_position = position_3;                                  //-B1 / 2 / B3;       // is positive
            valley_position = position_2;                                           //-B1 / 2 / B2;       // is negative
            return position_3 < floatScale - 1 && position_3 - valley_position > 2; // scale -1: prevent apex position to be at the edge of the data

        default:
            return false; // invalid case
        }
    }
#pragma endregion calcApexAndValleyPos

#pragma region "isValidApexToEdge"

    float apexToEdgeRatio(
        const size_t idxStart,
        const size_t idxApex,
        const size_t idxEnd,
        const std::vector<float> *intensities)
    {
        // is the apex at least twice as large as the outermost point?
        // assumption: outermost point is already noise
        if (idxApex >= intensities->size())
        {
            return false;
        }

        float apex = intensities->at(idxApex);
        float left = intensities->at(idxStart);
        float right = intensities->at(idxEnd);
        return (left < right) ? (apex / left) : (apex / right);
    }

#pragma endregion "isValidApexToEdge"

#pragma region isValidQuadraticTerm
    bool isValidQuadraticTerm(
        const RegCoeffs coeff,
        const size_t scale,
        const float mse,
        const size_t df_sum)
    {
        float divisor = std::sqrt(INV_ARRAY[scale * 6 + 4] * mse); // inverseMatrix_2_2 is at position 4 of initialize()
        float tValue = std::max(                                   // t-value for the quadratic term
            std::abs(coeff.b2),                                    // t-value for the quadratic term left side of the peak
            std::abs(coeff.b3));                                   // t-value for the quadratic term right side of the peak
        return tValue > T_VALUES[df_sum - 5] * divisor;            // statistical significance of the quadratic term
        // note that the tvalue would have to be divided by the divisor, but this is not always compiled to a multiplication
    }
#pragma endregion isValidQuadraticTerm

#pragma region isValidPeakHeight

    void calcPeakHeightUncert(
        RegressionGauss *mutateReg,
        const float mse,
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
        const float mse,
        const size_t scale,
        const float apex_position,
        float valley_position, // this value gets mutated in the function
        const size_t df_sum,
        const float apexToEdge)
    {
        // check if the peak height is significantly greater than edge signal
        double Jacobian_height2[4]{0, 0, 0, 0};

        if (apex_position < 0)
        {
            // float edge_position = (valley_position != 0) ? valley_position : static_cast<float>(-scale);
            if (valley_position == 0)
            {
                valley_position = static_cast<float>(scale) * -1;
            }

            Jacobian_height2[1] = apex_position - valley_position;
            Jacobian_height2[2] = apex_position * apex_position - valley_position * valley_position; // adjust for uncertainty calculation of apex to edge ratio
        }
        else
        {
            if (valley_position == 0)
            {
                valley_position = static_cast<float>(scale);
            }
            Jacobian_height2[1] = apex_position - valley_position;
            Jacobian_height2[3] = apex_position * apex_position - valley_position * valley_position; // adjust for uncertainty calculation of apex to edge ratio
        }
        float uncertainty_apexToEdge = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height2, scale));
        return (apexToEdge - 2) / (apexToEdge * uncertainty_apexToEdge) > T_VALUES[df_sum - 5];
    }
#pragma endregion isValidPeakHeight

#pragma region isValidPeakArea

    void calcPeakAreaUncert(RegressionGauss *mutateReg, const float mse, const size_t scale)
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
        const RegCoeffs coeff,
        const float mse,
        const size_t scale,
        const size_t df_sum)
    {
        double doubleScale = scale;
        double b1 = coeff.b1;
        double b2 = coeff.b2;
        double b3 = coeff.b3;
        // assert(!(b2 > 0 && b3 > 0)); // there would be two valley points, so no maximum of the peak
        assert(doubleScale > 0);

        double _SQRTB2 = 1 / std::sqrt(std::abs(b2));
        double _SQRTB3 = 1 / std::sqrt(std::abs(b3));
        double B1_2_B2 = b1 / 2 / b2;
        double EXP_B12 = exp_approx_d(-b1 * B1_2_B2 / 2);
        double B1_2_B3 = b1 / 2 / b3;
        double EXP_B13 = exp_approx_d(-b1 * B1_2_B3 / 2);

        // calculate the Jacobian matrix terms
        double J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
        double J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
        double J_2_common_L = B1_2_B2 / b1;
        double J_2_common_R = B1_2_B3 / b1;

        double J_covered[4];          // Jacobian matrix for the covered peak area
        double x_left = -doubleScale; // left limit due to the window
        double x_right = doubleScale; // right limit due to the window
        double y_left = 0;            // y value at the left limit
        double y_right = 0;           // y value at the right limit

        // here we have to check if there is a valley point or not
        double err_L = 0;
        double B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
        double err_L_covered = 0;
        if (b2 < 0)
        {
            // no valley point
            // error = 1 - std::erf(b1 / 2 / SQRTB2)
            err_L = experfc(B1_2_SQRTB2, -1.0);
            // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            // std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
            err_L_covered = erf_approx_f((b1 - 2 * b2 * doubleScale) / 2 * _SQRTB2) * EXP_B12 * SQRTPI_2 + err_L - SQRTPI_2 * EXP_B12;
        }
        else
        {
            // valley point on the left side of the apex
            // error = erfi(b1 / 2 / SQRTB2)
            err_L = dawson5(B1_2_SQRTB2);
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
        double err_R = 0;
        double B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
        double err_R_covered = 0;
        if (b3 < 0)
        {
            // no valley point
            // error = 1 + std::erf(b1 / 2 / SQRTB3)
            err_R = experfc(B1_2_SQRTB3, 1.0);
            // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            // err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
            err_R_covered = err_R - SQRTPI_2 * EXP_B13 - erf_approx_f((b1 + 2 * b3 * doubleScale) / 2 * _SQRTB3) * SQRTPI_2 * EXP_B13;
        }
        else
        {
            // valley point is on the right side of the apex
            // error = - erfi(b1 / 2 / SQRTB3)
            err_R = dawson5(-B1_2_SQRTB3);
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

        // calculate the y values at the left and right limits
        y_left = exp_approx_d(b1 * x_left + b2 * x_left * x_left);
        y_right = exp_approx_d(b1 * x_right + b3 * x_right * x_right);
        const double dX = x_right - x_left;

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

        float area_uncertainty_covered = std::sqrt(mse * multiplyVecMatrixVecTranspose(J_covered, doubleScale));

        return J_covered[0] > T_VALUES[df_sum - 5] * area_uncertainty_covered; // statistical significance of the peak area (boolean)
    }
#pragma endregion isValidPeakArea

#pragma region "calcUncertaintyPosition"
    float calcUncertaintyPos(
        const float mse,
        const RegCoeffs coeff,
        const float apex_position,
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
}