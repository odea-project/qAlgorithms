// qalgorithms_measurement_data_lcms.cpp

// internal
#include "qalgorithms_measurement_data_lcms.h"

// external
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace q
{
    namespace MeasurementData
    {
        // usings
        using VariableType = DataType::MassSpectrum::variableType;
        using DataField = DataType::DataField;
        using MassSpectrum = DataType::MassSpectrum;
        using DataPoint = DataType::MassSpectrum::DataPoint;
        using DataPointVector = std::vector<std::unique_ptr<DataPoint>>;

        LCMSData::LCMSData()
        {
            maxKey = 0;
        }

        // LCMSData::~LCMSData() // this is implicity declared
        // {
        // }

        bool LCMSData::readCSV(std::string filename, size_t rowStart, int rowEnd, int colStart, int colEnd, char separator, std::vector<DataField> variableTypes)
        {
            // @todo currenty only
            // open the file
            std::ifstream file(filename);
            std::string line;
            std::vector<std::vector<std::string>> raw_data;

            if (!file.is_open())
            {
                std::cout << "the file could not be opened\n";
                return false;
            }

            if (!file.good())
            {
                std::cout << "something is wrong with the input file\n";
                return false;
            }

            // if rowEnd is -1, then set it to the maximum number of rows @todo cast to unsigned int, -1 = max size
            if (rowEnd == -1)
            {
                rowEnd = 1000000;
            }

            // if colEnd is -1, then set it to the maximum number of columns
            if (colEnd == -1)
            {
                colEnd = 1000000;
            }

            // find the DataFiled::SCANNUMBER as this is the key for the LCMSData vector
            bool flag = false;
            size_t scanNumberIndex = 0;
            for (size_t i = 0; i < variableTypes.size(); i++)
            {
                if (variableTypes[i] == DataField::SCANNUMBER)
                {
                    scanNumberIndex = i;
                    flag = true;
                    break;
                }
            }
            // check if scanNumberIndex is found
            if (!flag)
            {
                std::cerr << "The scan number is not found in the variable types" << std::endl;
                return false;
            }

            // find the DataFiled::RETENTIONTIME
            flag = false;
            size_t retentionTimeIndex = 0;
            for (size_t i = 0; i < variableTypes.size(); i++)
            {
                if (variableTypes[i] == DataField::RETENTIONTIME)
                {
                    retentionTimeIndex = i;
                    flag = true;
                    break;
                }
            }
            // check if retentionTimeIndex is found
            if (!flag)
            {
                std::cerr << "The retention time is not found in the variable types" << std::endl;
                return false;
            }

            // find the DataFiled::MZ
            flag = false;
            size_t mzIndex = 0;
            for (size_t i = 0; i < variableTypes.size(); i++)
            {
                if (variableTypes[i] == DataField::MZ)
                {
                    mzIndex = i;
                    flag = true;
                    break;
                }
            }
            // check if mzIndex is found
            if (!flag)
            {
                std::cerr << "The mz is not found in the variable types" << std::endl;
                return false;
            }

            // find the DataFiled::INTENSITY
            flag = false;
            size_t intensityIndex = 0;
            for (size_t i = 0; i < variableTypes.size(); i++)
            {
                if (variableTypes[i] == DataField::INTENSITY)
                {
                    intensityIndex = i;
                    flag = true;
                    break;
                }
            }
            // check if intensityIndex is found
            if (!flag)
            {
                std::cerr << "The intensity is not found in the variable types" << std::endl;
                return false;
            }

            // read the file
            int rowCounter = 0;
            int colCounter = 0;

            for (size_t i = 0; i < rowStart; i++)
            {
                std::getline(file, line); // only start parsing after rowStart lines
            }
            while (std::getline(file, line) && rowCounter < rowEnd)
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
                rowCounter++;
            }
            file.close();

            if (raw_data[0].size() != variableTypes.size())
            {
                std::cerr << "The number of columns in the raw data does not match the number of variable types" << std::endl;
                return false;
                // exit(102);
            }

            // transfer the raw data to the data vector
            maxKey = 0;

            this->data.reserve(raw_data.size());
            for (size_t i = 0; i < raw_data.size(); i++)
            {
                // check if the scan number is already in the data vector
                size_t scanNumber = std::stoi(raw_data[i][scanNumberIndex]);
                if (this->data.size() < scanNumber + 1)
                { // scan number is not found in the data vector; initialize a new MassSpectrum object and add meta information
                    // add the MassSpectrum object to the data vector
                    this->data.push_back(std::make_unique<MassSpectrum>());
                    // add the scan number to the MassSpectrum object
                    (*this->data[scanNumber]->metaData)[DataField::SCANNUMBER] = VariableType(scanNumber);
                    // add the retention time to the MassSpectrum object
                    (*this->data[scanNumber]->metaData)[DataField::RETENTIONTIME] = VariableType(std::stod(raw_data[i][retentionTimeIndex]));
                    // update the maxKey
                    if (scanNumber > maxKey)
                    {
                        maxKey = scanNumber;
                    }
                }
                // create a new DataPoint object and add it to the DataPoint Vector
                double intensity = std::stod(raw_data[i][intensityIndex]);
                int df = (intensity > 0) ? 1 : 0;
                this->data[scanNumber]->dataPoints.push_back(std::make_unique<DataPoint>(intensity, std::stod(raw_data[i][mzIndex]), df));
            }
            return true;
        }

        bool LCMSData::readStreamCraftMZML(sc::MZML &data, std::array<double, 2> rt_window)
        {
            std::vector<std::vector<std::vector<double>>> spectra = data.get_spectra();
            std::vector<double> retentionTimes = data.get_spectra_rt();
            std::vector<int> scanNumbers = data.get_spectra_scan_number();
            std::vector<int> ms_levels = data.get_spectra_level();

            // filter data to fit the specified windows in mz and rt.
            // a value of -1 means the entire range is taken.
            // filter in rt first:
            if (rt_window[0] != -1)
            {
                auto it = std::find_if(retentionTimes.begin(), retentionTimes.end(), [rt_window](double rt)
                                       { return rt > rt_window[0]; });
                
                if (it != retentionTimes.begin()) // copy elements of old vectors excluding the discarded scans
                {
                    size_t rt_start = std::distance(retentionTimes.begin(), it);
                    std::vector<decltype(spectra)::value_type>(spectra.begin()+rt_start, spectra.end()).swap(spectra);
                    std::vector<decltype(retentionTimes)::value_type>(retentionTimes.begin()+rt_start, retentionTimes.end()).swap(retentionTimes);
                    std::vector<decltype(scanNumbers)::value_type>(scanNumbers.begin()+rt_start, scanNumbers.end()).swap(scanNumbers);
                    std::vector<decltype(ms_levels)::value_type>(ms_levels.begin()+rt_start, ms_levels.end()).swap(ms_levels);
                } else {
                    std::cout << "The selected lower bound in rt is already the first spectrum in the dataset.\n";
                }
                
            }

            if (rt_window[1] != -1)
            {
                auto it = std::find_if(retentionTimes.begin(), retentionTimes.end(), [rt_window](double rt)
                                       { return rt > rt_window[1]; });
                
                if (it != retentionTimes.end()) // copy elements of old vectors excluding the discarded scans
                {
                    size_t newLength = std::distance(retentionTimes.begin(), it) - 2; 
                    // -2 since first element past the limit is found and the size is one larger than the index
                    spectra.resize(newLength);
                    retentionTimes.resize(newLength);
                    scanNumbers.resize(newLength);
                    ms_levels.resize(newLength);
                } else {
                    std::cout << "The selected upper bound in rt is already the last spectrum in the dataset.\n";
                }
                
            }

            if (spectra.empty())
            {
                return false;
            }

            size_t number_of_spectra = spectra.size();
            for (size_t i = 0; i < number_of_spectra; i++)
            {
                // checks if the MS level is ms1 @todo: add ms2, ms3, etc.
                // @todo implement this as std::filter for less wasted cycles
                if (ms_levels[i] != 1)
                {
                    continue;
                }
                size_t number_of_data_points = spectra[i][0].size();
                size_t scanNumber = scanNumbers[i];
                double retentionTime = retentionTimes[i];
                if (this->data.size() < i + 1)
                { // scan number is not found in the data vector; initialize a new MassSpectrum object and add meta information
                    // add the MassSpectrum object to the data vector
                    this->data.push_back(std::make_unique<MassSpectrum>());
                    // add the scan number to the MassSpectrum object
                    (*this->data.back()->metaData)[DataField::SCANNUMBER] = VariableType(scanNumber);
                    // add the retention time to the MassSpectrum object
                    (*this->data.back()->metaData)[DataField::RETENTIONTIME] = VariableType(retentionTime);
                    // update the maxKey
                    if (i > maxKey)
                    {
                        maxKey = i;
                    }
                }
                for (size_t j = 0; j < number_of_data_points; j++)
                {
                    double intensity = spectra[i][1][j];
                    int df = (intensity > 0) ? 1 : 0;
                    this->data.back()->dataPoints.push_back(std::make_unique<DataPoint>(intensity, spectra[i][0][j], df));
                }
            }
            return true;
        }

        void LCMSData::writeCSV(std::string filename)
        {
            // open the file
            std::ofstream file(filename);
            // write header using: ID, ScanNumber, RetentionTime, MZ, Intensity (ID is the key for the data vector)
            file << "ID,ScanNumber,RetentionTime,MZ,Intensity" << std::endl;
            // iterate over all data sets in the data vector
            for (const auto &spectrum : this->data)
            {
                for (const auto &dp : spectrum->dataPoints)
                {
                    file << std::get<int>((*spectrum->metaData)[DataField::SCANNUMBER]) << ","
                         << std::get<int>((*spectrum->metaData)[DataField::SCANNUMBER]) << ","
                         << std::get<double>((*spectrum->metaData)[DataField::RETENTIONTIME]) << ","
                         << std::setprecision(7) << dp->mz << ","
                         << std::setprecision(4) << dp->intensity << std::endl;
                }
                // iterate over all data points
            }
            // close the file
            file.close();
        }

        void LCMSData::zeroFilling()
        {
            // create a varDataType Object, which is a pointer to a vector of mass spectra
            varDataType dataObject = &(this->data);
            this->MeasurementData::zeroFilling(dataObject, 8);
        }

        void LCMSData::cutData()
        {
            // create a varDataType Object, which is a pointer to a vector of mass spectra
            varDataType dataObject = &(this->data);
            this->MeasurementData::cutData(dataObject, maxKey);
        }

        void LCMSData::filterSmallDataSets()
        {
            // create a varDataType Object, which is a pointer to a vector of mass spectra
            varDataType dataObject = &(this->data);
            this->MeasurementData::filterSmallDataSets(dataObject);
        }

        void LCMSData::interpolateData()
        {
            // create a varDataType Object, which is a pointer to a vector of mass spectra
            varDataType dataObject = &(this->data);
            this->MeasurementData::interpolateData(dataObject);
        }

        void LCMSData::print()
        {
            for (const auto &spectrum : this->data)
            {
                std::cout << "Scan Number: " << std::get<double>((*spectrum->metaData)[DataField::SCANNUMBER]) << std::endl;
                std::cout << "Retention Time: " << std::get<double>((*spectrum->metaData)[DataField::RETENTIONTIME]) << std::endl;
                std::cout << "Number of Data Points: " << spectrum->dataPoints.size() << std::endl;
                std::cout << "MZ values: ";
                for (const auto &dp : spectrum->dataPoints)
                {
                    std::cout << dp->mz << " ";
                }
                std::cout << std::endl;

                std::cout << "Intensity values: ";
                for (const auto &dp : spectrum->dataPoints)
                {
                    std::cout << dp->intensity << " ";
                }
                std::cout << std::endl;
                std::cout << "cuttingPoints: ";
                for (const auto &cp : spectrum->cuttingPoints)
                {
                    std::cout << *cp << " ";
                }
                std::cout << std::endl;
            }
        }

        size_t LCMSData::numDatasets()
        {
            return this->data.size(); // @todo note that some mass spectra are empty - fix?
        }

    } // namespace MeasurementData

} // namespace q
