#ifndef QALGORITHMS_MEASUREMENT_DATA_LCMS_H
#define QALGORITHMS_MEASUREMENT_DATA_LCMS_H

#include "qalgorithms_measurement_data.h"
#include "qalgorithms_datatype_mass_spectrum.h"
#include <map>
#include <vector>
#include <string>

namespace q {
/**
 * @brief Derived class for handling LC-MS measurement data.
 *
 * This class implements the processing and management of LC-MS (Liquid Chromatography-Mass Spectrometry) measurement data.
 * It extends the MeasurementData class with specific attributes and methods required to handle LC-MS data, such as managing
 * multiple intensity profiles corresponding to different retention times.
 */
class LCMSData : public MeasurementData {
public:
    /**
     * @brief Constructor for LCMSData.
     */
    LCMSData() {}

    /**
     * @brief Destructor for LCMSData.
     */
    virtual ~LCMSData() {}

    /**
     * @brief 
     * 
     * @param FILE_PATH 
     * @param row0 
     * @param rowN 
     * @param col0 
     * @param colN 
     * @param dataFields 
     */
    void readCSV(std::string FILE_PATH, int row0, int rowN, int col0, int colN, std::vector<DataType::DataField> dataFields) override {}

private:
    /**
     * @brief Specific Data Structure that holds intensity profiles with additional meta information.
     * 
     */
    std::map<int, DataType::LC_MS> data; 

};
}
#endif // QALGORITHMS_MEASUREMENT_DATA_LCMS_H
