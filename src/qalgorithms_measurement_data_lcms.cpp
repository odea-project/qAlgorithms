// qalgorithms_measurement_data_lcms.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"

// external
#include <fstream>
#include <sstream>

namespace q
{
    LCMSData::LCMSData()
    {
    }

    LCMSData::~LCMSData()
    {
    }

    void LCMSData::readCSV(std::string filename, int rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataType::DataField> variableTypes)
    {
        std::ifstream file(filename);
        std::string line;
        std::vector<std::vector<std::string>> raw_data;

        // if rowEnd is -1, then set it to the maximum number of rows
        if (rowEnd == -1)
        {
            rowEnd = 1000000;
        }

        // if colEnd is -1, then set it to the maximum number of columns
        if (colEnd == -1)
        {
            colEnd = 1000000;
        }

        // find the DataFiled::SCANNUMBER as this is the key for the LCMSData map
        int scanNumberIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataType::DataField::SCANNUMBER)
            {
                scanNumberIndex = i;
                break;
            }
        }
        // check if scanNumberIndex is found
        if (scanNumberIndex == -1)
        {
            std::cerr << "The scan number is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::RETENTIONTIME
        int retentionTimeIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataType::DataField::RETENTIONTIME)
            {
                retentionTimeIndex = i;
                break;
            }
        }
        // check if retentionTimeIndex is found
        if (retentionTimeIndex == -1)
        {
            std::cerr << "The retention time is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::MZ
        int mzIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataType::DataField::MZ)
            {
                mzIndex = i;
                break;
            }
        }
        // check if mzIndex is found
        if (mzIndex == -1)
        {
            std::cerr << "The mz is not found in the variable types" << std::endl;
            return;
        }

        // find the DataFiled::INTENSITY
        int intensityIndex = -1;
        for (int i = 0; i < variableTypes.size(); i++)
        {
            if (variableTypes[i] == DataType::DataField::INTENSITY)
            {
                intensityIndex = i;
                break;
            }
        }
        // check if intensityIndex is found
        if (intensityIndex == -1)
        {
            std::cerr << "The intensity is not found in the variable types" << std::endl;
            return;
        }

        // read the file
        int rowCounter = 0;
        int colCounter = 0;
        while (std::getline(file, line) && rowCounter < rowEnd)
        {
            if (rowCounter >= rowStart)
            {
                std::vector<std::string> row;
                std::stringstream lineStream(line);
                std::string cell;
                colCounter = 0;
                while (std::getline(lineStream, cell, separator))
                {
                    if (colCounter >= colStart && colCounter < colEnd)
                    {
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
        if (raw_data[0].size() != variableTypes.size())
        {
            std::cerr << "The number of columns in the raw data does not match the number of variable types" << std::endl;
            return;
        }

        // transfere the raw data to the data map
        // internal datapoint iterator
        int j = 0;
        for (int i = 0; i < raw_data.size(); i++)
        {
            // add the scan number to the data map
            int scanNumber = std::stoi(raw_data[i][scanNumberIndex]);
            // check if the scan number is already in the data map
            if (this->data.find(scanNumber) == this->data.end())
            {
                // add the LC_MS object to the data map
                this->data[scanNumber] = std::make_unique<DataType::LC_MS>();
                // add the scan number to the LC_MS object
                int* scanNumberPtr = new int(scanNumber);
                this->data[scanNumber]->data[DataType::DataField::SCANNUMBER] = scanNumberPtr;
                // reset the internal datapoint iterator
                j = 0;
                // add the DataPoint Map to the data map
                this->data[scanNumber]->data[DataType::DataField::DATAPOINT] = new std::unordered_map<int, DataType::MassSpectrum::DataPoint>();
            }

            // add the retention time to the data map
            double* retentionTimePtr = new double(std::stod(raw_data[i][retentionTimeIndex]));
            this->data[scanNumber]->data[DataType::DataField::RETENTIONTIME] = retentionTimePtr;

            // add the DataPoint to the data map
            DataType::MassSpectrum::DataPoint* dataPoint = new DataType::MassSpectrum::DataPoint();
            dataPoint->mz = std::stod(raw_data[i][mzIndex]);
            dataPoint->intensity = std::stod(raw_data[i][intensityIndex]);
            dataPoint->df = 1;
            // add the DataPoint to the DataPoint Map at the internal datapoint iterator
            static_cast<std::unordered_map<int, DataType::MassSpectrum::DataPoint>*>(this->data[scanNumber]->data[DataType::DataField::DATAPOINT])->insert({j, *dataPoint});
            j++;
        }
    }

    // void LCMSData::zeroFilling()
    // {
    //     // iterate over all data sets
    //     for (auto it = this->data.begin(); it != this->data.end(); it++)
    //     {
    //         // iterate over all sub data sets
    //         for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
    //         {
    //             this->MeasurementData::zeroFilling(it2->second.mz, it2->second.intensity, 8);
    //         }
    //     }
    // }

    // void LCMSData::cutData()
    // {
    //     // create a new data map to store the updated data
    //     std::unordered_map<int, std::unordered_map<int, DataType::LC_MS>> updatedData;
    //     // iterate over all data sets
    //     for (auto it = this->data.begin(); it != this->data.end(); it++)
    //     {
    //         // iterate over all sub data sets
    //         for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
    //         {
    //             // read the indices where the data needs to be cut
    //             std::vector<size_t> indices = this->MeasurementData::cutData(it2->second.mz, it2->second.intensity);

    //             if (indices.size() == 0)
    //             {
    //                 // no cut is needed. however, zerofilling will add a separator at least at the beginning of the data.
    //                 updatedData[it->first][it2->first] = it2->second;
    //             }
    //             else
    //             {
    //                 // cut the data
    //                 int subID = 0;
    //                 // add the last index as the size of the data
    //                 indices.push_back(it2->second.mz.size());
    //                 for (int i = 0; i < indices.size() - 1; i++)
    //                 {
    //                     // create a new data subset
    //                     DataType::LC_MS newSubset = it2->second;
    //                     // adjust the mz and intensity vectors
    //                     newSubset.mz = std::vector<double>(it2->second.mz.begin() + indices[i] + 1, it2->second.mz.begin() + indices[i + 1]);
    //                     newSubset.intensity = std::vector<double>(it2->second.intensity.begin() + indices[i] + 1, it2->second.intensity.begin() + indices[i + 1]);
    //                     updatedData[it->first][subID] = newSubset;
    //                     subID++;
    //                 }
    //             }
    //         }
    //     }

    //     // update the data map
    //     this->data = updatedData;
    // }

    // void LCMSData::interpolateData()
    // {
    //     // iterate over all data sets
    //     for (auto it = this->data.begin(); it != this->data.end(); it++)
    //     {
    //         // iterate over all sub data sets
    //         for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
    //         {
    //             this->MeasurementData::interpolateData(it2->second.mz, it2->second.intensity);
    //         }
    //     }
    // }

    void LCMSData::print()
    {
        for (auto it = this->data.begin(); it != this->data.end(); it++)
        {
            std::cout << "Scan Number: " << it->first << std::endl;
            std::cout << "Retention Time: " << *static_cast<double*>(it->second->data[DataType::DataField::RETENTIONTIME]) << std::endl;

            std::cout << "MZ: ";
            // iterate through the DataPoint Map for MZ
            for (auto it2 = static_cast<std::unordered_map<int, DataType::MassSpectrum::DataPoint>*>(it->second->data[DataType::DataField::DATAPOINT])->begin(); it2 != static_cast<std::unordered_map<int, DataType::MassSpectrum::DataPoint>*>(it->second->data[DataType::DataField::DATAPOINT])->end(); it2++)
            {
                std::cout << it2->second.mz << " ";
            }
            std::cout << std::endl;

            std::cout << "Intensity: ";
            // iterate through the DataPoint Map for Intensity
            for (auto it2 = static_cast<std::unordered_map<int, DataType::MassSpectrum::DataPoint>*>(it->second->data[DataType::DataField::DATAPOINT])->begin(); it2 != static_cast<std::unordered_map<int, DataType::MassSpectrum::DataPoint>*>(it->second->data[DataType::DataField::DATAPOINT])->end(); it2++)
            {
                std::cout << it2->second.intensity << " ";
            }
            std::cout << std::endl;
            

            
        }
    }

} // namespace q