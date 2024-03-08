#ifndef QALGORITHMS_MEASUREMENT_DATA_H
#define QALGORITHMS_MEASUREMENT_DATA_H

#include "qalgorithms_datatype_mass_spectrum.h"
#include <map>
#include <vector>
#include <string>

namespace q {
/**
 * @brief Abstract base class for managing measurement data.
 *
 * This class serves as a foundation for handling various types of measurement data.
 * It defines a common interface for processing and accessing the data, ensuring that
 * all derived classes implement these functionalities. The class is designed to be 
 * flexible and extensible, allowing for the addition of new data types with different 
 * structures such as LC-HRMS, LC-IMS-HRMS, or Raman data, each potentially requiring 
 * unique data management and processing strategies.
 */
class MeasurementData {
public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~MeasurementData() {}

    // You can add more common interfaces here, such as methods for accessing
    // or modifying the data, depending on what is common across your different data types.
    virtual void readCSV(std::string FILE_PATH, int row0, int rowN, int col0, int colN, std::vector<DataType::DataField> dataFields) = 0;

protected:
    // Protected members and methods can be added here, which will be accessible
    // to derived classes but not to the users of the class.

private:
    // Private members and methods can be added here, which will be accessible
    // only within this class and not by derived classes.
};
}
#endif // QALGORITHMS_MEASUREMENT_DATA_H
