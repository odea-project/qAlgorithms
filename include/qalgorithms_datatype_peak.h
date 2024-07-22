// qalgorithms_datatype_peak.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

// internal
#include "qalgorithms_datatype.h"

// external
// #include <unordered_map>
// #include <iostream>
// #include <vector>

/* This file includes the q::DataType::Peak and q::DataType::PeakList classes*/
namespace q
{
  namespace DataType // @todo combine into other datatypes
  {
    /**
     * @brief A class to store peak data
     * @details The Peak class is used to store peak data. It contains the position, height, width, and area of the peak. The class also contains the uncertainty of the peak position, height, width, and area, the data quality score of the peak, the regression coefficients, the valley position, the degrees of freedom, and the index of the regression window position.
     * @param sampleID The sample ID
     * @param position The position of the peak
     * @param height The height of the peak
     * @param width The width of the peak
     * @param area The area of the peak
     * @param positionUncertainty The uncertainty of the peak position
     * @param heightUncertainty The uncertainty of the peak height
     * @param widthUncertainty The uncertainty of the peak width
     * @param areaUncertainty The uncertainty of the peak area
     * @param dqsPeak The data quality score of the peak \cite{reuschenbach2022development}
     * @param beta0 The regression coefficient @todo uniform naming, either beta_0 or beta0
     * @param beta1 The regression coefficient
     * @param beta2 The regression coefficient
     * @param beta3 The regression coefficient
     * @param valleyPosition The valley position of the peak
     * @param df The degrees of freedom
     * @param xIndex The index of the regression window position
     * @param x0 The center point (x value) of the regression window in the original data set
     * @param dx The difference in x near the peak position
     */
    class Peak
    {
    public:
      // constructors
      Peak();
      // Peak(int sampleID, double position, double height);

      // destructor
      ~Peak();

      // properties
      // required
      int sampleID;
      // double position;
      double height;

      // optional
      double area;
      double width;
      // double positionUncertainty;
      double heightUncertainty;
      // double widthUncertainty;
      double areaUncertainty;
      double dqsPeak;
      double dqsBin;
      double dqsCen;
      // double beta0;
      // double beta1;
      // double beta2;
      // double beta3;
      // double valleyPosition;
      // int df;
      // int xIndex;
      // double x0;
      // double dx;
      double retentionTime;
      double mz;
      double retentionTimeUncertainty;
      double mzUncertainty;
      // methods
      void print();
    };
  } // namespace DataType
} // namespace q

#endif // QALGORITHMS_DATATYPE_PEAK_H