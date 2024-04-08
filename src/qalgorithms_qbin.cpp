#include "../include/qalgorithms_qbin.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <thread> // unused as of now
#include <ranges>
#include <deque> // main bin container is a deque
#include <string>
#include <iomanip> // for printing with full precision
#include <chrono>  // time code execution
#include <ctime>
#include <regex>

namespace q
{
    int pt_subsetcount = 0;
    int pt_OSzero = 0;
    int pt_subsets = 0;
    std::vector<double> pt_stepsForDQS;
    std::vector<double> pt_scanRelSteps;
    std::vector<Datapoint *> pt_outOfBins;
#define IGNORE -256 // nonsense value if no real number exists. Must be negative since it is used later when searching for the smallest distance
#define NO_MIN_FOUND -256
#define NO_MAX_FOUND -256

#pragma region "Rawdata"
    RawData::RawData() {}
    q::RawData::~RawData() {} // potential memory leak
    bool RawData::readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int pt_d_binID)
    {
        int now = time(0);
        lengthAllFeatures = 0;
        allDatapoints.push_back(std::vector<Datapoint>(0)); // first element empty since scans are 1-indexed
        std::ifstream file(user_file);
        if (!file.is_open())
        {
            std::cout << "the file could not be opened\n";
            return false;
        }

        if (!file.good())
        {
            std::cout << "something is wrong with the input file\n";
            return false;
        }
        std::string line;
        std::string dummy;
        std::getline(file, dummy); // do not read first row ßßß data is probably not going to be passed with headers
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string cell;
            std::vector<double> row;
            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }
            // add conditional here to set feature error to mz* x ppm ßßß
            unsigned int i_scanNo = (unsigned int)row[d_scanNo];
            if (i_scanNo > allDatapoints.size() - 1)
            {
                for (size_t i = allDatapoints.size() - 1; i < i_scanNo; i++)
                {
                    allDatapoints.push_back(std::vector<Datapoint>(0));
                }
            }
            Datapoint F = Datapoint{row[d_mz], row[d_mzError], row[d_RT], i_scanNo, (int)row[pt_d_binID]};

            ++lengthAllFeatures;
            allDatapoints[i_scanNo].push_back(F); // every subvector in allDatapoints is one complete scan - does not require a sorted input file!
        }
        for (size_t i = 1; i < allDatapoints.size(); i++) // necessary if features aren't already sorted, performance impact should be very small
        {
            std::sort(allDatapoints[i].begin(), allDatapoints[i].end(), [](const Datapoint lhs, const Datapoint rhs)
                  { return lhs.mz < rhs.mz; });
        }
        std::cout << "Finished reading file in " << time(0) - now << " seconds\n";
        return true;
        // RawData is always a vector of vectors where the first index is the scan number (starting at 1) and every scsn is sorted by mz
    }

    // bool RawData::readtxt(std::string user_file) // @todo function must be defined for qcentroid
    // {
    //     std::ifstream file(user_file);
    //     assert(file.is_open());
    //     assert(file.good());
    //     std::string line;
    //     int currentScan;
    //     double currentRT;
    //     Datapoint F;
    //     const char newScan = *"N";
    //     std::regex number("[0-9\.]*");
    //     std::smatch match;
    //     while (std::getline(file, line))
    //     {
    //         if (line[0] == newScan)
    //         {
    //             /* code */
    //         }

    //         F.scanNo = currentScan;
    //         F.RT = currentRT;
    //     }
    // }

#pragma endregion "Rawdata"

