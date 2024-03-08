#ifndef QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
#define QALGORITHMS_DATATYPE_MASS_SPECTRUM_H

#include <vector>

namespace q {

class DataType {
public:
    
    /**
     * @brief Class to represent a mass spectrum with additional metadata.
     * 
     * This class stores an entire mass spectrum, including spectral data points,
     * and metadata such as the MS level, ionization mode, and data type (profile or centroid).
     * 
     * @param msLevel MS1 or MS2
     * @param mode Ionization Mode : Positive or Negative
     * @param dataType Profile or Centroid
     * @param points: Vector of Structure element contains various categroies, see \ref SpectralPoint
     */
    class MassSpectrum {
    public:
        /**
         * @brief Struct to represent a single point in a mass spectrum.
         * 
         * This struct holds the mass-to-charge ratio (m/z), intensity,
         * and an optional quality score for the spectral data point.
         * @param mz
         * @param intensity
         * @param qualityScore: Optional Qualityparameter for centroids. \cite reuschenbach2022development
         */
        struct SpectralPoint {
            double mz; ///< Mass-to-charge ratio
            double intensity; ///< Signal intensity
            double qualityScore; ///< Optional quality score for the data point

            SpectralPoint(double mz, double intensity, double qualityScore = 0.0)
                : mz(mz), intensity(intensity), qualityScore(qualityScore) {}
        };

        enum class MSLevel {MS1, MS2}; 
        enum class IonizationMode { Positive, Negative };
        enum class DataFormat { Profile, Centroid };

        MSLevel msLevel;
        IonizationMode mode;
        DataFormat dataType;
        std::vector<SpectralPoint> points; ///< Vector of spectral data points

        /**
         * @brief Construct a new Mass Spectrum object
         * 
         * @param msLevel MS1 or MS2
         * @param mode Ionization Mode : Positive or Negative
         * @param dataType Profile or Centroid
         */
        MassSpectrum(MSLevel msLevel, IonizationMode mode, DataFormat dataType)
            : msLevel(msLevel), mode(mode), dataType(dataType) {}


        // Additional methods for processing and accessing the mass spectrum data can be added here
    };

    class LC_MS : public MassSpectrum {
    public:
        int scanNumber;
        double retentionTime;

        LC_MS(MSLevel msLevel, IonizationMode mode, DataFormat dataType, int scanNumber, double retentionTime)
            : MassSpectrum(msLevel, mode, dataType), scanNumber(scanNumber), retentionTime(retentionTime) {}
    };

    class LC_IMS_MS : public MassSpectrum {
    public:
        int scanNumber;
        double retentionTime;
        double driftTime;

        LC_IMS_MS(MSLevel msLevel, IonizationMode mode, DataFormat dataType, int scanNumber, double retentionTime)
            : MassSpectrum(msLevel, mode, dataType), scanNumber(scanNumber), retentionTime(retentionTime), driftTime(driftTime) {}
    };

}; // class datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
