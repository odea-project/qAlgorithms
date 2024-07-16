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
            sc::MZML &data)
        {
            std::vector<int> indices = data.get_spectra_index(); // get all indices
#pragma omp parallel for
            for (int index : indices) // loop over all indices
            {
                // load spectrum
                alignas(64) std::vector<std::vector<double>> spectrum = data.get_spectrum(index); // get spectrum at index
                alignas(32) std::vector<std::vector<float>> spectrum_float(spectrum.size());
                for (size_t i = 0; i < spectrum.size(); i++)
                {
                    spectrum_float[i].reserve(spectrum[i].size());
                    std::transform(spectrum[i].begin(), spectrum[i].end(), spectrum_float[i].begin(), [](double d) { return static_cast<float>(d); });
                }
                spectrum_float.push_back(std::vector<float>(spectrum_float[0].size(), 1.0)); // add df column for interpolation

                // zero filling
                zeroFilling_vec(spectrum_float);
            }
        }
    } // namespace MeasurementData
} // namespace q