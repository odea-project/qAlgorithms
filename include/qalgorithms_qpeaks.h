// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_datatypes.h"
#include "qalgorithms_measurement_data.h"

// external
#include <vector>
#include <array>
#include <immintrin.h> // AVX

namespace qAlgorithms
{
    // @todo rework this module so all centroiding / feature construction parts are in a separate block and qPeaks can be used as a library header

    std::vector<RegCoeffs> findCoefficients(
        const std::vector<float> *intensity_log,
        const size_t scale); // maximum scale that will be checked. Should generally be limited by peakFrame

    void findCentroidPeaks(std::vector<CentroidPeak> *retPeaks, // results are appended to this vector
                           const std::vector<ProfileBlock> *treatedData,
                           const size_t scanNumber,
                           const size_t accessor);

    void findFeatures(std::vector<FeaturePeak> &all_peaks,
                      treatedData &treatedData);

    // const std::vector<qCentroid> passToBinning(const std::vector<CentroidPeak> *allPeaks);

    void runningRegression(
        const std::vector<float> *intensities,
        const std::vector<float> *ylog_start,
        const std::vector<bool> *degreesOfFreedom,
        std::vector<RegressionGauss> &validRegressions,
        const size_t maxScale);

    void validateRegression(
        const std::vector<RegCoeffs> *coeffs, // coefficients for single-b0 peaks, spans all regressions over a peak window
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        const std::vector<bool> *degreesOfFreedom,
        const size_t maxScale, // scale, i.e., the number of data points in a half window excluding the center point
        std::vector<RegressionGauss> &validRegressions);

    void makeValidRegression(
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const std::vector<bool> *degreesOfFreedom,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log);

    std::vector<RegressionGauss> mergeRegressionsOverScales(
        std::vector<RegressionGauss> validRegressions,
        const std::vector<float> *intensities);

