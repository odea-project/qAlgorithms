#ifndef QALGORITHMS_DATATYPE_CHROMATOGRAM_H
#define QALGORITHMS_DATATYPE_CHROMATOGRAM_H

// internal
#include "qalgorithms_datatype.h"

// external
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace q
{
  namespace DataType
  {
    /**
     * @brief A class to store chromatogram data
     * @details The Chromatogram class is used to store chromatogram data.
     */

    class Chromatogram
    {
    public:
      /**
       * @brief Struct to store chromatogram data points
       * @details The DataPoint struct is used to store chromatogram data points. It contains the intensity, retention time, and degrees of freedom (important for the uncertainty estimation when interpolating the data is considered).
       *
       * @param intensity The intensity of the chromatogram data point
       * @param retention_time The retention time of the chromatogram data point
       * @param df The degrees of freedom of the chromatogram data point
       */
      struct DataPoint
      {
        double intensity;
        double retention_time;
        int df;

        DataPoint(double intensity, double retention_time, int df) : intensity(intensity), retention_time(retention_time), df(df) {}
        // getters for x and y
        double &x() { return retention_time; }
        double &y() { return intensity; }
        void setY(double y) { intensity = y; }
      };

      // constructors
      Chromatogram();

      /**
       * @brief Construct a new Chromatogram object from another Chromatogram object
       * @details The Chromatogram constructor constructs a new Chromatogram object from another Chromatogram object. The constructor takes a reference to another Chromatogram object. All MetaData will be referenced, and the the last k data points will be moved to the new object.
       *
       * @param reference chromatogram object
       * @param k number of data points to be moved to the new object, starting from the end of the reference object
       */
      Chromatogram(Chromatogram &parent, size_t k);

      // destructor
      ~Chromatogram();

      // methods
      /**
       * @brief Add a data point to the chromatogram
       * @details The addDataPoint method adds a data point to the chromatogram. The method takes the intensity, retention time, and degrees of freedom of the data point.
       *
       * @param intensity The intensity of the chromatogram data point
       * @param retention_time The retention time of the chromatogram data point
       * @param df The degrees of freedom of the chromatogram data point
       */
      void
      addDataPoint(double intensity, double retention_time, int df);

      /**
       * @brief Sort the mass spectrum data points
       * @details The sortDataPoints method sorts the mass spectrum data points based on the x-axis values.
       */
      void
      sortDataPoints();

      // properties, i.e. metadata
      using variableType = std::variant< // allowed variable types for chromatogram data
          int,                           // e.g. scan number
          size_t,                        // e.g. scan number, number of data points
          double,                        // e.g. wavelength or mass-to-charge ratio
          >;

      std::unordered_map<DataField, variableType> *metaData;
      std::vector<std::unique_ptr<DataPoint>> dataPoints;
      std::pair<bool, Chromatogram *> isParent;

      /**
       * @brief This vector stores information for cutting the chromatogram data.
       * @details The cuttingPoints vector can be seen as a job list for cutting the chromatogram data. Each element in the vector shows the number of data points counted from the end of the dataPoints vector that should be moved to the new object. The number of elements within the cuttingPoints vector determines the number of new objects that will be created. E.g. cuttingPoints = {10, 5} will create two new objects. The first object will contain the last 10 data points of the original object while these datapoints are removed from the original object. The second object will contain the next last 5 data points of the original object, while these datapoints are also removed from the original object. The original object will contain all data points except the ones that were moved to the new objects.
       */
      std::vector<std::unique_ptr<size_t>> cuttingPoints;

      // getter
      int
      getScanNumber() const;
    };
  }
}

#endif // QALGORITHMS_DATATYPE_CHROMATOGRAM_H