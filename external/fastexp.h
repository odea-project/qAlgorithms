#ifndef EXP_TABLE_H
#define EXP_TABLE_H

#include <cmath>
#include "exptables.h" // @todo these could be a constexpr function

// @todo also consider this method: http://spfrnd.de/posts/2018-03-10-fast-exponential.html

// function taken from here: https://github.com/nadavrot/fast_log/blob/main/src/exp_approx.cc
double fast_exp_1(double x)
{
    double integer = trunc(x);
    // X is now the fractional part of the number.
    x = x - integer;

    // Use a 4-part polynomial to approximate exp(x);
    double c[] = {0.28033708, 0.425302, 1.01273643, 1.00020947};

    // Use Horner's method to evaluate the polynomial.
    double val = c[3] + x * (c[2] + x * (c[1] + x * (c[0])));
    return val * EXP_TABLE[(unsigned)integer + 710];
}

// Approximate the function \p exp in the range -0.004, 0.004.
// Q = fpminimax(exp(x), 5, [|D...|], [-0.0039, 0.0039])
double approximate_exp_pol_around_zero(const float x)
{
    return 1 +
           x * (1 + x * (0.49999999999985944576508245518198236823081970214844 +
                         x * (0.166666666666697105281258473041816614568233489990234 +
                              x * (4.1666696240209417922972789938285131938755512237549e-2 +
                                   x * 8.3333337622652735310335714302709675393998622894287e-3))));
}

// function taken from here: https://github.com/nadavrot/fast_log/blob/main/src/exp_accurate.cc
float fast_exp_2(float x)
{
    if (x >= 710)
    {
        return INFINITY;
    }
    else if (x <= -710)
    {
        return 0;
    }

    // Split X into 3 numbers such that: x = I1 + (I2 << 8) + xt;
    int Int1 = int(x);
    x = x - Int1;
    int Int2 = int(x * 256);
    x = x - (float(Int2) / 256);

    return approximate_exp_pol_around_zero(x) * EXP_TABLE[Int1 + 710] * EXP_TABLE_r256[Int2 + 256];
}

#endif
