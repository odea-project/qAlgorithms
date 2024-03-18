// qalgorithms_measurement_data_lcms.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"

// external
#include <fstream>
#include <sstream>

namespace q {
    LCMSData::LCMSData() {
    }

    LCMSData::~LCMSData() {
    }

    void LCMSData::readCSV(std::string filename,int rowStart, int rowEnd, int colStart, int colEnd, char separator,std::vector<DataType::DataField> variableTypes) {
        std::ifstream file(filename);
        std::string line;
        std::vector<std::vector<std::string>> raw_data;

        // if rowEnd is -1, then set it to the maximum number of rows
        if (rowEnd == -1) {
            rowEnd = 1000000;
        }

        // if colEnd is -1, then set it to the maximum number of columns
        if (colEnd == -1) {
            colEnd = 1000000;
        }

        // find the DataFiled::SCANNUMBER as this is the key for the LCMSData map
        int scanNumberIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++) {
            if (variableTypes[i] == DataType::DataField::SCANNUMBER) {
                scanNumberIndex = i;
                break;
            }
        }

        // find the DataFiled::RETENTIONTIME 
        int retentionTimeIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++) {
            if (variableTypes[i] == DataType::DataField::RETENTIONTIME) {
                retentionTimeIndex = i;
                break;
            }
        }

        // find the DataFiled::MZ
        int mzIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++) {
            if (variableTypes[i] == DataType::DataField::MZ) {
                mzIndex = i;
                break;
            }
        }

        // find the DataFiled::INTENSITY
        int intensityIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++) {
            if (variableTypes[i] == DataType::DataField::INTENSITY) {
                intensityIndex = i;
                break;
            }
        }

        // read the file
        int rowCounter = 0;
        int colCounter = 0;
        while (std::getline(file, line) && rowCounter < rowEnd) {
            if (rowCounter >= rowStart) {
                std::vector<std::string> row;
                std::stringstream lineStream(line);
                std::string cell;
                colCounter = 0;
                while (std::getline(lineStream, cell, separator)) {
                    if (colCounter >= colStart && colCounter < colEnd) {
                      row.push_back(cell);
                    }
                    colCounter++;
                }
                raw_data.push_back(row);
            }
            rowCounter++;
        }
        file.close();

        // check if the raw data has same number of columns as the variableTypes
        if (raw_data[0].size() != variableTypes.size()) {
            std::cerr << "The number of columns in the raw data does not match the number of variable types" << std::endl;
            return;
        }

        // transfere the raw data to the data map
        for (int i = 0; i < raw_data.size(); i++) {
            // get/set the scan number
            int scanNumber = std::stoi(raw_data[i][scanNumberIndex]);
            this->data[scanNumber].scanNumber = scanNumber;

            // get/set the retention time
            this->data[scanNumber].retentionTime = std::stod(raw_data[i][retentionTimeIndex]);

            // get/set the mz
            this->data[scanNumber].mz.push_back(std::stod(raw_data[i][mzIndex]));

            // get/set the intensity
            this->data[scanNumber].intensity.push_back(std::stod(raw_data[i][intensityIndex]));

            // get/set optional variable types
            // following fields are allowed:
            // DataField::IONIZATIONMODE,
            // DataField::MEASUREMENTMODE,
            // DataField::MSLEVEL
            for (int j = 0; j < variableTypes.size(); j++) {
                if (variableTypes[j] == DataType::DataField::IONIZATIONMODE) {
                    this->data[scanNumber].ionizationMode = (raw_data[i][j] == "Positive") ? DataType::IonizationMode::Positive : DataType::IonizationMode::Negative;
                } else if (variableTypes[j] == DataType::DataField::MEASUREMENTMODE) {
                    this->data[scanNumber].measurementMode = (raw_data[i][j] == "Profile") ? DataType::MeasurementMode::Profile : DataType::MeasurementMode::Centroid;
                } else if (variableTypes[j] == DataType::DataField::MSLEVEL) {
                    this->data[scanNumber].msLevel = (raw_data[i][j] == "MS1") ? DataType::MSLevel::MS1 : DataType::MSLevel::MS2;
                }
            }

        }

    }

    void LCMSData::zeroFilling() {
        // iterate over all data sets
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            // calculate the initial expected difference between mz values
            double expectedDifference = 0.0;
            std::vector<double> differences;
            for (int i = 1; i < it->second.mz.size(); i++) {
                differences.push_back(it->second.mz[i] - it->second.mz[i-1]);
            }
            // median of differences
            std::sort(differences.begin(), differences.end());
            if (differences.size() % 2 == 0) {
                expectedDifference = (differences[differences.size()/2 - 1] + differences[differences.size()/2]) / 2;
            } else {
                expectedDifference = differences[differences.size()/2];
            }

            // fill the gaps
            for (int i = 1; i < it->second.mz.size(); i++) {
                double difference = it->second.mz[i] - it->second.mz[i-1];
                /* We will always add new data points in the beginning of the data set. */
                if (i == 1) {
                    int gapSize = 4;
                    for (int j = gapSize; j >= 1; j--) { // fill the gap backwards
                        it->second.mz.insert(it->second.mz.begin(), it->second.mz[0] - expectedDifference);
                        it->second.intensity.insert(it->second.intensity.begin(), 0.0);
                    }
                    i += gapSize; // Update the index to the next data point
                } else {
                    if (difference > 1.75 * expectedDifference) {
                        int gapSize = (int) (difference / expectedDifference - 1);
                        /* Check if the gapsSize is larger than 8, as this is the maximum gap size. If the gap size is less or equal to 8, we add gapSize new data points to the data set between the two data points that show the gap, i.e. (i-1 and i). The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values.
                        */
                        if (gapSize <= 8) {
                            for (int j = gapSize; j >= 1; j--) { // fill the gap backwards
                                it->second.mz.insert(it->second.mz.begin() + i, it->second.mz[i-1] + j * expectedDifference);
                                it->second.intensity.insert(it->second.intensity.begin() + i, 0.0);
                            }
                        } else {
                            /* If the gapSize is larger than 8, we limit the gap size to 8 and add 4 new data points close to the (i-1) data point and 4 new data points close to the (i) data point. The new data points are filled with zero values for the intensity and inter/extrapolated values for the x-axis values. Moreover, we add a separator for later splitting the data set into smaller data sets by adding an additional zero in between where the intensity and x-axis are set to -1.0. This is done to indicate that the data set should be split at this point.  
                            */
                            gapSize = 8;
                            // add 4 new data points close to the (i) data point
                            for (int j = 1; j <= 4; j++) { // fill the gap forwards
                                it->second.mz.insert(it->second.mz.begin() + i, it->second.mz[i] -  expectedDifference);
                                it->second.intensity.insert(it->second.intensity.begin() + i, 0.0);
                            }
                            // add a separator
                            it->second.mz.insert(it->second.mz.begin() + i, -1.0);
                            it->second.intensity.insert(it->second.intensity.begin() + i, -1.0);
                            // add 4 new data points close to the (i-1) data point 
                            for (int j = 4; j >= 1; j--) { // fill the gap backwards
                                it->second.mz.insert(it->second.mz.begin() + i, it->second.mz[i-1] + j * expectedDifference);
                                it->second.intensity.insert(it->second.intensity.begin() + i, 0.0);
                            }     
                        }
                        i += gapSize; // Update the index to the next data point
                    }  else {
                        // update the expected difference
                        expectedDifference = (expectedDifference + difference) / 2;
                    } 
                }   
            }
            /* We will always add new data points in the end of the data set. */
            int gapSize = 4;
            for (int j = 1; j <= gapSize; j++) { // fill the gap forwards
                it->second.mz.push_back(it->second.mz[it->second.mz.size()-1] + expectedDifference);
                it->second.intensity.push_back(0.0);
            }
        }
    }

    void LCMSData::print() {
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            std::cout << "Scan Number: " << it->second.scanNumber << std::endl;
            std::cout << "Retention Time: " << it->second.retentionTime << std::endl;
            std::cout << "MZ: ";
            for (int i = 0; i < it->second.mz.size(); i++) {
                std::cout << it->second.mz[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "Intensity: ";
            for (int i = 0; i < it->second.intensity.size(); i++) {
                std::cout << it->second.intensity[i] << " ";
            }
            std::cout << std::endl;
        }
    }

} // namespace q