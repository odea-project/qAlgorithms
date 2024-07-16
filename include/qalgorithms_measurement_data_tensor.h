#ifndef QALGORITHMS_MEASUREMENT_DATA_TENSOR_H
#define QALGORITHMS_MEASUREMENT_DATA_TENSOR_H

// internal
#include "qalgorithms_measurement_data.h"

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
            std::vector<DataType::DataField> variableTypes
        );

        // methods
        /**
         * @brief Read 3D tensor data from a StreamCraft mzML object
         * @details The readStreamCraftMZML method reads 3D tensor data from a StreamCraft mzML object and stores it in the TensorData object. A StreamCraft mzML object is created by using sc::MZML z("path to mzML file"). 
         * 
         * @param data : StreamCraft mzML object
         */
        void 
        readStreamCraftMZML(
            sc::MZML &data
        );


    };
    } // namespace MeasurementData  
} // namespace q

#endif // QALGORITHMS_MEASUREMENT_DATA_TENSOR_H