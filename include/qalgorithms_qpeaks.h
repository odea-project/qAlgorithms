// qalgorithms_qpeaks.h
#ifndef QALGORITHMS_QPEAKS_H
#define QALGORITHMS_QPEAKS_H

// internal
#include "qalgorithms_matrix.h"
#include "qalgorithms_refmatrix.h"
#include "qalgorithms_measurement_data.h"
#include "qalgorithms_peak.h"
#include "qalgorithms_utils.h"

// external
#include <vector>
#include <memory>
#include <omp.h>
#include <limits>

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
    void findPeaks(const varDataType &dataVec);


    
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
      int index;
      int scale;
      double apex_position;
      double mse;
      Matrix B;
      bool isValid;

      validRegression(int index, int scale, double apex_position, double mse, Matrix B, bool isValid = true) : index(index), scale(scale), apex_position(apex_position), mse(mse), B(B), isValid(isValid) {}
    };

    int global_maxScale;

    // methods
    int calculateNumberOfRegressions(const int n) const;

    void runningRegression(const RefMatrix &X, const RefMatrix &Y);

    void validateRegressions(const Matrix &B, const Matrix &Ylog, const int scale, std::vector<std::unique_ptr<validRegression>> &validRegressions);

    void mergeRegressionsOverScales(std::vector<std::unique_ptr<validRegression>> &validRegressions, Matrix &Ylog);

    double calcMse(const Matrix &yhat, const Matrix &y) const;

    std::pair<Matrix, double> jacobianMatrix_PeakArea(const Matrix &B, int scale) const;

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