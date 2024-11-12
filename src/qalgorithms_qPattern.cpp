// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qPattern.h"
#include "qalgorithms_datatype_peak.h"
#include "qalgorithms_global_vars.h"

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

namespace qAlgorithms
{
    // function no longer needed
    // std::vector<Peak *> collapseFeaturelist(std::vector<std::vector<Peak>> &peaks, std::vector<size_t> binIndices)
    // {
    //     std::vector<Peak *> returnVec;
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

    void binningRT(std::vector<std::vector<int>> &componentStartEnd, std::vector<FeaturePeak> &featureList,
                   std::vector<float> &OS, std::vector<float> &error, int startBin, int endBin)
    {
        /// adapted generic binning function @todo add into qBinning
        unsigned int size = endBin - startBin + 1;
        float vcrit = 3.05037165842070 * pow(log(size), (TOLERANCE_BINNING)) * (error[endBin + 1] - error[startBin]); // cumError starts with 0
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
                componentStartEnd.push_back(std::vector<int>{startBin});
            }
            else
            {
                binningRT(componentStartEnd, featureList, OS, error, startBin, cutIdx);
            }
            if (cutIdx + 1 == endBin)
            {
                componentStartEnd.push_back(std::vector<int>{endBin});
            }
            else
            {
                binningRT(componentStartEnd, featureList, OS, error, cutIdx + 1, endBin);
            }
        }
        return;
    }

    int absoluteNearest(const unsigned int sourceFeature, std::vector<FeaturePeak> &featureList,
                        std::vector<float> limits_L, std::vector<float> limits_R, std::vector<bool> validPositions)
    {
        // assumes feature list is sorted by RT.
        // limits are defined as RT +- RT uncertiainty
        unsigned int minRT = sourceFeature;
        while ((limits_R[minRT] > limits_L[sourceFeature]) && (minRT > 0))
        {
            minRT--;
        }
        if (limits_R[minRT] < limits_L[sourceFeature])
        {
            minRT++;
        }

        unsigned int maxRT = sourceFeature;
        while ((limits_L[maxRT] < limits_R[sourceFeature]) && (maxRT < limits_L.size()))
        {
            maxRT++;
        }
        if (limits_L[maxRT] > limits_R[sourceFeature])
        {
            maxRT++;
        }
        // the range minRT : maxRT contains all features which are relevant for the pairwise comparison
        // find feature with the least dissimilar regression (excluding height information)
    }

    void initialComponentBinner(std::vector<FeaturePeak> &featureList, unsigned int replicateID)
    {
        int featureCount = featureList.size();
        std::vector<float> orderSpace(featureCount, 0);
        std::vector<float> cumError;
        cumError.reserve(featureCount);

        std::sort(featureList.begin(), featureList.end(), [](FeaturePeak &lhs, FeaturePeak &rhs)
                  { return lhs.retentionTime < rhs.retentionTime; });

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
        int overlapCount = 0;
        for (size_t i = 1; i < preComponents.size(); i++) // @todo change back to i = 1
        {
            std::vector<int> tmpComp = preComponents[i];
            float bridge = featureList[tmpComp[0]].retentionTime - featureList[preComponents[i - 1].back()].retentionTime;
            float tol1 = featureList[tmpComp[0]].retentionTimeUncertainty;
            float tol2 = featureList[preComponents[i - 1].back()].retentionTimeUncertainty;

            if (0.6 < std::min(tol1, tol2))
            {
                std::cout << "-";
            }

            if (bridge < std::min(tol1, tol2))
            {
                std::cout << "H";
                ++overlapCount;
            }
            else if (bridge < std::max(tol1, tol2))
            {
                std::cout << "X";
                ++overlapCount;
            }
            else if (bridge < tol1 + tol2)
            {
                std::cout << "O";
                ++overlapCount;
            }

            // std::cout << tmpComp.size() << "\n\n";
            if (preComponents[i].size() == 1)
            {
                continue;
            }
            // std::cout << tmpComp.size() << "; ";
            for (size_t j = 1; j < tmpComp.size(); j++)
            {
                // std::cout << featureList[tmpComp[j]].retentionTime << ",";
                // float diff = featureList[tmpComp[j]].retentionTime - featureList[tmpComp[j - 1]].retentionTime;
                // float err1 = featureList[tmpComp[j - 1]].retentionTimeUncertainty;
                // float err2 = featureList[tmpComp[j]].retentionTimeUncertainty;
                // if (diff > (err1 + err2))
                // {
                //     std::cout << "H";
                // }
                // else if (diff > std::max(err1, err2))
                // {
                //     std::cout << "X";
                // }
                // else if (diff > std::min(err1, err2))
                // {
                //     std::cout << "O";
                // }
            }
            // std::cout << "b\n";
            // for (size_t j = 0; j < tmpComp.size(); j++)
            // {
            //     std::cout << featureList[tmpComp[j]].mz << ",";
            // }
            // std::cout << "\n";
        }
        std::cout << "\noverlap: " << overlapCount << "; percent: " << float(overlapCount) / preComponents.size() << "\n";
    }
}
