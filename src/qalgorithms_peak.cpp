// qalgorithms_peak.cpp

#include "../include/qalgorithms_peak.h"

namespace q {

  /* Global Map for Enum Items to Label strings. This is mostly used for exporting data */
  std::map<Property, std::string> propertyLabels = {
    {Property::CoeffB0, "CoeffB0"},
    {Property::CoeffB1, "CoeffB1"},
    {Property::CoeffB2, "CoeffB2"},
    {Property::CoeffB3, "CoeffB3"},
    {Property::Xindex, "Xindex"},
    {Property::Df, "Df"},
    {Property::PeakID, "PeakID"},
    {Property::SmplID, "SmplID"},
    {Property::Position, "Position"},
    {Property::ValleyPosition, "ValleyPosition"},
    {Property::Height, "Height"},
    {Property::Width, "Width"},
    {Property::Area, "Area"},
    {Property::SigmaPosition, "SigmaPosition"},
    {Property::SigmaHeight, "SigmaHeight"},
    {Property::SigmaWidth, "SigmaWidth"},
    {Property::SigmaArea, "SigmaArea"},
    {Property::Dqs, "Dqs"},
  };

  std::map<Property, double> Peak::initializeProperties() {
    std::map<Property, double> props;
    for (int i = static_cast<int>(Property::CoeffB0); 
          i < static_cast<int>(Property::Dqs); 
          i++) {
            props[static_cast<Property>(i)] = 0.0; //
    }
    return props;
  }

  // Constructor
  Peak::Peak() : properties(initializeProperties()) {
    /* optional code for initialization */
  }

  // Destructor
  Peak::~Peak() {};

  // Access
  double Peak::getProperty(Property property) const {
    auto it = properties.find(property);
    if (it != properties.end()) {
      return it -> second;
    }
    /* error if property does not exist*/
    throw std::out_of_range("Property not found");
  }

  void Peak::setProperty(Property property, double value) {
    properties[property] = value;
  }

  // Validation Methods
  void Peak::validate() {
    std::cout << validate_coefficients(
      properties[Property::Df],
      properties[Property::CoeffB1],
      properties[Property::CoeffB2],
      properties[Property::CoeffB3],
      properties[Property::Xindex],
      properties[Property::Position],
      properties[Property::ValleyPosition]) << std::endl;
  }

  bool Peak::validate_coefficients(
      const double df,
      const double b1,
      const double b2,
      const double b3,
      const double xIndex,
      double& apex_position,
      double& valley_position) const {
    /*
    This functions checks if a set of regression coefficients (b0, b1, b2, b3) are valid
    and describe an analytical peak after exp-transform.
     # valid peak case 1: b2<0 & b3<0 & apex in window
     # valid peak case 2a: b2>0 & b3<0 & b1>0 & |valley - apex| > 2 & apex in window
     # valud peak case 2b: b2<0 & b3>0 & b1<0 & |valley - apex| > 2 & apex in window
    */

    /* creating unique checksum */
    int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);

    switch (key) {
        case 7:  // Case 1a: apex left
            return calcApex_position(df, b1, b2, xIndex, apex_position);
        case 3:  // Case 1b: apex right
            return calcApex_position(df, b1, b3, xIndex, apex_position);
        case 6:  // Case 2a: apex left | valley right
            return calcApexValley_position(df, b1, b2, b3, xIndex, apex_position, valley_position);
        case 1:  // Case 2b: apex right | valley left
            return calcApexValley_position(df, b1, b3, b2, xIndex, apex_position, valley_position);
        default:
            return false;
    };
  }

  // Calculate Property Methods
  bool Peak::calcApex_position(
    const double df, 
    const double b1, 
    const double b2, 
    const double xIndex, 
    double& apex_position 
    ) const {
      apex_position = - b1 / 2 / b2;
      /* check if apex position is in window with at least one point buffer */
      if (abs(apex_position) < (df+3)/2) {
        apex_position += xIndex;
        return true;
      } else {
        return false;
      }
  }

  bool Peak::calcApexValley_position(
    const double df, 
    const double b1, 
    const double b2, 
    const double b3, 
    const double xIndex, 
    double& apex_position, 
    double& valley_position) const {
      apex_position = - b1 / 2 / b2;
      valley_position = -b1 / 2 / b3;
      /* check if apex position is in window with at least one point buffer */
      if ((abs(apex_position) < (df+3)/2) & (abs(apex_position - valley_position) > 2)) {
        apex_position += xIndex;
        valley_position += xIndex;
        return true;
      } else {
        return false;
      }
  }

  void Peak::print() {
    std::cout << std::endl;
    std::cout << "#######################" << std::endl;
    /* Printing all properties */
    for (const auto& prop : properties) {
            std::cout << propertyLabels[prop.first] << ": " << prop.second << std::endl;
        }
    std::cout << "#######################" << std::endl;
    std::cout << std::endl;
  }
}