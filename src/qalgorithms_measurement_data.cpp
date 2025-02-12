// qalgorithm_measurement_data.cpp

// internal
#include "qalgorithms_measurement_data.h"
#include "qalgorithms_utils.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_qpeaks.h"

// external
#include <cmath>
#include <fstream>
#include <algorithm>

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
                centroids[i].back().dqsCen = -1.0;
                centroids[i].back().mzUncertainty = spectrum[0][j] * PPMerror * 10e-6; // 0.25 ppm default
            }
        }
        return centroids;
    }

    double calcExpectedDiff(std::vector<double> &data)
    {
        const int numPoints = data.size(); // number of data points
        int upperLimit = numPoints * 0.05; // check lowest 5% for expected difference
        double expectedDifference = 0.0;
        // static approach: mean of the 8 lowest distances @todo why 8?
        std::vector<double> differences(numPoints - 1);
        for (int i = 0; i < numPoints - 1; i++)
        {
            differences[i] = data[i + 1] - data[i];
        }
        std::sort(differences.begin(), differences.end());
        for (int i = 0; i < upperLimit; i++)
        {
            expectedDifference += differences[i];
        }
        expectedDifference /= upperLimit;

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

    treatedData pretreatData(
        std::vector<dataPoint> &dataPoints,
        std::vector<unsigned int> &binIdx,
        float expectedDifference,
        const bool updateExpectedDifference)
    {
        static dataPoint zeroedPoint{0.f, 0.f, false, 0.f, 0.f, 0, 0.f};

        treatedData treatedData;
        treatedData.dataPoints.reserve(dataPoints.size() * 2);

        binIdx.reserve(dataPoints.size() * 2);

        unsigned int realIdx = 0; // this should be handled outside of this function
        // add the first two zeros to the dataPoints_new vector @todo skip this by doing log interpolation during the log transform
        for (int i = 0; i < 2; i++)
        {
            treatedData.dataPoints.push_back(zeroedPoint);
            binIdx.push_back(realIdx);
            treatedData.intensity.push_back(0);
            assert(binIdx.size() == treatedData.dataPoints.size());
        }
        treatedData.separators.push_back({0, 0}); // correctness check: start != end

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
                const int gapSize = static_cast<int>(delta_x / expectedDifference) - 1;
                if (gapSize < 4)
                {
                    // add gapSize interpolated datapoints @todo this can be zero
                    const float dy = std::pow(dataPoints[pos + 1].y / dataPoints[pos].y, 1.0 / float(gapSize + 1)); // dy for log interpolation
                    for (int i = 1; i <= gapSize; i++)
                    {
                        binIdx.push_back(realIdx);
                        treatedData.dataPoints.emplace_back(
                            dataPoints[pos].x + i * expectedDifference, // x-axis
                            dataPoints[pos].y * std::pow(dy, i),        // intensity
                            false,                                      // df
                            0.f,                                        // DQSC
                            0.f,                                        // DQSB
                            0,                                          // scanNumber
                            0.f);                                       // mz
                        treatedData.intensity.push_back(dataPoints[pos].y * std::pow(dy, i));
                    }
                    assert(binIdx.size() == treatedData.dataPoints.size());
                }
                else
                { // END OF BLOCK, EXTRAPOLATION STARTS @todo move this into its own function
                    // add 4 datapoints (two extrapolated [end of current block] and two zeros
                    // [start of next block]) extrapolate the first two datapoints of this block
                    if (blockSize < 5)
                    {
                        // delete all data points of the block in treatedData.dataPoints except the
                        // first two zeros marked by the separator.back()+2
                        auto it_startOfBlock = treatedData.dataPoints.begin() + treatedData.separators.back().start + 2;
                        treatedData.dataPoints.erase(it_startOfBlock, treatedData.dataPoints.end());
                        treatedData.intensity.erase(treatedData.intensity.begin() + treatedData.separators.back().start + 2, treatedData.intensity.end());
                        while (binIdx.size() != treatedData.dataPoints.size())
                        {
                            binIdx.pop_back();
                        }
                    }
                    else
                    {
                        const dataPoint dp_startOfBlock = treatedData.dataPoints[treatedData.separators.back().start + 2];
                        // check if the maximum of the block is the first or last data point
                        if (maxOfBlock == pos || dataPoints[maxOfBlock].x == dp_startOfBlock.x)
                        {
                            // extrapolate the left side using the first non-zero data point (i.e, the start of the block)
                            for (int i = 0; i < 2; i++)
                            {
                                // LEFT SIDE
                                dataPoint &dp_left = treatedData.dataPoints[treatedData.separators.back().start + i];
                                dp_left.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                                dp_left.y = dp_startOfBlock.y;

                                // RIGHT SIDE
                                treatedData.dataPoints.emplace_back(
                                    dataPoints[pos].x + float(i + 1) * expectedDifference, // x-axis
                                    dataPoints[pos].y,                                     // intensity
                                    false,                                                 // df
                                    0.f,                                                   // DQSC
                                    0.f,                                                   // DQSB
                                    0,                                                     // scanNumber
                                    0.f);                                                  // mz
                                treatedData.intensity.push_back(dataPoints[pos].y);
                                binIdx.push_back(realIdx);
                                assert(binIdx.size() == treatedData.dataPoints.size());
                            }
                        }
                        else
                        {
                            const float x[3] = {0.f, dataPoints[maxOfBlock].x - dp_startOfBlock.x, dataPoints[pos].x - dp_startOfBlock.x};
                            const float y[3] = {std::log(dp_startOfBlock.y), std::log(dataPoints[maxOfBlock].y), std::log(dataPoints[pos].y)};
                            // float b0, b1, b2;
                            // calculateCoefficients(x, y, b0, b1, b2);
                            auto coeffs = interpolateQuadratic(x, y);
                            // assert(coeffs[0] == b0);
                            // extrapolate the left side of the block
                            for (int i = 0; i < 2; i++)
                            {
                                dataPoint &curr_dp = treatedData.dataPoints[treatedData.separators.back().start + i];
                                curr_dp.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                                const float x = curr_dp.x - dp_startOfBlock.x;
                                curr_dp.y = std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2]));
                            }
                            // add the extrapolated data points to the right side of the block
                            for (int i = 0; i < 2; i++)
                            {
                                const float dp_x = dataPoints[pos].x + float(i + 1) * expectedDifference;
                                const float x = dp_x - dp_startOfBlock.x;
                                treatedData.dataPoints.emplace_back(
                                    dp_x,                                                  // x-axis
                                    std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])), // intensity
                                    false,                                                 // df
                                    0.f,                                                   // DQSC
                                    0.f,                                                   // DQSB
                                    0,                                                     // scanNumber
                                    0.f);                                                  // mz
                                treatedData.intensity.push_back(std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])));
                                binIdx.push_back(realIdx);
                                assert(binIdx.size() == treatedData.dataPoints.size());
                            }
                        }
                        // add the zeros to the treatedData.dataPoints vector to start the next block
                        // separator struct stores indices of first and last element
                        treatedData.separators.back().end = treatedData.dataPoints.size() - 1;
                        treatedData.separators.push_back({treatedData.dataPoints.size(), 0});
                        for (int i = 0; i < 2; i++)
                        {
                            treatedData.dataPoints.push_back(zeroedPoint);
                            treatedData.intensity.push_back(0);
                            binIdx.push_back(realIdx);
                            assert(binIdx.size() == treatedData.dataPoints.size());
                        }
                        maxOfBlock = pos + 1;
                    }
                    blockSize = 0; // reset the block size
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
                if (updateExpectedDifference && delta_x > 0.8 * expectedDifference && delta_x < 1.2 * expectedDifference)
                {
                    expectedDifference = (expectedDifference + delta_x) * 0.5; // update the expected difference
                }
            }
        } // end of for loop

        // std::cerr << countSubOneGap << ", -" << countNoGap << ", ";

        assert(binIdx.size() == treatedData.dataPoints.size());
        assert(binIdx.size() == treatedData.intensity.size());
        // delete the last two zeros // @todo why?
        binIdx.pop_back();
        binIdx.pop_back();
        treatedData.dataPoints.pop_back();
        treatedData.dataPoints.pop_back();
        treatedData.intensity.pop_back();
        treatedData.intensity.pop_back();
        assert(treatedData.dataPoints.back().y == treatedData.intensity.back()); // works

        treatedData.separators.pop_back(); // last element is constructed with a start index of datapoints.size()
        if (treatedData.separators.back().end != treatedData.dataPoints.size() - 1)
        {
            std::cerr << "measurement_data: incomplete block!\n"; // should never be the case
            treatedData.separators.back().end = treatedData.dataPoints.size() - 1;
        }

        return treatedData;
    } // end of pretreatData

