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
    // static std::vector<double> meanDistance(const std::vector<const qCentroid *> pointsInBin);

    /// @brief calculate the mean distance in mz to all other close elements of a sorted vector for one element
    /// @param pointsInBin vector of data points sorted by scans
    /// @param maxdist maximum distance in scans for similar points
    /// @return vector of the mean inner distances for all elements in the same order as pointsInBin
    static std::vector<float> meanDistanceRegional(const std::vector<const qCentroid *> pointsInBin, size_t maxdist);

    /// @brief calculate the data quality score as described by Reuschenbach et al. for one datapoint in a bin
    /// @param MID mean inner distance in mz to all other elements in the bin
    /// @param MOD minimum outer distance - the shortest distance in mz to a data point that is within maxdist and not in the bin
    /// @return the data quality score for the specified element
    static inline float calcDQS(const float MID, const float MOD); // Mean Inner Distance, Minimum Outer Distance

    // static void scaleDistancesForDQS_gauss(size_t maxdist); // removed

    static std::vector<float> scaleDistancesForDQS_linear(size_t maxdist); // @experimental

#define IGNORE -256 // nonsense value if no real number exists. Must be
                    // negative since it is used later when searching for
                    // the smallest distance. It is not set to -Inf so that
                    // the resulting DQS is still numeric.
#define NO_MIN_FOUND -INFINITY
#define NO_MAX_FOUND -INFINITY

    const double binningCritVal(size_t n, double stdDev)
    {
        return (OS_CRIT_A + (OS_CRIT_B / std::sqrt(std::log(n + 1)))) * stdDev;
    }

#pragma endregion "misc"

