#ifndef QALGORITHMS_UTILS_H
#define QALGORITHMS_UTILS_H

#include <cstddef>
#include <cstdint>
#include <stddef.h>

// ### functions in this file should never call malloc! ###

/// @brief
namespace qAlgorithms
{
    // sqrt(pi) / 2
    static const double SQRTPI_2 = 0.886226925452758013649;

    /**
     * @brief perform the welch-test for comparing two means of unequal variance
     * @param mean mean of sample 1 or 2
     * @param sd standard deviation of sample 1 or 2
     * @param count number of samples of sample 1 or 2
     * @return true if H0 is rejected, false otherwise
     */
    bool t_test_welch(const double mean_1, const double sd_1, const double count_1,
                      const double mean_2, const double sd_2, const double count_2,
                      const float alpha);

    /**
     * @brief performs the F-test for comparing two regressions
     *
     * @param RSS_complex     Residual sum of squares for the full (alternative) model
     * @param RSS_simple      Residual sum of squares under the null (reduced) model
     * @param params_complex  Number of parameters in the full model
     * @param params_simple   Number of parameters in the null model
     * @param n               Number of real data points both models apply to
     * @return                Is the complex model better than the simple model (y/n)?
     */
    bool F_test_regs(const double RSS_complex, const double RSS_simple,
                     const uint32_t params_complex, const uint32_t params_simple,
                     const size_t n, const double alpha);

    /**
     * @brief Computes the F-statistic / F-Value for comparing a full model against a null model
     *
     * @param RSS_complex     Residual sum of squares for the full (alternative) model
     * @param RSS_simple      Residual sum of squares under the null (reduced) model
     * @param params_complex  Number of parameters in the full model
     * @param params_simple   Number of parameters in the null model
     * @param n               Number of real data points both models apply to
     * @return                The calculated F-statistic
     */
    double F_value(const double RSS_complex, const double RSS_simple,
                   const double params_complex, const double params_simple,
                   const size_t n);

    /**
     * @brief Computes the critical F‐statistic for a given alpha and model parameter counts using the cdff function from cdflib
     * @param alpha            Significance level (ex. 0.05 for 95% confidence)
     * @param params_complex   Number of parameters in the more complex model
     * @param params_simple    Number of parameters in the simpler (nested) model
     * @param numPoints        Total number of observations
     * @return The critical F value (quantile) such that P(F ≤ returned_value) = 1 – α,
     *         for an F distribution with numerator df = params_complex – params_simple
     *         and denominator df = numPoints – params_complex.
     */
    double F_stat(double alpha, size_t params_complex, size_t params_simple, size_t numPoints);

    // simplified linear regression using a positive integer axis for x
    void linReg_intx(const float *yvals,
                     const size_t length,
                     double *slope, double *intercept);

    void weightedLinReg(const double *xvals,
                        const double *yvals,
                        const double *variance,
                        const size_t length,
                        double *slope, double *intercept);

    // exact solution for quadratic equation with three points and thing to get the y for a given x
    void coeffsQuadratic(const double x1, const double x2, const double x3,
                         const double y1, const double y2, const double y3,
                         double *b0, double *b1, double *b2);
    double quadraticAt(const double b0, const double b1, const double b2,
                       const double x);

    // well-behaved function to calculate the quadratic formula for a x^2 + b x + c = 0
    // the standard equation x_(1,2) = (-b +- sqrt(b^2 - 4ac)) / (2a) is unstable at small differences
    // between intermediates, see https://www.av8n.com/physics/quadratic-formula.htm
    int solveQuadratic(const double a, const double b, const double c,
                       double *x1, double *x2);

    /// @brief Calculate residual sum of squares (RSS) for predicted and observed values
    /// @param predict
    /// @param observed
    /// @param range
    /// @return

    /**
     * @brief returns the critical value for binning using the order spaces
     * @details hard-coded, empirically determined values for alpha = 0.05
     * @param n number of points in the tested distribution
     * @param stdDev standard deviation of the distribution
     * @return maximum tolerated order space under the assumption that the data are from one distribution
     */
    double binningCritVal(const size_t n, const double stdDev); // critical value for deciding if a bin exists or not

    // min and max functions
    size_t min(size_t a, size_t b);
    size_t max(size_t a, size_t b);
    uint16_t min(uint16_t a, uint16_t b);
    uint16_t max(uint16_t a, uint16_t b);
    int min(int a, int b);
    int max(int a, int b);
    double min(double a, double b);
    double max(double a, double b);

    float *minVal(float *const arrayStart, const size_t length);
    const float *minVal(const float *const arrayStart, const size_t length);
    double *minVal(double *const arrayStart, const size_t length);
    const double *minVal(const double *const arrayStart, const size_t length);

    float *maxVal(float *const arrayStart, const size_t length);
    const float *maxVal(const float *const arrayStart, const size_t length);
    double *maxVal(double *const arrayStart, const size_t length);
    const double *maxVal(const double *const arrayStart, const size_t length);

    void clampSorted(const float *const array,
                     size_t *const startIdx,
                     size_t *const length,
                     const float lim_lower,
                     const float lim_upper);

