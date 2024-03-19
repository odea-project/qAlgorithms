// qalgorhithms_datatype_mass_spectrum.cpp

// internal
#include "../include/qalgorithms_datatype_mass_spectrum.h"

// external
#include <iostream>

namespace q {
  namespace DataType {
    MassSpectrum::MassSpectrum() : data(std::unordered_map<DataField, void*>()) {
      // constructor
    }

    MassSpectrum::~MassSpectrum() {
      // destructor
    }

    // void MassSpectrum::print() {
    //   std::cout << "Intensity: ";
    //   for (int i = 0; i < data.size(); i++) {
    //     std::cout << data[DataField::DATAPOINT] << " ";
    //     std::cout << data[i]->intensity << " ";
    //   }
    //   std::cout << std::endl;

    //   std::cout << "MZ: ";
    //   for (int i = 0; i < data.size(); i++) {
    //     std::cout << data[i]->mz << " ";
    //   }
    //   std::cout << std::endl;

    //   std::cout << "Ionization Mode: ";
    //   if (ionizationMode == IonizationMode::Positive) {
    //     std::cout << "Positive" << std::endl;
    //   } else {
    //     std::cout << "Negative" << std::endl;
    //   }

    //   std::cout << "Measurement Mode: ";
    //   if (measurementMode == MeasurementMode::Profile) {
    //     std::cout << "Profile" << std::endl;
    //   } else { 
    //     std::cout << "Centroid" << std::endl;
    //   }

    //   std::cout << "MS Level: ";
    //   if (msLevel == MSLevel::MS1) {
    //     std::cout << "MS1" << std::endl;
    //   } else {
    //     std::cout << "MS2" << std::endl;
    //   }
    // }

    LC_MS::LC_MS() {
      // constructor
    }

    LC_MS::~LC_MS() {
      // destructor
    }
  }
}