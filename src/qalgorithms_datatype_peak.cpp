// qalgorithms_datatype_peak.cpp

#include "../include/qalgorithms_datatype_peak.h"

namespace q {
  namespace DataType {
    // Peak
    Peak::Peak() {};
    Peak::Peak(int sampleID, double position, double height) : sampleID(sampleID), position(position), height(height) {};
    Peak::~Peak() {};
  } // namespace DataType
} // namespace q