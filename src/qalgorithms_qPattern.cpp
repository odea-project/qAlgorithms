// #include <../include/qalgorithms_qbin.h>
#include "../include/qalgorithms_qPattern.h"
#include "qalgorithms_datatype_peak.h"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
// #include <string>
#include <vector>
// #include <iostream>
// #include <fstream>
// #include <sstream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

namespace q
{
    namespace qPattern
    {
        void binningRT(std::vector<int[2]> &componentStartEnd, std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin)
        {
            // adapted generic binning function @todo add into qBinning
            unsigned int size = endBin - startBin + 1;
            float vcrit = 3.05037165842070 * pow(log(size), (-0.4771864667153)) * (error[endBin] - error[startBin]);
            auto pmax = std::max_element(OS.begin() + startBin, OS.begin() + endBin);
            float max = *pmax * size;

            if (max < vcrit)
            {
                // grouping OK
                componentStartEnd.push_back({startBin, endBin});
                return;
            }
            else
            {
                // invoke recursive function
                int cutIdx = std::distance(OS.begin(), pmax); // end of the first half
                if (cutIdx == startBin)
                {
                    componentStartEnd.push_back({startBin, startBin});
                }
                else
                {
                    binningRT(componentStartEnd, OS, error, startBin, cutIdx);
                }
                if (cutIdx + 1 == endBin)
                {
                    componentStartEnd.push_back({endBin, endBin});
                }
                else
                {
                    binningRT(componentStartEnd, OS, error, cutIdx + 1, endBin);
                }
            }
            return;
        }

        std::vector<FeatureComponent> initialComponentBinner(std::vector<q::DataType::Peak> &featureList)
        {
            int featureCount = featureList.size();
            std::vector<float> orderSpace(featureCount, 0);
            std::vector<std::pair<float, int>> RT_ID;
            RT_ID.reserve(featureCount);
            std::vector<float> cumError;
            cumError.reserve(featureCount);

            // create order space
            for (int i = 0; i < featureCount; i++)
            {
                RT_ID.push_back(std::pair(featureList[i].retentionTime, i));
                cumError.push_back(featureList[i].retentionTimeUncertainty);
            }
            std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
            std::sort(RT_ID.begin(), RT_ID.end(), [](const std::pair<float, int> *lhs, std::pair<float, int> *rhs)
                      { return lhs->first < rhs->first; });
            for (int i = 0; i < featureCount - 1; i++)
            {
                orderSpace[i] = RT_ID[i + 1].first - RT_ID[i].first;
            }
            // orderSpace contains all differences of the ordered data and a 0 at the last position

            std::vector<int[2]> componentStartEnd;
            binningRT(componentStartEnd, orderSpace, cumError, 0, featureCount);
            // all components will be constructed within the thusly defined groups
        }
    }
}
