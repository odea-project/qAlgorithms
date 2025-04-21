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
        std::vector<MovedRegression> features; // keep this sorted by intensity
        std::vector<float> shapeScores;        // tanimoto scores on a per-feature basis. Access pattern [idx1] * number of features + [idx2]
        std::vector<EIC *> EICs;

        // add a sample ID thing here eventually
        float score(size_t idx1, size_t idx2);
        void calcScores();
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

    struct CompAssignment
    {
        const size_t feature;
        int component = -1; // -1 means unassigned, groups start at 0
    };

    // pre-group the region relevant to componentisation based on retention time uncertainty
    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks);

    // determine the order of comparisons based on the tanimoto score
    std::vector<size_t> getCompareOrder(const PreGrouping *group);

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

    void calcRSS(MovedRegression *reg, const EIC *bin);

    struct ReducedEIC
    {
        // the reduced EIC only contains those centroids relevant for a feature (equivalent to featCens result file)
        // points in the EIC are already interpolated using the origin feature
        // std::vector<float> RTs; // retention times are determined first so all points are equal
        std::vector<float> intensity;
        std::vector<float> intensity_log;
        std::vector<size_t> scanNo;
        std::vector<float> RSS_cum; // cumulative RSS - this also serves as the indicator for interpolated / not interpolated
        size_t feature_ID;
        size_t bin_ID;
        // these two are the limits of the feature the given EIC belonged to
        unsigned int featLim_L;
        unsigned int featLim_R;
    };

    ReducedEIC harmoniseEIC(const MovedRegression *feature,
                            const EIC *bin,
                            const std::vector<float> *RTs,
                            const unsigned int minScan,
                            const unsigned int maxScan,
                            const size_t minIdx,
                            const size_t maxIdx);

    float logIntAt(const MovedRegression *feature, float RT);

    std::vector<float> pairwiseRSS(const PreGrouping *group, const std::vector<ReducedEIC> *points);

    /// @brief Does the combined regression apply to both input regressions?
    /// @param rss_complex sum of the residual sums of squares for the individual models
    /// @param rss_simple residual sum of squares for the combined model
    /// @param n_total number of data points (is identical for both cases)
    /// @param p_complex parameters (coefficients) of the two individual models
    /// @param p_simple parameters (coefficients) of the combined model
    /// @return returns true if the combined model is not significantly (alpha = 0.05) worse than the combination of the individual models
    bool preferMerge(float rss_complex, float rss_simple, size_t n_total, size_t p_complex, size_t p_simple);

    void multiFit(const std::vector<ReducedEIC> *fitRegion, const std::vector<MovedRegression> *regressions);
}

#endif