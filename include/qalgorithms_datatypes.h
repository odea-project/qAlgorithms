#ifndef QALGORITHMS_DATATYPES_H
#define QALGORITHMS_DATATYPES_H

#include <cstdint>
#include <vector>

// This file includes the structs used for data management in qAlgorithms.
// All data structures required by multiple parts of the full program should be listed here.
// Further, all OS-dependent conditionals are handled here if possible

#ifdef _WIN32
typedef wchar_t path_char;
    #define _STR "%ls" // NOLINT
#else
typedef char path_char;
    #define _STR "%s" // NOLINT
#endif

namespace qAlgorithms
{
    // handle polarity switching
    enum Polarities : uint8_t
    {
        unknown_polarity,
        positive,
        negative,
        mixed,
    };

    struct RegCoeffs
    {
        double b0 = 0, b1 = 0, b2 = 0, b3 = 0;
        uint16_t scale = 0, x0 = 0;
    };

    struct Range_i
    {
        size_t startIdx;
        size_t endIdx;
        size_t length;
    };

    struct Span_i32
    {
        int32_t startIdx = 0;
        int32_t length = 0;
        int32_t endIdx() const
        {
            return startIdx + length - 1;
        }
        void set_endIdx(const int32_t idx)
        {
            length = idx - startIdx + 1;
        }
    };

    struct RegressionGauss
    {
        RegCoeffs coeffs = {0};
        Range_i regSpan = {0, 0, 0};
        Span_i32 span = {0, 0};
        float area = 0, position = 0, height = 0;
        float area_unc = 0, position_unc = 0, height_unc = 0;
        float jaccard = 0;
        float fwhm = 0;
        float dqs = 0;
        uint16_t startIdx = 0;
        uint16_t length = 0;
        uint16_t df = 0;            // degrees of freedom, interpolated data points will not be considered
        uint8_t numCompetitors = 0; // number of points that were discarded in favour of this regression
        int8_t apexGroup = -1;
        bool isValid = false; // flag to indicate if the regression is valid
    };

    struct CentroidPeak
    {
        float mz = 0;
        float height = 0;
        float area = 0;
        float width = 0;
        float heightUncertainty = 0, areaUncertainty = 0, mzUncertainty = 0;
        float DQSC = 0;
        uint32_t ID = 0;
        // the binning tolerates at most three non-occurrences of a mass in order, but should not include interpolated spectra for this.
        // for conversion, number_MS1 is also the index into a vector that stores the "corrected" scan numbers after interpolation
        uint16_t number_MS1 = 0;
        // uint32_t df = 0; // degrees of freedom
        uint16_t scale = 0;
        uint8_t numCompetitors = 0;
        // uint8_t interpolations;
    };

    struct EIC // Extracted Ion Chromatogram @todo find a better representation that does not involve this many allocations
    {
        std::vector<uint32_t> scanNumbers = {0};
        std::vector<float> mz = {0};
        std::vector<float> predInterval{0};
        std::vector<float> ints_area{0};
        std::vector<float> ints_height{0};
        std::vector<uint16_t> df{0}; // this is required for dealing with interpolations, but should be moved into qPeaks eventually @todo
        // std::vector<float> DQSB{0};
        std::vector<float> DQSC{0};
        std::vector<uint32_t> cenID{0};
        std::vector<uint32_t> interpolatedScans{0};
        std::vector<float> RT{0};
        // std::vector<float> interpolatedDQSB;
        uint32_t componentID = 0; // this is only set during componentisation
        // bool interpolations;          // are there interpolated values?
    };

    struct FeaturePeak
    {
        RegCoeffs coefficients{0};
        float height = 0;
        float area = 0;
        // float width;
        float heightUncertainty = 0;
        float areaUncertainty = 0;
        float DQSF = 0, DQSB = 0, DQSC = 0;
        float retentionTime = 0;
        float mz = 0;
        float RT_Uncertainty = 0;
        float mzUncertainty = 0;
        uint32_t eic_ID = 0;
        // these refer to the EIC
        uint32_t idxPeakStart = 0;
        uint32_t length = 0;
        float lowerRT = 0;
        float upperRT = 0; // @todo set these during feature construction
    };
} // namespace qAlgorithms

#endif // QALGORITHMS_DATATYPES_H