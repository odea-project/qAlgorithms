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

    struct TreatedData // @todo remove this struct / maybe not?
    {
        std::vector<float> RT;
        std::vector<float> intensity;
        std::vector<size_t> cenIDs;
        std::vector<unsigned int> cumulativeDF; // degrees of freedom
        unsigned int lowestScan, largestScan;
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
        double b0, b1, b2, b3 = 0;
    };

    struct RegressionGauss
    {
        RegCoeffs coeffs;                         // regression coefficients
        int index_x0 = 0;                         // index of window center (x==0) in the Y matrix
        int scale = 0;                            // scale of the regression window, i.e., 2*scale+1 = window size
        int df = 0;                               // degrees of freedom, interpolated data points will not be considered
        float apex_position = 0;                  // position of the apex of the peak
        float mse = 0;                            // @todo mean squared error; this is not always used and should not be a part of the regression struct
        unsigned int left_limit, right_limit = 0; // limits of the peak regression window
        float area = 0;                           // area of the peak
        float uncertainty_area, uncertainty_pos, uncertainty_height = 0;
        int numCompetitors = 0; // number of points that were discarded in favour of this regression
        bool isValid = false;   // flag to indicate if the regression is valid
    };

    struct ProfilePos // gives the range of points covered by a centroid and the access index for streamfind
    {
        unsigned int access;
        unsigned int start, end;
    };

    struct CentroidPeak
    {
        double mz;
        float RT;
        float height;
        float area;
        float width;
        float heightUncertainty, areaUncertainty, mzUncertainty;
        float DQSC;
        // the binning tolerates at most three non-occurrences of a mass in order, but should not include interpolated spectra for this.
        // for conversion, number_MS1 is also the index into a vector that stores the "corrected" scan numbers after interpolation
        unsigned int number_MS1;
        unsigned int scan_interpolate;
        unsigned int df; // degrees of freedom
        ProfilePos trace;
        unsigned int numCompetitors;
        unsigned int scale;
        unsigned int ID;
        // unsigned int interpolations;
    };

    struct EIC // Extracted Ion Chromatogram
    {
        std::vector<unsigned int> scanNumbers;
        std::vector<float> rententionTimes;
        std::vector<float> mz;
        std::vector<float> predInterval;
        std::vector<float> ints_area;
        std::vector<float> ints_height;
        std::vector<unsigned int> df;
        std::vector<float> DQSB;
        std::vector<float> DQSC;
        std::vector<unsigned int> cenID;
        std::vector<size_t> interpolatedIDs;
        std::vector<float> interpolatedDQSB;
        size_t componentID = 0; // this is only set during componentisation
        bool interpolations;    // @todo probably redundant
    };

    struct FeaturePeak
    {
        RegCoeffs coefficients;
        float height;
        float area;
        // float width;
        float heightUncertainty;
        float areaUncertainty;
        float DQSF, DQSB, DQSC;
        float retentionTime;
        float mz;
        float RT_Uncertainty;
        float mzUncertainty;
        unsigned int componentID = 0; // this is only set during execution of qPattern / qComponent / whatever better name i think of. Zero means uninitialised -> components start at 1!
        unsigned int idxBin;
        // these refer to the interpolated EIC!
        unsigned int idxPeakStart, idxPeakEnd, index_x0_offset;
        // relates to abstracted MS1 scan counts, starts at 2 for real points
        unsigned int scanPeakStart, scanPeakEnd;
        // indices into the non-interpolated bin; degrees of freedom = idxBinEnd - idxBinStart + 1
        unsigned int idxBinStart, idxBinEnd;
        // temporary values, @todo remove?
        unsigned int interpolationCount;
        unsigned int competitorCount;
        unsigned int scale;
        float mse_base;
        bool apexLeft;
    };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H