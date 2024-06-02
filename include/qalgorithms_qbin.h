#ifndef _QALGORITHMS_QBIN_INCLUDED
#define _QALGORITHMS_QBIN_INCLUDED

#include <deque>
#include <vector>
#include <string>

// Goal: all functions modify individual features, which are combined to a bin. The bin contains all features and the functions necessary to summarise which features are present

namespace q
{
    // Datapoint Struct (contains all user-specified variables found in source file); Output of qCentroiding
    struct Datapoint
    {
        double mz;
        double mzError = -1;
        double RT;
        unsigned int scanNo;
        double intensity;
        double DQScentroid;
        double control_DQSbin;
    };

    class RawData //@todo rename to centroided data?
    {
    public:
        RawData(); // @todo add handling for reading in data without centroid error
        ~RawData();
        int lengthAllPoints;
        std::vector<std::vector<Datapoint>> allDatapoints;
        // "mz" "mzError" "RT" "scanNo" "intensity" "controlID" "controlCentroidDQS" "controlBinDQS"
        bool readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int d_intensity, int d_DQScentroid, int d_control_DQSbin);
    };

    // return object of qbinning

    struct DatapointEIC
    {
        double mz;
        double rt;
        unsigned int scan;
        double intensity; // convert to int here?
        double DQS;
    };

    struct EIC // Extracted Ion Chromatogram
    {
        std::vector<DatapointEIC> pointsInEIC;
        double meanDQS;
        double meanMZ;
        unsigned int medianScans;
        double maxInt;
    };

    // Bin Class
    class Bin
    {
    private:
        std::vector<double> cumError; // cumulative error in mz
        double pt_MakeDQSB;
        bool duplicateScan = false; // are two points with the same scan number in this bin?
        unsigned int medianScan;
        double medianMZ;

    public:
        double mzmin;
        double mzmax;
        int scanmin;
        int scanmax;
        std::vector<Datapoint *> pointsInBin; // @todo does not change after bin creation, check for performance improvement when using cast const
        std::vector<double> activeOS;         // Order Space
        std::vector<double> DQSB_base;        // DQSB when all distances are considered equal
        std::vector<double> DQSB_scaled;      // DQSB if a gaussian falloff is assumed

        /// @brief generate a bin that is a subset of an existing bin using two iterators in one existing bin
        /// @param startBin left border of the new bin
        /// @param endBin right border of the new bin
        Bin(const std::vector<Datapoint *>::iterator &startBin, const std::vector<Datapoint *>::iterator &endBin); // @todo pass by reference correct?

        /// @brief generate a bin by adding pointers to all points in rawdata to pointsInBin
        /// @param rawdata a set of raw data on which binning is to be performed
        Bin(RawData *rawdata);
        ~Bin();

        /// @brief generate the activeOS vector containing the difference to the next greatest mz for every point
        void makeOS();

        /// @brief generate the cumError vector for this bin
        void makeCumError();
        void makeCumError(const double ppm); // if no centroid error exists

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
        /// @param counter counter shared between all calls of the recursive function, used to count number of function calls
        void subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, const int startBin, const int endBin, unsigned int &counter);

        /// @brief divide a bin sorted by scans if there are gaps greater than maxdist in it. Bins that cannot be divided are closed.
        /// @details this function sorts all members of a bin by scans and iterates over them. If a gap greater than maxdist exists,
        /// the bin is split at this point. Only subsets with more than five elements are added to the bin deque. If a bin is not split,
        /// it is added to the finishedBins vector and no further subsets will be performed on it. As such, subsetScan() must be the last
        /// subset function and cannot be used in combination with any other subsetting function that decides if a bin is completed or not.
        /// @param bincontainer if the input bin was split, the newly created bins will be added to this
        /// @param finishedBins if the input bin was not split, it will be added to this
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        /// @param counter counter shared between all calls of the recursive function, used to count number of function calls
        void subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int maxdist, unsigned int &counter);

        /// @brief generate the data quality score for all data points in a bin
        /// @details for every point in the bin the mean distance in mz to other elements of the bin and the shortest distance to an
        /// element not in the bin is calculated. The outer distance may not be to a point more than maxdist scans away from the
        /// binned datapoint. It is assumed that the bin is sorted by scans when makeDQSB is called @todo change to more generic solution?
        /// @param rawdata the RawData object from which the bin was generated
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void makeDQSB(const RawData *rawdata, const unsigned int maxdist);

        /// @brief summarise the bin to one line, with the parameters size, mean_mz, median_mz, stdev_mz, mean_scans, median_scans,
        /// DQSB_base, DQSB_control, DQSB_worst, min_DQSC, meanError. DQSB_worst is calculated by assuming the MOD of all points in the
        /// bin to be equal to the critical value. Additionally, the function computes a 1-byte code for up to 8 states of interest.
        /// The respective bit is set to 1 if the defined state is present. Possible states of interest are too large a discrepancy
        /// between mean and median or the presence of duplicate values.
        /// @return A tuple containing the summary information. The first entry is the error code.
        std::tuple<std::byte, size_t, double, double, double, double, unsigned int, double, double, double, double, double, double> summariseBin();

        const EIC createEIC();

        // void controlMedianMZ();

        const double calcDQSmin();
    };

    // BinContainer
    class BinContainer
    {
    private:
        // void readcsv(std::string user_file, std::vector<Datapoint> output, int d_mz, int d_mzError, int d_RT, int d_scanNo); // implemented for featurelist
        std::deque<Bin> binDeque;      // TODO add case for no viable bins
        std::vector<Bin> finishedBins; // only includes bins which cannot be further subdivided, added DQSB_base
        unsigned int subsetCount;

    public:
        BinContainer();
        ~BinContainer();

        /// @brief create a bin out of rawdata and add it to the binDeque. This function exists since all subsetting functions require a bin to start
        /// @param rawdata a set of raw data on which binning is to be performed
        void makeFirstBin(RawData *rawdata); // @todo find way for rawdata to be const

        /// @brief perform the specified subsetting operations on the binDeque. Elements are added to the back and deleted from the front
        /// @param dimensions which dimensions should be used for subsetting in what order. 1 = subsetting by mz, 2 = subsetting by scans.
        /// Important: The last element of dimensions must determine bins to be finished, and no other subsetter may add to finsihedBins.
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void subsetBins(const std::vector<int> dimensions, const unsigned int maxdist, const double massError);

        /// @brief apply DQSB_base function to all completed bins
        /// @param rawdata the RawData object from which all bins in finishedBins were generated
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void assignDQSB(const RawData *rawdata, const unsigned int maxdist);

        void printAllBins(std::string path, const RawData *rawdata); // @todo remove rawdata dependency

        void printBinSummary(std::string path); // @todo create a better summary function which gives a
                                                // detailed report, including what the test actually does,
                                                // the mz subset after which a bin was complete, statistics
                                                // regarding the rest of the bins in terms of size, count etc.
                                                // Make sure to test the report for human-readability and
                                                // include metadata from the mzml where possible.
                                                // Some measure of overall spectrum quality should also be given.

        void printBinningReport(std::string path); // <- here

        const std::vector<EIC> returnBins();

        // void printTstats();

        // void printworstDQS();

        /// @brief The function calculates the summary of a bin and selects bins with at least one state of interest.
        /// @param mask every bit of this byte controls if the corresponding test is considered for returning the
        /// selection. For example, for returning only bins in which the first test condition applies,
        /// the input would have to be std::byte{0b00000001}
        /// @return A vector of all closed bins which have at least one state of interest
        const std::vector<int> makeBinSelection(std::byte mask);

        /// @brief Print the individual bin members and optionally the bin summary in a separate .csv
        /// @param indices A vector of the selected bins, probably makeBinSelection()
        /// @param summary Should a summary table be printed
        /// @param location path to the folder in which both files should be created
        void printSelectBins(const std::vector<int> indices, bool summary, const std::string location);
    };

    // utility functions

    /// @brief calculate the mean distance in mz to all other elements of a sorted vector for one element
    /// @param pointsInBin vector of data points sorted by mz
    /// @return vector of the mean inner distances for all elements in the same order as pointsInBin
    std::vector<double> meanDistance(const std::vector<Datapoint *> pointsInBin);

    /// @brief calculate the data quality score as described by Reuschenbach et al. for one datapoint in a bin
    /// @param MID mean inner distance in mz to all other elements in the bin
    /// @param MOD minimum outer distance - the shortest distance in mz to a data point that is within maxdist and not in the bin
    /// @return the data quality score for the specified element
    double calcDQS(const double MID, const double MOD); // Mean Inner Distance, Minimum Outer Distance

    void check_MOD_outOfBins(const Bin *target, const std::vector<q::Datapoint *> notBinned, const int maxdist);

    void scaleDistancesForDQS_gauss(int maxdist); // @experimental

}

#endif