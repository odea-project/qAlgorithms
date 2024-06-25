// qalgorithms_matrix.cpp

#include "qalgorithms_matrix.h"

#include <cmath>

namespace q
{
  namespace Matrices //@todo check if the less extensive solution also works
  {
    // constructor and destructor
    Matrix::Matrix() : rows(0), cols(0){};

    Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols)
    {
      elements = new double[rows * cols]();
    }

    Matrix::Matrix(const Matrix &other) : rows(other.rows), cols(other.cols)
    {
      elements = new double[rows * cols];
      std::copy(other.elements, other.elements + (rows * cols), elements);
    }

    Matrix::~Matrix()
    {
      delete[] elements;
    }

    // access
    double &
    Matrix::operator()(size_t row, size_t col)
    {
      return elements[row * cols + col];
    }

    const double &
    Matrix::operator()(size_t row, size_t col) const
    {
      return elements[row * cols + col];
    }

    double &
    Matrix::operator[](size_t idx) const
    {
      return elements[idx];
    }

    Matrix &
    Matrix::operator=(const Matrix &other)
    {
      if (this == &other)
        return *this;

      delete[] elements;

      rows = other.rows;
      cols = other.cols;
      elements = new double[rows * cols]; // @todo is this deleted after function terminates?
      std::copy(other.elements, other.elements + (rows * cols), elements);

      return *this;
    }

#pragma region "Matrix_mc"
    Matrix_mc::Matrix_mc(const size_t rows, const size_t cols) : rows(rows), cols(cols)
    {
      elements = new double[rows * cols]();
    }

    Matrix_mc::~Matrix_mc()
    {
      delete[] elements;
    }

    Matrix_mc::Matrix_mc(const Matrix_mc &other) : rows(other.rows), cols(other.cols)
    {
      rows = other.rows;
      cols = other.cols;
      elements = new double[rows * cols];
      std::copy(other.elements, other.elements + (rows * cols), elements);
    }

    double &
    Matrix_mc::operator()(size_t row, size_t col)
    {
      return elements[col * rows + row];
    }

    const double &
    Matrix_mc::operator()(size_t row, size_t col) const
    {
      return elements[col * rows + row];
    }

    double &
    Matrix_mc::operator[](size_t idx)
    {
      return elements[idx];
    }

    const double &
    Matrix_mc::operator[](size_t idx) const
    {
      return elements[idx];
    }

    Matrix_mc &
    Matrix_mc::operator=(const Matrix_mc &other)
    {
      if (this == &other)
        return *this;

      delete[] elements;

      rows = other.rows;
      cols = other.cols;
      elements = new double[rows * cols];
      std::copy(other.elements, other.elements + (rows * cols), elements);

      return *this;
    }
#pragma endregion "Matrix_mc"

#pragma region "Matrix_mc_4x4"
    Matrix_mc_4x4::Matrix_mc_4x4()
    {
      std::fill_n(elements, 16, 0.0);
    }

    Matrix_mc_4x4::~Matrix_mc_4x4()
    {
      // empty because elements is a stack array
    }

    Matrix_mc_4x4::Matrix_mc_4x4(const Matrix_mc_4x4 &other)
    {
      std::copy(other.elements, other.elements + 16, elements);
    }

    double &
    Matrix_mc_4x4::operator()(size_t row, size_t col)
    {
      return elements[col * 4 + row];
    }

    const double &
    Matrix_mc_4x4::operator()(size_t row, size_t col) const
    {
      return elements[col * 4 + row];
    }

    double &
    Matrix_mc_4x4::operator[](size_t idx)
    {
      return elements[idx];
    }

    const double &
    Matrix_mc_4x4::operator[](size_t idx) const
    {
      return elements[idx];
    }

    Matrix_mc_4x4 &
    Matrix_mc_4x4::operator=(const Matrix_mc_4x4 &other)
    {
      if (this == &other)
        return *this;

      std::copy(other.elements, other.elements + 16, elements);

      return *this;
    }

    double *
    Matrix_mc_4x4::begin()
    {
      return elements;
    }

    const double *
    Matrix_mc_4x4::begin() const
    {
      return elements;
    }

    double *
    Matrix_mc_4x4::end()
    {
      return elements + 16;
    }

    const double *
    Matrix_mc_4x4::end() const
    {
      return elements + 16;
    }

#pragma endregion "Matrix_mc_4x4"

#pragma region "Vector"
    Vector::Vector(const size_t n) : n(n)
    {
      elements = new double[n]();
    }

    Vector::~Vector()
    {
      delete[] elements;
    }

    Vector::Vector(const Vector &other) : n(other.n)
    {
      elements = new double[n];
      std::copy(other.elements, other.elements + n, elements);
    }

    double &
    Vector::operator[](size_t idx)
    {
      return elements[idx];
    }

    const double &
    Vector::operator[](size_t idx) const
    {
      return elements[idx];
    }

    double *
    Vector::begin()
    {
      return elements;
    }

    const double *
    Vector::begin() const
    {
      return elements;
    }

    double *
    Vector::end()
    {
      return elements + n;
    }

    const double *
    Vector::end() const
    {
      return elements + n;
    }

    Vector &
    Vector::operator=(const Vector &other)
    {
      if (this == &other)
        return *this;

      delete[] elements;

      const size_t n = other.n;
      elements = new double[n];
      std::copy(other.elements, other.elements + n, elements);

      return *this;
    }
#pragma endregion "Vector"

#pragma region "BoolVector" // @todo why is this not using std::vector or a normal array?
    BoolVector::BoolVector(const size_t n) : n(n)
    {
      elements = new bool[n]();
    }

    BoolVector::~BoolVector()
    {
      delete[] elements;
    }

    BoolVector::BoolVector(const BoolVector &other) : n(other.n)
    {
      elements = new bool[n];
      std::copy(other.elements, other.elements + n, elements);
    }

    bool &
    BoolVector::operator[](size_t idx)
    {
      return elements[idx];
    }

    const bool &
    BoolVector::operator[](size_t idx) const
    {
      return elements[idx];
    }

    BoolVector &
    BoolVector::operator=(const BoolVector &other)
    {
      if (this == &other)
        return *this;

      delete[] elements;

      const size_t n = other.n;
      elements = new bool[n];
      std::copy(other.elements, other.elements + n, elements);

      return *this;
    }

    bool *
    BoolVector::begin()
    {
      return elements;
    }

    const bool *
    BoolVector::begin() const
    {
      return elements;
    }

    bool *
    BoolVector::end()
    {
      return elements + n;
    }

    const bool *
    BoolVector::end() const
    {
      return elements + n;
    }
#pragma endregion "BoolVector"
  } // namespace Matrices
} // namespace q
