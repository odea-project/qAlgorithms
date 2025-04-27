#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatypes.h"
// #include <map>

#include <vector>

namespace qAlgorithms
{
    // main function to execute a componentiation step on data
    void findComponents(const std::vector<FeaturePeak> *peaks,
                        const std::vector<EIC> *bins,
                        const std::vector<float> *convertRT); // this is needed to perform interpolation at the same RT as in qPeaks

    struct PreGrouping
    {
        std::vector<const FeaturePeak *> features; // keep this sorted by intensity
        std::vector<float> shapeScores;            // tanimoto scores on a per-feature basis. Access pattern [idx1] * number of features + [idx2]
        std::vector<EIC *> EICs;

        // add a sample ID thing here eventually
        float score(size_t idx1, size_t idx2);
    };

    struct GroupLims
    {
        size_t start;
        size_t end;
    };

    struct MultiMatrix
    {
        std::vector<float> designMatrix;
        size_t n_rows;
        size_t n_cols;
    };

    struct RSS_pair
    {
        std::vector<float> cumRSS;
        size_t idx_S;          // relates to half matrix
        size_t idx_L;          // relates to half matrix
        unsigned int idxStart; // relates to harmonised EIC space
        unsigned int idxEnd;   // relates to harmonised EIC space
        float RSS;             // residual sum of squares
        int component = -1;    // no pair is assigned by default
    };

    struct CompAssignment
    {
        std::vector<float> cumRSS;
        unsigned int limit_L; // refers to harmonised EIC space
        unsigned int limit_R; // refers to harmonised EIC space
        unsigned int members; // number of peaks included in this component
        float RSS = INFINITY;
        int component = -1; // -1 means unassigned, groups start at 0
    };

    // pre-group the region relevant to componentisation based on retention time uncertainty
    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks);

    // This function modifies the coefficients of a regression so that it describes a peak with the apex at
    // x = RT of the feature and the height 1. Features are manipulated this way to allow a shape comparison
    MovedRegression moveAndScaleReg(const FeaturePeak *feature);

    // calculate the shape tanimoto / jaccard index of two moved regressions. The score is weighted slightly.
    // Calculation is done by dividing the area covered by the overlap both regression curves by the area covered
    // by both regressions independently
    // The antiderivative of our regression curve involves exp(6000) and greater numbers, so we use
    // trapezoid integration instead.
    // @todo this runs into problems when applying scaling, check if removal is a good idea
    float calcTanimoto_reg(MovedRegression *feature_A, MovedRegression *feature_B);

    MultiMatrix combinedMatrix(std::vector<std::vector<float>> *intensities);

    // void calcRSS(MovedRegression *reg, const EIC *bin);

    struct ReducedEIC
    {
        // the reduced EIC only contains those centroids relevant for a feature (equivalent to featCens result file)
        // points in the EIC are already interpolated using the origin feature
        std::vector<float> intensity;
        std::vector<float> intensity_log;
        std::vector<size_t> scanNo;
        std::vector<float> RSS_cum; // cumulative RSS - considers the entire possible block
        std::vector<bool> df;       // @todo get rid of this somehow
        size_t feature_ID;
        size_t bin_ID;
        // these are the limits of the original feature and the index where the abstract x-axis is 0
        unsigned int featLim_L;
        unsigned int featLim_R;
        unsigned int index_x0;
    };

    struct MergedEIC
    {
        // combined EICs have only one axis for RT / scans, but combine multiple intensity values. This means that the vectors now
        // differ in size. Since a new merged EIC is not used for further combinations, its limits are automatically that of the
        // RT dimension. The related bins and features no longer provide anything for the analysis, and are only sensible to include
        // for traceability if needed @todo check for use case
        std::vector<float> intensity;
        std::vector<float> intensity_log;
        std::vector<float> RSS_cum; // cumulative RSS - this also serves as the indicator for interpolated / not interpolated
        std::vector<bool> df;       // @todo get rid of this somehow
        size_t numPeaks;
        size_t peakFrame;
        size_t groupIdxStart; // this is equal to the offset within the group for later
    };

    ReducedEIC harmoniseEIC(const FeaturePeak *feature,
                            const EIC *bin,
                            const std::vector<float> *RTs,
                            const unsigned int minScan,
                            const unsigned int maxScan);

    // always calculate the RSS over the entire region of intensities. Since the regression is always
    // centered, only the index where x is 0 must be known in advance
    std::vector<float> cumulativeRSS(const std::vector<float> *intensities,
                                     const RegCoeffs *coeff,
                                     size_t index_x0);

    float logIntAt(const MovedRegression *feature, float RT);

    struct MultiRegression
    {
        std::vector<float> cum_RSS; // one element per scan of the subgroup
        std::vector<float> b0_vec;
        // all indices relate to the full subgroup!
        size_t idxStart;
        size_t idxEnd;
        size_t idx_x0;
        size_t scale;
        float b1;
        float b2;
        float b3;
        float RT_apex;
    };

    struct MovedMultiRegression
    {
        // this regression is used only to calculate the RSS, make function-local? @todo
        std::vector<float> b0_L_vec;
        std::vector<float> b0_R_vec;
        float b1_L;
        float b1_R;
        float b2;
        float b3;
        float RT_switch;
    };

    MergedEIC mergeEICs(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd);

    std::vector<MultiRegression> findCoefficients_multi(
        const std::vector<float> *intensity_log,
        const size_t scale,      // maximum scale that will be checked. Should generally be limited by peakFrame
        const size_t numPeaks,   // only > 1 during componentisation (for now? @todo)
        const size_t peakFrame); // how many points are covered per peak? For single-peak data, this is the length of intensity_log

    MultiRegression runningRegression_multi(
        const MergedEIC *eic,
        const std::vector<ReducedEIC> *eics,
        const std::vector<size_t> *selection,
        const size_t idxStart,
        const size_t idxEnd,
        const size_t numPeaks);

    void makeValidRegression_multi(
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const std::vector<unsigned int> *degreesOfFreedom,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log);

    std::vector<float> pairwiseRSS(const PreGrouping *group, const std::vector<ReducedEIC> *points);

    /// @brief Does the combined regression apply to both input regressions?
    /// @param rss_complex sum of the residual sums of squares for the individual models
    /// @param rss_simple residual sum of squares for the combined model
    /// @param n_total number of data points (is identical for both cases)
    /// @param p_complex parameters (coefficients) of the two individual models
    /// @param p_simple parameters (coefficients) of the combined model
    /// @return returns true if the combined model is not significantly (alpha = 0.05) worse than the combination of the individual models
    bool preferMerge(float rss_complex,
                     float rss_simple,
                     size_t n_total,
                     size_t p_complex);

    float simpleRSS(std::vector<float> *RSS_simple_cum,
                    std::vector<float> *RSS_complex_cum_A,
                    std::vector<float> *RSS_complex_cum_B,
                    size_t idxStart,
                    size_t idxEnd,
                    size_t peakCount,
                    size_t p_complex); // needed for the F-test, should be replaced by a better solution sometime

    void multiFit(const std::vector<ReducedEIC> *fitRegion, const std::vector<MovedRegression> *regressions);
}

#endif