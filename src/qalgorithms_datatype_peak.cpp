// qalgorithms_datatype_peak.cpp

// @todo merge as struct with datatype

#include "../include/qalgorithms_datatype_peak.h"

#include <iostream>

namespace q
{
  namespace DataType
  {
    // Peak
    // Peak::Peak(int sampleID, double position, double height) : sampleID(sampleID), position(position), height(height){};

    // dubugging
    void Peak::print()
    {
      std::cout << "Peak: " << std::endl;
      std::cout << "  sampleID: " << sampleID << std::endl;
      // std::cout << "  position: " << position << std::endl;
      std::cout << "  height: " << height << std::endl;
      std::cout << "--------------------------------" << std::endl;
    }
  } // namespace DataType
} // namespace q