#pragma region "BinContainer"

    BinContainer::BinContainer() {}
    BinContainer::~BinContainer() {}

    void BinContainer::makeFirstBin(RawData *rawdata)
    {
        const Bin firstBin(rawdata);
        binDeque.push_back(firstBin);
    }

    enum SubsetMethods
    {
        mz,
        scans
    };

    void BinContainer::subsetBins(std::vector<int> dimensions, const unsigned int maxdist)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        auto timeEnd = std::chrono::high_resolution_clock::now();
        std::cout << "\nTotal bins: 1";
        // while elements are in the bin deque -> if any bin is not fully subset
        while (!binDeque.empty())
        {
            for (size_t i = 0; i < dimensions.size(); i++) // dimensions vector allows user to choose order of executing subsetting actions. It is also possible to only subset in mz or RT
            {
                switch (dimensions[i]) // TODO name case statements, enums
                {
                case 1:
                { // brackets needed to prevent error
                    // bin in mz
                    size_t startpoint = binDeque.size(); // startpoint is the first element of the deque that will not be subset, equivalent to do n times
                    for (size_t i = 0; i < startpoint; i++)
                    { // random access not needed, first element -> make bins -> move element or make bins, then remove first entry ; do n times
                        binDeque.front().makeOS();
                        binDeque.front().makeCumError();                                                                          // always after order space, since the features get sorted @todo remember ppm
                        binDeque.front().subsetMZ(&binDeque, binDeque.front().activeOS, 0, binDeque.front().activeOS.size() - 1); // takes element from binDeque, starts subsetting, appends bins to binDeque
                        binDeque.pop_front();                                                                                     // remove the element that was processed from binDeque
                    }
                    timeEnd = std::chrono::high_resolution_clock::now();
                    std::cout << "\nmz subsetting done\nTotal bins: " << binDeque.size() << " ; time: " << (timeEnd - timeStart).count() << " ns";
                    timeStart = std::chrono::high_resolution_clock::now();
                    break;
                }

                case 2:
                {
                    // bin using scan numbers
                    size_t startpoint = binDeque.size();
                    for (size_t i = 0; i < startpoint; i++)
                    {
                        binDeque.front().subsetScan(&binDeque, &finishedBins, maxdist);
                        binDeque.pop_front();
                    }
                    timeEnd = std::chrono::high_resolution_clock::now();
                    std::cout << "\nscan subsetting done\nClosed bins: " << finishedBins.size() << " ; time: " << (timeEnd - timeStart).count() << " ns"
                              << "\nTotal bins: " << binDeque.size();
                    timeStart = std::chrono::high_resolution_clock::now();
                    break;
                }

                default:
                    std::cout << "\nSeparation method " << dimensions[i] << " is not a valid parameter, skipping... \n";
                    break;
                }
                ++pt_subsets;
            }
        }
        // finishedBins.resize(finishedBins.size()); //ßßß why does this result in an error?
    }

    void BinContainer::assignDQSB(const RawData *rawdata, const unsigned int maxdist)
    {
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finishedBins[i].makeDQSB(rawdata, maxdist);
        }
    }

    void BinContainer::printAllBins(std::string path, RawData *rawdata)
    {
        std::fstream file_out;
        file_out.open(path);
        assert(file_out.is_open());
        // possible symbols
        char shapes[] = "acGuxnoQzRsTvjIEf";

        file_out << "mz,rt,ID,color,shape\n";
        // for (size_t i = 0; i < rawdata->allDatapoints.size(); i++)
        // {
        //     file_out << std::setprecision(15) << rawdata->allDatapoints[i]->mz << "," << rawdata->allDatapoints[i]->scanNo << "," << 0 << "\n";
        // }

        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            char x = shapes[rand() % 17];
            int colour = i % 9; // 10 colours total
            std::vector<Datapoint *> binnedPoints = finishedBins[i].pointsInBin;
            for (size_t j = 0; j < binnedPoints.size(); j++)
            {
                file_out << std::setprecision(15) << binnedPoints[j]->mz << "," << binnedPoints[j]->scanNo << "," << i + 1 << "," << colour << "," << x << "\n";
            }
            // file_out << finishedBins[i].pointsInBin.size() << "\n";
        }
    }

    void BinContainer::printBinSummary(std::string path)
    {
        std::fstream file_out;
        file_out.open(path);
        assert(file_out.is_open());
        file_out << "ID,size,mean_mz,mean_scans,mean_DQS\n";
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            file_out << i << "," << finishedBins[i].summariseBin() << "\n";
        }
    }

    void BinContainer::controlAllBins()
    {
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finishedBins[i].controlBin(i);
        }
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin(const std::vector<Datapoint *>::iterator &binStartInOS, const std::vector<Datapoint *>::iterator &binEndInOS) // const std::vector<Datapoint> &sourceList,
    {
        pointsInBin = std::vector<Datapoint *>(binStartInOS, binEndInOS);
    }
    Bin::Bin(RawData *rawdata) // relatively time intensive, better solution?
    {
        pointsInBin.reserve(rawdata->lengthAllFeatures);
        for (size_t i = 1; i < rawdata->allDatapoints.size(); i++)
        {
            for (size_t j = 0; j < rawdata->allDatapoints[i].size(); j++)
            {
                pointsInBin.push_back(&rawdata->allDatapoints[i][j]);
            }
        }
    }
    Bin::~Bin() {}

    void Bin::makeOS()
    {
        // sort features by mz
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const Datapoint *lhs, const Datapoint *rhs)
                  { return lhs->mz < rhs->mz; });

        activeOS.reserve(pointsInBin.size());               // OS = Order Space
        for (size_t i = 0; i + 1 < pointsInBin.size(); i++) // +1 to prevent accessing outside of vector
        {
            activeOS.push_back(pointsInBin[i + 1]->mz - pointsInBin[i]->mz);
        }
        activeOS.push_back(IGNORE); // IGNORE to denote last element in OS
    }

    void Bin::makeCumError()
    {
        cumError.reserve(pointsInBin.size());
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [](Datapoint *F)
                       { return F->mzError; });                               // transfer error values from feature objects
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::makeCumError(const double ppm)
    {
        cumError.reserve(pointsInBin.size());
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [ppm](Datapoint *F)
                       { return F->mz * 0.000001 * ppm; });
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin());
    }

    void Bin::subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, int binStartInOS, int binEndInOS) // bincontainer is binDeque of BinContainer // OS cannot be solved with pointers since index has to be transferred to frature list
    {
        ++pt_subsetcount;
        assert(binStartInOS >= 0);
        assert(binEndInOS >= 0);
        const int binsizeInOS = binEndInOS - binStartInOS + 1; // +1 to avoid length zero
        if (binsizeInOS < 5)
        {
            if (binsizeInOS == 1)
            {
                Datapoint *F = *(pointsInBin.begin() + binStartInOS);
                pt_outOfBins.push_back(F);
            }
            else
            {
                for (int i = 0; i < binsizeInOS; i++)
                {
                    Datapoint *F = *(pointsInBin.begin() + binStartInOS + i);
                    pt_outOfBins.push_back(F);
                }

                // pt_outOfBins.insert(pt_outOfBins.end(), pointsInBin.begin() + binStartInOS, pointsInBin.begin() + binEndInOS + 1); // ßßß place discarded features in separate table
            }
            return;
        }

        auto pmax = std::max_element(OS.begin() + binStartInOS, OS.begin() + binEndInOS); // @TODO check if correct

        double vcrit = 3.05037165842070 * pow(log(binsizeInOS), (-0.4771864667153)) * (cumError[binEndInOS] - cumError[binStartInOS]) / (binsizeInOS - 1); // critical value for alpha = 0.01. ßßß add functionality for custom alpha?
        double max = *pmax;
        if (max < vcrit) // all values in range are part of one mz bin
        {
            // make bin
            // std::cout << binStartInOS << "," << binEndInOS << "," << subsetcount << "\n"; // not all that useful, since knowledge of position in original order space is always lost
            pt_subsetcount = 0;
            const Bin output(pointsInBin.begin() + binStartInOS, pointsInBin.begin() + binEndInOS + 1); // binEndInOS+1 since the iterator has to point behind the last element to put into the vector
            // append Bin to bin container
            bincontainer->push_back(output);
            return;
        }
        else
        {
            int cutpos = std::distance(OS.begin() + binStartInOS, pmax);
            subsetMZ(bincontainer, OS, binStartInOS, binStartInOS + cutpos);
            subsetMZ(bincontainer, OS, binStartInOS + cutpos + 1, binEndInOS);
            return;
        }
    }

    void Bin::subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const unsigned int &maxdist)
    {
        // function is called on a bin sorted by mz
        size_t binSize = pointsInBin.size();
        double tmp_pt_mzmin = pointsInBin.front()->mz;
        double tmp_pt_mzmax = pointsInBin.back()->mz;
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const Datapoint *lhs, const Datapoint *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        std::vector<Datapoint *>::iterator newstart = pointsInBin.begin();
        int lastpos = 0;
        for (size_t i = 1; i < binSize; i++)
        {
            if (pointsInBin[i]->scanNo - pointsInBin[i - 1]->scanNo > maxdist) // bin needs to be split @todo suppress warning
            {
                // less than five features in bin
                if (i - lastpos - 1 < 5)
                {
                    pt_outOfBins.insert(pt_outOfBins.end(), newstart, pointsInBin.begin() + i - 1); // ßßß place discarded features in separate table
                    newstart = pointsInBin.begin() + i;                                             // new start is one behind current i
                }
                else
                {
                    // no bins of size 5 are ever produced when splitting by scans - ßßß try reduced maxdist
                    // viable bin, stable in scan dimension
                    Bin output(newstart, pointsInBin.begin() + i - 1);
                    bincontainer->push_back(output);
                    newstart = pointsInBin.begin() + i;
                }
                lastpos = i; // sets previ to the position one i ahead, since current value is i-1
            }
        }
        // check for open bin at the end
        if (lastpos == 0) // no cut has occurred
        {
            bincontainer->front().pt_mzmin = tmp_pt_mzmin;
            bincontainer->front().pt_mzmax = tmp_pt_mzmax;
            bincontainer->front().pt_scanmin = pointsInBin.front()->scanNo;
            bincontainer->front().pt_scanmax = pointsInBin.back()->scanNo;
            bincontainer->front().pt_subsetcount = pt_subsets; // ßßß rm
            finishedBins->push_back(bincontainer->front());
        }
        else if (binSize + 1 - lastpos > 5)
        {
            // viable bin, stable in scan dimension
            Bin output(newstart, pointsInBin.end());
            bincontainer->push_back(output);
        }
        else
        {
            pt_outOfBins.insert(pt_outOfBins.end(), newstart, pointsInBin.end()); // ßßß place discarded features in separate table
        }
    }

    void Bin::makeDQSB(const RawData *rawdata, const unsigned int &maxdist)
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        size_t binsize = pointsInBin.size();
        DQSB.reserve(binsize);
        // determine start and end of relevant scan section, used as repeats for the for loop; -1 since accessed vector is zero-indexed
        unsigned int minScanInner = pointsInBin.front()->scanNo; //
        int scanRangeStart = minScanInner - maxdist;
        unsigned int scanRangeEnd = pointsInBin.back()->scanNo + maxdist;

        int maxScansReduced = 0;              // add this many dummy values to prevent segfault when bin is in one of the last scans
        std::vector<double> minMaxOutPerScan; // contains both mz values (per scan) next or closest to all m/z in the bin
        minMaxOutPerScan.reserve((scanRangeEnd - scanRangeStart) * 2);
        if (scanRangeStart < 1)
        {
            for (int i = 0; i < (1 - scanRangeStart) * 2; i++) // fill with dummy values to prevent segfault when distance checker expects negative scan number
            {
                minMaxOutPerScan.push_back(IGNORE);
            }

            scanRangeStart = 1;
        }
        else if (scanRangeEnd > rawdata->allDatapoints.size())
        {
            maxScansReduced = scanRangeEnd - rawdata->allDatapoints.size(); // dummy values have to be added later
            scanRangeEnd = rawdata->allDatapoints.size();
        }

        // determine min and max in mz - sort, since then calculation of inner distances is significantly faster
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const Datapoint *lhs, const Datapoint *rhs)
                  { return lhs->mz < rhs->mz; });
        const double minInnerMZ = pointsInBin.front()->mz;
        const double maxInnerMZ = pointsInBin.back()->mz;
        std::vector<double> meanInnerDistances = meanDistance(pointsInBin);

        // for all scans relevant to the bin
        int needle = 0;     // position in scan where a value was found - starts at 0 for first scan
