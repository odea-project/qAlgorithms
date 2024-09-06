// qalgorithms_measurement_data_tensor.cpp

// internal
#include "../include/qalgorithms_measurement_data_tensor.h"

// external
#include <cassert>
#include <fstream>
#include <algorithm>
#include <numeric>

namespace q
{
    namespace MeasurementData
    {
#pragma region "private methods"
        // methods
        float
        TensorData::calcRTDiff(std::vector<double> &retention_times)
        {
            float sum = 0.0;
            for (size_t i = 1; i < retention_times.size(); ++i)
            {
                sum += retention_times[i] - retention_times[i - 1];
            }
            return sum / (retention_times.size() - 1);
        }

        std::vector<TensorData::dataPoint>
        TensorData::mzmlToDataPoint(
            sc::MZML &data,
            const int index)
        {
            std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // get spectrum at index
            std::vector<TensorData::dataPoint> dataPoints;                        // create vector of data points
            dataPoints.reserve(spectrum[0].size());                               // reserve memory for data points
            for (size_t i = 0; i < spectrum[0].size(); ++i)
            {
                if (spectrum[1][i] == 0.0)
                {
                    continue; // skip zero values
                }
                TensorData::dataPoint dp( // create data point
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
            TensorData::dataPoint dp(                   // create data point
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

        std::vector<TensorData::dataPoint>
        TensorData::qbinToDataPoint(
            q::Algorithms::qBinning::EIC &eic)
        {
            std::vector<TensorData::dataPoint> dataPoints;  // create vector of data points
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
                    TensorData::dataPoint dp(            // create data point
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
                    TensorData::dataPoint dp(   // create data point
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
            TensorData::dataPoint dp(                   // create data point
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
        std::vector<std::vector<DataType::Peak>> TensorData::findCentroids_MZML(
            q::Algorithms::qPeaks &qpeaks,
            sc::MZML &data,
            std::vector<unsigned int> &addEmpty,
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

            bool displayPPMwarning = false;
            if (ppm_for_precentroided_data == -5)
            {
                ppm_for_precentroided_data = 5;
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
            }

            // FILTER POLARITY and START INDEX and ARRAY LENGTH
            indices.erase(std::remove_if(indices.begin(), indices.end(), [&spectrum_polarity, &num_datapoints, &polarity, &start_index](int i)
                                         { return spectrum_polarity[i] != polarity || i < start_index || num_datapoints[i] < 5; }),
                          indices.end()); // keep only spectra with the specified polarity and start index and array length > 5

            // CHECK IF CENTROIDED SPECTRA
            int num_centroided_spectra = std::count(spectrum_mode.begin(), spectrum_mode.end(), "centroid");
            if (num_centroided_spectra > spectrum_mode.size() / 2) // in profile mode sometimes centroided spectra appear as well
            {
                std::cerr << "Warning: qAlgorithms is intended for profile spectra. A base uncertainty of "
                          << ppm_for_precentroided_data << " ppm is assumed for all supplied centroids\n";
                std::vector<double> retention_times = data.get_spectra_rt(indices); // get retention times
                rt_diff = calcRTDiff(retention_times);
                return transferCentroids(data, indices, retention_times, start_index, ppm_for_precentroided_data);
            }

            if (displayPPMwarning)
            {
                std::cerr << "Notice: the changed centroid certainty will only affect pre-centroided data.\n";
            }

            // FILTER SPECTRUM MODE (PROFILE)
            if (num_centroided_spectra != 0) // @todo should these really be discarded?
            {
                indices.erase(std::remove_if(indices.begin(), indices.end(), [&spectrum_mode](int i)
                                             { return spectrum_mode[i] != "profile"; }),
                              indices.end()); // keep only profile spectra
            }

            std::vector<double> retention_times = data.get_spectra_rt(indices); // get retention times
            rt_diff = calcRTDiff(retention_times);                              // retention time difference

            std::vector<std::vector<DataType::Peak>> centroids =
                std::vector<std::vector<DataType::Peak>>(indices.size()); // create vector of unique pointers to peaks

            // CALCULATE EXPECTED DIFFERENCE & CHECK FOR ZEROS
            {
                std::vector<std::vector<double>> data_vec = data.get_spectrum(indices[start_index]); // get first spectrum (x-axis)
                expectedDifference = calcExpectedDiff(data_vec[0]);                                  // calculate expected difference & check if Orbitrap
            }

#pragma omp parallel for
            for (size_t i = 0; i < indices.size(); ++i) // loop over all indices
            {
                const int index = indices[i];                                                       // spectrum index
                std::vector<TensorData::dataPoint> dataPoints = mzmlToDataPoint(data, index);       // convert mzml to data points
                TensorData::treatedData treatedData = pretreatData(dataPoints, expectedDifference); // inter/extrapolate data, and identify data blocks
                qpeaks.findCentroids(centroids[i], treatedData, index, retention_times[i]);         // find peaks in data blocks of treated data
            }

            if (!displayPPMwarning)
            {
                ppm_for_precentroided_data = -5;
            }

            // determine where the peak finding will interpolate points and pass this information
            // to the binning step. addEmpty contains the number of empty scans to be added into
            // the qCentroids object at the given position.
            addEmpty.resize(indices.size());
            std::fill(addEmpty.begin(), addEmpty.end(), 0);
            for (size_t i = 1; i < indices.size() + 1; i++)
            {
                if (retention_times[i + 1] - retention_times[i] > rt_diff * 1.75)
                {
                    addEmpty[i]++;
                    retention_times[i] += rt_diff * 1.75;
                    i--;
                }
            }

            return centroids;
        } // readStreamCraftMZML
#pragma endregion "find centroids"

#pragma region "find peaks"
        std::vector<std::vector<DataType::Peak>>
        TensorData::findPeaks_QBIN(
            q::Algorithms::qPeaks &qpeaks,
            std::vector<q::Algorithms::qBinning::EIC> &data)
        {
            std::vector<std::vector<DataType::Peak>> peaks(data.size()); // create vector of unique pointers to peaks
                                                                         // #pragma omp parallel for
            for (size_t i = 0; i < data.size(); ++i)                     // loop over all data
            {
                const int num_data_points = data[i].scanNumbers.size(); // number of data points
                if (num_data_points < 5)
                {
                    continue; // skip due to lack of data, i.e., degree of freedom will be zero
                }
                std::vector<dataPoint> dataPoints = qbinToDataPoint(data[i]);       // convert qbin to data points
                treatedData treatedData = pretreatData(dataPoints, rt_diff, false); // inter/extrapolate data, and identify data blocks
                qpeaks.findPeaks(peaks[i], treatedData);
            } // parallel for
            return peaks;

            // std::vector<std::vector<DataType::Peak>> returnVec(peaks.size());
            // for (size_t i = 0; i < peaks.size(); i++)
            // {
            //     returnVec[i].reserve(peaks[i].size());
            //     for (size_t j = 0; j < peaks[i].size(); j++)
            //     {
            //         returnVec[i].push_back(*peaks[i][j]);
            //     }
            // }
            // return returnVec;
        } // readQBinning
    } // namespace MeasurementData
} // namespace q