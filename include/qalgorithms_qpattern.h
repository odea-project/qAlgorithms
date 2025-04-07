#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatypes.h"
// #include <map>

#include <vector>

namespace qAlgorithms
{
    // main function to execute a componentiation step on data
    void findComponents(const std::vector<FeaturePeak> *peaks, const std::vector<EIC> *bins);

    struct ComponentGroup
    {
        std::vector<MovedRegression> features; // keep this sorted by intensity
        std::vector<float> shapeScores;        // tanimoto scores on a per-feature basis. Access pattern [idx1] * number of features + [idx2]

        // add a sample ID thing here eventually
        float score(size_t idx1, size_t idx2);
        void calcScores();
    };

    struct ReducedEIC
    {
        // the reduced EIC only contains those centroids relevant for a feature (equivalent to featCens result file)
        std::vector<float> RTs;
        std::vector<float> intensity;
        std::vector<float> intensity_log;
        std::vector<size_t> scanNo;
        // consider recalculating mz @todo
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

    // pre-group the region relevant to componentisation based on retention time uncertainty
    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks);

    // determine the order of comparisons based on the tanimoto score
    std::vector<size_t> getCompareOrder(const ComponentGroup *group);

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

    ReducedEIC harmoniseEICs(const EIC *bin, const size_t minScan, const size_t maxScan, const size_t minIdx, const size_t maxIdx);

    std::vector<float> pairwiseRSS(const ComponentGroup *group, const std::vector<ReducedEIC> *points);

    bool preferMerge(float rss_complex, float rss_simple, size_t n_complex, size_t p_complex, size_t p_simple);

    void multiFit(const std::vector<ReducedEIC> *fitRegion, const std::vector<FeaturePeak> *peaks);
}

#endif