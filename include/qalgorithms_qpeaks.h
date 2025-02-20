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
    const std::array<float, 384> initialize();

    std::vector<CentroidPeak> findCentroids(const std::vector<ProfileBlock> treatedData, const size_t scanNumber);

    void findFeatures(std::vector<FeaturePeak> &all_peaks, treatedData &treatedData);

    std::vector<qCentroid> passToBinning(std::vector<std::vector<CentroidPeak>> &allPeaks);

    void runningRegression(
        const float *y_start,
        const float *ylog_start,
        const std::vector<bool> degreesOfFreedom,
        const size_t arrayMaxLength,
        const size_t n,
        std::vector<RegressionGauss> &validRegressions,
        const size_t maxScale);

    void validateRegressions(
        const __m128 *beta,
        size_t n_segments,
        const float *y_start,
        const float *ylog_start,
        const std::vector<bool> degreesOfFreedom,
        size_t arrayMaxLength,
        size_t scale,
        std::vector<RegressionGauss> &validRegressions);

    void makeValidRegression(
        RegressionGauss *mutateReg,
        size_t arrayMaxLength,
        const size_t idxStart,
        const size_t scale,
        const std::vector<bool> degreesOfFreedom,
        const float *intensities,
        const float *intensities_log);

    std::vector<RegressionGauss> mergeRegressionsOverScales(
        std::vector<RegressionGauss> validRegressions,
        const float *y_start);

    void createCentroidPeaks(
        std::vector<CentroidPeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        ProfileBlock block,
        const size_t scanNumber);

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const float *y_start,
        const float *mz_start,
        const float *rt_start,
        const float *DQSC,
        const float *DQSB);

    float calcSSE_base(const RegCoeffs coeff, const float *y_start, size_t scale, size_t idxStart);

    float calcSSE_exp(const RegCoeffs coeff, const float *y_start, size_t limit_L, size_t limit_R, size_t index_x0);

    float calcSSE_chisqared(const RegCoeffs coeff, const float *y_start, size_t limit_L, size_t limit_R, size_t index_x0);

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
     * @return std::pair<double,int> : MSE and index of the best regression window
     */

    std::pair<size_t, float> findBestRegression(
        const float *y_start,
        std::vector<RegressionGauss> regressions,
        const std::vector<bool> degreesOfFreedom,
        size_t startIdx,
        size_t endIdx);

    /**
     * @brief Calculate the degree of freedom of the regression model with the given regression window.
     * @details The degree of freedom is the number of data points minus the
     * number of regression coefficients. Moreover, the degree of freedom is
     * reduced by the number of interpolated data points. For calculating
     * the degree of freedom, the function uses the df vector that contains a
     * value of 1 if the data point is not interpolated and 0 if the data point is interpolated.
     *
     * @param df : q::Matrices::Vector of integers that indicates if the data point is interpolated or not
     * @param left_limit : Start index of the regression window
     * @param right_limit : End index of the regression window
     * @return int : Degree of freedom
     */
    size_t calcDF(
        const bool *degreesOfFreedom,
        unsigned int left_limit,
        unsigned int right_limit);

    size_t calcDF(
        const std::vector<bool> degreesOfFreedom,
        unsigned int left_limit,
        unsigned int right_limit);

    /**
     * @brief Calculate the apex (and if possible the valley) position of the peak. Returns true
     * if the positions are calculated are valid.
     * @param coeff : Matrix of regression coefficients
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @param apex_position : apex position
     * @param valley_position : valley position
     * @return true : if the apex and valley positions are valid
     * @return false : if the apex and valley positions are not valid (e.g., the apex position is not in the regression window)
     */
    bool calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        const int scale, // this scale needs to be an int since it needs to be negative in the function body
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
    inline float multiplyVecMatrixVecTranspose(const float vec[4], size_t scale);

    /**
     * @brief Checks if peak maximum is twice as high as the signal at the edge of the regression window.
     * @details The test is used as a fast pre-test for signal-to-noise ratio which will be
     * calculated later. However, s/n siginificance is time consuming due to MSE calculation.
     * The reference value of two is used due to t = (apex/edge - 2) / (apex/edge * sqrt()).
     * If apex is equal or less than two times the edge, the t value is less than zero,
     * which means that SNR cannot be significant.
     *
     * @param apex_position
     * @param scale
     * @param idxStart
     * @param Y
     * @param apexToEdge : The ratio of the peak maximum to the signal at the edge of
     * the regression window. This value is calculated by the function.
     * @return true
     * @return false
     */
    inline float apexToEdgeRatio(
        const size_t idxStart,
        const size_t idxApex,
        const size_t idxEnd,
        const float *y_start);

    /**
     * @brief Check if the quadratic term of the regression model is valid using t-test.
     * @param coeff : Matrix of regression coefficients
     * @param inverseMatrix_2_2 : quadratic term (left side) (diagonal element of the inverse matrix)
     * @param mse : mean squared error
     * @param df_sum : sum of the degree of freedom of the regression model
     * @return true : if the quadratic term is valid
     * @return false : if the quadratic term is not valid
     */
    bool isValidQuadraticTerm(
        const RegCoeffs coeff,
        const size_t scale,
        const float mse,
        const size_t df_sum);

    /**
     * @brief Check if the peak height is valid using t-test.
     * @param B : Matrix of regression coefficients
     * @param C : Variance-covariance matrix of the regression coefficients
     * @param index : Index of the regression window
     * @param apex_position : apex position
     * @param df_sum : sum of the degree of freedom of the regression model
     * @return true : if the peak height is valid
     * @return false : if the peak height is not valid
     */
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

    void calcPeakAreaUncert(RegressionGauss *mutateReg, const float mse, const int scale);

    bool isValidPeakArea(
        RegCoeffs coeff,
        const float mse,
        const int scale, // scale must be an int here for this function to work @todo sensible to change?
        const size_t df_sum);

    float calcUncertaintyPos(
        const float mse,
        RegCoeffs coeff,
        const float apex_position,
        const size_t scale);

    void convolve_SIMD(
        const size_t scale,
        const float *vec,
        const size_t n,
        __m128 *result);

    std::pair<float, float> weightedMeanAndVariance(const float *x, const float *weight,
                                                    int left_limit, int right_limit);
}

#endif