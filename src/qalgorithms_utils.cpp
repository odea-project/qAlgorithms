// qalgorithms_utils.cpp
#include "../include/qalgorithms_utils.h"

namespace q
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

  float
  sum8(const __m256 &vec)
  {
    const __m128 hiQuad = _mm256_extractf128_ps(vec, 1);     // hiQuad = ( x7, x6, x5, x4 )
    const __m128 loQuad = _mm256_castps256_ps128(vec);       // loQuad = ( x3, x2, x1, x0 )
    const __m128 sumQuad = _mm_add_ps(loQuad, hiQuad);       // sumQuad = ( x3 + x7, x2 + x6, x1 + x5, x0 + x4 )
    const __m128 loDual = sumQuad;                           // loDual = ( -, -, x1 + x5, x0 + x4 )
    const __m128 hiDual = _mm_movehl_ps(sumQuad, sumQuad);   // hiDual = ( -, -, x3 + x7, x2 + x6 )
    const __m128 sumDual = _mm_add_ps(loDual, hiDual);       // sumDual = ( -, -, x1 + x3 + x5 + x7, x0 + x2 + x4 + x6 )
    const __m128 lo = sumDual;                               // lo = ( -, -, -, x0 + x2 + x4 + x6 )
    const __m128 hi = _mm_shuffle_ps(sumDual, sumDual, 0x1); // hi = ( -, -, -, x1 + x3 + x5 + x7 )
    const __m128 sum = _mm_add_ss(lo, hi);                   // sum = ( -, -, -, x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7 )
    return _mm_cvtss_f32(sum);
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
      result[i] = A[i] & B[i];
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

    return D * exp_approx_d(x * x); /// SQRTPI_2; // @todo: define M_2_SQRTPI in utils
  }

  constexpr float tValuesArray[256] = {6.313751515, 2.91998558, 2.353363435, 2.131846781, 2.015048373, 1.94318028, 1.894578605, 1.859548038, 1.833112933, 1.812461123, 1.795884819, 1.782287556, 1.770933396, 1.761310136, 1.753050356, 1.745883676, 1.739606726, 1.734063607, 1.729132812, 1.724718243, 1.720742903, 1.717144374, 1.713871528, 1.71088208, 1.708140761, 1.70561792, 1.703288446, 1.701130934, 1.699127027, 1.697260894, 1.695518789, 1.693888742, 1.692360304, 1.690924251, 1.689572454, 1.688297711, 1.687093617, 1.685954458, 1.684875119, 1.683851011, 1.682878, 1.681952356, 1.681070702, 1.680229975, 1.679427392, 1.678660413, 1.677926721, 1.677224195, 1.676550892, 1.675905025, 1.67528495, 1.674689153, 1.674116236, 1.673564906, 1.673033965, 1.672522303, 1.672028888, 1.671552762, 1.671093032, 1.670648865, 1.670219484, 1.669804162, 1.669402222, 1.669013025, 1.668635976, 1.668270514, 1.667916114, 1.667572281, 1.667238549, 1.666914479, 1.666599658, 1.666293696, 1.665996224, 1.665706893, 1.665425373, 1.665151353, 1.664884537, 1.664624644, 1.664371409, 1.664124579, 1.663883913, 1.663649184, 1.663420175, 1.663196679, 1.6629785, 1.662765449, 1.662557349, 1.662354029, 1.662155326, 1.661961084, 1.661771155, 1.661585397, 1.661403674, 1.661225855, 1.661051817, 1.66088144, 1.66071461, 1.660551217, 1.660391156, 1.660234326, 1.66008063, 1.659929976, 1.659782273, 1.659637437, 1.659495383, 1.659356034, 1.659219312, 1.659085144, 1.658953458, 1.658824187, 1.658697265, 1.658572629, 1.658450216, 1.658329969, 1.65821183, 1.658095744, 1.657981659, 1.657869522, 1.657759285, 1.657650899, 1.657544319, 1.657439499, 1.657336397, 1.65723497, 1.657135178, 1.657036982, 1.656940344, 1.656845226, 1.656751594, 1.656659413, 1.656568649, 1.65647927, 1.656391244, 1.656304542, 1.656219133, 1.656134988, 1.65605208, 1.655970382, 1.655889868, 1.655810511, 1.655732287, 1.655655173, 1.655579143, 1.655504177, 1.655430251, 1.655357345, 1.655285437, 1.655214506, 1.655144534, 1.6550755, 1.655007387, 1.654940175, 1.654873847, 1.654808385, 1.654743774, 1.654679996, 1.654617035, 1.654554875, 1.654493503, 1.654432901, 1.654373057, 1.654313957, 1.654255585, 1.654197929, 1.654140976, 1.654084713, 1.654029128, 1.653974208, 1.653919942, 1.653866317, 1.653813324, 1.653760949, 1.653709184, 1.653658017, 1.653607437, 1.653557435, 1.653508002, 1.653459126, 1.6534108, 1.653363013, 1.653315758, 1.653269024, 1.653222803, 1.653177088, 1.653131869, 1.653087138, 1.653042889, 1.652999113, 1.652955802, 1.652912949, 1.652870547, 1.652828589, 1.652787068, 1.652745977, 1.65270531, 1.652665059, 1.652625219, 1.652585784, 1.652546746, 1.652508101, 1.652469842, 1.652431964, 1.65239446, 1.652357326, 1.652320556, 1.652284144, 1.652248086, 1.652212376, 1.652177009, 1.652141981, 1.652107286, 1.65207292, 1.652038878, 1.652005156, 1.651971748, 1.651938651, 1.651905861, 1.651873373, 1.651841182, 1.651809286, 1.651777679, 1.651746359, 1.65171532, 1.65168456, 1.651654074, 1.651623859, 1.651593912, 1.651564228, 1.651534805, 1.651505638, 1.651476725, 1.651448062, 1.651419647, 1.651391475, 1.651363544, 1.65133585, 1.651308391, 1.651281164, 1.651254165, 1.651227393, 1.651200843, 1.651174514, 1.651148402, 1.651122505, 1.65109682, 1.651071345, 1.651046077, 1.651021013, 1.650996152, 1.65097149, 1.650947025, 1.650922755, 1.650898678, 1.650874791, 1.650851092, 1.650827579};

  constexpr float chiSquareArray[256] = {3.84146, 5.99146, 7.81473, 9.48773, 11.0705, 12.59159, 14.06714, 15.50731, 16.91898, 18.30704, 19.67514, 21.02607, 22.36203, 23.68479, 24.99579, 26.29623, 27.58711, 28.8693, 30.14353, 31.41043, 32.67057, 33.92444, 35.17246, 36.41503, 37.65248, 38.88514, 40.11327, 41.33714, 42.55697, 43.77297, 44.98534, 46.19426, 47.39988, 48.60237, 49.80185, 50.99846, 52.19232, 53.38354, 54.57223, 55.75848, 56.94239, 58.12404, 59.30351, 60.48089, 61.65623, 62.82962, 64.00111, 65.17077, 66.33865, 67.50481, 68.66929, 69.83216, 70.99345, 72.15322, 73.31149, 74.46832, 75.62375, 76.7778, 77.93052, 79.08194, 80.2321, 81.38102, 82.52873, 83.67526, 84.82065, 85.96491, 87.10807, 88.25016, 89.39121, 90.53123, 91.67024, 92.80827, 93.94534, 95.08147, 96.21667, 97.35097, 98.48438, 99.61693, 100.74862, 101.87947, 103.00951, 104.13874, 105.26718, 106.39484, 107.52174, 108.64789, 109.77331, 110.898, 112.02199, 113.14527, 114.26787, 115.38979, 116.51105, 117.63165, 118.75161, 119.87094, 120.98964, 122.10773, 123.22522, 124.34211, 125.45842, 126.57415, 127.68931, 128.80391, 129.91796, 131.03146, 132.14442, 133.25686, 134.36878, 135.48018, 136.59107, 137.70146, 138.81136, 139.92077, 141.0297, 142.13816, 143.24615, 144.35367, 145.46074, 146.56736, 147.67353, 148.77926, 149.88456, 150.98943, 152.09388, 153.1979, 154.30152, 155.40472, 156.50752, 157.60992, 158.71193, 159.81355, 160.91478, 162.01563, 163.1161, 164.2162, 165.31593, 166.4153, 167.5143, 168.61295, 169.71125, 170.8092, 171.9068, 173.00406, 174.10098, 175.19757, 176.29382, 177.38975, 178.48535, 179.58063, 180.6756, 181.77025, 182.86458, 183.95861, 185.05233, 186.14575, 187.23887, 188.33169, 189.42422, 190.51646, 191.6084, 192.70007, 193.79144, 194.88254, 195.97336, 197.06391, 198.15418, 199.24418, 200.33391, 201.42337, 202.51258, 203.60152, 204.6902, 205.77863, 206.8668, 207.95472, 209.04239, 210.12981, 211.21698, 212.30391, 213.3906, 214.47705, 215.56326, 216.64924, 217.73498, 218.82049, 219.90577, 220.99082, 222.07565, 223.16025, 224.24462, 225.32878, 226.41272, 227.49644, 228.57994, 229.66323, 230.7463, 231.82917, 232.91182, 233.99427, 235.07651, 236.15855, 237.24038, 238.32201, 239.40344, 240.48467, 241.56571, 242.64654, 243.72719, 244.80764, 245.8879, 246.96797, 248.04784, 249.12754, 250.20704, 251.28636, 252.3655, 253.44445, 254.52322, 255.60182, 256.68023, 257.75847, 258.83653, 259.91441, 260.99212, 262.06966, 263.14702, 264.22422, 265.30124, 266.3781, 267.45479, 268.53131, 269.60767, 270.68387, 271.7599, 272.83577, 273.91148, 274.98703, 276.06242, 277.13765, 278.21272, 279.28764, 280.36241, 281.43702, 282.51148, 283.58578, 284.65994, 285.73394, 286.80779, 287.8815, 288.95506, 290.02847, 291.10174, 292.17486, 293.24784, 294.32067};

  const double SQRTPI_2 = std::sqrt(3.141592653589793) / 2;

  const q::Matrices::Matrix linreg(
      const std::vector<double> &xData,
      const std::vector<double> &yData,
      const size_t degree)
  {
    // Create the matrix X
    q::Matrices::Matrix X(xData.size(), degree + 1);
    for (size_t i = 0; i < xData.size(); i++)
    {
      for (size_t j = 0; j < degree + 1; j++)
      {
        X(i, j) = std::pow(xData[i], j);
      }
    }
    // Create the matrix Y
    q::Matrices::Matrix Y(yData.size(), 1);
    for (size_t i = 0; i < yData.size(); i++)
    {
      Y(i, 0) = yData[i];
    }
    // Calculate the coefficients
    q::Matrices::Matrix X_T = transpose(X);
    q::Matrices::Matrix X_T_X = X_T * X;
    q::Matrices::Matrix X_T_X_inv = inv(X_T_X);
    q::Matrices::Matrix X_T_Y = X_T * Y;
    q::Matrices::Matrix coefficients = X_T_X_inv * X_T_Y;
    return coefficients;
  }

