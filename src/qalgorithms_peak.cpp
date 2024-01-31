// qalgorithms_peak.cpp

#include "../include/qalgorithms_peak.h"

namespace q {
  std::map<Peak::Property, double> Peak::initializeProperties() {
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

  // Access
  double Peak::getProperty(Property property) const {
    auto it = properties.find(property);
    if (it != properties.end()) {
      return it -> second;
    }
  }

  void Peak::setProperty(Property property, double value) {
    properties[property] = value;
  }

  void Peak::print() {
    std::cout << std::endl;
    std::cout << "#######################" << std::endl;
    /* Some Printings to add here - it should be a list of properties */
    std::cout << "#######################" << std::endl;
    std::cout << std::endl;
  }
}