// qalgorithms_qpeaks.cpp

#include "qalgorithms_qpeaks.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_datatypes.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <cmath>
#include <array>

namespace qAlgorithms
{

#pragma region "pass to qBinning"

    std::vector<qCentroid> passToBinning(std::vector<std::vector<CentroidPeak>> &allPeaks) //, std::vector<unsigned int> addEmpty)
    {
        // initialise empty vector with enough room for all scans - centroids[0] must remain empty
        std::vector<qCentroid> centroids;
        centroids.reserve(allPeaks.size() * 2);
        unsigned int totalCentroids = 0;
        for (size_t i = 0; i < allPeaks.size(); ++i)
        {
            if (!allPeaks[i].empty())
            {
                for (size_t j = 0; j < allPeaks[i].size(); ++j)
                {
                    auto &peak = allPeaks[i][j];
                    qCentroid F = qCentroid{peak.mz, peak.mzUncertainty, peak.scanNumber, peak.area, peak.height, peak.dqsCen, peak.df, totalCentroids};
                    assert(F.scanNo > 0);
                    centroids.push_back(F);
                    ++totalCentroids;
                }
            }
        }
        return centroids;
    }
#pragma endregion "pass to qBinning"

#pragma region "initialize"

    const std::array<float, 384> initialize()
    { // array to store the 6 unique values of the inverse matrix for each scale
        std::array<float, 384> invArray;
        // init invArray
        // XtX = transposed(Matrix X ) * Matrix X
        // XtX_xy: x = row number; y = column number
        float XtX_00 = 1.f;
        float XtX_02 = 0.f;
        float XtX_11 = 0.f;
        float XtX_12 = 0.f;
        float XtX_13 = 0.f;
        float XtX_22 = 0.f;
        for (int i = 1; i < 64; ++i)
        {
            XtX_00 += 2.f;
            XtX_02 += i * i;
            XtX_11 = XtX_02 * 2.f;
            XtX_13 += i * i * i;
            XtX_12 = -XtX_13;
            XtX_22 += i * i * i * i;

            float L_00 = std::sqrt(XtX_00);
            float L_11 = std::sqrt(XtX_11);
            float L_20 = XtX_02 / L_00;
            float L_21 = XtX_12 / L_11;
            float L_20sq = L_20 * L_20;
            float L_21sq = L_21 * L_21;
            float L_22 = std::sqrt(XtX_22 - L_20sq - L_21sq);
            float L_32 = 1 / L_22 * (-L_20sq + L_21sq);
            float L_33 = std::sqrt(XtX_22 - L_20sq - L_21sq - L_32 * L_32);

            float inv_00 = 1.f / L_00;
            float inv_11 = 1.f / L_11;
            float inv_22 = 1.f / L_22;
            float inv_33 = 1.f / L_33;
            float inv_20 = -L_20 * inv_00 / L_22;
            float inv_30 = -(L_20 * inv_00 + L_32 * inv_20) / L_33;
            float inv_21 = -L_21 * inv_11 / L_22;
            float inv_31 = -(-L_21 * inv_11 + L_32 * inv_21) / L_33;
            float inv_32 = -L_32 * inv_22 / L_33;

            invArray[i * 6 + 0] = inv_00 * inv_00 + inv_20 * inv_20 + inv_30 * inv_30; // cell: 0,0
            invArray[i * 6 + 1] = inv_22 * inv_20 + inv_32 * inv_30;                   // cell: 0,2
            invArray[i * 6 + 2] = inv_11 * inv_11 + inv_21 * inv_21 + inv_31 * inv_31; // cell: 1,1
            invArray[i * 6 + 3] = -inv_31 * inv_33;                                    // cell: 1,2
            invArray[i * 6 + 4] = inv_33 * inv_33;                                     // cell: 2,2
            invArray[i * 6 + 5] = inv_32 * inv_33;                                     // cell: 2,3
        }
        return invArray;
    }
#pragma endregion "initialize"

#pragma region "find peaks"
    std::vector<CentroidPeak> findCentroids(const std::vector<ProfileBlock> treatedData, const size_t scanNumber)
    {
        std::vector<CentroidPeak> all_peaks;
        size_t maxWindowSize = 0;
        for (size_t i = 0; i < treatedData.size(); i++)
        {
            size_t length = treatedData[i].df.size();
            assert(length > 4); // data must contain at least five points
            maxWindowSize = maxWindowSize < length ? length : maxWindowSize;
        }
        assert(maxWindowSize > 0);
        float *intensity = new float[maxWindowSize];
        float *logIntensity = new float[maxWindowSize];

        size_t GLOBAL_MAXSCALE_CENTROID = 8; // @todo this is a critical part of the algorithm and should not be hard-coded
        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(treatedData.size() / 2); // probably too large, shouldn't matter
        for (size_t i = 0; i < treatedData.size(); i++)
        {
            auto block = treatedData[i];
            size_t length = block.df.size();
            for (size_t position = 0; position < length; position++)
            {
                intensity[position] = block.intensity[position];
            }

            // perform log-transform on intensity
            std::transform(block.intensity.begin(), block.intensity.end(), logIntensity, [](float y)
                           { return std::log(y); });
            // @todo adjust the scale dynamically based on the number of valid regressions found, early terminate after x iterations
            size_t maxScale = std::min(GLOBAL_MAXSCALE_CENTROID, size_t((length - 1) / 2));
            runningRegression(intensity, logIntensity, block.df, maxWindowSize, length, validRegressions, maxScale);
            if (validRegressions.empty())
            {
                continue; // no valid peaks
            }
            createCentroidPeaks(&all_peaks, &validRegressions, block, scanNumber);
            validRegressions.clear();
        }

        delete[] intensity;
        delete[] logIntensity;
        return all_peaks;
    }

