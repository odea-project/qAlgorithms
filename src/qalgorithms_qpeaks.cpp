// qalgorithms_qpeaks.cpp
//
// internal
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_utils.h"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <cmath>

// external

namespace q
{
    namespace Algorithms
    {

#pragma region "plotPeaksToPython"
        void
        qPeaks::plotPeaksToPython(
            const std::string &filename_input,
            const std::string &filename_output,
            const bool includeFits,
            const bool featureMap) const
        {
            // check if python is installed
            if (system("python --version"))
            {
                std::cerr << "Python is not installed on this system. Please install Python to use this function.\n";
                return;
            }
            // check if python in the path
            if (system("python -c \"import sys\""))
            {
                std::cerr << "Python is not in the path. Please add Python to the path to use this function.\n";
                return;
            }
            std::string command = "python python/plotPeaksToPython.py " + filename_input + " " + filename_output + " " + std::to_string(includeFits) + " " + std::to_string(featureMap);
            std::string result;
            std::system(command.c_str());
            std::cout << result;
        }
#pragma endregion "plotPeaksToPython"

#pragma region "pass to qBinning"
        qBinning::CentroidedData
        qPeaks::passToBinning(std::vector<std::vector<q::DataType::Peak>> &allPeaks, std::vector<unsigned int> addEmpty)
        {
            // initialise empty vector with enough room for all scans - centroids[0] must remain empty
            std::vector<std::vector<q::Algorithms::qBinning::qCentroid>> centroids(allPeaks.size() + 1, std::vector<qBinning::qCentroid>(0));
            int totalCentroids = 0;
            int scanRelative = 0;
            // std::vector<qBinning::qCentroid> scan(0);
            for (size_t i = 0; i < allPeaks.size(); ++i)
            {
                if (addEmpty[i] != 0)
                {
                    for (size_t j = 0; j < addEmpty[i]; j++)
                    {
                        centroids.push_back(std::vector<qBinning::qCentroid>(0));
                        scanRelative++;
                    }
                }

                if (!allPeaks[i].empty())
                {
                    ++scanRelative; // scans start at 1
                    // sort the peaks in ascending order of retention time
                    std::sort(allPeaks[i].begin(), allPeaks[i].end(), [](const q::DataType::Peak a, const q::DataType::Peak b)
                              { return a.retentionTime < b.retentionTime; });
                    for (size_t j = 0; j < allPeaks[i].size(); ++j)
                    {
                        auto &peak = allPeaks[i][j];
                        qBinning::qCentroid F = qBinning::qCentroid{peak.mz, peak.mzUncertainty, peak.retentionTime,
                                                                    scanRelative, peak.area, peak.height, peak.dqsCen};
                        centroids[scanRelative].push_back(F);
                        ++totalCentroids;
                    }
                }
                else
                {
                    centroids.push_back(std::vector<qBinning::qCentroid>(0));
                    scanRelative++;
                }
            }
            // the first scan must be empty for compatibility with qBinning
            assert(centroids[0].empty());

            for (size_t i = 1; i < centroids.size(); i++)
            {
                std::sort(centroids[i].begin(), centroids[i].end(), [](qBinning::qCentroid lhs, qBinning::qCentroid rhs)
                          { return lhs.mz < rhs.mz; });
            }

            return qBinning::CentroidedData{centroids, totalCentroids};
        }
#pragma endregion "pass to qBinning"

#pragma region "initialize"
        alignas(float) float q::Algorithms::qPeaks::x_square[128];   // array to store the square of the x values
        alignas(float) float q::Algorithms::qPeaks::invArray[64][6]; // array to store the 6 unique values of the inverse matrix for each scale
        __m128 q::Algorithms::qPeaks::ZERO_128;                      // [0., 0., 0., 0.]
        __m256 q::Algorithms::qPeaks::ZERO_256;                      // [0., 0., 0., 0., 0., 0., 0., 0.]
        __m128 q::Algorithms::qPeaks::KEY_128;                       // [0., 4., 2., 1.]
        __m256 q::Algorithms::qPeaks::LINSPACE_UP_POS_256;           // [7., 6., 5., 4., 3., 2., 1., 0.]
        __m256 q::Algorithms::qPeaks::LINSPACE_UP_NEG_256;           // [-7., -6., -5., -4., -3., -2., -1., 0.]
        __m256 q::Algorithms::qPeaks::LINSPACE_DOWN_NEG_256;         // [0., -1., -2., -3., -4., -5., -6., -7.]
        __m256i q::Algorithms::qPeaks::LINSPACE_UP_INT_256;          // [7, 6, 5, 4, 3, 2, 1, 0]
        __m256i q::Algorithms::qPeaks::LINSPACE_DOWN_INT_256;        // [0, 1, 2, 3, 4, 5, 6, 7]
        __m256 q::Algorithms::qPeaks::MINUS_ONE_256;                 // [-1., -1., -1., -1., -1., -1., -1., -1.]