#pragma region "connective functions between centroiding and binning"

    std::vector<dataPoint> qbinToDataPoint(EIC &eic)
    {
        std::vector<dataPoint> dataPoints;              // create vector of data points
        dataPoints.reserve(eic.scanNumbers.size() + 1); // reserve memory for data points

        if (!is_sorted(eic.rententionTimes.begin(), eic.rententionTimes.end())) // WILL BE DELETED IN THE FUTURE
        {
            auto compare = [&eic](size_t a, size_t b)
            {
                return eic.rententionTimes[a] < eic.rententionTimes[b];
            };
            std::vector<size_t> indices(eic.scanNumbers.size());
            std::iota(indices.begin(), indices.end(), 0);
            std::sort(indices.begin(), indices.end(), compare);
            for (size_t i = 0; i < eic.scanNumbers.size(); ++i)
            {
                dataPoint dp(                        // create data point
                    eic.rententionTimes[indices[i]], // x-axis value
                    eic.ints_area[indices[i]],       // y-axis value
                    // eic.ints_height[indices[i]],
                    true,                        // df value
                    eic.DQSC[indices[i]],        // dqs centroid value
                    eic.DQSB[indices[i]],        // dqs binning value
                    eic.scanNumbers[indices[i]], // scan number
                    eic.mz[indices[i]]);         // mz ratio
                dataPoints.push_back(dp);        // add data point to vector
            }
        }
        else
        {
            for (size_t i = 0; i < eic.scanNumbers.size(); ++i)
            {
                dataPoint dp(               // create data point
                    eic.rententionTimes[i], // x-axis value
                    eic.ints_area[i],       // y-axis value
                    // eic.ints_height[i],
                    true,                 // df value
                    eic.DQSC[i],          // dqs centroid value
                    eic.DQSB[i],          // dqs binning value
                    eic.scanNumbers[i],   // scan number
                    eic.mz[i]);           // mz ratio
                dataPoints.push_back(dp); // add data point to vector
            }
        }
        // add end point for later pretreatment
        dataPoint dp(                               // create data point
            std::numeric_limits<float>::infinity(), // x-axis value
            0.0,                                    // y-axis value
            false,                                  // df value
            0.0,                                    // dqs centroid value
            0.0,                                    // dqs binning value
            0,                                      // scan number
            0.0);                                   // mz ratio
        dataPoints.push_back(dp);
        return dataPoints;
    }
