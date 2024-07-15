// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_matrix.h"
#include "qalgorithms_measurement_data.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_datatype_peak.h"

// external
#include <vector>
#include <memory>
#include <omp.h>
#include <limits>
#include <numeric>
#include <fstream>
#include <cstdlib>
#include <xmmintrin.h> // SSE
#include <immintrin.h> // AVX
#include <cassert>

/* This file includes the q::qPeaks class*/
namespace q
{
    namespace Algorithms
    {
        /**
         * @brief A class to store and apply a peak evaluation model.
         */
        class qPeaks
        {
        public:
            static const int global_maxScale = 15; // maximum scale for regression window

            // Constructor and Destructor
            qPeaks();
            ~qPeaks();

            // methods
            /**
             * @brief Find the peaks in the data. Container function to call the findPeaks method.
             * @param dataVec
             */
            std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
            findPeaks(const q::MeasurementData::varDataType &dataVec);

            // export
            void
            peakListToCSV(
                const std::vector<std::vector<std::unique_ptr<DataType::Peak>>> &allPeaks,
                const std::string &filename) const;

            void
            plotPeaksToPython(
                const std::string &filename_input,
                const std::string &filename_output,
                const bool includeFits = true,
                const bool featureMap = false) const;

            static void initialize();

        private:
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
            alignas(16) static float invArray[64][6]; // contains the unique entries from the inverse matrix
            alignas(16) static float x_square[128];   // contains the squares from 0 to 127^2
            static __m128 ZERO_128;                   // 128 bit register with all zeros
            static __m256 ZERO_256;                   // 256 bit register with all zeros
            static __m128 KEY_128;                    // 128 bit register with 0, 4, 2, 1
            static __m256 LINSPACE_UP_POS_256;        // 256 bit register with 0, 1, 2, 3, 4, 5, 6, 7
            static __m256 LINSPACE_UP_NEG_256;        // 256 bit register with 0, -1, -2, -3, -4, -5, -6, -7
            static __m256 LINSPACE_DOWN_NEG_256;      // 256 bit register with -7, -6, -5, -4, -3, -2, -1, 0
            static __m256i LINSPACE_UP_INT_256;       // 256 bit register with 0, 1, 2, 3, 4, 5, 6, 7
            static __m256i LINSPACE_DOWN_INT_256;     // 256 bit register with 7, 6, 5, 4, 3, 2, 1, 0
            static __m256 MINUS_ONE_256;              // 256 bit register with -1

            // define valid regression struct
            struct validRegression
            {
                int index_x0;        // index of window center (x==0) in the Y matrix
                int scale;           // scale of the regression window, i.e., 2*scale+1 = window size
                int df;              // degree of freedom, interpolated data points will not be considered
                float apex_position; // position of the apex of the peak
                float mse;           // mean squared error
                __m128 coeff;        // regression coefficients
                bool isValid;        // flag to indicate if the regression is valid
                float left_limit;    // left limit of the peak regression window
                float right_limit;   // right limit of the peak regression window
                // float area;               // area of the peak
                // float uncertainty_area;   // uncertainty of the area
                // float uncertainty_height; // uncertainty of the height

                validRegression(
                    int index_x0 = 0,
                    int scale = 0,
                    int df = 0,
                    float apex_position = 0.0f,
                    float mse = 0.0f,
                    __m128 coeff = _mm_setzero_ps(),
                    bool isValid = false,
                    float left_limit = 0.0f,
                    float right_limit = 0.0f) //,
                    // float area = 0.0f,
                    // float uncertainty_area = 0.0f,
                    // float uncertainty_height = 0.0f)
                    : index_x0(index_x0),
                      scale(scale),
                      df(df),
                      apex_position(apex_position),
                      mse(mse),
                      coeff(coeff),
                      isValid(isValid),
                      left_limit(left_limit),
                      right_limit(right_limit)
                {
                } //,
                  //   area(area),
                  //   uncertainty_area(uncertainty_area),
                  //   uncertainty_height(uncertainty_height) {}
            };

            struct validRegression_static
            {
                int index_x0;        // index of window center (x==0) in the Y matrix
                int scale;           // scale of the regression window, i.e., 2*scale+1 = window size
                int df;              // degree of freedom, interpolated data points will not be considered
                float apex_position; // position of the apex of the peak
                float mse;           // mean squared error
                __m128 coeff;        // regression coefficients
                bool isValid;        // flag to indicate if the regression is valid
                int left_limit;      // left limit of the peak regression window
                int right_limit;     // right limit of the peak regression window
                // float area;               // area of the peak
                // float uncertainty_area;   // uncertainty of the area
                // float uncertainty_height; // uncertainty of the height
                validRegression_static() = default;
                // void setValues(
                //     int index_x0 = 0,
                //     int scale = 0,
                //     int df = 0,
                //     float apex_position = 0.0f,
                //     float mse = 0.0f,
                //     __m128 coeff = _mm_setzero_ps(),
                //     bool isValid = false,
                //     float left_limit = 0.0f,
                //     float right_limit = 0.0f)
                // {
                //     this->index_x0 = index_x0;
                //     this->scale = scale;
                //     this->df = df;
                //     this->apex_position = apex_position;
                //     this->mse = mse;
                //     this->coeff = coeff;
                //     this->isValid = isValid;
                //     this->left_limit = left_limit;
                //     this->right_limit = right_limit;
                // }
            };

