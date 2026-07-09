#ifndef QALGORITHMS_DATATYPES_H
#define QALGORITHMS_DATATYPES_H

#include <cstdint>
#include <qalgorithms_utils.h>
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

    struct RegressionGauss
    {
        RegCoeffs coeffs = {0};      // regression coefficients
        Range_i regSpan = {0, 0, 0}; // limits of the peak regression window
        float apex_position = 0;     // position of the apex of the peak
        float area = 0;              // area of the peak (in evenly spaced x dimension, scaled later)
        float uncert_area = 0, uncert_position = 0, uncert_height = 0;
        float jaccard = 0;
        uint16_t startIdx = 0;
        uint16_t length = 0;
        uint16_t df = 0;             // degrees of freedom, interpolated data points will not be considered
        uint16_t numCompetitors = 0; // number of points that were discarded in favour of this regression
        bool isValid = false;        // flag to indicate if the regression is valid
    };

    // The distinction between centroid and feature is not really sensible as a core part of the project
    struct PeakFit
    {
        RegCoeffs coeffs = {0};
        size_t startIdx = 0;
        size_t length = 0;
        float position = 0;
        float uncert_position = 0;
        float limit_L = 0;
        float limit_R = 0;
        float height = 0;
        float uncert_height = 0;
        float fwhm = 0;
        float area = 0;
        float uncert_area = 0;
        float dqs = 0;
        float jaccard = 0;
    };

    struct ProfilePos // gives the range of points covered by a centroid and the access index for streamfind
    {
        unsigned int ID_spectrum = 0;
        unsigned int start = 0, end = 0; // start and end into the original MS1 spectrum
        int start_rel = 0, end_rel = 0;  // start and end in the abstract dimension where a peak is centered on 0
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
        // unsigned int df = 0; // degrees of freedom
        // ProfilePos trace = {0};
        uint16_t scale = 0;
        uint8_t numCompetitors = 0;
        // uint8_t interpolations;
    };

    struct EIC // Extracted Ion Chromatogram @todo find a better representation that does not involve this many allocations
    {
        std::vector<unsigned int> scanNumbers = {0};
        std::vector<float> mz = {0};
        std::vector<float> predInterval{0};
        std::vector<float> ints_area{0};
        std::vector<float> ints_height{0};
        std::vector<uint16_t> df{0}; // this is required for dealing with interpolations, but should be moved into qPeaks eventually @todo
        // std::vector<float> DQSB{0};
        std::vector<float> DQSC{0};
        std::vector<unsigned int> cenID{0};
        std::vector<unsigned int> interpolatedScans{0};
        std::vector<float> RT{0};
        // std::vector<float> interpolatedDQSB;
        unsigned int componentID = 0; // this is only set during componentisation
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
        unsigned int eic_ID = 0;
        // these refer to the EIC
        unsigned int idxPeakStart = 0;
        unsigned int length = 0;
        float lowerRT = 0;
        float upperRT = 0; // @todo set these during feature construction
    };
} // namespace qAlgorithms

#endif // QALGORITHMS_DATATYPES_H