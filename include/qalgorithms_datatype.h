#ifndef QALGORITHMS_DATATYPE_H
#define QALGORITHMS_DATATYPE_H

namespace q {

namespace DataType {
    enum class DataField{
        DATATYPE,
        DRIFTTIME,
        INTENSITY,
        IONIZATIONMODE,
        MSLEVEL,
        MZ,
        RETENTIONTIME, 
        SCANNUMBER
        // Add new data fields here
    };
}; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_H
