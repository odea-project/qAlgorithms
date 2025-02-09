// qalgorithms_datatypes.h
#ifndef QALGORITHMS_DATATYPE_PEAK_H
#define QALGORITHMS_DATATYPE_PEAK_H

#include <immintrin.h> // AVX
#include <cstddef>     // errorcode in EIC @todo just make this an int
#include <vector>

/* This file includes the structs used for data management in qAlgorithms*/

namespace qAlgorithms
{
    struct dataPoint // @todo remove this (again)
    {
        float x;
        float y;
        bool df;
        float DQSC;
        float DQSB;
        int scanNumber;
        float mz;
    };

    struct treatedData // @todo remove this struct
    {
        std::vector<dataPoint> dataPoints;
        std::vector<size_t> separators;
        // points as individual vectors
        // std::vector<float> RT;
        std::vector<float> intensity;
        // std::vector<bool> df;
        // std::vector<float> DQSC;
        // std::vector<float> DQSB;
        // std::vector<int> scanNumber;
        // std::vector<float> mz;
    };

    struct RegCoeffs
    {
        float b0, b1, b2, b3 = 0;
    };

    struct RegressionGauss
    {
        RegCoeffs newCoeffs;          // regression coefficients
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

    struct CentroidPeak
    {
        double mz;
        float height;
        float area;
        float width;
        float heightUncertainty;
        float areaUncertainty;
        float dqsCen;
        float mzUncertainty;
        unsigned int scanNumber;
        unsigned int df; // degrees of freedom
        unsigned int numCompetitors;
        unsigned int scale;
    };

    struct qCentroid
    {
        double mz;
        float mzError = -1;
        unsigned int scanNo;
        float int_area;   // the intensity is never used during binning @todo
        float int_height; // s.o.
        float DQSCentroid;
        unsigned int df; // degrees of freedom
        unsigned int cenID;
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
    };

    struct FeaturePeak
    {
        RegCoeffs coefficients;
        float height;
        float area;
        float width;
        float heightUncertainty;
        float areaUncertainty;
        float DQSF;          // only relevant for features
        float dqsBin;        // can be calculated when needed
        float dqsCen;        // can be calculated when needed
        float retentionTime; // only relevant for features
        float mz;
        float retentionTimeUncertainty;
        float mzUncertainty;
        unsigned int idxBin;
        unsigned int idxPeakStart; // degrees of freedom = idxPeakEnd - idxPeakStart
        unsigned int idxPeakEnd;
        unsigned int interpolationCount;
        unsigned int competitorCount;
        unsigned int scale;
        bool apexLeft;
    };
}

#endif // QALGORITHMS_DATATYPE_PEAK_H