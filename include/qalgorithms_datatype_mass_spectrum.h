#ifndef QALGORITHMS_DATATYPE_MASS_SPECTRUM_H
#define QALGORITHMS_DATATYPE_MASS_SPECTRUM_H

// internal
#include "qalgorithms_datatype.h"

// external
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>
#include <algorithm>

namespace q
{

    namespace DataType // @todo combine all datatypes into one file
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
             */
            struct DataPoint
            {
                double intensity;
                double mz;
                int df;

                DataPoint(double intensity, double mz, int df) : intensity(intensity), mz(mz), df(df) {}
                // getters for x and y
                double &x() { return mz; }
                double &y() { return intensity; }
                void setY(double y) { intensity = y; }
            };

            // constructors
            MassSpectrum();

            /**
             * @brief Construct a new Mass Spectrum object from another Mass Spectrum object
             * @details The MassSpectrum constructor constructs a new Mass Spectrum object from another Mass Spectrum object. The constructor takes a reference to another Mass Spectrum object. All MetaData will be referenced, and the the last k data points will be moved to the new object.
             *
             * @param parent mass spectrum object
             * @param k number of data points to be moved to the new object, starting from the end of the reference object
             */
            MassSpectrum(MassSpectrum &parent, const size_t k);

            // destructor
            ~MassSpectrum();

            // methods
            /**
             * @brief Add a data point to the mass spectrum
             * @details The addDataPoint method adds a data point to the mass spectrum. The method takes the intensity, mz, and degrees of freedom of the data point.
             *
             * @param intensity The intensity of the mass spectrum data point
             * @param mz The mass-to-charge ratio of the mass spectrum data point
             * @param df The degrees of freedom of the mass spectrum data point
             */
            void
            addDataPoint(double intensity, double mz, int df);

            /**
             * @brief Sort the mass spectrum data points
             * @details The sortDataPoints method sorts the mass spectrum data points based on the x-axis values.
             */
            void
            sortDataPoints();

            // properties, i.e. metadata
            using variableType = std::variant< // allowed variable types for mass spectrum data
                int,                           // e.g. scan number
                size_t,                        // e.g. scan number, number of data points
                double,                        // e.g. retention time
                MSLevel,                       // e.g. MS1 or MS2
                IonizationMode,                // e.g. positive or negative
                MeasurementMode>;              // e.g. profile or centroid

            std::unordered_map<DataField, variableType> *metaData;
            std::vector<std::unique_ptr<DataPoint>> dataPoints;
            std::pair<bool, MassSpectrum *> isParent;

            /**
             * @brief This vector stores information for cutting the mass spectrum data.
             * @details The cuttingPoints vector can be seen as a job list for cutting the mass spectrum data. Each element in the vector shows the number of data points counted from the end of the dataPoints vector that should be moved to the new object. The number of elements within the cuttingPoints vector determines the number of new objects that will be created. E.g. cuttingPoints = {10, 5} will create two new objects. The first object will contain the last 10 data points of the original object while these datapoints are removed from the original object. The second object will contain the next last 5 data points of the original object, while these datapoints are also removed from the original object. The original object will contain all data points except the ones that were moved to the new objects.
             */
            std::vector<std::unique_ptr<size_t>> cuttingPoints;

            // getter
            int
            getScanNumber() const;
        };
    }; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_MASS_SPECTRUM_H