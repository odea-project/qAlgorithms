// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_datatypes.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace qAlgorithms
{
    // @todo rework this module so all centroiding / feature construction parts are in a separate block and qPeaks can be used as a library header

    // ### General Implementation ### //

    /// @brief This function serves as the central interface for performing peak detection on an arbitrary set of x and y data.
    /// @param intensity_base Values used to determine height of the peak
    /// @param x_values Values used as width of the peak, same length as y_values. It is a requirement that x values are evenly
    ///                 spaced when applying this function, since it depends on that assumption for its precalculated matric transform.
    ///                 We use "x" because this is either m/z or RT.
    /// @param DF_cum cumulative degrees of freedom to account for interpolations and mean values. This is the cumulative
    //                              sum of the number of measured points for all y. It has the same length as x and y.
    //                              This can be set to nullptr if all df are one.
    /// @param length length of y, x and df if supplied
    /// @param maxscale up to which scale regressions should be attempted. The maximum peak width (in points) is maxscale * 2 + 1.
    //                  If maxscale would exceed the length of x or y, it is set to (the length of either -1) / 2.
    /// @param result All found peaks are appended to this vector.
    /// @return the total number of appended peaks. If this is < 0, the function did not execute correctly.
    /// @details Errorcodes:
    ///  0 = no valid peaks were found (this is not an error per se, when supplying ex. a constant y this is the expected result)
    /// -1 = one of y_values, x_values or detectedPeaks was nullptr
    /// -2 = lenght is smaller than minimum required
    /// -3 = maxscale is < 2
    /// -4 = maxscale exceeds maxscale determined by implementation
    /// ### Assumptions: ###  these are not tested @todo
    /// x and degreesOfFreedom_cum increase monotonically.
    /// y has equal variance at every point
    /// there is enough space to write the results
    int qpeaks_find(
        const float *intensity_base,
        const float *x_values,
        const uint16_t *DF_cum,
        const size_t length,
        size_t maxscale,
        std::vector<PeakFit> *result);

    class XML_File;                         // forward declaration so at least the header does not couple with read_file
    int findCentroids(const XML_File *data, // @todo the internal file representation should be more generic
                      const std::vector<unsigned int> *selectedIndices,
                      std::vector<CentroidPeak> *centroids);

    void centroids_to_mzml(const path_char *pathTarget, const path_char *pathSource);

    /// @param intensity_log logarithmy of the intensity values. This is the y axis of the fit. The x axis is required to be equidistant.
    /// @param maxscale maximum scale of a peak that should be attempted to fit.
    /// @param coeffs Sets of coefficients for all possible regressions. They are written out in the order scale = 2, scale = 3, ... , scale = maxscale
    //           Note that not all scales have the same number of regressions
    void findCoefficients(
        const float *intensity_log,
        const size_t length,
        size_t maxscale,
        std::vector<RegCoeffs> *coeffs);

    void findBestScales(std::vector<RegressionGauss> *validRegressions,
                        std::vector<RegressionGauss> *validRegsTmp,
                        const float *intensities,
                        const uint16_t *const degreesOfFreedom_cum);

    // mutate b0 so that it is optimal for the exponential case if b1, b2 and b3 are identical

    /// @brief adjust the height of a regression to better fit the exponential data
    /// @param intensities non-logarithmic intensity values the regression was fitted to
    /// @param r range of the regression
    /// @param predicted empty vector that the predicted values for intensity (AFTER correction) are written to
    /// @param coeff coefficients that should be updated
    /// @return used correction factor
    double correctB0(const float *const intensities,
                     const Range_i *r,
                     float *predicted,
                     RegCoeffs *coeff);

    enum invalid // NOLINT @todo rework causes for invalidation
    {
        ok = 0,
        no_apex = 1,
        invalid_apex = 2,
        no_df = 3,
        invalid_apexToEdge = 4, // this will probably be removed
        f_test_fail = 5,
        invalid_quadratic = 6,
        invalid_area = 7,
        invalid_height = 8,
        invalid_chisq = 9,
        none = -1
    };

    /// @brief perform various statistical tests to see if a regression describes a valid peak
    /// @param degreesOfFreedom_cum cumulative degrees of freedom (only relevant for interpolated data)
    /// @param intensities measured intensities
    /// @param intensities_log log of measured intensities - must have same length as intensities
    /// @param mutateReg regression that should be mutated by this function
    /// @return 0 if the regression is valid, otherwise the filter step which kicked it out
    invalid calcRegressionProperties(
        const float *intensities,
        const float *intensities_log,
        const float *x_axis,
        const float *predict,
        const size_t df_sum,
        const size_t length,
        RegressionGauss *mutateReg);

    // ### Feature-specific Code ### //

    int findFeatures(const std::vector<EIC> *EICs,
                     const std::vector<float> *convertRT, // correct RT corresponding to every scan number
                     std::vector<FeaturePeak> *res);

    // take a jacobian matrix as input and return the transpose at scale
    double matProductReg(const double J[4], const size_t scale);

    bool isValidQuadraticTerm(const RegCoeffs *coeffs, const double mse, const size_t df_sum);

    // utility functions for calculating regression values
    double regAt(const RegCoeffs *coeff, const double x);

    double fullWidthHalfMax(const RegCoeffs *coeff, const double height, const double delta_x);

    // this one does not include b0
    double regExp_fac(const RegCoeffs *coeff, const double x);

    double medianVec(const std::vector<float> *vec);

    double peakPositionUncert(const RegCoeffs *c, const double mse);
    double peakHeightUncert(const RegCoeffs *c, const double mse);

    double peakArea(const RegCoeffs *c, const double delta_x, const double mse, double *uncert);

    double peakPosition(const RegCoeffs *c);

#define GLOBAL_MINSCALE 2
#define MINLENGTH 5

#include "../external/qalgorithms_matinverse.h"

#ifndef QALGORITHMS_MAXSCALE_PRECOMPILED
    #error Required array of matrix inverses not defined. Ensure that qalgorithms_matinverse.h exists. You can try regenerating it by going into the "external" directory and running 'cc qalgorithms_matinverse.c -o a.exe && ./a.exe'.
#endif

    // the maximum possible number of apexes that can be present in one continuous section
    // we assume a small number for stack allocation
    const size_t max_apex_per_group = 128;

    // This file contains precomputed values for the inverse matrix transpose (X^T X)^(-1).
    // The precalculation is required since it is a very expensive operation, especially for large X.

    // (X^T X)^(-1) is pre-calculated and stored in the array qalgo_matInverse (included via qalgorithms_matinverse.h).
    // Every array element is accessed by the index of its corresponding scale. This means that, while
    // qalgo_matInverse[0] and qalgo_matInverse[1] are defined, they can never be used to calculate anything
    // because the regression function is only defined starting at scale 2.
    // Only six values of the final matrix are required for the simple case, see below:

    // xT is the transpose of the design matrix X.
    // for scale = 2:
    // xT = | 1  1  1  1  1 |    : all ones
    //      |-2 -1  0  1  2 |    : from -scale to scale
    //      | 4  1  0  0  0 |    : x^2 values for x < 0
    //      | 0  0  0  1  4 |    : x^2 values for x > 0

    // It contains one additional row of all ones for every additional peak that is added into the model

    // When adding multiple peaks to the regression model, we need to adjust the inverse values.
    // This will change the number of unique values in the inv_values array from 6 to 7.
    // Here we use the inv_array[1] position and shift all values from that point onwards to the right.
    // example for num_peaks = 2:
    // original matrix with the unique values [a, b, c, d, e, f] (six unique values)
    // | A  0  B  B |
    // | 0  C  D -D |
    // | B  D  E  F |
    // | B -D  F  E |

    // new matrix with the unique values [A1, A2, B, C, D, E, F] (seven unique values)
    // | A1  A2  0  B  B |
    // | A2  A1  0  B  B |
    // | 0   0   C  D -D |
    // | B   B   D  E  F |
    // | B   B  -D  F  E |

    // for num_peaks = 3:
    // new matrix with the unique values [A1, A2, B, C, D, E, F] (the same seven unique values)
    // | A1  A2  A2  0  B  B |
    // | A2  A1  A2  0  B  B |
    // | A2  A2  A1  0  B  B |
    // | 0   0   0   C  D -D |
    // | B   B   B   D  E  F |
    // | B   B   B  -D  F  E |

    // Note that no more than seven different values are needed per scale, even for a multidimensional approach.
    // A1 and A2 can be calculated from the initial values of A and B, which is why they are not included in the
    // precomputation.
} // namespace qAlgorithms

#endif