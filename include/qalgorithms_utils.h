// qalgorithms_utils.h
#ifndef QALGORITHMS_UTILS_H // Include guarde to prevent double inclusion
#define QALGORITHMS_UTILS_H

// internal
#include "qalgorithms_matrix.h"

// external
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <cstdint>
#include <numeric>

namespace q
{

    int sum(const std::vector<int> &vec);
    size_t sum(const std::vector<size_t> &vec);
    double sum(const std::vector<double> &vec);
    int sum(const bool *vec, size_t n);

    /**
     * @brief Fast exponential approximation base on Bit Manipulation.
     * @details This function is a fast approximation of the exponential function. It is based on the bit manipulation of the double value. The function is not as accurate as the standard exponential function but is much faster. It is based on the following repository: https://github.com/edrosten/fasterexp
     *
     * @param x : value between 0 and 26
     * @return double
     */
    double exp_approx(const double x);

    template <typename T>
    std::vector<bool> operator<(
        const std::vector<T> &vec,
        T scalar);

    template <typename T>
    std::vector<bool> operator>(
        const std::vector<T> &vec,
        T scalar);

    template <typename T>
    std::vector<T> operator*(
        const std::vector<T> &A,
        const std::vector<T> &B);

    std::vector<bool> operator&&(
        const std::vector<bool> &A,
        const std::vector<bool> &B);

    std::vector<bool> operator!(const std::vector<bool> &A);

    void operator|=(
        std::vector<bool> &A,
        const std::vector<bool> &B);

    double erfi(const double x);

    const Matrix linreg(
        const std::vector<double> &xData,
        const std::vector<double> &yData,
        const int degree);

    extern const double tValuesArray[500];
    extern const double chiSquareArray[500];
    extern const double SQRTPI_2;

#pragma region "Matrix Operations"
    /**
     * @brief Overloaded operator for matrix addition
     * @details This function overloads the + operator to allow for matrix addition. The function takes two matrices as input and returns a new matrix that is the element-wise sum of the two input matrices.
     *
     * @param A : Matrix
     * @param B : Matrix
     * @return Matrix
     */
    Matrix
    operator+(
        const Matrix &A,
        const Matrix &B);

    /**
     * @brief Overloaded operator for matrix multiplication
     * @details This function overloads the * operator to allow for matrix multiplication. The function takes two matrices as input and returns a new matrix that is the product of the two input matrices. The function uses the standard matrix multiplication algorithm, i.e., the dot product of the rows of the first matrix with the columns of the second matrix.
     *
     * @param A : Matrix
     * @param B : Matrix
     * @return Matrix
     */
    Matrix
    operator*(
        const Matrix &A,
        const Matrix &B);

    /**
     * @brief Overloaded operator for matrix multiplication by a vector
     *
     * @param A : Matrix
     * @param B : Vector
     * @return Vector
     */
    Vector
    operator*(
        const Matrix &A,
        const Vector &B);

    /**
     * @brief Overloaded operator for matrix multiplication by a scalar
     * @details This function overloads the * operator to allow for matrix multiplication by a scalar. The function takes a matrix and a scalar as input and returns a new matrix that is the product of the input matrix and the scalar. The function multiplies each element of the matrix by the scalar.
     *
     * @param A : Matrix
     * @param scalar : double
     * @return Matrix
     */
    Matrix
    operator*(
        const Matrix &A,
        const double scalar);

    /**
     * @brief Overloaded operator for matrix multiplication by a scalar
     * @details This function overloads the * operator to allow for matrix multiplication by a scalar. The function takes a scalar and a matrix as input and returns a new matrix that is the product of the scalar and the input matrix. The function multiplies each element of the matrix by the scalar.
     *
     * @param scalar : double
     * @param A : Matrix
     * @return Matrix
     */
    Matrix
    operator*(
        const double scalar,
        const Matrix &A);

    Matrix_mc_4x4
    multiplyScalarTo4x4Matrix(
        const double scalar,
        const Matrix &A);
    
    double 
    multiplyVecMatrixVecTranspose(
        const double vec[4],
        const Matrix_mc_4x4 &A);

