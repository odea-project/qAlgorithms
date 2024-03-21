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
    // methods
    void MassSpectrum::addDataPoint(double intensity, double mz, int df)
    {
      std::get<std::vector<std::unique_ptr<DataPoint>>>(data[DataField::DATAPOINT]).push_back(std::make_unique<DataPoint>(intensity, mz, df));
    }

    void MassSpectrum::sortDataPoints()
    {
      auto& dataPoints = std::get<std::vector<std::unique_ptr<DataPoint>>>(data[DataField::DATAPOINT]);
      std::sort(dataPoints.begin(), dataPoints.end(), [](const std::unique_ptr<DataPoint>& a, const std::unique_ptr<DataPoint>& b) { return a->x() < b->x(); });
    }
  }
}