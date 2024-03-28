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

  void RefMatrix::assignRef(size_t row, size_t col, double& value)
  {
    elements[row * cols + col] = std::ref(value);
  }

  void RefMatrix::assignVal(size_t row, size_t col, double value)
  {
    elements[row * cols + col] = std::ref(value);
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