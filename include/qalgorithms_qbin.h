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
        double intensity = 1;
    };

    class RawData //@todo rename to centroided data?
    {
    public:
        RawData(int in_numberOfScans); // @todo add handling for reading in data without centroid error
        ~RawData();
        int numberOfScans;
        int lengthAllFeatures;
        std::vector<std::vector<Datapoint>> allDatapoints;
        bool readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int pt_d_binID);
        bool readtxt(std::string user_file);
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

        /// @brief generate a bin that is a subset of an existing bin
        /// @param startBin left border of the new bin
        /// @param endBin right border of the new bin
        Bin(const std::vector<Datapoint *>::iterator &startBin, const std::vector<Datapoint *>::iterator &endBin);

        /// @brief generate a bin by adding pointers to all points in rawdata to pointsInBin
        /// @param rawdata a set of raw data on which binning is to be performed
        Bin(RawData *rawdata);
        ~Bin();

        /// @brief generate the activeOS vector containing the difference to the next greatest mz for every point
        void makeOS();

        /// @brief generate the cumError vector for this bin
        void makeCumError();

        /// @brief divide a bin sorted by mz the difference in mz of its members and return the new bins to the bin deque. Recursive function.
        /// @details this function iterates over the order space of a previously generated bin by searching for the maximum
        /// of the order space between startBin and endBin. If the maximum is smaller than the critical value, all data points
        /// between startBin and endBin are added to bincontainer as a new bin. The function terminates if it is called on less
        /// than five datapoints. For details on the critical value, see: "qBinning: Data Quality-Based Algorithm for Automized Ion Chromatogram
        /// Extraction from High-Resolution Mass Spectrometry. Max Reuschenbach, Felix Drees, Torsten C. Schmidt, and Gerrit Renner. Analytical
        /// Chemistry 2023 95 (37), 13804-13812. DOI: 10.1021/acs.analchem.3c01079"
        /// https://pubs.acs.org/doi/suppl/10.1021/acs.analchem.3c01079/suppl_file/ac3c01079_si_001.pdf page 26
        /// Assuming an order space [1,1,1,1,1,1,1,5,1,1,1] and a critical value of 4 for n = 11 (this does not reflect the actual critical value),
        /// the function would add a bin containing the datapoints 0 to 5 in the source bin to the bin deque
        /// @param bincontainer the newly created bins will be added to the back of this deque
        /// @param OS the order space generated for the bin using makeOS()
        /// @param startBin index relating to the order space at which the bin starts
        /// @param endBin index relating to the order space at which the bin ends
        void subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, int startBin, int endBin);

        /// @brief divide a bin sorted by scans if there are gaps greater than maxdist in it. Bins that cannot be divided are closed.
        /// @details this function sorts all members of a bin by scans and iterates over them. If a gap greater than maxdist exists,
        /// the bin is split at this point. Only subsets with more than five elements are added to the bin deque. If a bin is not split,
        /// it is added to the finishedBins vector and no further subsets will be performed on it. As such, subsetScan() must be the last
        /// subset function and cannot be used in combination with any other subsetting function that decides if a bin is completed or not.
        /// @param bincontainer if the input bin was split, the newly created bins will be added to this
        /// @param finishedBins if the input bin was not split, it will be added to this
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int &maxdist);

        /// @brief generate the data quality score for all data points in a bin
        /// @details for every point in the bin the mean distance in mz to other elements of the bin and the shortest distance to an
        /// element not in the bin is calculated. The outer distance may not be to a point more than maxdist scans away from the
        /// binned datapoint.
        /// @param rawdata the RawData object from which the bin was generated
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void makeDQSB(const RawData *rawdata, const int &maxdist);

        std::string summarisePerf();
        void controlBin(int binID);
    };

    // BinContainer
    class BinContainer
    {
    private:
        // void readcsv(std::string user_file, std::vector<Datapoint> output, int d_mz, int d_mzError, int d_RT, int d_scanNo); // implemented for featurelist
        std::deque<Bin> binDeque;      // TODO add case for no viable bins
        std::vector<Bin> finishedBins; // only includes bins which cannot be further subdivided, added DQSB

    public:
        BinContainer();
        ~BinContainer();

        /// @brief create a bin out of rawdata and add it to the binDeque. This function exists since all subsetting functions require a bin to start
        /// @param rawdata a set of raw data on which binning is to be performed
        void makeFirstBin(RawData *rawdata);

        /// @brief perform the specified subsetting operations on the binDeque. Elements are added to the back and deleted from the front
        /// @param dimensions which dimensions should be used for subsetting in what order. 1 = subsetting by mz, 2 = subsetting by scans.
        /// Important: The last element of dimensions must determine bins to be finished, and no other subsetter may add to finsihedBins.
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void subsetBins(std::vector<int> dimensions, int maxdist);

        /// @brief apply DQSB function to all completed bins
        /// @param rawdata the RawData object from which all bins in finishedBins were generated
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void assignDQSB(const RawData *rawdata, int maxdist);

        void printAllBins(std::string path, RawData *rawdata);
        void printBinSummary(std::string path);

        void controlAllBins();

        // @todo add wrapper function which has bins independent from RawData as return value
    };

    // extracted ion chromatogram
    struct EIC
    {
        std::vector<Datapoint> pointsInEIC;
        std::vector<double> DQSB;
        // mean DQS?
        // size?
        // mean mz?
        // RT range?
    };

    // utility functions

    /// @brief calculate the mean distance in mz to all other elements of a sorted vector for one element
    /// @param pointsInBin vector of data points sorted by mz
    /// @return vector of the mean inner distances for all elements in the same order as pointsInBin
    std::vector<double> meanDistance(std::vector<Datapoint *> pointsInBin);

    /// @brief calculate the data quality score as described by Reuschenbach et al. for one datapoint in a bin
    /// @param MID mean inner distance in mz to all other elements in the bin
    /// @param MOD minimum outer distance - the shortest distance in mz to a data point that is within maxdist and not in the bin
    /// @return the data quality score for the specified element
    double calcDQS(double MID, double MOD); // Mean Inner Distance, Minimum Outer Distance

}

#endif