#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

#include "qalgorithms_datatypes.h"

#include <vector>

namespace qAlgorithms
{
    struct MultiRegression // @todo this will need to contain all the uncertainties of the original features and find a better solution to the whole bin window output
    {
        float b0_vec[32];
        std::vector<float> cum_RSS; // one element per scan of the subgroup @todo this should is limited by 2*maxscale + 1
        // std::vector<float> b0_vec;
        // all indices relate to the full subgroup!
        size_t idxStart;
        size_t idxEnd;
        unsigned int scanStart; // this is the smallest scan still included in the regression
        unsigned int scanEnd;
        unsigned int idx_x0;
        unsigned int scale;
        unsigned int numPeaks; // @todo track this
        float b1;
        float b2;
        float b3;
        // float RT_apex;
        float DQS;
    };

    // main function to execute a componentiation step on data
    std::vector<MultiRegression> findComponents(
        // note: both features and bins contain a "componentID" field that is 0 by default.
        // the componentisation function updates these fields in addition to returning the component regressions
        std::vector<FeaturePeak> *features,
        std::vector<EIC> *bins,
        const std::vector<float> *convertRT, // this is needed to perform interpolation at the same RT as in qPeaks
        float lowestArea,
        bool printRegs);

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
        size_t idx_S;          // relates to half matrix
        size_t idx_L;          // relates to half matrix
        unsigned int idxStart; // relates to harmonised EIC space
        unsigned int idxEnd;   // relates to harmonised EIC space
        float RSS;             // residual sum of squares
        int component = -1;    // no pair is assigned by default
    };

    struct CompAssignment
    {
        MultiRegression regression;
        std::vector<float> cumRSS;
        unsigned int limit_L;  // refers to harmonised EIC space
        unsigned int limit_R;  // refers to harmonised EIC space
        unsigned int numPeaks; // number of peaks included in this component
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
        std::vector<unsigned int> scanNo;
        std::vector<float> RSS_cum; // cumulative RSS - considers the entire possible block
        std::vector<bool> df;       // @todo get rid of this somehow
        // std::vector<int> DF_cum;    // cumulative degrees of freedom, @todo use this to ensure at least five points overlap
        unsigned int feature_ID;
        unsigned int bin_ID;
        unsigned int minScan;
        // these are the limits of the original feature and the index where the abstract x-axis is 0
        unsigned int featLim_L;
        unsigned int featLim_R;
        unsigned int index_x0;
        // these limits are the first and last point in the EIC which need to be included in a region
        // if that region is to be valid. They are set to the fifth real point from every direction
        unsigned int minIncludedIndex;
        unsigned int maxIncludedIndex;
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
        unsigned int numPeaks;
        unsigned int peakFrame;
        unsigned int groupIdxStart; // this is equal to the offset within the group for later
        unsigned int minScan;       // needed for one-axis position of regression at the end
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

    MergedEIC mergeEICs(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd);

    std::vector<MultiRegression> findCoefficients_multi(
        const std::vector<float> *intensity_log,
        const unsigned int scale,      // maximum scale that will be checked. Should generally be limited by peakFrame
        const unsigned int numPeaks,   // only > 1 during componentisation (for now? @todo)
        const unsigned int peakFrame); // how many points are covered per peak? For single-peak data, this is the length of intensity_log

    MultiRegression runningRegression_multi(
        const MergedEIC *eic,
        const std::vector<ReducedEIC> *eics,
        const std::vector<size_t> *selection,
        const size_t idxStart,
        const size_t idxEnd,
        const unsigned int numPeaks);

    void makeValidRegression_multi(
        RegressionGauss *mutateReg,
        const size_t idxStart,
        const size_t scale,
        const std::vector<unsigned int> *cum_DF,
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