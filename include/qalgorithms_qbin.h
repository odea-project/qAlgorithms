#ifndef _QALGORITHMS_QBIN_INCLUDED
#define _QALGORITHMS_QBIN_INCLUDED

#include <deque>
#include <vector>
#include <string>
#include <filesystem> // printing absolute path in case read fails

#include "qalgorithms_datatypes.h"

namespace qAlgorithms
{
#pragma region "utility"
    // the metacentroid contains information that is centroid-specific but not
    // required during the processing.
    // struct metaCentroid
    // {
    //     float int_area;
    //     float int_height;
    //     float DQSCentroid;
    //     int profileIdxStart;
    //     int profileIdxEnd;
    // };

    // depending on the instrumentation, binning requires different subsetting
    // procedures. Instead of making those user parameters, the correct steps
    // are selected based on known behaviour @todo make maxdist depend on this
    // enum MeasurementType
    // {
    //     // instrumentation detail which could be relevant to binning
    //     LC_HRMS,
    //     GC_HRMS,
    //     IMS_HRMS,
    //     LC_IMS_HRMS,
    //     LC_LC_HRMS,
    //     LCxLC_HRMS,
    //     LC_UV_HRMS,
    //     LC_MS_IMS,
    //     // measurement independent binning application
    //     FIND_COMPONENT
    // };

    enum SubsetMethods
    {
        mz,
        scans,
        duplicates,
        massjump,
        finaliser // this filter checks if the bin was changed and adds it to the "viable bins" vector if not
    };

    // @todo does the ion source have an effect on the binning decisions to be made?

    struct RunAssessor // one of these per analysed measurement
    {
        std::vector<std::byte> errorcodes;
        std::vector<int> countOfBins;
        std::vector<int> countOfPoints;
        std::vector<float> meanDQSB;
        int totalPoints;
        int unbinnedPoints;
        int countDQSbelow0;
    };

    const double binningCritVal(size_t n, double uncertainty); // critical value for deciding if a bin exists or not

#pragma endregion "utility"

#pragma region "Bin"

    // Bin Class @todo get under size 128
    class Bin
    {
    private:
        std::vector<double> cumError; // cumulative error in mz
        // float medianMZ;

    public:
        std::vector<const qCentroid *> pointsInBin; // @todo does not change after bin creation, check for performance improvement when using cast const
        std::vector<double> activeOS;               // Order Space
        std::vector<float> DQSB_base;               // DQSB when all distances are considered equal
        std::vector<float> DQSB_scaled;             // DQSB if a gaussian falloff is assumed

        // @todo get mz and scan min/max at the earliest opportunity
        float mzMin = -1;
        float mzMax = -1;
        int scanMin = -1;
        int scanMax = -1;
        float medianMZ = -1; // only used for removing duplicates

        bool unchanged = false;     // if this is true after every test has run once, the bin is viable
        bool duplicateScan = false; // are two points with the same scan number in this bin?
        bool l_maxdist_tooclose = false;
        bool r_maxdist_tooclose = false; // Check if there is a point within maxdist

        /// @brief generate a bin that is a subset of an existing bin using two iterators.
        /// @details since this extracts a continuous sequence, it is only a good idea
        /// to construct a new bin like this after a completed subsetting step.
        /// @param startBin left border of the new bin
        /// @param endBin right border of the new bin
        Bin(const std::vector<const qCentroid *>::iterator &startBin, const std::vector<const qCentroid *>::iterator &endBin); //

        /// @brief generate a bin by adding pointers to all points in rawdata to pointsInBin
        /// @param rawdata a set of raw data on which binning is to be performed
        Bin(CentroidedData *rawdata);
        Bin();

        /// @brief generate the activeOS vector containing the difference to the next greatest mz for every point
        void makeOS();

        /// @brief generate the cumError vector for this bin
        void makeCumError();
        // void makeCumError(const double ppm); // if no centroid error exists @todo remove

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
        void subsetMZ(std::vector<Bin> *bincontainer, std::vector<double> &OS, std::vector<const qCentroid *> &notInBins,
                      const int startBin, const int endBin, int &counter);