    void findFeatures(std::vector<FeaturePeak> &all_peaks, treatedData &treatedData)
    {
        size_t maxWindowSize = 0;
        for (size_t i = 0; i < treatedData.separators.size(); i++)
        {
            size_t length = treatedData.separators[i].end - treatedData.separators[i].start + 1;
            assert(length > 4); // data must contain at least five points
            maxWindowSize = maxWindowSize < length ? length : maxWindowSize;
        }
        float *intensity = new float[maxWindowSize];
        float *logIntensity = new float[maxWindowSize];
        float *RT = new float[maxWindowSize];
        float *mz = new float[maxWindowSize];
        float *DQSC = new float[maxWindowSize];
        float *DQSB = new float[maxWindowSize]; // @todo only process these after feature construction

        std::vector<RegressionGauss> validRegressions;
        validRegressions.reserve(treatedData.separators.size() / 5); // we expect ~ 20% of all bins to have a feature
        static const size_t GLOBAL_MAXSCALE_FEATURES = 30;
        for (size_t i = 0; i < treatedData.separators.size(); i++)
        {
            size_t length = treatedData.separators[i].end - treatedData.separators[i].start + 1;
            std::vector<bool> degreesOfFreedom;
            for (size_t position = 0; position < length; position++)
            {
                size_t idx = position + treatedData.separators[i].start;
                intensity[position] = treatedData.dataPoints[idx].y;
                RT[position] = treatedData.dataPoints[idx].x;
                degreesOfFreedom.push_back(treatedData.dataPoints[idx].df);
                mz[position] = treatedData.dataPoints[idx].mz;
                DQSC[position] = treatedData.dataPoints[idx].DQSC;
                DQSB[position] = treatedData.dataPoints[idx].DQSB;
            }

            // perform log-transform on Y
            std::transform(intensity, intensity + length, logIntensity, [](float y)
                           { return std::log(y); });
            size_t maxScale = std::min(GLOBAL_MAXSCALE_FEATURES, size_t((length - 1) / 2));
            runningRegression(intensity, logIntensity, degreesOfFreedom, length, length, validRegressions, maxScale);
            if (validRegressions.empty())
            {
                continue; // no valid peaks
            }
            createFeaturePeaks(&all_peaks, &validRegressions, intensity, mz, RT, DQSC, DQSB);
            validRegressions.clear();
        }
        delete[] intensity;
        delete[] logIntensity;
        delete[] RT;
        delete[] mz;
        delete[] DQSC;
        delete[] DQSB;
    }
#pragma endregion "find peaks"

#pragma region "running regression"
    void runningRegression(
        const float *y_start,
        const float *ylog_start,
        const std::vector<bool> degreesOfFreedom,
        const size_t arrayMaxLength,
        const size_t n, // number of data points
        std::vector<RegressionGauss> &validRegressions,
        const size_t maxScale)
    {
        assert(n > 4); // five point minimum is guaranteed by previous functions

        // maximum size of the coefficients array is known at time of function call
        size_t maxSize = n - 4; // max size is at scale = 2
        assert(maxScale <= arrayMaxLength);
        // re-use this array for all coefficients
        __m128 beta[maxSize];

        for (size_t scale = 2; scale <= maxScale; scale++)
        {
            const int k = 2 * scale + 1;      // window size
            const int n_segments = n - k + 1; // number of segments, i.e. regressions considering the array size

            convolve_SIMD(scale, ylog_start, n, beta); // beta past position n-1 contains initialized, but wrong values - make sure they cannot be accessed
                                                       // n_segments must be smaller than the array
            validateRegressions(beta, n_segments, y_start, ylog_start, degreesOfFreedom, arrayMaxLength, scale, validRegressions);
        }

        if (validRegressions.size() > 1)
        {
            validRegressions = mergeRegressionsOverScales(validRegressions, y_start);
        }
        // number of competitors is intialised to 0, so no special case for size = 1 needed
        // there can be 0, 1 or more than one regressions in validRegressions
        return;
    }