        void qPeaks::initialize()
        {

            ZERO_128 = _mm_setzero_ps();
            ZERO_256 = _mm256_setzero_ps();
            KEY_128 = _mm_set_ps(1.f, 2.f, 4.f, 0.f);
            LINSPACE_UP_POS_256 = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
            LINSPACE_UP_NEG_256 = _mm256_set_ps(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
            LINSPACE_DOWN_NEG_256 = _mm256_set_ps(0.f, -1.f, -2.f, -3.f, -4.f, -5.f, -6.f, -7.f);
            LINSPACE_UP_INT_256 = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
            LINSPACE_DOWN_INT_256 = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
            MINUS_ONE_256 = _mm256_set1_ps(-1.f);
            for (int i = 0; i < 128; ++i)
            {
                x_square[i] = i * i;
            }
            // init invArray
            float XtX_00 = 1.f;
            float XtX_02 = 0.f;
            float XtX_11 = 0.f;
            float XtX_12 = 0.f;
            float XtX_13 = 0.f;
            float XtX_22 = 0.f;
            for (int i = 1; i < 64; ++i)
            {
                XtX_00 += 2.f;
                XtX_02 += x_square[i];
                XtX_11 = XtX_02 * 2.f;
                XtX_13 += x_square[i] * i;
                XtX_12 = -XtX_13;
                XtX_22 += x_square[i] * x_square[i];

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

                invArray[i][0] = inv_00 * inv_00 + inv_20 * inv_20 + inv_30 * inv_30; // cell: 0,0
                invArray[i][1] = inv_22 * inv_20 + inv_32 * inv_30;                   // cell: 0,2
                invArray[i][2] = inv_11 * inv_11 + inv_21 * inv_21 + inv_31 * inv_31; // cell: 1,1
                invArray[i][3] = -inv_31 * inv_33;                                    // cell: 1,2
                invArray[i][4] = inv_33 * inv_33;                                     // cell: 2,2
                invArray[i][5] = inv_32 * inv_33;                                     // cell: 2,3
            }
        }
#pragma endregion "initialize"

#pragma region "find centroids"
        void
        qPeaks::findCentroids(
            std::vector<DataType::Peak> &all_peaks,
            q::MeasurementData::MeasurementData::treatedData &treatedData,
            const int scanNumber,
            const float retentionTime)
        {
            const float rt[2] = {-255.f, retentionTime}; // this will asign peaks to the retention time
            const float *rt_start = rt;                  // start of the retention time vector

            for (auto it_separators = treatedData.separators.begin(); it_separators != treatedData.separators.end() - 1; it_separators++)
            {
                const int n = *(it_separators + 1) - *it_separators; // calculate the number of data points in the block
                if (n <= 512)
                {
                    // STATIC APPROACH
                    alignas(float) float Y[512];                   // measured y values
                    alignas(float) float Ylog[512];                // log-transformed measured y values
                    alignas(float) float X[512];                   // measured x values
                    alignas(bool) bool df[512];                    // degree of freedom vector, 0: interpolated, 1: measured
                    validRegression_static validRegressions[2048]; // array of valid regressions with default initialization, i.e., random states
                    int validRegressionsIndex = 0;                 // index of the valid regressions

                    // iterators to the start of the data
                    const auto y_start = Y;
                    const auto ylog_start = Ylog;
                    const auto mz_start = X;
                    const auto df_start = df;

                    int i = 0;
                    for (int idx = *it_separators; idx < *(it_separators + 1); idx++)
                    {
                        Y[i] = treatedData.dataPoints[idx].y;
                        X[i] = treatedData.dataPoints[idx].x;
                        df[i] = treatedData.dataPoints[idx].df;
                        i++;
                    }

                    // perform log-transform on Y
                    std::transform(y_start, y_start + n, ylog_start, [](float y)
                                   { return std::log(y); });
                    runningRegression_static(y_start, ylog_start, df_start, n, validRegressions, validRegressionsIndex);
                    if (validRegressionsIndex == 0)
                    {
                        continue; // no valid peaks
                    }
                    createPeaks_static(all_peaks, validRegressions, validRegressionsIndex, y_start, mz_start, rt_start, df_start, nullptr, nullptr, rt_start, scanNumber);
                }
                else
                {
                    // DYNAMIC APPROACH
                    alignas(float) float *Y = new float[n];
                    alignas(float) float *Ylog = new float[n];
                    alignas(float) float *X = new float[n];
                    alignas(float) bool *df = new bool[n];
                    std::vector<validRegression_static> validRegressions;

                    // iterator to the start
                    const auto y_start = Y;
                    const auto ylog_start = Ylog;
                    const auto mz_start = X;
                    const auto df_start = df;

                    int i = 0;
                    for (int idx = *it_separators; idx < *(it_separators + 1); idx++)
                    {
                        Y[i] = treatedData.dataPoints[idx].y;
                        X[i] = treatedData.dataPoints[idx].x;
                        df[i] = treatedData.dataPoints[idx].df;
                        i++;
                    }

                    // perform log-transform on Y
                    std::transform(y_start, y_start + n, ylog_start, [](float y)
                                   { return std::log(y); });
                    runningRegression(y_start, ylog_start, df_start, n, validRegressions);
                    if (validRegressions.empty())
                    {
                        continue; // no valid peaks
                    }
                    createPeaks(all_peaks, validRegressions, y_start, mz_start, rt_start, df_start, nullptr, nullptr, rt_start, scanNumber);
                    delete[] Y;
                    delete[] Ylog;
                    delete[] X;
                    delete[] df;
                }
            }
        }
#pragma endregion "find centroids"

#pragma region "find peaks"
        void
        qPeaks::findPeaks(
            std::vector<DataType::Peak> &all_peaks,
            q::MeasurementData::MeasurementData::treatedData &treatedData)
        {
            for (auto it_separators = treatedData.separators.begin(); it_separators != treatedData.separators.end() - 1; it_separators++)
            {
                const int n = *(it_separators + 1) - *it_separators; // calculate the number of data points in the block
                assert(n > 0);                                       // check if the number of data points is greater than 0
                if (n <= 512)
                {
                    // STATIC APPROACH
                    alignas(float) float Y[512];                   // measured y values
                    alignas(float) float Ylog[512];                // log-transformed measured y values
                    alignas(float) float X[512];                   // measured x values
                    alignas(bool) bool df[512];                    // degree of freedom vector, 0: interpolated, 1: measured
                    alignas(float) float mz[512];                  // measured mz values
                    alignas(float) float dqs_cen[512];             // measured dqs values
                    alignas(float) float dqs_bin[512];             // measured dqs values
                    validRegression_static validRegressions[2048]; // array of valid regressions with default initialization, i.e., random states
                    int validRegressionsIndex = 0;                 // index of the valid regressions

                    // iterators to the start of the data
                    const auto y_start = Y;
                    const auto ylog_start = Ylog;
                    const auto rt_start = X;
                    const auto df_start = df;
                    const auto mz_start = mz;
                    const auto dqs_cen_start = dqs_cen;
                    const auto dqs_bin_start = dqs_bin;
                    int i = 0;
                    for (int idx = *it_separators; idx < *(it_separators + 1); idx++)
                    {
                        Y[i] = treatedData.dataPoints[idx].y;
                        X[i] = treatedData.dataPoints[idx].x;
                        df[i] = treatedData.dataPoints[idx].df;
                        mz[i] = treatedData.dataPoints[idx].mz;
                        dqs_cen[i] = treatedData.dataPoints[idx].dqsCentroid;
                        dqs_bin[i] = treatedData.dataPoints[idx].dqsBinning;
                        i++;
                    }

                    // perform log-transform on Y
                    std::transform(y_start, y_start + n, ylog_start, [](float y)
                                   { return std::log(y); });
                    runningRegression_static(y_start, ylog_start, df_start, n, validRegressions, validRegressionsIndex);
                    if (validRegressionsIndex == 0)
                    {
                        continue; // no valid peaks
                    }
                    mz[0] = -255.f; // this is to calculate mz as weighted mean
                    createPeaks_static(all_peaks, validRegressions, validRegressionsIndex, y_start, mz_start, rt_start, df_start, dqs_cen_start, dqs_bin_start, nullptr, -1);
                }
                else
                {
                    // DYNAMIC APPROACH
                    alignas(float) float *Y = new float[n];
                    alignas(float) float *Ylog = new float[n];
                    alignas(float) float *X = new float[n];
                    alignas(bool) bool *df = new bool[n];
                    alignas(float) float *mz = new float[n];
                    alignas(float) float *dqs_cen = new float[n];
                    alignas(float) float *dqs_bin = new float[n];
                    std::vector<validRegression_static> validRegressions;

                    // iterator to the start
                    const auto y_start = Y;
                    const auto ylog_start = Ylog;
                    const auto rt_start = X;
                    const auto df_start = df;
                    const auto mz_start = mz;
                    const auto dqs_cen_start = dqs_cen;
                    const auto dqs_bin_start = dqs_bin;

                    int i = 0;
                    assert(n == *(it_separators + 1) - *it_separators);
                    for (int idx = *it_separators; idx < *(it_separators + 1); idx++)
                    {
                        Y[i] = treatedData.dataPoints[idx].y;
                        X[i] = treatedData.dataPoints[idx].x;
                        df[i] = treatedData.dataPoints[idx].df;
                        mz[i] = treatedData.dataPoints[idx].mz;
                        dqs_cen[i] = treatedData.dataPoints[idx].dqsCentroid;
                        dqs_bin[i] = treatedData.dataPoints[idx].dqsBinning;
                        i++;
                    }

                    // perform log-transform on Y
                    std::transform(y_start, y_start + n, ylog_start, [](float y)
                                   { return std::log(y); });
                    runningRegression(y_start, ylog_start, df_start, n, validRegressions);
                    if (validRegressions.empty())
                    {
                        continue; // no valid peaks
                    }
                    mz[0] = -255.f; // this is to calculate mz as weighted mean
                    createPeaks(all_peaks, validRegressions, y_start, mz_start, rt_start, df_start, dqs_cen_start, dqs_bin_start, nullptr, -1);
                    delete[] Y;
                    delete[] Ylog;
                    delete[] X;
                    delete[] df;
                    delete[] mz;
                    delete[] dqs_cen;
                    delete[] dqs_bin;
                }
            }
        }
#pragma endregion "find peaks"

#pragma region "running regression"
        void qPeaks::runningRegression(
            const float *y_start,
            const float *ylog_start,
            const bool *df_start,
            const int n, // number of data points
            std::vector<validRegression_static> &validRegressions)
        {
            const int maxScale = std::min(global_maxScale, (int)(n - 1) / 2);
            validRegressions.reserve(calculateNumberOfRegressions(n));
            for (int scale = 2; scale <= maxScale; scale++)
            {
                const int k = 2 * scale + 1;                           // window size
                const int n_segments = n - k + 1;                      // number of segments, i.e. regressions considering the array size
                alignas(__m128) __m128 *beta = new __m128[n_segments]; // coefficients matrix
                convolve_dynamic(scale, ylog_start, n, beta);          // do the regression
                validateRegressions(beta, n_segments, y_start, ylog_start, df_start, scale, validRegressions);
            } // end for scale loop
            mergeRegressionsOverScales(validRegressions, y_start, ylog_start, df_start);
        } // end runningRegression
#pragma endregion "running regression"

#pragma region "running regression static"
        void
        qPeaks::runningRegression_static(
            const float *y_start,
            const float *ylog_start,
            const bool *df_start,
            const int n,
            validRegression_static *validRegressions,
            int &validRegressionsIndex)
        {
            int maxScale = std::min(global_maxScale, (int)(n - 1) / 2);

            for (int scale = 2; scale <= maxScale; scale++)
            {
                const int k = 2 * scale + 1;                 // window size
                alignas(__m128) __m128 beta[512];            // coefficients matrix
                convolve_static(scale, ylog_start, n, beta); // do the regression
                const int n_segments = n - k + 1;            // number of segments, i.e. regressions considering the number of data points
                validateRegressions_static(beta, n_segments, y_start, ylog_start, df_start, scale, validRegressionsIndex, validRegressions);
            } // end for scale loop
            mergeRegressionsOverScales_static(validRegressions, validRegressionsIndex, y_start, ylog_start, df_start);
        }
#pragma endregion "running regression static"

#pragma region validateRegressions
        void qPeaks::validateRegressions(
            const __m128 *beta,      // coefficients matrix
            const int n_segments,    // number of segments, i.e. regressions
            const float *y_start,    // pointer to the start of the Y matrix
            const float *ylog_start, // pointer to the start of the Ylog matrix
            const bool *df_start,    // degree of freedom vector, 0: interpolated, 1: measured
            const int scale,         // scale, i.e., the number of data points in a half window excluding the center point
            std::vector<validRegression_static> &validRegressions)
        {
            // declare constants
            const float inverseMatrix_2_2 = invArray[scale][4]; // variance of the quadratic term left side of the peak

            // declare variables
            std::vector<validRegression_static> validRegressionsTmp; // temporary vector to store valid regressions <index, apex_position>

            // iterate columwise over the coefficients matrix beta
            for (int i = 0; i < n_segments; i++)
            {
                const __m128 &coeff = beta[i]; // coefficient register from beta @ i
                int df_sum = 0;                // sum of the degree of freedom
                float apex_position = 0.f;     // apex position
                int left_limit = 0;            // left limit of the peak
                int right_limit = 0;           // right limit of the peak
                float area = 0.f;              // peak area
                float uncertainty_area = 0.f;  // uncertainty of the peak area
                float uncertainty_pos = 0.f;   // uncertainty of the peak position
                float uncertainty_height = 0.f;

                // validate the regression
                if (!validateRegressions_testseries(inverseMatrix_2_2, i, scale, df_start, y_start,
                                                    ylog_start, coeff, df_sum, apex_position,
                                                    left_limit, right_limit, area, uncertainty_area,
                                                    uncertainty_pos, uncertainty_height))
                {
                    continue; // invalid regression
                }

                // at this point, the peak is validated
                /*
                  Add to a temporary vector of valid regressions:
                  This block of code adds the valid peak to a temporary vector of valid regressions.
                  It calculates the left and right limits of the peak based on the valley position.
                  Then it stores the index of the valid regression in the temporary vector of valid regressions.
                */
                validRegression_static vr;
                vr.index_x0 = i + scale;                      // index of the center of the window (x==0) in the Y matrix
                vr.scale = scale;                             // scale
                vr.df = df_sum - 4;                           // df
                vr.apex_position = apex_position + i + scale; // apex position in x-axis 0:n
                vr.mse = 0;                                   // initial MSE
                vr.coeff = coeff;                             // coefficients register
                vr.isValid = true;                            // isValid
                vr.left_limit = left_limit;                   // left_limit
                vr.right_limit = right_limit;                 // right_limit
                vr.area = area;                               // peak area
                vr.uncertainty_area = uncertainty_area;       // uncertainty of the peak area
                vr.uncertainty_height = uncertainty_height;   // uncertainty of the peak height
                vr.uncertainty_pos = uncertainty_pos;         // uncertainty of the peak position
                validRegressionsTmp.push_back(vr);            // push to the temporary vector of valid regressions
            } // end for loop
            // early return if no or only one valid peak
            if (validRegressionsTmp.size() < 2)
            {
                if (validRegressionsTmp.empty())
                {
                    return; // no valid peaks
                }
                validRegressions.push_back(std::move(validRegressionsTmp[0]));
                return; // not enough peaks to form a group
            }
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

            std::vector<std::vector<validRegression_static>> regressionGroups; // vector of vectors of valid regressions, i.e., groups of valid regressions

            auto it_peak = validRegressionsTmp.begin(); // iterator for the peak
            auto it_peak_next = std::next(it_peak);     // iterator for the next peak

            while (it_peak_next != validRegressionsTmp.end()) // iterate over the temporary vector of valid regressions
            {
                // check if the difference between two peak apexes is less than 4 (Nyquist Shannon Sampling Theorem, separation of two maxima), or if the apex of a peak is within the window of the other peak (Overlap of two maxima)
                if (
                    std::abs(it_peak->apex_position - it_peak_next->apex_position) > 4 && // Nyquist Shannon Sampling Theorem, separation of two maxima
                    it_peak->apex_position < it_peak_next->left_limit &&                  // Left peak is not within the window of the right peak
                    it_peak_next->apex_position > it_peak->right_limit                    // Right peak is not within the window of the left peak
                )
                {                                                                                                      // the two regressions differ, i.e. create a new group and move all regressions from start to it_peak to this group
                    regressionGroups.push_back(std::vector<q::Algorithms::qPeaks::validRegression_static>());          // create a new group
                    std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back())); // move all regressions from start to it_peak to this group
                    it_peak_next = validRegressionsTmp.erase(validRegressionsTmp.begin(), it_peak_next);               // erase the moved regressions from the temporary vector of valid regressions
                }
                it_peak = it_peak_next;
                ++it_peak_next;
                if (it_peak_next == validRegressionsTmp.end())
                {                                                                                                      // the last peak is reached
                    regressionGroups.push_back(std::vector<q::Algorithms::qPeaks::validRegression_static>());          // create a new group
                    std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back())); // move all regressions from start to it_peak to this group
                }
            } // end while loop

            /*
              Survival of the Fittest Filter:
              This block of code implements the survival of the fittest filter. It selects the peak with the lowest mean squared error (MSE) as the representative of the group. If the group contains only one peak, the peak is directly pushed to the valid regressions. If the group contains multiple peaks, the peak with the lowest MSE is selected as the representative of the group and pushed to the valid regressions.
            */
            for (auto &group : regressionGroups)
            {
                if (group.size() == 1)
                { // already isolated peak => push to valid regressions
                    validRegressions.push_back(std::move(group.front()));
                }
                else
                { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
                    calcExtendedMse(y_start, group, df_start);
                    for (auto &regression : group)
                    {
                        if (regression.isValid)
                        {
                            validRegressions.push_back(std::move(regression));
                        }
                    }
                } // end if; single item or group with multiple members
            } // end for loop (group in vector of groups)
        } // end validateRegressions