#pragma region "Matrix Operations"
  bool
  operator==(
      const q::Matrices::Matrix_mc &A,
      const q::Matrices::Matrix_mc &B)
  {
    if (A.rows != B.rows || A.cols != B.cols)
      return false;

    for (size_t i = 0; i < A.rows * A.cols; i++)
    {
      if (A.elements[i] != B.elements[i])
        return false;
    }

    return true;
  }

  q::Matrices::Matrix
  operator+(
      const q::Matrices::Matrix &A,
      const q::Matrices::Matrix &B)
  {
    if (A.rows != B.rows || A.cols != B.cols)
      throw std::invalid_argument("Matrix dimensions must match.");

    q::Matrices::Matrix result(A.rows, A.cols);
    for (size_t i = 0; i < A.rows * A.cols; i++)
      result.elements[i] = A.elements[i] + B.elements[i];

    return result;
  }

  q::Matrices::Matrix
  operator*(
      const q::Matrices::Matrix &A,
      const q::Matrices::Matrix &B)
  {
    if (A.cols != B.rows)
      throw std::invalid_argument("Matrix dimensions must match.");

    q::Matrices::Matrix result(A.rows, B.cols);
    for (size_t i = 0; i < A.rows; i++)
    {
      for (size_t j = 0; j < B.cols; j++)
      {
        double sum = 0.0;
        for (size_t k = 0; k < A.cols; k++)
          sum += A(i, k) * B(k, j);
        result(i, j) = sum;
      }
    }

    return result;
  }

  q::Matrices::Vector
  operator*(
      const q::Matrices::Matrix &A,
      const q::Matrices::Vector &B)
  {
    if (A.cols != B.n)
      throw std::invalid_argument("Matrix dimensions must match.");

    q::Matrices::Vector result(A.rows);
    for (size_t i = 0; i < A.rows; i++)
    {
      result[i] = std::inner_product(
          A.elements + i * A.cols,       // start of the i-th row of A
          A.elements + (i + 1) * A.cols, // end of the i-th row of A
          B.begin(), 0.0);               // start of B
    }

    return result;
  }

  q::Matrices::Matrix
  operator*(
      const q::Matrices::Matrix &A,
      const double scalar)
  {
    q::Matrices::Matrix result(A.rows, A.cols);
    std::transform(
        A.elements,                   // start of A
        A.elements + A.rows * A.cols, // end of A
        result.elements,              // start of result
        [scalar](double a)
        {
          return a * scalar; // multiply each element of A by scalar
        });

    return result;
  }

  q::Matrices::Matrix
  operator*(
      const double scalar,
      const q::Matrices::Matrix &A)
  {
    return A * scalar;
  }

  q::Matrices::Matrix_mc_4x4
  multiplyScalarTo4x4Matrix(
      const double scalar,
      const q::Matrices::Matrix &A)
  {
    q::Matrices::Matrix_mc_4x4 result;
    std::transform(
        A.elements,      // start of A
        A.elements + 16, // end of A
        result.elements, // start of result
        [scalar](double a)
        {
          return a * scalar; // multiply each element of A by scalar
        });

    return result;
  }

  q::Matrices::Matrix
  operator/(
      const q::Matrices::Matrix &A,
      const double scalar)
  {
    return A * (1.0 / scalar);
  }

  q::Matrices::Matrix
  transpose(
      const q::Matrices::Matrix &A)
  {
    q::Matrices::Matrix result(A.cols, A.rows);
    for (size_t i = 0; i < A.rows; i++)
    {
      for (size_t j = 0; j < A.cols; j++)
        result(j, i) = A(i, j);
    }

    return result;
  }

  q::Matrices::Matrix
  inv(
      const q::Matrices::Matrix &A)
  {
    if (A.rows != A.cols)
      throw std::invalid_argument("Matrix must be square.");

    q::Matrices::Matrix L = choleskyDecomposition(A);
    q::Matrices::Matrix L_inv = inverseLowerTriangle(L); // @todo: combine transpose(L_inv) * L_inv into one function (inverseLowerTriangleTranspose)

    return transpose(L_inv) * L_inv;
  }

  q::Matrices::Matrix
  choleskyDecomposition(
      const q::Matrices::Matrix &A)
  {
    if (A.rows != A.cols)
      throw std::invalid_argument("Matrix must be square.");

    size_t n = A.rows;
    q::Matrices::Matrix L(n, n);

    for (size_t i = 0; i < n; i++)
    {
      for (size_t j = 0; j <= i; j++)
      {
        double sum = 0.0;
        for (size_t k = 0; k < j; k++)
          sum += L(i, k) * L(j, k);

        if (i == j)
          L(i, j) = std::sqrt(A(i, i) - sum);
        else
          L(i, j) = (1.0 / L(j, j) * (A(i, j) - sum));
      }
    }

    return L;
  }

  q::Matrices::Matrix
  inverseLowerTriangle(
      const q::Matrices::Matrix &A)
  {
    if (A.rows != A.cols)
      throw std::invalid_argument("Matrix must be square.");

    size_t n = A.rows;
    q::Matrices::Matrix result(n, n);

    for (size_t i = 0; i < n; i++)
    {
      result(i, i) = 1.0 / A(i, i);
      for (size_t j = 0; j < i; j++)
      {
        double sum = 0.0;
        for (size_t k = j; k < i; k++)
          sum += A(i, k) * result(k, j);
        result(i, j) = -sum / A(i, i);
      }
    }

    return result;
  }

  q::Matrices::Vector
  calcYhat(
      const q::Matrices::Matrix &X,
      const q::Matrices::Matrix_mc &beta,
      const size_t idx)
  {
    size_t n = X.rows;
    q::Matrices::Vector yhat(n);

    for (size_t i = 0; i < n; i++)
    {
      double sum = 0.0;
      for (size_t j = 0; j < X.cols; j++)
        sum += X(i, j) * beta(j, idx);
      yhat[i] = sum;
    }

    return yhat;
  }

  q::Matrices::Vector
  calcYhatExtended(
      const q::Matrices::Matrix &X,
      const q::Matrices::Vector &beta,
      const size_t x_row_start,
      const size_t x_row_end)
  {
    size_t n = x_row_end == 0 ? X.rows : x_row_end - x_row_start;
    q::Matrices::Vector yhat(n);

    for (size_t i = 0; i < n; i++)
    {
      double sum = 0.0;
      for (size_t j = 0; j < X.cols; j++)
        sum += X(i + x_row_start, j) * beta[j];
      yhat[i] = sum;
    }

    return yhat;
  }

  q::Matrices::Vector
  extractCol(
      const q::Matrices::Matrix_mc &A,
      const size_t col)
  {
    q::Matrices::Vector result(A.rows);
    std::copy(A.elements + col * A.rows, A.elements + (col + 1) * A.rows, result.begin());
    return result;
  }

