#include "qalgorithms_measurement_data.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"

#include <cmath>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

#include <random> // only temporarily needed

namespace qAlgorithms
{

    inline std::array<double, 3> interpolateQuadratic(const float *x, const float *y)
    {
        double x1 = x[0], y1 = y[0];
        double x2 = x[1], y2 = y[1];
        double x3 = x[2], y3 = y[2];

        double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);

        double a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
        double b = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
        double c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

        return {c, b, a};
    }

#pragma region "Feature Detection"
    void extrapolateEIC(const std::vector<size_t> scanNums, std::vector<float> *intensity)
    {
        // x-axis can be either mz (for centroids) or RT (for features)
        assert(scanNums.size() > 8); // at least 9; five real points and four edge points
        assert(scanNums.size() == intensity->size());
        size_t pos = scanNums.size() - 3; // index of last real point
        // auto maxInt = std::max(intensity->begin(), intensity->end());
        // size_t maxIdx = std::distance(intensity->begin(), maxInt);
        size_t maxIdx = 0;
        for (size_t i = 2; i < scanNums.size() - 2; i++)
        {
            maxIdx = intensity->at(i) > intensity->at(maxIdx) ? i : maxIdx;
        }
        // quadratic interpolation using outer two points and the maximum
        const float x[3] = {float(scanNums[2]), float(scanNums[maxIdx]), float(scanNums[pos])};
        const float y[3] = {std::log(intensity->at(2)), std::log(intensity->at(maxIdx)), std::log(intensity->at(pos))};
        auto coeffs = interpolateQuadratic(x, y);
#define INTERPOLATE(x) (std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])))
        // left side
        if (intensity->at(maxIdx) == intensity->at(2))
        {
            intensity->at(0) = intensity->at(2);
            intensity->at(1) = intensity->at(2);
        }
        else
        {
            // double tmp = std::exp(coeffs[0] + x_axis[0] * (coeffs[1] + x_axis[0] * coeffs[2]));
            intensity->at(0) = INTERPOLATE(scanNums[0]);
            intensity->at(1) = INTERPOLATE(scanNums[1]);
        }
        // right side
        if (intensity->at(maxIdx) == intensity->at(2))
        {
            intensity->at(pos + 1) = intensity->at(pos);
            intensity->at(pos + 2) = intensity->at(pos);
        }
        else
        {
            intensity->at(pos + 1) = INTERPOLATE(scanNums[pos + 1]);
            intensity->at(pos + 2) = INTERPOLATE(scanNums[pos + 2]);
        }
    }

    treatedData pretreatEIC(
        EIC &eic,
        float expectedDifference,
        size_t maxScan)
    {
        // @todo this function just copies data from the eic output, the respective parts of the binning
        // and feature detection module should be reworked such that it doesn't need to exist
        std::vector<dataPoint> dataPoints_internal;
        dataPoints_internal.reserve(eic.interpolatedIDs.size());

        assert(is_sorted(eic.rententionTimes.begin(), eic.rententionTimes.end()));

        for (size_t i = 0; i < eic.scanNumbers.size(); ++i)
        {
            dataPoint dp(
                eic.rententionTimes[i],
                eic.ints_area[i],
                true); // the point is not interpolated
            dataPoints_internal.push_back(dp);
        }

        size_t maxSize = dataPoints_internal.size() * 2; // we do not know how many gaps there are beforehand
        treatedData treatedData;
        treatedData.dataPoints.reserve(maxSize);

        unsigned int realIdx = 0; // this should be handled outside of this function @todo
        static dataPoint zeroedPoint{0, 0, false};
        // add the first two zeros to the dataPoints_new vector @todo skip this by doing log interpolation during the log transform
        for (int i = 0; i < 2; i++)
        {
            treatedData.dataPoints.push_back(zeroedPoint);
            treatedData.intensity.push_back(0);
        }

        size_t maxOfBlock = 0;
        size_t blockSize = 0; // size of the current block
        for (size_t pos = 0; pos < dataPoints_internal.size() - 1; pos++)
        {
            blockSize++;
            treatedData.dataPoints.push_back(dataPoints_internal[pos]);
            treatedData.intensity.push_back(dataPoints_internal[pos].y);
            ++realIdx;
            const float delta_x = dataPoints_internal[pos + 1].x - dataPoints_internal[pos].x;

            if (delta_x > 1.75 * expectedDifference)
            {
                // gap detected
                const int gapSize = static_cast<int>(delta_x / expectedDifference + 0.25 * expectedDifference) - 1;
                if (gapSize < 4)
                {
                    // add gapSize interpolated datapoints @todo this can be zero
                    const float dy = std::pow(dataPoints_internal[pos + 1].y / dataPoints_internal[pos].y, 1.0 / float(gapSize + 1)); // dy for log interpolation
                    float interpolateDiff = delta_x / (gapSize + 1);
                    for (int i = 1; i <= gapSize; i++)
                    {
                        treatedData.dataPoints.emplace_back(
                            dataPoints_internal[pos].x + i * interpolateDiff, // retention time
                            dataPoints_internal[pos].y * std::pow(dy, i),     // intensity
                            false);                                           // interpolated point
                        treatedData.intensity.push_back(dataPoints_internal[pos].y * std::pow(dy, i));
                    }
                }
            }
            else
            {
                if (dataPoints_internal[maxOfBlock].y < dataPoints_internal[pos].y)
                {
                    maxOfBlock = pos;
                }
            }
        } // end of for loop
        // last element
        blockSize++;
        treatedData.dataPoints.push_back(dataPoints_internal.back());
        treatedData.intensity.push_back(dataPoints_internal.back().y);

        // END OF BLOCK, EXTRAPOLATION STARTS @todo move this into its own function
        assert(blockSize == eic.cenID.size());
        // add 4 datapoints (two extrapolated [end of current block] and two zeros
        // [start of next block]) extrapolate the first two datapoints of this block

        const dataPoint dp_startOfBlock = treatedData.dataPoints[2];
        // check if the maximum of the block is the first or last data point.
        // in this case, it is not possible to extrapolate using the quadratic form
        // @todo is it sensible to use quadratic extrapolation in the first place? This
        // could introduce a bias towards phantom signals and only makes sense from the
        // instrumentation side of things with centroids in a FT-HRMS setup
        // if (maxOfBlock == blockSize - 1 || maxOfBlock == 0)
        // {
        // extrapolate the left side using the first non-zero data point (i.e, the start of the block)
        treatedData.dataPoints[0].x = dp_startOfBlock.x - 2 * expectedDifference;
        treatedData.dataPoints[1].x = dp_startOfBlock.x - expectedDifference;
        treatedData.dataPoints[0].y = dp_startOfBlock.y / 4;
        treatedData.dataPoints[1].y = dp_startOfBlock.y / 2;

        treatedData.intensity[0] = dp_startOfBlock.y / 4;
        treatedData.intensity[1] = dp_startOfBlock.y / 2;

        size_t l = treatedData.dataPoints.size() - 2;
        auto endPoint = treatedData.dataPoints[l];

        treatedData.dataPoints[l + 1].x = endPoint.x + 2 * expectedDifference;
        treatedData.dataPoints[l].x = endPoint.x + expectedDifference;
        treatedData.dataPoints[l + 1].y = endPoint.y / 4;
        treatedData.dataPoints[l].y = endPoint.y / 2;

        treatedData.intensity[l + 1] = endPoint.y / 4;
        treatedData.intensity[l] = endPoint.y / 2;

        assert(treatedData.intensity[0] > 0);
        // assert(!isnanf(treatedData.intensity.back())); // @todo this is os-specific, windows does not have access to isnanf
        // }
        // else
        // {
        //     const float x[3] = {0.f, dataPoints_internal[maxOfBlock].x - dp_startOfBlock.x, dataPoints_internal[blockSize - 1].x - dp_startOfBlock.x};
        //     const float y[3] = {std::log(dp_startOfBlock.y), std::log(dataPoints_internal[maxOfBlock].y), std::log(dataPoints_internal[blockSize - 1].y)};

        //     auto coeffs = interpolateQuadratic(x, y);
        //     // extrapolate the left side of the block
        //     for (int i = 0; i < 2; i++)
        //     {
        //         dataPoint &curr_dp = treatedData.dataPoints[0 + i];
        //         curr_dp.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
        //         const float x = curr_dp.x - dp_startOfBlock.x;
        //         curr_dp.y = std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2]));
        //     }
        //     // add the extrapolated data points to the right side of the block
        //     for (int i = 0; i < 2; i++)
        //     {
        //         const float dp_x = dataPoints_internal[blockSize - 1].x + float(i + 1) * expectedDifference;
        //         const float x = dp_x - dp_startOfBlock.x;
        //         treatedData.dataPoints.emplace_back(
        //             dp_x,                                                  // retention time
        //             std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])), // intensity
        //             false);                                                // df
        //         treatedData.intensity.push_back(std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])));
        //     }
        //     assert(treatedData.intensity[0] > 0);
        //     assert(!isnanf(treatedData.intensity.back()));
        // }

        assert(treatedData.dataPoints.size() == treatedData.intensity.size());
        assert(treatedData.dataPoints.back().y == treatedData.intensity.back()); // works
        // assert(treatedData.dataPoints.size() == eic.interpolatedDQSB.size()); // @todo redo this in good

        treatedData.cenIDs = eic.interpolatedIDs;
        treatedData.lowestScan = eic.scanNumbers.front() - 2;
        treatedData.largestScan = eic.scanNumbers.back() + 2;
        // assert(binIdx.size() == treatedData.largestScan - treatedData.lowestScan + 1 + 4); // extrapolations
        treatedData.cumulativeDF.reserve(treatedData.dataPoints.size());
        treatedData.cumulativeDF.push_back(0);
        size_t accum = 0;
        for (size_t i = 1; i < treatedData.dataPoints.size(); i++)
        {
            // i starts at 1 since the first element is always 0
            accum += treatedData.dataPoints[i].df ? 1 : 0;
            treatedData.cumulativeDF.push_back(accum); // @todo this is not vectorised!
        }
        assert(treatedData.largestScan < maxScan);
        return treatedData;
    }

    std::vector<FeaturePeak> findPeaks_QBIN(std::vector<EIC> &EICs, float rt_diff, size_t maxScan)
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
            // if (currentEIC.interpolations)
            // {
            //     continue;
            // }

            treatedData treatedData = pretreatEIC(currentEIC, rt_diff, maxScan); // inter/extrapolate data, and identify data blocks
            findFeatures(tmpPeaks, treatedData);
            if (tmpPeaks.empty())
            {
                continue;
            }
            for (size_t j = 0; j < tmpPeaks.size(); j++)
            {
                FeaturePeak currentPeak = tmpPeaks[j];

                currentPeak.scanPeakStart = treatedData.lowestScan + currentPeak.idxPeakStart;
                currentPeak.scanPeakEnd = treatedData.lowestScan + currentPeak.idxPeakEnd;
                assert(currentPeak.scanPeakEnd < maxScan);
                // assert(currentPeak.idxPeakEnd < binIndexConverter.size());
                currentPeak.idxBin = i;
                // the end point is only correct if it is real. Check if the next point
                // has the same index - if yes, -1 to end index
                // currentPeak.idxPeakStart = binIndexConverter[currentPeak.idxPeakStart];
                // unsigned int tmpIdx = currentPeak.idxPeakEnd;
                // currentPeak.idxPeakEnd = binIndexConverter[currentPeak.idxPeakEnd];
                // assert(currentPeak.idxPeakEnd < currentEIC.ints_area.size());
                // if (tmpIdx + 1 != binIndexConverter.size())
                // {
                //     if (binIndexConverter[tmpIdx] == binIndexConverter[tmpIdx + 1])
                //     {
                //         currentPeak.idxPeakEnd--;
                //     }
                // }
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

                unsigned int limit_L = treatedData.cumulativeDF[currentPeak.idxPeakStart];
                limit_L = std::min(limit_L, limit_L - 1); // uint underflows, so no issues.
                unsigned int limit_R = treatedData.cumulativeDF[currentPeak.idxPeakEnd] - 1;
                assert(limit_L < limit_R);

                // @todo these are a temporary solution, rework bins to already contain interpolations
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

#pragma region "find centroids"

    std::vector<std::vector<CentroidPeak>> transferCentroids(
        StreamCraft::MZML &data,
        std::vector<int> &indices,
        std::vector<double> &retention_times,
        const int start_index,
        double PPMerror)
    {
        std::vector<std::vector<CentroidPeak>> centroids(indices.size());
        // #pragma omp parallel for
        for (size_t i = 0; i < indices.size(); ++i) // loop over all indices
        {
            const int index = indices[i]; // spectrum index
            if (index < start_index)
            {
                continue; // skip due to index
            }
            // get the spectrum
            std::vector<std::vector<double>> spectrum = data.get_spectrum(index);
            if (spectrum.empty())
            {
                std::cerr << "Error: spectrum decode at position " << index << " failed.\n";
                continue;
            }

            for (size_t j = 0; j < spectrum[0].size(); j++)
            {
                if (spectrum[1][j] < 750)
                {
                    continue;
                }

                centroids[i].push_back(CentroidPeak());
                centroids[i].back().scanNumber = index;
                centroids[i].back().mz = spectrum[0][j];
                centroids[i].back().area = spectrum[1][j];
                centroids[i].back().height = spectrum[1][j];
                // centroids[i].back().retentionTime = retention_times[i]; // @todo fix this
                centroids[i].back().DQSC = -1.0;
                centroids[i].back().mzUncertainty = spectrum[0][j] * PPMerror * 10e-6; // 0.25 ppm default
            }
        }
        return centroids;
    }

    double calcExpectedDiff(const std::vector<std::vector<double>> *spectrum)
    {
        const std::vector<double> mz = spectrum->at(0); // @todo make this a named struct
        const std::vector<double> intensity = spectrum->at(1);
        const size_t numPoints = mz.size();         // number of data points
        const size_t upperLimit = numPoints * 0.05; // check lowest 5% for expected difference
        double expectedDifference = 0.0;
        std::vector<double> differences;
        differences.reserve(numPoints / 2);
        for (size_t i = 0; i < numPoints - 1; i++)
        {
            if (intensity[i] == 0 || intensity[i] == 0)
            {
                continue;
            }
            differences.push_back(mz[i + 1] - mz[i]);
        }
        std::sort(differences.begin(), differences.end());
        for (size_t i = 0; i < upperLimit; i++)
        {
            expectedDifference += differences[i];
        }
        expectedDifference /= upperLimit;
        assert(expectedDifference > 0);
        return expectedDifference;
    }

    inline float calcRTDiff(const std::vector<double> *retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times->size(); ++i)
        {
            sum += retention_times->at(i) - retention_times->at(i - 1);
        }
        return sum / (retention_times->size() - 1);
    }

    std::vector<size_t> makeRelativeIndex(std::vector<double> *retention_times,
                                          std::vector<float> &convertRT, // @todo merge into struct with actual RTs
                                          size_t countSelected,
                                          float *rt_diff) // this gets mutated, when it should probably not exist at all
    {
        *rt_diff = calcRTDiff(retention_times); // this is the wrong approach - the scan interval is not static
        assert(convertRT.empty());
        convertRT.reserve(countSelected + 4);
        // first two scans do not have retention times @todo this will lead to slightly wrong results, should be fine due to void time
        convertRT.push_back(std::max(float(retention_times->front()) - 2 * *rt_diff, float(0)));
        convertRT.push_back(std::max(float(retention_times->front() - 0.999 * *rt_diff), float(0)));

        // this is the scan counting only MS1 spectra. It starts at two so we don't run into
        // problems with our front extrapolation during feature construction. It shouldn't matter
        // anyway since the first two scans should not contain relevant information, but this is
        // less questionable than cutting out the first two mass spectra entirely - consider the
        // possibility that they were trimmed of void time beforehand!
        size_t abstractScanNumber = 2;

        std::vector<size_t> relativeIndex(countSelected, 0);

        for (size_t i = 0; i < countSelected; i++)
        {
            float scandiff = retention_times->at(i) - convertRT.back(); // avoids problem of iterating n - 1!
            assert(scandiff > 0);
            int gapSize = int((scandiff / *rt_diff) + 0.25 * *rt_diff); // round up at 0.75

            // correctedIndex[i] = i + gapSize;
            if (gapSize > 1)
            {
                std::cerr << "."; // @todo this is probably useless at best and probably does more harm than good
                float gapStep = scandiff / float(gapSize);
                for (int gap = 1; gap < gapSize + 1; gap++)
                {
                    assert(convertRT.back() + gap * gapStep > convertRT.back());
                    convertRT.push_back(convertRT.back() + gap * gapStep);
                }
                abstractScanNumber += gapSize;
            }
            if (convertRT.back() >= retention_times->at(i)) [[unlikely]]
            {
                // workaround for slightly misaligned scans, will need to be fixed with a full rework
                std::cerr << "Warning: bad estimation of scan no. " << abstractScanNumber << "\n";
                convertRT.pop_back();
                abstractScanNumber--;
            }

            // assert(convertRT.back() < retention_times[i]);
            convertRT.push_back(retention_times->at(i)); // convertRT[scan] = retention time of centroid
            relativeIndex[i] = abstractScanNumber;       // abstract scan with same index as the input data
            abstractScanNumber++;
        }
        // account for extrapolations at the back @todo the extrapolations should probably be a copile-time variable
        abstractScanNumber += 2;
        convertRT.push_back(retention_times->back() + *rt_diff);
        convertRT.push_back(retention_times->back() + *rt_diff + *rt_diff);
        assert(convertRT.size() == abstractScanNumber); // ensure that every index has an assigned RT
        return relativeIndex;
    }

    std::vector<unsigned int> getRelevantSpectra(StreamCraft::MZML &data,
                                                 const bool polarity,
                                                 const bool ms1only)
    {
        // accessor contains the indices of all spectra that should be fetched
        std::vector<unsigned int> accessor(data.number_spectra, 0);
        std::iota(accessor.begin(), accessor.end(), 0);

        std::vector<bool> spectrum_mode = data.get_spectra_mode(&accessor); // get spectrum mode (centroid or profile)
        std::vector<size_t> indices = data.get_spectra_index(&accessor);    // get all indices
        std::vector<int> ms_levels = data.get_spectra_level(&accessor);     // get all MS levels
        assert(!indices.empty());

        std::vector<unsigned int> selectedIndices;
        selectedIndices.reserve(indices.size());

        std::vector<bool> spectrum_polarity = data.get_spectra_polarity(&accessor); // get spectrum polarity (positive or negative)
        for (size_t i = 0; i < indices.size(); i++)
        {
            if (ms1only && ms_levels[i] != 1)
            {
                continue;
            }
            if (spectrum_polarity[i] != polarity)
            {
                continue;
            }
            if (!spectrum_mode[i])
            {
                continue;
            }

            selectedIndices.push_back(indices[i]);
        }
        return selectedIndices;
    }

    std::vector<CentroidPeak> findCentroids( // this function needs to be reworked further @todo
        StreamCraft::MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool polarity,
        const bool ms1only)
    {
        std::vector<unsigned int> selectedIndices = getRelevantSpectra(data, polarity, ms1only);
        if (selectedIndices.empty())
        {
            return std::vector<CentroidPeak>{};
        }

        std::vector<double> retention_times = data.get_spectra_RT(&selectedIndices);
        // for (size_t i = 0; i < retention_times.size() - 1; i++)
        // {
        //     std::cout << retention_times[i] << ",";
        // }
        // std::cout << retention_times.back();
        // exit(1);

        selectedIndices.shrink_to_fit();
        const size_t countSelected = selectedIndices.size();

        // determine where the peak finding will interpolate points and pass this information
        // to the binning step. addEmpty contains the number of empty scans to be added into
        // the qCentroids object at the given position. convertRT can later be used to look up
        // the retention time by the scan number, so that memory usage is reduced during binning
        // (note: doesn't have a significant effect, consider removal for readability @todo)
        // note 2: since the regression operates over a fixed window anyway, it should not be a
        // drastic change if the retention time is not actively adjusted. For the case of 1.75
        // difference in RT an error of 1 - 0.875 = 0.225 relative distance is made. The largest
        // issue occurs with 2.7499.... Here, the error is 0.75 / 3 = 0.25. We need to check how often
        // such a problem arises and if it negatively affects result accuracy.

        std::vector<size_t> relativeIndex = makeRelativeIndex(&retention_times, convertRT, countSelected, &rt_diff);

        std::vector<CentroidPeak> centroids;
        centroids.reserve(countSelected * 1000);
        centroids.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}); // dummy value used for binning
        for (size_t i = 0; i < countSelected; ++i)
        {
            const std::vector<std::vector<double>> spectrum = data.get_spectrum(selectedIndices[i]);
            const auto profileGroups = pretreatDataCentroids(&spectrum);
            findCentroidPeaks(&centroids, &profileGroups, relativeIndex[i], selectedIndices[i]);
        }
        for (unsigned int i = 0; i < centroids.size(); i++)
        {
            centroids[i].ID = i;
        }

        return centroids;
    }

    struct Block
    {
        size_t start;
        size_t end;
        bool insertCandidate = false;
    };

    void binProfileSpec(std::vector<Block> *result,
                        const std::vector<double> *diffs,
                        const std::vector<double> *cumDiffs, // indices into cumDiffs must be right-shifted by one!
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
        auto pmax = std::max_element(diffs->begin() + start, diffs->begin() + end);
        double max = *pmax;
        size_t maxPos = std::distance(diffs->begin(), pmax);

        if (max < critVal)
        {
            bool interpolate = false;
            if (max / 3 > critVal)
            {
                // @todo orbitrap data should never need interpolation, add a check for this
                interpolate = true; // there could be points inserted here to bridge the difference
                std::cout << "Possible interpolation of profile data\n";
            }

            // block is complete, add limits to result vector
            result->push_back({start, end + 1, interpolate}); // end + 1 since difference has one point less
            return;
        }
        if (maxPos == start || maxPos == end)
        {
            return;
        }

        // recursive split at max - different calling convention since we work with differences
        binProfileSpec(result, diffs, cumDiffs, start, maxPos - 1); // when setting the block, 1 is added to end
        binProfileSpec(result, diffs, cumDiffs, maxPos + 1, end);   // one past the max to avoid large value
    }

    inline ProfileBlock initBlock(size_t blocksize)
    {
        std::vector<float> mz_int(blocksize, 0);
        std::vector<unsigned int> cumdf(blocksize, 0);
        std::vector<bool> df(blocksize, false);
        ProfileBlock ret = {mz_int,
                            mz_int,
                            cumdf,
                            df};
        return ret;
    }

    std::vector<ProfileBlock> pretreatDataCentroids(const std::vector<std::vector<double>> *spectrum)
    {
        std::vector<double> intensities_profile;
        std::vector<double> mz_profile;
        std::vector<unsigned int> idxConvert; // used to trace back processing steps to untreated data

        { // remove zeroes
            const auto mz = spectrum->at(0);
            intensities_profile.reserve(mz.size() / 2);
            mz_profile.reserve(mz.size() / 2);
            // Depending on the vendor, a profile contains a lot of points with intensity 0.
            // These were added by the vendor software and must be removed prior to processing.
            for (unsigned int i = 0; i < mz.size(); ++i)
            {
                if (spectrum->at(1)[i] == 0.0)
                {
                    continue; // skip values with no intensity @todo minimum intensity?
                }
                intensities_profile.push_back(spectrum->at(1)[i]);
                mz_profile.push_back(mz[i]);
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
        binProfileSpec(&result, &diffs, &cumDiffs, 0, diffs.size() - 1);

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
            for (size_t i = 0; i < entrySize - 4; i++)
            {
                size_t rpos = i + 2;
                entry.df[rpos] = true;
                entry.cumdf[rpos] = i + 1;
            }

            entry.cumdf[entrySize - 2] = entry.cumdf[entrySize - 3];
            entry.cumdf[entrySize - 1] = entry.cumdf[entrySize - 3];

            // extrapolate two points to each size of the entry
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

            // add traceability information from untreated spectrum
            entry.startPos = idxConvert[res.start];
            entry.endPos = idxConvert[res.end];

            groupedData.push_back(entry);
        }
        assert(!groupedData.empty());
        return groupedData;
    }
}