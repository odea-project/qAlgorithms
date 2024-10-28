// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpeaks.h"
// #include <map>

#include <vector>

namespace q
{
    namespace qPattern
    {
        struct FeatureMatch
        {
            unsigned int featureIdx;
            float similarity;
            float areaPercent;
            float heightPercent;
        };

        struct FeatureComponent
        {
            std::vector<q::DataType::Peak *> members; // size n
            std::vector<FeatureMatch> connections;    // size n^2 - n
            float totalScore;
        };

    }
}