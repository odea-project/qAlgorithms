#include "qalgorithms_measurement_data.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_qpeaks.h"

#include <cmath>
#include <fstream>
#include <algorithm>
#include <sstream>

namespace qAlgorithms
{
    std::vector<std::vector<CentroidPeak>> transferCentroids(
        sc::MZML &data,
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
        const std::vector<double> mz = (*spectrum)[0];
        const std::vector<double> intensity = (*spectrum)[1];
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
            maxIdx = (*intensity)[i] > (*intensity)[maxIdx] ? i : maxIdx;
        }
        // quadratic interpolation using outer two points and the maximum
        const float x[3] = {float(scanNums[2]), float(scanNums[maxIdx]), float(scanNums[pos])};
        const float y[3] = {std::log((*intensity)[2]), std::log((*intensity)[maxIdx]), std::log((*intensity)[pos])};
        auto coeffs = interpolateQuadratic(x, y);
#define INTERPOLATE(x) (std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])))
        // left side
        if ((*intensity)[maxIdx] == (*intensity)[2])
        {
            (*intensity)[0] = (*intensity)[2];
            (*intensity)[1] = (*intensity)[2];
        }
        else
        {
            // double tmp = std::exp(coeffs[0] + x_axis[0] * (coeffs[1] + x_axis[0] * coeffs[2]));
            (*intensity)[0] = INTERPOLATE(scanNums[0]);
            (*intensity)[1] = INTERPOLATE(scanNums[1]);
        }
        // right side
        if ((*intensity)[maxIdx] == (*intensity)[2])
        {
            (*intensity)[pos + 1] = (*intensity)[pos];
            (*intensity)[pos + 2] = (*intensity)[pos];
        }
        else
        {
            (*intensity)[pos + 1] = INTERPOLATE(scanNums[pos + 1]);
            (*intensity)[pos + 2] = INTERPOLATE(scanNums[pos + 2]);
        }
    }

    treatedData pretreatEIC(
        EIC &eic,
        std::vector<unsigned int> &binIdx,
        float expectedDifference)
    {
        std::vector<dataPoint> dataPoints;              // create vector of data points
        dataPoints.reserve(eic.interpolatedIDs.size()); // reserve memory for data points

        assert(is_sorted(eic.rententionTimes.begin(), eic.rententionTimes.end()));

        for (size_t i = 0; i < eic.scanNumbers.size(); ++i)
        {
            dataPoint dp(               // create data point
                eic.rententionTimes[i], // x-axis value
                eic.ints_area[i],       // y-axis value
                // eic.ints_height[i],
                true,                 // df value
                eic.DQSC[i],          // dqs centroid value
                eic.DQSB[i],          // dqs binning value
                eic.mz[i]);           // mz ratio
            dataPoints.push_back(dp); // add data point to vector
        }

        treatedData treatedData;
        treatedData.dataPoints.reserve(dataPoints.size() * 2);

        binIdx.reserve(dataPoints.size() * 2);

        unsigned int realIdx = 0; // this should be handled outside of this function
        static dataPoint zeroedPoint{0.f, 0.f, false, 0.f, 0.f, 0};
        // add the first two zeros to the dataPoints_new vector @todo skip this by doing log interpolation during the log transform
        for (int i = 0; i < 2; i++)
        {
            treatedData.dataPoints.push_back(zeroedPoint);
            binIdx.push_back(realIdx);
            treatedData.intensity.push_back(0);
            assert(binIdx.size() == treatedData.dataPoints.size());
        }

        // iterate over the data points
        size_t maxOfBlock = 0;
        size_t blockSize = 0; // size of the current block
        size_t countSubOneGap = 0;
        size_t countNoGap = 0;
        for (size_t pos = 0; pos < dataPoints.size() - 1; pos++)
        {
            blockSize++;
            treatedData.dataPoints.push_back(dataPoints[pos]);
            treatedData.intensity.push_back(dataPoints[pos].y);
            binIdx.push_back(realIdx);
            assert(binIdx.size() == treatedData.dataPoints.size());
            ++realIdx;
            const float delta_x = dataPoints[pos + 1].x - dataPoints[pos].x;

            size_t gapSize2 = 0;
            if (delta_x > 1.75 * expectedDifference)
            {
                gapSize2 = static_cast<int>(delta_x / expectedDifference) - 1;
                if (gapSize2 == 0)
                {
                    countSubOneGap++;
                }
            }

            if (delta_x > 1.75 * expectedDifference)
            { // gap detected
                // assert(gapSize2 != 0);
                const int gapSize = static_cast<int>(delta_x / expectedDifference + 0.25 * expectedDifference) - 1;
                if (gapSize < 4)
                {
                    // add gapSize interpolated datapoints @todo this can be zero
                    const float dy = std::pow(dataPoints[pos + 1].y / dataPoints[pos].y, 1.0 / float(gapSize + 1)); // dy for log interpolation
                    float interpolateDiff = delta_x / (gapSize + 1);
                    for (int i = 1; i <= gapSize; i++)
                    {
                        binIdx.push_back(realIdx);
                        treatedData.dataPoints.emplace_back(
                            dataPoints[pos].x + i * interpolateDiff, // retention time
                            dataPoints[pos].y * std::pow(dy, i),     // intensity
                            false,                                   // interpolated point
                            0.f,                                     // DQSC
                            0.f,                                     // DQSB
                            0.f);                                    // mz
                        treatedData.intensity.push_back(dataPoints[pos].y * std::pow(dy, i));
                    }
                    assert(binIdx.size() == treatedData.dataPoints.size());
                }
            }
            else if (gapSize2 == 0) // no gap found
            {
                countNoGap++;
                assert(gapSize2 == 0);
                if (dataPoints[maxOfBlock].y < dataPoints[pos].y)
                {
                    maxOfBlock = pos;
                }
            }
        } // end of for loop
        // last element
        blockSize++;
        treatedData.dataPoints.push_back(dataPoints.back());
        treatedData.intensity.push_back(dataPoints.back().y);
        binIdx.push_back(realIdx);

        // END OF BLOCK, EXTRAPOLATION STARTS @todo move this into its own function
        assert(blockSize == eic.cenID.size());
        // assert(pos == blockSize - 1);
        // add 4 datapoints (two extrapolated [end of current block] and two zeros
        // [start of next block]) extrapolate the first two datapoints of this block

        const dataPoint dp_startOfBlock = treatedData.dataPoints[2];
        // check if the maximum of the block is the first or last data point
        if (maxOfBlock == blockSize - 1 || dataPoints[maxOfBlock].x == dp_startOfBlock.x)
        {
            // extrapolate the left side using the first non-zero data point (i.e, the start of the block)
            for (int i = 0; i < 2; i++)
            {
                // LEFT SIDE
                dataPoint &dp_left = treatedData.dataPoints[i];
                dp_left.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                dp_left.y = dp_startOfBlock.y;

                // RIGHT SIDE
                treatedData.dataPoints.emplace_back(
                    dataPoints[blockSize - 1].x + float(i + 1) * expectedDifference, // x-axis
                    dataPoints[blockSize - 1].y,                                     // intensity
                    false,                                                           // df
                    0.f,                                                             // DQSC
                    0.f,                                                             // DQSB
                    0.f);                                                            // mz
                treatedData.intensity.push_back(dataPoints[blockSize - 1].y);
                binIdx.push_back(realIdx);
                assert(binIdx.size() == treatedData.dataPoints.size());
            }
        }
        else
        {
            const float x[3] = {0.f, dataPoints[maxOfBlock].x - dp_startOfBlock.x, dataPoints[blockSize - 1].x - dp_startOfBlock.x};
            const float y[3] = {std::log(dp_startOfBlock.y), std::log(dataPoints[maxOfBlock].y), std::log(dataPoints[blockSize - 1].y)};
            // float b0, b1, b2;
            // calculateCoefficients(x, y, b0, b1, b2);
            auto coeffs = interpolateQuadratic(x, y);
            // assert(coeffs[0] == b0);
            // extrapolate the left side of the block
            for (int i = 0; i < 2; i++)
            {
                dataPoint &curr_dp = treatedData.dataPoints[0 + i];
                curr_dp.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                const float x = curr_dp.x - dp_startOfBlock.x;
                curr_dp.y = std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2]));
            }
            // add the extrapolated data points to the right side of the block
            for (int i = 0; i < 2; i++)
            {
                const float dp_x = dataPoints[blockSize - 1].x + float(i + 1) * expectedDifference;
                const float x = dp_x - dp_startOfBlock.x;
                treatedData.dataPoints.emplace_back(
                    dp_x,                                                  // x-axis
                    std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])), // intensity
                    false,                                                 // df
                    0.f,                                                   // DQSC
                    0.f,                                                   // DQSB
                    0.f);                                                  // mz
                treatedData.intensity.push_back(std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])));
                binIdx.push_back(realIdx);
                assert(binIdx.size() == treatedData.dataPoints.size());
            }
        }

        assert(binIdx.size() == treatedData.dataPoints.size());
        assert(binIdx.size() == treatedData.intensity.size());
        assert(treatedData.dataPoints.back().y == treatedData.intensity.back()); // works
        assert(treatedData.dataPoints.size() == eic.interpolatedDQSB.size());

        treatedData.cenIDs = eic.interpolatedIDs;
        return treatedData;
    } // end of pretreatEIC

