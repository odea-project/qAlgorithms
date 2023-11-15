// qalgorithms_matrix.cpp

#include "../include/qalgorithms_matrix.h"

namespace q {
  // constructor
  Matrix::Matrix() : rows(0), cols(0) {};
  Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    elements = new double[rows * cols]();
  }
  Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    elements = new double[rows * cols];
    std::copy(other.elements, other.elements + (rows * cols), elements);
  }

  void Matrix::reinitialize(size_t newRows, size_t newCols) {
    // Free the old memory
    delete[] elements;
    
    // Allocate memory for the new matrix
    elements = new double[newRows * newCols];
    rows = newRows;
    cols = newCols;
  }

  // clear object
  Matrix::~Matrix() {
    delete[] elements;
  }

  // get dimensions
  size_t Matrix::numRows() const {
    return rows;
  }

  size_t Matrix::numCols() const {
    return cols;
  }

  size_t Matrix::numel() const {
    return cols * rows;
  }

  // access
  double& Matrix::operator()(size_t row, size_t col) {
    return elements[row * cols + col];
  }

  const double& Matrix::operator()(size_t row, size_t col) const {
    return elements[row * cols + col];
  }

  double& Matrix::getElement(size_t idx) const {
    return elements[idx];
  }

  // get row
  Matrix Matrix::row(size_t rowIndex) const {
    Matrix rowMatrix(1,cols);
    for (size_t i = 0; i < cols; ++i) {
        rowMatrix(0,i) = elements[rowIndex * cols + i];
    }
    return rowMatrix;
  }

  // get col
  Matrix Matrix::col(size_t colIndex) const {
    Matrix colMatrix(rows, 1);
    for (size_t i = 0; i < rows; ++i) {
        colMatrix(i,0) = elements[i * cols + colIndex];
    }
    return colMatrix;
  } 

  // get sub-Matrix
  Matrix Matrix::subMatrix(
    size_t startRow, 
    size_t endRow, 
    size_t startCol, 
    size_t endCol) const {
      if (startRow >= rows || startCol >= cols || endRow > rows || endCol > cols || startRow > endRow || startCol > endCol) {
              throw std::out_of_range("Indices are out of range");
          }
          
          Matrix subMat(endRow - startRow, endCol - startCol);
          
          for (size_t i = startRow; i < endRow; ++i) {
              for (size_t j = startCol; j < endCol; ++j) {
                  subMat(i - startRow, j - startCol) = (*this)(i, j);
              }
          }
          
      return subMat;
  }

  // Transpose function
  Matrix Matrix::T() const {
      size_t rows = numRows();
      size_t cols = numCols();
      Matrix transposed(cols, rows);
      for (size_t i = 0; i < rows; i++) {
          for (size_t j = 0; j < cols; j++) {
              transposed(j, i) = operator()(i, j);
          }
      }
      return transposed;
  }

  // Matrix multiplication
  Matrix Matrix::operator*(const Matrix& other) const {
      size_t rows1 = numRows();
      size_t cols1 = numCols();
      size_t rows2 = other.numRows();
      size_t cols2 = other.numCols();

      if (cols1 != rows2) {
          throw std::invalid_argument("Matrix dimensions do not allow multiplication");
      }

      Matrix result(rows1, cols2);
      for (size_t i = 0; i < rows1; i++) {
          for (size_t j = 0; j < cols2; j++) {
              for (size_t k = 0; k < cols1; k++) {
                  result(i, j) += operator()(i, k) * other(k,j);
              }
          }
      }
      return result;
  }

  Matrix Matrix::operator*(const double scalar) const {
    Matrix result(numRows(),numCols());
    for (size_t i = 0; i < rows*cols; i++) {
      result.elements[i] = elements[i] * scalar;
    }
    return result;
  }

  // Matrix subtraction
  Matrix Matrix::operator-(const Matrix& other) const {
    size_t rows1 = numRows();
    size_t cols1 = numCols();
    size_t rows2 = other.numRows();
    size_t cols2 = other.numCols();

    if (cols1 != cols2) {
        std::cout << cols1 << " <-//-> " << cols2 << std::endl;
        throw std::invalid_argument("Matrix dimensions (col) do not allow subtraction");
    }
    if (rows1 != rows2) {
        std::cout << rows1 << " <-//-> " << rows2 << std::endl;
        throw std::invalid_argument("Matrix dimensions (row) do not allow subtraction");
    }

    Matrix result(rows1, cols1);
    for (size_t i = 0; i < result.numel(); i++) {
      result.elements[i] = elements[i] - other.elements[i];
    }
    return result;
  }

  // Matrix addition
  Matrix Matrix::operator+(const Matrix& other) const {
    size_t rows1 = numRows();
    size_t cols1 = numCols();
    size_t rows2 = other.numRows();
    size_t cols2 = other.numCols();

    if (cols1 != cols2) {
        std::cout << cols1 << " <-//-> " << cols2 << std::endl;
        throw std::invalid_argument("Matrix dimensions (col) do not allow additon");
    }
    if (rows1 != rows2) {
        std::cout << rows1 << " <-//-> " << rows2 << std::endl;
        throw std::invalid_argument("Matrix dimensions (row) do not allow additon");
    }

    Matrix result(rows1, cols1);
    for (size_t i = 0; i < result.numel(); i++) {
      result.elements[i] = elements[i] + other.elements[i];
    }
    return result;
  }

  // Matrix power
  Matrix Matrix::operator^(const double power) const {
    Matrix result(numRows(),numCols());
    for (size_t i = 0; i < rows*cols; i++) {
      result.elements[i] = std::pow(elements[i],power);
    }
    return result;
  }

  Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) return *this;

    delete[] elements;

    rows = other.rows;
    cols = other.cols;
    elements = new double[rows * cols];
    std::copy(other.elements, other.elements + (rows * cols), elements);

    return *this;
  }

  // log of Matrix elements
  Matrix Matrix::log() const {
    Matrix result(rows,cols);
    for (size_t i = 0; i < cols*rows; i++) {
      result.elements[i] = std::log(elements[i]);
    }
    return result;
  }

  // Matrix Sum of all elements
  double Matrix::sumElements() const {
    double sum = 0.0;
    for (size_t i=0; i < rows*cols; i++) {
        sum += elements[i];
    }
    return sum;
  }

  //Matrix inverse (symmetric case)
  Matrix Matrix::inv() const {
    Matrix L = choleskyDecomposition();
    Matrix L_inv = L.inverseLowerTriangle();
    Matrix L_inv_T = L_inv.T();

    Matrix inv_matrix = L_inv_T * L_inv;

    return inv_matrix;
  }

  // calculate the Cholesky Decomposition Matrix L
  Matrix Matrix::choleskyDecomposition() const {
    // get size of the matrix
    size_t n = numRows();
    // create cholesky Matrix
    Matrix L(n,n);
    // do calculation
    for (size_t i = 0; i < n; i++)
    {
      for (size_t j = 0; j <= i; j++)
      {
        double sum = 0;
        for (size_t k = 0; k < j; k++)
        {
          sum += L(i, k) * L(j, k);
        }
        if (i == j)
        {
          L(i, j) = std::sqrt(operator()(i,i) - sum);
        } else
        {
          L(i, j) = (operator()(i, j) - sum) / L(j, j);
        }
      }
    }
    return L;
  }

  Matrix Matrix::inverseLowerTriangle() const {
    size_t n = numRows();
    Matrix inv_L(n, n);

    for (size_t i = 0; i < n; ++i) {
        inv_L(i, i) = 1 / operator()(i,i);
        for (size_t j = 0; j < i; ++j) {
            double s = 0.0;
            for (size_t k = j; k < i; ++k) {
                s += operator()(i,k) * inv_L(k,j);
            }
            inv_L(i, j) = -s / operator()(i, i);
        }
    }

    return inv_L;
  }

  Matrix Matrix::convolveSymmetric(const Matrix& kernel) const {
    size_t n = numRows();
    size_t k = kernel.numel();
    size_t n_segments = n - k + 1;
    size_t center_point = k/2;

    Matrix conv(1,n_segments);
    // calculation from left to center (excluding center)
    for (size_t i = 0; i < center_point; i++) {
      double* products = new double[n-2*i];
      int u = 0;
      for (size_t j = i; j < (n-i); j++) {
        products[u] = kernel(0,i) * operator()(j, 0);
        u++; 
      }
      for (size_t j = 0; j < n_segments; j++) {
        conv(0, j) += products[j];
        conv(0, j) += products[k-1-2*i+j];
      }
    }
    // calculation of the center terms
    for (size_t j = 0; j < n_segments; j++) {
      conv(0, j) += kernel(0,center_point) * operator()(center_point+j, 0);
    }
    return conv;
  }

  Matrix Matrix::convolveRotation(const Matrix& kernel) const {
    size_t n = numRows();
    size_t k = kernel.numel();
    size_t n_segments = n - k + 1;
    size_t center_point = k/2;

    Matrix conv(1,n_segments);
    // calculation from left to center (excluding center)
    for (size_t i = 0; i < center_point; i++) {
      double* products = new double[n-2*i];
      int u = 0;
      for (size_t j = i; j < (n-i); j++) {
        products[u] = kernel(0,i) * operator()(j, 0);
        u++; 
      }
      for (size_t j = 0; j < n_segments; j++) {
        conv(0, j) += products[j];
        conv(0, j) -= products[k-1-2*i+j];
      }
    }
    return conv;
  }

  Matrix Matrix::convolveAntisymmetric(const Matrix& kernel) const {
    size_t n =numRows();
    size_t k = kernel.numel();
    size_t n_segments = n - k + 1;
    size_t center_point = k/2 + 1;

    Matrix conv(2,n_segments);
    // calculation from left to center (including center)
    for (size_t i = 0; i < center_point; i++) {
      double* products = new double[n-2*i];
      int u = 0;
      for (size_t j = i; j < (n-i); j++) {
        products[u] = kernel(0,i) * operator()(j, 0);
        u++; 
      }
      for (size_t j = 0; j < n_segments; j++) {
        conv(0, j) += products[j];
        conv(1, j) += products[k-1-2*i+j];
      }
      delete[] products; // kill object
    }
    // calculation from center + 1  to right edge
    int u = 1;
    for (size_t i = center_point; i < k; i++)
    {
      int s = k - i - 1;
      double* products = new double[n-2*k + 2*i+2];
      int v = 0;
      for (size_t j = s; j < (n-s); j++) {
        products[v] = kernel(0,i) * operator()(j, 0);
        v++; 
      }
      for (size_t j = 0; j < n_segments; j++) {
        conv(0, j) += products[2*u+j];
        conv(1, j) += products[j];
      }
      u++;
      delete[] products; // kill object
    }
    

    return conv;
  }

  // Sorting by first column
  void Matrix::sort1() {
    size_t* indices = new size_t[rows];
    for (size_t i = 0; i < rows; ++i) {
        indices[i] = i;
    }

    // Sort the indices based on the first column values
    std::sort(indices, indices + rows, [this](size_t a, size_t b) {
    return this->operator()(a, 0) < this->operator()(b, 0);
    });

    // Create a new elements vector and rearrange rows based on sorted indices
    double* new_elements = new double[rows * cols];
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            new_elements[i * cols + j] = this->operator()(indices[i], j);
        }
    }

    // Replace the old elements vector with the new one
    delete[] elements;
    elements = new_elements;
    delete[] indices;
    }

  // debuging
  void Matrix::print() const {
      for (size_t i = 0; i < numRows(); ++i) {
        for (size_t j = 0; j < numCols(); ++j) {
            std::cout << operator()(i, j) << " ";
        }
        std::cout << std::endl;
    }
  }

}