#pragma endregion validateRegressions

#pragma region "validate regressions static"
        void
        qPeaks::validateRegressions_static(
            const __m128 *beta,
            const int n_segments,
            const float *y_start,
            const float *ylog_start,
            const bool *df_start,
            const int scale,
            int &validRegressionsIndex,
            validRegression_static *validRegressions)
        {
            // declare constants
            const float inverseMatrix_2_2 = invArray[scale][4]; // variance of the quadratic term left side of the peak

            // declare variables
            validRegression_static validRegressionsTmp[512]; // temporary vector to store valid regressions initialized with random states
            int validRegressionsIndexTmp = 0;                // index of the valid regressions

            // iterate columwise over the coefficients matrix beta
            for (int i = 0; i < n_segments; i++)
            {
                const __m128 &coeff = beta[i]; // coefficient register from beta @ i
                int df_sum = 0;                // sum of the degree of freedom
                float apex_position = 0.f;     // apex position
                int left_limit = 0;            // left limit of the peak
                int right_limit = 0;           // right limit of the peak
                float area = 0.f;              // peak area
                float uncertainty_area = 0.f;  // uncertainty of the peak area
                float uncertainty_height = 0.f;
                float uncertainty_pos = 0.f; // uncertainty of the peak position

                // validate the regression
                if (!validateRegressions_testseries(inverseMatrix_2_2, i, scale, df_start, y_start,
                                                    ylog_start, coeff, df_sum, apex_position,
                                                    left_limit, right_limit, area, uncertainty_area,
                                                    uncertainty_pos, uncertainty_height))
                {
                    continue; // invalid regression
                }

                // at this point, the peak is validated
                /*
                  Add to a temporary vector of valid regressions:
                  This block of code adds the valid peak to a temporary vector of valid regressions. It calculates the left and right limits of the peak based on the valley position. Then it stores the index of the valid regression in the temporary vector of valid regressions.
                */
                validRegressionsTmp[validRegressionsIndexTmp].index_x0 = i + scale;                      // index of the center of the window (x==0) in the Y matrix
                validRegressionsTmp[validRegressionsIndexTmp].scale = scale;                             // scale
                validRegressionsTmp[validRegressionsIndexTmp].df = df_sum - 4;                           // df
                validRegressionsTmp[validRegressionsIndexTmp].apex_position = apex_position + i + scale; // apex position in x-axis 0:n
                validRegressionsTmp[validRegressionsIndexTmp].mse = 0;                                   // initial MSE
                validRegressionsTmp[validRegressionsIndexTmp].coeff = coeff;                             // coefficients register
                validRegressionsTmp[validRegressionsIndexTmp].isValid = true;                            // isValid
                validRegressionsTmp[validRegressionsIndexTmp].left_limit = left_limit;                   // left_limit
                validRegressionsTmp[validRegressionsIndexTmp].right_limit = right_limit;                 // right_limit
                validRegressionsTmp[validRegressionsIndexTmp].area = area;                               // peak area
                validRegressionsTmp[validRegressionsIndexTmp].uncertainty_area = uncertainty_area;       // uncertainty of the peak area
                validRegressionsTmp[validRegressionsIndexTmp].uncertainty_height = uncertainty_height;   // uncertainty of the peak height
                validRegressionsTmp[validRegressionsIndexTmp].uncertainty_pos = uncertainty_pos;         // uncertainty of the peak position
                validRegressionsIndexTmp++;
            } // end for loop
            // early return if no or only one valid peak
            if (validRegressionsIndexTmp < 2)
            {
                if (validRegressionsIndexTmp == 1)
                {
                    *(validRegressions + validRegressionsIndex) = validRegressionsTmp[0];
                    validRegressionsIndex++;
                }
                return; // not enough peaks to form a group
            }

            // lambda function to process a group of valid regressions, i.e., find the peak with the lowest MSE and push it to the valid regressions
            auto processGroup = [this, &validRegressions, &validRegressionsTmp, &validRegressionsIndex, y_start, df_start](int i, int start_index_group)
            {
                int group_size = i - start_index_group + 1; // size of the group
                if (group_size == 1)
                { // single item group
                    *(validRegressions + validRegressionsIndex) = validRegressionsTmp[start_index_group];
                    validRegressionsIndex++;
                }
                else
                { // multiple item group
                    // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
                    validRegression_static *regression_start = &validRegressionsTmp[start_index_group]; // start of the group
                    calcExtendedMse_static(y_start, regression_start, group_size, df_start);            // calculate the extended MSE
                    for (int j = 0; j < group_size; j++)
                    {
                        if (regression_start[j].isValid)
                        {
                            *(validRegressions + validRegressionsIndex) = regression_start[j]; // push the peak to the valid regressions
                            validRegressionsIndex++;
                        }
                    }
                }
            };

            /*
              Grouping:
              This block of code implements the grouping. It groups the valid peaks based on the apex positions. Peaks are defined as similar, i.e., members of the same group, if they fullfill at least one of the following conditions:
              - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
              - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
            */
            const int last_valid_index = validRegressionsIndexTmp - 1; // last valid index in the temporary vector of valid regressions
            int start_index_group = 0;                                 // start index of the group
            for (int i = 0; i < last_valid_index; i++)
            {
                if (
                    std::abs(validRegressionsTmp[i].apex_position - validRegressionsTmp[i + 1].apex_position) > 4 && // difference between two peak apexes is greater than 4 (Nyquist Shannon Sampling Theorem, separation of two maxima)
                    validRegressionsTmp[i].apex_position < validRegressionsTmp[i + 1].left_limit &&                  // left peak is not within the window of the right peak
                    validRegressionsTmp[i + 1].apex_position > validRegressionsTmp[i].right_limit)                   // right peak is not within the window of the left peak
                {                                                                                                    // the two regressions differ,
                    processGroup(i, start_index_group);                                                              // process the group
                    start_index_group = i + 1;                                                                       // start index of the next group
                    if (i == last_valid_index - 1)
                    { // if last round compare, add the last regression to the valid regressions
                        *(validRegressions + validRegressionsIndex) = validRegressionsTmp[i + 1];
                        validRegressionsIndex++;
                    }
                }
                else
                {
                    if (i == last_valid_index - 1)
                    {                                                      // if last round compare
                        processGroup(last_valid_index, start_index_group); // process the group
                    }
                }
            }
        }
#pragma endregion "validate regressions static"