#pragma endregion "connective functions between centroiding and binning"

#pragma region "find centroids"
    float calcRTDiff(std::vector<double> &retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times.size(); ++i)
        {
            sum += retention_times[i] - retention_times[i - 1];
        }
        return sum / (retention_times.size() - 1);
    }

    // global variable, this is a demporary solution
    size_t NO_INTERPOLATE_COUNT = 0;
    size_t UNCERTAIN_BLOCK_END = 0;
    double AVERAGE_EXTRAPOLATION_DIFF = 0;

    std::vector<std::vector<CentroidPeak>> findCentroids_MZML( // this function needs to be split @todo
        sc::MZML &data,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool ms1only,
        const std::string polarity,
        const int start_index)
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
        double expectedDifference = 0.0;                                          // expected difference between two consecutive x-axis values
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

        std::vector<double> retention_times = data.get_spectra_rt(selectedIndices); // get retention times
        rt_diff = calcRTDiff(retention_times);                                      // retention time difference

        selectedIndices.shrink_to_fit();

        std::vector<std::vector<CentroidPeak>> centroids(selectedIndices.size()); // create vector of peaks

        // CALCULATE EXPECTED DIFFERENCE & CHECK FOR ZEROS
        std::vector<std::vector<double>> data_vec = data.get_spectrum(selectedIndices[start_index]); // get first spectrum (x-axis)
        expectedDifference = calcExpectedDiff(data_vec[0]);                                          // calculate expected difference & check if Orbitrap

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
            std::vector<std::vector<double>> spectrum = data.get_spectrum(selectedIndices[i]);
            // inter/extrapolate data, and identify data blocks @todo these should be two different functions
            auto treatedData = pretreatDataCentroids(spectrum, expectedDifference);
            assert(relativeIndex[i] != 0);
            centroids[i] = findCentroids(treatedData, relativeIndex[i]); // find peaks in data blocks of treated data
        }
        if (!displayPPMwarning)
        {
            PPM_PRECENTROIDED = -INFINITY; // reset value before the next function call
        }
        std::cerr << "    Gaps which could not be interpolated: " << NO_INTERPOLATE_COUNT << "\n"
                  << "    Blocks with posssibly premature termination: " << UNCERTAIN_BLOCK_END << "\n"
                  << "    Average difference between interpolation methods: " << AVERAGE_EXTRAPOLATION_DIFF << "\n";
        NO_INTERPOLATE_COUNT = 0;
        UNCERTAIN_BLOCK_END = 0;
        AVERAGE_EXTRAPOLATION_DIFF = 0;
        return centroids;
    }

    constexpr ProfileBlock blockStart()
    {
        ProfileBlock p;
        p.df.reserve(16);
        p.intensity.reserve(16);
        p.mz.reserve(16);
        p.df.push_back(false);
        p.df.push_back(false);
        p.intensity.push_back(0);
        p.intensity.push_back(0);
        p.mz.push_back(0);
        p.mz.push_back(0);
        return p;
    }

    std::vector<ProfileBlock> pretreatDataCentroids(std::vector<std::vector<double>> spectrum, float expectedDifference)
    {
        std::vector<centroidPoint> dataPoints;
        dataPoints.reserve(spectrum[0].size());
        for (size_t i = 0; i < spectrum[0].size(); ++i)
        {
            if (spectrum[1][i] == 0.0)
            {
                continue; // skip values with no intensity @todo minimum intensity?
            }
            centroidPoint dp(
                spectrum[0][i], // mz
                spectrum[1][i], // intensity
                true);          // real point
            dataPoints.push_back(dp);
        }
        // a point with infinite mz is added to the end to ensure that the block end code is always executed
        centroidPoint dp(
            INFINITY, // mz
            0.0,      // intensity
            false);   // interpolated point
        dataPoints.push_back(dp);

        std::vector<ProfileBlock> subProfiles;

        // iterate over the data points
        size_t maxOfBlock = 0;
        size_t blockSize = 0; // size of the current block
        size_t countSubOneGap = 0;
        size_t countNoGap = 0;
        ProfileBlock currentBlock = blockStart(); // initialised with two zeroed values in each vector
        for (size_t pos = 0; pos < dataPoints.size() - 1; pos++)
        {
            blockSize++;

            currentBlock.intensity.push_back(dataPoints[pos].intensity);
            currentBlock.mz.push_back(dataPoints[pos].mz);
            currentBlock.df.push_back(true);

            const float delta_x = dataPoints[pos + 1].mz - dataPoints[pos].mz;
            assert(delta_x > 0);

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
                const int gapSize = static_cast<int>(delta_x / expectedDifference) - 1;
                int interpolationCount = int(delta_x / (1.5 * expectedDifference)) + 1;

                if (gapSize < 4)
                {
                    if (gapSize == 0)
                    {
                        NO_INTERPOLATE_COUNT++;
                    }

                    // std::cerr << interpolationCount - gapSize << ", "; // between 0 to 2 more interpolations
                    // add gapSize interpolated datapoints @todo this can be zero
                    const float dy = std::pow(dataPoints[pos + 1].intensity / dataPoints[pos].intensity,
                                              1.0 / float(gapSize + 1)); // dy for log interpolation ; 1 if gapsize == 0
                    for (int i = 0; i < gapSize; i++)
                    {
                        currentBlock.intensity.push_back(dataPoints[pos].intensity * std::pow(dy, i + 1));
                        currentBlock.mz.push_back(dataPoints[pos].mz + (i + 1) * expectedDifference);
                        currentBlock.df.push_back(false);
                    }
                }
                else
                { // END OF BLOCK, EXTRAPOLATION STARTS @todo move this into its own function
                    // assert(interpolationCount > 3);
                    if (interpolationCount < 4)
                    {
                        UNCERTAIN_BLOCK_END++;
                    }
                    if (blockSize > 4)
                    {
                        float blockStartMZ = currentBlock.mz[2];
                        // check if the maximum of the block is the first or last data point
                        if (maxOfBlock == pos || dataPoints[maxOfBlock].mz == blockStartMZ)
                        {
                            // extrapolate the left side using the first non-zero data point (i.e, the start of the block)
                            // @todo is this a good idea?
                            for (int i = 0; i < 2; i++)
                            {
                                // LEFT SIDE
                                currentBlock.intensity[i] = currentBlock.intensity[2];
                                currentBlock.mz[i] = blockStartMZ - (2 - i) * expectedDifference;
                                // degrees of freedom is already false
                                // RIGHT SIDE
                                currentBlock.intensity.push_back(dataPoints[pos].intensity);
                                currentBlock.mz.push_back(dataPoints[pos].mz + float(i + 1) * expectedDifference);
                                currentBlock.df.push_back(false);
                            }
                        }
                        else
                        {
                            // this is the expected case
                            const float x_old[3] = {0.f,
                                                    dataPoints[maxOfBlock].mz - blockStartMZ,
                                                    dataPoints[pos].mz - blockStartMZ};
                            const float x[3] = {blockStartMZ,
                                                dataPoints[maxOfBlock].mz,
                                                dataPoints[pos].mz};
                            const float y[3] = {std::log(currentBlock.intensity[2]),
                                                std::log(dataPoints[maxOfBlock].intensity),
                                                std::log(dataPoints[pos].intensity)};
                            auto coeffs_old = interpolateQuadratic(x_old, y);
                            auto coeffs = interpolateQuadratic(x, y);

                            for (int i = 0; i < 2; i++)
                            {
                                const float x = blockStartMZ - (2 - i) * expectedDifference;
                                currentBlock.intensity[i] = std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2]));
                                float prev_x = -(2 - i) * expectedDifference;
                                float prediff = (coeffs[0] + x * (coeffs[1] + x * coeffs[2])) - (coeffs_old[0] + prev_x * (coeffs_old[1] + prev_x * coeffs_old[2]));
                                float test = currentBlock.intensity[i] - std::exp(coeffs_old[0] + prev_x * (coeffs_old[1] + prev_x * coeffs_old[2]));
                                currentBlock.mz[i] = blockStartMZ - (2 - i) * expectedDifference;
                                AVERAGE_EXTRAPOLATION_DIFF += test;
                            }
                            // add the extrapolated data points to the right side of the block
                            for (int i = 0; i < 2; i++)
                            {
                                const float dp_x = dataPoints[pos].mz + float(i + 1) * expectedDifference;
                                const float prev_x = dp_x - blockStartMZ;

                                currentBlock.intensity.push_back(std::exp(coeffs[0] + dp_x * (coeffs[1] + dp_x * coeffs[2])));
                                auto test = currentBlock.intensity[i] - std::exp(coeffs_old[0] + prev_x * (coeffs_old[1] + prev_x * coeffs_old[2]));
                                currentBlock.mz.push_back(dp_x);
                                currentBlock.df.push_back(false);
                                AVERAGE_EXTRAPOLATION_DIFF += test;
                            }
                        }
                        maxOfBlock = pos + 1;
                        // block is finished, add currentBlock to storage vector
                        // interpolateEdges(currentBlock.mz, &currentBlock.intensity);
                        subProfiles.push_back(currentBlock);
                    }
                    currentBlock = blockStart(); // reset block for next iteration
                    blockSize = 0;
                }
            }
            else // if (gapSize2 == 0) // no gap found
            {
                countNoGap++;
                assert(gapSize2 == 0);
                if (dataPoints[maxOfBlock].intensity < dataPoints[pos].intensity)
                {
                    maxOfBlock = pos;
                }
                if (delta_x > 0.8 * expectedDifference && delta_x < 1.2 * expectedDifference)
                {
                    expectedDifference = (expectedDifference + delta_x) * 0.5; // update the expected difference
                }
            }
        } // end of for loop
        if (blockSize > 4)
        {
            subProfiles.push_back(currentBlock);
        }
        AVERAGE_EXTRAPOLATION_DIFF /= subProfiles.size() / 4;
        // std::cerr << countSubOneGap << ", -" << countNoGap << ", ";
        return subProfiles;
    }

    void interpolateEdges(const std::vector<float> x_axis, std::vector<float> *intensity)
    {
        // x-axis can be either mz (for centroids) or RT (for features)
        assert(x_axis.size() > 8); // at least 9; five real points and four edge points
        assert(x_axis.size() == intensity->size());
        size_t pos = x_axis.size() - 3; // index of last real point
        // auto maxInt = std::max(intensity->begin(), intensity->end());
        // size_t maxIdx = std::distance(intensity->begin(), maxInt);
        size_t maxIdx = 0;
        for (size_t i = 2; i < x_axis.size() - 2; i++)
        {
            maxIdx = (*intensity)[i] > (*intensity)[maxIdx] ? i : maxIdx;
        }
        // quadratic interpolation using outer two points and the maximum
        const float x[3] = {x_axis[2], x_axis[maxIdx], x_axis[pos]};
        const float y[3] = {std::log((*intensity)[2]), std::log((*intensity)[maxIdx]), std::log((*intensity)[pos])};
        auto coeffs = interpolateQuadratic(x, y);
#define INTERPOLATE(x) (std::exp(coeffs[0] + x * (coeffs[1] + x * coeffs[2])))
        // left side
        if ((*intensity)[maxIdx] == (*intensity)[2])
        {
            assert((*intensity)[0] == (*intensity)[2]);
            assert((*intensity)[1] == (*intensity)[2]);
        }
        else
        {
            double tmp = std::exp(coeffs[0] + x_axis[0] * (coeffs[1] + x_axis[0] * coeffs[2]));
            assert((*intensity)[0] == INTERPOLATE(x_axis[0]));
            assert((*intensity)[1] == INTERPOLATE(x_axis[1]));
        }
        // right side
        if ((*intensity)[maxIdx] == (*intensity)[2])
        {
            assert((*intensity)[pos + 1] == (*intensity)[pos]);
            assert((*intensity)[pos + 2] == (*intensity)[pos]);
        }
        else
        {
            assert((*intensity)[pos + 1] == INTERPOLATE(x_axis[pos + 1]));
            assert((*intensity)[pos + 2] == INTERPOLATE(x_axis[pos + 2]));
        }
    }

