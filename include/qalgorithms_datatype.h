#ifndef QALGORITHMS_DATATYPE_H
#define QALGORITHMS_DATATYPE_H

namespace q
{

    namespace DataType
    {
        /**
         * @brief Different types of data that can be stored in a data file
         * @details The data fields are used to store different types of data in a data file.
         * @param INTENSITY The intensity of the MS signal
         * @param SUB_ID The sub-dataset ID @todo unused
         * @param DATA_ID The data ID @todo unused
         * @param DATAPOINT The data point @todo unused
         * @param IONIZATIONMODE The ionization mode, i.e. whether the signal is from positive or negative mode @todo unused
         * @param MZ The mass-to-charge ratio of the MS signal
         * @param MSLEVEL The MS level, i.e. whether the signal is from MS1 or MS2 @todo unused
         * @param MEASUREMENTMODE The measurement mode, i.e. whether the signal is profile or centroid @todo underscore  @todo unused
         * @param RETENTIONTIME The retention time of the LC-MS signal @todo RT as property of the LC, scan as property of MS
         * @param SCANNUMBER The scan number of the LC-MS signal
         * @param PEAK_ID The peak ID @todo unused
         * @param PEAK_AREA The area of the peak @todo unused
         * @param PEAK_HEIGHT The height of the peak @todo unused
         * @param PEAK_WIDTH The width of the peak @todo unused
         * @param PEAK_POSITION The position of the peak @todo unused
         * @param DQS_PEAK The data quality score of the peak \cite{reuschenbach2022development} @todo unused
         * @param PEAK_AREA_UNCERTAINTY The uncertainty of the peak area @todo unused
         * @param PEAK_HEIGHT_UNCERTAINTY The uncertainty of the peak height @todo unused
         * @param PEAK_WIDTH_UNCERTAINTY The uncertainty of the peak width @todo unused
         * @param PEAK_POSITION_UNCERTAINTY The uncertainty of the peak position @todo unused
         * @param BETA_0 The regression coefficient @todo unused
         * @param BETA_1 The regression coefficient @todo unused
         * @param BETA_2 The regression coefficient @todo unused
         * @param BETA_3 The regression coefficient @todo unused
         * @param VALLEY_POSITION The valley position of the peak @todo change to VALLEY_POSITION_PEAK @todo unused
         * @param DF The degrees of freedom
         * @param X_INDEX The index of the regression window position @todo rename to REGRESSION_WINDOW_INDEX
         *
         */
        enum class DataField // @todo check if this can be split
        {
            // general fields
            INTENSITY, // signal intensity
            SUB_ID,    // sub-dataset ID
            DATA_ID,   // data ID
            DATAPOINT, // data point

            // MS fields
            IONIZATIONMODE,  // positive or negative @todo unused
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
            DQS_PEAK, // DQS: Data Quality Score \cite{reuschenbach2022development}
            PEAK_AREA_UNCERTAINTY,
            PEAK_HEIGHT_UNCERTAINTY,
            PEAK_WIDTH_UNCERTAINTY,
            PEAK_POSITION_UNCERTAINTY,
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