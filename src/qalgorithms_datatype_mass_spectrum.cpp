// qalgorhithms_datatype_mass_spectrum.cpp

#include "../include/qalgorithms_datatype_mass_spectrum.h"
#include <algorithm> // std::reverse, std::sort

// up to date with commit 2fa36fa

namespace q
{
  namespace DataType
  {
    MassSpectrum::MassSpectrum()
    {
      dataPoints = std::vector<std::unique_ptr<DataPoint>>();
      metaData = new std::unordered_map<DataField, variableType>();
      isParent = std::pair<bool, MassSpectrum *>(true, nullptr);
    }

    MassSpectrum::MassSpectrum(MassSpectrum &parent, const size_t k)
    {
      // define the new object as a child of the other object
      isParent = std::pair<bool, MassSpectrum *>(false, &parent);
      // create a reference to the other object's metadata
      metaData = parent.metaData; // shared metadata from the parent object
      // move the last k data points to the new object
      for (size_t i = 0; i < k; i++)
      {
        dataPoints.push_back(std::move(parent.dataPoints.back()));
        parent.dataPoints.pop_back();
      }
      // flip dataPoints
      std::reverse(dataPoints.begin(), dataPoints.end());
    }

    // methods
    void MassSpectrum::addDataPoint(double intensity, double mz, int df)
    {
      dataPoints.push_back(std::make_unique<DataPoint>(intensity, mz, df));
    }

    void MassSpectrum::sortDataPoints()
    {
      std::sort(dataPoints.begin(), dataPoints.end(), [](const std::unique_ptr<DataPoint> &a, const std::unique_ptr<DataPoint> &b)
                { return a->x() < b->x(); });
    }

    int MassSpectrum::getScanNumber() const
    {
      return std::get<size_t>(metaData->at(DataField::SCANNUMBER));
    }
  }
}