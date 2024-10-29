#ifndef _QALGORITHMS_QPATTERN_INCLUDED
#define _QALGORITHMS_QPATTERN_INCLUDED

// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_datatype_peak.h"
// #include <map>

#include <vector>

namespace q
{
    namespace qPattern
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

        struct FeatureComponent
        {
            std::vector<q::DataType::Peak *> members; // size n
            std::vector<FeatureMatch> connections;    // size n^2 - n ; idx = n * featureIdx + 0:n-1
            float totalScore;
            unsigned int measurementID;
        };

        struct Eliminator
        {
            int originIdx[2];
            int targetIdx[2];
            FeatureComponent &origin;
            FeatureComponent &target;
        };

#pragma endregion structs

#pragma region functions

        std::vector<FeatureComponent> initialComponentBinner(std::vector<q::DataType::Peak> *featureList);

        void binningRT(std::vector<int[2]> *componentStartEnd, std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin);

#pragma endregion functions

    }
}
#endif