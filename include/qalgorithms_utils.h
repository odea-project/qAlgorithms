// qalgorithms_utils.h
#ifndef QALGORITHMS_UTILS_H // Include guarde to prevent double inclusion
#define QALGORITHMS_UTILS_H

#include <vector>
#include <iostream>
#include <cmath>
#include <map>

namespace q {
  
  int sum(const std::vector<int>& vec);
  size_t sum(const std::vector<size_t>& vec);
  double sum(const std::vector<double>& vec);

  int sum(const bool* vec, size_t n);

  template <typename T>
  std::vector<bool> operator<(
    const std::vector<T>& vec, 
    T scalar);

  template <typename T>
  std::vector<bool> operator>(
    const std::vector<T>& vec, 
    T scalar);

  template <typename T>
  std::vector<T> operator*(
    const std::vector<T>& A, 
    const std::vector<T>& B);

  std::vector<bool> operator&&(
    const std::vector<bool>& A, 
    const std::vector<bool>& B);

  std::vector<bool> operator!(const std::vector<bool>& A);

  void operator|=(
    std::vector<bool>& A, 
    const std::vector<bool>& B);
  
  double erfi(const double x);

  class tValues {
    private:
      std::map<int, double> values;
    
    public:
      tValues();
      const double& operator[](size_t degreeOfFreedom) const;
  };
}
#endif // QALGORITHMS_UTILS_H