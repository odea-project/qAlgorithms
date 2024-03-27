// qalgorithms_peak.h
#ifndef QALGORITHMS_PEAK_H // Include guarde to prevent double inclusion
#define QALGORITHMS_PEAK_H

// external
#include <map>
#include <iostream>
#include <array>

/* This file includes the q::peak class*/
namespace q
{
  class Peak
  {
    public:
      // Constructor
      Peak();
      Peak(int index, int scale);
      // Destructor
      ~Peak();

      // methods
      // getters
      int getIndex() const;
      int getScale() const;
      const std::array<double, 4>& getB() const;
      double getPositionNorm() const;
      double getPosition() const;
      double getHeight() const;
      double getWidth() const;
      double getArea() const;
      int getDf() const;
      double getPositionUncertainty() const;
      double getHeightUncertainty() const;
      double getWidthUncertainty() const;
      double getAreaUncertainty() const;
      double getMse() const;
      // setters
      void setIndex(int index);
      void setScale(int scale);
      void setB(const std::array<double, 4> &B);
      void setPositionNorm(double position_norm);
      void setPosition(double position);
      void setHeight(double height);
      void setWidth(double width);
      void setArea(double area);
      void setDf(int df);
      void setPositionUncertainty(double position_uncertainty);
      void setHeightUncertainty(double height_uncertainty);
      void setWidthUncertainty(double width_uncertainty);
      void setAreaUncertainty(double area_uncertainty);
      void setMse(double mse);
      // debugging
      void print() const;
    private:
      // Member variables
      // regression information
      int index; // index of the center of the regression window within the original data set
      int scale; // scale of the regression window (e.g., 2 means 5 data points, i.e., -2:2)
      std::array<double, 4> B; // regression coefficients
      double position_norm; // normalized position of the peak within the regression window

      // peak information
      double position; // position of the peak in the original data set
      double height; // height of the peak
      double width; // width of the peak
      double area; // area of the peak (area under the curve till the baseline=0)

      // uncertainty information
      int df; // degrees of freedom
      double position_uncertainty; // uncertainty of the position
      double height_uncertainty; // uncertainty of the height
      double width_uncertainty; // uncertainty of the width
      double area_uncertainty; // uncertainty of the area
      double mse; // mean squared error of the regression
  };
}
#endif // QALGORITHMS_PEAK_H