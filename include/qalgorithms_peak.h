// qalgorithms_peak.h
#ifndef QALGORITHMS_PEAK_H // Include guarde to prevent double inclusion
#define QALGORITHMS_PEAK_H

#include <map>
#include <iostream>

/* This file includes the q::peak class*/
namespace q {
  class Peak {
    public: /* List of Properties */
      enum class Property {
          CoeffB0, // THIS MUST BE THE FIRST ENTRY
          CoeffB1,
          CoeffB2,
          CoeffB3,
          PeakID,
          SmplID,
          Position,
          Height,
          Width,
          Area,
          SigmaPosition,
          SigmaHeight,
          SigmaWidth,
          SigmaArea, 
          Dqs // THIS MUST BE THE LAST ENTRY
        };

    private:
      std::map<Property, double> properties;

    public:
      /* support function to create the Peak object*/
      static std::map<Property, double> initializeProperties();

      // Constructor
      Peak();

      // Cleaning
      ~Peak();
      
      //access methods
      double getProperty(Property property) const;
      void setProperty(Property property, double value);

      // export methods
      void print();
  };
}
#endif // QALGORITHMS_PEAK_H