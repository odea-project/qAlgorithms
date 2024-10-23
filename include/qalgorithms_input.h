#include <filesystem>
#include <vector>

namespace q {
    class MeasurementSeries {
        public:
        enum MeasurementType{
            sample,
            spikedSample,
            standards,
            blank,
            instrumentBlank
        };
        struct Measurement
        {
            std::filesystem::path filepath;
            MeasurementType type;

            int msLevel;
            
            int position; // allows sorting by concentration etc.
            // sample name (only refers to unknown component)
            // replicate number
            // date and time of measurement
            // filehash to ensure uniqueness
            // chromatography
            // mass detector
        };
        

    };
}