        /// @brief divide a bin sorted by scans if there are gaps greater than maxdist in it. Bins that cannot be divided are closed.
        /// @details this function sorts all members of a bin by scans and iterates over them. If a gap greater than maxdist exists,
        /// the bin is split at this point. Only subsets with more than five elements are added to the bin deque. If a bin is not split,
        /// it is added to the finishedBins vector and no further subsets will be performed on it. As such, subsetScan() must be the last
        /// subset function and cannot be used in combination with any other subsetting function that decides if a bin is completed or not.
        /// @param bincontainer if the input bin was split, the newly created bins will be added to this
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        /// @param counter counter shared between all calls of the recursive function, used to count number of function calls
        void subsetScan(std::vector<Bin> *bincontainer, std::vector<const qCentroid *> &notInBins, const size_t maxdist, int &counter);

        /// @brief generate the data quality score for all data points in a bin
        /// @details for every point in the bin the mean distance in mz to other elements of the bin and the shortest distance to an
        /// element not in the bin is calculated. The outer distance may not be to a point more than maxdist scans away from the
        /// binned datapoint. It is assumed that the bin is sorted by scans when makeDQSB is called @todo change to more generic solution?
        /// @param rawdata the CentroidedData object from which the bin was generated
        /// @param maxdist the largest gap in scans which a bin can have while still being considered valid
        void makeDQSB(const CentroidedData *rawdata, const std::vector<float> scalarForMOD, const size_t maxdist);

        EIC createEIC(std::vector<float> convertRT, size_t maxdist);
    };

    bool binLimitsOK(Bin sourceBin, const CentroidedData *rawdata, const int maxdist);

#pragma endregion "Bin"

#pragma region "Bin Container"
    // BinContainer
    struct BinContainer
    {
        std::vector<Bin> processBinsF;            // bin target in the starting case
        std::vector<Bin> processBinsT;            // bin target one past the starting case
        std::vector<Bin> viableBins;              // only includes bins which cannot be further subdivided
        std::vector<Bin> finalBins;               // bins which have been confirmed to not include incorrect binning
        std::vector<const qCentroid *> notInBins; // this vector contains all points which are not included in bins
        int subsetCount;                          // @todo necessary?
        int inputBinCount;                        // number of bins that were rebinned in the last iteration
        bool readFrom = false;
        // pointers for use during subsetting loop
        std::vector<Bin> *sourceBins;
        std::vector<Bin> *targetBins;
    };

    void switchTarget(BinContainer *bincontainer);

    BinContainer initialiseBinning(CentroidedData *rawdata);

    std::string subsetBins(BinContainer &bincontainer, const size_t maxdist);

    int selectRebin(BinContainer *bins, const CentroidedData *rawdata, const size_t maxdist);

    // remove points with duplicate scans from a bin by choosing the one closest to the median
    void deduplicateBin(std::vector<Bin> *target, std::vector<const qCentroid *> *notInBins, Bin bin);

    void removeMassJumps(std::vector<Bin> *target, std::vector<const qCentroid *> *notInBins, Bin bin);

#pragma endregion "Bin Container"
    // ####################################################################################################### //

    /// @brief wrapper function to execute qbinning on a CentroidedData struct
    /// @param centroidedData centroid list generated by qPeaks.passToBinning(...), defined in qalgorithms_qpeaks.cpp
    /// @param convertRT vector containing the retention time for every scan number
    /// @param maxdist maximum distance in the scans dimension that is allowed within a bin
    /// @param verbose if this option is selected, additional progress report is written to standard out
    /// @return returns the centroids as a collection of vectors
    std::vector<EIC> performQbinning(const CentroidedData *centroidedData, const std::vector<float> convertRT,
                                     size_t maxdist, bool verbose);

    // ###################################################################################################### //
}

#endif