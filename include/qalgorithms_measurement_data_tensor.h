#ifndef QALGORITHMS_MEASUREMENT_DATA_TENSOR_H
#define QALGORITHMS_MEASUREMENT_DATA_TENSOR_H

// internal
#include "../include/qalgorithms_measurement_data.h"
#include "../include/qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"

namespace qAlgorithms
{

    /**
     * @brief A class to store 3D tensor measurement data, e.g., LC-MS data
     * @details The TensorData class is a subclass of the MeasurementData class used to store 3D tensor measurement data, e.g., LC-MS data.
     *
     * @param data A 3D tensor of LC-MS data /ref DataType::LC_MS
     */

    float calcRTDiff(std::vector<double> &retention_times);

    std::vector<dataPoint> mzmlToDataPoint(sc::MZML &data, const int index);

    std::vector<dataPoint> qbinToDataPoint(EIC &eic);

    // methods
    /**
     * @brief Read 3D tensor data from a StreamCraft mzML object
     * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file").
     *
     * @param data : StreamCraft mzML object
     * @param ms1only : boolean to indicate if only MS1 spectra should be read
     * @param start_index : index of the first spectrum to be read
     * @return std::vector<std::vector<DataType::Peak>> : list of peaks
     */
    std::vector<std::vector<CentroidPeak>> findCentroids_MZML(
        sc::MZML &data,
        std::vector<unsigned int> &addEmpty,
        std::vector<float> &convertRT,
        float &rt_diff,
        const bool ms1only = true,
        const std::string polarity = "positive",
        const int start_index = 0);

    std::vector<FeaturePeak> findPeaks_QBIN(
        std::vector<EIC> &data,
        float rt_diff);

}

#endif // QALGORITHMS_MEASUREMENT_DATA_TENSOR_H