    void convolve_SIMD(
        const size_t scale,
        const float *vec, // ylog_start
        const size_t n,   // sumber of data points in vec
        __m128 *result)
    {
        assert(scale > 1);
        assert(n > 2 * scale);
        // this function calculates the regression coefficients from the pseudoinverse matrix P and the y-values supplied through vec.
        // It is equivalent to sum(y[i] * P[i][0]) for beta0, beta1 etc. Every element of result is one set of coefficients (b0, b1, b2, b3)

        size_t n_segments = n - 2 * scale;
        __m128 products[n]; // holds the products of vec and the scale factor
        for (size_t i = 0; i < n; ++i)
        {
            products[i] = _mm_setzero_ps();
        }
        size_t scale6 = scale * 6;
        // activeKernel starts as the kernel offsets
        // this kernel is used to exploit symmetry in the pseudoinverse of the regression. It is constructed iteratively in the loop
        __m128 activeKernel = _mm_set_ps(INV_ARRAY[scale6 + 1],  // coeff b3
                                         INV_ARRAY[scale6 + 1],  // coeff b2
                                         0.0f,                   // coeff b1
                                         INV_ARRAY[scale6 + 0]); // coeff b0
        // this kernel is used to calculate the next active kernel
        __m128 updateKernel = _mm_set_ps(INV_ARRAY[scale6 + 3] - INV_ARRAY[scale6 + 5],
                                         -INV_ARRAY[scale6 + 3] - INV_ARRAY[scale6 + 4],
                                         -INV_ARRAY[scale6 + 2] - INV_ARRAY[scale6 + 3],
                                         -INV_ARRAY[scale6 + 1]);
        // this kernel updates the updateKernel. Since the funciton is non-linear, this must be done iteratively
        const __m128 delayedKernel = _mm_set_ps(2.f * INV_ARRAY[scale6 + 5],
                                                2.f * INV_ARRAY[scale6 + 4],
                                                2.f * INV_ARRAY[scale6 + 3],
                                                2.f * INV_ARRAY[scale6 + 1]);

        for (size_t i = 0; i < n_segments; i++)
        {
            // calculate the center point of the regression window for all applicable vec
            // max index of vec: vec[n - (2 * scale + 1) + 1 + (2 * scale + 1) / 2]
            // = n + 1 - (2 * scale + 1) / 2
            // = n + 1 - scale // scale is always > 1, so vec is not accessed past n - 1
            __m128 vec_values = _mm_set1_ps(vec[i + scale]);
            // result[i][2] is always 0
            result[i] = _mm_mul_ps(vec_values, activeKernel);
        }

        for (size_t i = 1; i < scale + 1; i++)
        {
            // updateKernel = updateKernel original + i * delayedKernel
            updateKernel = _mm_add_ps(updateKernel, delayedKernel);
            // activeKernel = activeKernel original + i * updateKernel
            activeKernel = _mm_add_ps(activeKernel, updateKernel);

            size_t u = 0;
            for (size_t j = scale - i; j < (n - scale + i); j++) // j_start = scale - 1 : 0; j_end = n - scale + 1 : n - scale + scale
            {
                __m128 vec_values = _mm_set1_ps(vec[j]);
                products[u] = _mm_mul_ps(vec_values, activeKernel); // contains the four coefficient products for vec[j]
                u++;
            }

            for (size_t j = 0; j < n_segments; j++)
            {
                __m128 products_temp = _mm_permute_ps(products[j], 0b10110100);              // swap places of b0 and b1(?)
                __m128 sign_flip = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);                      // why only flip the sign of b2?
                products_temp = _mm_fmadd_ps(products_temp, sign_flip, products[2 * i + j]); // why this access pattern? @todo
                result[j] = _mm_add_ps(result[j], products_temp);
            }
        }
        const __m128 flipSign = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
        for (size_t i = 0; i < n_segments; i++)
        { // swap beta2 and beta3 and flip the sign of beta1 // @todo: this is a temporary solution
            result[i] = _mm_mul_ps(_mm_shuffle_ps(result[i], result[i], 0b10110100), flipSign);
        }
    }

    void convolve_details(
        const size_t scale,
        const float *measuredPoints, // ylog_start
        const size_t arraySize,      // number of data points in vec
        __m128 *result)              // coefficients b0, b1, b2, b3 for the points supplied through measuredPoints
    {
        // this function calculates the four regression coefficients needed to describe a peak
        // normally, the pseudoinverse would need to be calculated. An equivalent is pre-computed with the
        // INV_ARRAY global constant.

        // for every element of measuredPoints, the sum of measuredPoints[i] * the pseudoinverse is calculated
        // per coefficient. The pseudoinverse is an array of size 2 * scale + 1. producs[i] is the sum of
        // measuredPoints[i] * pseudoiverse[0] to measuredPoints[i + 2 * scale + 1] * pseudoiverse[i + 2 * scale + 1]

        // we use a vector array so that every coefficient can be calculated at once.
        // __m128 products[arraySize];
        // for (size_t i = 0; i < arraySize; ++i)
        // {
        //     products[i] = _mm_setzero_ps();
        // }
    }

#pragma endregion "running regression"

#pragma region validateRegressions
    void validateRegressions(
        const __m128 *beta,      // coefficients matrix
        size_t n_segments,       // number of segments, i.e. regressions
        const float *y_start,    // pointer to the start of the Y matrix
        const float *ylog_start, // pointer to the start of the Ylog matrix
        const std::vector<bool> degreesOfFreedom,
        size_t arrayMaxLength,
        size_t scale, // scale, i.e., the number of data points in a half window excluding the center point
        std::vector<RegressionGauss> &validRegressions)
    {
        std::vector<RegressionGauss> validRegsTmp; // temporary vector to store valid regressions <index, apex_position>
        // the start index of the regression is the same as the index in beta. The end index is at 2*scale + index in beta.
        validRegsTmp.push_back(RegressionGauss{});
        for (size_t idxStart = 0; idxStart < n_segments; idxStart++)
        {
            size_t df = calcDF(degreesOfFreedom, idxStart, 2 * scale + idxStart);
            if (df < 5)
            {
                continue;
            }
            const __m128 coeff = beta[idxStart];
            if ((coeff[1] == 0.0f) | (coeff[2] == 0.0f) | (coeff[3] == 0.0f))
            {
                // None of these are a valid regression with the asymmetric model
                continue;
            }
            validRegsTmp.back().newCoeffs.b0 = coeff[0];
            validRegsTmp.back().newCoeffs.b1 = coeff[1];
            validRegsTmp.back().newCoeffs.b2 = coeff[2];
            validRegsTmp.back().newCoeffs.b3 = coeff[3];
            // idxStart must be smaller than the size of the checked region
            makeValidRegression(&validRegsTmp.back(), arrayMaxLength, idxStart, scale, degreesOfFreedom, y_start, ylog_start);
            if (validRegsTmp.back().isValid)
            {
                validRegsTmp.push_back(RegressionGauss{});
            }
        }
        if (validRegsTmp.size() == 1)
        {
            return; // no valid peaks
        }
        // early return if only one valid peak, so no grouping possible
        if (validRegsTmp.size() == 2)
        {
            validRegressions.push_back(std::move(validRegsTmp[0]));
            return;
        }
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
                auto bestRegIdx = findBestRegression(y_start, validRegsTmp, degreesOfFreedom,
                                                     startEndGroups[groupIdx], startEndGroups[groupIdx + 1]);

                RegressionGauss bestReg = validRegsTmp[bestRegIdx.first];
                bestReg.mse = bestRegIdx.second;
                validRegressions.push_back(std::move(bestReg));
            }
        } // end for loop (group in vector of groups)
    } // end validateRegressions
#pragma endregion validateRegressions