    double meanOfCumulative(double *const cumArray, const size_t startIdx, const size_t endIdx);
    double meanOfCumulative(const double *const cumArray, const size_t startIdx, const size_t endIdx);
    size_t sumOfCumulative(const uint16_t *const cumArray, const size_t startIdx, const size_t length);

    // standard deviation
    double sdev(double *const array, const size_t n);
    double sdev(const double *const array, const size_t n);

    double calcJaccardIdx(const float *const array1, const float *const array2, const size_t length);

    // @todo get rid of these by putting them into a function
    // first element is -1 so the correct access is T_VALS[df]
    static constexpr double T_VALUES[257] = {-1, 6.313751515, 2.91998558, 2.353363435, 2.131846781, 2.015048373, 1.94318028, 1.894578605, 1.859548038, 1.833112933, 1.812461123, 1.795884819, 1.782287556, 1.770933396, 1.761310136, 1.753050356, 1.745883676, 1.739606726, 1.734063607, 1.729132812, 1.724718243, 1.720742903, 1.717144374, 1.713871528, 1.71088208, 1.708140761, 1.70561792, 1.703288446, 1.701130934, 1.699127027, 1.697260894, 1.695518789, 1.693888742, 1.692360304, 1.690924251, 1.689572454, 1.688297711, 1.687093617, 1.685954458, 1.684875119, 1.683851011, 1.682878, 1.681952356, 1.681070702, 1.680229975, 1.679427392, 1.678660413, 1.677926721, 1.677224195, 1.676550892, 1.675905025, 1.67528495, 1.674689153, 1.674116236, 1.673564906, 1.673033965, 1.672522303, 1.672028888, 1.671552762, 1.671093032, 1.670648865, 1.670219484, 1.669804162, 1.669402222, 1.669013025, 1.668635976, 1.668270514, 1.667916114, 1.667572281, 1.667238549, 1.666914479, 1.666599658, 1.666293696, 1.665996224, 1.665706893, 1.665425373, 1.665151353, 1.664884537, 1.664624644, 1.664371409, 1.664124579, 1.663883913, 1.663649184, 1.663420175, 1.663196679, 1.6629785, 1.662765449, 1.662557349, 1.662354029, 1.662155326, 1.661961084, 1.661771155, 1.661585397, 1.661403674, 1.661225855, 1.661051817, 1.66088144, 1.66071461, 1.660551217, 1.660391156, 1.660234326, 1.66008063, 1.659929976, 1.659782273, 1.659637437, 1.659495383, 1.659356034, 1.659219312, 1.659085144, 1.658953458, 1.658824187, 1.658697265, 1.658572629, 1.658450216, 1.658329969, 1.65821183, 1.658095744, 1.657981659, 1.657869522, 1.657759285, 1.657650899, 1.657544319, 1.657439499, 1.657336397, 1.65723497, 1.657135178, 1.657036982, 1.656940344, 1.656845226, 1.656751594, 1.656659413, 1.656568649, 1.65647927, 1.656391244, 1.656304542, 1.656219133, 1.656134988, 1.65605208, 1.655970382, 1.655889868, 1.655810511, 1.655732287, 1.655655173, 1.655579143, 1.655504177, 1.655430251, 1.655357345, 1.655285437, 1.655214506, 1.655144534, 1.6550755, 1.655007387, 1.654940175, 1.654873847, 1.654808385, 1.654743774, 1.654679996, 1.654617035, 1.654554875, 1.654493503, 1.654432901, 1.654373057, 1.654313957, 1.654255585, 1.654197929, 1.654140976, 1.654084713, 1.654029128, 1.653974208, 1.653919942, 1.653866317, 1.653813324, 1.653760949, 1.653709184, 1.653658017, 1.653607437, 1.653557435, 1.653508002, 1.653459126, 1.6534108, 1.653363013, 1.653315758, 1.653269024, 1.653222803, 1.653177088, 1.653131869, 1.653087138, 1.653042889, 1.652999113, 1.652955802, 1.652912949, 1.652870547, 1.652828589, 1.652787068, 1.652745977, 1.65270531, 1.652665059, 1.652625219, 1.652585784, 1.652546746, 1.652508101, 1.652469842, 1.652431964, 1.65239446, 1.652357326, 1.652320556, 1.652284144, 1.652248086, 1.652212376, 1.652177009, 1.652141981, 1.652107286, 1.65207292, 1.652038878, 1.652005156, 1.651971748, 1.651938651, 1.651905861, 1.651873373, 1.651841182, 1.651809286, 1.651777679, 1.651746359, 1.65171532, 1.65168456, 1.651654074, 1.651623859, 1.651593912, 1.651564228, 1.651534805, 1.651505638, 1.651476725, 1.651448062, 1.651419647, 1.651391475, 1.651363544, 1.65133585, 1.651308391, 1.651281164, 1.651254165, 1.651227393, 1.651200843, 1.651174514, 1.651148402, 1.651122505, 1.65109682, 1.651071345, 1.651046077, 1.651021013, 1.650996152, 1.65097149, 1.650947025, 1.650922755, 1.650898678, 1.650874791, 1.650851092, 1.650827579};
} // namespace qAlgorithms
#endif // QALGORITHMS_UTILS_H