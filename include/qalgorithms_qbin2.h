#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <thread>
#include <ranges>
#include <deque>

// Goal: all functions modify individual features, which are combined to a bin. The bin contains all features and the functions necessary to summarise which features are present

namespace q
{
    // Feature Struct (contains all user-specified variables found in raw data and an index)
    struct Feature // ßßß kann über Feature f{mz, mzError, RT, scanNo} initialisiert werden
    {
        // int idx; // keep as test parameter if at all
        double mz;
        double mzError;
        double RT;
        int scanNo;
    };

    class FeatureList
    {
    public:
        FeatureList();
        ~FeatureList();
        std::vector<Feature> allFeatures;
        void readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo);
    };

    // Bin Class
    class Bin
    {
    private:
        std::vector<Feature> featurelist;
        std::vector<double> cumError; // cumulative error in mz

    public:
        std::vector<double> activeOS;                                                                                         // Order Space
        Bin(const std::vector<Feature>::iterator &startBin, const std::vector<Feature>::iterator &endBin, const double &max); // const std::vector<Feature> &sourceList,
        Bin(FeatureList rawdata);
        ~Bin();
        void makeOS();
        void makeCumError();
        void subsetMZ(std::deque<Bin> bincontainer, const std::vector<double> &OS, int startBin, int endBin); // mz, error, RT and beginning/end are dictated by bin contents
        void subsetRT(const Bin &target, const int &maxdist);
        // Feature makeFeature(); // combine all features to one using means, modify mzError
    };

    // BinContainer
    class BinContainer
    {
    private:
        // void readcsv(std::string user_file, std::vector<Feature> output, int d_mz, int d_mzError, int d_RT, int d_scanNo); // implemented for featurelist
        std::deque<Bin> binDeque; //ßßß add case for no viable bins

    public:
        BinContainer();
        ~BinContainer();
        void makeFirstBin(FeatureList rawdata);       // calls readcsv()
        void subsetBins(std::vector<int> dimensions); // select which of the hard-coded subsetting tools should be used in which order, always applies to binDeque. Append to the end, delete from the front
        void printAllBins(std::string path);
        void printBinSummary(std::string path);
        void firstBinValid(); // move first bin in binDeque to end
        void clearFirstBin(); // remove first bin in binDeque
        // implement way to check for good bin being as long as OS -> no change, move bin to back
    };

}