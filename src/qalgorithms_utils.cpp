#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include <cstdint> // uint64_t
#include <cmath>   // std::abs()

namespace qAlgorithms
{
    double exp_approx_d(const double x)
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

    float exp_approx_f(const float x)
    {
        // Entsprechende Konstanten für float
        constexpr float LOG2E  = 1.4426950408889634f;
        constexpr float OFFSET = 126.94269504f; // Siehe Erklärung unten
        constexpr uint32_t EXP_OFFSET = 1UL << 23; // 23 Bit Mantisse bei float

        union
        {
            uint32_t i;
            float f;
        } v = { (uint32_t)((x * LOG2E + OFFSET) * EXP_OFFSET) };
        return v.f;
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