#pragma region "validate regression test series"

    void makeValidRegression(
        RegressionGauss *mutateReg,
        size_t arrayMaxLength,
        const size_t idxStart,
        const size_t scale,
        const std::vector<bool> degreesOfFreedom,
        const float *intensities,
        const float *intensities_log)
    { // @todo order by effort to calculate
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
        if (mutateReg->apex_position * mutateReg->newCoeffs.b1 > 50 || valley_position * mutateReg->newCoeffs.b1 < -50)
        {
            return; // invalid area pre-filter
        }

        if (valley_position == 0)
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
        assert(mutateReg->right_limit < arrayMaxLength + 1);

        /*
            Note: left and right limit are not the limits of the regression, but of the window the regression applies in.
            When multiple regressions are combined, the window limits are combined by maximum.
        */

        if (scale + idxStart == mutateReg->left_limit || scale + idxStart == mutateReg->right_limit)
        {
            // only one half of the regression applies to the data
            return;
        }

        /*
          Degree of Freedom Filter:
          This block of code implements the degree of freedom filter. It calculates the
          degree of freedom based df vector. If the degree of freedom is less than 5,
          the loop continues to the next iteration. The value 5 is chosen as the
          minimum number of data points required to fit a quadratic regression model.
        */
        size_t df_sum = calcDF(degreesOfFreedom, mutateReg->left_limit, mutateReg->right_limit); // degrees of freedom considering the left and right limits
        if (df_sum < 5)
        {
            return; // degree of freedom less than 5; i.e., less then 5 measured data points
        }
        assert(mutateReg->right_limit - mutateReg->left_limit > 4);
        /*
          Chi-Square Filter:
          This block of code implements the chi-square filter. It calculates the chi-square
          value based on the weighted chi squared sum of expected and measured y values in
          the exponential domain. If the chi-square value is less than the corresponding
          value in the CHI_SQUARES, the regression is invalid.
        */
        float chiSquare = calcSSE_chisqared(mutateReg->newCoeffs, intensities, mutateReg->left_limit, mutateReg->right_limit, scale + idxStart);
        if (chiSquare < CHI_SQUARES[df_sum - 5])
        {
            return; // statistical insignificance of the chi-square value
        }

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates
          the ratio of the apex signal to the edge signal and ensures that the
          ratio is greater than 2. This is a pre-filter for later
          signal-to-noise ratio checkups. apexToEdge is also required in isValidPeakHeight further down
        */
        //         size_t idx_apex = (size_t)std::round(apex_position) + scale + idxStart;
        size_t idxApex = (size_t)std::round(mutateReg->apex_position) + scale + idxStart;
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
        float mse = calcSSE_base(mutateReg->newCoeffs, intensities_log, mutateReg->left_limit, mutateReg->right_limit, scale + idxStart);
        mse /= (df_sum - 4);

        if (!isValidQuadraticTerm(mutateReg->newCoeffs, scale, mse, df_sum))
        {
            return; // statistical insignificance of the quadratic term
        }
        if (!isValidPeakArea(mutateReg->newCoeffs, mse, scale, df_sum))
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

        calcPeakHeightUncert(mutateReg, mse, scale);
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

        mutateReg->uncertainty_pos = calcUncertaintyPos(mse, mutateReg->newCoeffs, mutateReg->apex_position, scale);
        mutateReg->df = df_sum - 4; // @todo add explanation for -4
        mutateReg->apex_position += idxStart + scale;
        mutateReg->scale = scale;
        mutateReg->index_x0 = scale + idxStart;
        mutateReg->isValid = true;
        return;
    }
#pragma endregion "validate regression test series"

#pragma region mergeRegressionsOverScales
    std::vector<RegressionGauss> mergeRegressionsOverScales(std::vector<RegressionGauss> validRegressions, const float *y_start)
    {
        // std::cout << y_start[-1];
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
                        if (validRegressions[j].mse == 0.0)
                        { // calculate the mse of the ref peak
                            validRegressions[j].mse = calcSSE_exp(
                                validRegressions[j].newCoeffs,
                                y_start,
                                validRegressions[j].left_limit,
                                validRegressions[j].right_limit,
                                validRegressions[j].index_x0);
                            validRegressions[j].mse /= validRegressions[j].df;
                        }
                        DF_group += validRegressions[j].df;                            // add the degree of freedom
                        MSE_group += validRegressions[j].mse * validRegressions[j].df; // add the sum of squared errors
                        // add the iterator of the ref peak to a vector of iterators
                        validRegressionsInGroup.push_back(j);
                        competitors += validRegressions[j].numCompetitors + 1; // a regression can have beaten a previous one
                    }
                }
            } // end for loop, inner loop, it_ref_peak

            if (validRegressionsInGroup.empty()) // no competing regressions exist
            {
                // assert(validRegressions[i].mse != 0); // not always the case
                assert(DF_group < 1); // @todo should be redundant
                continue;
            }

            MSE_group /= DF_group;

            if (validRegressions[i].mse == 0.0)
            { // calculate the mse of the current peak
                validRegressions[i].mse = calcSSE_exp(
                    validRegressions[i].newCoeffs,
                    y_start,
                    left_limit,
                    right_limit,
                    validRegressions[i].index_x0);
                validRegressions[i].mse /= validRegressions[i].df;
            }
            // assert(validRegressionsInGroup.size() == 1); // not always the case
            if (validRegressions[i].mse < MSE_group)
            {
                // Set isValid to false for the candidates from the group
                for (size_t it_ref_peak : validRegressionsInGroup)
                {
                    validRegressions[it_ref_peak].isValid = false;
                }
                // std::cout << validRegressionsInGroup.size();
                // assert(validRegressionsInGroup.size() == 1);
                // only advance competitor count if regression is actually better
                validRegressions[i].numCompetitors = competitors;
            }
            else
            { // Set isValid to false for the current peak
                validRegressions[i].isValid = false;
            }
            // validRegressions[i].numCompetitors = validRegressionsInGroup.size(); // this doesn't work because the group is reset every iteration
            // assert(validRegressionsInGroup.size() == 1);
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
    } // end mergeRegressionsOverScales
#pragma endregion mergeRegressionsOverScales

