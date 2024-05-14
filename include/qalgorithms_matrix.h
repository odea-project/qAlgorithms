// qalgorithms_matrix.h
#ifndef QALGORITHMS_MATRIX_H // Include guarde to prevent double inclusion
#define QALGORITHMS_MATRIX_H

// internal


// external
#include <iostream>
#include <cmath>
#include <algorithm>
#include <array>

/* This file includes the q::matrix class*/
namespace q
{
  /**
   * @brief A class to store a matrix
   * @details The Matrix class is used to store a matrix of doubles. The class includes methods for matrix operations such as addition, subtraction, multiplication, and inversion.
   *
   * @param elements A pointer to the elements of the matrix
   * @param rows The number of rows
   * @param cols The number of columns
   */
  class Matrix
  {
  private:
    double *elements;
    size_t rows, cols;

    // Methods for inv()
    Matrix choleskyDecomposition() const;
    Matrix inverseLowerTriangle() const;

    double fasterpow2(double p) const;
    double fasterexp(double p) const;

  public:
    // Constructors and member functions
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix &other);
    void reinitialize(size_t newRows, size_t newCols);
    // Clear Object
    ~Matrix();
    // get dimensions
    size_t numRows() const;
    size_t numCols() const;
    size_t numel() const;
    // access
    double &operator()(size_t row, size_t col);
    const double &operator()(size_t row, size_t col) const;
    double &getElement(size_t idx) const;

    Matrix row(size_t rowIndex) const;
    Matrix col(size_t colIndex) const;

    Matrix subMatrix(
        size_t startRow,
        size_t endRow,
        size_t startCol,
        size_t endCol) const;

    // functions
    Matrix T() const;
    Matrix operator*(const Matrix &other) const;
    Matrix operator*(const double scalar) const;
    Matrix operator-(const Matrix &other) const;
    Matrix operator+(const Matrix &other) const;
    Matrix operator^(const double power) const;
    Matrix &operator=(const Matrix &other);
    void assign(size_t row, size_t col, double& value);
    Matrix log() const;
    Matrix exp() const;

    double sumElements() const;
    Matrix inv() const;

    Matrix convolveSymmetric(const Matrix &kernel) const;
    Matrix convolveRotation(const Matrix &kernel) const;
    Matrix convolveAntisymmetric(const Matrix &kernel) const;
    Matrix convoleCombiend(const Matrix &kernel) const;
    void sort1(); // sorting by the first column
    void print() const;
  };
}
#endif // QALGORITHMS_MATRIX_H