    /**
     * @brief Overloaded operator for matrix division by a scalar
     * @details This function overloads the / operator to allow for matrix division by a scalar. The function takes a matrix and a scalar as input and returns a new matrix that is the result of dividing the input matrix by the scalar. The function divides each element of the matrix by the scalar.
     *
     * @param A : Matrix
     * @param scalar : double
     */
    Matrix
    operator/(
        const Matrix &A,
        const double scalar);

    /**
     * @brief Transpose a matrix
     * @details This function takes a matrix as input and returns a new matrix that is the transpose of the input matrix. The transpose of a matrix is obtained by swapping the rows and columns of the matrix.
     *
     * @param A : Matrix
     * @return Matrix
     */
    Matrix
    transpose(
        const Matrix &A);

    /**
     * @brief inverse of a matrix
     * @details This function takes a matrix as input and returns a new matrix that is the inverse of the input matrix. The inverse of a matrix is a matrix that, when multiplied by the original matrix, gives the identity matrix. The function uses the Cholesky decomposition method to calculate the inverse of the input matrix.
     *
     * @param A : Matrix
     * @return Matrix
     */
    Matrix
    inv(
        const Matrix &A);

    /**
     * @brief Cholesky decomposition of a matrix
     * @details This function takes a matrix as input and returns a new matrix that is the Cholesky decomposition of the input matrix. The Cholesky decomposition of a matrix is a lower triangular matrix L such that A = LL^T, where A is the input matrix. The function uses the Cholesky decomposition algorithm to calculate the Cholesky decomposition of the input matrix.
     *
     * @param A : Matrix
     * @return Matrix
     */
    Matrix
    choleskyDecomposition(
        const Matrix &A);

    /**
     * @brief Inverse of a lower triangular matrix
     * @details This function takes a lower triangular matrix as input and returns a new matrix that is the inverse of the input matrix. The inverse of a lower triangular matrix is also a lower triangular matrix. The function uses the inverse of a lower triangular matrix algorithm to calculate the inverse of the input matrix.
     *
     * @param A : Matrix
     * @return Matrix
     */
    Matrix
    inverseLowerTriangle(
        const Matrix &A);

    /**
     * @brief Convolution of a vector with a kernel matrix
     * @details This function takes a vector and a kernel matrix as input and returns a new matrix that is the result of convolving the input vector with the kernel matrix.
     *
     * @param vec : array of doubles* stored in the vector class
     * @param n : size of the array
     * @param kernel : Matrix
     * @return Matrix
     */
    Matrix_mc
    convolve(
        const Vector &vec,
        const size_t n,
        const Matrix &kernel);

    /**
     * @brief Calculate the dot product of the design matrix and the beta vector, i.e., X * beta(:, idx)
     *
     * @param X : design matrix
     * @param beta : beta matrix
     * @param idx : index of the column of beta to use
     * @return Vector
     */
    Vector
    calcYhat(
        const Matrix &X,
        const Matrix_mc &beta,
        const size_t idx);

    Vector
    calcYhatExtended(
        const Matrix &X,
        const Vector &beta,
        const size_t x_row_start = 0,
        const size_t x_row_end = 0);

    /**
     * @brief Extract a column from a matrix
     *
     * @param A : Matrix
     * @param col : column index
     * @return Vector
     */
    Vector
    extractCol(
        const Matrix_mc &A,
        const size_t col);

#pragma endregion "Matrix Operations"

#pragma region "Vector Operations"
    /**
     * @brief calculate the log of a vector
     * @details This function takes a vector as input and returns a new vector that is the element-wise natural logarithm of the input vector.
     *
     * @param vec : vector of doubles
     * @return Vector
     */
    Vector
    logn(
        const Vector &vec);

#pragma endregion "Vector Operations"

#pragma region "print functions"
    /**
     * @brief Print a matrix
     * @details This function takes a matrix as input and prints the elements of the matrix to the console.
     *
     * @param A : Matrix
     */
    void
    print(
        const Matrix &A);

    void
    print(
        const Matrix_mc &A);

    void
    print(
        const Vector &A);
} // namespace q
#endif // QALGORITHMS_UTILS_H