// #include <../include/qalgorithms_qbin.h>
#include "../include/qalgorithms_qPattern.h"
#include "qalgorithms_datatype_peak.h"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
// #include <string>
#include <vector>
#include <iostream>
// #include <fstream>
// #include <sstream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

namespace q
{
    namespace qPattern
    {
        // function no longer needed
        // std::vector<q::DataType::Peak *> collapseFeaturelist(std::vector<std::vector<q::DataType::Peak>> &peaks, std::vector<size_t> binIndices)
        // {
        //     std::vector<q::DataType::Peak *> returnVec;
        //     returnVec.reserve(peaks.size() / 2);
        //     for (size_t i = 0; i < peaks.size(); i++)
        //     {
        //         if (!peaks[i].empty())
        //         {
        //             for (size_t j = 0; j < peaks[i].size(); j++)
        //             {
        //                 returnVec.push_back(&peaks[i][j]);
        //             }
        //         }
        //     }
        //     returnVec.shrink_to_fit();
        //     return returnVec;
        // }

        void binningRT(std::vector<std::vector<int>> &componentStartEnd, std::vector<q::DataType::Peak *> &featureList,
                       std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin)
        {
            /// adapted generic binning function @todo add into qBinning
            unsigned int size = endBin - startBin + 1;
            float vcrit = 3.05037165842070 * pow(log(size), (-0.4771864667153)) * (error[endBin + 1] - error[startBin]); // cumError starts with 0
            /// @todo check for correct calculation in qBinning
            assert(vcrit > 0);
            auto pmax = std::max_element(OS.begin() + startBin, OS.begin() + endBin);
            float max = *pmax * size;

            if (max < vcrit)
            {
                // grouping OK
                std::vector<int> component;
                component.reserve(size);

                for (size_t i = 0; i < size; i++)
                {
                    component.push_back(startBin + i);
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
                    componentStartEnd.push_back(std::vector<int>{1, startBin});
                }
                else
                {
                    binningRT(componentStartEnd, featureList, OS, error, startBin, cutIdx);
                }
                if (cutIdx + 1 == endBin)
                {
                    componentStartEnd.push_back(std::vector<int>{1, endBin});
                }
                else
                {
                    binningRT(componentStartEnd, featureList, OS, error, cutIdx + 1, endBin);
                }
            }
            return;
        }

        void initialComponentBinner(std::vector<q::DataType::Peak> &featureList, unsigned int replicateID)
        {
            int featureCount = featureList.size();
            std::vector<float> orderSpace(featureCount, 0);
            std::vector<float> cumError;
            cumError.reserve(featureCount);

            std::sort(featureList.begin(), featureList.end(), [](q::DataType::Peak *lhs, q::DataType::Peak *rhs)
                      { return lhs->retentionTime < rhs->retentionTime; });

            // create order space
            cumError.push_back(0); // always substract the sum of everything before the current range
            for (int i = 0; i < featureCount; i++)
            {
                cumError.push_back(featureList[i].retentionTimeUncertainty);
            }
            std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum

            for (int i = 0; i < featureCount - 1; i++)
            {
                orderSpace[i] = featureList[i + 1].retentionTime - featureList[i].retentionTime;
            }
            // orderSpace contains all differences of the ordered data and a 0 at the last position

            // for (size_t i = 0; i < featureCount - 1; i++)
            // {
            //     if (featureList[i]->retentionTimeUncertainty < orderSpace[i + 1])
            //     {
            //         std::cout << i << ",";
            //     }
            // }

            std::vector<std::vector<int>> preComponents; // every vector contains the indices of component members
            binningRT(preComponents, featureList, orderSpace, cumError, 0, featureCount - 1);
            // all components will be constructed within the thusly defined groups
            /// @todo make sure to control for adherence to binning when finalising components

            // groups of size one are always valid components?

            /// @todo process parameter: groups of size 1 after component decay
            for (size_t i = 0; i < preComponents.size(); i++)
            {
                if (preComponents[i].size() > 30)
                {
                    exit(1);
                }
                std::vector<int> tmpComp = preComponents[0];
                std::cout << tmpComp.size() << "\n\n";
                for (size_t j = 0; j < tmpComp.size(); j++)
                {
                    std::cout << featureList[tmpComp[j]].retentionTime << ",";
                }
                std::cout << "b\n";
                for (size_t j = 0; j < tmpComp.size(); j++)
                {
                    std::cout << featureList[tmpComp[j]].area << ",";
                }
                std::cout << "\n\n";
            }
        }
    }
}
