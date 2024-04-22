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
    // @todo remove test values
    std::vector<double> pt_stepsForDQS;
    std::vector<double> pt_scanRelSteps;
    std::vector<Datapoint *> control_outOfBins;

#define IGNORE -256 // nonsense value if no real number exists. Must be negative since it is used later when searching for the smallest distance
#define NO_MIN_FOUND -INFINITY
#define NO_MAX_FOUND -INFINITY

#pragma region "Rawdata"
    RawData::RawData() {}
    q::RawData::~RawData() {} // potential memory leak
    bool RawData::readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int d_intensity, int d_control_binID, int d_control_DQScentroid, int d_control_DQSbin)
    { // @todo stop segfaults when reading empty lines; use buffers for speedup
        int now = time(0);
        lengthAllPoints = 0;
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
        std::getline(file, dummy); // do not read first row @todo check if first row starts with a number
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string cell;
            std::vector<double> row;
            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }
            const unsigned int i_scanNo = (unsigned int)row[d_scanNo];
            if (i_scanNo > allDatapoints.size() - 1)
            {
                for (size_t i = allDatapoints.size() - 1; i < i_scanNo; i++)
                {
                    allDatapoints.push_back(std::vector<Datapoint>(0));
                }
            }
            const Datapoint F = Datapoint{row[d_mz], row[d_mzError], row[d_RT], i_scanNo, row[d_intensity],
                                          (int)row[d_control_binID], row[d_control_DQScentroid], row[d_control_DQSbin]};

            // if (F.mz > 993.7895 && F.mz < 993.8005) // rm
            // {
            ++lengthAllPoints;
            allDatapoints[i_scanNo].push_back(F); // every subvector in allDatapoints is one complete scan - does not require a sorted input file!
            // }
        }
        for (size_t i = 1; i < allDatapoints.size(); i++) // necessary if features aren't already sorted, performance impact should be very small
        {
            std::sort(allDatapoints[i].begin(), allDatapoints[i].end(), [](const Datapoint lhs, const Datapoint rhs)
                      { return lhs.mz < rhs.mz; });
        }
        std::cout << "Finished reading file in " << time(0) - now << " seconds\n Read " << lengthAllPoints << " datapoints in " << allDatapoints.size() << " scans\n";
        return true;
        // RawData is always a vector of vectors where the first index is the scan number (starting at 1) and every scsn is sorted by mz
    }

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

    void BinContainer::subsetBins(const std::vector<int> dimensions, const unsigned int maxdist, const double massError = -1)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        auto timeEnd = std::chrono::high_resolution_clock::now();
        std::string subsetType;
        std::cout << "\nOpen Bins: 1 - ";

        while (!binDeque.empty()) // while elements are in the bin deque -> if any bin is not fully subset
        {
            for (size_t i = 0; i < dimensions.size(); i++) // dimensions vector allows user to choose order of executing subsetting actions.
            {
                const size_t startpoint = binDeque.size(); // startpoint is the first element of the deque that will not be subset, equivalent to do n times
                subsetCount = 0;
                switch (dimensions[i])
                {
                case mz:
                { // brackets needed to prevent error
                    // bin in mz
                    subsetType = "MZ";
                    for (size_t j = 0; j < startpoint; j++) // for every element in the deque before writing new bins
                    {
                        binDeque.front().makeOS();
                        if (massError < 0) // @todo more efficient solution?
                        {
                            binDeque.front().makeCumError();
                        }
                        else
                        {
                            binDeque.front().makeCumError(massError);
                        }                                                                                                                      // always after order space, since the features get sorted
                        binDeque.front().subsetMZ(&binDeque, binDeque.front().activeOS, 0, binDeque.front().activeOS.size() - 1, subsetCount); // takes element from binDeque, starts subsetting, appends bins to binDeque
                        binDeque.pop_front();                                                                                                  // remove the element that was processed from binDeque
                    }
                    break;
                }

                case scans:
                {
                    // bin using scan numbers
                    subsetType = "Scans";
                    for (size_t j = 0; j < startpoint; j++)
                    {
                        binDeque.front().subsetScan(&binDeque, &finishedBins, maxdist, subsetCount);
                        binDeque.pop_front();
                    }
                    break;
                }

                default:
                    std::cout << "\nSeparation method " << dimensions[i] << " is not a valid parameter, skipping... \n";
                    break;
                }
                timeEnd = std::chrono::high_resolution_clock::now();
                std::cout << "subset in " << subsetType << "\nTime: " << (timeEnd - timeStart).count() << " ns\nSubsets performed: "
                          << subsetCount << "\nClosed Bins: " << finishedBins.size() << "\n--\nOpen Bins: " << binDeque.size() << " - ";
                timeStart = std::chrono::high_resolution_clock::now();
            }
        }
        std::cout << "completed subsetting\n";
    }

    void BinContainer::assignDQSB(const RawData *rawdata, const unsigned int maxdist)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finishedBins[i].makeDQSB(rawdata, maxdist);
        }
        auto timeEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Finished Calculating DQSBs in " << (timeEnd - timeStart).count() << " ns\n";
    }

    void BinContainer::printAllBins(std::string path, const RawData *rawdata)
    {
        std::fstream file_out;
        std::stringstream output;
        file_out.open(path, std::ios::out);
        assert(file_out.is_open());
        // possible symbols
        // char shapes[] = "acGuxnoQzRsTvjIEf"; // random shape when displaying with makie @todo does not work

        output << "mz,scan,ID,DQS,control_ID,control_DQS,control_DQSB\n";

        for (size_t i = 0; i < finishedBins.size(); i++) // @todo make faster
        {
            // char x = shapes[rand() % 17];
            // int colour = i % 9; // 10 colours total @todo printf
            const std::vector<Datapoint *> binnedPoints = finishedBins[i].pointsInBin;

            for (size_t j = 0; j < binnedPoints.size(); j++)
            {
                char buffer[128];
                sprintf(buffer, "%0.15f,%d,%zu,%0.15f,%d,%0.15f,%0.15f\n", binnedPoints[j]->mz, binnedPoints[j]->scanNo, i + 1, finishedBins[i].DQSB[j],
                        binnedPoints[j]->control_binID, binnedPoints[j]->control_DQScentroid, binnedPoints[j]->control_DQSbin);
                output << buffer;
            }
        }
        file_out << output.str();
    }

    void BinContainer::printBinSummary(std::string path)
    {
        std::fstream file_out;
        std::stringstream output;
        file_out.open(path, std::ios::out);
        assert(file_out.is_open());
        output << "ID,size,mean_mz,mean_scans,mean_DQS\n";
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            output << i << "," << finishedBins[i].summariseBin() << "\n";
        }
        file_out << output.str();
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
    Bin::Bin(RawData *rawdata) // @todo relatively time intensive, better solution?; why does rawdata need to be mutable?
    {
        pointsInBin.reserve(rawdata->lengthAllPoints);
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
            activeOS.push_back((pointsInBin[i + 1]->mz - pointsInBin[i]->mz) * 1000000);
        }
        activeOS.push_back(NAN); // last element of OS is never checked, dummy value due to recursive function
    }

    void Bin::makeCumError()
    {
        cumError.reserve(pointsInBin.size());
        cumError.push_back(0); // the first element has a precursor error of 0
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [](Datapoint *F)
                       { return F->mzError * 1000000; });                     // transfer error values from feature objects
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::makeCumError(const double ppm)
    {
        cumError.reserve(pointsInBin.size());
        std::transform(pointsInBin.begin(), pointsInBin.end(), back_inserter(cumError), [ppm](Datapoint *F)
                       { return F->mz * ppm; });
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin());
    }

    void Bin::subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, const int binStartInOS, const int binEndInOS, unsigned int &counter) // bincontainer is binDeque of BinContainer // OS cannot be solved with pointers since index has to be transferred to frature list
    {
        assert(binStartInOS >= 0);
        assert(binEndInOS >= 0);
        const int binsizeInOS = binEndInOS - binStartInOS + 1; // +1 to avoid length zero

        if (binsizeInOS < 5)
        {
            for (int i = 0; i < binsizeInOS; i++)
            {
                Datapoint *F = *(pointsInBin.begin() + binStartInOS + i);
                control_outOfBins.push_back(F);
            }
            return;
        }
        ++counter;
        const auto pmax = std::max_element(OS.begin() + binStartInOS, OS.begin() + binEndInOS);

        const double vcrit = 3.05037165842070 * pow(log(binsizeInOS), (-0.4771864667153)) * // critical value for alpha = 0.01 @todo add functionality for custom alpha?
                             (cumError[binEndInOS + 1] - cumError[binStartInOS]);           // + 1 to binEnd since cumerror starts at 0
        const double max = *pmax * binsizeInOS;                                             // moved binsize here since multiplication is faster than division

        if (max < vcrit) // all values in range are part of one mz bin
        {
            const Bin output(pointsInBin.begin() + binStartInOS, pointsInBin.begin() + binEndInOS + 1); // binEndInOS+1 since the iterator has to point behind the last element to put into the vector
            bincontainer->push_back(output);
            return;
        }
        else
        {
            const int cutpos = std::distance(OS.begin() + binStartInOS, pmax);
            subsetMZ(bincontainer, OS, binStartInOS, binStartInOS + cutpos, counter);
            subsetMZ(bincontainer, OS, binStartInOS + cutpos + 1, binEndInOS, counter);
            return;
        }
    }

    void Bin::subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const unsigned int &maxdist, unsigned int &counter)
    {
        // function is called on a bin sorted by mz
        const size_t binSize = pointsInBin.size();
        double tmp_pt_mzmin = pointsInBin.front()->mz;
        double tmp_pt_mzmax = pointsInBin.back()->mz;
        std::sort(pointsInBin.begin(), pointsInBin.end(), [](const Datapoint *lhs, const Datapoint *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        std::vector<Datapoint *>::iterator newstart = pointsInBin.begin();
        int lastpos = 0;
        for (size_t i = 0; i < binSize - 1; i++) // -1 since difference to next data point is checked @todo rework to start at i = 0
        {
            if (pointsInBin[i + 1]->scanNo - pointsInBin[i]->scanNo > maxdist) // bin needs to be split
            {
                ++counter;
                // less than five features in bin
                if (i - lastpos + 1 < 5) // +1 since i starts at 0
                {
                    for (size_t j = lastpos; j <= i; j++) // @todo checked for correctness
                    {
                        Datapoint *F = *(pointsInBin.begin() + j);
                        control_outOfBins.push_back(F);
                    }
                }
                else
                {
                    // viable bin, stable in scan dimension @todo fixed error of last element being omitted
                    const Bin output(newstart, pointsInBin.begin() + i + 1); // +1 since otherwise last element of the correct range is not included
                    bincontainer->push_back(output);
                }
                lastpos = i + 1;                          // sets previous i to the position one i ahead, since
                newstart = pointsInBin.begin() + lastpos; // for the next split this is the first element
            }
        }
        // check for open bin at the end
        if (lastpos == 0) // no cut has occurred
        {
            bincontainer->front().pt_mzmin = tmp_pt_mzmin;
            bincontainer->front().pt_mzmax = tmp_pt_mzmax;
            bincontainer->front().pt_scanmin = pointsInBin.front()->scanNo;
            bincontainer->front().pt_scanmax = pointsInBin.back()->scanNo;
            finishedBins->push_back(bincontainer->front());
        }
        else if (binSize - lastpos >= 5) // binsize starts at 1 // @todo check for correctness
        {
            // viable bin, stable in scan dimension @todo checked for correctness
            const Bin output(newstart, pointsInBin.end());
            bincontainer->push_back(output);
        }
        else
        {
            for (size_t j = lastpos; j < pointsInBin.size(); j++) // @todo checked for correctness
            {
                Datapoint *F = *(pointsInBin.begin() + j);
                control_outOfBins.push_back(F);
            }
        }
    }

    void Bin::makeDQSB(const RawData *rawdata, const unsigned int &maxdist)
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        const size_t binsize = pointsInBin.size();
        DQSB.reserve(binsize);
        // determine start and end of relevant scan section, used as repeats for the for loop; -1 since accessed vector is zero-indexed
        const unsigned int minScanInner = pointsInBin.front()->scanNo; //
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
        const std::vector<double> meanInnerDistances = meanDistance(pointsInBin);

        // for all scans relevant to the bin
        int needle = 0;     // position in scan where a value was found - starts at 0 for first scan
#pragma GCC diagnostic push // do not display the specific warning for rounding a double to integer
#pragma GCC diagnostic ignored "-Wsign-compare"
        for (int i = scanRangeStart; i < scanRangeEnd; i++)
#pragma GCC diagnostic pop
        {
            bool minFound = false; // only execute search if min or max is present in the scan
            bool maxFound = false;
            const int scansize = rawdata->allDatapoints[i].size() - 1;
            if (scansize == -1)
            {
                minMaxOutPerScan.push_back(NO_MIN_FOUND);
                minMaxOutPerScan.push_back(NO_MAX_FOUND);
                continue;
            }
            else if (needle > scansize)
            {
                needle = scansize; // prevent searching outside of the valid scan region
            }

            if (rawdata->allDatapoints[i][0].mz >= minInnerMZ) 
            {
                minMaxOutPerScan.push_back(NO_MIN_FOUND);
                minFound = true;
                if (rawdata->allDatapoints[i][0].mz > maxInnerMZ)
                {
                    minMaxOutPerScan.push_back(rawdata->allDatapoints[i][0].mz);
                    maxFound = true;
                    continue;
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
                    continue; // @todo work with goto here
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
            double minDist = INFINITY;
            const double currentMZ = pointsInBin[i]->mz;
            const unsigned int currentRangeStart = (pointsInBin[i]->scanNo - minScanInner) * 2; // gives position of the first value in minMaxOutPerScan that must be considered, assuming the first value in minMaxOutPerScan (index 0) is only relevant to the Datapoint in the lowest scan. For every increase in scans, that range starts two elements later
            const unsigned int currentRangeEnd = currentRangeStart + maxdist * 4 + 1;
            for (unsigned int j = currentRangeStart; j <= currentRangeEnd; j++) // from lowest scan to highest scan relevant to feature, +1 since scan no of feature has to be included
            {
                const double dist = std::abs(currentMZ - minMaxOutPerScan[j]);
                if (dist < minDist)
                {
                    minDist = dist;
                    assert(minDist > 0);
                }
            }

            // calculate DQSB on a per-feature basis
            double tmp_DQS = calcDQS(meanInnerDistances[i], minDist);
            // assert(tmp_DQS == pointsInBin[i]->control_DQSbin);
            DQSB.push_back(tmp_DQS);
        }
        // throw; // rm
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
            if (pointsInBin[i]->control_binID != pointsInBin[i + 1]->control_binID) // color 11 for mismatch  mz,rt,ID,color,shape
            {
                std::cout << pointsInBin[i]->mz << "," << pointsInBin[i]->scanNo << "," << binID << ",11,L\n";
            }
        }
        return;
    }

#pragma endregion "Bin"

#pragma region "Functions"

    std::vector<double> meanDistance(const std::vector<Datapoint *> featurelistBin)
    {
        // assumes bin is sorted by mz
        const size_t binsize = featurelistBin.size();
        double totalSum = 0;
        std::vector<double> output(binsize);
        for (size_t i = 0; i < binsize; i++)
        {
            totalSum += featurelistBin[i]->mz;
        }
        double beforeSum = 0;
        for (size_t i = 0; i < binsize; i++)
        {
            const double v1 = (totalSum - featurelistBin[i]->mz * (binsize - i));
            const double v2 = (featurelistBin[i]->mz * i - beforeSum);
            beforeSum += featurelistBin[i]->mz;
            totalSum -= featurelistBin[i]->mz;
            output[i] = (v1 + v2) / (binsize - 1);
        }
        return output;
    }

    inline double calcDQS(const double MID, const double MOD) // mean inner distance, minimum outer distance
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

    inline double calcVcrit(const int binSize, const double errorStart, const double errorEnd)
    {
        const double vcrit = 3.05037165842070 * pow(log(binSize), (-0.4771864667153)) * (errorEnd - errorStart) / binSize;
        return vcrit;
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
    // path to data, mz, centroid error, RT, scan number, intensity, control ID, DQS centroid, control DQS Bin
    testdata.readcsv("../../rawdata/control_bins.csv", 0, 1, 2, 3, 4, 5, 6, 7); // ../../rawdata/control_bins.csv reduced_DQSdiff
    // for (size_t i = 0; i < testdata.scanBreaks.size(); i++)
    // {
    //     std::cout << i << "," << testdata.scanBreaks[i] << "\n";
    // }
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin(&testdata);
    std::vector<int> dim = {q::SubsetMethods::mz, q::SubsetMethods::scans}; // at least one element must be terminator
    testcontainer.subsetBins(dim, 6);                                       // int = max dist in scans; add value after for error in ppm instead of centroid error
    std::cout << "\ncalculating DQSBs...\n";
    testcontainer.assignDQSB(&testdata, 6); // int = max dist in scans
    // return 0;

    // std::ofstream result("../../qbinning_faultybins.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    // std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!

    // std::cout << "mz,rt,ID,color,shape\n";
    // testcontainer.controlAllBins();

    // std::cout.rdbuf(coutbuf); // reset to standard output again

    testcontainer.printAllBins("../../qbinning_binlist.csv", &testdata);
    std::cout << "printed all bins\n";

    // std::fstream file_out;
    // std::stringstream output;
    // file_out.open("../../qbinning_notbinned.csv", std::ios::out);
    // if (!file_out.is_open())
    // {
    //     std::cout << "could not open file\n";
    // }

    // output << "mz,scan,ID,control_ID,control_DQSB\n";
    // for (size_t i = 0; i < q::control_outOfBins.size(); i++) // segfault at 348348
    // {
    //     output << std::setprecision(15) << q::control_outOfBins[i]->mz << "," << q::control_outOfBins[i]->scanNo << ",-1,"
    //            << q::control_outOfBins[i]->control_binID << "," << q::control_outOfBins[i]->control_DQSbin << "\n";
    // }
    // file_out << output.str();
    // testcontainer.printBinSummary("../../qbinning_binsummary.csv");

    std::cout << "\n\nDone!\n\n";
    return 0;
}