#pragma region "validate regression test series"
        bool qPeaks::validateRegressions_testseries(
            const float inverseMatrix_2_2,
            const int i,
            const int scale,
            const bool *df_start,
            const float *y_start,
            const float *ylog_start,
            const __m128 &coeff,
            int &df_sum,
            float &apex_position,
            int &left_limit,
            int &right_limit,
            float &area,
            float &uncertainty_area,
            float &uncertainty_pos,
            float &uncertainty_height)
        {
            /*
                Degree of Freedom Filter:
                This block of code implements the degree of freedom filter.
                It calculates the degree of freedom based df vector. If the
                degree of freedom is less than 5, the loop continues to the next
                iteration. The value 5 is chosen as the minimum number of data
                points required to fit a quadratic regression model.
              */
            df_sum = calcDF(df_start, i, 2 * scale + i); // calculate the sum of the degree of freedom (df_sum)
            if (df_sum < 5)
            {
                return false; // degree of freedom less than 5; i.e., less then 5 measured data points
            }

            /*
              Apex and Valley Position Filter:
              This block of code implements the apex and valley position filter.
              It calculates the apex and valley positions based on the coefficients
              matrix B. If the apex is outside the data range, the loop continues
              to the next iteration. If the apex and valley positions are too close
              to each other, the loop continues to the next iteration.
            */
            float valley_position;
            if (!calculateApexAndValleyPositions(coeff, scale, apex_position, valley_position))
            {
                return false; // invalid apex and valley positions
            }

            /*
              Degree of Freedom Filter:
              This block of code implements the degree of freedom filter. It calculates the
              degree of freedom based df vector. If the degree of freedom is less than 5,
              the loop continues to the next iteration. The value 5 is chosen as the
              minimum number of data points required to fit a quadratic regression model.
            */
            left_limit = (valley_position < 0) ? std::max(i, static_cast<int>(valley_position) + i + scale) : i;
            right_limit = (valley_position > 0) ? std::min(i + 2 * scale, static_cast<int>(valley_position) + i + scale) : i + 2 * scale;
            df_sum = calcDF(df_start, left_limit, right_limit); // update the degree of freedom considering the left and right limits
            if (df_sum < 5)
            {
                return false; // degree of freedom less than 5; i.e., less then 5 measured data points
            }

            /*
              Area Pre-Filter:
              This test is used to check if the later-used arguments for exp and erf
              functions are within the valid range, i.e., |x^2| < 25. If the test fails,
              the loop continues to the next iteration.
              x is in this case -apex_position * b1 / 2 and -valley_position * b1 / 2.
            */
            if (apex_position * ((float *)&coeff)[1] > 50 || valley_position * ((float *)&coeff)[1] < -50)
            {
                return false; // invalid area pre-filter
            }

            /*
              Apex to Edge Filter:
              This block of code implements the apex to edge filter. It calculates
              the ratio of the apex signal to the edge signal and ensures that the
              ratio is greater than 2. This is a pre-filter for later
              signal-to-noise ratio checkups.
            */
            float apexToEdge = 0.f;
            if (!isValidApexToEdge(apex_position, scale, i, y_start, apexToEdge))
            {
                return false; // invalid apex to edge ratio
            }

            /*
              Quadratic Term Filter:
              This block of code implements the quadratic term filter. It calculates
              the mean squared error (MSE) between the predicted and actual values.
              Then it calculates the t-value for the quadratic term. If the t-value
              is less than the corresponding value in the tValuesArray, the quadratic
              term is considered statistically insignificant, and the loop continues
              to the next iteration.
            */
            const float mse = calcSSE(-scale, scale, coeff, ylog_start + i) / (df_sum - 4); // mean squared error

            if (!isValidQuadraticTerm(coeff, inverseMatrix_2_2, mse, df_sum))
            {
                return false; // statistical insignificance of the quadratic term
            }
            /*
              Height Filter:
              This block of code implements the height filter. It calculates the height
              of the peak based on the coefficients matrix B. Then it calculates the
              uncertainty of the height based on the Jacobian matrix and the variance-covariance
              matrix of the coefficients. If the height is statistically insignificant,
              the loop continues to the next iteration.
            */
            // float uncertainty_height = 0.0;
            // at this point without height, i.e., to get the real uncertainty
            // multiply with height later. This is done to avoid exp function at this point
            if (!isValidPeakHeight(mse, i, scale, apex_position, valley_position, df_sum, apexToEdge, uncertainty_height))
            {
                return false; // statistical insignificance of the height
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
            if (!isValidPeakArea(coeff, mse, scale, df_sum, area, uncertainty_area))
            {
                return false; // statistical insignificance of the area
            }

            /*
              Chi-Square Filter:
              This block of code implements the chi-square filter. It calculates the chi-square
              value based on the weighted chi squared sum of expected and measured y values in
              the exponential domain. If the chi-square value is less than the corresponding
              value in the chiSquareArray, the loop continues to the next iteration.
            */
            const float chiSquare = calcSSE(-scale, scale, coeff, y_start + i, true, true);
            if (chiSquare < chiSquareArray[df_sum - 5])
            {
                return false; // statistical insignificance of the chi-square value
            }

            calcUncertaintyPosition(mse, coeff, apex_position, scale, uncertainty_pos);

            return true;
        }
#pragma endregion "validate regression test series"

#pragma region mergeRegressionsOverScales
        void
        qPeaks::mergeRegressionsOverScales(
            std::vector<validRegression_static> &validRegressions,
            const float *y_start,
            const float *ylog_start,
            const bool *df_start)
        {
            if (validRegressions.empty())
            {
                return; // no valid peaks at all
            }

            if (validRegressions.size() == 1)
            {
                return; // only one valid regression, i.e., no further grouping required; validRegressions is already fine.
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
            for (auto it_new_peak = validRegressions.begin(); it_new_peak != validRegressions.end(); ++it_new_peak)
            {
                const double left_limit = it_new_peak->left_limit;          // left limit of the current peak regression window in the Y matrix
                const double right_limit = it_new_peak->right_limit;        // right limit of the current peak regression window in the Y matrix
                double grpMSE = 0;                                          // group mean squared error
                int grpDF = 0;                                              // group degree of freedom
                int numPeaksInGroup = 0;                                    // number of peaks in the group
                auto it_ref_peak = validRegressions.begin();                // iterator for the reference peak, i.e., a valid peak to compare with the new peak
                std::vector<decltype(it_ref_peak)> validRegressionsInGroup; // vector of iterators

                // iterate over the validRegressions vector till the new peak
                for (; it_ref_peak < it_new_peak; ++it_ref_peak)
                {
                    if (!it_ref_peak->isValid)
                    {
                        continue; // skip the invalid peaks
                    }
                    if ( // check for the overlap of the peaks
                        (
                            it_ref_peak->apex_position > left_limit &&   // ref peak matches the left limit
                            it_ref_peak->apex_position < right_limit) || // ref peak matches the right limit
                        (
                            it_new_peak->apex_position > it_ref_peak->left_limit && // new peak matches the left limit
                            it_new_peak->apex_position < it_ref_peak->right_limit)) // new peak matches the right limit
                    {
                        if (it_ref_peak->mse == 0.0)
                        { // calculate the mse of the ref peak
                            it_ref_peak->mse = calcSSE(
                                                   (it_ref_peak->left_limit) - it_ref_peak->index_x0,  // left limit of the ref peak
                                                   (it_ref_peak->right_limit) - it_ref_peak->index_x0, // right limit of the ref peak
                                                   it_ref_peak->coeff,                                 // regression coefficients
                                                   y_start + (int)it_ref_peak->left_limit,             // pointer to the start of the Y matrix
                                                   true) /
                                               it_ref_peak->df;
                        }
                        grpDF += it_ref_peak->df;                     // add the degree of freedom
                        grpMSE += it_ref_peak->mse * it_ref_peak->df; // add the sum of squared errors
                        numPeaksInGroup++;                            // increment the number of peaks in the group
                        // add the iterator of the ref peak to a vector of iterators
                        validRegressionsInGroup.push_back(it_ref_peak);
                    }
                } // end for loop, inner loop, it_ref_peak

                if (grpDF > 0)
                {
                    grpMSE /= grpDF;
                }
                else
                {
                    continue; // no peaks in the group, i.e., the current peak stays valid
                }

                if (it_new_peak->mse == 0.0)
                { // calculate the mse of the current peak
                    it_new_peak->mse = calcSSE(
                                           (it_new_peak->left_limit) - it_new_peak->index_x0,  // left limit of the new peak
                                           (it_new_peak->right_limit) - it_new_peak->index_x0, // right limit of the new peak
                                           it_new_peak->coeff,                                 // regression coefficients
                                           y_start + (int)it_new_peak->left_limit,             // pointer to the start of the Y matrix
                                           true) /
                                       it_new_peak->df;
                }

                if (numPeaksInGroup == 1)
                {
                    // calculate the extended MSE using the current peak and the ref peak and set the worse one to invalid
                    // create a temporary std::vector<std::unique_ptr<validRegression>> with the new peak and the ref peak
                    std::vector<validRegression_static> tmpRegressions;
                    tmpRegressions.push_back(std::move(*it_new_peak));
                    tmpRegressions.push_back(std::move(validRegressionsInGroup[0][0]));
                    calcExtendedMse(y_start, tmpRegressions, df_start);
                    // Move the unique_ptrs back to validRegressionsInGroup
                    validRegressionsInGroup[0][0] = std::move(tmpRegressions[1]);
                    *it_new_peak = std::move(tmpRegressions[0]);
                    continue;
                }
                if (it_new_peak->mse < grpMSE)
                {
                    // Set isValid to false for the candidates from the group
                    for (auto it_ref_peak : validRegressionsInGroup)
                    {
                        it_ref_peak->isValid = false;
                    }
                }
                else
                { // Set isValid to false for the current peak
                    it_new_peak->isValid = false;
                }
            } // end for loop, outer loop, it_current_peak

            // Remove the peaks with isValid == false from the validRegressions
            validRegressions.erase(std::remove_if(validRegressions.begin(), validRegressions.end(),
                                                  [](const auto &peak)
                                                  { return !peak.isValid; }),
                                   validRegressions.end());
        } // end mergeRegressionsOverScales
#pragma endregion mergeRegressionsOverScales

#pragma region "merge regressions over scales static"
        void
        qPeaks::mergeRegressionsOverScales_static(
            validRegression_static *validRegressions,
            const int n_regressions,
            const float *y_start,
            const float *ylog_start,
            const bool *df_start)
        {
            if (n_regressions == 0)
            {
                return; // no valid peaks at all
            }

            if (n_regressions == 1)
            {
                return; // only one valid regression, i.e., no further grouping required; validRegressions is already fine.
            }

            /*
              Grouping Over Scales:
              This block of code implements the grouping over scales. It groups the valid peaks based
              on the apex positions. Peaks are defined as similar, i.e., members of the same group,
              if they fullfill at least one of the following conditions:
              - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
              - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
            */
            // iterate over the validRegressions vector
            for (int i_new_peak = 1; i_new_peak < n_regressions; i_new_peak++)
            {
                if (!validRegressions[i_new_peak].isValid)
                {
                    continue; // skip the invalid peaks
                }
                const int current_scale = validRegressions[i_new_peak].scale;                   // scale of the current peak
                const int current_left_limit = validRegressions[i_new_peak].left_limit;         // left limit of the current peak regression window in the Y matrix
                const int current_right_limit = validRegressions[i_new_peak].right_limit;       // right limit of the current peak regression window in the Y matrix
                const float current_apex_position = validRegressions[i_new_peak].apex_position; // apex position of the current peak
                std::vector<int> validRegressionsInGroup;                                       // vector of indices of valid regressions in the group
                // iterate over the validRegressions vector till the new peak
                for (int i_ref_peak = 0; i_ref_peak < i_new_peak; i_ref_peak++)
                {
                    if (!validRegressions[i_ref_peak].isValid)
                    {
                        continue; // skip the invalid peaks
                    }
                    if (validRegressions[i_ref_peak].scale >= current_scale)
                    {
                        break; // skip the peaks with a scale greater or equal to the current scale
                    }
                    // check for overlaps
                    if (
                        (
                            validRegressions[i_ref_peak].apex_position > current_left_limit &&   // ref peak matches the left limit
                            validRegressions[i_ref_peak].apex_position < current_right_limit) || // ref peak matches the right limit
                        (
                            current_apex_position > validRegressions[i_ref_peak].left_limit && // new peak matches the left limit
                            current_apex_position < validRegressions[i_ref_peak].right_limit)) // new peak matches the right limit
                    {                                                                          // overlap detected
                        validRegressionsInGroup.push_back(i_ref_peak);                         // add the index of the ref peak to the vector of indices
                        continue;                                                              // continue with the next ref peak
                    }
                } // end for loop, inner loop, i_ref_peak

                if (validRegressionsInGroup.size() == 0)
                {
                    continue; // no peaks in the group, i.e., the current peak stays valid
                }

                if (validRegressionsInGroup.size() == 1)
                { // comparison of two regressions just with different scale.
                    calcExtendedMsePair_static(y_start, &validRegressions[validRegressionsInGroup[0]], &validRegressions[i_new_peak], df_start);
                    continue; // continue with the next new peak
                }

                // comparison of the new regression (high) with multiple ref regressions (low)
                calcExtendedMseOverScales_static(y_start, validRegressions, validRegressionsInGroup, i_new_peak);
            }
        } // end mergeRegressionsOverScales_static
#pragma endregion "merge regressions over scales static"

#pragma region "create peaks"
        void qPeaks::createPeaks(
            std::vector<DataType::Peak> &peaks,
            const std::vector<validRegression_static> &validRegressions,
            const float *y_start,
            const float *mz_start,
            const float *rt_start,
            const bool *df_start,
            const float *dqs_cen,
            const float *dqs_bin,
            const float *dqs_peak,
            const int scanNumber)
        {
            // iterate over the validRegressions vector
            for (auto &regression : validRegressions)
            {
                if (regression.isValid)
                {
                    addPeakProperties(peaks, regression, y_start, mz_start, rt_start,
                                      df_start, dqs_cen, dqs_bin, dqs_peak, scanNumber);
                }
            }
        }

        void
        qPeaks::createPeaks_static(
            std::vector<DataType::Peak> &peaks,
            validRegression_static *validRegressions,
            const int validRegressionsIndex,
            const float *y_start,
            const float *mz_start,
            const float *rt_start,
            const bool *df_start,
            const float *dqs_cen,
            const float *dqs_bin,
            const float *dqs_peak,
            const int scanNumber)
        {
            // iterate over the validRegressions vector
            for (int i = 0; i < validRegressionsIndex; i++)
            {
                auto &regression = validRegressions[i];
                if (regression.isValid)
                {
                    addPeakProperties(peaks, regression, y_start, mz_start, rt_start,
                                      df_start, dqs_cen, dqs_bin, dqs_peak, scanNumber);
                }
            }
        }
#pragma endregion "create peaks"

#pragma region "add peak properties"
        void
        qPeaks::addPeakProperties(
            std::vector<DataType::Peak> &peaks,
            const validRegression_static &regression,
            const float *y_start,
            const float *mz_start,
            const float *rt_start,
            const bool *df_start,
            const float *dqs_cen,
            const float *dqs_bin,
            const float *dqs_peak,
            const int scanNumber)
        {
            // Lambda function to calculate the weighted mean and variance
            auto calcWeightedMeanAndVariance = [](const float *x, const float *w, const bool *df,
                                                  const int left_limit, const int right_limit) -> std::pair<float, float>
            {
                // weighted mean using y_start as weighting factor and left_limit right_limit as range
                int n = right_limit - left_limit + 1;
                float mean_wt = 0.0; // mean of w
                for (int j = left_limit; j <= right_limit; j++)
                {
                    if (!*(df + j))
                    {
                        n--;
                        continue;
                    }
                    mean_wt += *(w + j);
                }
                mean_wt /= n;
                float sum_xw = 0.0;     // sum of x*w
                float sum_weight = 0.0; // sum of w
                for (int j = left_limit; j <= right_limit; j++)
                {
                    if (!*(df + j))
                    {
                        continue;
                    }
                    sum_xw += *(x + j) * *(w + j) / mean_wt;
                    sum_weight += *(w + j) / mean_wt;
                }
                float weighted_mean = sum_xw / sum_weight;
                float sum_Qxxw = 0.0; // sum of (x - mean)^2 * w
                for (int j = left_limit; j <= right_limit; j++)
                {
                    if (*(x + j) <= 0.f)
                    {
                        continue;
                    }
                    sum_Qxxw += (*(x + j) - weighted_mean) * (*(x + j) - weighted_mean) * *(w + j);
                }
                float uncertaintiy = std::sqrt(sum_Qxxw / sum_weight / n);
                return std::make_pair(weighted_mean, uncertaintiy);
            }; // end lambda function

            // create new peak object and push it to the peaks vector
            DataType::Peak peak;
            // peaks.push_back(std::make_unique<DataType::Peak>());

            // add scanNumber
            if (scanNumber != -1)
            {
                peak.sampleID = scanNumber;
            }

            // add height
            const __m128 coeff = regression.coeff;
            peak.height = exp_approx_d(((float *)&coeff)[0] + (regression.apex_position - regression.index_x0) * ((float *)&coeff)[1] * .5); // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2
            peak.heightUncertainty = regression.uncertainty_height * peak.height;

            // add area
            const float exp_b0 = exp_approx_d(((float *)&coeff)[0]); // exp(b0)
            peak.area = regression.area * exp_b0;
            peak.areaUncertainty = regression.uncertainty_area * exp_b0;

            // add rt
            if (*rt_start != -255.f)
            {
                // re-scale the apex position to x-axis
                const double rt0 = *(rt_start + (int)std::floor(regression.apex_position));
                const double drt = *(rt_start + (int)std::floor(regression.apex_position) + 1) - rt0;
                const double apex_position = rt0 + drt * (regression.apex_position - std::floor(regression.apex_position));
                peak.retentionTime = apex_position;
                peak.retentionTimeUncertainty = regression.uncertainty_pos * drt;
            }
            else
            {
                peak.retentionTime = *(rt_start + 1);
            }

            // add mz
            if (*mz_start != -255.f)
            {
                // re-scale the apex position to x-axis
                const double mz0 = *(mz_start + (int)std::floor(regression.apex_position));
                const double dmz = *(mz_start + (int)std::floor(regression.apex_position) + 1) - mz0;
                const double apex_position = mz0 + dmz * (regression.apex_position - std::floor(regression.apex_position));
                peak.mz = apex_position;
                peak.mzUncertainty = regression.uncertainty_pos * dmz;
            }
            else
            {
                std::pair<float, float> mz = calcWeightedMeanAndVariance(mz_start, y_start, df_start, regression.left_limit, regression.right_limit);
                peak.mz = mz.first;
                peak.mzUncertainty = mz.second;
            }

            // add dqs centroid
            if (dqs_cen != nullptr)
            { // weigthed mean using y_start as weighting factor
                peak.dqsCen = calcWeightedMeanAndVariance(dqs_cen, y_start, df_start, regression.left_limit, regression.right_limit).first;
            }
            else
            {
                peak.dqsCen = experfc(regression.uncertainty_area / regression.area, -1.0);
            }

            // add dqs bin
            if (dqs_bin != nullptr)
            { // weigthed mean using y_start as weighting factor
                peak.dqsBin = calcWeightedMeanAndVariance(dqs_bin, y_start, df_start, regression.left_limit, regression.right_limit).first;
            }

            // add dqs peak
            if (dqs_peak != nullptr)
            { // do not add dqs peak if nullptr is passed
            }
            else
            {
                peak.dqsPeak = experfc(regression.uncertainty_area / regression.area, -1.0);
            }

            // start and end indices
            peak.idxPeakStart = regression.left_limit;

            peak.idxPeakEnd = regression.right_limit;

            // peak.dqsPeak = regression.scale;

            peaks.push_back(peak);
        }

#pragma endregion "add peak properties"

#pragma region calcSSE
        float
        qPeaks::calcSSE(
            const int left_limit,
            const int right_limit,
            const __m128 &coeff,
            const float *y_start,
            const bool calc_EXP,
            const bool calc_CHISQ) const
        {
            // exception handling if the right limits is negative or the left limit is positive
            if (right_limit < 0 || left_limit > 0)
            {
                throw std::invalid_argument("right_limit must be positive and left_limit must be negative");
            }

            // exception if nullptr is passed
            if (y_start == nullptr)
            {
                throw std::invalid_argument("y_start must not be nullptr");
            }

            float result = 0.0f; // result variable

            const int n = right_limit - left_limit + 1;      // length of the result vector
            const int nFullSegments_left = -left_limit / 8;  // calculate the number of full segments of 8 elements per side
            const int nFullSegments_right = right_limit / 8; // calculate the number of full segments of 8 elements per side
            const int nRemaining_left = -left_limit % 8;     // calculate the number of remaining elements
            const int nRemaining_right = right_limit % 8;    // calculate the number of remaining elements

            // Load the coefficients
            const __m256 b0 = _mm256_set1_ps(((float *)&coeff)[0]);
            const __m256 b1 = _mm256_set1_ps(((float *)&coeff)[1]);
            const __m256 b2 = _mm256_set1_ps(((float *)&coeff)[2]);
            const __m256 b3 = _mm256_set1_ps(((float *)&coeff)[3]);

            // Lambda function to calculate the full segments
            auto calcFullSegments = [&](int j, const int dj, float i, const float i_sign, const int nFullSegments, const __m256 LINSPACE, const __m256 b_quadratic) -> void
            {
                for (int iSegment = 0; iSegment < nFullSegments; ++iSegment, i += 8.0f, j += dj)
                {
                    // Load 8 values of i directly as float
                    const __m256 x = _mm256_add_ps(_mm256_set1_ps(i * i_sign), LINSPACE); // x vector : -k to -k+7
                    // Calculate the yhat values
                    __m256 yhat = _mm256_fmadd_ps(_mm256_fmadd_ps(b_quadratic, x, b1), x, b0); // b0 + b1 * x + b2 * x^2
                    if (calc_EXP)
                    {
                        yhat = exp_approx_vf(yhat); // calculate the exp of the yhat values (if needed)
                    }
                    const __m256 y_vec = _mm256_loadu_ps(y_start + j); // Load 8 values from y considering the offset j
                    const __m256 diff = _mm256_sub_ps(y_vec, yhat);    // Calculate the difference between y and yhat
                    __m256 diff_sq = _mm256_mul_ps(diff, diff);        // Calculate the square of the difference
                    if (calc_CHISQ)
                    {
                        diff_sq = _mm256_div_ps(diff_sq, yhat); // Calculate the weighted square of the difference
                    }
                    result += sum8(diff_sq); // Calculate the sum of the squares and add it to the result
                }
            };

            // Lambda function to calculate the yhat values for the remaining elements
            auto calcRemaining = [&](const int nRemaining, const __m256 x, const int y_start_offset, const int y_end_offset, const int mask_offset, const __m256 b_quadratic) -> void
            {
                // Calculate the yhat values for the remaining elements
                __m256 yhat = _mm256_fmadd_ps(_mm256_fmadd_ps(b_quadratic, x, b1), x, b0); // b0 + b1 * x + b2 * x^2
                if (calc_EXP)
                {
                    yhat = exp_approx_vf(yhat); // calculate the exp of the yhat values (if needed)
                }
                // Load the remaining values from y
                alignas(float) float y_remaining[8] = {0.0f};
                std::copy(y_start - left_limit + y_start_offset, y_start - left_limit + y_end_offset, y_remaining);
                const __m256 y_vec = _mm256_loadu_ps(y_remaining);

                const __m256i mask = _mm256_cmpgt_epi32(_mm256_set1_epi32(nRemaining + mask_offset), LINSPACE_UP_INT_256); // mask for the remaining elements
                yhat = _mm256_blendv_ps(y_vec, yhat, _mm256_castsi256_ps(mask));                                           // set the remaining elements to zero

                const __m256 diff = _mm256_sub_ps(y_vec, yhat); // calculate the difference between y and yhat
                __m256 diff_sq = _mm256_mul_ps(diff, diff);     // calculate the square of the difference
                if (calc_CHISQ)
                {
                    diff_sq = _mm256_div_ps(diff_sq, yhat);                                              // calculate the weighted square of the difference
                    diff_sq = _mm256_blendv_ps(_mm256_setzero_ps(), diff_sq, _mm256_castsi256_ps(mask)); // set the nan values to zero
                }
                result += sum8(diff_sq); // calculate the sum of the squares and add it to the result
            };

            // Calculate the full segments
            calcFullSegments(0, 8, static_cast<float>(left_limit), 1.f, nFullSegments_left, LINSPACE_UP_POS_256, b2);
            calcFullSegments(n - 8, -8, static_cast<float>(-right_limit), -1.f, nFullSegments_right, LINSPACE_DOWN_NEG_256, b3);

            // Calculate the yhat values for the remaining elements
            if (nRemaining_left > 0)
            {
                __m256 x_left = _mm256_add_ps(_mm256_set1_ps(-static_cast<float>(nRemaining_left)), LINSPACE_UP_POS_256); // x vector : -nRemaining_left to -nRemaining_left+7
                calcRemaining(nRemaining_left, x_left, -nRemaining_left, 0, 0, b2);
            }

            if (nRemaining_right > 0)
            {
                calcRemaining(nRemaining_right, LINSPACE_UP_POS_256, 0, nRemaining_right + 1, 1, b3);
            }

            return result;
        } // end calcSSE
#pragma endregion calcSSE

#pragma region calcExtendedMse
        void
        qPeaks::calcExtendedMse(
            const float *y_start,                             // start of the measured data
            std::vector<validRegression_static> &regressions, // regressions to compare
            const bool *df_start)                             // degrees of freedom
        {
            /*
              The function consists of the following steps:
              1. Identify left and right limit of the grouped regression windows.
              2. Calculate the mean squared error (MSE) between the predicted and actual values.
              3. Identify the best regression based on the MSE and return the MSE and the index of the best regression.
            */
            // declare variables
            float best_mse = std::numeric_limits<float>::infinity();
            auto best_regression = regressions.begin();

            // step 1: identify left (smallest) and right (largest) limit of the grouped regression windows
            int left_limit = static_cast<int>(regressions.front().left_limit);
            int right_limit = static_cast<int>(regressions.front().right_limit);
            for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
            {
                left_limit = std::min(left_limit, static_cast<int>(regression->left_limit));
                right_limit = std::max(right_limit, static_cast<int>(regression->right_limit));
            }

            const int df_sum = calcDF(df_start, left_limit, right_limit);
            if (df_sum <= 4)
            {
                // set isValid to false for all regressions
                for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
                {
                    regression->isValid = false;
                }
                return; // not enough degrees of freedom
            }

            for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
            {
                // step 2: calculate the mean squared error (MSE) between the predicted and actual values
                const float mse = calcSSE(
                                      left_limit - regression->index_x0,  // left limit of the regression window (normalized scale)
                                      right_limit - regression->index_x0, // right limit of the regression window (normalized scale)
                                      regression->coeff,                  // regression coefficients
                                      y_start + left_limit,               // start of the measured data
                                      true) /                             // calculate the exp of the yhat values
                                  (df_sum - 4);

                // step 3: identify the best regression based on the MSE and return the MSE and the index of the best regression
                if (mse < best_mse)
                {
                    best_mse = mse;
                    regression->mse = mse;
                    best_regression = regression;
                }
                else
                {
                    regression->isValid = false;
                }
            } // end for loop (index in groupIndices)
            // set isValid to false for all regressions except the best one
            for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
            {
                if (regression != best_regression)
                {
                    regression->isValid = false;
                }
            }
        } // end qPeaks::calcExtendedMse
#pragma endregion calcExtendedMse

#pragma region calcExtendedMse_static
        void
        qPeaks::calcExtendedMse_static(
            const float *y_start,
            validRegression_static *regressions_start,
            const int n_regressions,
            const bool *df_start)
        {
            /*
              The function consists of the following steps:
              1. Identify left and right limit of the grouped regression windows.
              2. Calculate the mean squared error (MSE) between the predicted and actual values.
              3. Identify the best regression based on the MSE and return the MSE and the index of the best regression.
            */
            // declare variables
            float best_mse = std::numeric_limits<float>::infinity();

            // step 1: identify left (smallest) and right (largest) limit of the grouped regression windows
            unsigned int left_limit = regressions_start->left_limit;
            unsigned int right_limit = regressions_start->right_limit;
            for (int i = 1; i < n_regressions; i++)
            {
                left_limit = std::min(left_limit, (regressions_start + i)->left_limit);
                right_limit = std::max(right_limit, (regressions_start + i)->right_limit);
            }

            const int df_sum = calcDF(df_start, left_limit, right_limit);
            if (df_sum <= 4)
            {
                // set isValid to false for all regressions
                for (int i = 0; i < n_regressions; ++i)
                {
                    (regressions_start + i)->isValid = false;
                }
                return; // not enough degrees of freedom
            }

            for (int i = 0; i < n_regressions; ++i)
            {
                // step 2: calculate the mean squared error (MSE) between the predicted and actual values
                const float mse = calcSSE(
                                      left_limit - (regressions_start + i)->index_x0,  // left limit of the regression window (normalized scale)
                                      right_limit - (regressions_start + i)->index_x0, // right limit of the regression window (normalized scale)
                                      (regressions_start + i)->coeff,                  // regression coefficients
                                      y_start + left_limit,                            // start of the measured data
                                      true) /                                          // calculate the exp of the yhat values
                                  (df_sum - 4);

                // step 3: identify the best regression based on the MSE and return the MSE and the index of the best regression
                if (mse < best_mse)
                {
                    best_mse = mse;
                    (regressions_start + i)->mse = mse;
                }
                else
                {
                    (regressions_start + i)->isValid = false;
                }
            } // end for loop (index in groupIndices)
            // set isValid to false for all regressions except the best one
            for (int i = 0; i < n_regressions; ++i)
            {
                if ((regressions_start + i)->mse > best_mse)
                {
                    (regressions_start + i)->isValid = false;
                }
            }
        }
#pragma endregion calcExtendedMse_static

#pragma region calcExtendedMsePair_static
        void
        qPeaks::calcExtendedMsePair_static(
            const float *y_start,
            validRegression_static *low_scale_regression,
            validRegression_static *hi_scale_regression,
            const bool *df_start)
        {
            /*
              The function consists of the following steps:
              1. Identify left and right limit of the grouped regression windows.
              2. Calculate the mean squared error (MSE) between the predicted and actual values.
              3. Identify the best regression based on the MSE and return the MSE and the index of the best regression.
            */

            // step 1: identify left (smallest) and right (largest) limit of the grouped regression windows
            int left_limit = std::min(low_scale_regression->left_limit, hi_scale_regression->left_limit);
            int right_limit = std::max(low_scale_regression->right_limit, hi_scale_regression->right_limit);

            const int df_sum = calcDF(df_start, left_limit, right_limit);
            if (df_sum <= 4)
            {
                // set isValid to false for all regressions
                low_scale_regression->isValid = false;
                hi_scale_regression->isValid = false;
                return; // not enough degrees of freedom
            }

            // step 2: calculate the mean squared error (MSE) between the predicted and actual values
            const float mse_low_scale = calcSSE(
                                            left_limit - low_scale_regression->index_x0,  // left limit of the regression window (normalized scale)
                                            right_limit - low_scale_regression->index_x0, // right limit of the regression window (normalized scale)
                                            low_scale_regression->coeff,                  // regression coefficients
                                            y_start + left_limit,                         // start of the measured data
                                            true) /                                       // calculate the exp of the yhat values
                                        (df_sum - 4);

            const float mse_hi_scale = calcSSE(
                                           left_limit - hi_scale_regression->index_x0,  // left limit of the regression window (normalized scale)
                                           right_limit - hi_scale_regression->index_x0, // right limit of the regression window (normalized scale)
                                           hi_scale_regression->coeff,                  // regression coefficients
                                           y_start + left_limit,                        // start of the measured data
                                           true) /                                      // calculate the exp of the yhat values
                                       (df_sum - 4);

            // step 3: identify the best regression based on the MSE and return the MSE and the index of the best regression
            if (mse_low_scale < mse_hi_scale)
            {
                low_scale_regression->mse = mse_low_scale;
                hi_scale_regression->isValid = false;
            }
            else
            {
                hi_scale_regression->mse = mse_hi_scale;
                low_scale_regression->isValid = false;
            }
        }
#pragma endregion calcExtendedMsePair_static

#pragma region calcExtendedMseOverScales_static
        void qPeaks::calcExtendedMseOverScales_static(
            const float *y_start,
            validRegression_static *validRegressions,
            const std::vector<int> &validRegressionsInGroup,
            const int i_new_peak)
        {
            // comparison of the new regression (high) with multiple ref regressions (low)
            // check new_peak for mse
            if (validRegressions[i_new_peak].mse == 0.0)
            { // calculate the mse of the current peak
                validRegressions[i_new_peak].mse = calcSSE(
                                                       validRegressions[i_new_peak].left_limit - validRegressions[i_new_peak].index_x0,  // left limit of the new peak (normalized scale)
                                                       validRegressions[i_new_peak].right_limit - validRegressions[i_new_peak].index_x0, // right limit of the new peak (normalized scale)
                                                       validRegressions[i_new_peak].coeff,                                               // regression coefficients
                                                       y_start + validRegressions[i_new_peak].left_limit,
                                                       true) /
                                                   validRegressions[i_new_peak].df;
            }
            // calculate the group sse
            float groupSSE = 0.0f;
            int groupDF = 0;
            // iterate through the group of reference peaks
            for (int i_ref_peak : validRegressionsInGroup)
            {
                if (validRegressions[i_ref_peak].mse == 0.0)
                { // calculate the mse of the ref peak
                    validRegressions[i_ref_peak].mse = calcSSE(
                                                           validRegressions[i_ref_peak].left_limit - validRegressions[i_ref_peak].index_x0,  // left limit of the ref peak (normalized scale)
                                                           validRegressions[i_ref_peak].right_limit - validRegressions[i_ref_peak].index_x0, // right limit of the ref peak (normalized scale)
                                                           validRegressions[i_ref_peak].coeff,                                               // regression coefficients
                                                           y_start + validRegressions[i_ref_peak].left_limit,
                                                           true) /
                                                       validRegressions[i_ref_peak].df;
                }
                groupSSE += validRegressions[i_ref_peak].mse * validRegressions[i_ref_peak].df;
                groupDF += validRegressions[i_ref_peak].df;
            }
            // compare mse of the new peak with the group mse
            if (validRegressions[i_new_peak].mse < groupSSE / groupDF)
            {
                // Set isValid to false for the candidates from the group
                for (int i_ref_peak : validRegressionsInGroup)
                {
                    validRegressions[i_ref_peak].isValid = false;
                }
            }
            else
            { // Set isValid to false for the current peak
                validRegressions[i_new_peak].isValid = false;
            }
        }
#pragma endregion calcExtendedMseOverScales_static

#pragma region calcDF
        int qPeaks::calcDF(
            const bool *df_start,     // start of the degrees of freedom
            unsigned int left_limit,  // left limit
            unsigned int right_limit) // right limit
        {
            unsigned int degreesOfFreedom = 0;
            for (size_t i = left_limit; i < right_limit + 1; i++)
            {
                if (df_start[i])
                {
                    ++degreesOfFreedom;
                }
            }
            return degreesOfFreedom;
            // return std::accumulate(
            //     df_start + left_limit,
            //     df_start + right_limit + 1,
            //     0,
            //     [](int sum, bool p)
            //     { return sum + p; });
        } // end calcDF
#pragma endregion calcDF

#pragma region calculateApexAndValleyPositions
        bool
        qPeaks::calculateApexAndValleyPositions(
            const __m128 &coeff,
            const int scale,
            float &apex_position,
            float &valley_position) const
        {
            // check if beta 2 or beta 3 is zero
            if (((float *)&coeff)[2] == 0.0f || ((float *)&coeff)[3] == 0.0f)
            {
                return false; // invalid case
            }
            // calculate key by checking the signs of coeff
            __m128 res = _mm_set1_ps(-.5f);               // res = -0.5
            __m128 signs = _mm_cmplt_ps(coeff, ZERO_128); // compare the coefficients with zero, results will have the following values: 0xFFFFFFFF if the value is negative, 0x00000000 if the value is positive
            signs = _mm_and_ps(signs, KEY_128);           // multiply a key value if the value of the coefficient is negative, i.e., b0 * 0, b1 * 4, b2 * 2, b3 * 1
            signs = _mm_hadd_ps(signs, signs);            // horizontal add of the signs
            signs = _mm_hadd_ps(signs, signs);            // horizontal add of the signs, now all values are the same, i.e. the sum
            int key = _mm_cvtss_si32(signs);

            switch (key)
            {
            case 7:                                                              // Case 1a: apex left
                res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101)); // res = -0.5 * b1
                res = _mm_div_ps(res, coeff);                                    // res = -0.5 * b1 / b2
                apex_position = ((float *)&res)[2];                              //-B1 / 2 / B2;  // is negative
                valley_position = 0;                                             // no valley point
                return apex_position > -scale + 1;                               // scale +1: prevent apex position to be at the edge of the data

            case 3:                                                              // Case 1b: apex right
                res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101)); // res = -0.5 * b1
                res = _mm_div_ps(res, coeff);                                    // res = -0.5 * b1 / b2
                apex_position = ((float *)&res)[3];                              //-B1 / 2 / B3;     // is positive
                valley_position = 0;                                             // no valley point
                return apex_position < scale - 1;                                // scale -1: prevent apex position to be at the edge of the data

            case 6:                                                                       // Case 2a: apex left | valley right
                res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101));          // res = -0.5 * b1
                res = _mm_div_ps(res, coeff);                                             // res = -0.5 * b1 / b2
                apex_position = ((float *)&res)[2];                                       //-B1 / 2 / B2;                                             // is negative
                valley_position = ((float *)&res)[3];                                     //-B1 / 2 / B3;                                           // is positive
                return apex_position > -scale + 1 && valley_position - apex_position > 2; // scale +1: prevent apex position to be at the edge of the data

            case 1:                                                                      // Case 2b: apex right | valley left
                res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101));         // res = -0.5 * b1
                res = _mm_div_ps(res, coeff);                                            // res = -0.5 * b1 / b2
                apex_position = ((float *)&res)[3];                                      //-B1 / 2 / B3;                                            // is positive
                valley_position = ((float *)&res)[2];                                    //-B1 / 2 / B2;                                          // is negative
                return apex_position < scale - 1 && apex_position - valley_position > 2; // scale -1: prevent apex position to be at the edge of the data

            default:
                return false; // invalid case
            } // end switch
        }
