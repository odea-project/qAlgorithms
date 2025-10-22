// qalgorithms_datatypes.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

#include <vector>
#include <cmath> // INFINITY and other number macros
#include <string>

/*  This file includes the structs used for data management in qAlgorithms.
    Anything required by multiple parts of the full program should be listed here.
*/

namespace qAlgorithms
{
    // handle polarity switching
    enum Polarities
    {
        unknown_polarity,
        positive,
        negative,
        mixed,
    };

    struct CompoundFilter
    {
        // this struct is used to limit the amount of operations performed by the program to
        // a selection of masses and RTs that is relevant to the analysis at hand.
        double mz_expected[16] = {0}; // @todo reasonable amount of points?
        double mz_tolerance_ppm = 0;  // @todo is ppm the only relevant choice?

        double RT = -1;
        double RT_tol = -1; // tolerance to either side, assumes symmetrical peaks

        std::string compoundName = ""; // @todo better solution?
        std::string methodName = "";
        Polarities polarity = Polarities::unknown_polarity;
    };

    // inclusive range in an array @todo replace all start / end index cases with this
    // always assert start and end correctness when writing to such a struct!
    // addition: think about replacing this with a start pointer and length, sicne they
    // are generally just used as ranged access pointers into a larger array
    struct Range_i
    {
        size_t startIdx;
        size_t endIdx;
    };
    inline size_t rangeLen(const Range_i *range)
    {
        return range->endIdx - range->startIdx + 1;
    }

    struct slice_d
    {
        double *const start;
        size_t length;
    };

    enum SourceFileType
    {
        unknown_filetype,
        mzML,
        mzxML,
        hdf5,
        json,
    };

    struct ParsedProfileData
    {
        char sourceFilePath[256] = {0}; // @todo while a dynamic solution is possible, it is probably not sensible. Potential fix: only return the last 256 chars of path
        // array keeping offset of start pointers to mz and intensity array - all are allocated to the same arena
        SourceFileType type;
        size_t *accessOffset;
        size_t *arrayLength;
        float *mz;
        float *intensity;

        size_t numSpectra = 0;
    };

    struct ProfileBlock
    {
        std::vector<float> intensity;
        std::vector<float> mz;
        // the degrees of freedom are redundant because there are never interpolated points within a centroid
        // we still use them because this way there is no largely redundant second peak fitting function
        std::vector<unsigned int> cumdf; // cumulative df @todo find a better solution for the general case
        size_t startPos, endPos;
    };

    struct RegCoeffs
    {
        double b0 = 0, b1 = 0, b2 = 0, b3 = 0;
    };

    struct RegressionGauss
    {
        RegCoeffs coeffs = {0};   // regression coefficients
        size_t idxCenter = 0;     // index of window center (x==0) in the Y matrix
        int scale = 0;            // scale of the regression window, i.e., 2*scale+1 = window size
        int df = 0;               // degrees of freedom, interpolated data points will not be considered
        float apex_position = 0;  // position of the apex of the peak
        float mse = 0;            // mean squared error
        Range_i regSpan = {0, 0}; // limits of the peak regression window
        float area = 0;           // area of the peak
        float uncertainty_area = 0, uncertainty_pos = 0, uncertainty_height = 0;
        int numCompetitors = 0; // number of points that were discarded in favour of this regression
        bool isValid = false;   // flag to indicate if the regression is valid
    };

    struct ProfilePos_ // gives the range of points covered by a centroid and the access index for streamfind
    {
        unsigned int access = 0;
        unsigned int start = 0, end = 0; // start and end into the original MS1 spectrum
        int start_rel = 0, end_rel = 0;  // start and end in the abstract dimension where a peak is centered on 0
    };

    struct CentroidPeak
    {
        double mz = 0;
        float RT = 0;
        float height = 0;
        float area = 0;
        float width = 0;
        float heightUncertainty = 0, areaUncertainty = 0, mzUncertainty = 0;
        float DQSC = 0;
        // the binning tolerates at most three non-occurrences of a mass in order, but should not include interpolated spectra for this.
        // for conversion, number_MS1 is also the index into a vector that stores the "corrected" scan numbers after interpolation
        unsigned int number_MS1 = 0;
        unsigned int df = 0; // degrees of freedom
        ProfilePos trace = {0};
        unsigned int numCompetitors = 0;
        unsigned int scale = 0;
        unsigned int ID = 0;
        // unsigned int interpolations;
    };

    struct RT_Grouping
    {
        size_t originalIndex = -1;
        size_t interpolatedIndex = -1;
        float trueRT = -1;
        bool interpolated = true;
    };

    struct RT_Converter
    {
        std::vector<RT_Grouping> groups;
        // index into the groups vector. The "originalIndex" field ind the RT_Grouping struct is the index into this vector
        std::vector<size_t> indexOfOriginalInInterpolated = {0};
    };

    struct EIC // Extracted Ion Chromatogram
    {
        std::vector<unsigned int> scanNumbers = {0};
        std::vector<float> mz = {0};
        std::vector<float> predInterval{0};
        std::vector<float> ints_area{0};
        std::vector<float> ints_height{0};
        std::vector<unsigned int> df{0};
        std::vector<float> DQSB{0};
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
        unsigned int componentID = 0; // this is only set during execution of qPattern / qComponent / whatever better name i think of. Zero means uninitialised -> components start at 1!
        unsigned int idxBin = 0;
        // these refer to the interpolated EIC!
        unsigned int idxPeakStart = 0, idxPeakEnd = 0, idxCenter_offset = 0;
        // relates to abstracted MS1 scan counts, starts at 2 for real points
        unsigned int scanPeakStart = 0, scanPeakEnd = 0;
        // indices into the non-interpolated bin; degrees of freedom = idxBinEnd - idxBinStart + 1
        unsigned int idxBinStart = 0, idxBinEnd = 0;
        // temporary values, @todo remove?
        unsigned int interpolationCount = 0;
        unsigned int competitorCount = 0;
        unsigned int scale = 0;
        float mse_base = 0;
        float lowerRT = 0;
        float upperRT = 0; // @todo set these during feature construction
    };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H