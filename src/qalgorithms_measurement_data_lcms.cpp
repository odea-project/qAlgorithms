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
            int subID = 0;
            this->data[scanNumber][subID].scanNumber = scanNumber;

            // get/set the retention time
            this->data[scanNumber][subID].retentionTime = std::stod(raw_data[i][retentionTimeIndex]);

            // get/set the mz
            this->data[scanNumber][subID].mz.push_back(std::stod(raw_data[i][mzIndex]));

            // get/set the intensity
            this->data[scanNumber][subID].intensity.push_back(std::stod(raw_data[i][intensityIndex]));

            // get/set optional variable types
            // following fields are allowed:
            // DataField::IONIZATIONMODE,
            // DataField::MEASUREMENTMODE,
            // DataField::MSLEVEL
            for (int j = 0; j < variableTypes.size(); j++) {
                if (variableTypes[j] == DataType::DataField::IONIZATIONMODE) {
                    this->data[scanNumber][subID].ionizationMode = (raw_data[i][j] == "Positive") ? DataType::IonizationMode::Positive : DataType::IonizationMode::Negative;
                } else if (variableTypes[j] == DataType::DataField::MEASUREMENTMODE) {
                    this->data[scanNumber][subID].measurementMode = (raw_data[i][j] == "Profile") ? DataType::MeasurementMode::Profile : DataType::MeasurementMode::Centroid;
                } else if (variableTypes[j] == DataType::DataField::MSLEVEL) {
                    this->data[scanNumber][subID].msLevel = (raw_data[i][j] == "MS1") ? DataType::MSLevel::MS1 : DataType::MSLevel::MS2;
                }
            }
        }
    }

    void LCMSData::zeroFilling() {
        // iterate over all data sets
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            // iterate over all sub data sets
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                this->MeasurementData::zeroFilling(it2->second.mz, it2->second.intensity, 8);
            }
        }
    }

    void LCMSData::cutData() {
        // create a new data map to store the updated data
        std::unordered_map<int, std::unordered_map<int, DataType::LC_MS>> updatedData;
        // iterate over all data sets
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            // iterate over all sub data sets
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                // read the indices where the data needs to be cut
                std::vector<size_t> indices = this->MeasurementData::cutData(it2->second.mz, it2->second.intensity);
                
                if (indices.size() == 0) {
                    // no cut is needed. however, zerofilling will add a separator at least at the beginning of the data.
                    updatedData[it->first][it2->first] = it2->second;
                } else {
                    // cut the data
                    int subID = 0;
                    // add the last index as the size of the data
                    indices.push_back(it2->second.mz.size());
                    for (int i = 0; i < indices.size() - 1; i++) {
                        // create a new data subset
                        DataType::LC_MS newSubset = it2 -> second;
                        // adjust the mz and intensity vectors
                        newSubset.mz = std::vector<double>(it2->second.mz.begin() + indices[i] + 1, it2->second.mz.begin() + indices[i+1]);
                        newSubset.intensity = std::vector<double>(it2->second.intensity.begin() + indices[i] + 1, it2->second.intensity.begin() + indices[i+1]);
                        updatedData[it->first][subID] = newSubset;
                        subID++;
                    }
                }
            }
        }

        // update the data map
        this->data = updatedData;
    }

    void LCMSData::interpolateData() {
        // iterate over all data sets
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            // iterate over all sub data sets
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                this->MeasurementData::interpolateData(it2->second.mz, it2->second.intensity);
            }
        }
    }

    void LCMSData::print() {
        for (auto it = this->data.begin(); it != this->data.end(); it++) {
            // iterate over all sub data sets
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                std::cout << "Scan Number: " << it2->second.scanNumber << std::endl;
                std::cout << "SubID: " << it2->first << std::endl;
                std::cout << "Retention Time: " << it2->second.retentionTime << std::endl;
                std::cout << "MZ: ";
                for (int i = 0; i < it2->second.mz.size(); i++) {
                    std::cout << it2->second.mz[i] << " ";
                }
                std::cout << std::endl;
                std::cout << "Intensity: ";
                for (int i = 0; i < it2->second.intensity.size(); i++) {
                    std::cout << it2->second.intensity[i] << " ";
                }
                std::cout << std::endl;
            }
        }
    }

} // namespace q