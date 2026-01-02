// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_datatypes.h"
#include "qalgorithms_read_file.h" // @todo get rid of this coupling!

#include <vector>

namespace qAlgorithms
{
    // @todo rework this module so all centroiding / feature construction parts are in a separate block and qPeaks can be used as a library header

    // ### General Implementation ### //

    /// @brief This function serves as the central interface for performing peak detection on an arbitrary set of x and y data.
    /// @param y_values Values used to determine height of the peak
    /// @param x_values Values used as width of the peak, same length as y_values. It is a requirement that x values are evenly
    ///                 spaced when applying this function, since it depends on that assumption for its precalculated matric transform.
    /// @param degreesOfFreedom_cum cumulative degrees of freedom to account for interpolations and mean values. This is the cumulative
    ///                             sum of the number of measured points for all y. It has the same length as x and y.
    /// @param maxScale up to which scale regressions should be attempted. The maximum peak width (in points) is maxScale * 2 + 1. maxScale must be > 1.
    ///                 If maxScale would exceed the length of x or y, it is set to (the length of either -1) / 2.
    /// @param detectedPeaks All found peaks are appended to this vector.
    /// @return the total number of appended peaks. If this is < 0, the function did not execute correctly.
    /// @details Errorcodes:
    ///  0 = no valid peaks were found (this is not an error per se, when supplying ex. a constant y this is the expected result)
    /// -1 = one of y_values, x_values or detectedPeaks was nullptr
    /// -2 = y_values, x_values or degreesOfFreedom_cum have unequal lengths
    /// -3 maxScale is < 2
    /// ### Assumptions: ###  these are not tested @todo
    /// x and degreesOfFreedom_cum increase monotonically.
    /// y has equal variance at every point
    /// there is enough space to write the results
    int qpeaks_find(
        const std::vector<float> *y_values,
        const std::vector<float> *x_values,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const size_t maxScale,
        std::vector<RegressionGauss> *detectedPeaks);

    /// @brief calculate all coefficients for all valid, symmetrical scales
    /// @details This function performs a convolution with the kernel (xTx)^-1 xT and the data array intensity_log.
    ///     (xTx)^-1 is pre-calculated and stored in the vector INV_ARRAY (calculated in the "initialise" function).
    ///     Only six values of the final matrix are required for the simple case, see below:

    ///     xT is the transpose of the design matrix X.
    ///     for scale = 2:
    ///     xT = | 1  1  1  1  1 |    : all ones
    ///          |-2 -1  0  1  2 |    : from -scale to scale
    ///          | 4  1  0  0  0 |    : x^2 values for x < 0
    ///          | 0  0  0  1  4 |    : x^2 values for x > 0

    ///     It contains one additional row of all ones for every additional peak that is added into the model

    ///     When adding multiple peaks to the regression model, we need to adjust the inverse values.
    ///     This will change the number of unique values in the inv_values array from 6 to 7.
    ///     Here we use the inv_array[1] position and shift all values from that point onwards to the right.
    ///     example for num_peaks = 2:
    ///     original matrix with the unique values [a, b, c, d, e, f] (six unique values)
    ///     | a  0  b  b |
    ///     | 0  c  d -d |
    ///     | b  d  e  f |
    ///     | b -d  f  e |

    ///     new matrix with the unique values [A1, A2, B, C, D, E, F] (seven unique values)
    ///     | A1  A2  0  B  B |
    ///     | A2  A1  0  B  B |
    ///     | 0   0   C  D -D |
    ///     | B   B   D  E  F |
    ///     | B   B  -D  F  E |

    ///     for num_peaks = 3:
    ///     new matrix with the unique values [A1, A2, B, C, D, E, F] (the same seven unique values)
    ///     | A1  A2  A2  0  B  B |
    ///     | A2  A1  A2  0  B  B |
    ///     | A2  A2  A1  0  B  B |
    ///     | 0   0   0   C  D -D |
    ///     | B   B   B   D  E  F |
    ///     | B   B   B  -D  F  E |

    ///     Note that no more than seven different values are needed per scale, even for a multidimensional approach.
    /// @param intensity_log logarithmy of the intensity values. This is the y axis of the fit. The x axis is required to be equidistant.
    /// @param maxscale maximum scale of a peak that should be attempted to fit.
    /// @param coeffs Sets of coefficients for all possible regressions. They are written out in the order scale = 2, scale = 3, ... , scale = maxscale
    ///               Note that not all scales have the same number of regressions
    void findCoefficients(
        const std::vector<float> *intensity_log,
        size_t maxscale,
        std::vector<RegCoeffs> *coeffs);

    void findBestScales(std::vector<RegressionGauss> *validRegressions,
                        std::vector<RegressionGauss> *validRegsTmp,
                        const float *intensities,
                        const std::vector<unsigned int> *degreesOfFreedom_cum);

    void runningRegression(
        const float *intensities,
        std::vector<float> *intensities_log,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        std::vector<RegressionGauss> *validRegressions,
        const size_t length,
        const size_t maxScale);

    // mutate b0 so that it is optimal for the exponential case if b1, b2 and b3 are identical