    void createCentroidPeaks(
        std::vector<CentroidPeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const ProfileBlock *block,
        const size_t scanNumber,
        const size_t accessor);

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const std::vector<float> *RT,
        const float *rt_start);

    float calcSSE_base(const RegCoeffs coeff,
                       const std::vector<float> *y_start,
                       std::vector<float> *selectLog,
                       std::vector<float> *predictLog,
                       size_t limit_L,
                       size_t limit_R,
                       size_t index_x0);

    float calcRegressionFvalue(const std::vector<float> *selectLog,
                               const std::vector<float> *intensities,
                               const float mse,
                               const float b0);

    float calcSSE_exp(const RegCoeffs coeff,
                      const std::vector<float> *y_start,
                      size_t limit_L, size_t limit_R,
                      size_t index_x0);

    float calcSSE_chisqared(const RegCoeffs coeff,
                            const std::vector<float> *y_start,
                            size_t limit_L,
                            size_t limit_R,
                            size_t index_x0);

    std::pair<float, float> smearingCorrection(
        const std::vector<float> *predictLog,
        const std::vector<float> *selectLog,
        const size_t scale);

    /**
     * @brief Calculate the best mean squared error of the regression model
     * with different regression windows BUT same window size.
     * @details The function extends the regression windows that all the windows
     * cover the range from the lowest x value to the highest x value. I.e., if
     * window A is [4,5,6,7,8,9,10] and window B is [6,7,8,9,10,11,12], the extended
     * window is [4,5,6,7,8,9,10,11,12]. The function calculates the mean squared
     * error of the regression model with the extended window and returns the
     * mean squared error and the index of the best regression.
     *
     * @param Y : Measurement data (not log transformed)
     * @param B : Matrix of regression coefficients
     * @param groupIndices : Indices of the regression windows
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @return MSE and index of the best regression window
     */

    struct RegPair
    {
        unsigned int idx;
        float mse;
    };

    RegPair findBestRegression(
        const std::vector<float> *intensities,
        const std::vector<RegressionGauss> *regressions,
        const std::vector<bool> *degreesOfFreedom,
        const size_t startIdx,
        const size_t endIdx);

    /**
     * @brief Calculate the degree of freedom of the regression model with the given regression window.
     * @details The degree of freedom is the number of data points minus the
     * number of regression coefficients. Moreover, the degree of freedom is
     * reduced by the number of interpolated data points. For calculating
     * the degree of freedom, the function uses the df vector that contains a
     * value of 1 if the data point is not interpolated and 0 if the data point is interpolated.
     *
     * @param df : true = real point, false = interpolated
     * @param left_limit : Start index of the regression window
     * @param right_limit : End index of the regression window
     * @return size_t : Degree of freedom
     */
    size_t calcDF(
        const std::vector<bool> *degreesOfFreedom,
        unsigned int left_limit,
        unsigned int right_limit);

    /**
     * @brief Calculate the apex (and if possible the valley) position of the peak. Returns true
     * if the positions are calculated are valid.
     * @param mutateReg : regression whose position fields are manipulated in the function
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @param valley_position : valley position
     * @return true : if the apex and valley positions are valid
     * @return false : if the apex and valley positions are not valid (e.g., the apex position is not in the regression window)
     */
    bool calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        const size_t scale,
        float &valley_position);

    /**
     * @brief Calculate the Matrix Product of J * Xinv * J^T for uncertainty calculation.
     * @details The function calculates the matrix product of J * Xinv * J^T. The matrix J is
     * the Jacobian matrix with an 1x4 size. The matrix Xinv is the inverse matrix of X^T * X,
     * where X is the design matrix. The matrix J * Xinv * J^T is a 1x1 matrix, i.e., a scalar value.
     * @param vec
     * @param scale
     * @return float
     */
    inline double multiplyVecMatrixVecTranspose(const double vec[4],
                                                size_t scale);

    float apexToEdgeRatio(
        const size_t idxStart,
        const size_t idxApex,
        const size_t idxEnd,
        const std::vector<float> *intensities);

    bool isValidQuadraticTerm(
        const RegCoeffs coeff,
        const size_t scale,
        const float mse,
        const size_t df_sum);

    bool isValidPeakHeight(
        const float mse,
        const size_t scale,
        const float apex_position,
        float valley_position,
        const size_t df_sum,
        const float apexToEdge);

    void calcPeakHeightUncert(
        RegressionGauss *mutateReg,
        const float mse,
        const size_t scale);

    /**
     * @brief Check if the peak area and the covered peak area are valid using t-test.
     * @details The function calculates the peak area and the covered peak area using the
     * regression coefficients. The peak area is the integral of the regression model
     * from -infinity to +infinity. The covered peak area is the integral of the regression
     * model from the left limit of the regression window to the right limit of the
     * regression window. Moreover, the trapzoid under the peak is considered as
     * not covered peak area.
     *
     * @param coeff : Matrix of regression coefficients
     * @param mse : mean squared error
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @param df_sum : sum of the degree of freedom of the regression model
     * @param area : area of the peak
     * @param uncertainty_area : uncertainty of the area
     * @return true : if the peak area is valid
     * @return false : if the peak area is not valid
     */

    void calcPeakAreaUncert(RegressionGauss *mutateReg,
                            const float mse,
                            const size_t scale);

    bool isValidPeakArea(
        const RegCoeffs coeff,
        const float mse,
        const size_t scale,
        const size_t df_sum);

    float calcUncertaintyPos(
        const float mse,
        const RegCoeffs coeff,
        const float apex_position,
        const size_t scale);

    struct MeanVar
    {
        float mean;
        float var;
    };

    MeanVar weightedMeanAndVariance(const float *x,
                                    const std::vector<float> *weight,
                                    size_t left_limit,
                                    size_t right_limit);

    MeanVar weightedMeanAndVariance_EIC(const std::vector<float> *weight,
                                        const std::vector<float> *values,
                                        size_t left_limit,
                                        size_t right_limit);

#define MAXSCALE 63

    constexpr std::array<float, (MAXSCALE + 1) * 6> initialize()
    { // array to store the 6 unique values of the inverse matrix for each scale
        std::array<float, (MAXSCALE + 1) * 6> invArray;
        // init invArray
        // XtX = transposed(Matrix X ) * Matrix X
        // XtX_xy: x = row number; y = column number
        float XtX_00 = 1.f;
        float XtX_02 = 0.f;
        float XtX_11 = 0.f;
        float XtX_12 = 0.f;
        float XtX_13 = 0.f;
        float XtX_22 = 0.f;
        for (int i = 1; i < MAXSCALE; ++i)
        {
            XtX_00 += 2.f;
            XtX_02 += i * i;
            XtX_11 = XtX_02 * 2.f;
            XtX_13 += i * i * i;
            XtX_12 = -XtX_13;
            XtX_22 += i * i * i * i;

            // decomposition matrix L, see https://en.wikipedia.org/wiki/Cholesky_decomposition
            float L_00 = std::sqrt(XtX_00);
            float L_11 = std::sqrt(XtX_11);
            float L_20 = XtX_02 / L_00;
            float L_21 = XtX_12 / L_11;
            float L_20sq = L_20 * L_20;
            float L_21sq = L_21 * L_21;
            float L_22 = std::sqrt(XtX_22 - L_20sq - L_21sq);
            float L_32 = 1 / L_22 * (-L_20sq + L_21sq);
            float L_33 = std::sqrt(XtX_22 - L_20sq - L_21sq - L_32 * L_32);

            // inverse of L
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
}

#endif