// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_matrix.h"
#include "qalgorithms_refmatrix.h"
#include "qalgorithms_measurement_data.h"
#include "qalgorithms_peak.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_datatype_peak.h"

// external
#include <vector>
#include <memory>
#include <omp.h>
#include <limits>
#include <numeric>

/* This file includes the q::qPeaks class*/
namespace q
{
  /**
   * @brief A class to store and apply a peak evaluation model.
   */
  class qPeaks
  {
  public:
    // Constructor
    qPeaks();

    /**
     * @brief Construct a new q Peaks object using varDataType object to initialize the object Matrices.
     *
     * @param dataVec
     */
    qPeaks(const varDataType &dataVec);

    // Destructor
    ~qPeaks();

    // methods
    /**
     * @brief Find the peaks in the data. Container function to call the findPeaks method.
     *
     * @param dataVec
     */
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>> 
    findPeaks(const varDataType &dataVec);

    // debugging
    void info() const;
    void printMatrices(int scale) const;

  private:
    std::vector<std::unique_ptr<Matrix>> designMatrices;
    std::vector<std::unique_ptr<Matrix>> inverseMatrices;
    std::vector<std::unique_ptr<Matrix>> psuedoInverses;

    int global_maxScale;

    // define valid regression struct
    struct validRegression
    {
      int index_x0;         // index of window center (x==0) in the Y matrix
      int scale;            // scale of the regression window, i.e., 2*scale+1 = window size
      int df;               // degree of freedom, interpolated data points will not be considered
      double apex_position; // position of the apex of the peak
      double mse;           // mean squared error
      Vector B;             // regression coefficients
      bool isValid;         // flag to indicate if the regression is valid
      double left_limit;    // left limit of the peak regression window
      double right_limit;   // right limit of the peak regression window
      int X_row_0;          // start of the cutted Deisgn Matrix
      int X_row_1;          // end of the cutted Design Matrix
      double dqs;           // data quality score

      validRegression(
          int index_x0,
          int scale,
          int df,
          double apex_position,
          double mse,
          Vector B,
          bool isValid = true,
          double left_limit = 0.0,
          double right_limit = 0.0,
          int X_row_0 = 0,
          int X_row_1 = 0,
          double dqs = 0.0)
          : index_x0(index_x0),
            scale(scale),
            df(df),
            apex_position(apex_position),
            mse(mse),
            B(B),
            isValid(isValid),
            left_limit(left_limit),
            right_limit(right_limit),
            X_row_0(X_row_0),
            X_row_1(X_row_1),
            dqs(dqs) {}
    };

    struct extendedMSE
    {
      double mse;      // mean squared error
      int index;       // index of the best regression (in B matrix)
      int scale;       // extended scale of the regression window
      int df;          // degree of freedom
      int left_limit;  // left limit of the peak regression window
      int right_limit; // right limit of the peak regression window
      int X_row_0;     //  start of the cutted Deisgn Matrix
      int X_row_1;     // end of the cutted Design Matrix
      extendedMSE(
          double mse,
          int index,
          int scale,
          int df,
          int left_limit,
          int right_limit,
          int X_row_0,
          int X_row_1)
          : mse(mse),
            index(index),
            scale(scale),
            df(df),
            left_limit(left_limit),
            right_limit(right_limit),
            X_row_0(X_row_0),
            X_row_1(X_row_1) {}
    };

    // methods
    int
    calculateNumberOfRegressions(const int n) const;

    void
    runningRegression(
        const Vector &Y,
        const BoolVector &df,
        std::vector<std::unique_ptr<validRegression>> &validRegressions);

    void
    validateRegressions(
        const Matrix_mc &B,
        const Vector &Y,
        const Vector &Ylog,
        const BoolVector &df,
        const int scale,
        std::vector<std::unique_ptr<validRegression>> &validRegressions);

    void
    mergeRegressionsOverScales(
        std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const Vector &Ylog,
        const Vector &Y,
        const BoolVector &df);

    std::vector<std::unique_ptr<DataType::Peak>>
    createPeaks(
        const std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const Vector &Y,
        const Vector &X,
        const int scanNumber);

