#ifndef QALGORITHMS_DATATYPE_H
#define QALGORITHMS_DATATYPE_H

// up to date with commit d769ca8
namespace q
{

    namespace DataType
    {
        /**
         * @brief Different types of data that can be stored in a data file
         * @details The data fields are used to store different types of data in a data file.
         * @param INTENSITY The intensity of the MS signal
         * @param SUB_ID The sub-dataset ID
         * @param DATA_ID The data ID
         * @param DATAPOINT The data point
         * @param IONIZATIONMODE The ionization mode, i.e. whether the signal is from positive or negative mode
         * @param MZ The mass-to-charge ratio of the MS signal
         * @param MSLEVEL The MS level, i.e. whether the signal is from MS1 or MS2
         * @param MEASUREMENTMODE The measurement mode, i.e. whether the signal is profile or centroid
         * @param RETENTIONTIME The retention time of the LC-MS signal
         * @param SCANNUMBER The scan number of the LC-MS signal
         * @param PEAK_ID The peak ID
         * @param PEAK_AREA The area of the peak
         * @param PEAK_HEIGHT The height of the peak
         * @param PEAK_WIDTH The width of the peak
         * @param PEAK_POSITION The position of the peak
         * @param DQS_PEAK The data quality score of the peak \cite{reuschenbach2022development}
         * @param PEAK_AREA_UNCERTAINTY The uncertainty of the peak area
         * @param PEAK_HEIGHT_UNCERTAINTY The uncertainty of the peak height
         * @param PEAK_WIDTH_UNCERTAINTY The uncertainty of the peak width
         * @param PEAK_POSITION_UNCERTAINTY The uncertainty of the peak position
         * @param BETA_0 The regression coefficient
         * @param BETA_1 The regression coefficient
         * @param BETA_2 The regression coefficient
         * @param BETA_3 The regression coefficient
         * @param VALLEY_POSITION The valley position of the peak
         * @param DF The degrees of freedom
         * @param X_INDEX The index of the regression window position
         *
         */
        enum class DataField
        {
            // general fields
            INTENSITY, // signal intensity
            SUB_ID,    // sub-dataset ID
            DATA_ID,   // data ID
            DATAPOINT, // data point

            // MS fields
            IONIZATIONMODE,  // positive or negative
            MEASUREMENTMODE, // profile or centroid
            MSLEVEL,         // MS1 or MS2
            MZ,              // mass-to-charge ratio

            // Chromatography fields
            RETENTIONTIME, // retention time
            SCANNUMBER,    // scan number

            // Peak-related fields
            PEAK_ID,
            PEAK_AREA,
            PEAK_HEIGHT,
            PEAK_WIDTH,
            PEAK_POSITION,

            // Quality scores
            DQS_CENTROID,
            DQS_PEAK,
            DQS_BIN,

            // Uncertainty fields
            PEAK_AREA_UNCERTAINTY,
            PEAK_HEIGHT_UNCERTAINTY,
            PEAK_WIDTH_UNCERTAINTY,
            PEAK_POSITION_UNCERTAINTY,
            RETENTIONTIME_UNCERTAINTY,
            MZ_UNCERTAINTY,

            // Regression fields
            BETA_0,          // regression coefficients
            BETA_1,          // regression coefficients
            BETA_2,          // regression coefficients
            BETA_3,          // regression coefficients
            VALLEY_POSITION, // optional: some peaks have a valley position
            DF,              // degrees of freedom
            X_INDEX,         // index of the regression window position

            // Add new data fields here
        };

        // additional categorical data sub types
        enum class IonizationMode
        {
            Positive,
            Negative
        };
        enum class MeasurementMode
        {
            Profile,
            Centroid
        };
        enum class MSLevel
        {
            MS1,
            MS2
        };
    }; // namespace datatype
} // namespace q

#endif // QALGORITHMS_DATATYPE_H