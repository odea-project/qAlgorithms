// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_datatype_peak.h"
#include "qalgorithms_measurement_data.h"
#include "qalgorithms_qbin.h"

// external
#include <vector>

#include <immintrin.h> // AVX

/* This file includes the q::qPeaks class*/

namespace qAlgorithms
{
    const std::array<float, 384> initialize();

    std::vector<CentroidPeak> findCentroids(
        treatedData &treatedData,
        const int scanNumber,
        const float retentionTime);

    void findPeaks(
        std::vector<FeaturePeak> &all_peaks,
        treatedData &treatedData);

    CentroidedData passToBinning(std::vector<std::vector<CentroidPeak>> &allPeaks,
                                 std::vector<unsigned int> addEmpty);

    /**
     * @brief Array of the unique entries from the inverse matrix: ( X.T * X ) ^-1
     * @details the matrix has the following structure:
     * A  0  B  B
     * 0  C  D -D
     * B  D  E  F
     * B -D  F  E
     * The structure of the array is as follows:
     * invArray[scale][{A,B,C,D,E,F}]
     */
    // alignas(16) static float invArray[64][6]; // contains the unique entries from the inverse matrix

    // methods
    int calcNumberOfRegressions(const int n);

    void runningRegression(
        const float *y_start,
        const float *ylog_start,
        const bool *df_start,
        const int n,
        std::vector<ValidRegression_static> &validRegressions);

    void runningRegression_static(
        const float *y_start,
        const float *ylog_start,
        const bool *df_start,
        const int n,
        ValidRegression_static *validRegressions,
        int &validRegressionsIndex);

    void validateRegressions(
        const __m128 *beta,
        const int n_segments,
        const float *y_start,
        const float *ylog_start,
        const bool *df_start,
        const int scale,
        std::vector<ValidRegression_static> &validRegressions);

    void validateRegressions_static(
        const __m128 *beta,
        const int n_segments,
        const float *y_start,
        const float *ylog_start,
        const bool *df_start,
        const int scale,
        int &validRegressionsIndex,
        ValidRegression_static *validRegressions);

    ValidRegression_static makeValidRegression(
        const int i,
        const int scale,
        const bool *df_start,
        const float *y_start,
        const float *ylog_start,
        const __m128 coeff);

    void mergeRegressionsOverScales(
        std::vector<ValidRegression_static> &validRegressions,
        const float *y_start,
        const bool *df_start);

    void mergeRegressionsOverScales_static(
        ValidRegression_static *validRegressions,
        const int n_regressions,
        const float *y_start,
        // const float *ylog_start,
        const bool *df_start);

    void createCentroidPeaks(
        std::vector<CentroidPeak> &peaks,
        ValidRegression_static *validRegressions,
        std::vector<ValidRegression_static> *validRegressionsVec,
        const int validRegressionsIndex,
        const float *y_start,
        const float *mz_start,
        const bool *df_start,
        const int scanNumber);

    void createFeaturePeaks(
        std::vector<FeaturePeak> &peaks,
        ValidRegression_static *validRegressions,
        std::vector<ValidRegression_static> *validRegressionsVec,
        const int validRegressionsIndex,
        const float *y_start,
        const float *mz_start,
        const float *rt_start,
        const bool *df_start,
        const float *dqs_cen,
        const float *dqs_bin);

    float calcSSE(
        const int left_limit,
        const int right_limit,
        RegCoeffs coeff,
        const float *y_start = nullptr,
        const bool calc_EXP = false,
        const bool calc_CHISQ = false);

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
    void calcExtendedMse(
        const float *y_start,
        std::vector<ValidRegression_static> &regressions,
        const bool *df_start);

    void calcExtendedMse_static(
        const float *y_start,
        ValidRegression_static *regressions_start,
        const int n_regressions,
        const bool *df_start);

    void calcExtendedMsePair(
        const float *y_start,
        ValidRegression_static *low_scale_regression,
        ValidRegression_static *hi_scale_regression,
        const bool *df_start);

    void calcExtendedMseOverScales(
        const float *y_start,
        ValidRegression_static *validRegressions,
        const std::vector<int> &validRegressionsInGroup,
        const int i_new_peak);

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
    int calcDF(
        const bool *df_start,
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
        const __m128 coeff,
        const int scale,
        float &apex_position,
        float &valley_position);

    /**
     * @brief Calculate the Matrix Product of J * Xinv * J^T for uncertainty calculation.
     * @details The function calculates the matrix product of J * Xinv * J^T. The matrix J is
     * the Jacobian matrix with an 1x4 size. The matrix Xinv is the inverse matrix of X^T * X,
     * where X is the design matrix. The matrix J * Xinv * J^T is a 1x1 matrix, i.e., a scalar value.
     *
     * @param vec
     * @param scale
     * @return float
     */
    float multiplyVecMatrixVecTranspose(
        const float vec[4],
        int scale);

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
     * @param index_loop
     * @param Y
     * @param apexToEdge : The ratio of the peak maximum to the signal at the edge of
     * the regression window. This value is calculated by the function.
     * @return true
     * @return false
     */
    float calcApexToEdge(
        const double apex_position,
        const int scale,
        const int index_loop,
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
        RegCoeffs coeff,
        const int scale,
        const float mse,
        const int df_sum);

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
        const int scale,
        const float apex_position,
        float valley_position,
        const int df_sum,
        const float apexToEdge);

    float calcPeakHeightUncert(
        const float mse,
        const int scale,
        const float apex_position);

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
    bool isValidPeakArea(
        RegCoeffs coeff,
        const float mse,
        const int scale,
        const int df_sum,
        float &area,
        float &uncertainty_area);

    float calcUncertaintyPos(
        const float mse,
        RegCoeffs coeff,
        const float apex_position,
        const int scale);

    __m128 convolve_static(
        const size_t scale,
        const float *vec,
        const size_t n);

    void convolve_dynamic(
        const size_t scale,
        const float *vec,
        const size_t n,
        __m128 *beta);

    void convolve_SIMD(
        const size_t scale,
        const float *vec,
        const size_t n,
        __m128 *result,
        __m128 *products,
        const size_t buffer_size);

    std::pair<float, float> weightedMeanAndVariance(const float *x, const float *w, const bool *df,
                                                    int left_limit, int right_limit);
}

#endif // QALGORITHMS_QPEAKS_H