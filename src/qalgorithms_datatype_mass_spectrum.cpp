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
      dataPoints = std::vector<std::unique_ptr<DataPoint>>();
      metaData = std::unordered_map<DataField, variableType>();
      isReference = std::pair<bool, MassSpectrum*>(true, nullptr);
    }

    MassSpectrum::MassSpectrum(const MassSpectrum& other, const size_t k)
    {
      // reference MetaData

    }

    MassSpectrum::~MassSpectrum()
    {
      // destructor
    }
    // methods
    void MassSpectrum::addDataPoint(double intensity, double mz, int df)
    {
      dataPoints.push_back(std::make_unique<DataPoint>(intensity, mz, df));
    }

    void MassSpectrum::sortDataPoints()
    {
      std::sort(dataPoints.begin(), dataPoints.end(), [](const std::unique_ptr<DataPoint>& a, const std::unique_ptr<DataPoint>& b) { return a->x() < b->x(); });
    }
  }
}