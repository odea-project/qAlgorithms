// qalgorhithms_datatype_mass_spectrum.cpp

// internal
#include "../include/qalgorithms_datatype_mass_spectrum.h"

// external

namespace q {
  namespace DataType {
    MassSpectrum::MassSpectrum() : data(std::unordered_map<DataField, std::unique_ptr<variableType>>()) {
      // constructor
    }

    MassSpectrum::~MassSpectrum() {
      // destructor
    }


    LC_MS::LC_MS() {
      // constructor
    }

    LC_MS::~LC_MS() {
      // destructor
    }
  }
}