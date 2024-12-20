#include "qalgorithms_qbin.h"
#include "qalgorithms_global_vars.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <deque> // main bin container is a deque
#include <string>
#include <chrono> // time code execution
#include <ctime>
// #include <omp.h>
#include <filesystem> // printing absolute path in case read fails

namespace qAlgorithms
{

#pragma region "misc"
    // declarations of static functions
    /// @brief calculate the mean distance in mz to all other elements of a sorted vector for one element
    /// @param pointsInBin vector of data points sorted by mz
    /// @return vector of the mean inner distances for all elements in the same order as pointsInBin
    // static std::vector<double> meanDistance(const std::vector<qCentroid *> pointsInBin);

    /// @brief calculate the mean distance in mz to all other close elements of a sorted vector for one element
    /// @param pointsInBin vector of data points sorted by scans
    /// @param maxdist maximum distance in scans for similar points
    /// @return vector of the mean inner distances for all elements in the same order as pointsInBin
    static std::vector<float> meanDistanceRegional(const std::vector<qCentroid *> pointsInBin, int maxdist);

    /// @brief calculate the data quality score as described by Reuschenbach et al. for one datapoint in a bin
    /// @param MID mean inner distance in mz to all other elements in the bin
    /// @param MOD minimum outer distance - the shortest distance in mz to a data point that is within maxdist and not in the bin
    /// @return the data quality score for the specified element
    static inline float calcDQS(const float MID, const float MOD); // Mean Inner Distance, Minimum Outer Distance

    // static void scaleDistancesForDQS_gauss(int maxdist); // removed

    static void scaleDistancesForDQS_linear(int maxdist); // @experimental

    // binning-specific variables
    int maxdist;
    std::vector<qCentroid *> notInBins; // this vector contains all points which are not included in bins
    int duplicatesTotal;                // total times the distance 0 (in scans) was measured in closed bins
    std::vector<float> scalarForMOD;    // This vector contains factors for an inverse gaussian scaling of distances during DQS calculation

#define IGNORE -256 // nonsense value if no real number exists. Must be
                    // negative since it is used later when searching for
                    // the smallest distance. It is not set to -Inf so that
                    // the resulting DQS is still numeric.
#define NO_MIN_FOUND -INFINITY
#define NO_MAX_FOUND -INFINITY

#pragma endregion "misc"

#pragma region "BinContainer"

    enum SubsetMethods
    {
        mz,
        scans
    };

    BinContainer::BinContainer() {}

    void BinContainer::makeFirstBin(CentroidedData *rawdata)
    {
        finishedBins.reserve(rawdata->lengthAllPoints / 50); // max of two reallocations
        Bin firstBin(rawdata);
        assert(firstBin.pointsInBin.size() > 4); // @todo remove
        binDeque.push_back(firstBin);
    }

