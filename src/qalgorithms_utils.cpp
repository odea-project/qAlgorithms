// qalgorithms_utils.cpp
#include "qalgorithms_utils.h"
#include "qalgorithms_matrix.h"

#include <algorithm> // std::transform
#include <cstdint>   // uint64_t
#include <cmath>
#include <stdexcept> // std::invalid_argument
#include <iostream>
#include <numeric> // std::inner_product
#include <array>

#include <vector>
// #include <map>
// #include <cstdio>
#include <memory> // std::unique_ptr
#include <string>

namespace q
{

  std::array<double, 3> calcQuadraticCoefficients(double x1, double x2, double x3, double y1, double y2, double y3)
  {
    // calculate regression coefficients for y a * x^2 + b * x + c
    // pulling the division in a variable saves one instruction
    double divisor_bc = 1 / ((x1 - x2) * (x1 - x3) * (x2 - x3));
    double a = (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / ((x1 - x2) * (x1 - x3) * (x3 - x2));
    double b = (x1 * x1 * (y2 - y3) + x2 * x2 * (y3 - y1) + x3 * x3 * (y1 - y2)) * divisor_bc;
    double c = (x1 * x1 * (x2 * y3 - x3 * y2) + x1 * (x3 * x3 * y2 - x2 * x2 * y3) + x2 * x3 * y1 * (x2 - x3)) * divisor_bc;
    // returns the regression coefficients
    return {a, b, c};
  }

  double exp_approx(const double x)
  {
    constexpr double LOG2E = 1.44269504088896340736;
    constexpr double OFFSET = 1022.9329329329329;
    constexpr uint64_t EXP_OFFSET = 1LL << 52; // @todo why does this need 8 byte specifically?
    union
    {
      uint64_t i;
      double d;
    } v = {(uint64_t)((x * LOG2E + OFFSET) * EXP_OFFSET)};
    return v.d;
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

    return D * std::exp(x * x) / SQRTPI_2; // @todo: define M_2_SQRTPI in utils
  }

  const double tValuesArray[500] = {6.313751515, 2.91998558, 2.353363435, 2.131846781, 2.015048373, 1.94318028, 1.894578605, 1.859548038, 1.833112933, 1.812461123, 1.795884819, 1.782287556, 1.770933396, 1.761310136, 1.753050356, 1.745883676, 1.739606726, 1.734063607, 1.729132812, 1.724718243, 1.720742903, 1.717144374, 1.713871528, 1.71088208, 1.708140761, 1.70561792, 1.703288446, 1.701130934, 1.699127027, 1.697260894, 1.695518789, 1.693888742, 1.692360304, 1.690924251, 1.689572454, 1.688297711, 1.687093617, 1.685954458, 1.684875119, 1.683851011, 1.682878, 1.681952356, 1.681070702, 1.680229975, 1.679427392, 1.678660413, 1.677926721, 1.677224195, 1.676550892, 1.675905025, 1.67528495, 1.674689153, 1.674116236, 1.673564906, 1.673033965, 1.672522303, 1.672028888, 1.671552762, 1.671093032, 1.670648865, 1.670219484, 1.669804162, 1.669402222, 1.669013025, 1.668635976, 1.668270514, 1.667916114, 1.667572281, 1.667238549, 1.666914479, 1.666599658, 1.666293696, 1.665996224, 1.665706893, 1.665425373, 1.665151353, 1.664884537, 1.664624644, 1.664371409, 1.664124579, 1.663883913, 1.663649184, 1.663420175, 1.663196679, 1.6629785, 1.662765449, 1.662557349, 1.662354029, 1.662155326, 1.661961084, 1.661771155, 1.661585397, 1.661403674, 1.661225855, 1.661051817, 1.66088144, 1.66071461, 1.660551217, 1.660391156, 1.660234326, 1.66008063, 1.659929976, 1.659782273, 1.659637437, 1.659495383, 1.659356034, 1.659219312, 1.659085144, 1.658953458, 1.658824187, 1.658697265, 1.658572629, 1.658450216, 1.658329969, 1.65821183, 1.658095744, 1.657981659, 1.657869522, 1.657759285, 1.657650899, 1.657544319, 1.657439499, 1.657336397, 1.65723497, 1.657135178, 1.657036982, 1.656940344, 1.656845226, 1.656751594, 1.656659413, 1.656568649, 1.65647927, 1.656391244, 1.656304542, 1.656219133, 1.656134988, 1.65605208, 1.655970382, 1.655889868, 1.655810511, 1.655732287, 1.655655173, 1.655579143, 1.655504177, 1.655430251, 1.655357345, 1.655285437, 1.655214506, 1.655144534, 1.6550755, 1.655007387, 1.654940175, 1.654873847, 1.654808385, 1.654743774, 1.654679996, 1.654617035, 1.654554875, 1.654493503, 1.654432901, 1.654373057, 1.654313957, 1.654255585, 1.654197929, 1.654140976, 1.654084713, 1.654029128, 1.653974208, 1.653919942, 1.653866317, 1.653813324, 1.653760949, 1.653709184, 1.653658017, 1.653607437, 1.653557435, 1.653508002, 1.653459126, 1.6534108, 1.653363013, 1.653315758, 1.653269024, 1.653222803, 1.653177088, 1.653131869, 1.653087138, 1.653042889, 1.652999113, 1.652955802, 1.652912949, 1.652870547, 1.652828589, 1.652787068, 1.652745977, 1.65270531, 1.652665059, 1.652625219, 1.652585784, 1.652546746, 1.652508101, 1.652469842, 1.652431964, 1.65239446, 1.652357326, 1.652320556, 1.652284144, 1.652248086, 1.652212376, 1.652177009, 1.652141981, 1.652107286, 1.65207292, 1.652038878, 1.652005156, 1.651971748, 1.651938651, 1.651905861, 1.651873373, 1.651841182, 1.651809286, 1.651777679, 1.651746359, 1.65171532, 1.65168456, 1.651654074, 1.651623859, 1.651593912, 1.651564228, 1.651534805, 1.651505638, 1.651476725, 1.651448062, 1.651419647, 1.651391475, 1.651363544, 1.65133585, 1.651308391, 1.651281164, 1.651254165, 1.651227393, 1.651200843, 1.651174514, 1.651148402, 1.651122505, 1.65109682, 1.651071345, 1.651046077, 1.651021013, 1.650996152, 1.65097149, 1.650947025, 1.650922755, 1.650898678, 1.650874791, 1.650851092, 1.650827579, 1.65080425, 1.650781102, 1.650758134, 1.650735342, 1.650712727, 1.650690284, 1.650668012, 1.65064591, 1.650623976, 1.650602207, 1.650580601, 1.650559157, 1.650537873, 1.650516748, 1.650495779, 1.650474964, 1.650454303, 1.650433793, 1.650413433, 1.65039322, 1.650373154, 1.650353233, 1.650333455, 1.650313819, 1.650294323, 1.650274966, 1.650255746, 1.650236662, 1.650217713, 1.650198896, 1.650180211, 1.650161656, 1.650143229, 1.650124931, 1.650106758, 1.650088711, 1.650070786, 1.650052985, 1.650035304, 1.650017743, 1.650000301, 1.649982976, 1.649965767, 1.649948674, 1.649931694, 1.649914828, 1.649898073, 1.649881428, 1.649864893, 1.649848466, 1.649832147, 1.649815934, 1.649799826, 1.649783823, 1.649767922, 1.649752124, 1.649736428, 1.649720831, 1.649705334, 1.649689935, 1.649674634, 1.649659429, 1.649644319, 1.649629305, 1.649614384, 1.649599556, 1.64958482, 1.649570176, 1.649555622, 1.649541157, 1.649526781, 1.649512493, 1.649498293, 1.649484178, 1.649470149, 1.649456205, 1.649442344, 1.649428567, 1.649414873, 1.64940126, 1.649387728, 1.649374276, 1.649360905, 1.649347611, 1.649334397, 1.649321259, 1.649308199, 1.649295214, 1.649282305, 1.649269471, 1.649256711, 1.649244024, 1.649231411, 1.64921887, 1.6492064, 1.649194001, 1.649181673, 1.649169415, 1.649157226, 1.649145105, 1.649133053, 1.649121068, 1.64910915, 1.649097298, 1.649085513, 1.649073792, 1.649062137, 1.649050545, 1.649039017, 1.649027553, 1.649016151, 1.649004811, 1.648993533, 1.648982315, 1.648971159, 1.648960062, 1.648949026, 1.648938048, 1.648927129, 1.648916269, 1.648905466, 1.64889472, 1.648884031, 1.648873399, 1.648862822, 1.648852302, 1.648841836, 1.648831425, 1.648821068, 1.648810764, 1.648800515, 1.648790318, 1.648780173, 1.648770081, 1.648760041, 1.648750052, 1.648740114, 1.648730226, 1.648720389, 1.648710601, 1.648700863, 1.648691174, 1.648681534, 1.648671941, 1.648662397, 1.648652901, 1.648643451, 1.648634049, 1.648624693, 1.648615383, 1.64860612, 1.648596901, 1.648587728, 1.6485786, 1.648569516, 1.648560477, 1.648551481, 1.648542529, 1.64853362, 1.648524754, 1.64851593, 1.648507149, 1.64849841, 1.648489713, 1.648481057, 1.648472442, 1.648463868, 1.648455335, 1.648446842, 1.648438388, 1.648429975, 1.648421601, 1.648413266, 1.648404969, 1.648396712, 1.648388493, 1.648380311, 1.648372168, 1.648364062, 1.648355993, 1.648347961, 1.648339967, 1.648332008, 1.648324086, 1.6483162, 1.648308349, 1.648300534, 1.648292755, 1.64828501, 1.648277301, 1.648269625, 1.648261984, 1.648254378, 1.648246805, 1.648239266, 1.64823176, 1.648224287, 1.648216847, 1.648209441, 1.648202066, 1.648194724, 1.648187415, 1.648180137, 1.64817289, 1.648165676, 1.648158492, 1.64815134, 1.648144219, 1.648137128, 1.648130068, 1.648123038, 1.648116038, 1.648109068, 1.648102128, 1.648095217, 1.648088336, 1.648081483, 1.64807466, 1.648067866, 1.6480611, 1.648054362, 1.648047653, 1.648040972, 1.648034319, 1.648027693, 1.648021096, 1.648014525, 1.648007982, 1.648001465, 1.647994976, 1.647988513, 1.647982077, 1.647975667, 1.647969283, 1.647962926, 1.647956594, 1.647950288, 1.647944008, 1.647937753, 1.647931523, 1.647925318, 1.647919139, 1.647912984, 1.647906854};

  const double chiSquareArray[500] = {3.84146, 5.99146, 7.81473, 9.48773, 11.07050, 12.59159, 14.06714, 15.50731, 16.91898, 18.30704, 19.67514, 21.02607, 22.36203, 23.68479, 24.99579, 26.29623, 27.58711, 28.86930, 30.14353, 31.41043, 32.67057, 33.92444, 35.17246, 36.41503, 37.65248, 38.88514, 40.11327, 41.33714, 42.55697, 43.77297, 44.98534, 46.19426, 47.39988, 48.60237, 49.80185, 50.99846, 52.19232, 53.38354, 54.57223, 55.75848, 56.94239, 58.12404, 59.30351, 60.48089, 61.65623, 62.82962, 64.00111, 65.17077, 66.33865, 67.50481, 68.66929, 69.83216, 70.99345, 72.15322, 73.31149, 74.46832, 75.62375, 76.77780, 77.93052, 79.08194, 80.23210, 81.38102, 82.52873, 83.67526, 84.82065, 85.96491, 87.10807, 88.25016, 89.39121, 90.53123, 91.67024, 92.80827, 93.94534, 95.08147, 96.21667, 97.35097, 98.48438, 99.61693, 100.74862, 101.87947, 103.00951, 104.13874, 105.26718, 106.39484, 107.52174, 108.64789, 109.77331, 110.89800, 112.02199, 113.14527, 114.26787, 115.38979, 116.51105, 117.63165, 118.75161, 119.87094, 120.98964, 122.10773, 123.22522, 124.34211, 125.45842, 126.57415, 127.68931, 128.80391, 129.91796, 131.03146, 132.14442, 133.25686, 134.36878, 135.48018, 136.59107, 137.70146, 138.81136, 139.92077, 141.02970, 142.13816, 143.24615, 144.35367, 145.46074, 146.56736, 147.67353, 148.77926, 149.88456, 150.98943, 152.09388, 153.19790, 154.30152, 155.40472, 156.50752, 157.60992, 158.71193, 159.81355, 160.91478, 162.01563, 163.11610, 164.21620, 165.31593, 166.41530, 167.51430, 168.61295, 169.71125, 170.80920, 171.90680, 173.00406, 174.10098, 175.19757, 176.29382, 177.38975, 178.48535, 179.58063, 180.67560, 181.77025, 182.86458, 183.95861, 185.05233, 186.14575, 187.23887, 188.33169, 189.42422, 190.51646, 191.60840, 192.70007, 193.79144, 194.88254, 195.97336, 197.06391, 198.15418, 199.24418, 200.33391, 201.42337, 202.51258, 203.60152, 204.69020, 205.77863, 206.86680, 207.95472, 209.04239, 210.12981, 211.21698, 212.30391, 213.39060, 214.47705, 215.56326, 216.64924, 217.73498, 218.82049, 219.90577, 220.99082, 222.07565, 223.16025, 224.24462, 225.32878, 226.41272, 227.49644, 228.57994, 229.66323, 230.74630, 231.82917, 232.91182, 233.99427, 235.07651, 236.15855, 237.24038, 238.32201, 239.40344, 240.48467, 241.56571, 242.64654, 243.72719, 244.80764, 245.88790, 246.96797, 248.04784, 249.12754, 250.20704, 251.28636, 252.36550, 253.44445, 254.52322, 255.60182, 256.68023, 257.75847, 258.83653, 259.91441, 260.99212, 262.06966, 263.14702, 264.22422, 265.30124, 266.37810, 267.45479, 268.53131, 269.60767, 270.68387, 271.75990, 272.83577, 273.91148, 274.98703, 276.06242, 277.13765, 278.21272, 279.28764, 280.36241, 281.43702, 282.51148, 283.58578, 284.65994, 285.73394, 286.80779, 287.88150, 288.95506, 290.02847, 291.10174, 292.17486, 293.24784, 294.32067, 295.39336, 296.46591, 297.53832, 298.61059, 299.68272, 300.75471, 301.82656, 302.89828, 303.96986, 305.04131, 306.11262, 307.18380, 308.25485, 309.32576, 310.39654, 311.46719, 312.53771, 313.60810, 314.67837, 315.74850, 316.81851, 317.88839, 318.95815, 320.02778, 321.09729, 322.16667, 323.23593, 324.30507, 325.37408, 326.44297, 327.51175, 328.58040, 329.64894, 330.71735, 331.78565, 332.85383, 333.92189, 334.98984, 336.05767, 337.12539, 338.19299, 339.26048, 340.32785, 341.39511, 342.46226, 343.52930, 344.59623, 345.66304, 346.72975, 347.79635, 348.86283, 349.92921, 350.99549, 352.06165, 353.12771, 354.19366, 355.25950, 356.32524, 357.39088, 358.45641, 359.52184, 360.58716, 361.65238, 362.71750, 363.78252, 364.84744, 365.91225, 366.97697, 368.04158, 369.10610, 370.17051, 371.23483, 372.29905, 373.36317, 374.42720, 375.49113, 376.55496, 377.61869, 378.68233, 379.74588, 380.80933, 381.87268, 382.93595, 383.99912, 385.06219, 386.12518, 387.18807, 388.25087, 389.31357, 390.37619, 391.43872, 392.50116, 393.56350, 394.62576, 395.68793, 396.75001, 397.81200, 398.87390, 399.93572, 400.99745, 402.05909, 403.12065, 404.18212, 405.24350, 406.30480, 407.36602, 408.42714, 409.48819, 410.54915, 411.61003, 412.67082, 413.73154, 414.79216, 415.85271, 416.91318, 417.97356, 419.03386, 420.09408, 421.15422, 422.21428, 423.27426, 424.33417, 425.39399, 426.45373, 427.51339, 428.57298, 429.63249, 430.69192, 431.75127, 432.81055, 433.86974, 434.92887, 435.98791, 437.04688, 438.10578, 439.16460, 440.22334, 441.28201, 442.34060, 443.39912, 444.45757, 445.51594, 446.57424, 447.63247, 448.69062, 449.74870, 450.80671, 451.86465, 452.92251, 453.98031, 455.03803, 456.09568, 457.15326, 458.21077, 459.26821, 460.32558, 461.38288, 462.44011, 463.49727, 464.55436, 465.61139, 466.66834, 467.72523, 468.78205, 469.83880, 470.89549, 471.95211, 473.00866, 474.06514, 475.12156, 476.17791, 477.23419, 478.29041, 479.34656, 480.40265, 481.45868, 482.51463, 483.57053, 484.62636, 485.68212, 486.73782, 487.79346, 488.84903, 489.90454, 490.95999, 492.01538, 493.07070, 494.12596, 495.18115, 496.23629, 497.29136, 498.34637, 499.40132, 500.45621, 501.51104, 502.56580, 503.62051, 504.67516, 505.72974, 506.78427, 507.83873, 508.89314, 509.94749, 511.00177, 512.05600, 513.11017, 514.16428, 515.21834, 516.27233, 517.32627, 518.38014, 519.43396, 520.48773, 521.54143, 522.59508, 523.64867, 524.70221, 525.75569, 526.80911, 527.86247, 528.91578, 529.96903, 531.02223, 532.07537, 533.12846, 534.18149, 535.23447, 536.28739, 537.34026, 538.39307, 539.44583, 540.49853, 541.55118, 542.60378, 543.65632, 544.70881, 545.76124, 546.81362, 547.86595, 548.91823, 549.97045, 551.02262, 552.07474, 553.12681};

  const double SQRTPI_2 = std::sqrt(M_PI) / 2;

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

    std::array<double, 3> test = calcQuadraticCoefficients(xData[0], xData[2], xData[1], yData[0], yData[2], yData[1]);
    double a = std::abs(coefficients.elements[1] - test[1]);
    if ((a > 0.0001) & (xData.size() == 3))
    {
      std::cout << a << "\n";
    }

    return coefficients;
  }

#pragma region "Matrix Operations" // why are matrix operations not defined in matrix.h?
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

