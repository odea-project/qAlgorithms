// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h" // @todo get rid of this coupling!

#include <vector>
#include <array>
#include <math.h>

namespace qAlgorithms
{
    // @todo rework this module so all centroiding / feature construction parts are in a separate block and qPeaks can be used as a library header

    // ### General Implementation ### //

    std::vector<RegCoeffs> findCoefficients(
        const std::vector<float> *intensity_log,
        const size_t scale); // maximum scale that will be checked. Should generally be limited by peakFrame

    void runningRegression(
        const std::vector<float> *intensities,
        std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        std::vector<RegressionGauss> *validRegressions,
        const size_t maxScale,
        const size_t maxApexIdx);

    void validateRegression(
        std::vector<RegressionGauss> &validRegressions,
        const std::vector<RegCoeffs> *coeffs, // coefficients for single-b0 peaks, spans all regressions over a peak window
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const size_t maxApexIdx);

    void makeValidRegression(
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log,
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const size_t maxApexPos);

    void mergeRegressionsOverScales(std::vector<RegressionGauss> *validRegressions,
                                    const std::vector<float> *intensities);

    // ### Centroiding-specific Code ### //

    std::vector<CentroidPeak> findCentroids(
        XML_File &data, // @todo replace with custom struct for a generic parsed file
        const std::vector<unsigned int> *selectedIndices);

    size_t pretreatDataCentroids(
        std::vector<ProfileBlock> *groupedData,
        const std::vector<double> *spectrum_mz,
        const std::vector<double> *spectrum_int);

    void findCentroidPeaks(std::vector<CentroidPeak> *retPeaks, // results are appended to this vector
                           const std::vector<ProfileBlock> *treatedData,
                           const size_t scanNumber,
                           const size_t accessor,
                           const size_t maxWindowSize);

    void createCentroidPeaks(
        std::vector<CentroidPeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const ProfileBlock *block,
        const size_t scanNumber,
        const size_t accessor);

    // ### Feature-specific Code ### //

    struct Block
    {
        size_t start;
        size_t end;
    };

    void binProfileSpec(std::vector<Block> *result,
                        const std::vector<double> *diffs,
                        // const std::vector<unsigned int> *diffOrder,
                        const std::vector<double> *cumDiffs, // indices into cumDiffs must be right-shifted by one!
                        // size_t previousDiffPos,              // skip this many points in the diffOrder vector
                        size_t start, size_t end);

    std::vector<FeaturePeak> findFeatures(std::vector<EIC> &data,
                                          std::vector<unsigned int> *backConvert,
                                          std::vector<float> *RT);

    void findFeaturePeaks(std::vector<RegressionGauss> *validRegressions, const EIC *eic, const size_t maxApexIdx);

    // ### Retention Time Conversion @todo make less annoying ### //

    struct RT_Converter
    {
        // find the spectrum index after interpolation from MS1 spectrum number and vice versa
        std::vector<unsigned int> countToInterp;
        std::vector<unsigned int> interpToCount;
        // retention time at the interpolated index
        std::vector<float> interpToRT;
    };

    RT_Converter interpolateScanNumbers(const std::vector<float> *retentionTimes);

    inline float calcRTDiff(const std::vector<double> *retention_times);

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        std::vector<unsigned int> *backConvert,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const std::vector<float> *RT);

    double calcSSE_base(const RegCoeffs coeff,
                        const std::vector<float> *y_start,
                        size_t limit_L,
                        size_t limit_R,
                        size_t index_x0);

    bool f_testRegression(const std::vector<float> *observed, double RSS_reg, size_t limit_L, size_t limit_R);

    float calcRegressionFvalue(const std::vector<float> *selectLog,
                               const std::vector<float> *intensities,
                               const float mse);

    float calcSSE_exp(const RegCoeffs coeff,
                      const std::vector<float> *y_start,
                      size_t limit_L, size_t limit_R,
                      size_t index_x0);

    float calcSSE_chisqared(const RegCoeffs coeff,
                            const std::vector<float> *y_start,
                            size_t limit_L,
                            size_t limit_R,
                            size_t index_x0);

    struct CorrectionFactors
    {
        double log_C;
        double varLog_C;
    };

    CorrectionFactors smearingCorrection(
        const std::vector<float> *predictLog,
        const std::vector<float> *selectLog);

    struct RegPair
    {
        unsigned int idx;
        float mse;
    };

    RegPair findBestRegression(
        const std::vector<float> *intensities,
        const std::vector<RegressionGauss> *regressions,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const size_t startIdx,
        const size_t endIdx);

    // this just substracts the two relevant values from a cumsum vector
    size_t calcDF_cum(
        const std::vector<unsigned int> *degreesOfFreedom,
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
    inline double multiplyVecMatrixVecTranspose(const double vec[4], size_t scale);

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

    MeanVar weightedMeanAndVariance_EIC(const std::vector<float> *weight,
                                        const std::vector<float> *values,
                                        size_t left_limit,
                                        size_t right_limit);

    // ### pre-calculate the regression matrix ### //
#define MAXSCALE 63

    constexpr std::array<double, (MAXSCALE + 1) * 6> initialize()
    { // array to store the 6 unique values of the inverse matrix for each scale
        std::array<double, (MAXSCALE + 1) * 6> invArray = {0};
        // init invArray
        // XtX = transposed(Matrix X ) * Matrix X
        // XtX_xy: x = row number; y = column number
        double XtX_00 = 1;
        double XtX_02 = 0;
        double XtX_11 = 0;
        double XtX_12 = 0;
        double XtX_13 = 0;
        double XtX_22 = 0;
        for (int i = 1; i < MAXSCALE; ++i)
        {
            XtX_00 += 2;
            XtX_02 += double(i * i);
            XtX_11 = XtX_02 * 2;
            XtX_13 += double(i * i * i);
            XtX_12 = -XtX_13;
            XtX_22 += double(i * i * i * i);

            // decomposition matrix L, see https://en.wikipedia.org/wiki/Cholesky_decomposition
            double L_00 = std::sqrt(XtX_00);
            double L_11 = std::sqrt(XtX_11);
            double L_20 = XtX_02 / L_00;
            double L_21 = XtX_12 / L_11;
            double L_20sq = L_20 * L_20;
            double L_21sq = L_21 * L_21;
            double L_22 = std::sqrt(XtX_22 - L_20sq - L_21sq);
            double L_32 = 1 / L_22 * (-L_20sq + L_21sq);
            double L_33 = std::sqrt(XtX_22 - L_20sq - L_21sq - L_32 * L_32);

            // inverse of L
            double inv_00 = 1 / L_00;
            double inv_11 = 1 / L_11;
            double inv_22 = 1 / L_22;
            double inv_33 = 1 / L_33;
            double inv_20 = -L_20 * inv_00 / L_22;
            double inv_30 = -(L_20 * inv_00 + L_32 * inv_20) / L_33;
            double inv_21 = -L_21 * inv_11 / L_22;
            double inv_31 = -(-L_21 * inv_11 + L_32 * inv_21) / L_33;
            double inv_32 = -L_32 * inv_22 / L_33;

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