#ifndef QALGORITHMS_MEASUREMENT_DATA_TENSOR_H
#define QALGORITHMS_MEASUREMENT_DATA_TENSOR_H

// internal
#include "../include/qalgorithms_measurement_data.h"
#include "../include/qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

namespace q
{
    namespace MeasurementData
    {
        /**
         * @brief A class to store 3D tensor measurement data, e.g., LC-MS data
         * @details The TensorData class is a subclass of the MeasurementData class used to store 3D tensor measurement data, e.g., LC-MS data.
         *
         * @param data A 3D tensor of LC-MS data /ref DataType::LC_MS
         */
        class TensorData : public MeasurementData
        {
        private:
            float rt_diff;
            float calcRTDiff(std::vector<double> &retention_times);

            std::vector<dataPoint>
            mzmlToDataPoint(
                sc::MZML &data,
                const int index);

        public:
            // constructors
            TensorData();

            // destructor
            ~TensorData();

            void
            readCSV(
                std::string filename,
                int rowStart,
                int rowEnd,
                int colStart,
                int colEnd,
                char separator,
                std::vector<DataType::DataField> variableTypes);

            // methods
            /**
             * @brief Read 3D tensor data from a StreamCraft mzML object
             * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file").
             *
             * @param data : StreamCraft mzML object
             * @param ms1only : boolean to indicate if only MS1 spectra should be read
             * @param start_index : index of the first spectrum to be read
             * @return std::vector<std::vector<std::unique_ptr<DataType::Peak>>> : list of peaks
             */
            std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
            findCentroids_MZML(
                q::Algorithms::qPeaks &qpeaks,
                sc::MZML &data,
                const bool ms1only = true,
                const std::string polarity = "positive",
                const int start_index = 0);

            std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
            findPeaks_QBIN(
                q::Algorithms::qPeaks &qpeaks,
                std::vector<q::Algorithms::qBinning::EIC> &data);
        };
    } // namespace MeasurementData
} // namespace q

#endif // QALGORITHMS_MEASUREMENT_DATA_TENSOR_H