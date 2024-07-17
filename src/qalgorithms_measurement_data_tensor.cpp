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
            sc::MZML &data,
            const bool ms1only)
        {
            std::vector<int> indices = data.get_spectra_index();   // get all indices
            std::vector<int> ms_levels = data.get_spectra_level(); // get all MS levels
            if (ms1only)
            {
                indices.erase(std::remove_if(indices.begin(), indices.end(), [&ms_levels](int i)
                                             { return ms_levels[i] != 1; }),
                              indices.end()); // keep only MS1 spectra
            }

            double expectedDifference = 0.0;
            {
                alignas(64) std::vector<double> data_vec = data.get_spectrum(indices[0])[0]; // get first spectrum (x-axis)
                expectedDifference = calcExpectedDiff(data_vec);                             // calculate expected difference
            }
            // #pragma omp parallel for
            for (int index : indices) // loop over all indices
            {
                // load spectrum
                alignas(64) std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // get spectrum at index
                if (spectrum[0].size() < 5)
                {
                    continue; // skip due to lack of data, i.e., degree of freedom will be zero
                }
                spectrum.push_back(std::vector<double>(spectrum[0].size(), 1.0)); // add df column for interpolation

                // zero filling
                zeroFilling_vec(spectrum, expectedDifference); // zero fill the spectrum
            }
        }
    } // namespace MeasurementData
} // namespace q