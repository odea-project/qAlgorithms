// qalgorithms_datatype_peak.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

// internal
// #include "qalgorithms_datatype.h"

// external

/* This file includes the Peak and PeakList classes*/

namespace qAlgorithms
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
   * @param idxBin bin in which the feature was found
   * @param idxPeakStart index of first (if sorted by RT) point of the bin which is part of this peak
   * @param idxPeakEnd index of last (if sorted by RT) point of the bin which is part of this peak
   */
  // struct Peak
  // {
  //   // properties
  //   int sampleID; // @todo only relevant after a dataset is done
  //   float height;
  //   float area;
  //   float width;
  //   float heightUncertainty;
  //   float areaUncertainty;
  //   float dqsPeak;       // only relevant for features
  //   float dqsBin;        // can be calculated when needed
  //   float dqsCen;        // relevant for centroids, can be calculated for features
  //   float retentionTime; // only relevant for features
  //   float mz;
  //   float retentionTimeUncertainty; // s.o.
  //   float mzUncertainty;

  //   // only relevant for features
  //   int idxBin;
  //   unsigned int idxPeakStart; // degrees of freedom = idxPeakEnd - idxPeakStart
  //   unsigned int idxPeakEnd;
  //   float beta0;
  //   float beta1;
  //   float beta2;
  //   float beta3;
  // };

  struct CentroidPeak
  {
    float height;
    float area;
    float width;
    float heightUncertainty;
    float areaUncertainty;
    float dqsCen;
    float mz;
    float mzUncertainty;
    int scanNumber;
  };

  struct FeaturePeak
  {
    float height;
    float area;
    float width;
    float heightUncertainty;
    float areaUncertainty;
    float dqsPeak;       // only relevant for features
    float dqsBin;        // can be calculated when needed
    float dqsCen;        // can be calculated when needed
    float retentionTime; // only relevant for features
    float mz;
    float retentionTimeUncertainty;
    float mzUncertainty;
    int idxBin;
    unsigned int idxPeakStart; // degrees of freedom = idxPeakEnd - idxPeakStart
    unsigned int idxPeakEnd;
    float beta0;
    float beta1;
    float beta2;
    float beta3;
  };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H