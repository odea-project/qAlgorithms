#include "qalgorithms_qbin.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_utils.h"

#include <cassert>
#include <iostream> // error printing
#include <vector>
#include <numeric> // used for std::partial_sum during order space construction
#include <math.h>
#include <algorithm> // sorting
#include <string>

namespace qAlgorithms
{
    const size_t maxdist = 3; // this is the maximum distance in scans which can later be interpolated during feature detection

    std::vector<Bin> performQbinning(std::vector<CentroidPeak> *centroids)
    {
        centroids->erase(centroids->begin()); // @todo do not pass in dummy values, this is a confusing interface

        BinContainer activeBins;
        Bin firstBin;
        for (size_t i = 0; i < centroids->size(); i++)
        {
            firstBin.pointsInBin.push_back(&(centroids->at(i)));
        }
        activeBins.processBinsF.push_back(firstBin);

        // rebinning is not separated into a function
        // binning is repeated until the input length is constant
        size_t prevFinal = 0;
        size_t prevDiscarded = 0;

        while (true) // @todo prove that this loop always terminates
        {
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
            activeBins.viableBins.clear();

            if (prevFinal == activeBins.finalBins.size())
            {
                // no observed change in the produced bins - is it possible to get false positives here? @todo
                assert(activeBins.notInBins.size() == prevDiscarded);
                break;
            }
            prevFinal = activeBins.finalBins.size();
            prevDiscarded = activeBins.notInBins.size();
            // only perform rebinning if at least one new bin could be formed
            if (activeBins.notInBins.size() > 4)
            {
                // add empty start bin for rebinner
                activeBins.processBinsF.push_back(Bin{});
                // add all points that were not binned into the new bin, since these centroids
                // tend to contain smaller bins which were not properly processed due to being
                // at the borders of a cutting region
                activeBins.processBinsF.back().pointsInBin = activeBins.notInBins;
                activeBins.processBinsF.back().pointsInBin = activeBins.notInBins;
                activeBins.notInBins.clear();
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

        // calculate the DQSB as the silhouette score, considering only non-separated points
        std::sort(activeBins.notInBins.begin(), activeBins.notInBins.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->mz < rhs->mz; });

        // setting start position to 0 at this point means that it can be reused, since it is incremented in makeDQSB
        size_t shared_idxStart = 0;
        for (size_t i = 0; i < activeBins.finalBins.size(); i++)
        {
            shared_idxStart = activeBins.finalBins[i].makeDQSB(&activeBins.notInBins, shared_idxStart);
        }

        size_t countPointsInBins = 0;
        for (size_t i = 0; i < activeBins.finalBins.size(); i++)
        {
            countPointsInBins += activeBins.finalBins[i].pointsInBin.size();
        }
        assert(countPointsInBins + activeBins.notInBins.size() == centroids->size());

        return activeBins.finalBins;
    }

    std::vector<EIC> performQbinning_old(const std::vector<CentroidPeak> *centroidedData,
                                         const RT_Converter *convertRT) // @todo split out subfunctions so the structure is subset -> score -> format
    {
        assert(centroidedData->front().mz == 0); // first value is dummy

        BinContainer activeBins;
        Bin firstBin;
        for (size_t i = 0; i < centroidedData->size(); i++)
        {
            firstBin.pointsInBin.push_back(&(centroidedData->at(i)));
        }
        activeBins.processBinsF.push_back(firstBin);

        // rebinning is not separated into a function
        // binning is repeated until the input length is constant
        size_t prevFinal = 0;

        size_t itCount = 0;
        while (true) // @todo prove that this loop always terminates
        {
            subsetBins(activeBins);
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
            activeBins.viableBins.clear();

            if (prevFinal == activeBins.finalBins.size())
            {
                itCount += 1;
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
                // @todo rebinning might be a very bad idea
                // int rebinCount = selectRebin(&activeBins, centroidedData, maxdist);
                // @todo logging
            }

            itCount += 1;
        }
        // no change in bin result, so all remaining bins cannot be coerced into a valid state
        if (!activeBins.processBinsF.empty())
        {
            for (Bin bin : activeBins.processBinsF)
            {
                // for (size_t i = 0; i < bin.pointsInBin.size(); i++)
                // {
                //     activeBins.notInBins.push_back(bin.pointsInBin[i]);
                // }
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

        // calculate the DQSB as the silhouette score, considering only non-separated points
        std::sort(activeBins.notInBins.begin(), activeBins.notInBins.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->mz < rhs->mz; });

        // setting start position to 0 at this point means that it can be reused, since it is incremented in makeDQSB
        size_t shared_idxStart = 0;
        for (size_t i = 0; i < activeBins.finalBins.size(); i++)
        {
            shared_idxStart = activeBins.finalBins[i].makeDQSB(&activeBins.notInBins, shared_idxStart);
        }

        // @todo add bin merger for halved bins here ; this ight be a bad idea, find way to prove it

        std::vector<EIC> finalBins;
        size_t binCount = activeBins.finalBins.size();
        finalBins.reserve(binCount);
        size_t countPointsInBins = 0;
        for (size_t i = 0; i < binCount; i++)
        {
            auto eic = activeBins.finalBins[i].createEIC(convertRT);
            interpolateEIC(&eic);
            finalBins.push_back(eic);
            countPointsInBins += eic.df.back(); // interpolated points are already included in the size
        }
        // assert(countPointsInBins + activeBins.notInBins.size() == centroidedData->size());
        assert(countPointsInBins + activeBins.notInBins.size() == centroidedData->size());
        return finalBins;
    }

#pragma region "BinContainer"

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

    double previousBinMax = 0; // @todo this is a very bad idea

    void subsetBins(BinContainer &bincontainer) // @todo string return values are stupid
    {
        // auto timeStart = std::chrono::high_resolution_clock::now();
        // auto timeEnd = std::chrono::high_resolution_clock::now();
        bincontainer.readFrom = false; // starting bin is in processBinsF
        assert(bincontainer.processBinsT.empty());
        // assert(!bincontainer.processBinsF.empty());
        if (bincontainer.processBinsF.empty())
        {
            // there are no centroids to process left after the first iteration
            return;
        }

        bincontainer.sourceBins = &bincontainer.processBinsF;
        bincontainer.targetBins = &bincontainer.processBinsT;

        size_t itCount = 0;
        while (!(bincontainer.processBinsF.empty() && bincontainer.processBinsT.empty()))
        {
            // loop until all bins have been moved into finsihedBins or discarded
            // the target array is supplied via pointer, facilitates reuse

            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++) // for every element in the deque before writing new bins
            {
                Bin processThis = bincontainer.sourceBins->at(j); // @todo do not allocate for every iteration here, also point to pointsInBin

                std::sort(processThis.pointsInBin.begin(), processThis.pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                          { return lhs->mz < rhs->mz; });
                auto activeOS = makeOrderSpace(&processThis);
                auto cumError = makeCumError(&processThis.pointsInBin);

                std::vector<Range_i> rangeStack(1, {0, activeOS.size() - 1});
                subsetMZ_stack(&rangeStack,
                               bincontainer.targetBins,
                               &bincontainer.notInBins,
                               &processThis.pointsInBin,
                               &activeOS,
                               &cumError);

                previousBinMax = 0; // @todo get rid of this without sacrificing validity
            }
            // @todo logging
            switchTarget(&bincontainer);

            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++)
            {
                bincontainer.sourceBins->at(j).subsetScan(bincontainer.targetBins, &bincontainer.notInBins);
            }
            // @todo logging
            switchTarget(&bincontainer);

            // if the "unchanged" property of a bin is true, all selected tests have passed
            for (size_t j = 0; j < bincontainer.sourceBins->size(); j++)
            {
                if (bincontainer.sourceBins->at(j).unchanged)
                {
                    bincontainer.viableBins.push_back(bincontainer.sourceBins->at(j));
                }
                else
                {
                    bincontainer.targetBins->push_back(bincontainer.sourceBins->at(j));
                }
            }
            switchTarget(&bincontainer);
            itCount += 1;
        }
        bincontainer.processBinsF.clear();
        bincontainer.processBinsT.clear();
        // reset to input condition
        bincontainer.readFrom = false;
        return;
    }