#pragma endregion "Matrix Operations"

#pragma region "Vector Operations"

  q::Matrices::Vector
  logn(const float *vec_start, const float *vec_end)
  {
    q::Matrices::Vector result(vec_end - vec_start);
    std::transform(
        vec_start,      // start of vec
        vec_end,        // end of vec
        result.begin(), // start of result
        [](double x)
        {
          return std::log(x); // calculate the natural logarithm of each element of vec
        });

    return result;
  }

#pragma endregion "Vector Operations"

#pragma region "print functions"
  void
  print(
      const q::Matrices::Matrix &A)
  {
    for (size_t i = 0; i < A.rows; i++)
    {
      for (size_t j = 0; j < A.cols; j++)
        std::cout << A(i, j) << " ";
      std::cout << std::endl;
    }
  }

  void
  print(
      const q::Matrices::Matrix_mc &A)
  {
    for (size_t i = 0; i < A.rows; i++)
    {
      for (size_t j = 0; j < A.cols; j++)
        std::cout << A(i, j) << " ";
      std::cout << std::endl;
    }
  }

  void
  print(
      const q::Matrices::Vector &vec)
  {
    for (size_t i = 0; i < vec.n; i++)
      std::cout << vec[i] << " ";
    std::cout << std::endl;
  }
#pragma endregion "print functions"

#pragma region "execute commands"
  std::string
  exec(const char *cmd)
  {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
      throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
      result += buffer.data();
    }
    return result;
  }
#pragma endregion "execute commands"

} // namespace q