    void BinContainer::subsetBins(const std::vector<int> dimensions, const int maxdist, bool rebin, const double massError = -1)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        auto timeEnd = std::chrono::high_resolution_clock::now();
        std::string subsetType;
        std::cout << "\nOpen Bins: " << binDeque.size() << " - ";
        while (!binDeque.empty()) // while elements are in the bin deque -> if any bin is not fully subset
        {
            for (size_t i = 0; i < dimensions.size(); i++) // dimensions vector allows user to choose order of executing subsetting actions.
            {
                const size_t startpoint = binDeque.size(); // startpoint is the first element of the deque that will not be subset, equivalent to do n times
                subsetCount = 0;
                switch (dimensions[i])
                {
                case mz:
                {
                    subsetType = "MZ";
                    for (size_t j = 0; j < startpoint; j++) // for every element in the deque before writing new bins
                    {
                        binDeque.front().makeOS();
                        binDeque.front().makeCumError(); // always after order space, since makeOS sorts
                        // takes element from binDeque, starts subsetting, appends bins to binDeque
                        binDeque.front().subsetMZ(&binDeque, binDeque.front().activeOS, 0, binDeque.front().activeOS.size() - 1, subsetCount);
                        binDeque.pop_front(); // remove the element that was processed from binDeque
                    }
                    break;
                }

                case scans:
                {
                    subsetType = "Scans";
                    // the function that determines if a bin was completed must know if
                    // binning was already completed once
                    std::vector<Bin> &target = rebin ? redoneBins : finishedBins;
                    for (size_t j = 0; j < startpoint; j++)
                    {
                        binDeque.front().subsetScan(&binDeque, &target, maxdist, subsetCount);
                        binDeque.pop_front();
                    }
                    break;
                }

                [[unlikely]] default:
                    std::cerr << "\nSeparation method " << dimensions[i] << " is not a valid parameter, terminating program\n";
                    finishedBins.clear();
                    goto exit;
                }
                timeEnd = std::chrono::high_resolution_clock::now();
                if (rebin)
                {
                    std::cout << "subset in " << subsetType << "\nTime: " << (timeEnd - timeStart).count() << " ns\nSubsets performed: "
                              << subsetCount << "\nClosed Bins: " << redoneBins.size() << "\n--\nOpen Bins: " << binDeque.size() << " - ";
                }
                else
                {
                    std::cout << "subset in " << subsetType << "\nTime: " << (timeEnd - timeStart).count() << " ns\nSubsets performed: "
                              << subsetCount << "\nClosed Bins: " << finishedBins.size() << "\n--\nOpen Bins: " << binDeque.size() << " - ";
                }

                timeStart = std::chrono::high_resolution_clock::now();
            }
        }
    exit:
        std::cout << "completed subsetting\n";
    }

    void BinContainer::assignDQSB(const CentroidedData *rawdata, const int maxdist, bool rebin)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();

        std::vector<Bin> &target = rebin ? redoneBins : finishedBins;
        // bins are sorted after DQSB calculation, meaning scanMin and ScanMax parameters are wrong
        // this only works if bins exist
        assert(target[0].DQSB_base.size() == 0);

        for (size_t i = 0; i < target.size(); i++)
        {
            target[i].makeDQSB(rawdata, maxdist);
        }
        auto timeEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Finished Calculating DQSBs in " << (timeEnd - timeStart).count() << " ns\n";
    }

    void BinContainer::redoBinningIfTooclose(const std::vector<int> dimensions, const CentroidedData *rawdata,
                                             std::vector<qCentroid *> notbinned, const int maxdist)
    {
        std::cout << "starting re-binning procedure...\n";
        // assemble new bin by copying bins with hot ends into notInBins
        std::vector<size_t> incompleteBins;

        incompleteBins.reserve(finishedBins.size() / 10);
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            if ((finishedBins[i].l_maxdist_tooclose || finishedBins[i].r_maxdist_tooclose))
            {
                incompleteBins.push_back(i);
            }
            else if (finishedBins[i].duplicateScan)
            {
                incompleteBins.push_back(i);
            }
            else
            {
                float DQSsum = std::accumulate(finishedBins[i].DQSB_base.begin(), finishedBins[i].DQSB_base.end(), 0.0);
                if (DQSsum <= 0) // no need to check average since DQSB is not scaled
                {
                    incompleteBins.push_back(i);
                }
            }
        }
        if (incompleteBins.empty())
        {
            std::cout << "nothing to re-bin, continuing...\n";
            return;
        }
        std::cout << "processing " << incompleteBins.size() << " bins\n";
        // Assemble new starting bin
        Bin startingRebin;
        startingRebin.pointsInBin.reserve(notInBins.size() + incompleteBins.size() * 16); // @todo test the average size for non-warburg data
        [[unlikely]] if (!notInBins.empty())
        {
            startingRebin.pointsInBin.insert(startingRebin.pointsInBin.end(), notInBins.begin(), notInBins.end()); // copy outofbins into new bin
            notInBins.clear();
        }
        else
        {
            std::cout << "no not-binned points, continuing...\n";
        }
        // copy old pointsInBin for each selected bin
        for (size_t i : incompleteBins)
        {
            startingRebin.pointsInBin.insert(startingRebin.pointsInBin.end(), finishedBins[i].pointsInBin.begin(), finishedBins[i].pointsInBin.end());
        }
        binDeque.push_back(startingRebin);
        // call binning wrapper with a vector other than finishedbins as write target
        this->subsetBins(dimensions, maxdist, true);
        if (redoneBins.empty())
        {
            return;
        }
        // calculate DQS on new bins
        this->assignDQSB(rawdata, maxdist, true);
        // re-add the newly constructed bins to finishedBins
        if (redoneBins.size() < incompleteBins.size()) // there will be gaps left in finsihedBins that must be closed
        {
            size_t i = 0;
            for (Bin newBin : redoneBins) // fill gaps with new bins
            {
                finishedBins[incompleteBins[i]] = newBin;
                ++i;
            }
            while (i < incompleteBins.size()) // transfer elements from finishedBins
            {
                finishedBins[incompleteBins[i]] = std::move(finishedBins.back());
                finishedBins.pop_back();
                ++i;
            }
        }
        else
        {
            size_t i = 0;
            for (size_t j : incompleteBins)
            {
                finishedBins[j] = redoneBins[i];
                ++i;
            }
            while (i < redoneBins.size())
            {
                finishedBins.push_back(redoneBins[i]);
                ++i;
            }
        }

        std::cout << "re-binning completed\nremoved " << incompleteBins.size() << " incomplete bins, added "
                  << redoneBins.size() << " corrected bins\n";

        redoneBins.resize(0); // allow other functions to use redoneBins
        return;
    }

    void BinContainer::reconstructFromStdev(const CentroidedData *rawdata, int maxdist)
    {

        // find all not binned points and add them to a bin
        // cond 1: mz must be within one standard deviation of mz
        // cond 2: point must be within maxdist scans of the bin borders
        // cond 3: If a point would be added to two bins, add it to none
        /*
        Bool vector of length notbinned
        vector of start, end for all bins (mz-stdev and mz+stdev)
        if point between start, end -> bool[i]
        if bool[i] true -> next point
        */
        // -1 = does not fit any bin, -2 = fits two ore more, bin number if only one fits so far
        struct BinBorders
        {

            float massRangeStart;
            float massRangeEnd;
            int binPosition;
            int scanRangeStart;
            int scanRangeEnd;
        };
        const size_t numOfBins = this->finishedBins.size();
        const size_t numOfUnbinned = notInBins.size();
        std::vector<int> selectedOnce(numOfUnbinned, -1);
        std::vector<BinBorders> binRange;
        binRange.reserve(numOfBins);
        // sort to stop test earlier
        std::sort(notInBins.begin(), notInBins.end(), [](const qCentroid *lhs, const qCentroid *rhs)
                  { return lhs->mz < rhs->mz; });

        int binpos = 0;
        for (Bin currentBin : finishedBins)
        {
            float stdev = currentBin.calcStdevMZ();

            // float trueVcrit = 3.05037165842070 * pow(log(currentBin.pointsInBin.size() + 1), (TOLERANCE_BINNING)) * stdev;
            float trueVcrit = OS_CRIT_A + OS_CRIT_B / std::sqrt(std::log(currentBin.pointsInBin.size() + 1));
            binRange.push_back(BinBorders{
                currentBin.mzMin - trueVcrit,
                currentBin.mzMax + trueVcrit,
                binpos,
                currentBin.scanMin - maxdist - 1,
                currentBin.scanMax + maxdist + 1,
            });
            ++binpos;
        }

        std::sort(binRange.begin(), binRange.end(), [](const BinBorders lhs, const BinBorders rhs)
                  { return lhs.massRangeEnd < rhs.massRangeEnd; });

        // store potential conflicts as pairs for later merging
        std::vector<std::pair<int, int>> conflictOfBinterest;

        // for every point, iterate until two matches are found
        size_t startPoint = 0;
        int tmpCounter = 0;
        int prevtest = -1;
        int binModCount = 0;
        for (size_t i = 0; i < numOfUnbinned; i++)
        {
            // size_t testposition = startPoint;
            bool oneMore = false;
            int binInsertPoint = -1;

            // while (testposition < numOfBins)
            for (size_t testposition = startPoint; testposition < numOfBins; testposition++)
            {
                // stop if mass is greater than last min +0.01
                if (binRange[testposition].massRangeEnd + 0.01 < notInBins[i]->mz)
                {
                    startPoint = testposition;
                    break;
                }
                // if the bin could accept this point
                else if (((binRange[testposition].scanRangeStart < int(notInBins[i]->scanNo)) &&
                          (binRange[testposition].scanRangeEnd > int(notInBins[i]->scanNo))) &&
                         ((binRange[testposition].massRangeStart <= notInBins[i]->mz) &&
                          (binRange[testposition].massRangeEnd >= notInBins[i]->mz)))
                {
                    if (oneMore)
                    {
                        // which bad pair is associated with this point is described by the negative index
                        conflictOfBinterest.push_back({testposition, selectedOnce[i]});
                        selectedOnce[i] = -conflictOfBinterest.size() - 1;
                        oneMore = false;
                        break;
                    }
                    selectedOnce[i] = testposition;
                    binInsertPoint = binRange[testposition].binPosition;
                    oneMore = true;
                    ++tmpCounter;
                }
                // ++testposition;
            }
            if (oneMore)
            {
                // add found points to the bins
                // points need to be added to finishedBins directly so they are merged
                // if an expanded bin would be merged
                finishedBins[binInsertPoint].pointsInBin.push_back(notInBins[i]);
                if (binInsertPoint != prevtest)
                {
                    prevtest = binInsertPoint;
                    ++binModCount;
                }
                // update duplicate finder and minmax of scans / mz
                bool duplicate = false;
                int scanMin = 2147483647; // max possible value of int
                int scanMax = 0;
                float mzMin = INFINITY;
                float mzMax = 0;
                for (qCentroid *cen : finishedBins[binInsertPoint].pointsInBin)
                {
                    if (cen->int_area == notInBins[i]->int_area)
                    {
                        duplicate = true;
                    }
                    if (cen->mz > mzMax)
                    {
                        mzMax = cen->mz;
                    }
                    if (cen->mz < mzMin)
                    {
                        mzMin = cen->mz;
                    }
                    if (cen->scanNo > scanMax)
                    {
                        scanMax = cen->scanNo;
                    }
                    if (cen->scanNo < scanMin)
                    {
                        scanMin = cen->scanNo;
                    }
                }
                finishedBins[binInsertPoint].duplicateScan = duplicate;
                finishedBins[binInsertPoint].mzMax = mzMax;
                finishedBins[binInsertPoint].mzMin = mzMin;
                finishedBins[binInsertPoint].scanMax = scanMax;
                finishedBins[binInsertPoint].scanMin = scanMin;
                finishedBins[binInsertPoint].makeDQSB(rawdata, maxdist);
            }
        }
        std::cout << tmpCounter << " points added to " << binModCount << " bins based on corrected estimate\n"
                  << conflictOfBinterest.size() << " points are within the range of two bins and have been discarded\n";

        // find overlap between bins
        std::vector<int> combinedBins;
        for (size_t i = 0; i < numOfBins - 1; i++)
        {
            BinBorders lowerMZbin = binRange[i];
            BinBorders upperMZbin = binRange[i + 1];
            if (lowerMZbin.massRangeEnd > upperMZbin.massRangeStart)
            {
                if (((lowerMZbin.scanRangeEnd - maxdist > upperMZbin.scanRangeStart) &
                     (lowerMZbin.scanRangeEnd - maxdist < upperMZbin.scanRangeEnd)) |
                    ((upperMZbin.scanRangeEnd - maxdist > lowerMZbin.scanRangeStart) &
                     (upperMZbin.scanRangeEnd - maxdist < lowerMZbin.scanRangeEnd)))
                {
                    Bin *binA = &finishedBins[lowerMZbin.binPosition];
                    Bin *binB = &finishedBins[upperMZbin.binPosition];
                    // only merge if at least one bin is slanted and the merge
                    // would negate the slantedness
                    if (((binA->l_slanted & (lowerMZbin.scanRangeStart > upperMZbin.scanRangeStart)) |
                         (binA->r_slanted & (lowerMZbin.scanRangeEnd < upperMZbin.scanRangeEnd))) |
                        ((binB->l_slanted & (upperMZbin.scanRangeStart > lowerMZbin.scanRangeStart)) |
                         (binB->r_slanted & (upperMZbin.scanRangeEnd < lowerMZbin.scanRangeEnd))))
                    {
                        if ((lowerMZbin.massRangeEnd > binB->mzMin) |
                            (upperMZbin.massRangeStart < binA->mzMin))
                        {
                            combinedBins.push_back(lowerMZbin.binPosition);
                            combinedBins.push_back(upperMZbin.binPosition);
                            Bin mergedBin;

                            mergedBin.mzMax = std::max(binA->mzMax, binB->mzMax);
                            mergedBin.mzMin = std::min(binA->mzMin, binB->mzMin);
                            mergedBin.scanMax = std::max(binA->scanMax, binB->scanMax);
                            mergedBin.scanMin = std::min(binA->scanMin, binB->scanMin);
                            // @todo no check for duplicates is performed here,
                            // since that is already included in the EIC export
                            // @todo only merge if the slanted side matches?

                            mergedBin.pointsInBin.insert(mergedBin.pointsInBin.end(), binA->pointsInBin.begin(), binA->pointsInBin.end());
                            mergedBin.pointsInBin.insert(mergedBin.pointsInBin.end(), binB->pointsInBin.begin(), binB->pointsInBin.end());

                            mergedBin.makeDQSB(rawdata, maxdist);

                            redoneBins.push_back(mergedBin);
                            // advance by one so one bin is not merged twice by accident
                            ++i;
                        }
                    }
                }
            }
        }
        if (redoneBins.empty())
        {
            return;
        }

        // there will always be gaps in the data
        size_t i = 0;
        for (Bin newBin : redoneBins) // fill gaps with new bins
        {
            finishedBins[combinedBins[i]] = newBin;
            ++i;
        }
        while (i < combinedBins.size()) // transfer elements from finishedBins
        {
            // volatile int position = combinedBins[i];
            finishedBins[combinedBins[i]] = std::move(finishedBins.back());
            finishedBins.pop_back();
            ++i;
        }

        std::cout << "merged " << combinedBins.size() << " one-sided bins into " << redoneBins.size() << " new bins\n";

        redoneBins.resize(0); // allow other functions to use redoneBins
        return;

        // return conflictOfBinterest; // @todo add a way to work with this
    }

    void BinContainer::printAllBins(std::string path) const
    {
        std::fstream file_out;
        std::stringstream output;
        file_out.open(path, std::ios::out);
        assert(file_out.is_open());

        output << "mz,scan,ID,DQSC,DQSB_scaled\n";

        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            std::vector<qCentroid *> binnedPoints = finishedBins[i].pointsInBin;

            for (size_t j = 0; j < binnedPoints.size(); j++)
            {
                char buffer[128];
                sprintf(buffer, "%0.8f,%d,%zu,%0.8f,%0.8f\n", binnedPoints[j]->mz, binnedPoints[j]->scanNo, i + 1,
                        binnedPoints[i]->DQSCentroid, finishedBins[i].DQSB_scaled[j]);
                output << buffer;
            }
        }
        file_out << output.str();
    }

    std::vector<EIC> BinContainer::returnBins(std::vector<float> convertRT)
    {
        std::vector<EIC> finalBins;
        finalBins.reserve(finishedBins.size());
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finalBins.push_back(finishedBins[i].createEIC(convertRT));
        }
        return finalBins;
    };

    void BinContainer::printSelectBins(bool printCentroids, bool printSummary, std::filesystem::path location, std::string filename)
    {
        // print optional summary file

        if (!std::filesystem::exists(location))
        {
            std::cerr << "Error during summary printing: The selected directory does not exist.\nSupplied path: " << std::filesystem::absolute(location)
                      << "\nCurrent directory: " << std::filesystem::current_path() << "\ncontinuing...\n";
            return;
        }

        if (printSummary)
        {
            std::vector<size_t> indices;
            std::fstream file_out_sum;
            std::stringstream output_sum;
            location /= (filename + "_summary.csv");
            std::cout << "writing summary to: " << location << "\n";
            file_out_sum.open(location, std::ios::out);
            assert(file_out_sum.is_open());
            output_sum << "ID,errorcode,size,mean_mz,median_mz,stdev_mz,mean_scans,DQSB_base,DQSB_scaled,DQSC_min,mean_error\n";
            for (size_t i = 0; i < finishedBins.size(); i++)
            {
                SummaryOutput res = finishedBins[i].summariseBin();
                indices.push_back(i); // save these bins for printing
                char buffer[256];
                sprintf(buffer, "%zu,%d,%zu,%0.8f,%0.8f,%0.8f,%0.2f,%0.6f,%0.6f,%0.6f,%0.8f\n",
                        i + 1, int(res.errorcode), res.binsize, res.mean_mz, res.median_mz, res.stddev_mz,
                        res.mean_scans, res.DQSB_base, res.DQSB_scaled, res.DQSC_min, res.mean_error);
                output_sum << buffer;
            }
            file_out_sum << output_sum.str();
            file_out_sum.close();
        }
        // print all bins
        if (printCentroids)
        {
            std::vector<size_t> indices;
            std::fstream file_out_all;
            std::stringstream output_all;
            location.replace_filename(filename + "_bins.csv");
            std::cout << "writing bins to: " << location << "\n";
            file_out_all.open(location, std::ios::out);
            assert(file_out_all.is_open());
            output_all << "ID,mz,scan,int_area,mzError,DQSC,DQSB_base,DQSB_scaled\n";
            for (size_t i = 0; i < indices.size(); i++)
            {
                const int pos = indices[i];
                std::vector<qCentroid *> binnedPoints = finishedBins[pos].pointsInBin;

                for (size_t j = 0; j < binnedPoints.size(); j++)
                {
                    char buffer[256];
                    sprintf(buffer, "%d,%0.8f,%d,%0.2f,%0.8f,%0.6f,%0.6f,%0.6f\n",
                            pos + 1, binnedPoints[j]->mz, binnedPoints[j]->scanNo,
                            binnedPoints[j]->int_area, binnedPoints[j]->mzError, binnedPoints[j]->DQSCentroid,
                            finishedBins[pos].DQSB_base[j], finishedBins[pos].DQSB_scaled[j]);
                    output_all << buffer;
                }
            }
            file_out_all << output_all.str();
            file_out_all.close();
        }
    };

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin() {};

    Bin::Bin(const std::vector<qCentroid *>::iterator &binStartInOS, const std::vector<qCentroid *>::iterator &binEndInOS) // const std::vector<qCentroid> &sourceList,
    {
        pointsInBin = std::vector<qCentroid *>(binStartInOS, binEndInOS);
    }
    Bin::Bin(CentroidedData *rawdata) // @todo relatively time intensive, better solution?; why does rawdata need to be mutable?
    {
        // collect indices for
        assert(pointsInBin.empty());
        pointsInBin.reserve(rawdata->lengthAllPoints);
        for (size_t i = 1; i < rawdata->allDatapoints.size(); i++)
        {
            for (size_t j = 0; j < rawdata->allDatapoints[i].size(); j++)
            {
                pointsInBin.push_back(&rawdata->allDatapoints[i][j]);
                assert(pointsInBin.back()->scanNo > 0);
                assert(pointsInBin.back()->scanNo < rawdata->allDatapoints.size() + 1);
            }
        }
    }

    void Bin::makeOS()
    {
        assert(pointsInBin.size() > 4);
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const qCentroid *lhs, const qCentroid *rhs)
                  { return lhs->mz < rhs->mz; });

        this->activeOS.reserve(pointsInBin.size());         // OS = Order Space
        for (size_t i = 0; i < pointsInBin.size() - 1; i++) // -1 to prevent accessing outside of vector
        {
            activeOS.push_back((pointsInBin[i + 1]->mz - pointsInBin[i]->mz));
        }

        activeOS.push_back(NAN); // last element of OS is never checked, dummy value due to recursive function
        return;
    }

    void Bin::makeCumError()
    {
        this->cumError.reserve(pointsInBin.size());
        cumError.push_back(0); // the first element has a precursor error of 0
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [](qCentroid *F)
                       { return F->mzError; });
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::subsetMZ(std::deque<Bin> *bincontainer, std::vector<double> &OS,
                       const int binStartInOS, const int binEndInOS, int &counter) // bincontainer is binDeque of BinContainer // OS cannot be solved with pointers since index has to be transferred to frature list
    {
        assert(binStartInOS >= 0);
        assert(binEndInOS >= binStartInOS);

        const int binsizeInOS = binEndInOS - binStartInOS + 1; // +1 to avoid length zero

        assert(binsizeInOS > 4);

        ++counter;
        auto pmax = std::max_element(OS.begin() + binStartInOS, OS.begin() + binEndInOS);

        // double vcrit = 3.05037165842070 * pow(log(binsizeInOS), (TOLERANCE_BINNING)) *  // critical value for alpha = 0.01 @todo add functionality for custom alpha?
        //                (this->cumError[binEndInOS + 1] - this->cumError[binStartInOS]); // + 1 to binEnd since cumerror starts at 0
        double vcrit = (OS_CRIT_A + OS_CRIT_B / std::sqrt(std::log(binsizeInOS))) *  (this->cumError[binEndInOS + 1] - this->cumError[binStartInOS]); // + 1 to binEnd since cumerror starts at 0
        double max = *pmax * binsizeInOS;                                               // moved binsize here since multiplication is faster than division

        [[unlikely]] if (max < vcrit) // all values in range are part of one mz bin
        {
            Bin output(pointsInBin.begin() + binStartInOS, pointsInBin.begin() + binEndInOS + 1); // binEndInOS+1 since the iterator has to point behind the last element to put into the vector
            bincontainer->push_back(output);
            return;
        }
        else
        {
            // the centroid at cutpos is included in the left fragment
            const int cutpos = std::distance(OS.begin() + binStartInOS, pmax);
            // only continue if binsize is greater five
            if (cutpos + 1 > 4)
            {
                subsetMZ(bincontainer, OS, binStartInOS, binStartInOS + cutpos, counter);
            }
            else
            {
                for (int i = 0; i < cutpos + 1; i++)
                {
                    notInBins.push_back(pointsInBin[binStartInOS + i]);
                }
            }
            if (binEndInOS - binStartInOS - cutpos - 1 > 4)
            {
                subsetMZ(bincontainer, OS, binStartInOS + cutpos + 1, binEndInOS, counter);
            }
            else
            {
                for (int i = cutpos + 1; i < binsizeInOS; i++)
                {
                    notInBins.push_back(pointsInBin[binStartInOS + i]);
                }
            }
            return;
        }
    }

    void Bin::subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int maxdist, int &counter)
    {
        assert(!pointsInBin.empty());
        // function is called on a bin sorted by mz
        int control_duplicatesIn = 0;
        const size_t binSize = pointsInBin.size();
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](qCentroid *lhs, qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        std::vector<qCentroid *>::iterator newstart = pointsInBin.begin();
        int lastpos = 0;
        for (size_t i = 0; i < binSize - 1; i++) // -1 since difference to next data point is checked
        {
            assert(pointsInBin[i]->scanNo > 0);
            if (pointsInBin[i + 1]->scanNo < pointsInBin[i]->scanNo)
            {
                // @todo probably undefined behaviour, since taking this print out causes
                // the assert to fail despite the sorting earlier
                std::cerr << pointsInBin[i + 1]->scanNo << ", " << pointsInBin[i]->scanNo << ", " << i << "\n";
            }

            assert(pointsInBin[i + 1]->scanNo >= pointsInBin[i]->scanNo);
            int distanceScan = pointsInBin[i + 1]->scanNo - pointsInBin[i]->scanNo;
            if (distanceScan > maxdist) // bin needs to be split
            {
                ++counter;
                // less than five points in bin
                if (i - lastpos + 1 < 5) // +1 since i starts at 0
                {
                    for (size_t j = lastpos; j <= i; j++)
                    {
                        qCentroid *F = *(pointsInBin.begin() + j);
                        notInBins.push_back(F);
                    }
                }
                else
                {
                    // viable bin, stable in scan dimension
                    // +1 since otherwise last element of the correct range is not included
                    Bin output(newstart, pointsInBin.begin() + i + 1);
                    assert(output.pointsInBin.size() > 4);
                    bincontainer->push_back(output);
                }
                lastpos = i + 1;                          // sets previous i to the position one i ahead, since
                newstart = pointsInBin.begin() + lastpos; // for the next split this is the first element
            }
            else if (distanceScan == 0)
            {
                duplicateScan = true;
                ++control_duplicatesIn;
            }
        }
        // check for open bin at the end
        if (lastpos == 0) // no cut has occurred
        {
            // check for half-peak shape by highest and 2nd highest int_area
            double intMax = 0;
            double intMax2 = 0;
            for (auto cen : pointsInBin)
            {
                if (cen->int_area > intMax)
                {
                    intMax2 = intMax;
                    intMax = cen->int_area;
                }
                else if (cen->int_area > intMax2)
                {
                    intMax2 = cen->int_area;
                }
            }
            bool lslant = false;
            bool rslant = false;
            if ((pointsInBin.front()->int_area == intMax) || (pointsInBin.front()->int_area == intMax2))
            {
                lslant = true;
            }
            else if ((pointsInBin.front() + 1)->int_area == intMax)
            {
                lslant = true;
            }
            if ((pointsInBin.back()->int_area == intMax) || (pointsInBin.back()->int_area == intMax2))
            {
                lslant = true;
            }
            else if ((pointsInBin.back() + 1)->int_area == intMax)
            {
                lslant = true;
            }
            if (lslant && rslant)
            {
                // a bin cannot be slanted to two sides
                lslant = false;
                rslant = false;
            }

            finishedBins->push_back(bincontainer->front());
            finishedBins->back().scanMin = pointsInBin.front()->scanNo;
            finishedBins->back().scanMax = pointsInBin.back()->scanNo;
            finishedBins->back().l_slanted = lslant;
            finishedBins->back().r_slanted = rslant;
            duplicatesTotal += control_duplicatesIn;
        }
        else if (binSize - lastpos > 4) // binsize starts at 1
        {
            // viable bin, stable in scan dimension
            Bin output(newstart, pointsInBin.end());
            assert(output.pointsInBin.size() > 4);
            bincontainer->push_back(output);
        }
        else
        {
            for (size_t j = lastpos; j < pointsInBin.size(); j++)
            {
                qCentroid *F = *(pointsInBin.begin() + j);
                notInBins.push_back(F);
            }
        }
    }

    void Bin::makeDQSB(const CentroidedData *rawdata, const int maxdist) // @todo split this function
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        const size_t binsize = pointsInBin.size();
        DQSB_base.reserve(binsize);
        DQSB_scaled.reserve(binsize);
        // determine start and end of relevant scan section, used as repeats for the for loop
        // borders include relevant values
        int scanRangeStart = scanMin - maxdist;
        int scanRangeEnd = scanMax + maxdist;
        // early abort if the bin covers all scans - this cannot be representative
        if (scanRangeEnd - scanRangeStart > rawdata->allDatapoints.size())
        {
            DQSB_base.assign(binsize, -1);
            DQSB_scaled.assign(binsize, -1);
            return;
        }

        // determine min and max in mz - sort, since then calculation of inner distances is significantly faster
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](qCentroid *lhs, qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });

        float mzMin1 = INFINITY;
        float mzMax1 = 0;
        for (auto cen : pointsInBin)
        {
            if (cen->mz < mzMin1)
            {
                mzMin1 = cen->mz;
            }
            if (cen->mz > mzMax1)
            {
                mzMax1 = cen->mz;
            }
        }
        this->mzMax = mzMax1;
        this->mzMin = mzMin1;

        std::vector<float> meanInnerDistances = meanDistanceRegional(pointsInBin, maxdist);

        bool maxScansReduced = false; // is maxScan + maxdist greater than the largest scan?

        // contains both mz values (per scan) next or closest to all m/z in the bin
        std::vector<float> minMaxOutPerScan;
        // +1 since both values are relevant to the range
        minMaxOutPerScan.reserve((scanMax - scanMin + 1) * 2 + 4 * maxdist);
        [[unlikely]] if (scanRangeStart < 1)
        {
            while (scanRangeStart < 1)
            {
                // fill with dummy values to prevent segfault when distance checker expects negative scan number
                minMaxOutPerScan.push_back(IGNORE);
                minMaxOutPerScan.push_back(IGNORE);
                ++scanRangeStart;
            }
        }
        [[unlikely]] if (scanRangeEnd > int(rawdata->allDatapoints.size() - 1))
        {
            maxScansReduced = true; // dummy values have to be added later
            scanRangeEnd = rawdata->allDatapoints.size() - 1;
        }

        // for all scans relevant to the bin
        int needle = 0; // position in scan where a value was found - starts at 0 for first scan

        for (int i = scanRangeStart; i <= scanRangeEnd; i++)
        {
            bool minFound = false; // only execute search if min or max is present in the scan
            bool maxFound = false;
            const int scansize = rawdata->allDatapoints[i].size() - 1;
            [[unlikely]] if (scansize == -1)
            {
                minMaxOutPerScan.push_back(NO_MIN_FOUND);
                minMaxOutPerScan.push_back(NO_MAX_FOUND);
                continue;
            }
            else [[unlikely]] if (needle > scansize)
            {
                needle = scansize; // prevent searching outside of the valid scan region
            }
            // check if the bin is outside the region this scan covers
            if (rawdata->allDatapoints[i][0].mz >= mzMin)
            {
                minMaxOutPerScan.push_back(NO_MIN_FOUND);
                minFound = true;
                if (rawdata->allDatapoints[i][0].mz > mzMax)
                {
                    minMaxOutPerScan.push_back(rawdata->allDatapoints[i][0].mz);
                    maxFound = true;
                    continue;
                }
                else
                {
                    needle = 0;
                }
            }
            // check end of bin
            // double lastmz = rawdata->allDatapoints[i][scansize].mz;
            if (rawdata->allDatapoints[i][scansize].mz <= mzMax)
            {
                minMaxOutPerScan.push_back(NO_MAX_FOUND);
                maxFound = true;
                if (rawdata->allDatapoints[i][scansize].mz < mzMin)
                {
                    minMaxOutPerScan.push_back(rawdata->allDatapoints[i][scansize].mz);
                    minFound = true;
                    continue;
                }
                else
                {
                    needle = scansize;
                }
            }
            // rawdata is always sorted by mz within scans
            // use two while loops to find desired value from any place in
            // the scan, accounts for large shifts between scans
            [[likely]] if (!minFound)
            {
                while (rawdata->allDatapoints[i][needle].mz < mzMin)
                {
                    ++needle; // steps through the dataset and increments until needle is the first value >= mzMin
                }
                while (rawdata->allDatapoints[i][needle].mz >= mzMin)
                {
                    --needle; // steps through the dataset and decrements until needle is the desired mz value
                }
                minMaxOutPerScan.push_back(rawdata->allDatapoints[i][needle].mz);
            }

            [[likely]] if (!maxFound)
            {
                while (rawdata->allDatapoints[i][needle].mz > mzMax)
                {
                    --needle;
                }
                while (rawdata->allDatapoints[i][needle].mz <= mzMax)
                {
                    ++needle;
                }
                minMaxOutPerScan.push_back(rawdata->allDatapoints[i][needle].mz);
            }
        }
        // minMaxOutPerScan contains the relevant points in order of scans,
        // for every scan the two closest values from both ends of the bin
        if (maxScansReduced) // add dummy values after last scan
        {
            while (minMaxOutPerScan.capacity() != minMaxOutPerScan.size())
            {
                minMaxOutPerScan.push_back(IGNORE);
            }
        }

        // calculate critical DQS for finding points that are within the critical distance in mz and maxdist scans
        float meanerror = std::accumulate(pointsInBin.begin(), pointsInBin.end(), 0.0, [](float error, const qCentroid *point)
                                          { return error + point->mzError; }) /
                          binsize;
        // float vcrit = 3.05037165842070 * pow(log(binsize + 1), (TOLERANCE_BINNING)) * meanerror;
        // binsize + 1 to not include points which would be removed after adding them
        // vcrit has the same scaling as mz of bin centroids
        float vcrit = (OS_CRIT_A + OS_CRIT_B / std::sqrt(std::log(binsize + 1))) *  meanerror; // + 1 to binEnd since cumerror starts at 0

        // find min distance in minMaxOutPerScan, then calculate DQS for that point
        for (size_t i = 0; i < binsize; i++)
        {
            float minDist_base = INFINITY;
            float minDist_scaled = INFINITY;
            float currentMZ = pointsInBin[i]->mz;

            // currentRangeStart gives position of the first value in minMaxOutPerScan that must be considered,
            // assuming the first value in minMaxOutPerScan (index 0) is only relevant to the
            // qCentroid in the lowest scan. For every increase in scans, that range starts two elements later
            const int currentRangeStart = (pointsInBin[i]->scanNo - scanMin) * 2;
            const int currentRangeEnd = currentRangeStart + maxdist * 4 + 1; // +1 removed

            assert(size_t(currentRangeEnd) < minMaxOutPerScan.size());

            for (int j = currentRangeStart; j < currentRangeEnd; j++)
            // from lowest scan to highest scan relevant to this
            // point, +1 since scan number of point has to be included.
            {
                float dist = std::abs(currentMZ - minMaxOutPerScan[j]);
                if (dist < minDist_base)
                {
                    minDist_base = dist;
                }
                float scaledDist = dist * scalarForMOD[j - currentRangeStart];
                if (scaledDist < minDist_scaled) // scaling is calculated by a separate function
                {
                    minDist_scaled = scaledDist;
                }
            }

            DQSB_base.push_back(calcDQS(meanInnerDistances[i], minDist_base));
            DQSB_scaled.push_back(calcDQS(meanInnerDistances[i], minDist_scaled));

            /// section to check hot ends ///
            // if the DQS at either end is less than critDQS, a point exists that could be included in the bin
            if (i == 0) // first element of the bin when sorted by mz
            {
                float critDQS = calcDQS(meanInnerDistances[i], vcrit);
                if (critDQS > DQSB_base[i])
                {
                    l_maxdist_tooclose = true;
                }
            }
            if (i == binsize - 1) // last element of the bin when sorted by mz
            {
                float critDQS = calcDQS(meanInnerDistances[i], vcrit);
                if (critDQS > DQSB_base[i])
                {
                    r_maxdist_tooclose = true;
                }
            }
        }
        return;
    }

    float Bin::calcStdevMZ()
    {
        float meanMZ = 0;
        for (size_t i = 0; i < pointsInBin.size(); i++)
        {
            meanMZ += pointsInBin[i]->mz;
        }

        meanMZ = meanMZ / pointsInBin.size();
        float sumOfDist = 0;
        std::for_each(pointsInBin.begin(), pointsInBin.end(), [&](const qCentroid *point)
                      { sumOfDist += (point->mz - meanMZ) * (point->mz - meanMZ); }); // squared distance to mean

        return sqrt(sumOfDist / (pointsInBin.size() - 1));
    }

    SummaryOutput Bin::summariseBin()
    {
        size_t binsize = pointsInBin.size();

        float meanMZ = 0;
        float meanScan = 0;
        float meanDQS_base = 0;
        float meanDQS_scaled = 0;
        float meanCenError = 0;
        float worstCentroid = INFINITY;
        float meanInt = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            meanMZ += pointsInBin[i]->mz;
            meanInt += pointsInBin[i]->int_area;
            meanScan += pointsInBin[i]->scanNo;
            meanCenError += pointsInBin[i]->mzError;
            if (pointsInBin[i]->DQSCentroid < worstCentroid)
            {
                worstCentroid = pointsInBin[i]->DQSCentroid;
            }

            meanDQS_base += DQSB_base[i];
            meanDQS_scaled += DQSB_scaled[i];
        }

        meanMZ = meanMZ / binsize;
        meanInt = meanInt / binsize;
        meanScan = meanScan / binsize;
        meanDQS_base = meanDQS_base / binsize;
        meanDQS_scaled = meanDQS_scaled / binsize;
        meanCenError = meanCenError / binsize;

        float sumOfDist = 0;
        std::for_each(pointsInBin.begin(), pointsInBin.end(), [&](const qCentroid *point)
                      { sumOfDist += (point->mz - meanMZ) * (point->mz - meanMZ); }); // squared distance to mean

        float stdevMZ = sqrt(sumOfDist / (binsize - 1));

        std::sort(pointsInBin.begin(), pointsInBin.end(), [](qCentroid *lhs, qCentroid *rhs)
                  { return lhs->int_area < rhs->int_area; });

        // is the first or last scan the highest int_area?
        // these bins cannot be fitted with a gaussian peak
        bool oneSided = false;
        if ((pointsInBin.back()->scanNo == scanMin) || (pointsInBin.back()->scanNo == scanMax))
        {
            oneSided = true;
        }
        // added this part since the smallest scale doesn't fit without interpolating here
        // else if ((pointsInBin.back()->scanNo == scanMin + 1) || (pointsInBin.back()->scanNo == scanMax - 1))
        // {
        //     oneSided = true;
        // }
        // else if ((pointsInBin[binsize - 2]->scanNo == scanMin) || (pointsInBin[binsize - 2]->scanNo == scanMax))
        // {
        //     oneSided = true;
        // }

        // the second and third highest int_area should each be
        // within maxdist of the highest instensity
        bool twoMaxima = false;

        int scanOfMax = pointsInBin.back()->scanNo;
        int scanOf2nd = pointsInBin[binsize - 2]->scanNo;
        int scanOf3rd = pointsInBin[binsize - 3]->scanNo;
        if ((abs(scanOfMax - scanOf2nd) > maxdist) || (abs(scanOfMax - scanOf3rd) > maxdist))
        {
            twoMaxima = true;
        }

        // iterate to calculate stdev of mz and int_area
        // identify the largest difference between ordered
        // intensities and compare to vcrit of int_area later
        // double mzErrorSquared = 0;
        float intensityErrorSquared = 0;
        float greatestIntGap = 0;
        // count the occurences of points outside the 1.3 sigma interval - should be 20% or less
        // 1.3 sigma was chosen to have a tolerance of one point for bins of size five
        bool toobroad = false;
        size_t noSigma = 0;
        float mzSigmaLower = meanMZ - stdevMZ * 1.3;
        float mzSigmaUpper = meanMZ + stdevMZ * 1.3;
        for (size_t i = 0; i < binsize; i++)
        {
            // count occurences of points outside of 1.3 sigma interval
            if ((pointsInBin[i]->mz > mzSigmaLower) && (pointsInBin[i]->mz < mzSigmaUpper))
            {
                ++noSigma;
            }
            // make squared error of mz and int_area, find largest gap between intensities
            intensityErrorSquared += (meanInt - pointsInBin[i]->int_area) * (meanInt - pointsInBin[i]->int_area);
            if (i < binsize - 1)
            {
                float intDiff = pointsInBin[i + 1]->int_area - pointsInBin[i]->int_area;
                if (intDiff > greatestIntGap)
                {
                    greatestIntGap = intDiff;
                }
            }
        }
        // const float vcritIntensity = 3.05037165842070 * pow(log(binsize), (TOLERANCE_BINNING)) *
        //                              sqrt(intensityErrorSquared / (binsize - 1));
        const float vcritIntensity = (OS_CRIT_A + OS_CRIT_B / std::sqrt(std::log(binsize))) *  sqrt(intensityErrorSquared / (binsize - 1));
        bool intensityOutlier = false;
        if (greatestIntGap > vcritIntensity)
        {
            intensityOutlier = true;
        }

        if (noSigma < (binsize * 4) / 5) // if less than 80% of the bin are within 1.3 sigma
        {
            toobroad = true;
        }

        bool halfPeakL = true;
        int prevScan = 0;
        int wrongCount = 0;
        for (qCentroid *cen : pointsInBin)
        {
            if (cen->scanNo > prevScan)
            {
                prevScan = cen->scanNo;
                wrongCount = 0;
            }
            else
            {
                ++wrongCount;
            }
            if (wrongCount > 1)
            {
                halfPeakL = false;
                break;
            }
        }

        bool halfPeakR = true;
        if (!halfPeakL)
        {
            prevScan = 2147483647; // max possible value of int
            wrongCount = 0;
            for (qCentroid *cen : pointsInBin)
            {
                if (cen->scanNo < prevScan)
                {
                    prevScan = cen->scanNo;
                    wrongCount = 0;
                }
                else
                {
                    ++wrongCount;
                }
                if (wrongCount > 1)
                {
                    halfPeakR = false;
                    break;
                }
            }
        }

        // @todo vcrit greater true vcrit
        // @todo intensities are all the same
        std::byte selector{0b00000000}; // used as bitmask during printSelectBins()
        if (duplicateScan)
        {
            selector |= std::byte{0b00000001};
        }
        if (l_maxdist_tooclose || r_maxdist_tooclose)
        {
            selector |= std::byte{0b00000010};
        }
        if (halfPeakL || halfPeakR)
        {
            selector |= std::byte{0b00000100};
        }
        // if (abs(meanMZ - medianMZ) > 2 * meanCenError) // standard error instead? @todo median calculation above
        // {
        //     selector |= std::byte{0b00001000};
        // }
        // middle third of total mz should be at least a third of total points
        if (oneSided)
        {
            selector |= std::byte{0b00010000}; // only check for max at the edges
        }
        if (intensityOutlier)
        {
            selector |= std::byte{0b00100000};
        }
        if (false) // twoMaxima mostly uniform increase in int_area over bin
        {
            selector |= std::byte{0b01000000};
        }
        // if ((meanMZ + 3 * stdevMZ < r_maxdist_abs) || (meanMZ - 3 * stdevMZ > l_maxdist_abs)) // if a value in the bin is outside of 3 sigma
        if (toobroad)
        {
            selector |= std::byte{0b10000000};
        }

        this->errorcode = selector;

        // return SummaryOutput{selector, binsize, meanMZ, medianMZ, stdevMZ, meanScan, meanDQS_base,
        //                      meanDQS_scaled, worstCentroid, meanCenError};
        return SummaryOutput{selector, binsize, meanMZ, -1, stdevMZ, meanScan, meanDQS_base,
                             meanDQS_scaled, worstCentroid, meanCenError};
    }

    EIC Bin::createEIC(std::vector<float> convertRT)
    {
        int eicsize = pointsInBin.size();

        std::byte bincode = this->summariseBin().errorcode; // this step contains sorting by scans for the time being @todo
        std::vector<int> tmp_scanNumbers;
        tmp_scanNumbers.reserve(eicsize);
        std::vector<float> tmp_rt;
        tmp_rt.reserve(eicsize);
        std::vector<float> tmp_mz;
        tmp_mz.reserve(eicsize);
        std::vector<float> tmp_ints_area;
        tmp_ints_area.reserve(eicsize);
        std::vector<float> tmp_ints_height;
        tmp_ints_height.reserve(eicsize);
        std::vector<float> tmp_DQSB;
        tmp_DQSB.reserve(eicsize);
        std::vector<float> tmp_DQSC;
        tmp_DQSC.reserve(eicsize);

        std::sort(pointsInBin.begin(), pointsInBin.end(), [](qCentroid *lhs, qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });

        int prevScan = 0;   // scan 0 is always empty
        double prevInt = 0; // for pre-centroided data
        for (size_t i = 0; i < pointsInBin.size(); i++)
        {

            qCentroid *point = pointsInBin[i];
            if (point->scanNo == prevScan)
            {
                if (point->DQSCentroid == tmp_DQSC.back())
                {
                    if (point->int_area < prevInt)
                    {
                        continue;
                    }
                    else
                    {
                        tmp_scanNumbers.pop_back();
                        tmp_rt.pop_back();
                        tmp_mz.pop_back();
                        tmp_ints_area.pop_back();
                        tmp_ints_height.pop_back();
                        tmp_DQSB.pop_back();
                        tmp_DQSC.pop_back();
                    }
                }

                // for duplicates, remove the centroid with the
                // worse DQSC. This decision was made based on
                // limited observations in four different real datasets
                if (point->DQSCentroid < tmp_DQSC.back())
                {
                    continue;
                }
                else
                {
                    tmp_scanNumbers.pop_back();
                    tmp_rt.pop_back();
                    tmp_mz.pop_back();
                    tmp_ints_area.pop_back();
                    tmp_ints_height.pop_back();
                    tmp_DQSB.pop_back();
                    tmp_DQSC.pop_back();
                }
            }
            if (!tmp_scanNumbers.empty())
            {
                assert(tmp_scanNumbers.back() < point->scanNo);
            }

            prevScan = point->scanNo;
            prevInt = point->int_area;
            tmp_scanNumbers.push_back(prevScan);
            tmp_rt.push_back(convertRT[prevScan]);
            tmp_mz.push_back(point->mz);
            tmp_ints_area.push_back(point->int_area);
            tmp_ints_height.push_back(point->int_height);
            tmp_DQSB.push_back(DQSB_base[i]);
            tmp_DQSC.push_back(point->DQSCentroid);
        }

        tmp_scanNumbers.shrink_to_fit();
        tmp_rt.shrink_to_fit();
        tmp_mz.shrink_to_fit();
        tmp_ints_area.shrink_to_fit();
        tmp_ints_height.shrink_to_fit();
        tmp_DQSB.shrink_to_fit();
        tmp_DQSC.shrink_to_fit();

        EIC returnVal = {
            bincode,
            tmp_scanNumbers,
            tmp_rt,
            tmp_mz,
            tmp_ints_area,
            tmp_ints_height,
            tmp_DQSB,
            tmp_DQSC};

        return returnVal;
    }