#pragma region "create peaks"

    std::pair<float, float> weightedMeanAndVariance(const float *values, const float *weight,
                                                    int left_limit, int right_limit)
    { // @todo rework this function to be more readable
        // weighted mean using intensity as weighting factor and left_limit right_limit as range
        int realPoints = right_limit - left_limit + 1;
        float mean_wt = 0.0;                            // mean of weight
        float sum_xw = 0.0;                             // sum of values * weight
        float sum_weight = 0.0;                         // sum of weight
        for (int j = left_limit; j <= right_limit; j++) // why does this work? there should be an array access at less than 0 @todo
        {
            if (values[j] != 0)
            {
                mean_wt += weight[j];
                sum_xw += values[j] * weight[j];
                sum_weight += weight[j];
            }
            else
            {
                realPoints--;
            }
        }
        mean_wt /= realPoints;
        sum_xw /= mean_wt;
        sum_weight /= mean_wt;

        float weighted_mean = sum_xw / sum_weight;
        float sum_Qxxw = 0.0; // sum of (values - mean)^2 * weight
        for (int j = left_limit; j <= right_limit; j++)
        {
            if (values[j] <= 0.f)
            {
                continue;
            }
            sum_Qxxw += (values[j] - weighted_mean) * (values[j] - weighted_mean) * weight[j];
        }
        float uncertaintiy = std::sqrt(sum_Qxxw / sum_weight / realPoints);
        return std::make_pair(weighted_mean, uncertaintiy);
    };

    void createCentroidPeaks(
        std::vector<CentroidPeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        ProfileBlock block,
        const size_t scanNumber)
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
            RegCoeffs coeff = regression.newCoeffs;
            peak.height = exp_approx_d(coeff.b0 + (regression.apex_position - regression.index_x0) * coeff.b1 * 0.5);
            // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.heightUncertainty = regression.uncertainty_height * peak.height;

            size_t offset = (size_t)std::floor(regression.apex_position);
            double mz0 = block.mz[offset];
            double delta_mz = block.mz[offset + 1] - mz0;

            // add scaled area
            float exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression.area * exp_b0 * delta_mz;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0 * delta_mz;

            // add scaled apex position (mz)
            peak.mz = mz0 + delta_mz * (regression.apex_position - std::floor(regression.apex_position));
            peak.mzUncertainty = regression.uncertainty_pos * delta_mz * T_VALUES[regression.df + 1] * sqrt(1 + 1 / (regression.df + 4));

            // quality params
            peak.dqsCen = 1 - erf_approx_f(regression.uncertainty_area / regression.area);
            peak.df = regression.df;

            peak.numCompetitors = regression.numCompetitors;
            peak.scale = regression.scale;

            peak.interpolations = regression.right_limit - regression.left_limit + 1 - regression.df - 4; // -4 since four coefficients take up degrees of freedom

            /// @todo consider adding these properties so we can trace back everything completely
            // peak.idxPeakStart = regression.left_limit;
            // peak.idxPeakEnd = regression.right_limit;
            peaks->push_back(std::move(peak));
        }
    }

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const float *intensity,
        const float *mz_start,
        const float *rt_start,
        const float *DQSC,
        const float *DQSB)
    {
        assert(!validRegressionsVec->empty());
        for (size_t i = 0; i < validRegressionsVec->size(); i++)
        {
            RegressionGauss regression = (*validRegressionsVec)[i];
            assert(regression.isValid);

            FeaturePeak peak;
            // add height
            RegCoeffs coeff = regression.newCoeffs;
            peak.height = exp_approx_d(coeff.b0 + (regression.apex_position - regression.index_x0) * coeff.b1 * 0.5); // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.heightUncertainty = regression.uncertainty_height * peak.height;

            const double rt_leftOfMax = *(rt_start + (int)std::floor(regression.apex_position)); // left of maximum
            const double delta_rt = *(rt_start + (int)std::floor(regression.apex_position) + 1) - rt_leftOfMax;
            const double apex_position = rt_leftOfMax + delta_rt * (regression.apex_position - std::floor(regression.apex_position));
            peak.retentionTime = apex_position;
            peak.retentionTimeUncertainty = regression.uncertainty_pos * delta_rt;

            // add area
            float exp_b0 = exp_approx_d(coeff.b0); // exp(b0)
            peak.area = regression.area * exp_b0 * delta_rt;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0 * delta_rt;

            std::pair<float, float> mz = weightedMeanAndVariance(mz_start, intensity, regression.left_limit, regression.right_limit);
            peak.mz = mz.first;
            peak.mzUncertainty = mz.second;

            peak.dqsCen = weightedMeanAndVariance(DQSC, intensity, regression.left_limit, regression.right_limit).first;
            peak.dqsBin = weightedMeanAndVariance(DQSB, intensity, regression.left_limit, regression.right_limit).first;
            peak.DQSF = 1 - erf_approx_f(regression.uncertainty_area / regression.area);

            peak.idxPeakStart = regression.left_limit;
            peak.idxPeakEnd = regression.right_limit - 1;

            // params needed to merge two peaks
            peak.apexLeft = regression.apex_position < regression.index_x0;
            coeff.b1 /= delta_rt;
            coeff.b2 /= delta_rt * delta_rt;
            coeff.b3 /= delta_rt * delta_rt;
            peak.coefficients = coeff;

            if (regression.scale < 3)
            {
                std::cerr << "regression found at scale 2!\n";
            }

            peak.scale = regression.scale;
            peak.interpolationCount = regression.right_limit - regression.left_limit - regression.df - 4; // -4 since the degrees of freedom are reduced by 1 per coefficient
            peak.competitorCount = regression.numCompetitors;

            peaks->push_back(std::move(peak));
        }
    }
#pragma endregion "create peaks"

