// qalgorithms_refmatrix.cpp

// internal
#include "../include/qalgorithms_refmatrix.h"

namespace q
{
  // Constructors
  RefMatrix::RefMatrix(){}

  RefMatrix::RefMatrix(size_t rows, size_t cols) : rows(rows), cols(cols)
  {
    double* values = new double[rows * cols]();
    elements.reserve(rows * cols);
    for (size_t i = 0; i < rows * cols; i++)
    {
      elements[i] = std::ref(values[i]);
    }
    delete[] values;
  }

  // Destructor
  RefMatrix::~RefMatrix()
  {
    elements.clear();
  }

  // methods
  size_t RefMatrix::getRows() const
  {
    return rows;
  }

  size_t RefMatrix::getCols() const
  {
    return cols;
  }

  // access
  double &RefMatrix::operator()(size_t row, size_t col)
  {
    return elements[row * cols + col];
  }

  double RefMatrix::operator()(size_t row, size_t col) const
  {
    return elements[row * cols + col];
  }

  double &RefMatrix::getElement(size_t idx) const
  {
    return elements[idx];
  }

  void RefMatrix::assignRef(size_t row, size_t col, double& value)
  {
    elements[row * cols + col] = std::ref(value);
  }

  void RefMatrix::assignVal(size_t row, size_t col, double value)
  {
    elements[row * cols + col] = std::ref(value);
  }

  // submatrix
  RefMatrix RefMatrix::subMatrix(size_t row_start, size_t row_end, size_t col_start, size_t col_end) const
  {
    RefMatrix result(row_end - row_start, col_end - col_start);
    for (size_t i = row_start; i < row_end; i++)
    {
      for (size_t j = col_start; j < col_end; j++)
      {
        result.assignRef(i - row_start, j - col_start, elements[i * cols + j]);
      }
    }
    return result;
  }

  // matrix operations
  Matrix RefMatrix::log() const
  {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; i++)
    {
      for (size_t j = 0; j < cols; j++)
      {
        result(i, j) = std::log(elements[i * cols + j]);
      }
    }
    return result;
  }

  // debugging
  void RefMatrix::print() const
  {
    for (size_t i = 0; i < rows; i++)
    {
      for (size_t j = 0; j < cols; j++)
      {
        std::cout << elements[i * cols + j] << " ";
      }
      std::cout << std::endl;
    }
  }
} // namespace q