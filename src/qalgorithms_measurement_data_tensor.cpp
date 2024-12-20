// qalgorithms_measurement_data_tensor.cpp

// internal
#include "qalgorithms_measurement_data_tensor.h"
#include "qalgorithms_global_vars.h"

// external
#include <cassert>
#include <fstream>
#include <algorithm>
#include <numeric>

namespace qAlgorithms
{
#pragma region "private methods"
    // methods
    float
    calcRTDiff(std::vector<double> &retention_times)
    {
        float sum = 0.0;
        for (size_t i = 1; i < retention_times.size(); ++i)
        {
            sum += retention_times[i] - retention_times[i - 1];
        }
        return sum / (retention_times.size() - 1);
    }

    std::vector<dataPoint> mzmlToDataPoint(sc::MZML &data, const int index)
    {
        std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // get spectrum at index
        std::vector<dataPoint> dataPoints;                                    // create vector of data points
        dataPoints.reserve(spectrum[0].size());                               // reserve memory for data points
        for (size_t i = 0; i < spectrum[0].size(); ++i)
        {
            if (spectrum[1][i] == 0.0)
            {
                continue; // skip zero values
            }
            dataPoint dp(             // create data point
                spectrum[0][i],       // x-axis value
                spectrum[1][i],       // y-axis value
                true,                 // df value
                0.0,                  // dqs centroid value
                0.0,                  // dqs binning value
                0,                    // scan number
                0.0);                 // mz ratio
            dataPoints.push_back(dp); // add data point to vector
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
#pragma endregion "private methods"

#pragma region "find centroids"
    std::vector<std::vector<CentroidPeak>> findCentroids_MZML(
        sc::MZML &data,
        std::vector<unsigned int> &addEmpty,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool ms1only,
        const std::string polarity,
        const int start_index)
    {
        std::vector<std::string> spectrum_mode = data.get_spectra_mode();         // get spectrum mode (centroid or profile)
        std::vector<std::string> spectrum_polarity = data.get_spectra_polarity(); // get spectrum polarity (positive or negative)
        std::vector<int> indices = data.get_spectra_index();                      // get all indices
        std::vector<int> ms_levels = data.get_spectra_level();                    // get all MS levels
        std::vector<int> num_datapoints = data.get_spectra_array_length();        // get number of data points
        double expectedDifference = 0.0;                                          // expected difference between two consecutive x-axis values

        assert(!indices.empty() && num_datapoints[0] > 0);

        bool displayPPMwarning = false;
        if (PPM_PRECENTROIDED == -INFINITY)
        {
            PPM_PRECENTROIDED = 0.25; // this error applied to most good centroids in the datasets we checked
        }
        else
        {
            displayPPMwarning = true;
        }

        // FILTER MS1 SPECTRA
        if (ms1only)
        {
            indices.erase(std::remove_if(indices.begin(), indices.end(), [&ms_levels](int i)
                                         { return ms_levels[i] != 1; }),
                          indices.end()); // keep only MS1 spectra
            assert(!indices.empty());
        }

        // FILTER POLARITY and START INDEX and ARRAY LENGTH
        indices.erase(std::remove_if(indices.begin(), indices.end(), [&spectrum_polarity, &num_datapoints, &polarity, &start_index](int i)
                                     { return spectrum_polarity[i] != polarity || i < start_index || num_datapoints[i] < 5; }),
                      indices.end()); // keep only spectra with the specified polarity and start index and array length > 5
        if (indices.empty())
        {
            std::vector<std::vector<CentroidPeak>> empty;
            assert(empty.empty());
            return empty;
        }

        indices.shrink_to_fit();

        std::vector<double> retention_times = data.get_spectra_rt(indices); // get retention times
        rt_diff = calcRTDiff(retention_times);                              // retention time difference
        convertRT.clear();
        convertRT.reserve(indices.size());
        convertRT.push_back(NAN);            // first scan is index 1
        addEmpty.resize(indices.size() + 1); // same length as convertRT if no empty scans exist
        std::fill(addEmpty.begin(), addEmpty.end(), 0);

        // CHECK IF CENTROIDED SPECTRA
        size_t num_centroided_spectra = std::count(spectrum_mode.begin(), spectrum_mode.end(), "centroid");
        if (num_centroided_spectra > spectrum_mode.size() / 2) // in profile mode sometimes centroided spectra appear as well @todo is 2 a good idea?
        {
            std::cerr << "Warning: qAlgorithms is intended for profile spectra. A base uncertainty of "
                      << PPM_PRECENTROIDED << " ppm is assumed for all supplied centroids\n";

            for (int i = 0; i < int(indices.size()) - 1; i++) // i can be -1 briefly if there is a scan missing between 1. and 2. element
            {
                if (retention_times[i + 1] - retention_times[i] > rt_diff * 1.75)
                {
                    addEmpty[i]++;
                    retention_times[i] += rt_diff * 1.75;
                    i--;
                }
                convertRT.push_back(retention_times[i]); // convertRT[scan] = retention time of centroid
            }
            convertRT.push_back(retention_times[indices.size() - 1]);
            assert(addEmpty.size() == indices.size() + 1);
            return transferCentroids(data, indices, retention_times, start_index, PPM_PRECENTROIDED);
        }

        // FILTER SPECTRUM MODE (PROFILE)
        if (num_centroided_spectra != 0) // @todo should these really be discarded?
        {
            indices.erase(std::remove_if(indices.begin(), indices.end(), [&spectrum_mode](int i)
                                         { return spectrum_mode[i] != "profile"; }),
                          indices.end()); // keep only profile spectra
            assert(!indices.empty());
        }
        indices.shrink_to_fit();
        addEmpty.resize(indices.size() + 1); // make sure empty scans (if needed) are added in the right place
        assert(addEmpty.size() == indices.size() + 1);

        std::vector<std::vector<CentroidPeak>> centroids(indices.size()); // create vector of peaks

        // CALCULATE EXPECTED DIFFERENCE & CHECK FOR ZEROS
        std::vector<std::vector<double>> data_vec = data.get_spectrum(indices[start_index]); // get first spectrum (x-axis)
        expectedDifference = calcExpectedDiff(data_vec[0]);                                  // calculate expected difference & check if Orbitrap

#pragma omp parallel for // this has been removed due to causing a memory leak
        for (size_t i = 0; i < indices.size(); ++i)
        {
            const int index = indices[i];                                     // spectrum index
            std::vector<dataPoint> dataPoints = mzmlToDataPoint(data, index); // convert mzml to data points
            std::vector<unsigned int> dummy;
            treatedData treatedData = pretreatData(dataPoints, dummy, expectedDifference); // inter/extrapolate data, and identify data blocks
            centroids[i] = findCentroids(treatedData, index, retention_times[i]);          // find peaks in data blocks of treated data
        }

        if (!displayPPMwarning)
        {
            PPM_PRECENTROIDED = -INFINITY;
        }

        // determine where the peak finding will interpolate points and pass this information
        // to the binning step. addEmpty contains the number of empty scans to be added into
        // the qCentroids object at the given position. convertRT can later be used to look up
        // the retention time by the scan number, so that memory usage is reduced during binning

        for (size_t i = 0; i < indices.size() - 1; i++) // i can be -1 briefly if there is a scan missing between 1. and 2. element
        {
            if (retention_times[i + 1] - retention_times[i] > rt_diff * 1.75)
            {
                addEmpty[i]++;
                assert(addEmpty[i] < 5);
                retention_times[i] += rt_diff * 1.75;
                i--;
            }
            convertRT.push_back(retention_times[i]); // convertRT[scan] = retention time of centroid
        }
        convertRT.push_back(retention_times[indices.size() - 1]);
        assert(addEmpty.size() == indices.size() + 1); // this assert fails with the
        return centroids;
    } // readStreamCraftMZML
#pragma endregion "find centroids"

#pragma region "find peaks"
    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        float rt_diff)
    {
        std::vector<FeaturePeak> peaks;    // return vector for feature list
        peaks.reserve(data.size() * 0.7);  // should be enough to fit all features without reallocation
        std::vector<FeaturePeak> tmpPeaks; // add features to this before pasting into FL

        // pragma omp parallel for
        /// activating this pracma invalidates results @todo why?

        for (size_t i = 0; i < data.size(); ++i) // loop over all data
        {
            if (data[i].scanNumbers.size() < 5)
            {
                continue; // skip due to lack of data, i.e., degree of freedom will be zero
            }
            std::vector<dataPoint> dataPoints = qbinToDataPoint(data[i]); // convert qbin to data points
            std::vector<unsigned int> binIndexConverter;
            treatedData treatedData = pretreatData(dataPoints, binIndexConverter, rt_diff, false); // inter/extrapolate data, and identify data blocks

            findPeaks(tmpPeaks, treatedData);
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
    } // readQBinning
}