// qalgorhithms_datatype_mass_spectrum.cpp

// internal
#include "../include/qalgorithms_datatype_mass_spectrum.h"

// external

namespace q
{
  namespace DataType
  {
    MassSpectrum::MassSpectrum()
    {
      data[DataField::DATAPOINT] = std::make_unique<variableType>(std::unordered_map<std::unique_ptr<double>, std::unique_ptr<DataPoint>>());
    }

    MassSpectrum::~MassSpectrum()
    {
      // destructor
    }

    LC_MS::LC_MS()
    {
      // constructor
    }

    LC_MS::~LC_MS()
    {
      // destructor
    }
  }
}