#pragma region calcSSE

    float calcSSE_base(const RegCoeffs coeff, const float *y_start, size_t limit_L, size_t limit_R, size_t index_x0)
    {
        double result = 0.0;
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x;
            double y_current = y_start[iSegment];
            double newdiff = (y_base - y_current) * (y_base - y_current);

            result += newdiff;
        }
        // center point
        result += (coeff.b0 - y_start[index_x0]) * (coeff.b0 - y_start[index_x0]); // x = 0 -> (b0 - y)^2

        // right side
        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // iSegment = 0 is center point calculated above
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = coeff.b0 + (coeff.b1 + coeff.b3 * new_x) * new_x; // b3 instead of b2
            double y_current = y_start[iSegment];                             // y_start[0] is the leftmost y value
            double newdiff = (y_current - y_base) * (y_current - y_base);

            result += newdiff;
        }
        return result;
    }

    float calcSSE_exp(const RegCoeffs coeff, const float *y_start, size_t limit_L, size_t limit_R, size_t index_x0)
    {
        double result = 0.0;
        // @todo error: the array is accessed at positions < 0
        // rework: operate over limit_L to limit_R
        // int limit_L2 = limit_L - index_x0;
        // std::cout << limit_L << ", " << limit_R << " " << index_x0 << "\n";
        // assert(-limit_L2 == limit_R2);
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0); // always negative
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = y_start[iSegment];
            // std::cout << y_current << ", ";
            double newdiff = (y_base - y_current) * (y_base - y_current);
            // std::cout << limit_L2 + iSegment << ", ";
            // assert(limit_L2 + iSegment >= 0);
            result += newdiff;
        }
        // center point
        result += (exp_approx_d(coeff.b0) - y_start[index_x0]) * (exp_approx_d(coeff.b0) - y_start[index_x0]); // x = 0 -> (b0 - y)^2
        // std::cout << y_start[index_x0] << ", ";
        // std::cout << limit_L2  -limit_L2 << ", ";
        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // start one past the center, include right limit index
        {
            double new_x = double(iSegment) - double(index_x0);                             // always positive
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * new_x) * new_x); // b3 instead of b2
            double y_current = y_start[iSegment];                                           // y_start[0] is the leftmost y value
            // std::cout << y_current << ", ";
            double newdiff = (y_current - y_base) * (y_current - y_base);
            // std::cout << limit_L2 + iSegment  -limit_L2 << ", ";
            result += newdiff;
        }
        // std::cout << "\n";
        // exit(1);
        return result;
    }

    float calcSSE_chisqared(const RegCoeffs coeff, const float *y_start, size_t limit_L, size_t limit_R, size_t index_x0)
    {
        double result = 0.0;
        // left side
        for (size_t iSegment = limit_L; iSegment < index_x0; iSegment++)
        {
            double new_x = double(iSegment) - double(index_x0);
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b2 * new_x) * new_x);
            double y_current = y_start[iSegment];
            double newdiff = (y_base - y_current) * (y_base - y_current);
            result += newdiff / y_base;
            // double newdiff = y_current * y_current / y_base + 2 * y_current + y_base;
            // result += newdiff;
        }
        // center point, left and right term are identical
        result += ((exp_approx_d(coeff.b0) - y_start[index_x0]) *
                   (exp_approx_d(coeff.b0) - y_start[index_x0])) /
                  exp_approx_d(coeff.b0); // x = 0 -> (b0 - y)^2

        for (size_t iSegment = index_x0 + 1; iSegment < limit_R + 1; iSegment++) // iSegment = 0 is center point (calculated above)
        {
            double y_base = exp_approx_d(coeff.b0 + (coeff.b1 + coeff.b3 * iSegment) * iSegment); // b3 instead of b2
            double y_current = y_start[iSegment];                                                 // y_start[0] is the leftmost y value
            double newdiff = (y_current - y_base) * (y_current - y_base);
            result += newdiff / y_base;
            // double newdiff = y_current * y_current / y_base + 2 * y_current + y_base;
            // result += newdiff;
        }
        return result;
    }

#pragma endregion calcSSE

    std::pair<size_t, float> findBestRegression( // index, mse
        const float *y_start,                    // start of the measured data
        std::vector<RegressionGauss> regressions,
        const std::vector<bool> degreesOfFreedom,
        size_t startIdx,
        size_t endIdx) // degrees of freedom
    {
        float best_mse = INFINITY;
        // auto best_regression = regressions.begin();
        size_t bestRegIdx = 0;

        // identify left (smallest) and right (largest) limit of the grouped regression windows
        unsigned int left_limit = regressions[startIdx].left_limit;
        unsigned int right_limit = regressions[startIdx].right_limit;
        for (size_t i = startIdx; i < endIdx + 1; i++)
        {
            left_limit = std::min(left_limit, regressions[i].left_limit);
            right_limit = std::max(right_limit, regressions[i].right_limit);
        }

        const size_t df_sum = calcDF(degreesOfFreedom, left_limit, right_limit);

        for (size_t i = startIdx; i < endIdx + 1; i++)
        {
            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            float mse = calcSSE_exp(regressions[i].newCoeffs,
                                    y_start,
                                    left_limit,
                                    right_limit,
                                    regressions[i].index_x0);
            mse /= (df_sum - 4);

            if (mse < best_mse)
            {
                best_mse = mse;
                bestRegIdx = i;
            }
        }
        return std::pair(bestRegIdx, best_mse);
    }

    size_t calcDF(
        const bool *degreesOfFreedom, // start of the degrees of freedom array
        unsigned int left_limit,
        unsigned int right_limit)
    {
        size_t DF = 0;
        for (size_t i = left_limit; i < right_limit; i++)
        {
            if (degreesOfFreedom[i])
            {
                ++DF;
            }
        }
        return DF;
    }

    size_t calcDF(
        const std::vector<bool> degreesOfFreedom,
        unsigned int left_limit,
        unsigned int right_limit)
    {
        size_t DF = 0;
        for (size_t i = left_limit; i < right_limit; i++)
        {
            if (degreesOfFreedom[i])
            {
                ++DF;
            }
        }
        return DF;
    }

