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
            /**
             * @brief Struct to store mass spectrum data points
             * @details The DataPoint struct is used to store mass spectrum data points. It contains the intensity, mz, and degrees of freedom (important for the uncertainty estimation when interpolating the data is considered).
             * @param intensity The intensity of the mass spectrum data point
             * @param mz The mass-to-charge ratio of the mass spectrum data point
             * @param df The degrees of freedom of the mass spectrum data point
             * @param DataPoint() Constructor for the DataPoint struct
             * @param x() Getter for the x-axis value
             * @param y() Getter for the y-axis value
             */
            struct DataPoint
            {
                double intensity;
                double mz;
                int df;

                DataPoint(double intensity, double mz, int df) : intensity(intensity), mz(mz), df(df) {}
                //getters for x and y
                double& x() { return mz; }
                double& y() { return intensity; }
            };

            // constructors
            MassSpectrum();

            // destructor
            ~MassSpectrum();

            // methods

            // properties
            using variableType = std::variant // allowed variable types for mass spectrum data
            <
                int, // e.g. scan number
                double, // e.g. retention time
                MSLevel, // e.g. MS1 or MS2
                IonizationMode, // e.g. positive or negative
                MeasurementMode, // e.g. profile or centroid
                std::vector<std::unique_ptr<DataPoint>> // e.g. intensity, mz, degrees of freedom
            >;
            std::unordered_map<DataField, variableType> data;
        };
    }; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
