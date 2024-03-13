#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

// internal
#include "qalgorithms_datatype_mass_spectrum.h"

// external
#include <string>
#include <iostream>
#include <algorithm>

namespace q {
    /**
     * @brief A class to store measurement data
     * @details The MeasurementData class is a virtual class used to store measurement data. Based on the type of measurement data, the MeasurementData class can be subclassed to store different types of measurement data.
     * 
     */
    class MeasurementData {
    public:
        // destructor
        virtual ~MeasurementData() {};

        // methods
        virtual void readCSV(std::string filename,int rowStart, int rowEnd, int colStart, int colEnd, char separator,std::vector<DataType::DataField> variableTypes) = 0;

        // debugging
        virtual void print() = 0;
    };
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H