#pragma region "BinContainer"

    BinContainer initialiseBinning(const CentroidedData *rawdata)
    {
        BinContainer bincontainer;
        Bin firstBin;
        firstBin.pointsInBin.reserve(rawdata->lengthAllPoints);
        for (size_t i = 0; i < rawdata->allDatapoints.size(); i++)
        {
            size_t points = rawdata->allDatapoints[i].size();
            for (size_t j = 0; j < points; j++)
            {
                firstBin.pointsInBin.push_back(&(rawdata->allDatapoints[i][j]));
            }
        }
        bincontainer.processBinsF.push_back(firstBin);
        return bincontainer;
    }

    void switchTarget(BinContainer *bincontainer)
    {
        bincontainer->sourceBins->clear();
        // switch target and source array
        bincontainer->readFrom = !bincontainer->readFrom;
        if (bincontainer->readFrom)
        {
            bincontainer->sourceBins = &bincontainer->processBinsT;
            bincontainer->targetBins = &bincontainer->processBinsF;
        }
        else
        {
            bincontainer->sourceBins = &bincontainer->processBinsF;
            bincontainer->targetBins = &bincontainer->processBinsT;
        }
    }

    std::string subsetBins(BinContainer &bincontainer, const size_t maxdist)
    {
        // auto timeStart = std::chrono::high_resolution_clock::now();
        // auto timeEnd = std::chrono::high_resolution_clock::now();
        std::string logOutput = "Binning Start:\n";
        bincontainer.readFrom = false; // starting bin is in processBinsF
        assert(bincontainer.processBinsT.empty());
        assert(!bincontainer.processBinsF.empty());

        bincontainer.sourceBins = &bincontainer.processBinsF;
        bincontainer.targetBins = &bincontainer.processBinsT;

        while (!(bincontainer.processBinsF.empty() && bincontainer.processBinsT.empty()))
        {
            // loop until all bins have been moved into finsihedBins or discarded
            // the target array is supplied via pointer, facilitates reuse

            int subsetCount = 0;

            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++) // for every element in the deque before writing new bins
            {
                Bin processThis = (*bincontainer.sourceBins)[j];
                processThis.makeOS();
                processThis.makeCumError();
                processThis.subsetMZ(bincontainer.targetBins, processThis.activeOS, bincontainer.notInBins,
                                     0, processThis.activeOS.size() - 1, subsetCount);
            }
            // @todo logging

            switchTarget(&bincontainer);

            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++)
            {
                (*bincontainer.sourceBins)[j].subsetScan(bincontainer.targetBins, bincontainer.notInBins, maxdist, subsetCount);
            }
            // @todo logging
            switchTarget(&bincontainer);

            // if the "unchanged" property of a bin is true, all selected tests have passed
            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++)
            {
                if ((*bincontainer.sourceBins)[j].unchanged)
                {
                    bincontainer.viableBins.push_back((*bincontainer.sourceBins)[j]);
                }
                else
                {
                    bincontainer.targetBins->push_back((*bincontainer.sourceBins)[j]);
                }
            }
            switchTarget(&bincontainer);
        }
        bincontainer.processBinsF.clear();
        bincontainer.processBinsT.clear();
        // reset to input condition
        bincontainer.readFrom = false;
        return logOutput;
    }

    void deduplicateBin(std::vector<Bin> *target, std::vector<const qCentroid *> *notInBins, Bin bin)
    {
        assert(bin.duplicateScan);
        assert(bin.medianMZ > 1);
        std::sort(bin.pointsInBin.begin(), bin.pointsInBin.end(), [](const qCentroid *lhs, const qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        Bin returnBin;
        returnBin.pointsInBin.reserve(bin.pointsInBin.size());
        size_t duplicateRemovedCount = 0;
        // add dummy centroid to the end of the bin
        qCentroid dummy;
        dummy.mz = 0;
        dummy.scanNo = 0;
        bin.pointsInBin.push_back(&dummy); // pointer will be not be invalidated since bin is copied
        for (size_t i = 1; i < bin.pointsInBin.size(); i++)
        {
            if (bin.pointsInBin[i]->scanNo == bin.pointsInBin[i - 1]->scanNo)
            {
                double left = abs(bin.medianMZ - bin.pointsInBin[i - 1]->mz);
                double right = abs(bin.medianMZ - bin.pointsInBin[i]->mz);
                if (left > right)
                {
                    // if this is true, the value at position i should be selected
                    // since i-1 is always added to the bin in the default case,
                    // it suffices to move the removed centroid into notInBins
                    notInBins->push_back(bin.pointsInBin[i - 1]);
                }
                else
                {
                    // the value at position i - 1 is correct, but position i
                    // must be added to the vector of discarded points.
                    // i is advanced by onr so the next point that will be added to
                    // the bin is i + 1
                    returnBin.pointsInBin.push_back(bin.pointsInBin[i - 1]);
                    notInBins->push_back(bin.pointsInBin[i]);
                    i++;
                }
                duplicateRemovedCount++;
            }
            else
            {
                returnBin.pointsInBin.push_back(bin.pointsInBin[i - 1]);
            }
        }
        if (returnBin.pointsInBin.size() < 5)
        {
            for (size_t i = 0; i < returnBin.pointsInBin.size(); i++)
            {
                notInBins->push_back(returnBin.pointsInBin[i]);
            }
            return;
        }
        assert(returnBin.pointsInBin.size() + duplicateRemovedCount == bin.pointsInBin.size() - 1);
        target->push_back(returnBin);
    }

    void removeMassJumps(std::vector<Bin> *target, std::vector<const qCentroid *> *notInBins, Bin bin)
    {
        // if the distance in mz between two points is too great, the violating point
        // should be removed.
    }

    bool binLimitsOK(Bin sourceBin, const CentroidedData *rawdata, const size_t maxdist)
    {
        // check if a point within maxdist is closer than the critical value
        // in every scan, the relevant points are outside of the bin limits but inside
        // the limits + the critical distance of a bin with n+1 members
        size_t binsize = sourceBin.pointsInBin.size();
        double binError = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            binError += sourceBin.pointsInBin[i]->mzError;
        }
        binError /= binsize;
        double vcrit = binningCritVal(binsize + 1, binError);
        double lowerLimitMZ = sourceBin.mzMin - vcrit;
        double upperLimitMZ = sourceBin.mzMax + vcrit;

        for (int scan = sourceBin.scanMin; scan < sourceBin.scanMax + 1; scan++)
        {
            auto currentScans = &rawdata->allDatapoints[scan];
            size_t position = 0;
            // check the entire array up to the bin region for possible missed inclusions
            while ((*currentScans)[position].mz < sourceBin.mzMin)
            {
                if ((*currentScans)[position].mz > lowerLimitMZ)
                {
                    return false;
                }
                position++;
                // stop access outside of array
                if (position == currentScans->size())
                {
                    position--;
                    break;
                }
            }
            // conditions switch, now the points of interest are smaller than the limit and greater than the bin border
            while ((*currentScans)[position].mz < upperLimitMZ)
            {
                if ((*currentScans)[position].mz > sourceBin.mzMax)
                {
                    return false;
                }
                position++;
                // stop access outside of array
                if (position == currentScans->size())
                {
                    break;
                }
            }
            // there are no points which should be part of the bin, but are not
        }
        return true;
    }

    int selectRebin(BinContainer *bins, const CentroidedData *rawdata, const size_t maxdist)
    {
        // move all bins without binning artefacts into finishedBins vector
        assert(bins->processBinsF.size() > 0);
        assert(!bins->readFrom);
        int dissolvedCount = 0;
        for (size_t i = 0; i < bins->viableBins.size(); i++)
        {
            Bin currentBin = bins->viableBins[i];

            if (binLimitsOK(currentBin, rawdata, maxdist))
            {
                bins->finalBins.push_back(currentBin);
            }
            else
            {
                dissolvedCount++;
                for (const qCentroid *cen : currentBin.pointsInBin)
                {
                    bins->processBinsF.front().pointsInBin.push_back(cen);
                }
            }
        }
        bins->viableBins.clear();
        return dissolvedCount;
    }

    void reconstructFromStdev(const CentroidedData *rawdata, size_t maxdist)
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

        // std::cout << "merged " << combinedBins.size() << " one-sided bins into " << redoneBins.size() << " new bins\n";

        // redoneBins.resize(0); // allow other functions to use redoneBins
        // return;
        // return conflictOfBinterest; // @todo add a way to work with this
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin() {};

    Bin::Bin(const std::vector<const qCentroid *>::iterator &binStartInOS, const std::vector<const qCentroid *>::iterator &binEndInOS) // const std::vector<qCentroid> &sourceList,
    {
        pointsInBin = std::vector<const qCentroid *>(binStartInOS, binEndInOS);
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
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [](const qCentroid *F)
                       { return F->mzError; });
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::subsetMZ(std::vector<Bin> *bincontainer, std::vector<double> &OS, std::vector<const qCentroid *> &notInBins,
                       const int binStartInOS, const int binEndInOS, int &counter) // bincontainer is binDeque of BinContainer_old // OS cannot be solved with pointers since index has to be transferred to frature list
    {
        assert(binStartInOS >= 0);
        assert(binEndInOS >= binStartInOS);

        const int binsizeInOS = binEndInOS - binStartInOS + 1; // +1 to avoid length zero

        assert(binsizeInOS > 4);

        ++counter;
        auto pmax = std::max_element(OS.begin() + binStartInOS, OS.begin() + binEndInOS);

        // double vcrit = 3.05037165842070 * pow(log(binsizeInOS), (TOLERANCE_BINNING)) *  // critical value for alpha = 0.01 @todo add functionality for custom alpha?
        //                (this->cumError[binEndInOS + 1] - this->cumError[binStartInOS]); // + 1 to binEnd since cumerror starts at 0
        // double max = *pmax * binsizeInOS;                                               // moved binsize here since multiplication is faster than division
        double vcrit = binningCritVal(binsizeInOS, (this->cumError[binEndInOS + 1] - this->cumError[binStartInOS]) / binsizeInOS);
        double max = *pmax;

        if (max < vcrit) // all values in range are part of one mz bin
        {
            Bin output(pointsInBin.begin() + binStartInOS, pointsInBin.begin() + binEndInOS + 1); // binEndInOS+1 since the iterator has to point behind the last element to put into the vector
            output.mzMin = output.pointsInBin.front()->mz;
            output.mzMax = output.pointsInBin.back()->mz;
            output.unchanged = true;
            output.medianMZ = output.pointsInBin[output.pointsInBin.size() / 2]->mz;
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
                subsetMZ(bincontainer, OS, notInBins, binStartInOS, binStartInOS + cutpos, counter);
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
                subsetMZ(bincontainer, OS, notInBins, binStartInOS + cutpos + 1, binEndInOS, counter);
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

    void Bin::subsetScan(std::vector<Bin> *bincontainer, std::vector<const qCentroid *> &notInBins, const size_t maxdist, int &counter)
    {
        assert(!pointsInBin.empty());
        // function is called on a bin sorted by mz
        const size_t binSize = pointsInBin.size();
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const qCentroid *lhs, const qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        std::vector<const qCentroid *>::iterator newstart = pointsInBin.begin();
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
            size_t distanceScan = pointsInBin[i + 1]->scanNo - pointsInBin[i]->scanNo;
            if (distanceScan > maxdist) // bin needs to be split
            {
                ++counter;
                // less than five points in bin
                if (i - lastpos + 1 < 5) // +1 since i starts at 0
                {
                    for (size_t j = lastpos; j <= i; j++)
                    {
                        const qCentroid *F = *(pointsInBin.begin() + j);
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
                this->duplicateScan = true;
            }
        }
        // check for open bin at the end
        if (lastpos == 0)
        {
            // no cut has occurred, the bin is viable
            this->scanMin = pointsInBin.front()->scanNo;
            this->scanMax = pointsInBin.back()->scanNo;
            this->unchanged = true;
            bincontainer->push_back(*this);
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
                const qCentroid *F = *(pointsInBin.begin() + j);
                notInBins.push_back(F);
            }
        }
    }

    void Bin::makeDQSB(const CentroidedData *rawdata, const std::vector<float> scalarForMOD, const size_t maxdist) // @todo split this function
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        // @todo it could be a good idea to only relate the score to points that are not in bins
        const size_t binsize = pointsInBin.size();
        DQSB_base.reserve(binsize);
        DQSB_scaled.reserve(binsize);
        // determine start and end of relevant scan section, used as repeats for the for loop
        // borders include relevant values
        int scanRangeStart = scanMin - maxdist;
        int scanRangeEnd = scanMax + maxdist;
        assert(scanRangeEnd - scanRangeStart > 0);
        size_t coverage = scanRangeEnd - scanRangeStart;
        // early abort if the bin covers all scans - this cannot be representative
        if (coverage > rawdata->allDatapoints.size())
        {
            DQSB_base.assign(binsize, -1);
            DQSB_scaled.assign(binsize, -1);
            return;
        }

        // determine min and max in mz - sort, since then calculation of inner distances is significantly faster
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const qCentroid *lhs, const qCentroid *rhs)
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
        double vcrit = binningCritVal(binsize, meanerror);

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

    // SummaryOutput Bin::summariseBin(size_t maxdist) // @todo rework this
    // {
    //     size_t binsize = pointsInBin.size();
    // }

    EIC Bin::createEIC(std::vector<float> convertRT, size_t maxdist)
    {
        int eicsize = pointsInBin.size();

        // std::byte bincode = this->summariseBin(maxdist).errorcode; // this step contains sorting by scans for the time being @todo
        std::vector<unsigned int> tmp_scanNumbers;
        tmp_scanNumbers.reserve(eicsize);
        std::vector<float> tmp_rt;
        tmp_rt.reserve(eicsize);
        std::vector<float> tmp_mz;
        tmp_mz.reserve(eicsize);
        std::vector<float> tmp_predInterval;
        tmp_predInterval.reserve(eicsize);
        std::vector<float> tmp_ints_area;
        tmp_ints_area.reserve(eicsize);
        std::vector<float> tmp_ints_height;
        tmp_ints_height.reserve(eicsize);
        std::vector<unsigned int> tmp_df;
        tmp_df.reserve(eicsize);
        std::vector<float> tmp_DQSC;
        tmp_DQSC.reserve(eicsize);

        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const qCentroid *lhs, const qCentroid *rhs)
                  { return lhs->scanNo < rhs->scanNo; });

        for (size_t i = 0; i < pointsInBin.size(); i++)
        {
            const qCentroid *point = pointsInBin[i];

            tmp_scanNumbers.push_back(point->scanNo);
            tmp_rt.push_back(convertRT[point->scanNo]);
            tmp_mz.push_back(point->mz);
            tmp_predInterval.push_back(point->mzError);
            tmp_ints_area.push_back(point->int_area);
            tmp_ints_height.push_back(point->int_height);
            tmp_DQSC.push_back(point->DQSCentroid);
        }

        EIC returnVal = {
            tmp_scanNumbers,
            tmp_rt,
            tmp_mz,
            tmp_predInterval,
            tmp_ints_area,
            tmp_ints_height,
            tmp_df,
            DQSB_base,
            tmp_DQSC};

        return returnVal;
    }

