#ifndef QALGORITHMS_MEASUREMENT_DATA_LCMS_H
#define QALGORITHMS_MEASUREMENT_DATA_LCMS_H

// internal
#include "qalgorithms_measurement_data.h"

#include <unordered_map>

namespace q
{
    /**
     * @brief A class to store LC-MS measurement data
     * @details The LCMSData class is a subclass of the MeasurementData class used to store LC-MS measurement data.
     * @param data A map of LC-MS data /ref DataType::LC_MS
     */
    class LCMSData : public MeasurementData
    {
    public:
        // constructors
        LCMSData();

        // destructor
        ~LCMSData();

        // methods
        /**
         * @brief Read LC-MS data from a CSV file
         * @details The readCSV method reads LC-MS data from a CSV file and stores it in the LCMSData object. The method takes the filename of the CSV file, the start and end rows and columns, the separator, and the variable types. The variable types are used to determine the type of data in each column. The method requires the variable types: \b scanNumber, \b retentionTime, \b mz, and \b intensity. Other variable types can be added to the list of variable types.\ref DataType::DataField
         * @param filename The filename of the CSV file
         * @param rowStart The start row (0-based index)
         * @param rowEnd The end row (if -1, then set to the maximum number of rows)
         * @param colStart The start column (0-based index)
         * @param colEnd The end column (if -1, then set to the maximum number of columns)
         * @param separator The separator, e.g., ',' or '\t'
         * @param variableTypes The variable types. Required variable types: DataField::SCANNUMBER, DataField::RETENTIONTIME, DataField::MZ, and DataField::INTENSITY
         */
        void readCSV(std::string filename, int rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataType::DataField> variableTypes);

        /**
         * @brief Write LC-MS data to a CSV file
         * @details The writeCSV method writes LC-MS data to a CSV file. The method takes the filename of the CSV file and writes the LC-MS data to the file.
         * @param filename The filename of the CSV file
         */
        void writeCSV(std::string filename);

        /**
         * @brief Identify and fill gaps in the LC-MS data
         * @details The zeroFilling method identifies and fills gaps in the LC-MS data. The method uses difference between two neighboring data points to identify gaps. If the difference is 1.75 times greater than expected, then the method fills the gap with zero values for intensity and inter/extrapolated values for x-axis values. For the expected difference, the method the difference of the last two data points that not show a gap. However, the first expected difference is set to the median of the differences of the total data points. However, the maximum gap size is set to 4 per side, i.e., 8 in total, where there is a gap leftover between the fourth and fifth data points.
         *
         */
        void zeroFilling();

        /**
         * @brief Cut the data into smaller data sets
         * @details The cutData method cuts the data into smaller data sets. The method uses the separator value to split the data into smaller data sets. The separator value is set to -1.0 for the x-axis and -1.0 for the y-axis. For each cut, the method creates a new data subset and stores it in the data map using a sub-dataset ID as the secondary key.
         */
        void cutData();

        /**
         * @brief Filter small data sets
         * @details The filterSmallDataSets method filters small data sets. The method removes data sets with less than 5 data points. This is due to the regression analysis that includes 4 coefficients and therefore requires at least 5 data points.
         */
        void filterSmallDataSets();

        /**
         * @brief Interpolate y-axis values
         * @details The interpolateData method interpolates the y-axis values. The method uses quadratic interpolation to interpolate the y-axis values in the log space.
         */
        void interpolateData();

        // debugging
        /**
         * @brief Print the LC-MS datapoints and metadata
         * @details The print method prints the LC-MS data to the console.
         */
        void print();
        /**
         * @brief Print the LC-MS number of data sets
         * @details The print method prints the LC-MS data to the console.
         */
        void info();

    private:
        std::unordered_map<int, std::unique_ptr<DataType::MassSpectrum>> data;
        int maxKey;
    };
}
#endif // QALGORITHMS_MEASUREMENT_DATA_LCMS_H