#pragma region "connective functions between centroiding and binning"

#pragma endregion "connective functions between centroiding and binning"

#pragma region "find centroids"
    inline float calcRTDiff(const std::vector<double> *retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times->size(); ++i)
        {
            sum += (*retention_times)[i] - (*retention_times)[i - 1];
        }
        return sum / (retention_times->size() - 1);
    }

    std::vector<std::vector<CentroidPeak>> findCentroids_MZML( // this function needs to be split @todo
        sc::MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const std::string polarity,
        const bool ms1only)
    {
        // this is only relevant when reading in pre-centroided data
        bool displayPPMwarning = false;
        if (PPM_PRECENTROIDED == -INFINITY)
        {
            PPM_PRECENTROIDED = 0.25; // global variable, this is a bad solution @todo
        }
        else
        {
            displayPPMwarning = true;
        }

        std::vector<std::string> spectrum_mode = data.get_spectra_mode();         // get spectrum mode (centroid or profile)
        std::vector<std::string> spectrum_polarity = data.get_spectra_polarity(); // get spectrum polarity (positive or negative)
        std::vector<int> indices = data.get_spectra_index();                      // get all indices
        std::vector<int> ms_levels = data.get_spectra_level();                    // get all MS levels
        std::vector<int> num_datapoints = data.get_spectra_array_length();        // get number of data points
        double expectedDifference_mz = 0.0;                                       // expected difference between two consecutive x-axis values
        assert(!indices.empty() && num_datapoints[0] > 4);

        // CHECK IF CENTROIDED SPECTRA
        size_t num_centroided_spectra = std::count(spectrum_mode.begin(), spectrum_mode.end(), "centroid");
        if (num_centroided_spectra > spectrum_mode.size() / 2) // in profile mode sometimes centroided spectra appear as well @todo is 2 a good idea?
        {
            std::cerr << "Centroided data is not supported in this version of qAlgorithms!\n"
                      << "Warning: qAlgorithms is intended for profile spectra. A base uncertainty of "
                      << PPM_PRECENTROIDED << " ppm is assumed for all supplied centroids\n";
            exit(1);
            // for (size_t i = 1; i < indices.size(); i++) // i can be 0 briefly if there is a scan missing between 1. and 2. element
            // {
            //   if (retention_times[i] - retention_times[i - 1] > rt_diff * 1.75)
            //   {
            //     addEmpty[i - 1]++;
            //     retention_times[i - 1] += rt_diff * 1.75;
            //     convertRT.push_back(retention_times[i - 1]); // convertRT[scan] = retention time of centroid
            //     i--;
            //   }
            //   else
            //   {
            //     convertRT.push_back(retention_times[i - 1]); // convertRT[scan] = retention time of centroid
            //   }
            // }
            // convertRT.push_back(retention_times[indices.size() - 1]);
            // assert(addEmpty.size() == indices.size() + 1);
            // return transferCentroids(data, indices, retention_times, start_index, PPM_PRECENTROIDED); // this should be on a per-spectrum basis
        }

        std::vector<int> selectedIndices;
        selectedIndices.reserve(indices.size());
        size_t centroidCount = 0;

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
            if (spectrum_mode[i] != "profile")
            {
                centroidCount++;
                continue;
            }

            // if begin and end indices are to be suppported, this is the place.
            // it's probably better to use retention times for that, anyway
            selectedIndices.push_back(indices[i]);
        }
        if (centroidCount != 0)
        {
            std::cerr << "Warning: removed " << centroidCount << " centroided spectra from measurement.\n";
        }
        if (selectedIndices.empty())
        {
            std::vector<std::vector<CentroidPeak>> empty;
            assert(empty.empty());
            return empty;
        }

        std::vector<double> retention_times = data.get_spectra_rt(selectedIndices);
        rt_diff = calcRTDiff(&retention_times);

        selectedIndices.shrink_to_fit();

        std::vector<std::vector<CentroidPeak>> centroids(selectedIndices.size());

        // take spectrum at half length to avoid potential interference from quality control scans in the instrument
        const std::vector<std::vector<double>> data_vec = data.get_spectrum(selectedIndices[selectedIndices.size() / 2]);
        expectedDifference_mz = calcExpectedDiff(&data_vec);

        // determine where the peak finding will interpolate points and pass this information
        // to the binning step. addEmpty contains the number of empty scans to be added into
        // the qCentroids object at the given position. convertRT can later be used to look up
        // the retention time by the scan number, so that memory usage is reduced during binning
        // (note: doesn't have a significant effect, consider removal for readability @todo)

        assert(convertRT.empty());
        convertRT.reserve(selectedIndices.size());
        convertRT.push_back(NAN); // first scan is index 1
        std::vector<size_t> relativeIndex(selectedIndices.size(), 0);
        size_t newIndex = 1;

        for (size_t i = 0; i < selectedIndices.size() - 1; i++)
        {
            assert(i >= 0);
            if (retention_times[i + 1] - retention_times[i] > rt_diff * 1.75)
            {
                retention_times[i] += rt_diff * 1.75;
                convertRT.push_back(retention_times[i]);
                i--;
            }
            else
            {
                convertRT.push_back(retention_times[i]); // convertRT[scan] = retention time of centroid
                relativeIndex[i] = newIndex;
            }
            newIndex++;
        }
        relativeIndex.back() = newIndex;
        convertRT.push_back(retention_times.back());
        assert(convertRT.size() == newIndex + 1); // ensure that every index has an assigned RT

        for (size_t i = 0; i < selectedIndices.size(); ++i)
        {
            const std::vector<std::vector<double>> spectrum = data.get_spectrum(selectedIndices[i]);
            // inter/extrapolate data, and identify data blocks @todo these should be two different functions
            const auto treatedData = pretreatDataCentroids(&spectrum, expectedDifference_mz);
            assert(relativeIndex[i] != 0);
            centroids[i] = findCentroids(&treatedData, relativeIndex[i]); // find peaks in data blocks of treated data
        }
        if (!displayPPMwarning)
        {
            PPM_PRECENTROIDED = -INFINITY; // reset value before the next function call
        }
        return centroids;
    }

    constexpr ProfileBlock blockStart()
    {
        ProfileBlock p;
        p.df.reserve(16);
        p.intensity.reserve(16);
        p.mz.reserve(16);
        return p;
    }

    std::vector<ProfileBlock> pretreatDataCentroids(const std::vector<std::vector<double>> *spectrum, float expectedDifference)
    {
        // note on double precision values: when using floats, results are different enough
        // to cause different behaviour for interpolation and block termination. Doubles are
        // used here since the loss of precision during exponentiation etc. is not taken into
        // account otherwise. Around 1000 centroids less than otherwise are produced for test cases.
        std::vector<double> intensities_profile;
        std::vector<double> mz_profile;
        intensities_profile.reserve((*spectrum)[0].size() / 2);
        mz_profile.reserve((*spectrum)[0].size() / 2);
        // Depending on the vendor, a profile contains a lot of points with intensity 0.
        // These were added by the vendor software and must be removed prior to processing.
        for (size_t i = 0; i < (*spectrum)[0].size(); ++i)
        {
            if ((*spectrum)[1][i] == 0.0)
            {
                continue; // skip values with no intensity @todo minimum intensity?
            }
            intensities_profile.push_back((*spectrum)[1][i]);
            mz_profile.push_back((*spectrum)[0][i]);
        }
        assert(!intensities_profile.empty());
        assert(!mz_profile.empty());

        std::vector<ProfileBlock> subProfiles;

        size_t blockSize = 0;
        ProfileBlock currentBlock = blockStart(); // initialised with two zeroed values in each vector
        for (size_t pos = 0; pos < mz_profile.size() - 1; pos++)
        {
            blockSize++;

            currentBlock.intensity.push_back(intensities_profile[pos]);
            currentBlock.mz.push_back(mz_profile[pos]);
            currentBlock.df.push_back(true);

            const double delta_mz = mz_profile[pos + 1] - mz_profile[pos];
            assert(delta_mz > 0);

            // 1.75 is used to round up asymmetrically. This parameter should be defined in a dynamic manner @todo
            if (delta_mz > 1.75 * expectedDifference)
            { // gap detected
                // either interpolate or break the block up
                if (delta_mz < 4.25 * expectedDifference) // at most three points can be interpolated, tolerated increase is < 0.1 per point
                {
                    // interpolate
                    // round up the number of points starting at 0.75
                    const int gapSize = static_cast<int>(delta_mz / expectedDifference + 0.25 * expectedDifference) - 1;
                    assert(gapSize < 4);
                    double interpolateDiff = delta_mz / (gapSize + 1);
                    const double dy = std::pow(intensities_profile[pos + 1] / intensities_profile[pos],
                                               1.0 / double(gapSize + 1)); // dy for log interpolation ; 1 if gapsize == 0
                    for (int i = 0; i < gapSize; i++)
                    {
                        currentBlock.intensity.push_back(intensities_profile[pos] * std::pow(dy, i + 1));
                        currentBlock.mz.push_back(mz_profile[pos] + (i + 1) * interpolateDiff);
                        currentBlock.df.push_back(false);
                    }
                }
                else
                {
                    if (blockSize > 4)
                    {
                        subProfiles.push_back(currentBlock);
                    }
                    currentBlock = blockStart(); // reset block for next iteration
                    blockSize = 0;
                }
            }
            else
            {
                // @todo reason about why these limit values are used, why take the mean for updating the expected diff?
                if (delta_mz > 0.8 * expectedDifference && delta_mz < 1.2 * expectedDifference)
                {
                    expectedDifference = (expectedDifference + delta_mz) * 0.5;
                }
            }
        } // end of for loop
        if (blockSize > 3)
        {
            // special case: last point. Add the last element for mz and intensity if the block did not terminate
            currentBlock.intensity.push_back(intensities_profile.back());
            currentBlock.mz.push_back(mz_profile.back());
            currentBlock.df.push_back(true);
            subProfiles.push_back(currentBlock);
        }
        return subProfiles;
    }

