// qalgorithms_datatype_peak.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

// internal
#include "qalgorithms_datatype.h"

// external
#include <unordered_map>

namespace q {
  namespace DataType {
    /**
     * @brief A class to store peak data
     * @details The Peak class is used to store peak data. It contains the position, height, width, and area of the peak. The class also contains the uncertainty of the peak position, height, width, and area, the data quality score of the peak, the regression coefficients, the valley position, the degrees of freedom, and the index of the regression window position.
     * @param peakID The peak ID
     * @param position The position of the peak
     * @param height The height of the peak
     * @param width The width of the peak
     * @param area The area of the peak
     * @param positionUncertainty The uncertainty of the peak position
     * @param heightUncertainty The uncertainty of the peak height
     * @param widthUncertainty The uncertainty of the peak width
     * @param areaUncertainty The uncertainty of the peak area
     * @param dqsPeak The data quality score of the peak \cite{reuschenbach2022development}
     * @param beta0 The regression coefficient
     * @param beta1 The regression coefficient
     * @param beta2 The regression coefficient
     * @param beta3 The regression coefficient
     * @param valleyPosition The valley position of the peak
     * @param df The degrees of freedom
     * @param xIndex The index of the regression window position
     */
    class Peak {
      public:
      // constructors
      Peak();
      Peak(int peakID, double position, double height);

      // destructor
      ~Peak();

      // properties
      // required
      int peakID;
      double position;
      double height;
     
      // optional
      double area;
      double width;
      double positionUncertainty;
      double heightUncertainty;
      double widthUncertainty;
      double areaUncertainty;
      double dqsPeak;
      double beta0;
      double beta1;
      double beta2;
      double beta3;
      double valleyPosition;
      int df;
      int xIndex;

      // debugging
      void print();
    };

    /**
     * @brief A class to store a list of peaks
     * @details The PeakList class is used to store a list of peaks. It contains a map of peaks, where the key is the peak ID and the value is the peak.
     * @param peaks A map of peaks
     */
    class PeakList {
      public:
      // constructors
      PeakList();
      PeakList(std::unordered_map<int,Peak> peaks);

      // destructor
      ~PeakList();

      // properties
      std::unordered_map<int, Peak> peaks;

      // methods
      void addPeak(Peak peak);
      void removePeak(int peakID);
      void clearPeaks();

      // debugging
      void print();
    };
  } // namespace DataType
} // namespace q

#endif // QALGORITHMS_DATATYPE_PEAK_H
