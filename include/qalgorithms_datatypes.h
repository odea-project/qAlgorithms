// qalgorithms_datatypes.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

#include <vector>

/* This file includes the structs used for data management in qAlgorithms*/

namespace qAlgorithms
{
    struct dataPoint // @todo remove this (again)
    {
        float x;
        float y;
        bool df;
    };

    struct separator
    {
        size_t start;
        size_t end;
    };

    struct treatedData // @todo remove this struct / maybe not?
    {
        std::vector<float> RT;
        std::vector<float> intensity;
        std::vector<size_t> cenIDs;
        std::vector<unsigned int> cumulativeDF; // degrees of freedom
        unsigned int lowestScan;
        unsigned int largestScan;
    };

    struct ProfileBlock
    {
        std::vector<float> intensity;
        std::vector<float> mz;
        // the degrees of freedom are redundant because there are never interpolated points within a centroid
        // we still use them because this way there is no largely redundant second peak fitting function
        std::vector<unsigned int> cumdf; // cumulative df
        // std::vector<bool> df;
        unsigned int startPos;
        unsigned int endPos;
    };

    struct RegCoeffs
    {
        float b0, b1, b2, b3, Sy;
    };

    struct RegressionGauss
    {
        RegCoeffs coeffs;             // regression coefficients
        int index_x0 = 0;             // index of window center (x==0) in the Y matrix
        int scale = 0;                // scale of the regression window, i.e., 2*scale+1 = window size
        int df = 0;                   // degree of freedom, interpolated data points will not be considered
        float apex_position = 0;      // position of the apex of the peak
        float mse = 0;                // mean squared error; this is not always used and should not be a part of the regression struct @todo
        unsigned int left_limit = 0;  // left limit of the peak regression window
        unsigned int right_limit = 0; // right limit of the peak regression window
        float area = 0;               // area of the peak
        float uncertainty_area = 0;   // uncertainty of the area
        float uncertainty_pos = 0;    // uncertainty of the position
        float uncertainty_height = 0;
        int numCompetitors = 0; // number of points that were discarded in favour of this regression
        bool isValid = false;   // flag to indicate if the regression is valid
    };

    struct ProfilePos // gives the range of points covered by a centroid and the access index for streamfind
    {
        unsigned int access;
        unsigned int start;
        unsigned int end;
    };

    struct CentroidPeak
    {
        double mz;
        float RT;
        float height;
        float area;
        float width;
        float heightUncertainty;
        float areaUncertainty;
        float DQSC;
        float mzUncertainty;
        unsigned int scanNumber;
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
        float DQSF;
        float DQSB;
        float DQSC;
        float retentionTime;
        float mz;
        float retentionTimeUncertainty;
        float mzUncertainty;
        unsigned int componentID = 0; // this is only set during execution of qPattern / qComponent / whatever better name i think of. Zero means uninitialised -> components start at 1!
        unsigned int idxBin;
        // these refer to the interpolated EIC!
        unsigned int idxPeakStart;
        unsigned int idxPeakEnd;
        unsigned int index_x0_offset;
        // relates to abstracted MS1 scan counts, starts at 2 for real points
        unsigned int scanPeakStart;
        unsigned int scanPeakEnd;
        // indices into the non-interpolated bin; degrees of freedom = idxBinEnd - idxBinStart + 1
        unsigned int idxBinStart;
        unsigned int idxBinEnd;
        // temporary values, @todo remove?
        unsigned int interpolationCount;
        unsigned int competitorCount;
        unsigned int scale;
        float mse_base;
        bool apexLeft;
    };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H