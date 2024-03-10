#ifndef QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
#define QALGORITHMS_DATATYPE_MASS_SPECTRUM_H

#include "qalgorithms_datatype.h"
#include <vector>

namespace q {

namespace DataType {   
    /**
     * @brief \b MassSpectrum: Class to represent a mass spectrum with additional metadata.
     * 
     * This class stores an entire mass spectrum, including spectral data points,
     * and metadata such as the MS level, ionization mode, and data type (profile or centroid).
     * 
     * @param msLevel MS1 or MS2
     * @param ionizationMode Ionization Mode : Positive or Negative
     * @param measurementMode Profile or Centroid
     * @param dataPoints: Vector of Structure element contains various categroies, see \ref SpectralPoint
     */
    class MassSpectrum {
    public:
        /**
         * @brief \b SpectralPoint: Struct to represent a single point in a mass spectrum.
         * 
         * This struct holds the mass-to-charge ratio (m/z), intensity,
         * and an optional quality score for the spectral data point.
         * @param mz
         * @param intensity
         * @param qualityScore: Optional Qualityparameter for centroids. \cite reuschenbach2022development
         */
        struct SpectralPoint {
            double mz; 
            double intensity; 
            double qualityScore; 

            SpectralPoint(double mz, double intensity, double qualityScore = 0.0)
                : mz(mz), intensity(intensity), qualityScore(qualityScore) {}
        };

        enum class MSLevel {MS1, MS2}; 
        enum class IonizationMode { Positive, Negative };
        enum class MeasurementMode { Profile, Centroid };

        MSLevel msLevel;
        IonizationMode ionizationMode;
        MeasurementMode measurementMode;
        std::vector<SpectralPoint> dataPoints; 


        MassSpectrum(MSLevel msLevel, IonizationMode ionizationMode, MeasurementMode measurementMode, std::vector<SpectralPoint> dataPoints)
            : msLevel(msLevel), ionizationMode(ionizationMode), measurementMode(measurementMode), dataPoints(dataPoints) {}

    };

    class LC_MS : public MassSpectrum {
    public:
        int scanNumber;
        double retentionTime;

        LC_MS(MSLevel msLevel, IonizationMode ionizationMode, MeasurementMode measurementMode, std::vector<SpectralPoint> dataPoints, int scanNumber, double retentionTime)
            : MassSpectrum(msLevel, ionizationMode, measurementMode, dataPoints), scanNumber(scanNumber), retentionTime(retentionTime) {}
    };

    class LC_IMS_MS : public MassSpectrum {
    public:
        int scanNumber;
        double retentionTime;
        double driftTime;

        LC_IMS_MS(MSLevel msLevel, IonizationMode ionizationMode, MeasurementMode measurementMode, std::vector<SpectralPoint> dataPoints, int scanNumber, double retentionTime, double driftTime)
            : MassSpectrum(msLevel, ionizationMode, measurementMode, dataPoints), scanNumber(scanNumber), retentionTime(retentionTime), driftTime(driftTime) {}
    };

}; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
