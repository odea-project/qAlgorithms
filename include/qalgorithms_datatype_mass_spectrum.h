#ifndef QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
#define QALGORITHMS_DATATYPE_MASS_SPECTRUM_H

#include "qalgorithms_datatype.h"
#include <vector>

namespace q {

namespace DataType {   
    /**
     * @brief A class to store mass spectrum data
     * @details The MassSpectrum class is used to store mass spectrum data. It contains the intensity, mz, ionization mode, measurement mode, and MS level of the mass spectrum.  
     * @param intensity The intensity of the MS signal
     * @param mz The mass-to-charge ratio of the MS signal
     * @param ionizationMode The ionization mode, i.e. whether the signal is from positive or negative mode
     * @param measurementMode The measurement mode, i.e. whether the signal is profile or centroid
     * @param msLevel The MS level, i.e. whether the signal is from MS1 or MS2
     * 
     */
    class MassSpectrum {
        public:
        // constructors
        MassSpectrum();
        MassSpectrum(std::vector<double> intensity, std::vector<double> mz, IonizationMode ionizationMode, MeasurementMode measurementMode, MSLevel msLevel);

        // destructor
        ~MassSpectrum();

        // properties
        std::vector<double> intensity; 
        std::vector<double> mz;
        IonizationMode ionizationMode;
        MeasurementMode measurementMode;
        MSLevel msLevel;

        // methods
        void setIntensity(std::vector<double>& intensity);
        void setMz(std::vector<double>& mz);
        void setIonizationMode(IonizationMode ionizationMode);
        void setMeasurementMode(MeasurementMode measurementMode);
        void setMSLevel(MSLevel msLevel);

        // debugging
        void print();
    };

    /**
     * @brief A class to store LC-MS data
     * @details The LC_MS class is used to store LC-MS data. It contains the intensity, mz, ionization mode, measurement mode, MS level, retention time, and scan number of the LC-MS data. The LC_MS class is a subclass of the MassSpectrum class. \ref MassSpectrum
     * 
     * @param intensity The intensity of the MS signal
     * @param mz The mass-to-charge ratio of the MS signal
     * @param ionizationMode The ionization mode, i.e. whether the signal is from positive or negative mode
     * @param measurementMode The measurement mode, i.e. whether the signal is profile or centroid
     * @param msLevel The MS level, i.e. whether the signal is from MS1 or MS2
     * @param retentionTime The retention time of the LC-MS signal
     * @param scanNumber The scan number of the LC-MS signal
     * 
     */
    class LC_MS : public MassSpectrum {
        public:
        // constructors
        LC_MS();
        LC_MS(std::vector<double> intensity, std::vector<double> mz, IonizationMode ionizationMode, MeasurementMode measurementMode, MSLevel msLevel, double retentionTime, int scanNumber);

        // destructor
        ~LC_MS();

        // properties
        double retentionTime;
        int scanNumber;

        // debugging
        void print();
    };
}; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
