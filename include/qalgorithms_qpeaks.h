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
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>> findPeaks(const varDataType &dataVec);

    // debugging
    void info() const;
    void printMatrices(int scale) const;

  private:
    std::vector<std::unique_ptr<Matrix>> designMatrices;
    std::vector<std::unique_ptr<Matrix>> inverseMatrices;
    std::vector<std::unique_ptr<Matrix>> psuedoInverses;

    // define valid regression struct
    struct validRegression
    {
      int index_x0; // index of window center (x==0) in the Y matrix 
      int scale;
      int df;
      double apex_position;
      double mse;
      Matrix B;
      bool isValid;
      double left_limit;
      double right_limit;
      int X_row_0;
      int X_row_1;
      double dqs;

      validRegression(
          int index_x0,
          int scale,
          int df,
          double apex_position,
          double mse,
          Matrix B,
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
      double mse;         // mean squared error
      int index;          // index of the best regression (in B matrix)
      int scale;          // extended scale of the regression window
      int df;             // degree of freedom
      int left_limit;     // left limit of the peak regression window
      int right_limit;    // right limit of the peak regression window
      int X_row_0;        //  start of the cutted Deisgn Matrix
      int X_row_1;        // end of the cutted Design Matrix
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

    int global_maxScale;

    // methods
    int calculateNumberOfRegressions(const int n) const;

    void runningRegression(
        const RefMatrix &Y,
        const std::vector<int *> &df,
        std::vector<std::unique_ptr<validRegression>> &validRegressions);

    void validateRegressions(
        const Matrix &B,
        const RefMatrix &Y,
        const Matrix &Ylog,
        const std::vector<int *> &df,
        const int scale,
        std::vector<std::unique_ptr<validRegression>> &validRegressions);

    void mergeRegressionsWithinScale(
        std::vector<std::unique_ptr<validRegression>> &validRegressions,
        std::vector<std::unique_ptr<validRegression>> &validRegressionsTmp,
        const RefMatrix &Y,
        const int scale,
        const std::vector<int *> &df);

    void mergeRegressionsOverScales(
        std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const Matrix &Ylog, 
        const RefMatrix &Y,
        const std::vector<int *> &df);

    std::vector<std::unique_ptr<DataType::Peak>> createPeaks(
        const std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const RefMatrix &Y,
        const std::vector<double *> &X,
        const int scanNumber);

    double calcMse(const Matrix &yhat, const Matrix &y) const;

    double calcMse(const Matrix &yhat, const RefMatrix &y) const;

    double calcMseEXP(const Matrix &yhat_log, const RefMatrix &y) const;

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
    void calcExtendedMse(
        const RefMatrix &Y,
        // const Matrix &B,
        const std::vector<std::unique_ptr<validRegression>> &regressions,
        // const int scale,
        const std::vector<int *> &df);

    void calcExtendedMse(
        const RefMatrix &Y,
        validRegression &currentReggression,
        validRegression &refReggression,
        const std::vector<int *> &df);

    /**
     * @brief Calculate the chi square value of the regression model with the given regression window in the exponential space.
     * 
     * @param yhat_log : Log transformed prediction 
     * @param y_exp : Exponential transformed measurement data
     * @return double : Chi square value
     */
    double calcChiSquareEXP(
        const Matrix &yhat_log,
        const RefMatrix &y_exp) const;

    std::pair<Matrix, Matrix> jacobianMatrix_PeakArea(const Matrix &B, int scale) const;

    /**
     * @brief Create a Design Matrix object for the given scale.
     * @details The design matrix is a matrix of size (2*scale+1) x 4. The basis for x is a vector of integers from -scale to scale. The first column is a vector of ones, the second column is a vector of x values, the third and fourth columns are vectors of x^2 values. However, the third column is only filled with x^2 values if x is less than 0. The fourth column is only filled with x^2 values if x is greater than 0.
     *
     * @param scale
     * @return Matrix
     */
    void createDesignMatrix(const int scale);

    /**
     * @brief Create the inverse and pseudo-inverse matrices for the given design matrix.
     *
     * @param X is input design matrix
     */
    void createInverseAndPseudoInverse(const Matrix &X);
  };
} // namespace q

#endif // QALGORITHMS_QPEAKS_H