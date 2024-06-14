// qalgorithms_matrix.h
#ifndef QALGORITHMS_MATRIX_H // Include guarde to prevent double inclusion
#define QALGORITHMS_MATRIX_H

// external
#include <iostream>
#include <cmath>
#include <algorithm>
#include <array>
#include <cstdint>

/* This file includes the q::matrix class*/
namespace q
{
  namespace Matrices
  {
  /**
   * @brief A class to store a matrix
   * @details The Matrix class is used to store a matrix of doubles. 
   *
   * @param elements A pointer to the elements of the matrix
   * @param rows The number of rows
   * @param cols The number of columns
   */
  class Matrix
  {
  public:
    double *elements;
    size_t rows, cols;
    // Constructors and Destructor
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix &other);
    ~Matrix();

    double &
    operator()(size_t row, size_t col);

    const double &
    operator()(size_t row, size_t col) const;

    double &
    operator[](size_t idx) const;

    Matrix &
    operator=(const Matrix &other);
  };

  /**
   * @brief A clase to store a matrix with major column order storage
   * @details The Matrix_mc class is used to store a matrix of doubles with major column order storage.
   * 
   * @param elements A pointer to the elements of the matrix
   * @param rows The number of rows
   * @param cols The number of columns
   */
  class Matrix_mc
  {
  public:
    double *elements;
    size_t rows, cols;

    Matrix_mc(const size_t rows, const size_t cols);
    ~Matrix_mc();

    Matrix_mc(const Matrix_mc &other);

    double &
    operator()(size_t row, size_t col);

    const double &
    operator()(size_t row, size_t col) const;

    double &
    operator[](size_t idx);

    const double &
    operator[](size_t idx) const;

    Matrix_mc &
    operator=(const Matrix_mc &other);
  };


  /**
   * @brief A class to store a 4x4 matrix
   * @details The Matrix_mc_4x4 class is used to store a 4x4 matrix of doubles with major column order storage.
   * 
   * @param elements A pointer to the elements of the matrix
   */
  class Matrix_mc_4x4
  {
    public:
    double elements[16];

    Matrix_mc_4x4();
    ~Matrix_mc_4x4();

    Matrix_mc_4x4(const Matrix_mc_4x4 &other);

    double &
    operator()(size_t row, size_t col);

    const double &
    operator()(size_t row, size_t col) const;

    double &
    operator[](size_t idx);

    const double &
    operator[](size_t idx) const;

    Matrix_mc_4x4 &
    operator=(const Matrix_mc_4x4 &other);

    double*
    begin();

    const double*
    begin() const;

    double*
    end();

    const double*
    end() const;  
  };


  /**
   * @brief A class to store a Vector
   * @details The Vector class is used to store a vector of doubles.
   * 
   * @param elements A pointer to the elements of the vector
   * @param n The number of elements in the vector
   */
  class Vector
  {
  public:
    double *elements;
    size_t n;

    Vector(const size_t n);
    ~Vector();

    Vector(const Vector& other);

    double &
    operator[](size_t idx);

    const double &
    operator[](size_t idx) const;

    double*
    begin();

    const double*
    begin() const;

    double*
    end();

    const double*
    end() const;

    Vector&
    operator=(const Vector &other);
  };

  /**
   * @brief A class to store a vector of booleans
   * @details The BoolVector class is used to store a vector of booleans.
   * 
   * @param elements A pointer to the elements of the vector
   * @param n The number of elements in the vector
   */
  class BoolVector
  {
  public:
    bool *elements;
    size_t n;

    BoolVector(const size_t n);
    ~BoolVector();

    BoolVector(const BoolVector& other);

    bool &
    operator[](size_t idx);

    const bool &
    operator[](size_t idx) const;

    BoolVector&
    operator=(const BoolVector &other);

    bool*
    begin();

    const bool*
    begin() const;

    bool*
    end();

    const bool*
    end() const;
  };
  } // namespace Matrices
}
#endif // QALGORITHMS_MATRIX_H