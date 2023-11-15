// qalgorithms.h
#ifndef QALGORITHMS_H // Include guarde to prevent double inclusion
#define QALGORITHMS_H

#include <stdexcept>
#include <cmath> 
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <variant>
#include <thread> // for parallel processing
#include <mutex> // for parallel processing
#include "qalgorithms_matrix.h"
#include "qalgorithms_utils.h"

namespace q {
  struct dataID {
    int sampleID;
    int subSampleID;

    /* comparison operator */
    bool operator<(const dataID& other) const;
  };
  enum class Mode {
    DEBUGGING,
    SILENT,
    PROGRESS
  };

  class Peakproperties
  {
  public:
  // constructors
    Peakproperties();

    Peakproperties(
      const double _coeff_b0,
      const double _coeff_b1,
      const double _coeff_b2,
      const double _coeff_b3,
      const double _peakID,
      const double _smplID,
      const double _position,
      const double _height,
      const double _width,
      const double _area,
      const double _sigmaPosition,
      const double _sigmaHeight,
      const double _sigmaWidth,
      const double _sigmaArea,
      const double _dqs);
  
  // debuging & printing
    enum PropertiesNames{
      COEFF_B0,
      COEFF_B1,
      COEFF_B2,
      COEFF_B3,
      PEAKID,
      SMPLID,
      POSITION,
      HEIGHT,
      WIDTH,
      AREA,
      SIGMAPOSITION,
      SIGMAHEIGHT,
      SIGMAWIDTH,
      SIGMAAREA,
      DQS,
    };
    
    double getProperty(PropertiesNames varName) const;
    void print() const;

  private:
    double coeff_b0;
    double coeff_b1;
    double coeff_b2;
    double coeff_b3;
    double peakID;
    double smplID;
    double position;
    double height;
    double width;
    double area;
    double sigmaPosition;
    double sigmaHeight;
    double sigmaWidth;
    double sigmaArea;
    double dqs;
  };

  class Peakmodel {
  public:
  // Constructors
    Peakmodel();
    // Constructor to initialize Peak model based on an upper limit for a
    Peakmodel(int a_max);
    // Constructor to initialize Peak model based on a lower and an upper limit for a
    Peakmodel(int a_min, int a_max);
    // Constructor to initialize Peak model based on a vector for a
    Peakmodel(std::vector<int> A);

  // Functions
    // add t_values
    void loadTValues(const tValues& tData);
    // create model
    void calculateDesignMatrix(int a);
    void calculatePseudoInverse(int a);
    // add data
    void addSingleMeasurement(Matrix& xyData, bool zero_interpolation = true);
    void addMultipleMeasurements(Matrix& xyData, bool zero_interpolation = true);
    // correct data
    void gapFilling(Matrix& xyData);
    void zeroInterpolation(Matrix& xyData);
    void zeroInterpolation_oneDirection(Matrix& xyData);
    void dataSplitting(dataID key, Matrix& xyData, std::map<dataID, Matrix>& DataSet);
    // get 
    std::vector<int> getScales() const;
    std::vector<int> getScaleVec(const std::vector<int>& scales, const int n, const int N) const;
    Matrix getDesignMatrix(int a) const;
    Matrix getPseudoInverse(int a) const; 
    Matrix getData(int sampleID, int subSampleID) const;

    // Apply Peakmodel to data (main function)
    void findPeaks();
    // Perform the Running Peak Regression
    void runRegression(const std::pair<const dataID, const Matrix>& pair, const std::vector<int>& scales, const size_t k, const int s);

    void convolveP(
      Matrix& beta, 
      std::vector<int>& xIndices, 
      const Matrix& P, 
      const Matrix& ylog, 
      int& currentIndex);

    double calcMse(const Matrix& yhat, const Matrix& y) const;

    // Filter functions
    void coefficientCriterion(
      const int N,
      bool*& fltrVec, 
      const std::vector<int>& scaleVec, 
      std::vector<double>& apex_positions, 
      std::vector<double>& valley_positions, 
      const std::vector<int>& xIndices, 
      const Matrix& beta,
      std::vector<int>& idx);

    void coefficientCriterionCases(
      const int scale, 
      const double b1, 
      const double b2, 
      const double b3, 
      const int xIndex, 
      double& apex_position, 
      double& valley_position, 
      bool& fltrVal,
      std::vector<int>& idx,
      const size_t IDX);

    void quadraticTermCriterion(
      const int N,
      bool*& fltrVec, 
      const std::vector<int>& scaleVec, 
      const std::vector<int>& xIndices, 
      const Matrix& beta, 
      const Matrix& ylog, 
      std::vector<double>& mse,
      const std::vector<int>& idx1,
      std::vector<int>& idx2); 

