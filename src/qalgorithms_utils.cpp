// qalgorithms_utils.cpp
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <assert.h>

namespace qAlgorithms
{
  int sum(const std::vector<int> &vec)
  {
    double sum = 0.0;
    for (const auto &elem : vec)
    {
      sum += elem;
    }
    return sum;
  }

  size_t
  sum(const std::vector<size_t> &vec)
  {
    double sum = 0.0;
    for (const auto &elem : vec)
    {
      sum += elem;
    }
    return sum;
  }

  double
  sum(const std::vector<double> &vec)
  {
    double sum = 0.0;
    for (const auto &elem : vec)
    {
      sum += elem;
    }
    return sum;
  }

  int sum(const bool *vec, size_t n)
  {
    int sum = 0;
    for (size_t i = 0; i < n; i++)
    {
      sum += vec[i];
    }
    return sum;
  }

  float sum8(const __m256 &vec) // why does this break when inlined?
  {
    // float sum = 0;
    // for (size_t i = 0; i < 8; i++)
    // {
    //   sum += vec[i];
    // }
    float sum = vec[0] + vec[1] + vec[3] + vec[4] + vec[5] + vec[6] + vec[7];
    return sum;

    // const __m128 hiQuad = _mm256_extractf128_ps(vec, 1);     // hiQuad = ( x7, x6, x5, x4 )
    // const __m128 loQuad = _mm256_castps256_ps128(vec);       // loQuad = ( x3, x2, x1, x0 )
    // const __m128 sumQuad = _mm_add_ps(loQuad, hiQuad);       // sumQuad = ( x3 + x7, x2 + x6, x1 + x5, x0 + x4 )
    // const __m128 loDual = sumQuad;                           // loDual = ( -, -, x1 + x5, x0 + x4 )
    // const __m128 hiDual = _mm_movehl_ps(sumQuad, sumQuad);   // hiDual = ( -, -, x3 + x7, x2 + x6 )
    // const __m128 sumDual = _mm_add_ps(loDual, hiDual);       // sumDual = ( -, -, x1 + x3 + x5 + x7, x0 + x2 + x4 + x6 )
    // const __m128 lo = sumDual;                               // lo = ( -, -, -, x0 + x2 + x4 + x6 )
    // const __m128 hi = _mm_shuffle_ps(sumDual, sumDual, 0x1); // hi = ( -, -, -, x1 + x3 + x5 + x7 )
    // const __m128 sum2 = _mm_add_ss(lo, hi);                   // sum = ( -, -, -, x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7 )
    // // return _mm_cvtss_f32(sum);
    // std::cout << sum << ", " << (_mm_cvtss_f32(sum2)) << "\n";
    // std::cout.flush();
  }

  double
  exp_approx_d(const double x)
  {
    constexpr double LOG2E = 1.44269504088896340736;
    constexpr double OFFSET = 1022.9329329329329;
    constexpr uint64_t EXP_OFFSET = 1LL << 52;
    union
    {
      uint64_t i;
      double d;
    } v = {(uint64_t)((x * LOG2E + OFFSET) * EXP_OFFSET)};
    return v.d;
  }

  __m256 exp_approx_vf(const __m256 x)
  {
    constexpr float LOG2E = 1.44269504f;
    constexpr float OFFSET = 126.94269504f;
    constexpr uint32_t EXP_OFFSET = 1 << 23;

    // calc (x * LOG2E + OFFSET)
    __m256 t = _mm256_fmadd_ps(x, _mm256_set1_ps(LOG2E), _mm256_set1_ps(OFFSET));

    // multiply with EXP_OFFSET
    t = _mm256_mul_ps(t, _mm256_set1_ps(static_cast<float>(EXP_OFFSET)));

    // convert to integer and shift right by 23 bits to get the result
    __m256i vi = _mm256_cvttps_epi32(t);
    __m256 result = _mm256_castsi256_ps(vi);

    return result;
  }

