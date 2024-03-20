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
      data[DataField::DATAPOINT] = std::vector<std::unique_ptr<DataPoint>>();
    }

    MassSpectrum::~MassSpectrum()
    {
      // destructor
    }
  }
}