#pragma endregion "find centroids"

#pragma region "find peaks"
    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        float rt_diff)
    {
        std::vector<FeaturePeak> peaks;    // return vector for feature list
        peaks.reserve(data.size() / 4);    // should be enough to fit all features without reallocation
        std::vector<FeaturePeak> tmpPeaks; // add features to this before pasting into FL

        for (size_t i = 0; i < data.size(); ++i) // loop over all data
        {
            if (data[i].scanNumbers.size() < 5)
            {
                continue; // skip due to lack of data, i.e., degree of freedom will be zero
            }
            std::vector<dataPoint> dataPoints = qbinToDataPoint(data[i]); // convert qbin to data points
            std::vector<unsigned int> binIndexConverter;
            treatedData treatedData = pretreatData(dataPoints, binIndexConverter, rt_diff, false); // inter/extrapolate data, and identify data blocks

            findFeatures(tmpPeaks, treatedData);
            if (!tmpPeaks.empty())
            {
                for (size_t j = 0; j < tmpPeaks.size(); j++)
                {
                    assert(tmpPeaks[j].idxPeakEnd < binIndexConverter.size());
                    tmpPeaks[j].idxPeakStart = binIndexConverter[tmpPeaks[j].idxPeakStart];
                    tmpPeaks[j].idxBin = i;
                    // the end point is only correct if it is real. Check if the next point
                    // has the same index - if yes, -1 to end index
                    unsigned int tmpIdx = tmpPeaks[j].idxPeakEnd;
                    tmpPeaks[j].idxPeakEnd = binIndexConverter[tmpPeaks[j].idxPeakEnd];
                    if (tmpIdx + 1 != binIndexConverter.size())
                    {
                        if (binIndexConverter[tmpIdx] == binIndexConverter[tmpIdx + 1])
                        {
                            tmpPeaks[j].idxPeakEnd--;
                        }
                    }
                    peaks.push_back(std::move(tmpPeaks[j])); // remove 2D structure of FL
                }
            }
            tmpPeaks.clear();
        }
        return peaks;
    }
}