#pragma endregion calculateApexAndValleyPositions

#pragma region "multiplyVecMatrixVecTranspose"
        float
        qPeaks::multiplyVecMatrixVecTranspose(
            const float vec[4],
            const int scale) const
        {
            // Prefetch the inverse matrix to improve cache performance
            _mm_prefetch(reinterpret_cast<const char *>(&invArray[scale][0]), _MM_HINT_T0);

            // Load the vector values
            __m128 vec_values = _mm_loadu_ps(vec);

            // Calculate the products of the vector values
            __m128 v0_v0 = _mm_mul_ps(vec_values, vec_values);                              // v0_v0 = vec[0] * vec[0]
            __m128 temp1 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(1, 0, 0, 0)); // temp1 = vec[1], vec[0], vec[0], vec[0]
            __m128 temp2 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(2, 2, 1, 1)); // temp2 = vec[2], vec[2], vec[1], vec[1]
            __m128 v0_v1 = _mm_mul_ps(temp1, temp2);                                        // v0_v1 = vec[0] * vec[1], vec[1] * vec[2], etc.

            __m128 temp3 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(3, 3, 3, 3)); // temp3 = vec[3], vec[3], vec[3], vec[3]
            __m128 v0_v2 = _mm_mul_ps(vec_values, temp3);                                   // v0_v2 = vec[0] * vec[2], vec[1] * vec[3], etc.

            __m128 temp4 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(1, 1, 1, 1)); // temp4 = vec[1], vec[1], vec[1], vec[1]
            __m128 v1_v1 = _mm_mul_ps(temp4, temp4);                                        // v1_v1 = vec[1] * vec[1]

            __m128 temp5 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(2, 2, 2, 2)); // temp5 = vec[2], vec[2], vec[2], vec[2]
            __m128 v1_v2 = _mm_mul_ps(temp5, temp4);                                        // v1_v2 = vec[2] * vec[1]

            __m128 temp6 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(3, 3, 3, 3)); // temp6 = vec[3], vec[3], vec[3], vec[3]
            __m128 v1_v3 = _mm_mul_ps(temp6, temp4);                                        // v1_v3 = vec[3] * vec[1]

            __m128 v2_v2 = _mm_mul_ps(temp5, temp5); // v2_v2 = vec[2] * vec[2]
            __m128 v2_v3 = _mm_mul_ps(temp5, temp6); // v2_v3 = vec[2] * vec[3]
            __m128 v3_v3 = _mm_mul_ps(temp6, temp6); // v3_v3 = vec[3] * vec[3]

            // Calculate the result
            float result = _mm_cvtss_f32(v0_v0) * invArray[scale][0] +
                           _mm_cvtss_f32(v1_v1) * invArray[scale][2] +
                           invArray[scale][4] * (_mm_cvtss_f32(v2_v2) + _mm_cvtss_f32(v3_v3)) +
                           2 * (invArray[scale][1] * (_mm_cvtss_f32(v0_v1) + _mm_cvtss_f32(v0_v2)) +
                                invArray[scale][3] * (_mm_cvtss_f32(v1_v2) - _mm_cvtss_f32(v1_v3)) +
                                _mm_cvtss_f32(v2_v3) * invArray[scale][5]);

            return result;
        }
