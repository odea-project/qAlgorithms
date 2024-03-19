#ifndef QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
#define QALGORITHMS_DATATYPE_MASS_SPECTRUM_H

#include "qalgorithms_datatype.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

namespace q
{

    namespace DataType
    {
        /**
         * @brief A class to store mass spectrum data
         * @details The MassSpectrum class is used to store mass spectrum data. It contains a map of Pointers to general property variables that are organized using the DataField as key. \ref DataField 
         * Moreover, it contains a struct for the data point. \ref DataPoint 
         * This struct contains the intensity, mz, and degrees of freedom (important for the uncertainty estimation when interpolating the data is considered) and will be organized in a map of Pointers as well.
         * @param data The map of Pointers to general property variables that are organized using the DataField as key
         */
        class MassSpectrum
        {
        public:
            // struct for data point
            struct DataPoint
            {
                double intensity;
                double mz;
                int df;

                DataPoint(double intensity, double mz, int df) : intensity(intensity), mz(mz), df(df) {}
            };

            // constructors
            MassSpectrum();

            // destructor
            ~MassSpectrum();

            // properties
            using variableType = std::variant
            <
                int,
                double, 
                MSLevel, 
                IonizationMode, 
                MeasurementMode, 
                std::unordered_map<int, std::unique_ptr<DataPoint>>
            >;
            std::unordered_map<DataField, std::unique_ptr<variableType>> data;
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
        class LC_MS : public MassSpectrum
        {
        public:
            // constructors
            LC_MS();

            // destructor
            ~LC_MS();

            // debugging
            void print();
        };
    }; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
