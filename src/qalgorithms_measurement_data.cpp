#include "qalgorithms_measurement_data.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_read_file.h"

#include <cmath>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

#include <random> // only temporarily needed

namespace qAlgorithms
{
#pragma region "Feature Detection"

    std::vector<unsigned int> interpolateScanNumbers(const std::vector<float> *retentionTimes)
    {
        // This function interpolates the existing RTs of MS1 spectra and produces a vector that contains,
        // for the index of every real MS1 spectrum, the scan number with interpolations for that spectrum.
        // Since we work with an integer scale, interpolations are rounded at 0.5.
        // It is assumed that the RT vector is sorted.

        assert(!retentionTimes->empty());
        std::vector<float> diffs(retentionTimes->size() - 1, 0);
        for (size_t i = 0; i < diffs.size(); i++)
        {
            diffs[i] = retentionTimes->at(i + 1) - retentionTimes->at(i);
        }
        assert(!diffs.empty());

        // the expected difference is the median of all differences
        float expectedDiff;
        { // @todo solve this via quickselect instead
            std::vector<float> tmpDiffs = diffs;
            std::sort(tmpDiffs.begin(), tmpDiffs.end());

            size_t size = tmpDiffs.size();
            assert(size > 0);
            if (size % 2 == 0)
            {
                expectedDiff = (tmpDiffs[size / 2 - 1] + tmpDiffs[size / 2]) / 2;
            }
            else
            {
                expectedDiff = tmpDiffs[size / 2];
            }
            assert(tmpDiffs[0] > expectedDiff / 2); // if this is not given, there are severe distortions at some point in the data
        }

        std::vector<unsigned int> res(retentionTimes->size(), 1);

        float critDiff = expectedDiff * 1.5; // if the difference is greater than the critDiff, there should be at least one interpolation
        unsigned int currentScan = 1;
        for (size_t i = 0; i < diffs.size(); i++)
        {
            if (diffs[i] < critDiff)
            {
                currentScan += 1;
            }
            else
            {
                // interpolate at least one point
                size_t numInterpolations = size_t(diffs[i] / expectedDiff + 0.5); // + 0.5 since value is truncated
                currentScan += numInterpolations;
            }
            res[i + 1] = currentScan;
        }
        assert(res.back() >= res.size());
        return res;
    }

    std::vector<FeaturePeak> findFeatures(std::vector<EIC> &EICs, std::vector<float> *RT)
    {
        std::vector<FeaturePeak> peaks;    // return vector for feature list
        peaks.reserve(EICs.size() / 4);    // should be enough to fit all features without reallocation
        std::vector<FeaturePeak> tmpPeaks; // add features to this before pasting into FL

        for (size_t i = 0; i < EICs.size(); ++i)
        {
            auto currentEIC = EICs[i];
            if (currentEIC.scanNumbers.size() < 5)
            {
                continue; // skip due to lack of data, i.e., degrees of freedom will be zero
            }

            findFeaturePeaks(&tmpPeaks, RT, &currentEIC);
            // @todo extract the peak construction here and possibly extract findFeatures into a generic function

            if (tmpPeaks.empty())
            {
                continue;
            }
            for (size_t j = 0; j < tmpPeaks.size(); j++)
            {
                FeaturePeak currentPeak = tmpPeaks[j];

                currentPeak.scanPeakStart = currentEIC.scanNumbers.front();
                currentPeak.scanPeakEnd = currentEIC.scanNumbers.back();
                assert(currentPeak.scanPeakEnd < RT->size());
                currentPeak.idxBin = i;

                if (currentPeak.idxPeakEnd - currentPeak.idxPeakStart < 4)
                {
                    // @todo this should be caught in the regression function, control
                    continue;
                }
                // @todo URGENT (resolved) this kicks out a massive amount of features, check if it makes sense for
                // centroids / replace the whole three-fold interpolation nonsense with one source of truth
                if ((currentPeak.index_x0_offset < 2) ||
                    (currentPeak.idxPeakEnd - currentPeak.idxPeakStart - currentPeak.index_x0_offset < 2))
                {
                    continue;
                }
                if (currentPeak.idxPeakEnd - currentPeak.idxPeakStart + 2 < currentPeak.index_x0_offset)
                {
                    continue;
                }
                // the correct limits in the non-interpolated EIC need to be determined. They are already included
                // in the cumulative degrees of freedom, but since there, df 0 is outside the EIC, we need to
                // use the index df[limit] - 1 into the original, non-interpolated vector

                unsigned int limit_L = currentEIC.df[currentPeak.idxPeakStart];
                limit_L = std::min(limit_L, limit_L - 1); // uint underflows, so no issues.
                unsigned int limit_R = currentEIC.df[currentPeak.idxPeakEnd] - 1;
                assert(limit_L < limit_R);

                currentPeak.idxBinStart = limit_L;
                currentPeak.idxBinEnd = limit_R;

                auto tmp = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.mz,
                                                       limit_L, limit_R);
                currentPeak.mz = tmp.mean;
                currentPeak.mzUncertainty = tmp.var;
                currentPeak.DQSC = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.DQSC,
                                                               limit_L, limit_R)
                                       .mean;
                currentPeak.DQSB = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.DQSB,
                                                               limit_L, limit_R)
                                       .mean;
                peaks.push_back(std::move(currentPeak)); // remove 2D structure of FL
            }

            tmpPeaks.clear();
        }
        // peaks are sorted here so they can be treated as const throughout the rest of the program
        std::sort(peaks.begin(), peaks.end(), [](FeaturePeak lhs, FeaturePeak rhs)
                  { return lhs.retentionTime < rhs.retentionTime; });
        return peaks;
    }

