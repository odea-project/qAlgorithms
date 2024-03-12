#ifndef QALGORITHMS_DATATYPE_H
#define QALGORITHMS_DATATYPE_H

namespace q {

namespace DataType {
    /**
     * @brief Different types of data that can be stored in a data file
     * @details The data fields are used to store different types of data in a data file.
     * @param INTENSITY The intensity of the MS signal
     * @param IONIZATIONMODE The ionization mode, i.e. whether the signal is from positive or negative mode
     * @param MZ The mass-to-charge ratio of the MS signal
     * @param MSLEVEL The MS level, i.e. whether the signal is from MS1 or MS2
     * @param MEASUREMENTMODE The measurement mode, i.e. whether the signal is profile or centroid
     * @param RETENTIONTIME The retention time of the LC-MS signal
     * @param SCANNUMBER The scan number of the LC-MS signal
     */
    enum class DataField{
        INTENSITY,
        IONIZATIONMODE,
        MEASUREMENTMODE,
        MSLEVEL,
        MZ,
        RETENTIONTIME,
        SCANNUMBER,
        // Add new data fields here
    };

    enum class IonizationMode { Positive, Negative };
    enum class MeasurementMode { Profile, Centroid };
    enum class MSLevel {MS1, MS2};

}; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_H