  double
  multiplyVecMatrixVecTranspose(
      const double vec[4],
      const q::Matrices::Matrix_mc_4x4 &A)
  {
    double result = 0.0;
    for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
      {
        result += vec[i] * A(i, j) * vec[j];
      }
    }
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

  q::Matrices::Matrix_mc
  convolve(
      const q::Matrices::Vector &vec,
      const size_t n,
      const q::Matrices::Matrix &kernel)
  {
    size_t k = kernel.cols;
    size_t n_segments = n - k + 1;
    size_t centerpoint = k / 2;

    q::Matrices::Matrix_mc result(4, n_segments); // @changed std::vector<std::array<double, 3>> products(n); rewrite to n*3 vector
    std::vector<double> products(n * 3);

    // calculation from left to center (excluding center)
    for (size_t i = 0; i < centerpoint; i++)
    {
      int u = 0;
      for (size_t j = i; j < (n - i); j++)
      {
        products[u * 1] = vec[j] * kernel(0, i);
        products[u * 2] = vec[j] * kernel(1, i);
        products[u * 3] = vec[j] * kernel(2, i);
        u++;
      }
      for (size_t j = 0; j < n_segments; j++)
      {
        result(0, j) += products[j * 1] + products[(k - 1 - 2 * i + j) * 1];
        result(1, j) += products[j * 2] - products[(k - 1 - 2 * i + j) * 2];
        result(2, j) += products[j * 3];
        result(3, j) += products[(k - 1 - 2 * i + j) * 3];
      }
    }

    // calculation of the center terms
    for (size_t i = 0; i < n_segments; i++)
    {
      result(0, i) += vec[i + centerpoint] * kernel(0, centerpoint);
      result(2, i) += vec[i + centerpoint] * kernel(2, centerpoint);
      result(3, i) += vec[i + centerpoint] * kernel(3, centerpoint);
    }

    // calculation from center to right (excluding center)
    int u = 1;
    for (size_t i = centerpoint + 1; i < k; i++)
    {
      int s = k - i - 1;
      int v = 0;
      for (size_t j = s; j < (n - s); j++)
      {
        products[v * 3] = vec[j] * kernel(2, i);
        v++;
      }
      for (size_t j = 0; j < n_segments; j++)
      {
        result(2, j) += products[(2 * u + j) * 3];
        result(3, j) += products[j * 3];
      }
      u++;
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
  logn(
      const q::Matrices::Vector &vec)
  {
    q::Matrices::Vector result(vec.n);
    std::transform(
        vec.begin(),    // start of vec
        vec.end(),      // end of vec
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
}
