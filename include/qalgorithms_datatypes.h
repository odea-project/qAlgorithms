// qalgorithms_datatypes.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

#include <immintrin.h> // AVX
#include <cstddef>     // errorcode in EIC @todo just make this an int
#include <vector>

/* This file includes the structs used for data management in qAlgorithms*/

namespace qAlgorithms
{
  struct dataPoint // @todo remove this (again)
  {
    float x;
    float y;
    bool df;
    float dqsCentroid;
    float dqsBinning;
    int scanNumber;
    float mz;
  };

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

  struct RegCoeffs
  {
    float b0, b1, b2, b3 = 0;
  };

  struct RegressionGauss
  {
    RegCoeffs newCoeffs;
    __m128 coeff;                 // regression coefficients
    int index_x0 = 0;             // index of window center (x==0) in the Y matrix
    int scale = 0;                // scale of the regression window, i.e., 2*scale+1 = window size
    int df = 0;                   // degree of freedom, interpolated data points will not be considered
    float apex_position = 0;      // position of the apex of the peak
    float mse = 0;                // mean squared error; this is not always used and should not be a part of the regression struct @todo
    unsigned int left_limit = 0;  // left limit of the peak regression window
    unsigned int right_limit = 0; // right limit of the peak regression window
    float area = 0;               // area of the peak
    float uncertainty_area = 0;   // uncertainty of the area
    float uncertainty_pos = 0;    // uncertainty of the position
    float uncertainty_height = 0;
    bool isValid = false; // flag to indicate if the regression is valid
  };

  struct CentroidPeak
  {
    double mz;
    float height;
    float area;
    float width;
    float heightUncertainty;
    float areaUncertainty;
    float dqsCen;
    float mzUncertainty;
    unsigned int scanNumber;
    unsigned int df; // degrees of freedom
  };

  struct FeaturePeak
  {
    RegCoeffs coefficients;
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
  };

  struct qCentroid
  {
    double mz;
    float mzError = -1;
    unsigned int scanNo;
    float int_area;   // the intensity is never used during binning @todo
    float int_height; // s.o.
    float DQSCentroid;
    unsigned int df; // degrees of freedom
  };

  struct CentroidedData
  {
    std::vector<std::vector<qCentroid>> allDatapoints;
    size_t lengthAllPoints; // number of centroids in all scans
  };

  struct EIC // Extracted Ion Chromatogram
  {
    // std::byte errorcode;
    std::vector<unsigned int> scanNumbers;
    std::vector<float> rententionTimes;
    std::vector<float> mz;
    std::vector<float> predInterval;
    std::vector<float> ints_area;
    std::vector<float> ints_height;
    std::vector<unsigned int> df;
    std::vector<float> DQSB;
    std::vector<float> DQSC;
  };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H