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

        /**
             * @brief Identify and fill gaps in the data
             * @details The zeroFilling method identifies and fills gaps in the data. The method uses difference between two neighboring data points to identify gaps. If the difference is 1.75 times greater than expected, then the method fills the gap with zero values for y-axis and inter/extrapolated values for x-axis values. For the expected difference, the method the difference of the last two data points that not show a gap. However, the first expected difference is set to the median of the differences of the total data points. However, the maximum gap size is set to "k/2" per side, i.e., "k" in total, where there is a gap leftover between the fourth and fifth data points. 
             *  
             */
            void zeroFilling(std::vector<double>& xData, std::vector<double>& yData, int k);

        // debugging
        virtual void print() = 0;
    };
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H
