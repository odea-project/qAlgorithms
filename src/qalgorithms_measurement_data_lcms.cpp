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