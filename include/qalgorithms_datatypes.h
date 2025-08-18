// qalgorithms_datatypes.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

#include <vector>

/* This file includes the structs used for data management in qAlgorithms*/

namespace qAlgorithms
{
    // handle polarity switching
    enum Polarities
    {
        positive,
        negative,
        mixed,
        unknown
    };

    struct ProfileBlock
    {
        std::vector<float> intensity;
        std::vector<float> mz;
        // the degrees of freedom are redundant because there are never interpolated points within a centroid
        // we still use them because this way there is no largely redundant second peak fitting function
        std::vector<unsigned int> cumdf; // cumulative df @todo find a better solution for the general case
        unsigned int startPos, endPos;
    };

    struct RegCoeffs
    {
        double b0 = 0, b1 = 0, b2 = 0, b3 = 0;
    };

    struct RegressionGauss
    {
        RegCoeffs coeffs = {0};                   // regression coefficients
        int index_x0 = 0;                         // index of window center (x==0) in the Y matrix
        int scale = 0;                            // scale of the regression window, i.e., 2*scale+1 = window size
        int df = 0;                               // degrees of freedom, interpolated data points will not be considered
        float apex_position = 0;                  // position of the apex of the peak
        float mse = 0;                            // @todo mean squared error; this is not always used and should not be a part of the regression struct
        unsigned int left_limit, right_limit = 0; // limits of the peak regression window
        float area = 0;                           // area of the peak
        float uncertainty_area = 0, uncertainty_pos = 0, uncertainty_height = 0;
        int numCompetitors = 0; // number of points that were discarded in favour of this regression
        bool isValid = false;   // flag to indicate if the regression is valid
    };

    struct ProfilePos // gives the range of points covered by a centroid and the access index for streamfind
    {
        unsigned int access = 0;
        unsigned int start = 0, end = 0;
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
        unsigned int idxPeakStart = 0, idxPeakEnd = 0, index_x0_offset = 0;
        // relates to abstracted MS1 scan counts, starts at 2 for real points
        unsigned int scanPeakStart = 0, scanPeakEnd = 0;
        // indices into the non-interpolated bin; degrees of freedom = idxBinEnd - idxBinStart + 1
        unsigned int idxBinStart = 0, idxBinEnd = 0;
        // temporary values, @todo remove?
        unsigned int interpolationCount = 0;
        unsigned int competitorCount = 0;
        unsigned int scale = 0;
        float mse_base = 0;
        bool apexLeft = false;
    };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H