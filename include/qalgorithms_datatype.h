#ifndef QALGORITHMS_DATATYPE_H
#define QALGORITHMS_DATATYPE_H

namespace q {

class DataType {
    public:
    enum class DataField{
        DATATYPE,
        DRIFTTIME,
        INTENSITY,
        MODE,
        MSLEVEL,
        MZ,
        RETENTIONTIME, 
        SCANNUMBER
    };
}; // class datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_H