    void parameterCriterion(
      const int N,
      bool*& fltrVec,
      std::vector<double>& peakHeight,
      std::vector<double>& peakHeight_uncertainty,
      std::vector<double>& peakArea,
      std::vector<double>& peakArea_uncertainty,
      const std::vector<int>& scaleVec,
      const Matrix& beta,
      const std::vector<double>& mse,
      const std::vector<int>& idx1,
      std::vector<int>& idx2);
    
    void peakCoverageCriterion(
      const int N,
      bool*& fltrVec,
      std::vector<double>& peakArea,
      const std::vector<int>& scaleVec, 
      const std::vector<int>& xIndices, 
      const std::vector<double>& apex_positions, 
      const std::vector<double>& valley_positions,
      const Matrix& beta,
      const std::vector<int>& idx1,
      std::vector<int>& idx2);

    // Peak Parameter Calculations
    void calcApex_position(
      const int scale, 
      const double b1, 
      const double b2, 
      const int xIndex, 
      double& apex_position, 
      bool& fltrVal);

    void calcApexValley_position(
      const int scale, 
      const double b1, 
      const double b2, 
      const double b3, 
      const int xIndex, 
      double& apex_position, 
      double& valley_position, 
      bool& fltrVal);
    
    std::pair<double,double> calcPeakHeight(
      const double b0,
      const double b1,
      const double b2,
      const double b3
      ) const;

    Matrix calcJacobianMatrix_Height(
      const double x_apex,
      const double y_apex
      ) const;

    std::vector<double> calcPeakArea(
      const double b0,
      const double b1,
      const double b2,
      const double b3,
      const double scale
      ) const;
    
    double calcPeakArea_half(
      const double b0,
      const double b1,
      const double b2,
      const double edge,
      const bool isleft,
      const bool hasvalley
    ) const;

    double calcPeakArea_halfValley(

     ) const;

    Matrix calcJacobianMatrix_Position(
        const double b1,
        const double b2,
        const double b3
    ) const;

    Matrix calcJacobianMatrix_Area(
        const double b0,
        const double b1,
        const double b2,
        const double b3
      ) const;
    
    Matrix calcJacobianMatrix_Area_half(
      const double b0,
      double b1,
      const double b2,
      const bool isLeft
    ) const;

    Matrix calcJacobianMatrix_Area_half_Valley(
      const double b0,
      double b1,
      const double b2,
      const bool isLeft
    ) const;

    double calcPeakAreaNotCovered(
      const double b0,
      const double b1,
      const double b2,
      const double b3,
      const double left_edge,
      const double right_edge) const;
    
    void mergeRegressions(
      const int N,
      bool*& fltrVec,
      const Matrix& beta,
      const std::vector<int>& scaleVec,
      const std::vector<int>& xIndices, 
      const std::vector<double>& apex_position,
      std::vector<double>& apex_position_uncertainty,
      const std::vector<double>& valley_position,
      const Matrix& Y,
      const std::vector<double> mse,
      const std::vector<int>& idx1,
      std::vector<int>& idx2
    );

    void rescalePosition(
      const int N,
      bool*& fltrVec,
      const Matrix& x_data,
      std::vector<double>& apex_position,
      std::vector<double>& apex_positions_uncertainty,
      const std::vector<int>& idx1
    );
    
    void exportResults(
      const Matrix& beta, 
      bool*& fltrVec, 
      const std::vector<int>& xIndices,
      const int N,
      const int smplID,
      const std::vector<double>& apex_position,
      const std::vector<double>& apex_position_uncertainty,
      const std::vector<double>& peakHeight,
      const std::vector<double>& peakHeight_uncertainty,
      const std::vector<double>& peakArea,
      const std::vector<double>& peakArea_uncertainty,
      const std::vector<int>& idx1,
      int& peakID);

    // debugging & printing
    std::vector<double> getPeakProperties(const Peakproperties::PropertiesNames& varName) const;

    const Peakproperties& operator[](int ID) const;

    void printTValues(); 

    void setMode(Mode m) {mode = m;};
    Mode getMode() const {return mode;};

  private:
    std::map<int, Matrix> designMatrices;
    std::map<int, Matrix> inverseMatrices;
    std::map<int, Matrix> pseudoInverses;
    std::map<dataID, Matrix> measurementData;
    std::map<int, Peakproperties> peakProperties;
    std::map<int, double> tVal;
    Mode mode;
    size_t n_measurementData;
};

};

#endif // QALGORITHMS_H