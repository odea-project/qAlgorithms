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

    // Bin Class
    class Bin
    {
    private:
        std::vector<Feature> featurelist;
        std::vector<double> cumError; // cumulative error in mz
        double prevmax;

    public:
        std::vector<double> activeOS;                                                                                         // Order Space
        Bin(const std::vector<Feature>::iterator &startBin, const std::vector<Feature>::iterator &endBin, const double &max); // const std::vector<Feature> &sourceList,
        ~Bin();
        void makeOS();
        void makeCumError();
        void subsetMZ(std::vector<Bin> bincontainer, const std::vector<double> &OS, int startBin, int endBin); // mz, error, RT and beginning/end are dictated by bin contents
        void subsetRT(const Bin &target, const int &maxdist);
        // Feature makeFeature(); // combine all features to one using means, modify mzError
    };

    // BinContainer
    class BinContainer
    {
    private:
        void readcsv(std::string user_file, std::vector<Feature> output, int d_mz, int d_mzError, int d_RT, int d_scanNo);
        std::vector<Bin> binVector;

    public:
        BinContainer();
        ~BinContainer();
        void makeFirstBin(std::string input_file);    // calls readcsv()
        void subsetBins(std::vector<int> dimensions); // select which of the hard-coded subsetting tools should be used in which order, always applies to binVector. Append to the end, delete from the front
        void printAllBins(std::string path);
        void printBinSummary(std::string path);
    };

}