    // @todo rework this function
    void deduplicateBin(std::vector<Bin> *target, std::vector<const CentroidPeak *> *notInBins, Bin bin)
    {
        assert(bin.duplicateScan);
        assert(bin.medianMZ > 1);
        // std::sort(bin.pointsInBin.begin(), bin.pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
        std::sort(bin.pointsInBin.begin(), bin.pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->number_MS1 < rhs->number_MS1; });
        Bin returnBin;
        returnBin.pointsInBin.reserve(bin.pointsInBin.size());

        // add dummy centroid to the end of the bin
        CentroidPeak dummy;
        dummy.mz = 0;
        dummy.number_MS1 = 0;
        bin.pointsInBin.push_back(&dummy); // pointer will be not be invalidated since bin is copied
        size_t duplicateRemovedCount = 1;  // initialse to 1 so the check at function end is less confusing

        for (size_t i = 1; i < bin.pointsInBin.size(); i++)
        {
            assert(bin.pointsInBin[i]->ID != bin.pointsInBin[i - 1]->ID);
            if (bin.pointsInBin[i]->number_MS1 == bin.pointsInBin[i - 1]->number_MS1)
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
        assert(returnBin.pointsInBin.size() + duplicateRemovedCount == bin.pointsInBin.size());

        target->push_back(returnBin);
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin makeBin_mz(const std::vector<const CentroidPeak *> *const centroids, const Range_i *range)
    {
        // function assumes bin is sorted by mz

        Bin res;
        res.pointsInBin.reserve(range->endIdx - range->startIdx + 1);

        for (size_t i = range->startIdx; i < range->endIdx + 1; i++)
        { // +1 since the range is inclusive
            auto point = (*centroids)[i];
            assert(point->ID != 0);
            res.pointsInBin.push_back(point);
        }
        assert(res.pointsInBin.size() > 4);

        res.mzMin = res.pointsInBin.front()->mz;
        res.mzMax = res.pointsInBin.back()->mz;
        res.unchanged = true;
        res.medianMZ = res.pointsInBin[res.pointsInBin.size() / 2]->mz;

        return res;
    }

    Bin makeBin_scan(const std::vector<const CentroidPeak *> *centroids, const size_t binStartPos, const size_t binEndPos)
    {
        assert(binStartPos < binEndPos);
        Bin res;
        res.pointsInBin.reserve(binEndPos - binStartPos + 1);
        for (size_t i = binStartPos; i <= binEndPos; i++)
        {
            res.pointsInBin.push_back((*centroids)[i]);
        }
        assert(res.pointsInBin.size() > 4);
        return res;
    }

    const std::vector<double> makeOrderSpace(const Bin *bin)
    {
        // this function assumes that the centroids are sorted by mz
        auto points = bin->pointsInBin;
        std::vector<double> OS;
        OS.reserve(points.size());
        for (size_t i = 0; i < points.size() - 1; i++)
        {
            OS.push_back((points[i + 1]->mz - points[i]->mz));
        }
        OS.push_back(NAN);
        return OS;
    }

    const std::vector<double> makeCumError(const std::vector<const qAlgorithms::CentroidPeak *> *bin)
    {
        std::vector<double> cumError;
        cumError.reserve(bin->size());
        for (size_t i = 0; i < bin->size(); i++)
        {
            cumError.push_back(bin->at(i)->mzUncertainty);
        }
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
        return cumError;
    }

    int subsetMZ_stack(std::vector<Range_i> *stack,
                       std::vector<Bin> *bincontainer,
                       std::vector<const CentroidPeak *> *notInBins,
                       const std::vector<const qAlgorithms::CentroidPeak *> *pointsInSourceBin,
                       const std::vector<double> *OS,
                       const std::vector<double> *cumError)
    {
        assert(OS->size() == cumError->size());
        assert(pointsInSourceBin->size() == OS->size());

        const double *pointerToStart = OS->data();

        size_t pointsProcessed = 0;
        size_t previousID = -1;

        while (!stack->empty())
        {
            const Range_i range = stack->back();
            stack->pop_back();

            const size_t binsizeInOS = rangeLen(&range);
            assert(binsizeInOS <= pointsInSourceBin->size());

            if (binsizeInOS < 5) // min of five points per bin
            {
                for (size_t i = range.startIdx; i < range.endIdx + 1; i++)
                {
                    notInBins->push_back(pointsInSourceBin->at(i));
                    pointsProcessed += 1;
                }

                // printf("region: %zu : %zu | add to notInBins\n", range.startIdx, range.endIdx);
                continue;
            }

            // the last distance of the order space is never relevant, since the point one past
            // it is already out of range for the current block
            const double *pointerToMax = maxVal(pointerToStart + range.startIdx, binsizeInOS - 1);
            const double max = *pointerToMax;

            size_t cenID = pointsInSourceBin->at(pointerToMax - pointerToStart)->ID;
            assert(cenID != previousID);
            previousID = cenID;

            const double meanError = meanOfCumulative(cumError->data(), range.startIdx, range.endIdx);

            const double vcrit = binningCritVal(binsizeInOS, meanError);

            if (max < vcrit) // all values in range are part of one mz bin
            {
                // the difference is calculated for the position n, and is accordingly excluded from the
                // next recursive call at every step. Since position n is still part of the bin (the distance
                // to n was not critical), one past the limit has to be included in the new bin

                Bin output = makeBin_mz(pointsInSourceBin, &range);
                pointsProcessed += output.pointsInBin.size();
                bincontainer->push_back(output);

                // printf("region: %zu : %zu | add to Bin\n", range.startIdx, range.endIdx);
                continue;
            }

            const size_t cutpos = pointerToMax - pointerToStart;

            {
                assert(cutpos >= range.startIdx);
                // printf("region: %zu : %zu | add to Stack\n", range.startIdx, cutpos);
                stack->push_back({range.startIdx, cutpos});
            }
            {
                assert(cutpos < range.endIdx);
                // printf("region: %zu : %zu | add to Stack\n", cutpos + 1, range.endIdx);
                stack->push_back({cutpos + 1, range.endIdx});
            }
            assert(pointsProcessed < pointsInSourceBin->size());
        }
        assert(pointsProcessed == pointsInSourceBin->size());

        // printf("completed one stack of mz subsets\n");
        return 0;
    }

    void Bin::subsetScan(std::vector<Bin> *bincontainer, std::vector<const CentroidPeak *> *notInBins)
    {
        assert(!pointsInBin.empty());
        // function is called on a bin sorted by mz
        const size_t binSize = pointsInBin.size();
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->number_MS1 < rhs->number_MS1; });
        std::vector<const CentroidPeak *>::iterator newstart = pointsInBin.begin();
        int lastpos = 0;
        for (size_t i = 0; i < binSize - 1; i++) // -1 since difference to next data point is checked
        {
            assert(pointsInBin[i + 1]->number_MS1 >= pointsInBin[i]->number_MS1);
            assert(pointsInBin[i]->ID != 0);
            size_t distanceScan = pointsInBin[i + 1]->number_MS1 - pointsInBin[i]->number_MS1;
            if (distanceScan > maxdist) // bin needs to be split
            {
                // less than five points in bin
                if (i - lastpos + 1 < 5) // +1 since i starts at 0
                {
                    for (size_t j = lastpos; j <= i; j++)
                    {
                        const CentroidPeak *F = *(pointsInBin.begin() + j);
                        notInBins->push_back(F);
                    }
                }
                else
                {
                    // viable bin, stable in scan dimension
                    // +1 since otherwise last element of the correct range is not included
                    Bin output = makeBin_scan(&pointsInBin, lastpos, i);
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
            this->scanMin = pointsInBin.front()->number_MS1;
            this->scanMax = pointsInBin.back()->number_MS1;
            this->unchanged = true;
            bincontainer->push_back(*this);
        }
        else if (binSize - lastpos > 4) // binsize starts at 1
        {
            // viable bin, stable in scan dimension
            Bin output = makeBin_scan(&pointsInBin, lastpos, pointsInBin.size() - 1);
            assert(output.pointsInBin.size() > 4);
            bincontainer->push_back(output);
        }
        else
        {
            for (size_t j = lastpos; j < pointsInBin.size(); j++)
            {
                const CentroidPeak *F = *(pointsInBin.begin() + j);
                notInBins->push_back(F);
            }
        }
    }

    size_t Bin::makeDQSB(const std::vector<const CentroidPeak *> *notInBins, size_t idx_lowerLimit)
    {
        // assume that bins are separated well enough that any gap of this size is close to perfect
        // separation already, so score = 1
        assert(idx_lowerLimit < notInBins->size());
        float mz_hardLimit = std::max(0.1, this->mzMax * 10e-5);
        if (this->mzMax - this->mzMin > mz_hardLimit)
        {
            // failsafe if a nonsense bin is produced, score zeroed
            std::vector<float> scores(this->pointsInBin.size(), 0);
            this->DQSB_base = scores;
            this->DQSB_scaled = scores;
            return idx_lowerLimit;
        }
        // iterate over points and only consider mass region + 0.1. It is assumed that if a distance
        // of 0.1 mz is exceeded, the bin is perfectly separated. This is about 100 times more than
        // the maximum tolerated distance between points even for very low density bins
        size_t expandedDist = maxdist + 2; // always consider points one past the gap to account for potentially bad separation
        this->DQSB_base.clear();

        if (notInBins->back()->mz < this->mzMin - mz_hardLimit ||
            notInBins->front()->mz > this->mzMax + mz_hardLimit)
        {
            // no points are within range, perfect score
            std::vector<float> scores(this->pointsInBin.size(), 1.0);
            this->DQSB_base = scores;
            this->DQSB_scaled = scores;
            return idx_lowerLimit;
        }

        // @todo we can use SIMD here with relatively little effort
        for (; idx_lowerLimit < notInBins->size(); idx_lowerLimit++)
        {
            if ((*notInBins)[idx_lowerLimit]->mz > this->mzMin - mz_hardLimit)
            {
                break;
            }
        }
        size_t idx_upperLimit = idx_lowerLimit;
        for (; idx_upperLimit < notInBins->size(); idx_upperLimit++)
        {
            if ((*notInBins)[idx_upperLimit]->mz > this->mzMax + mz_hardLimit)
            {
                break;
            }
        }
        // all points with a sensible mass distance are between the two indices
        // continue by moving all points within a relevant scan region into a separate vector
        std::vector<const CentroidPeak *> scoreRegion;
        scoreRegion.reserve((idx_upperLimit - idx_lowerLimit) / 2);
        size_t lowestPossibleScan = this->scanMin > expandedDist ? this->scanMin - expandedDist : 0;
        for (size_t i = idx_lowerLimit; i < idx_upperLimit; i++)
        {
            if ((*notInBins)[i]->number_MS1 > lowestPossibleScan && // cast to int due to negative being possible
                (*notInBins)[i]->number_MS1 < this->scanMax + expandedDist)
            {
                // centroid is within maxdist and relevant mz region. However,
                // one past maxdist is considered for better representativeness
                scoreRegion.push_back((*notInBins)[i]);
            }
        }
        std::sort(scoreRegion.begin(), scoreRegion.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->number_MS1 < rhs->number_MS1; });

        // calculate minimum outer distance
        std::vector<float> minOuterDistances(this->pointsInBin.size());
        // calc distance for every possible scan number to simplify algorithm
        for (size_t i = 0; i < this->pointsInBin.size(); i++)
        {
            int activeScan = this->pointsInBin[i]->number_MS1;
            float activeMZ = this->pointsInBin[i]->mz;
            float currentMin = INFINITY;
            size_t readVal = 0;
            // advance until first point within maxdist + 1 of scan
            // @todo replace with binary search
            for (; readVal < scoreRegion.size(); readVal++)
            {
                if (scoreRegion[readVal]->number_MS1 > activeScan - expandedDist)
                {
                    break;
                }
            }
            for (; readVal < scoreRegion.size(); readVal++)
            {
                if (scoreRegion[readVal]->number_MS1 > activeScan + expandedDist)
                {
                    break;
                }
                float distanceMZ = abs(scoreRegion[readVal]->mz - activeMZ);
                if (distanceMZ < currentMin)
                {
                    currentMin = distanceMZ;
                }
            }
            if (currentMin == 0)
            {
                currentMin = 1e-100; // set minimal distance to lowest sensible float
            }

            minOuterDistances[i] = currentMin;
        }

        // calculate mean inner distance
        std::vector<float> meanInnerDistances = meanDistanceRegional(&pointsInBin, expandedDist);

        for (size_t i = 0; i < this->pointsInBin.size(); i++)
        {
            if (meanInnerDistances[i] == minOuterDistances[i])
            {
                this->DQSB_base.push_back(0);
                continue;
            }

            float tmpDQS = calcDQS(meanInnerDistances[i], minOuterDistances[i]);
            assert(-1 < tmpDQS);
            assert(tmpDQS <= 1);
            this->DQSB_base.push_back(tmpDQS);
        }
        return idx_lowerLimit;
    }