#pragma region calcApexAndValleyPos
    bool calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        const int scale, // this scale needs to be an int since it needs to be negative in the function body
        float &valley_position)
    {
        // symmetric model should apply, this is not possible with the current peak model @todo
        assert(mutateReg->newCoeffs.b1 != 0);
        assert(mutateReg->newCoeffs.b2 != 0 && mutateReg->newCoeffs.b3 != 0);
        // calculate key by checking the signs of coeff
        // _mm_movemask_ps returns the sign bits of the input array
        // int key = _mm_movemask_ps(coeff);
        int key = 0;
        if (mutateReg->newCoeffs.b1 < 0)
        {
            key += 1;
        }
        if (mutateReg->newCoeffs.b2 < 0)
        {
            key += 3;
        }
        if (mutateReg->newCoeffs.b3 < 0)
        {
            key += 5;
        }

        // b1, b2, b3 are negative            ; 1110 = 14 ; 1 + 3 + 5 = 9
        // b2, b3 are negative, b1 is positive; 1100 = 12 ; 0 + 3 + 5 = 8
        // b1, b2 are negative, b3 is positive; 0110 = 6  ; 1 + 3 + 0 = 4
        // b1, b2 are positive, b3 is negative; 1000 = 8  ; 0 + 0 + 5 = 5
        enum keyCase
        {
            apexLeft_valleyNone = 9,
            apexRight_valleyNone = 8,
            apexLeft_valleyRight = 4,
            apexRight_valleyLeft = 5
        };

        float position_2 = -mutateReg->newCoeffs.b1 / (2 * mutateReg->newCoeffs.b2);
        float position_3 = -mutateReg->newCoeffs.b1 / (2 * mutateReg->newCoeffs.b3);

        switch (key)
        {
        case apexLeft_valleyNone:
            mutateReg->apex_position = position_2; //-B1 / 2 / B2;  // is negative
            valley_position = 0;                   // no valley point
            return position_2 > -scale + 1;        // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyNone:                 // Case 1b: apex right
            mutateReg->apex_position = position_3; //-B1 / 2 / B3;     // is positive
            valley_position = 0;                   // no valley point
            return position_3 < scale - 1;         // scale -1: prevent apex position to be at the edge of the data

        case apexLeft_valleyRight:
            mutateReg->apex_position = position_2;                              //-B1 / 2 / B2;      // is negative
            valley_position = position_3;                                       //-B1 / 2 / B3;      // is positive
            return position_2 > -scale + 1 && valley_position - position_2 > 2; // scale +1: prevent apex position to be at the edge of the data

        case apexRight_valleyLeft:
            mutateReg->apex_position = position_3;                             //-B1 / 2 / B3;       // is positive
            valley_position = position_2;                                      //-B1 / 2 / B2;       // is negative
            return position_3 < scale - 1 && position_3 - valley_position > 2; // scale -1: prevent apex position to be at the edge of the data

        default:
            return false; // invalid case
        }
    }
#pragma endregion calcApexAndValleyPos

#pragma region "isValidApexToEdge"

    inline float apexToEdgeRatio(
        const size_t idxStart,
        const size_t idxApex,
        const size_t idxEnd,
        const float *y_start)
    {
        float apex = y_start[idxApex];
        float left = y_start[idxStart];
        float right = y_start[idxEnd];
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
        float Jacobian_height[4]{1, 0, 0, 0};          // Jacobian matrix for the height
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
        float valley_position,
        const size_t df_sum,
        const float apexToEdge)
    {
        // check if the peak height is significantly greater than edge signal

        float Jacobian_height2[4]{0, 0, 0, 0};
        // Jacobian_height2[0] = 0.f; // adjust for uncertainty calculation of apex to edge ratio

        if (apex_position < 0)
        {
            // float edge_position = (valley_position != 0) ? valley_position : static_cast<float>(-scale);
            if (valley_position == 0)
            {
                valley_position = static_cast<float>(scale) * -1;
            }

            Jacobian_height2[1] = apex_position - valley_position;
            Jacobian_height2[2] = apex_position * apex_position - valley_position * valley_position; // adjust for uncertainty calculation of apex to edge ratio
            // Jacobian_height2[3] = 0;
        }
        else
        {
            if (valley_position == 0)
            {
                valley_position = static_cast<float>(scale);
            }
            Jacobian_height2[1] = apex_position - valley_position;
            // Jacobian_height2[2] = 0;
            Jacobian_height2[3] = apex_position * apex_position - valley_position * valley_position; // adjust for uncertainty calculation of apex to edge ratio
        }
        float uncertainty_apexToEdge = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height2, scale));
        return (apexToEdge - 2) / (apexToEdge * uncertainty_apexToEdge) > T_VALUES[df_sum - 5];
    }
#pragma endregion isValidPeakHeight

