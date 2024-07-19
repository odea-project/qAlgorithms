// qalgorithms_measurement_data_tensor.cpp

// internal
#include "../include/qalgorithms_measurement_data_tensor.h"
#include "../include/qalgorithms_qpeaks.h"

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
                std::cout << "expected difference: " << expectedDifference << std::endl;
                isZeroFillingNeeded(data_vec[1], needsZeroFilling); // check if zero filling is needed
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
                    // std::cout << "index: " << index << std::endl;
                    // std::cout << "spectrum size: " << spectrum[0].size() << std::endl;
                    spectrum.push_back(std::vector<double>(spectrum[0].size(), 1.0)); // add df column for interpolation
                    // print spectrum[0], spectrum[1] and spectrum[2] with 10 decimal places to csv file
                    // std::ofstream file("spectrum.csv");
                    // for (size_t i = 0; i < spectrum[0].size(); i++)
                    // {
                    //     file << std::fixed << std::setprecision(10) << spectrum[0][i] << "," << spectrum[1][i] << "," << spectrum[2][i] << std::endl;
                    // }
                    // file.close();
                    std::vector<std::vector<double>::iterator> separators;            // create separators
                    cutData_vec(spectrum, expectedDifference, separators);            // find separation points in the data
                    // preint separators with 10 deicmal places to csv file
                    // std::ofstream file2("separators.csv");
                    // for (size_t i = 0; i < separators.size(); i++)
                    // {
                    //     file2 << std::fixed << std::setprecision(10) << *separators[i] << std::endl;
                    // }
                    // file2.close();
                    interpolateData_vec(spectrum, separators);                        // interpolate the data
                    extrapolateData_vec(spectrum, separators);                        // extrapolate the data
                    
                    exit(0);
                }
            }
        } // readStreamCraftMZML
    } // namespace MeasurementData
} // namespace q