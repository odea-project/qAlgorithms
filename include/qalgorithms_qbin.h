#ifndef _QALGORITHMS_QBIN_INCLUDED
#define _QALGORITHMS_QBIN_INCLUDED

#include <deque>
#include <vector>
#include <string>

// Goal: all functions modify individual features, which are combined to a bin.
// The bin contains all features and the functions necessary to summarise which features are present
namespace q
{
    namespace Algorithms
    {
        namespace qBinning
        {
#pragma region "utility"
            // qCentroid Struct (contains all user-specified variables found in source file)
            // Output of qCentroiding @todo move to utils / shared data file
            struct qCentroid
            {
                double mz;
                double mzError = -1;
                double RT;
                int scanNo;
                double intensity;
                double DQScentroid;
            };

            struct CentroidedData
            {
                int lengthAllPoints; // number of centroids in all scans
                std::vector<std::vector<qCentroid>> allDatapoints;
            };

            bool readcsv(CentroidedData *rawData, std::string user_file, int d_mz, int d_mzError, int d_scanNo, int d_intensity, int d_DQScentroid);

            // return object of qbinning
            struct DatapointEIC
            {
                double mz;
                // double rt;
                int scan;
                double intensity; // convert to int here?
                double DQS;
            };

            struct EIC_old // Extracted Ion Chromatogram
            {
                std::vector<DatapointEIC> pointsInEIC;
                double meanDQS;
                double meanMZ;
                double maxInt;
            };

            struct EIC // Extracted Ion Chromatogram
            {
                std::byte errorcode;
                std::vector<int> scanNumbers;
                std::vector<double> rententionTimes;
                std::vector<double> mz;
                std::vector<double> intensities;
                std::vector<double> DQSB;
                std::vector<double> DQSC;

                // only include basic summary params if useful
                // double meanDQS;
                // double meanMZ;
                // double maxInt;
            };

            struct SummaryOutput
            {
                std::byte errorcode;
                size_t binsize;
                double mean_mz;
                double median_mz;
                double stddev_mz;
                double mean_scans;
                double DQSB_base;
                double DQSB_scaled;
                double DQSC_min;
                double mean_error;
            };

            struct RunAssessor // one of these per analysed measurement
            {
                std::vector<std::byte> errorcodes;
                std::vector<int> countOfBins;
                std::vector<int> countOfPoints;
                std::vector<double> meanDQSB;
                int totalPoints;
                int unbinnedPoints;
                int countDQSbelow0;
            };

            struct BinBorders
            {
                int binPosition;
                int scanRangeStart;
                int scanRangeEnd;
                double massRangeStart;
                double massRangeEnd;
            };

#pragma endregion "utility"

            // Bin Class @todo get under size 128
            class Bin
            {
            private:
                std::vector<double> cumError; // cumulative error in mz
                double medianMZ;

            public:
                // @todo get mz and scan min/max at the earliest opportunity
                double mzMin;
                double mzMax;
                int scanMin;
                int scanMax;

                bool duplicateScan = false; // are two points with the same scan number in this bin?
                bool l_maxdist_tooclose = false;
                bool r_maxdist_tooclose = false; // Check if there is a point within maxdist

                std::vector<qCentroid *> pointsInBin; // @todo does not change after bin creation, check for performance improvement when using cast const
                std::vector<double> activeOS;         // Order Space
                std::vector<double> DQSB_base;        // DQSB when all distances are considered equal
                std::vector<double> DQSB_scaled;      // DQSB if a gaussian falloff is assumed

                /// @brief generate a bin that is a subset of an existing bin using two iterators.
                /// @details since this extracts a continuous sequence, it is only a good idea
                /// to construct a new bin like this after a completed subsetting step.
                /// @param startBin left border of the new bin
                /// @param endBin right border of the new bin
                Bin(const std::vector<qCentroid *>::iterator &startBin, const std::vector<qCentroid *>::iterator &endBin); //

                /// @brief generate a bin by adding pointers to all points in rawdata to pointsInBin
                /// @param rawdata a set of raw data on which binning is to be performed
                Bin(CentroidedData *rawdata);
                Bin();
                ~Bin();

                /// @brief generate the activeOS vector containing the difference to the next greatest mz for every point
                void makeOS();

                /// @brief generate the cumError vector for this bin
                void makeCumError();
                void makeCumError(const double ppm); // if no centroid error exists @todo remove

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
                void subsetMZ(std::deque<Bin> *bincontainer, std::vector<double> &OS, const int startBin, const int endBin, int &counter);

                /// @brief divide a bin sorted by scans if there are gaps greater than maxdist in it. Bins that cannot be divided are closed.
                /// @details this function sorts all members of a bin by scans and iterates over them. If a gap greater than maxdist exists,
                /// the bin is split at this point. Only subsets with more than five elements are added to the bin deque. If a bin is not split,
                /// it is added to the finishedBins vector and no further subsets will be performed on it. As such, subsetScan() must be the last
                /// subset function and cannot be used in combination with any other subsetting function that decides if a bin is completed or not.
                /// @param bincontainer if the input bin was split, the newly created bins will be added to this
                /// @param finishedBins if the input bin was not split, it will be added to this
                /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
                /// @param counter counter shared between all calls of the recursive function, used to count number of function calls
                void subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int maxdist, int &counter);

