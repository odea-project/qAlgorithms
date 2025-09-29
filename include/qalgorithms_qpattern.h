#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

#include "qalgorithms_datatypes.h"

#include <vector>

namespace qAlgorithms
{
#define MAX_COMPONENT_SIZE 16
    struct MultiRegression // @todo this will need to contain all the uncertainties of the original features and find a better solution to the whole bin window output
    {
        float b0_vec[MAX_COMPONENT_SIZE];
        std::vector<float> cum_RSS; // one element per scan of the subgroup @todo this should is limited by 2*maxscale + 1
        // std::vector<float> b0_vec;
        // all indices relate to the full subgroup!
        size_t idxStart;
        size_t idxEnd;
        size_t scanStart; // this is the smallest scan still included in the regression
        size_t scanEnd;
        size_t idx_x0;
        size_t scale;
        size_t numPeaks; // @todo track this
        float b1;
        float b2;
        float b3;
        // float RT_apex;
        float DQS;
    };

    void findComponents_new(
        // note: both features and bins contain a "componentID" field that is 0 by default.
        // the componentisation function updates these fields in addition to returning the component regressions
        std::vector<FeaturePeak> *features,
        std::vector<EIC> *bins);

    std::vector<Range_i> preGroup_new(const std::vector<FeaturePeak> *features);

    struct ExcludeMatrix
    {
        // something to store the data, should probably not be a vector
        std::vector<double> storage;
        size_t featureCount = 0;

        void fill(size_t n);

        // mismatch is always maked by a field being 0
        void invalidate(size_t first, size_t second);
        bool isInvalid(size_t first, size_t second);
        size_t indexOf(size_t first, size_t second);
        double *at(size_t first, size_t second);
        size_t countMatches(size_t row);
    };

    ExcludeMatrix makeExcludeMat(const size_t numFeatures);

    // function to create the exclusion matrix
    void pairwiseMatch(const Range_i *region, const std::vector<FeaturePeak> *features, ExcludeMatrix *excludeMatrix);

    // calculate mse / some other regression param for the pair. Since a combined regression only makes sense
    // if it has at least two points to each side for every regression, the EICs do not need to be expanded
    double comparePair(const FeaturePeak *feat_A, const EIC *eic_A, const FeaturePeak *feat_B, const EIC *eic_B);

    // main function to execute a componentiation step on data
    std::vector<MultiRegression> findComponents(
        // note: both features and bins contain a "componentID" field that is 0 by default.
        // the componentisation function updates these fields in addition to returning the component regressions
        std::vector<FeaturePeak> *features,
        std::vector<EIC> *bins,
        const std::vector<float> *convertRT, // this is needed to perform interpolation at the same RT as in qPeaks
        float lowestArea,
        size_t *featuresInComponents);

    struct PreGrouping
    {
        std::vector<FeaturePeak *> features; // this reference is used to modify the original feature
        std::vector<EIC *> EICs;             // update component ID in bin

        // add a sample ID thing here eventually
    };

    struct GroupLims
    {
        size_t start;
        size_t end;
    };

    struct RSS_pair
    {
        MultiRegression regression;
        std::vector<float> cumRSS;
        size_t idx_S;       // relates to half matrix
        size_t idx_L;       // relates to half matrix
        size_t idxStart;    // relates to harmonised EIC space
        size_t idxEnd;      // relates to harmonised EIC space
        float RSS;          // residual sum of squares
        int component = -1; // no pair is assigned by default
    };

    struct CompAssignment
    {
        MultiRegression regression;
        std::vector<float> cumRSS;
        size_t limit_L;  // refers to harmonised EIC space
        size_t limit_R;  // refers to harmonised EIC space
        size_t numPeaks; // number of peaks included in this component
        float RSS = INFINITY;
        int component = -1; // -1 means unassigned, groups start at 0
    };

    // pre-group the region relevant to componentisation based on retention time uncertainty
    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks);

    struct ReducedEIC
    {
        // the reduced EIC only contains those centroids relevant for a feature (equivalent to featCens result file)
        // points in the EIC are already interpolated using the origin feature
        std::vector<float> intensity;
        std::vector<float> intensity_log;
        std::vector<size_t> scanNo;
        std::vector<float> RSS_cum; // cumulative RSS - considers the entire possible block
        std::vector<bool> df;       // @todo get rid of this somehow
        // std::vector<int> DF_cum;    // cumulative degrees of freedom, @todo use this to ensure at least five points overlap
        size_t feature_ID;
        size_t bin_ID;
        size_t minScan;
        // these are the limits of the original feature and the index where the abstract x-axis is 0
        size_t featLim_L;
        size_t featLim_R;
        size_t idxCenter;
        // these limits are the first and last point in the EIC which need to be included in a region
        // if that region is to be valid. They are set to the fifth real point from every direction
        size_t minIncludedIndex;
        size_t maxIncludedIndex;
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
        std::vector<int> DF_cum;
        std::vector<bool> df; // @todo make this cumulative
        size_t numPeaks;
        size_t peakFrame;
        size_t groupIdxStart; // this is equal to the offset within the group for later
        size_t minScan;       // needed for one-axis position of regression at the end
    };

    ReducedEIC harmoniseEIC(const FeaturePeak *feature,
                            const EIC *bin,
                            const std::vector<float> *RTs,
                            const size_t minScan,
                            const size_t maxScan);

    // always calculate the RSS over the entire region of intensities. Since the regression is always
    // centered, only the index where x is 0 must be known in advance
    std::vector<float> cumulativeRSS(const std::vector<float> *intensities,
                                     const RegCoeffs *coeff,
                                     size_t idxCenter);

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
        const std::vector<size_t> *cum_DF,
        const std::vector<float> *intensities,
        const std::vector<float> *intensities_log);

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

    float tanimotoScore(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd);
}

#endif