#pragma endregion "multiplyVecMatrixVecTranspose"

#pragma region "isValidApexToEdge"
        bool
        qPeaks::isValidApexToEdge(
            const double apex_position,
            const int scale,
            const int index_loop,
            const float *y_start,
            float &apexToEdge) const
        {
            int idx_apex = (int)std::round(apex_position) + scale + index_loop; // index of the apex
            int idx_left = index_loop;                                          // index of the left edge
            int idx_right = 2 * scale + index_loop;                             // index of the right edge
            float apex = *(y_start + idx_apex);                                 // apex value
            float left = *(y_start + idx_left);                                 // left edge value
            float right = *(y_start + idx_right);                               // right edge value
            apexToEdge = (left < right) ? (apex / left) : (apex / right);       // difference between the apex and the edge
            return apexToEdge > 2;                                              // return true if the ratio is greater than 2
        }

#pragma endregion "isValidApexToEdge"

#pragma region isValidQuadraticTerm
        bool qPeaks::isValidQuadraticTerm(
            const __m128 &coeff,
            const float inverseMatrix_2_2,
            const float mse,
            const int df_sum) const
        {
            double tValue = std::max(                                                 // t-value for the quadratic term
                std::abs(((float *)&coeff)[2]) / std::sqrt(inverseMatrix_2_2 * mse),  // t-value for the quadratic term left side of the peak
                std::abs(((float *)&coeff)[3]) / std::sqrt(inverseMatrix_2_2 * mse)); // t-value for the quadratic term right side of the peak
            return tValue > tValuesArray[df_sum - 5];                                 // statistical significance of the quadratic term
        }