  float erf_approx_f(const float x)
  {
    float sign = x < 0 ? -1.0f : 1.0f; // get sign as the approximation is only valid for positive x
    float t = std::abs(x);             // get the absolute value of x
    constexpr float a1 = 0.278393f;    // empirically determined
    constexpr float a2 = 0.230389f;    // empirically determined
    constexpr float a3 = 0.000972f;    // empirically determined
    constexpr float a4 = 0.078108f;    // empirically determined
    t = 1.0f + t * (a1 + t * (a2 + t * (a3 + t * a4)));
    t = t * t * t * t;               // t^4
    return sign * (1.0f - 1.0f / t); // return the final approximation
  }

  double dawson5(double x)
  {
    double y, p, q;
    y = x * x;
    p = 1.0 + y * (0.1049934947 + y * (0.0424060604 + y * (0.0072644182 + y * (0.0005064034 + y * (0.0001789971)))));
    q = 1.0 + y * (0.7715471019 + y * (0.2909738639 + y * (0.0694555761 + y * (0.0140005442 + y * (0.0008327945 + 2 * 0.0001789971 * y)))));
    return x * (p / q);
  }

  double experfc(double x, double sign)
  {
    constexpr double a = 0.978795604954049; // empirically determined
    constexpr double b = 1.25731022692317;  // empirically determined
    double t = -x * x;
    return SQRTPI_2 * exp_approx_d(t) + sign * a * x * exp_approx_d(t * b);
  }

  template <typename T>
  std::vector<bool> operator<(const std::vector<T> &vec, T scalar)
  {
    std::vector<bool> result(vec.size());
    int u = 0;
    for (const auto &elem : vec)
    {
      result[u] = elem < scalar;
      u++;
    }
    return result;
  }

  template <typename T>
  std::vector<bool> operator>(
      const std::vector<T> &vec,
      T scalar)
  {
    std::vector<bool> result(vec.size());
    int u = 0;
    for (const auto &elem : vec)
    {
      result[u] = elem > scalar;
      u++;
    }
    return result;
  }

  template <typename T>
  std::vector<T> operator*(
      const std::vector<T> &A,
      const std::vector<T> &B)
  {
    std::vector<T> product(A.size());
    for (size_t i = 0; i < A.size(); i++)
    {
      product[i] = A[i] * B[i];
    }
    return product;
  }

  std::vector<bool> operator&&(
      const std::vector<bool> &A,
      const std::vector<bool> &B)
  {
    std::vector<bool> result(A.size());
    for (size_t i = 0; i < A.size(); i++)
    {
      result[i] = A[i] && B[i];
    }
    return result;
  }

  std::vector<bool> operator!(const std::vector<bool> &A)
  {
    std::vector<bool> result(A.size());
    for (size_t i = 0; i < A.size(); i++)
    {
      result[i] = !A[i];
    }
    return result;
  }

  void operator|=(
      std::vector<bool> &A,
      const std::vector<bool> &B)
  {
    for (size_t i = 0; i < A.size(); i++)
    {
      bool tempA = A[i];
      bool tempB = B[i];
      tempA |= tempB;
      A[i] = tempA;
    }
  }

  double erfi(const double x)
  {
    /* This function uses the Dawson Integral, i.e.
    erfi(x) = 2 * Dawson * exp(x^2) / sqrt(pi)

    The Dawson Integral is calculated by Stan Sykora's rational function approximations. URL : http://www.ebyte.it/library/codesnippets/DawsonIntegralApproximations.html (Dawson5())
    */
    // calculate the Dawson Integral:
    double y, p, q;
    y = x * x;
    p = 1.0 + y * (0.1049934947 + y * (0.0424060604 + y * (0.0072644182 + y * (0.0005064034 + y * (0.0001789971)))));
    q = 1.0 + y * (0.7715471019 + y * (0.2909738639 + y * (0.0694555761 + y * (0.0140005442 + y * (0.0008327945 + 2 * 0.0001789971 * y)))));
    double D = x * (p / q);

    return D * exp_approx_d(x * x);
  }
}