            // methods
            int
            calculateNumberOfRegressions(const int n) const;

            void
            runningRegression(
                const float *y_start,
                const float *ylog_start,
                const bool *df_start,
                const int n,
                std::vector<std::unique_ptr<validRegression>> &validRegressions);

            void
            runningRegression_static(
                const float *y_start,
                const float *ylog_start,
                const bool *df_start,
                const int n,
                validRegression_static *validRegressions,
                int &validRegressionsIndex);

            void
            validateRegressions(
                const __m128 *beta,
                const int n_segments,
                const float *y_start,
                const float *ylog_start,
                const bool *df_start,
                const int scale,
                std::vector<std::unique_ptr<validRegression>> &validRegressions);

            void
            validateRegressions_static(
                const __m128 *beta,
                const int n_segments,
                const float *y_start,
                const float *ylog_start,
                const bool *df_start,
                const int scale,
                int &validRegressionsIndex,
                validRegression_static *validRegressions);

            bool
            validateRegressions_testseries(
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
                int &right_limit);

            void
            mergeRegressionsOverScales(
                std::vector<std::unique_ptr<validRegression>> &validRegressions,
                const float *y_start,
                const float *ylog_start,
                const bool *df_start);

            void
            mergeRegressionsOverScales_static(
                validRegression_static *validRegressions,
                const int n_regressions,
                const float *y_start,
                const float *ylog_start,
                const bool *df_start);

            std::vector<std::unique_ptr<DataType::Peak>>
            createPeaks(
                const std::vector<std::unique_ptr<validRegression>> &validRegressions,
                const float *y_start,
                const float *x_start,
                const int scanNumber);

            std::vector<std::unique_ptr<DataType::Peak>>
            createPeaks_static(
                validRegression_static *validRegressions,
                const int validRegressionsIndex,
                const float *y_start,
                const float *x_start,
                const int scanNumber);

            std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
            createPeakList(std::vector<std::vector<std::unique_ptr<DataType::Peak>>> &allPeaks);

            float
            calcSSE(
                const int left_limit,
                const int right_limit,
                const __m128 &coeff,
                const float *y_start = nullptr,
                const bool calc_EXP = false,
                const bool calc_CHISQ = false) const;

            /**
             * @brief Calculate the best mean squared error of the regression model with different regression windows BUT same window size.
             * @details The function extends the regression windows that all the windows cover the range from the lowest x value to the highest x value. I.e., if window A is [4,5,6,7,8,9,10] and window B is [6,7,8,9,10,11,12], the extended window is [4,5,6,7,8,9,10,11,12]. The function calculates the mean squared error of the regression model with the extended window and returns the mean squared error and the index of the best regression.
             *
             * @param Y : Measurement data (not log transformed)
             * @param B : Matrix of regression coefficients
             * @param groupIndices : Indices of the regression windows
             * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
             * @return std::pair<double,int> : MSE and index of the best regression window
             */
            void
            calcExtendedMse(
                const float *y_start,
                const std::vector<std::unique_ptr<validRegression>> &regressions,
                const bool *df_start);

            void
            calcExtendedMse_static(
                const float *y_start,
                validRegression_static *regressions_start,
                const int n_regressions,
                const bool *df_start);

            void
            calcExtendedMsePair_static(
                const float *y_start,
                validRegression_static *low_scale_regression,
                validRegression_static *hi_scale_regression,
                const bool *df_start);

            void
            calcExtendedMseOverScales_static(
                const float *y_start,
                validRegression_static *validRegressions,
                const std::vector<int> &validRegressionsInGroup,
                const int i_new_peak);

            /**
             * @brief Calculate the degree of freedom of the regression model with the given regression window.
             * @details The degree of freedom is the number of data points minus the number of regression coefficients. Moreover, the degree of freedom is reduced by the number of interpolated data points. For calculating the degree of freedom, the function uses the df vector that contains a value of 1 if the data point is not interpolated and 0 if the data point is interpolated.
             *
             * @param df : q::Matrices::Vector of integers that indicates if the data point is interpolated or not
             * @param left_limit : Start index of the regression window
             * @param right_limit : End index of the regression window
             * @return int : Degree of freedom
             */
            int
            calcDF(
                const bool *df_start,
                const int left_limit,
                const int right_limit);