    EIC Bin::createEIC(const RT_Converter *convertRT)
    {
        size_t eicsize = pointsInBin.size();
        assert(eicsize > 4);
        // sorting should be done beforehand @todo
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->number_MS1 < rhs->number_MS1; });

        // using the knowledge of where points should be interpolated, transfer centroids into
        // arrays with gaps for later processing

        volatile size_t frontTime = pointsInBin.front()->number_MS1;
        volatile size_t backTime = pointsInBin.back()->number_MS1;

        unsigned int firstScan = convertRT->indexOfOriginalInInterpolated[frontTime];
        unsigned int lastScan = convertRT->indexOfOriginalInInterpolated[backTime];
        size_t interpolatedSize = lastScan - firstScan + 1 + 4; // +4 since we extrapolate two points to each side later
        std::vector<unsigned int> tmp_interpScans(interpolatedSize, 0);
        std::iota(tmp_interpScans.begin(), tmp_interpScans.end(), firstScan - 2);

        std::vector<unsigned int> tmp_scanNumbers(interpolatedSize, 0);
        std::vector<float> tmp_mz(interpolatedSize, 0);
        std::vector<float> tmp_mzUncert(interpolatedSize, 0);
        std::vector<float> tmp_ints_area(interpolatedSize, 0);
        std::vector<float> tmp_ints_height(interpolatedSize, 0);
        std::vector<float> tmp_DQSC(interpolatedSize, 0);
        std::vector<unsigned int> tmp_cenID(interpolatedSize, 0);

        unsigned int prevaccess = -1; // max of uint
        for (size_t i = 0; i < eicsize; i++)
        {
            const CentroidPeak *point = pointsInBin[i];

            size_t interpolatedIdx = convertRT->indexOfOriginalInInterpolated[point->number_MS1];
            unsigned int access = interpolatedIdx - firstScan + 2; // two scans at the front are extrapolated later
            assert(access != prevaccess);
            assert(point->RT == convertRT->groups[interpolatedIdx].trueRT);

            tmp_scanNumbers[access] = point->number_MS1;
            tmp_mz[access] = point->mz;
            assert(point->mz > 0);
            tmp_mzUncert[access] = point->mzUncertainty;
            tmp_ints_area[access] = point->area;
            tmp_ints_height[access] = point->height;
            tmp_DQSC[access] = point->DQSC;
            tmp_cenID[access] = point->ID;
        }

        // cumulative degrees of freedom
        std::vector<unsigned int> tmp_df(interpolatedSize, 0);
        unsigned int df_total = 0;

        for (size_t i = 2; i < tmp_scanNumbers.size(); i++)
        {
            if (tmp_scanNumbers[i] != 0) // interpolated scan
                df_total += 1;

            tmp_df[i] = df_total;
        }
        assert(df_total == eicsize); // @todo this can be disturbed by supplying incorrectly formatted data as RT converter

        EIC returnVal = {
            tmp_scanNumbers,
            tmp_mz,
            tmp_mzUncert,
            tmp_ints_area,
            tmp_ints_height,
            tmp_df,
            DQSB_base,
            tmp_DQSC,
            tmp_cenID,
            tmp_interpScans};

        return returnVal;
    }

    EIC binToEIC(Bin *sourceBin, const std::vector<unsigned int> *convertIndex)
    {
        size_t eicsize = sourceBin->pointsInBin.size();
        assert(eicsize > 4);
        // sorting should be done beforehand @todo
        std::sort(sourceBin->pointsInBin.begin(), sourceBin->pointsInBin.end(), [](const CentroidPeak *lhs, const CentroidPeak *rhs)
                  { return lhs->number_MS1 < rhs->number_MS1; });

        // using the knowledge of where points should be interpolated, transfer centroids into
        // arrays with gaps for later processing

        volatile size_t frontTime = sourceBin->pointsInBin.front()->number_MS1;
        volatile size_t backTime = sourceBin->pointsInBin.back()->number_MS1;

        unsigned int firstScan = convertIndex->at(frontTime);
        unsigned int lastScan = convertIndex->at(backTime);
        size_t interpolatedSize = lastScan - firstScan + 1 + 4; // +4 since we extrapolate two points to each side later
        std::vector<unsigned int> tmp_interpScans(interpolatedSize, 0);
        std::iota(tmp_interpScans.begin(), tmp_interpScans.end(), firstScan - 2);

        std::vector<unsigned int> tmp_scanNumbers(interpolatedSize, 0);
        std::vector<float> tmp_mz(interpolatedSize, 0);
        std::vector<float> tmp_mzUncert(interpolatedSize, 0);
        std::vector<float> tmp_ints_area(interpolatedSize, 0);
        std::vector<float> tmp_ints_height(interpolatedSize, 0);
        std::vector<float> tmp_DQSC(interpolatedSize, 0);
        std::vector<float> tmp_DQSB(interpolatedSize, 0);
        std::vector<unsigned int> tmp_cenID(interpolatedSize, 0);

        unsigned int prevaccess = -1; // max of uint
        for (size_t i = 0; i < eicsize; i++)
        {
            unsigned int access = convertIndex->at(sourceBin->pointsInBin.at(i)->number_MS1) - firstScan + 2; // two scans at the front are extrapolated later
            assert(access != prevaccess);
            const CentroidPeak *point = sourceBin->pointsInBin[i];

            tmp_scanNumbers[access] = point->number_MS1;
            tmp_mz[access] = point->mz;
            tmp_mzUncert[access] = point->mzUncertainty;
            tmp_ints_area[access] = point->area;
            tmp_ints_height[access] = point->height;
            tmp_DQSC[access] = point->DQSC;
            tmp_DQSB[access] = sourceBin->DQSB_base[i];
            tmp_cenID[access] = point->ID;
        }

        // cumulative degrees of freedom
        std::vector<unsigned int> tmp_df(interpolatedSize, 0);
        unsigned int df_total = 0;

        for (size_t i = 2; i < tmp_scanNumbers.size(); i++)
        {
            if (tmp_scanNumbers[i] != 0) // interpolated scan
                df_total += 1;

            tmp_df[i] = df_total;
        }
        assert(df_total == eicsize); // @todo this can be disturbed by supplying incorrectly formatted data as RT converter

        EIC returnVal = {
            tmp_scanNumbers,
            tmp_mz,
            tmp_mzUncert,
            tmp_ints_area,
            tmp_ints_height,
            tmp_df,
            tmp_DQSB,
            tmp_DQSC,
            tmp_cenID,
            tmp_interpScans};

        return returnVal;
    }

    void interpolateEIC(EIC *eic)
    {
        // given the processed EICs from the above function, fill in gaps by performing quadratic inter- and extrapolation
        std::vector<float> *areas = &eic->ints_area;

        // extrapolate start
        float firstVal = areas->at(2);
        areas->at(0) = firstVal / 4;
        areas->at(1) = firstVal / 2;

        // extrapolate end
        float lastVal = areas->at(areas->size() - 3);
        areas->at(areas->size() - 2) = lastVal / 2;
        areas->at(areas->size() - 1) = lastVal / 4;

        bool noInterpolations = areas->size() - 4 == eic->df.back();
        if (noInterpolations) // no empty values
            return;

        // interpolate empty values
        bool openBlock = false;
        size_t startPos = 2;
        for (size_t i = 3; i < areas->size() - 2; i++)
        {
            if (areas->at(i) == 0)
            {
                openBlock = true;
            }
            else if (!openBlock)
            {
                firstVal = areas->at(i);
                startPos = i;
            }
            else
            {
                // close the block
                lastVal = areas->at(i);
                size_t gapsize = i - startPos - 1;

                // @todo is this the best possible approach?
                // assuming there is no baseline, two points suffice for determining y = a * b^x
                // we set the first real point as x = 0, so y1 = a. The second point is at gapsize + 1 (x = gap). It follows
                // y2 = y1 * b^gap
                // b^gap = y2 / y1
                // b = gap-th root of y2 / y1
                // b = (y2 / y1)^(1 / gap)

                float a = firstVal;
                float b = std::pow(lastVal / firstVal, 1.0 / float(gapsize + 1));
                for (size_t pos = startPos + 1; pos < i; pos++)
                {
                    int x = pos - startPos;
                    areas->at(pos) = a * std::pow(b, x);
                }
                openBlock = false;
                firstVal = areas->at(i);
                startPos = i;
            }
        }
    }