#pragma GCC diagnostic push // do not display the specific warning for rounding a double to integer
#pragma GCC diagnostic ignored "-Wsign-compare"
        for (int i = scanRangeStart; i < scanRangeEnd; i++)
#pragma GCC diagnostic pop
        {
            bool minFound = false; // only execute search if min or max is present in the scan
            bool maxFound = false;
            int scansize = rawdata->allDatapoints[i].size() - 1;
            if (needle > scansize)
                needle = scansize; // prevent searching outside of the valid scan region

            // check begin of bin for possible segfault
            // double firstmz = rawdata->allDatapoints[i][1].mz;
            if (rawdata->allDatapoints[i][0].mz >= minInnerMZ)
            {
                minMaxOutPerScan.push_back(NO_MIN_FOUND);
                minFound = true;
                if (rawdata->allDatapoints[i][0].mz > maxInnerMZ)
                {
                    minMaxOutPerScan.push_back(rawdata->allDatapoints[i][0].mz);
                    maxFound = true;
                }
                else
                {
                    needle = 0;
                }
            }
            // check end of bin
            // double lastmz = rawdata->allDatapoints[i][scansize].mz;
            if (rawdata->allDatapoints[i][scansize].mz <= maxInnerMZ)
            {
                minMaxOutPerScan.push_back(NO_MAX_FOUND);
                maxFound = true;
                if (rawdata->allDatapoints[i][scansize].mz < minInnerMZ)
                {
                    minMaxOutPerScan.push_back(rawdata->allDatapoints[i][scansize].mz);
                    minFound = true;
                }
                else
                {
                    needle = scansize;
                }
            }
            // rawdata is always sorted by mz within scans
            // use two while loops to find desired value from any place in the scan, accounts for large shifts between scans
            if (!minFound)
            {
                while (rawdata->allDatapoints[i][needle].mz < minInnerMZ)
                {
                    ++needle; // steps through the dataset and increments until needle is the first value >= minInnerMZ
                }
                while (rawdata->allDatapoints[i][needle].mz >= minInnerMZ)
                {
                    --needle; // steps through the dataset and decrements until needle is the desired mz value
                }
                minMaxOutPerScan.push_back(rawdata->allDatapoints[i][needle].mz);
            }

            if (!maxFound)
            {
                while (rawdata->allDatapoints[i][needle].mz > maxInnerMZ)
                {
                    --needle;
                }
                while (rawdata->allDatapoints[i][needle].mz <= maxInnerMZ)
                {
                    ++needle;
                }
                minMaxOutPerScan.push_back(rawdata->allDatapoints[i][needle].mz);
            }
        }
        // minMaxOutPerScan contains the relevant distances in order of scans, with both min and max per scan being stored for comparison
        if (maxScansReduced != 0) // add dummy values after last scan
        {
            for (int i = 0; i < 2 * maxScansReduced; i++)
            {
                minMaxOutPerScan.push_back(IGNORE);
            }
        }

        // find min distance in minMaxOutPerScan, assign to minOuterDistances in order of the pointsInBin
        for (size_t i = 0; i < binsize; i++)
        {
            double minDist = 256;
            double currentMZ = pointsInBin[i]->mz;
            unsigned int currentRangeStart = (pointsInBin[i]->scanNo - minScanInner) * 2; // gives position of the first value in minMaxOutPerScan that must be considered, assuming the first value in minMaxOutPerScan (index 0) is only relevant to the Datapoint in the lowest scan. For every increase in scans, that range starts two elements later
            unsigned int currentRangeEnd = currentRangeStart + maxdist * 2 + 1;
            for (unsigned int j = currentRangeStart; j <= currentRangeEnd; j++) // from lowest scan to highest scan relevant to feature, +1 since scan no of feature has to be included
            {
                double dist = std::abs(currentMZ - minMaxOutPerScan[j]);
                if (dist < minDist)
                {
                    minDist = dist;
                    assert(minDist > 0);
                }
            }

            // calculate DQSB on a per-feature basis
            DQSB.push_back(calcDQS(meanInnerDistances[i], minDist));
        }
    }

    std::string Bin::summariseBin()
    {
        // ID, size, mean_mz, mean_scans, mean_DQS \n
        std::stringstream buffer;
        size_t binsize = pointsInBin.size();
        double meanMZ = 0;
        double meanScan = 0;
        double meanDQS = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            meanMZ += pointsInBin[i]->mz;
            meanScan += pointsInBin[i]->scanNo;
            meanDQS += DQSB[i];
        }

        meanMZ = meanMZ / binsize;
        meanScan = meanScan / binsize;
        meanDQS = meanDQS / binsize;
        buffer << binsize << "," << meanMZ << "," << meanScan << "," << meanDQS;
        std::string output = buffer.str();
        return output;
    }

    void Bin::controlBin(int binID)
    {
        for (size_t i = 0; i < pointsInBin.size() - 1; i++)
        {
            if (pointsInBin[i]->pt_binID != pointsInBin[i + 1]->pt_binID) // color 11 for mismatch  mz,rt,ID,color,shape
            {
                std::cout << pointsInBin[i]->mz << "," << pointsInBin[i]->scanNo << "," << binID << ",11,L\n";
            }
        }
        return;
    }

