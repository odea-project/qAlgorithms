// produce two vectors x and y from two vectors a and b, with a being an index
// and b a measure of signal intensity. For every gap in a, the corresponding
// y is estimated using quadratic fitting. x and y can be used for interpolating
// a gaussian fit where applicable

// ALL THE DATA SETS CONSIDERED HAVE AT LEAST 5 DATA POINTS WITH INTENSITY > 0 (due to filterSmallDataSets)
// extract the largest y value, which is used for thershoulding the extrapolations, i.e. MAX < 3 * max(y)

/* Data points at the edges will be extrapolated. Therefore, first and last non-zero data points and
maximum data point are log-transformed and considered for parabola definition. Edge data points
are then replaced by the extrapolated values from parabola and back transformed into linear
space using exp function.*/
// the edges are always seven empty spaces
// find the first and last data points in dataPoints->y() that are not zero and store them in xDataTemp and yDataTemp
// find the maximum (in y) data point

/* For later peak detection, it has to be ensured that the maximum of y values is not the first or last non-zero data point.*/
// add a counter here

// interpolate y, but set it to max if it would be greater max*3

#include <vector>
#include <cmath>
#include <algorithm>
#include <array>

inline static size_t max_position(std::vector<double> vec)
{ // replace with std::ranges
    size_t position = 0;
    double largest = 0;
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i] > largest)
        {
            position = i;
            largest = vec[i];
        }
    }
    return position;
}

std::array<double, 3> calcQuadraticCoefficients(double x_1, double x_2, double x_3, double y_1, double y_2, double y_3)
{
    // calculate regression coefficients for y = a * x^2 + b * x + c
    // pulling the division in a variable saves one instruction
    double divisor_bc = 1 / ((x_1 - x_2) * (x_1 - x_3) * (x_2 - x_3));
    double a = (x_1 * (y_2 - y_3) + x_2 * (y_3 - y_1) + x_3 * (y_1 - y_2)) / ((x_1 - x_2) * (x_1 - x_3) * (x_3 - x_2));
    double b = (x_1 * x_1 * (y_2 - y_3) + x_2 * x_2 * (y_3 - y_1) + x_3 * x_3 * (y_1 - y_2)) * divisor_bc;
    double c = (x_1 * x_1 * (x_2 * y_3 - x_3 * y_2) + x_1 * (x_3 * x_3 * y_2 - x_2 * x_2 * y_3) + x_2 * x_3 * y_1 * (x_2 - x_3)) * divisor_bc;
    // returns the regression coefficients
    return {a, b, c};
}

// for four points, estimate x by merging all possible four functions f(x1,x2,x2), f(x2,x3,x4) etc.

int main()
{
    // data will have to be sorted by scans, again
    std::vector<int> a{8, 9, 10, 11, 12, 14, 15, 16, 20, 21, 22};
    std::vector<double> b{1.079409, 1.272361, 2.155299, 2.703853, 2.996987, 3.153226, 3.006734, 2.751035, 2.423615, 1.446203, 1.247027};
    std::vector<int> x;
    std::vector<double> y;
    // extrapolate outer values
    size_t newLength = a.back() - a.front() + 14; // +14 since seven points are added to each side
    x.reserve(newLength);
    y.reserve(newLength);

    /* extrapolated value at position x, after log transform:
    exp(
    B(0, 0) +
    B(1, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) +
    B(2, 0) * (dataPoints[i]->x() - dataPoints[0]->x()) * (dataPoints[i]->x() - dataPoints[0]->x()))
    ) */

    return 0;
}
