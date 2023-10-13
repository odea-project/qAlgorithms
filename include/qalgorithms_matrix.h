// qalgorithms_matrix.h
#ifndef QALGORITHMS_MATRIX_H // Include guarde to prevent double inclusion
#define QALGORITHMS_MATRIX_H

#include <iostream>
#include <cmath>
#include <algorithm>

/* This file includes the q::matrix class*/
namespace q {
  class Matrix {
  private:
    double *elements;
    size_t rows, cols;

    // Methods for inv()
    Matrix choleskyDecomposition() const;
    Matrix inverseLowerTriangle() const;

  public:
    // Constructors and member functions
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& other);
    // Clear Object
    ~Matrix();
    // get dimensions
    size_t numRows() const;
    size_t numCols() const;
    size_t numel() const;
    // access
    double& operator()(size_t row, size_t col);
    const double& operator()(size_t row, size_t col) const;
    double& getElement(size_t idx) const;

    Matrix row(size_t rowIndex) const;
    Matrix col(size_t colIndex) const;
    
    Matrix subMatrix(
      size_t startRow, 
      size_t endRow, 
      size_t startCol, 
      size_t endCol) const;

    // functions
    Matrix T() const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const double scalar) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator+(const Matrix& other) const;
    Matrix operator^(const double power) const;
    Matrix& operator=(const Matrix& other);
    Matrix log() const;
    
    double sumElements() const;
    Matrix inv() const;
    
    Matrix convolveSymmetric(const Matrix& kernel) const;
    Matrix convolveRotation(const Matrix& kernel) const;
    Matrix convolveAntisymmetric(const Matrix& kernel) const;
    void sort1(); // sorting by the first column
    void print() const;
  };
}
#endif // QALGORITHMS_MATRIX_H