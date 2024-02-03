// qalgorithms_peak.h
#ifndef QALGORITHMS_PEAK_H // Include guarde to prevent double inclusion
#define QALGORITHMS_PEAK_H

#include <map>
#include <iostream>

/* This file includes the q::peak class*/
namespace q {
  enum class Property {
          CoeffB0, // THIS MUST BE THE FIRST ENTRY
          CoeffB1,
          CoeffB2,
          CoeffB3,
          Xindex,
          Df,
          PeakID,
          SmplID,
          Position,
          ValleyPosition,
          Height,
          Width,
          Area,
          SigmaPosition,
          SigmaHeight,
          SigmaWidth,
          SigmaArea, 
          Dqs // THIS MUST BE THE LAST ENTRY
        };
  
  extern std::map<Property, std::string> propertyLabels;

  class Peak {
    public: /* List of Properties */

    private:
      std::map<Property, double> properties;

    public:
      /* support function to create the Peak object*/
      static std::map<Property, double> initializeProperties();

      // Constructor
      Peak();

      // Cleaning
      ~Peak();
      
      // Access methods
      double getProperty(Property property) const;
      void setProperty(Property property, double value);

      // Validation Methods
      void validate(); /* wrapper function for multiple validation methods */

      bool validate_coefficients(
          const double df,
          const double b1,
          const double b2,
          const double b3,
          const double xIndex,
          double& apex_position,
          double& valley_position) const;

      // Calculate Property Methods
      bool calcApex_position(
        const double df, 
        const double b1, 
        const double b2, 
        const double xIndex, 
        double& apex_position) const;
      
      bool calcApexValley_position(
        const double df, 
        const double b1, 
        const double b2, 
        const double b3, 
        const double xIndex, 
        double& apex_position, 
        double& valley_position) const;

      // Export methods
      void print();
  };
}
#endif // QALGORITHMS_PEAK_H