#pragma endregion "Bin"

#pragma region "Functions"

    static std::vector<float> meanDistanceRegional(const std::vector<const qCentroid *> pointsInBin, size_t maxdist)
    {
        // the other mean distance considers all points in the Bin.
        // It is sensible to only use the mean distance of all points within maxdist scans
        // this function assumes the bin to be sorted by scans
        const size_t binsize = pointsInBin.size();
        std::vector<float> output(binsize);
        size_t position = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            size_t scanRegionStart = maxdist < pointsInBin[i]->scanNo ? pointsInBin[i]->scanNo - maxdist : 0;
            size_t scanRegionEnd = pointsInBin[i]->scanNo + maxdist + 1;
            float accum = 0;
            for (; pointsInBin[position]->scanNo < scanRegionStart; position++) // increase position until a relevant point is found
                ;
            size_t readPos = position;
            for (; pointsInBin[readPos]->scanNo < scanRegionEnd;)
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

    static std::vector<float> scaleDistancesForDQS_linear(size_t maxdist) // @experimental
    {
        // This vector contains factors for an inverse gaussian scaling of distances during DQS calculation
        std::vector<float> scalarForMOD;
        // Assumption: every point which is found in a neighbouring scan, but not the
        // current one, has a 50% chance of existing. As such, it can only be half
        // as relevant as another point in the same scan. To scale, A distance in the
        // same scan is multiplied by one, one further by two, then by four etc.
        scalarForMOD.resize(maxdist * 4 + 2); // every value is doubled, same length as selected mz vector during DQS calculation
        float scalarSingle = pow(2, maxdist);
        for (size_t i = 0; i < 2 * maxdist + 1; i++) // scalarForMod contains the correct scaling to be used on the MOD for any maxdist
        {
            scalarForMOD[i] = scalarSingle;
            scalarForMOD[i + 1] = scalarSingle;
            scalarForMOD[scalarForMOD.size() - 1 - i] = scalarSingle;
            scalarForMOD[scalarForMOD.size() - 2 - i] = scalarSingle;
            i++;
            scalarSingle /= 2;
        }
        return scalarForMOD;
    }

#pragma endregion "Functions"

    std::vector<EIC> performQbinning(const CentroidedData *centroidedData, const std::vector<float> convertRT,
                                     size_t maxdist, bool verbose)
    {
        std::string logger = "";

        BinContainer activeBins = initialiseBinning(centroidedData);
        // rebinning is not separated into a function
        // binning is repeated until the input length is constant
        size_t prevFinal = 0;
        while (true) // @todo prove that this loop always terminates
        {
            logger += subsetBins(activeBins, maxdist);
            size_t producedBins = activeBins.viableBins.size();
            // if the same amount of bins as in the previous operation was found,
            // the process is considered complete
            // in the current configuration, rebinning takes three times as long
            // for two additional features, both of which are likely noise anyway
            int duplicateCount = 0;
            for (size_t j = 0; j < producedBins; j++)
            {
                if (activeBins.viableBins[j].duplicateScan)
                {
                    duplicateCount++;
                    deduplicateBin(&activeBins.processBinsF, &activeBins.notInBins, activeBins.viableBins[j]);
                }
                else
                {
                    // @todo check for min and max intensity being on the borders here.
                    // also consider if removing these points does affect the bin validity.
                    // the score should be reworked to consider all unbinned points to compensate
                    // for more aggressive culling anyhow.
                    activeBins.finalBins.push_back(activeBins.viableBins[j]);
                }
            }
            logger += "removed " + std::to_string(duplicateCount) + " duplicates\n";
            activeBins.viableBins.clear();

            if (prevFinal == activeBins.finalBins.size())
            {
                break;
            }
            prevFinal = activeBins.finalBins.size();
            // only perform rebinning if at least one new bin could be formed
            if (activeBins.notInBins.size() > 4)
            {
                // add empty start bin for rebinner
                activeBins.processBinsF.push_back(Bin{});
                // add all points that were not binned into the new bin, since these centroids
                // tend to contain smaller bins which were not properly processed due to being
                // at the borders of a cutting region
                activeBins.processBinsF.back().pointsInBin = activeBins.notInBins;
                activeBins.notInBins.clear();
                // re-binning during the initial loop would result in some bins being split prematurely
                // int rebinCount = selectRebin(&activeBins, centroidedData, maxdist);
                // @todo logging
            }
        }
        // no change in bin result, so all remaining bins cannot be coerced into a valid state
        if (!activeBins.processBinsF.empty())
        {
            for (Bin bin : activeBins.processBinsF)
            {
                for (size_t i = 0; i < bin.pointsInBin.size(); i++)
                {
                    activeBins.notInBins.push_back(bin.pointsInBin[i]);
                }
            }
            activeBins.processBinsF.clear();
        }

        if (!activeBins.processBinsT.empty())
        {
            for (Bin bin : activeBins.processBinsT)
            {
                for (size_t i = 0; i < bin.pointsInBin.size(); i++)
                {
                    activeBins.notInBins.push_back(bin.pointsInBin[i]);
                }
            }
            activeBins.processBinsT.clear();
        }

        auto scalar = scaleDistancesForDQS_linear(maxdist);
        for (size_t i = 0; i < activeBins.finalBins.size(); i++)
        {
            activeBins.finalBins[i].makeDQSB(centroidedData, scalar, maxdist);
        }

        // @todo add bin merger for halved bins here ; this ight be a bad idea, find way to prove it

        std::vector<EIC> finalBins;
        size_t binCount = activeBins.finalBins.size();
        finalBins.reserve(binCount);
        size_t countPointsInBins = 0;
        for (size_t i = 0; i < binCount; i++)
        {
            finalBins.push_back(activeBins.finalBins[i].createEIC(convertRT, maxdist));
            countPointsInBins += finalBins[i].scanNumbers.size();
        }
        assert(countPointsInBins + activeBins.notInBins.size() == centroidedData->lengthAllPoints);
        return finalBins;
    }
}