#pragma endregion isValidQuadraticTerm

#pragma region isValidPeakHeight
        bool
        qPeaks::isValidPeakHeight(
            const float mse,
            const int index,
            const int scale,
            const float apex_position,
            const float valley_position,
            const int df_sum,
            const float apexToEdge,
            float &uncertainty_height) const
        {
            float Jacobian_height[4];           // Jacobian matrix for the height
            Jacobian_height[0] = 1.f;           // height;
            Jacobian_height[1] = apex_position; // apex_position * height;
            if (apex_position < 0)
            {
                Jacobian_height[2] = apex_position * apex_position; // apex_position * Jacobian_height[1];
                Jacobian_height[3] = 0;
            }
            else
            {
                Jacobian_height[2] = 0;
                Jacobian_height[3] = apex_position * apex_position; // apex_position * Jacobian_height[1];
            }
            uncertainty_height = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale)); // at this point without height, i.e., to get the real uncertainty multiply with height later. This is done to avoid exp function at this point

            if (1 / uncertainty_height <= tValuesArray[df_sum - 5]) // statistical significance of the peak height
            {
                return false;
            }

            // check if the peak height is significantly greater than edge signal

            float edge_position = (apex_position < 0) // position of the egde not on the same side like the peak apex
                                      ?               // apex on the left
                                      (valley_position != 0) ? valley_position : static_cast<float>(-scale)
                                      : // apex on the right
                                      (valley_position != 0) ? valley_position : static_cast<float>(scale);

            Jacobian_height[0] = 0.f;            // adjust for uncertainty calculation of apex to edge ratio
            Jacobian_height[1] -= edge_position; // adjust for uncertainty calculation of apex to edge ratio
            if (apex_position < 0)
            {
                Jacobian_height[2] -= edge_position * edge_position; // adjust for uncertainty calculation of apex to edge ratio
            }
            else
            {
                Jacobian_height[3] -= edge_position * edge_position; // adjust for uncertainty calculation of apex to edge ratio
            }
            float uncertainty_apexToEdge = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale));
            return (apexToEdge - 2) / (apexToEdge * uncertainty_apexToEdge) > tValuesArray[df_sum - 5];
        }
