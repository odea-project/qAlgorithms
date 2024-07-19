// qalgorithms_measurement_data_tensor.cpp

// internal
#include "../include/qalgorithms_measurement_data_tensor.h"

namespace q
{
    namespace MeasurementData
    {
        // constructors and destructor
        TensorData::TensorData() {}

        TensorData::~TensorData() {}

        // methods
        void
        TensorData::readCSV(
            std::string filename,
            int rowStart,
            int rowEnd,
            int colStart,
            int colEnd,
            char separator,
            std::vector<DataType::DataField> variableTypes) {}

        void
        TensorData::readStreamCraftMZML(
            q::Algorithms::qPeaks &qpeaks,
            sc::MZML &data,
            const bool ms1only,
            const int start_index)
        {
            std::vector<int> indices = data.get_spectra_index();   // get all indices
            std::vector<int> ms_levels = data.get_spectra_level(); // get all MS levels
            if (ms1only)
            {
                indices.erase(std::remove_if(indices.begin(), indices.end(), [&ms_levels](int i)
                                             { return ms_levels[i] != 1; }),
                              indices.end()); // keep only MS1 spectra
            }

            double expectedDifference = 0.0; // expected difference between two consecutive x-axis values
            bool needsZeroFilling = true;    // check if the instrument is Orbitrap
            {
                alignas(64) std::vector<std::vector<double>> data_vec = data.get_spectrum(indices[start_index]); // get first spectrum (x-axis)
                expectedDifference = calcExpectedDiff(data_vec[0]);                                              // calculate expected difference & check if Orbitrap
                isZeroFillingNeeded(data_vec[1], needsZeroFilling);                                              // check if zero filling is needed
            }
            if (needsZeroFilling)
            {
#pragma omp parallel for
                for (int index : indices) // loop over all indices
                {
                    if (index < start_index)
                    {
                        continue; // skip due to index
                    }
                    // load spectrum
                    alignas(64) std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // get spectrum at index
                    if (spectrum[0].size() < 5)
                    {
                        continue; // skip due to lack of data, i.e., degree of freedom will be zero
                    }
                    spectrum.push_back(std::vector<double>(spectrum[0].size(), 1.0));                           // add df column for interpolation
                    int num_subsets = zeroFilling_vec(spectrum, expectedDifference);                            // zero fill the spectrum
                    std::vector<double>::iterator *separators = new std::vector<double>::iterator[num_subsets]; // Dynamically allocate memory for separators (y axis)
                    extrapolateData_vec(spectrum, separators);                                                  // interpolate the data when zero filled
                    delete[] separators;                                                                        // delete separators
                }
            }
            else
            {
#pragma omp parallel for
                for (size_t i = 0; i < indices.size(); ++i) // loop over all indices
                {
                    const int index = indices[i]; // spectrum index
                    if (index < start_index)
                    {
                        continue; // skip due to index
                    }
                    alignas(64) std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // spectrum at index
                    if (spectrum[0].size() < 5)
                    {
                        continue; // skip due to lack of data, i.e., degree of freedom will be zero
                    }
                    spectrum.push_back(std::vector<double>(spectrum[0].size(), 1.0)); // add df column for interpolation
                    std::vector<std::vector<double>::iterator> separators;            // vector of iterators at separation points (x axis)
                    cutData_vec(spectrum, expectedDifference, separators);            // find separation points in the data
                    interpolateData_vec(spectrum, separators);                        // interpolate the data
                    extrapolateData_vec(spectrum, separators);                        // extrapolate the data
                    std::vector<std::unique_ptr<DataType::Peak>> centroids;           // create vector of unique pointers to peaks
                    qpeaks.findPeaks(centroids, spectrum, separators);                // find peaks
                } // for
            } // else
        } // readStreamCraftMZML
    } // namespace MeasurementData
} // namespace q