#ifndef _QALGORITHMS_QBIN_INCLUDED
#define _QALGORITHMS_QBIN_INCLUDED

#include <deque>
#include <vector>
#include <string>

// Goal: all functions modify individual features, which are combined to a bin. The bin contains all features and the functions necessary to summarise which features are present

namespace q
{
    // Datapoint Struct (contains all user-specified variables found in raw data and an index)
    struct Datapoint
    {
        // int idx; // keep as test parameter if at all
        double mz;
        double mzError;
        double RT;
        int scanNo;
        int pt_binID;
    };

    class RawData
    {
    public:
        RawData(int in_numberOfScans);
        ~RawData();
        int numberOfScans;
        int lengthAllFeatures;
        std::vector<std::vector<Datapoint>> allDatapoints;
        bool readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int pt_d_binID);
    };

    // Bin Class
    class Bin
    {
    private:
        std::vector<double> cumError; // cumulative error in mz
        double pt_MakeDQSB;
        int pt_subsetcount;

    public:
        double pt_mzmin;
        double pt_mzmax;
        int pt_scanmin;
        int pt_scanmax;
        std::vector<Datapoint *> pointsInBin;
        std::vector<double> activeOS; // Order Space
        std::vector<double> DQSB;
        Bin(const std::vector<Datapoint *>::iterator &startBin, const std::vector<Datapoint *>::iterator &endBin); // const std::vector<Datapoint> &sourceList,
        Bin(RawData *rawdata);
        ~Bin();
        void makeOS();
        void makeCumError();
        void subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, int startBin, int endBin); // mz, error, RT and beginning/end are dictated by bin contents
        void subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int &maxdist);
        void makeDQSB(const RawData *rawdata, const int &maxdist);
        double findOuterMinmax(std::vector<Datapoint *>::const_iterator position, std::vector<Datapoint *>::const_iterator scanend, const double &innerMinmax, bool direction, int scansize);
        std::string summarisePerf();
        void controlBin(int binID);
    };

    // BinContainer
    class BinContainer
    {
    private:
        // void readcsv(std::string user_file, std::vector<Datapoint> output, int d_mz, int d_mzError, int d_RT, int d_scanNo); // implemented for featurelist
        std::deque<Bin> binDeque;      // ßßß add case for no viable bins
        std::vector<Bin> finishedBins; // only includes bins which cannot be further subdivided, added DQSB

    public:
        BinContainer();
        ~BinContainer();
        void makeFirstBin(RawData *rawdata);
        void subsetBins(std::vector<int> dimensions, int scanDiffLimit); // select which of the hard-coded subsetting tools should be used in which order, always applies to binDeque. Append to the end, delete from the front
        void printAllBins(std::string path, RawData *rawdata);
        void printBinSummary(std::string path);
        void firstBinValid();                                 // move first bin in binDeque to end
        void clearFirstBin();                                 // remove first bin in binDeque
        void assignDQSB(const RawData *rawdata, int maxdist); // apply DQSB function to all completed bins
        void controlAllBins();
    };
    // utility functions
    std::vector<double> meanDistance(std::vector<Datapoint *> pointsInBin);
    double calcDQS(double MID, double MOD); // Mean Inner Distance, Minimum Outer Distance

}

#endif