#pragma endregion "Bin"

#pragma region "Functions"

    static std::vector<float> meanDistanceRegional(const std::vector<qCentroid *> pointsInBin, int maxdist)
    {
        // the other mean distance considers all points in the Bin.
        // It is sensible to only use the mean distance of all points
        // within maxdist scans
        // this function assumes the bin to be sorted by scans
        const size_t binsize = pointsInBin.size();
        std::vector<float> output(binsize);
        size_t position = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            int scanRegionStart = pointsInBin[i]->scanNo - maxdist; // can be negative
            int scanRegionEnd = pointsInBin[i]->scanNo + maxdist + 1;
            float accum = 0;
            while (pointsInBin[position]->scanNo < scanRegionStart)
            {
                position++;
            }
            size_t readPos = position;
            while (pointsInBin[readPos]->scanNo < scanRegionEnd)
            {
                accum += abs(pointsInBin[readPos]->mz - pointsInBin[i]->mz);
                readPos++;
                if (readPos == binsize)
                {
                    break;
                }
            }
            output[i] = accum / float(readPos - position - 1); // -1 since the distance of an element to itself is not factored in
        }
        return output;
    }

    static inline float calcDQS(float MID, float MOD) // mean inner distance, minimum outer distance
    {
        float dqs(MOD); // MOD should always be less than MID, less cache misses
        [[unlikely]] if (dqs < MID)
        {
            dqs = MID;
        }
        // dqs = (MOD - MID) * (1 / (1 + MID)) / dqs
        return (MOD - MID) / fmal(MID, dqs, dqs);
    }

    static void scaleDistancesForDQS_linear(int maxdist) // @experimental
    {
        // Assumption: every point which is found in a neighbouring scan, but not the
        // current one, has a 50% chance of existing. As such, it can only be half
        // as relevant as another point in the same scan. To scale, A distance in the
        // same scan is multiplied by one, one further by two, then by four etc.
        scalarForMOD.resize(maxdist * 4 + 2); // every value is doubled, same length as selected mz vector during DQS calculation
        float scalarSingle = pow(2, maxdist);
        for (int i = 0; i < 2 * maxdist + 1; i++) // scalarForMod contains the correct scaling to be used on the MOD for any maxdist
        {
            scalarForMOD[i] = scalarSingle;
            scalarForMOD[i + 1] = scalarSingle;
            scalarForMOD[scalarForMOD.size() - 1 - i] = scalarSingle;
            scalarForMOD[scalarForMOD.size() - 2 - i] = scalarSingle;
            i++;
            scalarSingle /= 2;
        }
    }

