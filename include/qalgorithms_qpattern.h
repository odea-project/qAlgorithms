#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatypes.h"
// #include <map>

#include <vector>

namespace qAlgorithms
{
#pragma region structs

    /// @brief connector between two points of a component.
    /// @details for every two points, four similarities are calculated. If two
    /// features show very similar behaviour in all or most categories, they were
    /// likely assigned correctly.
    /// @param RToverlap 1 = identical position; 0 = no overlap between peak borders
    /// @param similarity sum parameter considering the regression coefficient match
    /// @param areaPercent relative area of the target feature in percent
    /// @param heightPercent relative height of the target feature in percent // redundant?
    struct FeatureMatch
    {
        // unsigned int featureIdx; // redundant
        float RToverlap;
        float similarity;
        float areaPercent;
        float heightPercent;
    };

    struct Eliminator
    {
        std::vector<int> conflictingIdxs;
        int trueIdx;
    };

    struct FeatureComponent
    {
        std::vector<int> members;              // size n
        std::vector<FeatureMatch> connections; // size n^2 - n ; idx = n * featureIdx + 0:n-1
        Eliminator *resolveConflict;           // nullptr if no conflicts exist
        float totalScore;
        unsigned int measurementID;
        bool rejected;
    };

#pragma endregion structs

#pragma region functions

    /// @brief Function to remove the two dimensional structure of the qPeaks result. The object itself
    /// cannot be modified after this
    /// @param peaks peaks object returned after call to findpeaks
    /// @return pointers to elements of the vector. Note that modifications to the original peaks vector invalidate the whole componentisation
    // std::vector<Peak *> collapseFeaturelist(std::vector<std::vector<Peak>> &peaks, std::vector<size_t> binIndices);

    /// @brief
    /// @details note that the FeatureComponents do not preserve binning information, since it cannot be used to reliably match datasets
    /// @param featureList feature list generated through collapseFeaturelist
    /// @param replicateID The component is assigned a unique ID to match it with other datasets
    /// @return @todo
    void initialComponentGrouping(std::vector<FeaturePeak> &featureList, unsigned int replicateID);

    /// @brief function which groups features within one sample by retention time. Groups tend to be overly large.
    /// @param componentStartEnd
    /// @param featureList
    /// @param OS
    /// @param error
    /// @param startBin
    /// @param endBin
    void binningRT(std::vector<std::vector<int>> &compGroups, std::vector<FeaturePeak> &featureList,
                   std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin);

    std::vector<std::vector<FeaturePeak>> groupDecay(std::vector<FeaturePeak>);

    int absoluteNearest(FeaturePeak sourceFeature, std::vector<FeaturePeak> &featureList,
                        std::vector<bool> validPositions);

#pragma endregion functions

}

#endif