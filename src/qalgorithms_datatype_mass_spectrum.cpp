// qalgorhithms_datatype_mass_spectrum.cpp

// internal
#include "../include/qalgorithms_datatype_mass_spectrum.h"

// external
#include <iostream>

namespace q {
  namespace DataType {
    MassSpectrum::MassSpectrum() {
      intensity = std::vector<double>();
      mz = std::vector<double>();
      ionizationMode = IonizationMode::Positive;
      measurementMode = MeasurementMode::Profile;
      msLevel = MSLevel::MS1;
    }

    MassSpectrum::MassSpectrum(std::vector<double> intensity, std::vector<double> mz, IonizationMode ionizationMode, MeasurementMode measurementMode, MSLevel msLevel) {
      this->intensity = intensity;
      this->mz = mz;
      this->ionizationMode = ionizationMode;
      this->measurementMode = measurementMode;
      this->msLevel = msLevel;
    }

    MassSpectrum::~MassSpectrum() {
      // destructor
    }

    void MassSpectrum::setIntensity(std::vector<double>& intensity) {
      this->intensity = intensity;
    }

    void MassSpectrum::setMz(std::vector<double>& mz) {
      this->mz = mz;
    }

    void MassSpectrum::setIonizationMode(IonizationMode ionizationMode) {
      this->ionizationMode = ionizationMode;
    }

    void MassSpectrum::setMeasurementMode(MeasurementMode measurementMode) {
      this->measurementMode = measurementMode;
    }

    void MassSpectrum::setMSLevel(MSLevel msLevel) {
      this->msLevel = msLevel;
    }

    void MassSpectrum::print() {
      std::cout << "Intensity: ";
      for (int i = 0; i < intensity.size(); i++) {
        std::cout << intensity[i] << " ";
      }
      std::cout << std::endl;

      std::cout << "MZ: ";
      for (int i = 0; i < mz.size(); i++) {
        std::cout << mz[i] << " ";
      }
      std::cout << std::endl;

      std::cout << "Ionization Mode: ";
      if (ionizationMode == IonizationMode::Positive) {
        std::cout << "Positive" << std::endl;
      } else {
        std::cout << "Negative" << std::endl;
      }

      std::cout << "Measurement Mode: ";
      if (measurementMode == MeasurementMode::Profile) {
        std::cout << "Profile" << std::endl;
      } else { 
        std::cout << "Centroid" << std::endl;
      }

      std::cout << "MS Level: ";
      if (msLevel == MSLevel::MS1) {
        std::cout << "MS1" << std::endl;
      } else {
        std::cout << "MS2" << std::endl;
      }
    }

    LC_MS::LC_MS() {
      retentionTime = 0;
      scanNumber = 0;
    }

    LC_MS::LC_MS(std::vector<double> intensity, std::vector<double> mz, IonizationMode ionizationMode, MeasurementMode measurementMode, MSLevel msLevel, double retentionTime, int scanNumber) : MassSpectrum(intensity, mz, ionizationMode, measurementMode, msLevel) {
      this->retentionTime = retentionTime;
      this->scanNumber = scanNumber;
    }

    LC_MS::~LC_MS() {
      // destructor
    }
  }
}