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
        std::vector<q::DataType::Peak *> collapseFeaturelist(std::vector<std::vector<q::DataType::Peak>> &peaks)
        {
            std::vector<q::DataType::Peak *> returnVec;
            returnVec.reserve(peaks.size() / 2);
            for (size_t i = 0; i < peaks.size(); i++)
            {
                if (!peaks[i].empty())
                {
                    for (size_t j = 0; j < peaks[i].size(); j++)
                    {
                        returnVec.push_back(&peaks[i][j]);
                    }
                }
            }
            returnVec.shrink_to_fit();
            return returnVec;
        }

        void binningRT(std::vector<std::vector<q::DataType::Peak *>> &componentStartEnd, std::vector<q::DataType::Peak *> &featureList,
                       std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin)
        {
            // adapted generic binning function @todo add into qBinning
            unsigned int size = endBin - startBin + 1;
            float vcrit = 3.05037165842070 * pow(log(size), (-0.4771864667153)) * (error[endBin] - error[startBin]);
            auto pmax = std::max_element(OS.begin() + startBin, OS.begin() + endBin);
            float max = *pmax * size;

            if (max < vcrit)
            {
                // grouping OK
                std::vector<q::DataType::Peak *> component;
                component.reserve(size);

                for (size_t i = 0; i < size; i++)
                {
                    component.push_back(featureList[startBin + i]);
                }

                componentStartEnd.push_back(component);
                return;
            }
            else
            {
                // invoke recursive function
                int cutIdx = std::distance(OS.begin(), pmax); // end of the first half
                if (cutIdx == startBin)
                {
                    componentStartEnd.push_back(std::vector<q::DataType::Peak *>{1, featureList[startBin]});
                }
                else
                {
                    binningRT(componentStartEnd, featureList, OS, error, startBin, cutIdx);
                }
                if (cutIdx + 1 == endBin)
                {
                    componentStartEnd.push_back(std::vector<q::DataType::Peak *>{1, featureList[endBin]});
                }
                else
                {
                    binningRT(componentStartEnd, featureList, OS, error, cutIdx + 1, endBin);
                }
            }
            return;
        }

        std::vector<FeatureComponent> initialComponentBinner(std::vector<q::DataType::Peak *> &featureList, unsigned int replicateID)
        {
            int featureCount = featureList.size();
            std::vector<float> orderSpace(featureCount, 0);
            std::vector<float> cumError;
            cumError.reserve(featureCount);

            std::sort(featureList.begin(), featureList.end(), [](q::DataType::Peak *lhs, q::DataType::Peak *rhs)
                      { return lhs->retentionTime < rhs->retentionTime; });

            // create order space
            for (int i = 0; i < featureCount; i++)
            {
                cumError.push_back(featureList[i]->retentionTimeUncertainty);
            }
            std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum

            for (int i = 0; i < featureCount - 1; i++)
            {
                orderSpace[i] = featureList[i + 1]->retentionTime - featureList[i]->retentionTime;
            }
            // orderSpace contains all differences of the ordered data and a 0 at the last position

            std::vector<std::vector<q::DataType::Peak *>> preComponents;
            binningRT(preComponents, featureList, orderSpace, cumError, 0, featureCount);
            // all components will be constructed within the thusly defined groups
            // @todo make sure to control for adherence to binning when finalising components

            // groups of size one are always valid components
            //
        }
    }
}
