#include <cmath>
#include <cstddef>
#include <cstdint>

namespace qAlgorithms
{
    struct sf_NormalParams
    {
        float sd = 0;
        float mean = 0;
        float upscale = 0;
    };

    // These are the properties of a peak relevant to a user of peak simulation
    struct PeakSimTarget
    {
        size_t points = 0;
        size_t idxPos = 0;  // index closest to the peak maximum
        float position = 0; // position of the apex of the signal
        float skewness = 0; // 0 is symmetric, negative values mean left shifted apex, positive ones shift right
    };

    struct SimulatedPeak
    {
        const double *xvals = nullptr;
        double *yvals = nullptr;
        size_t len = 0;
        // the below values are analytic if the function is defined theoretically
        // (are there other types of functions we should use?)
        float position = 0;
        float fwhm = 0;
        float area = 0;
        float height = 0;
    };

    // this file is intended for generative peak shape functions
    // intended interface per function:

    // a utility function is used to create a settings struct from desired properties such as position
    // or height
    // calling function sets up settings struct and ensures x and y values have the same number of elements
    // int count = func_array(const ParamsStruct *settings, const float *xvals float *res_arr)
    // {
    //     for all x values
    //     float res = func(settings, x)
    //     assert( func executed successfully )
    //     res_array[i] = res
    // }

    // 1) theoretical distributions

    // 1.1) normal gaussian distribution

    sf_NormalParams sf_normal_set(float position, float height, float fwhm)
    {
        return {
            0,
            position,
            0,
        };
    }

    inline double sf_normal_gen(const sf_NormalParams *settings, const double x)
    {
        const double apex = settings->mean;
        const double sdev = settings->sd;
        return settings->upscale * exp(-(x - apex) * (x - apex) / (2 * sdev * sdev));
    }

    SimulatedPeak sf_normal_arr(const sf_NormalParams *settings, const double *xvals, const size_t len, double *res_arr)
    {
        for (size_t i = 0; i < len; i++)
            res_arr[i] = sf_normal_gen(settings, xvals[i]);

        return {
            xvals,
            res_arr,
            len,
            0,
            0,
            0,
            0,
        };
    }
} // namespace qAlgorithms