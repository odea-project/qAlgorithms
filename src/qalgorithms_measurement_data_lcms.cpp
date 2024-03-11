#include "qalgorithms_measurement_data_lcms.h"
#include <fstream>
#include <sstream>

namespace q {

// Constructor
LCMSData::LCMSData() {
    // Add your constructor logic here
}

// Destructor
LCMSData::~LCMSData() {
    // Add your destructor logic here
}

// readCSV method
void LCMSData::readCSV(std::string FILE_PATH, int row0, int rowN, int col0, int colN, std::vector<DataType::DataField> dataFields) {
    std::ifstream file(FILE_PATH);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    int row_count = 0;
    while (std::getline(file, line)) {
        if (row_count >= row0 && row_count <= rowN) {
            std::stringstream ss(line);
            std::string value;
            int col_count = 0;
            DataType::DataField dataField;
            while (std::getline(ss, value, ',')) {
                if (col_count >= col0 && col_count <= colN) {
                    // Assuming DataType::DataField has a method to set value
                    dataField.setValue(value);
                }
                col_count++;
            }
            dataFields.push_back(dataField);
        }
        row_count++;
    }

    file.close();
}

} // namespace q