            /**
             * @brief Calculate the apex (and if possible the valley) position of the peak. And return true if the positions are calculated are valid.
             * @param coeff : Matrix of regression coefficients
             * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
             * @param apex_position : apex position
             * @param valley_position : valley position
             * @return true : if the apex and valley positions are valid
             * @return false : if the apex and valley positions are not valid (e.g., the apex position is not in the regression window)
             */
            bool
            calculateApexAndValleyPositions(
                const __m128 &coeff,
                const int scale,
                float &apex_position,
                float &valley_position) const;

            /**
             * @brief Calculate the Matrix Product of J * Xinv * J^T for uncertainty calculation.
             * @details The function calculates the matrix product of J * Xinv * J^T. The matrix J is the Jacobian matrix with an 1x4 size. The matrix Xinv is the inverse matrix of X^T * X, where X is the design matrix. The matrix J * Xinv * J^T is a 1x1 matrix, i.e., a scalar value.
             *
             * @param vec
             * @param scale
             * @return float
             */
            float
            multiplyVecMatrixVecTranspose(
                const float vec[4],
                const int scale) const;

            /**
             * @brief Checks if peak maximum is twice as high as the signal at the edge of the regression window.
             * @details The test is used as a fast pre-test for signal-to-noise ratio which will be calculated later. However, s/n siginificance is time consuming due to MSE calculation. The reference value of two is used due to t = (apex/edge - 2) / (apex/edge * sqrt()). If apex is equal or less than two times the edge, the t value is less than zero, which means that SNR cannot be significant.
             *
             * @param apex_position
             * @param scale
             * @param index_loop
             * @param Y
             * @param apexToEdge : The ratio of the peak maximum to the signal at the edge of the regression window. This value is calculated by the function.
             * @return true
             * @return false
             */
            bool
            isValidApexToEdge(
                const double apex_position,
                const int scale,
                const int index_loop,
                const float *y_start,
                float &apexToEdge) const;

            /**
             * @brief Check if the quadratic term of the regression model is valid using t-test.
             * @param coeff : Matrix of regression coefficients
             * @param inverseMatrix_2_2 : quadratic term (left side) (diagonal element of the inverse matrix)
             * @param mse : mean squared error
             * @param df_sum : sum of the degree of freedom of the regression model
             * @return true : if the quadratic term is valid
             * @return false : if the quadratic term is not valid
             */
            bool
            isValidQuadraticTerm(
                const __m128 &coeff,
                const float inverseMatrix_2_2,
                const float mse,
                const int df_sum) const;

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
            bool
            isValidPeakHeight(
                const float mse,
                const int index,
                const int scale,
                const float apex_position,
                const float valley_position,
                const int df_sum,
                const float apexToEdge,
                float &uncertainty_height) const;

            /**
             * @brief Check if the peak area and the covered peak area are valid using t-test.
             * @details The function calculates the peak area and the covered peak area using the regression coefficients. The peak area is the integral of the regression model from -infinity to +infinity. The covered peak area is the integral of the regression model from the left limit of the regression window to the right limit of the regression window. Moreover, the trapzoid under the peak is considered as not covered peak area.
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
            bool
            isValidPeakArea(
                const __m128 &coeff,
                const float mse,
                const int scale,
                const int df_sum,
                float &area,
                float &uncertainty_area) const;

            /**
             * @brief Create a Design Matrix object for the given scale.
             * @details The design matrix is a matrix of size (2*scale+1) x 4. The basis for x is a vector of integers from -scale to scale. The first column is a vector of ones, the second column is a vector of x values, the third and fourth columns are vectors of x^2 values. However, the third column is only filled with x^2 values if x is less than 0. The fourth column is only filled with x^2 values if x is greater than 0.
             *
             * @param scale
             * @return Matrix
             */
            void
            createDesignMatrix(const int scale);

            /**
             * @brief Create the inverse and pseudo-inverse matrices for the given design matrix.
             *
             * @param X is input design matrix
             */
            void
            createInverseAndPseudoInverse(const q::Matrices::Matrix &X);

            void
            convolve_static(
                const size_t scale,
                const float *vec,
                const size_t n,
                __m128 *beta);

            void
            convolve_dynamic(
                const size_t scale,
                const float *vec,
                const size_t n,
                __m128 *beta);

            void
            convolve_SIMD(
                const size_t scale,
                const float *vec,
                const size_t n,
                __m128 *result,
                __m128 *products,
                const size_t buffer_size);
        };
    } // namespace Algorithms
} // namespace q

#endif // QALGORITHMS_QPEAKS_H