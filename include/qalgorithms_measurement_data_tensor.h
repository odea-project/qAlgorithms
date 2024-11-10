#ifndef QALGORITHMS_MEASUREMENT_DATA_TENSOR_H
#define QALGORITHMS_MEASUREMENT_DATA_TENSOR_H

// internal
#include "../include/qalgorithms_measurement_data.h"
#include "../include/qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

namespace qAlgorithms
{
    extern double ppm_for_precentroided_data; // @todo should we have a header for global vars?

    /**
     * @brief A class to store 3D tensor measurement data, e.g., LC-MS data
     * @details The TensorData class is a subclass of the MeasurementData class used to store 3D tensor measurement data, e.g., LC-MS data.
     *
     * @param data A 3D tensor of LC-MS data /ref LC_MS
     */
    class TensorData // @todo remove this class, it is useless
    {
    public:
        float rt_diff;
        float calcRTDiff(std::vector<double> &retention_times);

        std::vector<dataPoint> mzmlToDataPoint(sc::MZML &data, const int index);

        std::vector<dataPoint> qbinToDataPoint(EIC &eic);

        /**
         * @brief Read 3D tensor data from a StreamCraft mzML object
         * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file").
         *
         * @param data : StreamCraft mzML object
         * @param ms1only : boolean to indicate if only MS1 spectra should be read
         * @param start_index : index of the first spectrum to be read
         * @return std::vector<std::vector<Peak>> : list of peaks
         */
        std::vector<std::vector<CentroidPeak>> findCentroids_MZML(
            qPeaks &qpeaks,
            sc::MZML &data,
            std::vector<unsigned int> &addEmpty,
            std::vector<float> &convertRT,
            const bool ms1only = true,
            const std::string polarity = "positive",
            const int start_index = 0);

        std::vector<FeaturePeak> findPeaks_QBIN(
            qPeaks &qpeaks,
            std::vector<EIC> &data);
    };
}

#endif // QALGORITHMS_MEASUREMENT_DATA_TENSOR_H