#pragma region isValidPeakArea

    void calcPeakAreaUncert(RegressionGauss *mutateReg, const float mse, const int scale)
    {
        float b1 = mutateReg->newCoeffs.b1;
        float b2 = mutateReg->newCoeffs.b2;
        float b3 = mutateReg->newCoeffs.b3;
        float _SQRTB2 = 1 / std::sqrt(std::abs(b2));
        float _SQRTB3 = 1 / std::sqrt(std::abs(b3));
        float B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
        float B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
        float B1_2_B2 = b1 / 2 / b2;
        float B1_2_B3 = b1 / 2 / b3;

        float J[4]; // Jacobian matrix

        // here we have to check if there is a valley point or not // @todo this can be simplified
        const float err_L =
            (b2 < 0)
                ? experfc(B1_2_SQRTB2, -1.0) // 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
                : dawson5(B1_2_SQRTB2);      // erfi(b1 / 2 / SQRTB2);        // peak with valley point;

        float err_R =
            (b3 < 0)
                ? experfc(B1_2_SQRTB3, 1.0) // 1 + std::erf(b1 / 2 / SQRTB3) // ordinary peak
                : dawson5(-B1_2_SQRTB3);    // -erfi(b1 / 2 / SQRTB3);       // peak with valley point ;

        // calculate the Jacobian matrix terms
        float J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
        float J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
        float J_2_common_L = B1_2_B2 / b1;
        float J_2_common_R = B1_2_B3 / b1;
        float J_1_L = J_1_common_L * err_L;
        float J_1_R = J_1_common_R * err_R;
        float J_2_L = J_2_common_L - J_1_L * B1_2_B2;
        float J_2_R = -J_2_common_R - J_1_R * B1_2_B3;

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
        RegCoeffs coeff,
        const float mse,
        const int scale, // scale must be an int here
        const size_t df_sum)
    {
        float b1 = coeff.b1;
        float b2 = coeff.b2;
        float b3 = coeff.b3;
        assert(!(b2 > 0 && b3 > 0)); // there would be two valley points, so no maximum of the peak
        assert(scale > 0);

        float _SQRTB2 = 1 / std::sqrt(std::abs(b2));
        float _SQRTB3 = 1 / std::sqrt(std::abs(b3));
        float B1_2_B2 = b1 / 2 / b2;
        float EXP_B12 = exp_approx_d(-b1 * B1_2_B2 / 2);
        float B1_2_B3 = b1 / 2 / b3;
        float EXP_B13 = exp_approx_d(-b1 * B1_2_B3 / 2);

        // calculate the Jacobian matrix terms
        float J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
        float J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
        float J_2_common_L = B1_2_B2 / b1;
        float J_2_common_R = B1_2_B3 / b1;

        float J_covered[4];    // Jacobian matrix for the covered peak area
        float x_left = -scale; // left limit due to the window
        float x_right = scale; // right limit due to the window
        float y_left = 0;      // y value at the left limit
        float y_right = 0;     // y value at the right limit

        // here we have to check if there is a valley point or not
        float err_L = 0;
        float B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
        float err_L_covered = 0;
        if (b2 < 0)
        {
            // no valley point
            // error = 1 - std::erf(b1 / 2 / SQRTB2)
            err_L = experfc(B1_2_SQRTB2, -1.0);
            // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            // std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
            err_L_covered = erf_approx_f((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12 * SQRTPI_2 + err_L - SQRTPI_2 * EXP_B12;
        }
        else
        {
            // valley point on the left side of the apex
            // error = erfi(b1 / 2 / SQRTB2)
            err_L = dawson5(B1_2_SQRTB2);
            // check if the valley point is inside the window for the regression and consider it if necessary
            if (-B1_2_B2 < -scale)
            {
                // valley point is outside the window, use scale as limit
                err_L_covered = err_L - erfi((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12;
            }
            else
            {
                // valley point is inside the window, use valley point as limit
                err_L_covered = err_L;
                x_left = -B1_2_B2;
            }
        }
        float err_R = 0;
        float B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
        float err_R_covered = 0;
        if (b3 < 0)
        {
            // no valley point
            // error = 1 + std::erf(b1 / 2 / SQRTB3)
            err_R = experfc(B1_2_SQRTB3, 1.0);
            // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            // err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
            err_R_covered = err_R - SQRTPI_2 * EXP_B13 - erf_approx_f((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * SQRTPI_2 * EXP_B13;
        }
        else
        {
            // valley point is on the right side of the apex
            // error = - erfi(b1 / 2 / SQRTB3)
            err_R = dawson5(-B1_2_SQRTB3);
            if (-B1_2_B3 > scale)
            {
                // valley point is outside the window, use scale as limit
                err_R_covered = erfi((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * EXP_B13 + err_R;
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
        const float dX = x_right - x_left;

        // calculate the trapzoid correction terms for the jacobian matrix
        const float trpzd_b0 = (y_right + y_left) * dX / 2;
        const float trpzd_b1 = (x_right * y_right + x_left * y_left) * dX / 2;
        const float trpzd_b2 = (x_left * x_left * y_left) * dX / 2;
        const float trpzd_b3 = (x_right * x_right * y_right) * dX / 2;

        const float J_1_L_covered = J_1_common_L * err_L_covered;
        const float J_1_R_covered = J_1_common_R * err_R_covered;
        const float J_2_L_covered = J_2_common_L - J_1_L_covered * B1_2_B2;
        const float J_2_R_covered = -J_2_common_R - J_1_R_covered * B1_2_B3;

        J_covered[0] = J_1_R_covered + J_1_L_covered - trpzd_b0;
        J_covered[1] = J_2_R_covered + J_2_L_covered - trpzd_b1;
        J_covered[2] = -B1_2_B2 * (J_2_L_covered + J_1_L_covered / b1) - trpzd_b2;
        J_covered[3] = -B1_2_B3 * (J_2_R_covered + J_1_R_covered / b1) - trpzd_b3;

        float area_uncertainty_covered = std::sqrt(mse * multiplyVecMatrixVecTranspose(J_covered, scale));

        return J_covered[0] > T_VALUES[df_sum - 5] * area_uncertainty_covered; // statistical significance of the peak area (boolean)
    }
#pragma endregion isValidPeakArea

#pragma region "calcUncertaintyPosition"
    float calcUncertaintyPos(
        const float mse,
        RegCoeffs coeff,
        const float apex_position,
        const size_t scale)
    {
        float _b1 = 1 / coeff.b1;
        float _b2 = 1 / coeff.b2;
        float _b3 = 1 / coeff.b3;
        float J[4]; // Jacobian matrix
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
    // these chain to return beta for a regression

    inline float multiplyVecMatrixVecTranspose(const float vec[4], size_t scale)
    {
        scale *= 6;
        float result = vec[0] * vec[0] * INV_ARRAY[scale + 0] +
                       vec[1] * vec[1] * INV_ARRAY[scale + 2] +
                       (vec[2] * vec[2] + vec[3] * vec[3]) * INV_ARRAY[scale + 4] +
                       2 * (vec[2] * vec[3] * INV_ARRAY[scale + 5] +
                            vec[0] * (vec[1] + vec[3]) * INV_ARRAY[scale + 1] +
                            vec[1] * (vec[2] - vec[3]) * INV_ARRAY[scale + 3]);

        return result;
    }
#pragma endregion "convolve regression"
}