    /// @brief adjust the height of a regression to better fit the exponential data
    /// @param intensities non-logarithmic intensity values the regression was fitted to
    /// @param r range of the regression
    /// @param predicted empty vector that the predicted values for intensity (AFTER correction) are written to
    /// @param coeff coefficients that should be updated
    /// @return used correction factor
    double correctB0(const float *const intensities,
                     const Range_i *r,
                     std::vector<float> *predicted,
                     RegCoeffs *coeff);

    enum invalid
    {
        ok,
        no_apex,
        invalid_apex,
        no_df,
        invalid_apexToEdge, // this will probably be removed
        f_test_fail,
        invalid_quadratic,
        invalid_area,
        invalid_height,
        invalid_chisq,
        none = -1
    };

    /// @brief perform various statistical tests to see if a regression describes a valid peak
    /// @param degreesOfFreedom_cum cumulative degrees of freedom (only relevant for interpolated data)
    /// @param intensities measured intensities
    /// @param intensities_log log of measured intensities - must have same length as intensities
    /// @param mutateReg regression that should be mutated by this function
    /// @return 0 if the regression is valid, otherwise the filter step which kicked it out
    invalid makeValidRegression(
        const float *intensities,
        const std::vector<float> *intensities_log,
        const size_t df_sum,
        const size_t length,
        RegressionGauss *mutateReg);

    invalid validRegWidth(const RegCoeffs *coeffs, Range_i *range);

    // ### Centroiding-specific Code ### //

    int findCentroids(XML_File &data, // @todo get rid of the direct coupling to pugixml
                      const std::vector<unsigned int> *selectedIndices,
                      std::vector<CentroidPeak> *centroids);

    size_t getProfileRegions(
        std::vector<ProfileBlock> *groupedData,
        const std::vector<float> *spectrum_mz,
        const std::vector<float> *spectrum_int);

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

    std::vector<FeaturePeak> findFeatures(std::vector<EIC> &data,
                                          const RT_Converter *convertRT);

    // ### Retention Time Conversion @todo make less annoying ### //

    RT_Converter interpolateScanNumbers(const std::vector<float> *retentionTimes);

    inline float calcRTDiff(const std::vector<double> *retention_times);

    void createFeaturePeaks(
        std::vector<FeaturePeak> *peaks,
        const std::vector<RegressionGauss> *validRegressionsVec,
        const RT_Converter *convertRT,
        const std::vector<float> *RTs);

    /// @brief calculate the residual sum of squares for the log regression / data
    /// @param mutateReg relevant regression
    /// @param y_start log data
    /// @return RSS value
    double calcRSS_log(const RegressionGauss *mutateReg, const std::vector<float> *y_start);

    /// @brief performs two F-tests against the log data. First H0 is the mean, second y = mx + b
    /// @param observed log data (or normal data, depends on the use case)
    /// @param RSS_reg previously calculated residual sum of squares of the complex model. Hard assumpion of four coefficients.
    /// @param range range of the regression.
    /// @return true: Regression is significant; false: Regression is not better than either alternative.
    bool f_testRegression(const float *observed, double RSS_reg, const Range_i *range);

    double calcMSE_exp(const RegCoeffs *coeff,
                       const float *observed,
                       const Range_i *regSpan,
                       const double df);

    double calcSSE_chisqared(const RegressionGauss *mutateReg,
                             const float *observed,
                             const std::vector<float> *predict);

    struct RegPair
    {
        unsigned int idx;
        double mse;
    };

    RegPair findBestRegression(
        const float *intensities,
        const std::vector<RegressionGauss> *regressions,
        const std::vector<unsigned int> *degreesOfFreedom_cum,
        const Range_i regSpan);

    /**
     * @brief updates apex position field of mutateReg and the supplied valley_position
     * @return 0 if positions could be calculated, otherwise the point of failure in the
     * function is returned.
     */
    int calcApexAndValleyPos(
        RegressionGauss *mutateReg,
        double *valley_position);

    // take a jacobian matrix as input and return the transpose at scale
    double calcUncertainty(const double J[4], const size_t scale, const double mse);
    double matProductReg(const double J[4], const size_t scale);

    bool isValidQuadraticTerm(const RegressionGauss *mutateReg, const size_t df_sum);

    bool isValidPeakHeight(
        const RegressionGauss *mutateReg,
        const double valley_position,
        const size_t df_sum,
        const double apexToEdge);

    void calcPeakHeightUncert(RegressionGauss *mutateReg);

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

    void calcPeakAreaUncert(RegressionGauss *mutateReg);

    bool isValidPeakArea(const RegressionGauss *mutateReg, const size_t df_sum);

    void calcUncertaintyPos(RegressionGauss *mutateReg);

    struct MeanVar
    {
        float mean;
        float var;
    };

    MeanVar weightedMeanAndVariance_EIC(const std::vector<float> *weight,
                                        const std::vector<float> *values,
                                        const Range_i regSpan);

    // utility functions for calculating regression values
    double regAt(const RegCoeffs *coeff, const double x);
    double regExpAt(const RegCoeffs *coeff, const double x);

    // this one does not include b0
    double regExp_fac(const RegCoeffs *coeff, const double x);

    double medianVec(const std::vector<float> *vec);

    // ### pre-calculate the regression matrix ### //
#define MAXSCALE 63
#define GLOBAL_MAXSCALE_CENTROID 8 // @todo this is a critical part of the algorithm and should not be hard-coded
#define GLOBAL_MINSCALE 2

#include <array>
#include <math.h> // square root

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