                // usage: Start once before rebinning, cut affected bins and add them to the deque
                void subsetNaturalBreaksMZ(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins);

                /// @brief generate the data quality score for all data points in a bin
                /// @details for every point in the bin the mean distance in mz to other elements of the bin and the shortest distance to an
                /// element not in the bin is calculated. The outer distance may not be to a point more than maxdist scans away from the
                /// binned datapoint. It is assumed that the bin is sorted by scans when makeDQSB is called @todo change to more generic solution?
                /// @param rawdata the CentroidedData object from which the bin was generated
                /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
                void makeDQSB(const CentroidedData *rawdata, const int maxdist);

                double calcStdevMZ();

                /// @brief summarise the bin to one line, with the parameters size, mean_mz, median_mz, stdev_mz, mean_scans, median_scans,
                /// DQSB_base, DQSB_control, DQSB_worst, min_DQSC, meanError. DQSB_worst is calculated by assuming the MOD of all points in the
                /// bin to be equal to the critical value. Additionally, the function computes a 1-byte code for up to 8 states of interest.
                /// The respective bit is set to 1 if the defined state is present. Possible states of interest are too large a discrepancy
                /// between mean and median or the presence of duplicate values.
                /// @return A struct containing the summary information. The first entry is the error code.
                SummaryOutput summariseBin();

                EIC_old createEIC_old();

                EIC createEIC();
            };

            // BinContainer
            class BinContainer
            {
            private:
                // void readcsv(std::string user_file, std::vector<qCentroid> output, int d_mz, int d_mzError, int d_RT, int d_scanNo); // implemented for featurelist
                std::deque<Bin> binDeque;    // @todo add case for no viable bins
                std::vector<Bin> redoneBins; // store bins after reassembly/cutting here
                int subsetCount;

            public:
                std::vector<Bin> finishedBins; // only includes bins which cannot be further subdivided

                BinContainer();

                /// @brief create a bin out of rawdata and add it to the binDeque. This function exists since all subsetting functions require a bin to start
                /// @param rawdata a set of raw data on which binning is to be performed
                void makeFirstBin(CentroidedData *rawdata); // @todo find way for rawdata to be const

                /// @brief create a new bin that contains all unused datapoints and the bins which were not fully completed
                /// @param selectedBins indices of bins which are to be rebinned
                /// @param notInBins points which could not be assigned to a bin on the first passthrough
                void makeReassemblyBin(std::vector<size_t> selectedBins, std::vector<qCentroid *> notInBins); // @todo integrate this function

                /// @brief perform the specified subsetting operations on the binDeque. Elements are added to the back and deleted from the front
                /// @param dimensions which dimensions should be used for subsetting in what order. 1 = subsetting by mz, 2 = subsetting by scans.
                /// Important: The last element of dimensions must determine bins to be finished, and no other subsetter may add to finsihedBins.
                /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
                void subsetBins(const std::vector<int> dimensions, const int maxdist, bool rebin, const double massError);

                /// @brief apply DQSB_base function to all completed bins
                /// @param rawdata the CentroidedData object from which all bins in finishedBins were generated
                /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
                void assignDQSB(const CentroidedData *rawdata, const int maxdist, bool rebin);

                void printBinSummary(std::string path); // @todo create a better summary function which gives a
                                                        // detailed report, including what the test actually does,
                                                        // the mz subset after which a bin was complete, statistics
                                                        // regarding the rest of the bins in terms of size, count etc.
                                                        // Make sure to test the report for human-readability and
                                                        // include metadata from the mzml where possible.
                                                        // Some measure of overall spectrum quality should also be given.

                /// @brief Add all bins identified as having hot ends to outofbins and redo the binning using subsetBins
                /// @param dimensions parameter for subsetBins, see there for a detailed explanation
                /// @param rawdata the CentroidedData object from which all bins in finishedBins were generated
                /// @param notbinned points which were not used in a previous binning step
                /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
                void redoBinningIfTooclose(const std::vector<int> dimensions, const CentroidedData *rawdata, std::vector<qCentroid *> notbinned, const int maxdist);

                void mergeByStdev(double mzFilterLower, double mzFilterUpper);

                void reconstructFromStdev(const CentroidedData *rawdata, int maxdist);

                void printAllBins(std::string path, const CentroidedData *rawdata); // @todo remove rawdata dependency

                void printBinningReport(std::string path); // <- here

                std::vector<EIC> returnBins();

                /// @brief Print the individual bin members and optionally the bin summary in a separate .csv
                /// @param mask every bit of this byte controls if the corresponding test is considered for returning the
                /// selection. For example, for returning only bins in which the first test condition applies,
                /// the input would have to be std::byte{0b00000001}
                /// @param fullBins Should all bins which are included in the summary be printed in full?
                /// @param location path to the folder in which both files should be created
                void printSelectBins(std::byte mask, bool fullBins, std::string location);
            };

            // utility functions

            void check_MOD_outOfBins(const Bin *target, const std::vector<qCentroid *> notBinned, const int maxdist);

            // ### wrapper function to execute qbinning on a CentroidedData struct ###
            std::vector<EIC> performQbinning(const CentroidedData centroidedData, std::string outpath, int maxdist, bool silent, bool printBinSummary);

        }
    }
}
#endif