#pragma endregion "Feature Detection"

#pragma region "find centroids"

    inline float calcRTDiff(const std::vector<double> *retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times->size(); ++i)
        {
            sum += retention_times->at(i) - retention_times->at(i - 1);
        }
        return sum / (retention_times->size() - 1);
    }

    std::vector<CentroidPeak> findCentroids( // this function needs to be reworked further @todo
        XML_File &data,
        const std::vector<pugi::xml_node> *linkNodes,
        const bool polarity,
        const bool ms1only)
    {
        const std::vector<unsigned int> selectedIndices = data.filter_spectra(linkNodes, ms1only, polarity, false);
        if (selectedIndices.empty())
        {
            // this currently only serves to eliminate spectra of the wrong polarity, @todo better solution?
            return std::vector<CentroidPeak>{};
        }
        const size_t countSelected = selectedIndices.size();

        std::vector<CentroidPeak> centroids;
        centroids.reserve(countSelected * 1000);
        centroids.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}); // dummy value used for binning

        std::vector<double> spectrum_mz(1000);
        std::vector<double> spectrum_int(1000);
        for (size_t i = 0; i < countSelected; ++i)
        {
            // avoid needless allocation / deallocation
            spectrum_mz.clear();
            spectrum_int.clear();

            size_t ID_spectrum = selectedIndices[i];
            data.get_spectrum(linkNodes, &spectrum_mz, &spectrum_int, ID_spectrum);

            const auto profileGroups = pretreatDataCentroids(&spectrum_mz, &spectrum_int);

            findCentroidPeaks(&centroids, &profileGroups, i + 1, ID_spectrum);
        }
        for (unsigned int i = 0; i < centroids.size(); i++)
        {
            centroids[i].ID = i;
        }

        return centroids;
    }

    void binProfileSpec(std::vector<Block> *result,
                        const std::vector<double> *diffs,
                        // const std::vector<unsigned int> *diffOrder,
                        const std::vector<double> *cumDiffs, // indices into cumDiffs must be right-shifted by one!
                        // size_t previousDiffPos,              // skip this many points in the diffOrder vector
                        size_t start, size_t end)
    {
        // perform the recursive split introduced during binning to find gaps in mz
        // CritVal uses the standard deviation, which is estimated as the mean centroid error during binning.
        // here, we can use the real SD of the point-to-point differences.
        assert(start <= end); // <= since one point at the end could get removed, see below
        assert(!diffs->empty());
        assert(cumDiffs->size() == diffs->size() + 1);
        assert(cumDiffs->at(0) == 0); // this is important so there are no special cases when forming the difference

        size_t length = end - start + 1;
        if (length < 4) // one point less due to differences being used
        {
            return;
        }

        // we use the mean difference here since the differences should follow a flat distribution. The basic assumption
        // is that every point has an error of half the distance to its neighbours, since all points are evenly spaced.
        double meanDiff = (cumDiffs->at(end + 1) - cumDiffs->at(start)) / length;
        double critVal = binningCritVal(length, meanDiff / 2);

        // max of difference // @todo extract to inline function to use in binning
        auto pmax = std::max_element(diffs->begin() + start, diffs->begin() + end + 1);
        double max = *pmax;
        size_t maxPos = std::distance(diffs->begin(), pmax);

        // size_t maxPos2 = 0;
        // size_t nextDiffPos = 0;
        // // @todo is there a difference too small to matter known ahead of time?
        // for (size_t i = previousDiffPos; i < diffOrder->size(); i++)
        // {
        //     unsigned int pos = diffOrder->at(i);
        //     if (start <= pos && pos <= end)
        //     {
        //         // pos is the index of the largest difference in the relevant region
        //         nextDiffPos = i;
        //         maxPos2 = pos;
        //         break;
        //     }
        // }
        // assert(maxPos2 == maxPos);
        // size_t nextDiffPos = 0;

        // size_t maxPos = maxPos2;
        // double max = diffs->at(maxPos2);

        if (max < critVal)
        {
            // block is complete, add limits to result vector
            result->push_back({start, end + 1}); // end + 1 since difference has one point less
            return;
        }
        // recursive split at max - different calling convention since we work with differences
        if (maxPos != start)
        {
            binProfileSpec(result, diffs, cumDiffs, start, maxPos - 1); // when setting the block, 1 is added to end
        }
        if (maxPos != end)
        {
            binProfileSpec(result, diffs, cumDiffs, maxPos + 1, end); // one past the max to avoid large value
        }
    }

    inline ProfileBlock initBlock(size_t blocksize)
    {
        std::vector<float> mz_int(blocksize, 0);
        std::vector<unsigned int> cumdf(blocksize, 0);
        std::iota(cumdf.begin() + 2, cumdf.end(), 1);
        cumdf[blocksize - 1] = blocksize - 4;
        cumdf[blocksize - 2] = blocksize - 4;

        ProfileBlock ret = {mz_int,
                            mz_int,
                            cumdf,
                            0, 0};
        return ret;
    }

    std::vector<ProfileBlock> pretreatDataCentroids(
        const std::vector<double> *spectrum_mz,
        const std::vector<double> *spectrum_int)
    {
        std::vector<double> intensities_profile;
        std::vector<double> mz_profile;
        std::vector<unsigned int> idxConvert; // used to trace back processing steps to untreated data

        { // remove zeroes
            size_t size = spectrum_mz->size();
            assert(spectrum_int->size() == size);
            intensities_profile.reserve(size);
            mz_profile.reserve(size);
            // Depending on the vendor, a profile contains a lot of points with intensity 0.
            // These were added by the vendor software and must be removed prior to processing.
            for (unsigned int i = 0; i < size; ++i)
            {
                if (spectrum_int->at(i) == 0.0)
                {
                    continue; // skip values with no intensity @todo minimum intensity?
                }
                intensities_profile.push_back(spectrum_int->at(i));
                mz_profile.push_back(spectrum_mz->at(i));
                idxConvert.push_back(i);
            }
            assert(!intensities_profile.empty());
            assert(!mz_profile.empty());
        }

        std::vector<double> diffs;
        std::vector<double> cumDiffs;
        { // calculate mz differences
            diffs.reserve(mz_profile.size() - 1);
            cumDiffs.reserve(mz_profile.size());
            cumDiffs.push_back(0);
            double totalDiff = 0;

            for (size_t i = 1; i < mz_profile.size(); i++)
            {
                double diff = mz_profile[i] - mz_profile[i - 1];
                diffs.push_back(diff);
                totalDiff += diff;
                cumDiffs.push_back(totalDiff);
            }
        }

        std::vector<Block> result; // result contains the start- and end indices of all relevant blocks in the data.
        result.reserve(128);

        // index vector into diffs, making diff itself obsolete // @todo this can be solved without recursion
        // std::vector<unsigned int> diffOrder(diffs.size());
        // std::iota(diffOrder.begin(), diffOrder.end(), 0);
        // std::sort(diffOrder.begin(), diffOrder.end(),
        //           [&](int a, int b)
        //           { return diffs[a] > diffs[b]; });

        double meanDiff = 0;
        for (size_t i = 0; i < diffs.size(); i++)
        {
            meanDiff += diffs[i];
        }
        meanDiff /= diffs.size();

        unsigned int knownStart = 0;
        unsigned int knownEnd = diffs.size() - 1;

        if (meanDiff > 0.1) [[likely]]
        {
            // in a real mass spectrum, this should always be a given. If not, the noise level is so
            // high that it might be impossible to find actually useful data here. Either way, since
            // we search for centroids in these regions, 0.1 is too large if anything. There will never
            // be a useable centroid with this large a m/z difference in a high res mass spectrum
            for (size_t i = 0; i < diffs.size(); i++)
            {
                if (diffs[i] > meanDiff)
                {
                    knownEnd = i - 1;
                    if (knownEnd > knownStart)
                    {
                        binProfileSpec(&result, &diffs, &cumDiffs, knownStart, knownEnd);
                    }
                    knownStart = i + 1;
                }
            }
        }
        else
        {
            // this is expected for ToF data (?) @todo
            // std::cerr << "Warning: a spectrum has no clear breaks.\n";
        }

        if (knownStart == 0 && knownEnd == diffs.size() - 1)
        {
            // std::cerr << "Warning: a spectrum has no clear breaks.\n";
            binProfileSpec(&result, &diffs, &cumDiffs, knownStart, knownEnd);
        }

        std::vector<ProfileBlock> groupedData;
        groupedData.reserve(result.size());

        // transfer the found groups into a representation accepted by the peak model fit
        for (size_t j = 0; j < result.size(); j++)
        {
            Block res = result[j];

            size_t entrySize = res.end + 1 - res.start + 4;
            ProfileBlock entry = initBlock(entrySize);

            for (size_t i = 0; i < entrySize - 4; i++)
            {
                size_t epos = i + res.start; // element position
                size_t rpos = i + 2;         // result vector position
                entry.intensity[rpos] = intensities_profile[epos];
            }
            for (size_t i = 0; i < entrySize - 4; i++)
            {
                size_t epos = i + res.start; // element position
                size_t rpos = i + 2;         // result vector position
                entry.mz[rpos] = mz_profile[epos];
            }
            // for (size_t i = 0; i < entrySize - 4; i++)
            // {
            //     size_t rpos = i + 2;
            //     entry.df[rpos] = true;
            //     entry.cumdf[rpos] = i + 1;
            // }

            // entry.cumdf[entrySize - 2] = entry.cumdf[entrySize - 3];
            // entry.cumdf[entrySize - 1] = entry.cumdf[entrySize - 3];

            // extrapolate two points to each size of the entry
            {
                entry.intensity[1] = entry.intensity[2] / 2;
                entry.intensity[0] = entry.intensity[2] / 4;
                entry.intensity[entrySize - 2] = entry.intensity[entrySize - 3] / 2;
                entry.intensity[entrySize - 1] = entry.intensity[entrySize - 3] / 4;

                double mzDiffFront = entry.mz[3] - entry.mz[2]; // @todo should there be two different distances?
                entry.mz[1] = entry.mz[2] - mzDiffFront;
                entry.mz[0] = entry.mz[2] - mzDiffFront * 2;

                double mzDiffBack = entry.mz[entrySize - 3] - entry.mz[entrySize - 4];
                entry.mz[entrySize - 2] = entry.mz[entrySize - 3] + mzDiffBack;
                entry.mz[entrySize - 1] = entry.mz[entrySize - 3] + mzDiffBack * 2;
            }

            // add traceability information from untreated spectrum
            entry.startPos = idxConvert[res.start];
            entry.endPos = idxConvert[res.end];

            groupedData.push_back(entry);
        }
        assert(!groupedData.empty());
        return groupedData;
    }
}