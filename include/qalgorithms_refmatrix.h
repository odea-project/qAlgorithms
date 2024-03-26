// qalgorithms_refmatrix.h
#ifndef QALGORITHMS_REFMATRIX_H
#define QALGORITHMS_REFMATRIX_H
//
// internal
#include "../include/qalgorithms_matrix.h"

// external
#include <functional>


namespace q
{
  class RefMatrix
  {
  private:
    std::vector<std::reference_wrapper<double>> elements;
    size_t rows, cols;

  public:
    // Constructors
    RefMatrix();
    RefMatrix(size_t rows, size_t cols);

    // Destructor
    ~RefMatrix();

    // methods
    size_t getRows() const;
    size_t getCols() const;
    // access
    double &operator()(size_t row, size_t col);
    double operator()(size_t row, size_t col) const;
    // assign new reference matrix using row and col and the value
    void assignRef(size_t row, size_t col, double& value);
    void assignVal(size_t row, size_t col, double value);

    // debugging
    void print() const;
  };

} // namespace q

#endif // QALGORITHMS_REFMATRIX_H