#pragma endregion "find centroids"

#pragma region "find peaks"
    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &EICs,
        float rt_diff)
    {
        std::vector<FeaturePeak> peaks;    // return vector for feature list
        peaks.reserve(EICs.size() / 4);    // should be enough to fit all features without reallocation
        std::vector<FeaturePeak> tmpPeaks; // add features to this before pasting into FL

        for (size_t i = 0; i < EICs.size(); ++i) // loop over all data
        {
            auto currentEIC = EICs[i];
            if (currentEIC.scanNumbers.size() < 5)
            {
                continue; // skip due to lack of data, i.e., degree of freedom will be zero
            }
            // if (currentEIC.interpolations)
            // {
            //     continue;
            // }

            std::vector<unsigned int> binIndexConverter;
            treatedData treatedData = pretreatEIC(currentEIC, binIndexConverter, rt_diff); // inter/extrapolate data, and identify data blocks
            findFeatures(tmpPeaks, treatedData);
            if (tmpPeaks.empty())
            {
                continue;
            }
            for (size_t j = 0; j < tmpPeaks.size(); j++)
            {
                auto currentPeak = tmpPeaks[j];
                assert(currentPeak.idxPeakEnd < binIndexConverter.size());
                currentPeak.idxBin = i;
                // the end point is only correct if it is real. Check if the next point
                // has the same index - if yes, -1 to end index
                unsigned int tmpIdx = currentPeak.idxPeakEnd;
                currentPeak.idxPeakEnd = binIndexConverter[currentPeak.idxPeakEnd];
                if (tmpIdx + 1 != binIndexConverter.size())
                {
                    if (binIndexConverter[tmpIdx] == binIndexConverter[tmpIdx + 1])
                    {
                        currentPeak.idxPeakEnd--;
                    }
                }
                auto tmp = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.mz,
                                                       currentPeak.idxPeakStart, currentPeak.idxPeakEnd);
                currentPeak.mz = tmp.first;
                currentPeak.mzUncertainty = tmp.second;
                currentPeak.DQSC = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.DQSC,
                                                               currentPeak.idxPeakStart, currentPeak.idxPeakEnd)
                                       .first;
                currentPeak.DQSB = weightedMeanAndVariance_EIC(&currentEIC.ints_area, &currentEIC.DQSB,
                                                               currentPeak.idxPeakStart, currentPeak.idxPeakEnd)
                                       .first;
                peaks.push_back(std::move(currentPeak)); // remove 2D structure of FL
            }

            tmpPeaks.clear();
        }
        return peaks;
    }
}