#pragma endregion "Bin"

#pragma region "Functions"

    std::vector<double> meanDistance(std::vector<Datapoint *> featurelistBin)
    {
        // assumes bin is sorted by mz
        size_t binsize = featurelistBin.size();
        double totalSum = 0;
        std::vector<double> output(binsize);
        for (size_t i = 0; i < binsize; i++)
        {
            totalSum += featurelistBin[i]->mz;
        }
        double beforeSum = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            double v1 = (totalSum - featurelistBin[i]->mz * (binsize - i));
            double v2 = (featurelistBin[i]->mz * i - beforeSum);
            beforeSum += featurelistBin[i]->mz;
            totalSum -= featurelistBin[i]->mz;
            output[i] = (v1 + v2) / (binsize - 1);
        }
        return output;
    }

    inline double calcDQS(double MID, double MOD) // mean inner distance, minimum outer distance
    {
        double dqs = MID;
        if (dqs < MOD)
        {
            dqs = MOD;
        }
        dqs = (MOD - MID) * (1 / (1 + MID)) / dqs; // sm(i) term
        dqs = (dqs + 1) / 2;                       // interval transform
        assert(0 <= dqs && dqs <= 1);
        return dqs;
    }

#pragma endregion "Functions"

}

int main()
{
    std::cout << "starting...\n";
    // std::ofstream result("../../qbinning_ßßß.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    // std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!
    // std::cout << "scan,position\n";

    q::RawData testdata;                                                                        
    // path to data, mz column, centroid error column, RT column, scan number column, control ID column (optional)
    testdata.readcsv("../../rawdata/qBinnings_Warburg_pos_171123_01_Zu_01.csv", 0, 4, 1, 2, 7); // ../../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv ../test/test.csv
    // for (size_t i = 0; i < testdata.scanBreaks.size(); i++)
    // {
    //     std::cout << i << "," << testdata.scanBreaks[i] << "\n";
    // }
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin(&testdata);
    std::vector<int> dim = {1, 2};    // last element must be the number of scans ßßß implement outside of the switch statement ßßß endless loop if scan terminator is not included
    testcontainer.subsetBins(dim, 7); // 7 = max dist in scans
    std::cout << "\ncalculating DQSBs...\n";

    std::ofstream result("../../qbinning_faultybins.csv");
    std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!

    std::cout << "mz,rt,ID,color,shape\n";
    testcontainer.controlAllBins();

    std::cout.rdbuf(coutbuf); // reset to standard output again

    testcontainer.printAllBins("../../qbinning_binlist.csv", &testdata);

    std::fstream file_out;
    file_out.open("../../qbinning_notbinned.csv");
    if (!file_out.is_open())
    {
        throw;
    }

    file_out << "mz,rt,ID,color,shape\n";
    q::pt_outOfBins.resize(q::pt_outOfBins.size());
    for (size_t i = 0; i < q::pt_outOfBins.size(); i++)
    {
        file_out << std::setprecision(15) << q::pt_outOfBins[i]->mz << "," << q::pt_outOfBins[i]->scanNo << "," << -1 << "," << -1 << ","
                 << "Y\n";
    }

    testcontainer.assignDQSB(&testdata, 7);

    testcontainer.printBinSummary("../../qbinning_binsummary.csv");

    std::cout << "\n\nDone!\n\n";
    return 0;
}
