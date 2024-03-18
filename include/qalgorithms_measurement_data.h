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
         * @param xData A vector of x-axis values
         * @param yData A vector of y-axis values
         * @param k The maximum gap size
         *  
         */
        void zeroFilling(std::vector<double>& xData, std::vector<double>& yData, int k);

        /**
         * @brief Cut the data into smaller data sets
         * @details The cutData method cuts the data into smaller data sets. The method uses the separator value to split the data into smaller data sets. The separator value is set to -1.0 for the x-axis and -1.0 for the y-axis. For each cut, the method creates a new data subset and stores it in the data map using a sub-dataset ID as the secondary key.
         * @param xData A vector of x-axis values
         * @param yData A vector of y-axis values
         * @return A vector of indices where the data needs to be cut
         * 
         */
        std::vector<size_t> cutData(std::vector<double>& xData, std::vector<double>& yData) const;

        // debugging
        virtual void print() = 0;
    };
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H