#pragma endregion "Bin"

#pragma region "Functions"

    std::vector<float> meanDistanceRegional(const std::vector<const CentroidPeak *> *pointsInBin, const size_t expandedDist)
    {
        // the other mean distance considers all points in the Bin.
        // It is sensible to only use the mean distance of all points within maxdist scans
        // this function assumes the bin to be sorted by scans
        const size_t binsize = pointsInBin->size();
        std::vector<float> output(binsize);
        size_t position = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            const auto cen = (*pointsInBin)[i];
            size_t scanRegionStart = cen->number_MS1 < expandedDist + 1 ? 0 : cen->number_MS1 - expandedDist - 1;
            size_t scanRegionEnd = cen->number_MS1 + expandedDist + 1;
            float accum = 0;
            for (; (*pointsInBin)[position]->number_MS1 < scanRegionStart; position++) // increase position until a relevant point is found
                ;
            size_t readPos = position;
            for (; (*pointsInBin)[readPos]->number_MS1 < scanRegionEnd;)
            {
                accum += abs((*pointsInBin)[readPos]->mz - cen->mz);
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

    inline float calcDQS(float meanInnerDist, float minOuterDist)
    {
        float maxInVal = std::max(minOuterDist, meanInnerDist); // meanInnerDist should generally be much smaller than minOuterDist
        if (maxInVal == INFINITY)
        {
            return 1;
        }

        // dqs = (minOuterDist - meanInnerDist) * (1 / (1 + meanInnerDist)) / maxInVal
        return (minOuterDist - meanInnerDist) / fmal(meanInnerDist, maxInVal, maxInVal);
    }
#pragma endregion "Functions"

    bool massTraceStable(std::vector<float> massesBin, int idxStart, int idxEnd) // @todo do this in regression
    {
        assert(idxEnd > idxStart);
        size_t peaksize = idxEnd - idxStart + 1;
        // std::cout << idxStart << ", " << idxEnd << ", " << peaksize << "\n";
        std::vector<float> massesPeak;
        for (size_t i = 0; i < peaksize; i++)
        {
            massesPeak.push_back(massesBin[idxStart + i]);
        }
        std::sort(massesPeak.begin(), massesPeak.end());

        // critval @todo make this one function
        float mean = 0;
        float stddev = 0;
        for (size_t i = 0; i < peaksize; i++)
        {
            mean += massesPeak[i];
        }
        mean /= peaksize;
        for (size_t i = 0; i < peaksize; i++)
        {
            stddev += (massesPeak[i] - mean) * (massesPeak[i] - mean);
        }
        stddev = sqrt(stddev / (peaksize - 1));

        // float vcrit = 3.05037165842070 * pow(log(peaksize), (TOLERANCE_BINNING)) * stddev;
        float vcrit = binningCritVal(peaksize, stddev);
        for (size_t i = 1; i < peaksize; i++)
        {
            [[unlikely]] if (massesPeak[i] - massesPeak[i - 1] > vcrit)
            {
                return false;
            }
        }
        return true;
    }
}