#pragma endregion isValidPeakHeight

#pragma region isValidPeakArea
        bool
        qPeaks::isValidPeakArea(
            const __m128 &coeff,
            const float mse,
            const int scale,
            const int df_sum,
            float &area,
            float &uncertainty_area) const
        {
            // predefine expressions
            const float b1 = ((float *)&coeff)[1];
            const float b2 = ((float *)&coeff)[2];
            const float b3 = ((float *)&coeff)[3];
            const float _SQRTB2 = 1 / std::sqrt(std::abs(-b2));
            const float _SQRTB3 = 1 / std::sqrt(std::abs(-b3));
            const float B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
            const float B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
            const float B1_2_B2 = b1 / 2 / b2;
            const float EXP_B12 = exp_approx_d(-b1 * B1_2_B2 / 2);
            const float B1_2_B3 = b1 / 2 / b3;
            const float EXP_B13 = exp_approx_d(-b1 * B1_2_B3 / 2);

            // initialize variables
            float J[4]; // Jacobian matrix

            // here we have to check if there is a valley point or not
            const float err_L =
                (b2 < 0)
                    ? experfc(B1_2_SQRTB2, -1.0) // 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
                    : dawson5(B1_2_SQRTB2);      // erfi(b1 / 2 / SQRTB2);        // peak with valley point;

            const float err_R =
                (b3 < 0)
                    ? experfc(B1_2_SQRTB3, 1.0) // 1 + std::erf(b1 / 2 / SQRTB3) // ordinary peak
                    : dawson5(-B1_2_SQRTB3);    // -erfi(b1 / 2 / SQRTB3);       // peak with valley point ;

            // calculate the Jacobian matrix terms
            const float J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
            const float J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
            const float J_2_common_L = B1_2_B2 / b1;
            const float J_2_common_R = B1_2_B3 / b1;
            const float J_1_L = J_1_common_L * err_L;
            const float J_1_R = J_1_common_R * err_R;
            const float J_2_L = J_2_common_L - J_1_L * B1_2_B2;
            const float J_2_R = -J_2_common_R - J_1_R * B1_2_B3;

            J[0] = J_1_R + J_1_L;
            J[1] = J_2_R + J_2_L;
            J[2] = -B1_2_B2 * (J_2_L + J_1_L / b1);
            J[3] = -B1_2_B3 * (J_2_R + J_1_R / b1);

            area = J[0]; // at this point the area is without exp(b0), i.e., to get the real area multiply with exp(b0) later. This is done to avoid exp function at this point
            uncertainty_area = std::sqrt(mse * multiplyVecMatrixVecTranspose(J, scale));

            if (area / uncertainty_area <= tValuesArray[df_sum - 5])
            {
                return false;
            }

            float J_covered[4];    // Jacobian matrix for the covered peak area
            float x_left = -scale; // left limit due to the window
            float x_right = scale; // right limit due to the window
            float y_left = 0;      // y value at the left limit
            float y_right = 0;     // y value at the right limit

            const float err_L_covered = ///@todo : need to be revised
                (b2 < 0)
                    ?                                                                                                   // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
                    erf_approx_f((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12 * SQRTPI_2 + err_L - SQRTPI_2 * EXP_B12 // std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
                    :                                                                                                   // valley point, i.e., check position
                    (-B1_2_B2 < -scale)
                        ? // valley point is outside the window, use scale as limit
                        err_L - erfi((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12
                        : // valley point is inside the window, use valley point as limit
                        err_L;

            const float err_R_covered = ///@todo : need to be revised
                (b3 < 0)
                    ?                                                                                                   // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
                    err_R - SQRTPI_2 * EXP_B13 - erf_approx_f((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * SQRTPI_2 * EXP_B13 // err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
                    :                                                                                                   // valley point, i.e., check position
                    (-B1_2_B3 > scale)
                        ? // valley point is outside the window, use scale as limit
                        erfi((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * EXP_B13 + err_R
                        : // valley point is inside the window, use valley point as limit
                        err_R;

            // adjust x limits
            if (b2 > 0 && -B1_2_B2 > -scale)
            { // valley point is inside the window, use valley point as limit
                x_left = -B1_2_B2;
            }

            if (b3 > 0 && -B1_2_B3 < scale)
            { // valley point is inside the window, use valley point as limit
                x_right = -B1_2_B3;
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

            const float area_uncertainty_covered = std::sqrt(mse * multiplyVecMatrixVecTranspose(J_covered, scale));

            return J_covered[0] / area_uncertainty_covered > tValuesArray[df_sum - 5]; // statistical significance of the peak area
        }
#pragma endregion isValidPeakArea

#pragma region "calcUncertaintyPosition"
        void
        qPeaks::calcUncertaintyPosition(
            const float mse,
            const __m128 &coeff,
            const float apex_position,
            const int scale,
            float &uncertainty_pos) const
        {
            const float _b1 = 1 / ((float *)&coeff)[1];
            const float _b2 = 1 / ((float *)&coeff)[2];
            const float _b3 = 1 / ((float *)&coeff)[3];
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
            uncertainty_pos = std::sqrt(mse * multiplyVecMatrixVecTranspose(J, scale));
        }
#pragma endregion "calcUncertaintyPosition"

#pragma region calculateNumberOfRegressions
        int qPeaks::calculateNumberOfRegressions(const int n) const
        {
            /*
            int maxScale = (int) (n - 1) / 2;
            int sumScales = (int) (maxScale * (maxScale + 1) / 2) - 1;
            return n * (maxScale-1) - sumScales*2;
            */
            int sum = 0;
            for (int i = 4; i <= global_maxScale * 2; i += 2)
            {
                sum += std::max(0, n - i);
            }
            return sum;
        }
#pragma endregion calculateNumberOfRegressions

#pragma region "convolve regression"

        void
        qPeaks::convolve_static(
            const size_t scale,
            const float *vec,
            const size_t n,
            __m128 *beta)
        {
            if (n < 2 * scale + 1)
            {
                throw std::invalid_argument("n must be greater or equal to 2 * scale + 1");
            }

            alignas(__m128) __m128 result[512];
            alignas(__m128) __m128 products[512];
            const __m128 flipSign = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
            convolve_SIMD(scale, vec, n, result, products, 512);

            for (size_t i = 0; i < n - 2 * scale; i++)
            {
                beta[i] = _mm_mul_ps(_mm_shuffle_ps(result[i], result[i], 0b10110100), flipSign); // swap beta2 and beta3 and flip the sign of beta1 // @todo: this is a temporary solution
            }
        }

        void
        qPeaks::convolve_dynamic(
            const size_t scale,
            const float *vec,
            const size_t n,
            __m128 *beta)
        {
            if (n < 2 * scale + 1)
            {
                throw std::invalid_argument("n must be greater or equal to 2 * scale + 1");
            }

            alignas(__m128) __m128 *result = new __m128[n - 2 * scale];
            alignas(__m128) __m128 *products = new __m128[n];
            const __m128 flipSign = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
            convolve_SIMD(scale, vec, n, result, products, n);

            for (size_t i = 0; i < n - 2 * scale; i++)
            {
                beta[i] = _mm_mul_ps(_mm_shuffle_ps(result[i], result[i], 0b10110100), flipSign); // swap beta2 and beta3 and flip the sign of beta1 // @todo: this is a temporary solution
            }
            delete[] result;
            delete[] products;
        }

        void
        qPeaks::convolve_SIMD(
            const size_t scale,
            const float *vec,
            const size_t n,
            __m128 *result,
            __m128 *products,
            const size_t buffer_size)
        {
            size_t k = 2 * scale + 1;
            size_t n_segments = n - k + 1;
            size_t centerpoint = k / 2;

            // removed: all elements of result are modified
            // for (size_t i = 0; i < n_segments; ++i)
            // {
            //     result[i] = _mm_setzero_ps();
            // }

            for (size_t i = 0; i < n; ++i)
            {
                products[i] = _mm_setzero_ps();
            }

            alignas(__m128) __m128 kernel[3];
            kernel[0] = _mm_set_ps(invArray[scale][1], invArray[scale][1], 0.0f, invArray[scale][0]);
            kernel[1] = _mm_set_ps(invArray[scale][3] - invArray[scale][5], -invArray[scale][3] - invArray[scale][4], -invArray[scale][2] - invArray[scale][3], -invArray[scale][1]);
            kernel[2] = _mm_set_ps(2.f * invArray[scale][5], 2.f * invArray[scale][4], 2.f * invArray[scale][3], 2.f * invArray[scale][1]);

#pragma GCC ivdep
#pragma GCC unroll 8
            for (size_t i = 0; i < n_segments; i++)
            {
                __m128 vec_values = _mm_set1_ps(vec[i + centerpoint]);
                // result[i] = _mm_fmadd_ps(vec_values, kernel[0], result[i]);
                result[i] = _mm_mul_ps(vec_values, kernel[0]);
            }

            for (size_t i = 1; i < scale + 1; i++)
            {
                int u = 0;
                kernel[1] = _mm_add_ps(kernel[1], kernel[2]);
                // kernel[1] = kernel[1] original + i * kernel[2]
                kernel[0] = _mm_add_ps(kernel[0], kernel[1]);

#pragma GCC ivdep
#pragma GCC unroll 8
                for (size_t j = scale - i; j < (n - scale + i); j++)
                {
                    __m128 vec_values = _mm_set1_ps(vec[j]);
                    products[u] = _mm_mul_ps(vec_values, kernel[0]);
                    u++;
                }

#pragma GCC ivdep
#pragma GCC unroll 8
                for (size_t j = 0; j < n_segments; j++)
                {
                    if (2 * i + j >= buffer_size)
                    {
                        throw std::out_of_range("Index out of range for products array: n=" + std::to_string(n) + " i=" + std::to_string(i) + " j=" + std::to_string(j));
                    }
                    __m128 products_temp = _mm_permute_ps(products[j], 0b10110100);
                    __m128 sign_flip = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
                    products_temp = _mm_fmadd_ps(products_temp, sign_flip, products[2 * i + j]);
                    result[j] = _mm_add_ps(result[j], products_temp);
                }
            }
        }
#pragma endregion "convolve regression"
    } // namespace Algorithms
} // namespace q