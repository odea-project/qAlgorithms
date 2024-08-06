// qalgorithms_datatype_peak.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

// internal
// #include "qalgorithms_datatype.h"

// external

/* This file includes the q::DataType::Peak and q::DataType::PeakList classes*/
namespace q
{
  namespace DataType
  {
    /**
     * @brief A class to store peak data
     * @details The Peak class is used to store peak data. It contains the position, height, width, and area of the peak. The class also contains the uncertainty of the peak position, height, width, and area, the data quality score of the peak, the regression coefficients, the valley position, the degrees of freedom, and the index of the regression window position.
     * @param sampleID The sample ID
     * @param height The height of the peak
     * @param area The area of the peak
     * @param width The width of the peak
     * @param heightUncertainty The uncertainty of the peak height
     * @param areaUncertainty The uncertainty of the peak area
     * @param dqsPeak The data quality score of the peak
     * @param dqsBin The data quality score of the bin
     * @param dqsCen The data quality score of the centroid
     * @param retentionTime The retention time of the peak
     * @param mz The mass-to-charge ratio of the peak
     * @param retentionTimeUncertainty The uncertainty of the retention time
     * @param mzUncertainty The uncertainty of the mass-to-charge ratio
     */
    class Peak
    {
    public:
      // properties
      int sampleID;
      float height;

      // optional
      float area;
      float width;
      float heightUncertainty;
      float areaUncertainty;
      float dqsPeak;
      float dqsBin;
      float dqsCen;
      float retentionTime;
      float mz;
      float retentionTimeUncertainty;
      float mzUncertainty;
    };
  } // namespace DataType
} // namespace q

#endif // QALGORITHMS_DATATYPE_PEAK_H