    double
    calcSSE(
        const Vector &yhat,
        const Vector &y,
        const double *y_start = nullptr) const;

    double
    calcSSEexp(
        const Vector &yhat_log,
        const Vector &y,
        const double *y_start = nullptr) const;

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
        const Vector &Y,
        const std::vector<std::unique_ptr<validRegression>> &regressions,
        const BoolVector &df);

    /**
     * @brief Calculate the chi square value of the regression model with the given regression window in the exponential space.
     *
     * @param yhat_log : Log transformed prediction
     * @param y_exp : Exponential transformed measurement data
     * @return double : Chi square value
     */
    double
    calcChiSquareEXP(
        const Vector &yhat_log,
        const Vector &y_exp,
        const double *y_start = nullptr) const;

    /**
     * @brief Calculate the degree of freedom of the regression model with the given regression window.
     * @details The degree of freedom is the number of data points minus the number of regression coefficients. Moreover, the degree of freedom is reduced by the number of interpolated data points. For calculating the degree of freedom, the function uses the df vector that contains a value of 1 if the data point is not interpolated and 0 if the data point is interpolated.
     *
     * @param df : Vector of integers that indicates if the data point is interpolated or not
     * @param left_limit : Start index of the regression window
     * @param right_limit : End index of the regression window
     * @return int : Degree of freedom
     */
    int
    calcDF(
        const BoolVector &df,
        const size_t left_limit,
        const size_t right_limit);

    /**
     * @brief Calculate the apex (and if possible the valley) position of the peak. And return true if the positions are calculated are valid.
     *
     * @param B : Matrix of regression coefficients
     * @param index : Index of the regression window
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @param apex_position : apex position
     * @param valley_position : valley position
     * @return true : if the apex and valley positions are valid
     * @return false : if the apex and valley positions are not valid (e.g., the apex position is not in the regression window)
     */
    bool
    calculateApexAndValleyPositions(
        const Matrix_mc &B,
        const size_t index,
        const int scale,
        double &apex_position,
        double &valley_position) const;

    /**
     * @brief Check if the quadratic term of the regression model is valid using t-test.
     *
     * @param B : Matrix of regression coefficients
     * @param index : Index of the regression window
     * @param inverseMatrix_2_2 : quadratic term (left side) (diagonal element of the inverse matrix)
     * @param inverseMatrix_3_3 : quadratic term (right side) (diagonal element of the inverse matrix)
     * @param mse : mean squared error
     * @param df_sum : sum of the degree of freedom of the regression model
     * @return true : if the quadratic term is valid
     * @return false : if the quadratic term is not valid
     */
    bool
    isValidQuadraticTerm(
        const Matrix_mc &B,
        const size_t index,
        const double inverseMatrix_2_2,
        const double inverseMatrix_3_3,
        const double mse,
        const int df_sum) const;

    /**
     * @brief Check if the peak height is valid using t-test.
     *
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
        const Matrix_mc &B,
        const Matrix_mc_4x4 &C,
        const size_t index,
        const double apex_position,
        const int df_sum) const;

    /**
     * @brief Check if the peak area and the covered peak area are valid using t-test.
     * @details The function calculates the peak area and the covered peak area using the regression coefficients. The peak area is the integral of the regression model from -infinity to +infinity. The covered peak area is the integral of the regression model from the left limit of the regression window to the right limit of the regression window. Moreover, the trapzoid under the peak is considered as not covered peak area.
     *
     * @param B : Matrix of regression coefficients
     * @param C : Variance-covariance matrix of the regression coefficients
     * @param index : Index of the regression window
     * @param scale : Window size scale, e.g., 5 means the window size is 11 (2*5+1)
     * @param df_sum : sum of the degree of freedom of the regression model
     * @return true : if the peak area is valid
     * @return false : if the peak area is not valid
     */
    bool
    isValidPeakArea(
        const Matrix_mc &B,
        const Matrix_mc_4x4 &C,
        const size_t index,
        const int scale,
        const int df_sum) const;

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
    createInverseAndPseudoInverse(const Matrix &X);
  };
} // namespace q

#endif // QALGORITHMS_QPEAKS_H