#pragma endregion "Functions"

    std::vector<EIC> performQbinning(CentroidedData centroidedData, std::vector<float> convertRT, std::filesystem::path outpath,
                                     std::string filename, int maxdist, bool silent, bool printBinSummary, bool printCentroids)
    {
        std::streambuf *old = std::cout.rdbuf(); // save standard out config @todo change this
        std::stringstream ss;

        if (silent) // @todo change this to a global variable
        {
            std::cout.rdbuf(ss.rdbuf()); // @todo this is a very bad idea
        }

        std::cout << "starting binning process...\n";

        notInBins.clear();                                     // @todo remove global variable
        notInBins.reserve(centroidedData.lengthAllPoints / 4); // @todo can this be estimated better?
        duplicatesTotal = 0;                                   // global variable
        BinContainer activeBins;
        activeBins.makeFirstBin(&centroidedData);
        // at least one element must be terminator @todo make this a function parameter
        std::vector<int> measurementDimensions = {SubsetMethods::mz, SubsetMethods::scans};
        activeBins.subsetBins(measurementDimensions, maxdist, false);

        if (activeBins.finishedBins.size() < 1)
        {
            // no bins were found -> critical error in the program
            // this case was observed when reading in pure noise, which contained no signals
            std::cout.rdbuf(old); // restore previous standard out
            return std::vector<EIC>{};
        }

        std::cout << "Total duplicates: " << duplicatesTotal << "\n--\ncalculating DQSBs...\n";

        // this sets q::scalarForMOD to the correct scaling so that at the
        // distance in scans == maxdist the curve encloses 99% of its area
        scaleDistancesForDQS_linear(maxdist);
        activeBins.assignDQSB(&centroidedData, maxdist, false);

        // move centroids from notInBins into existing bins
        activeBins.redoBinningIfTooclose(measurementDimensions, &centroidedData, notInBins, maxdist);
        // activeBins.reconstructFromStdev(&centroidedData, maxdist); // this part of the binning process has been removed due to lacking statistical foundation

        // @todo add bin merger for halved bins here

        if (printBinSummary || printCentroids)
        {
            activeBins.printSelectBins(printCentroids, printBinSummary, outpath, filename);
        }

        notInBins.clear();

        std::cout.rdbuf(old); // restore previous standard out

        auto test = activeBins.returnBins(convertRT);
        return test; // @todo do not calculate the summary twice when printing bins
    }

}
