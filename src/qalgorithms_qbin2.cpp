#include "../include/qalgorithms_qBin2.h"

namespace q
{
    int subsetcount = 0;
    int perfectbins = 0;
    int OSzero = 0;
#pragma region "Featurelist"
    FeatureList::FeatureList()
    {
        scanBreaks = {0, 0}; // two zeros so index 1 is equivalent to scan 1
    }
    FeatureList::~FeatureList() {}
    void FeatureList::readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo)
    {
        std::ifstream file(user_file);

        if (!file.good())
            std::cout << "something is wrong with the input file\n";
        std::string line;
        std::string dummy;
        std::getline(file, dummy); // do not read first row ßßß data is probably not going to be passed with headers
        int linecounter = 0;
        int prevScan = 1;
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string cell;
            std::vector<double> row;

            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }
#pragma GCC diagnostic push // do not display the specific warning for rounding a double to integer
#pragma GCC diagnostic ignored "-Wnarrowing"
            int i_scanNo = round(row[d_scanNo]); // round gives warning, conversion functions as intended
#pragma GCC diagnostic pop
            Feature *F = new Feature{row[d_mz], row[d_mzError], row[d_RT], i_scanNo};
            allFeatures.push_back(F);
            // check if scan changed from previous line
            if (i_scanNo != prevScan)
            {
                prevScan = i_scanNo;
                scanBreaks.push_back(linecounter); // scan == 1 from scanBreaks[0] to scanBreaks[1]-1 etc.
            }

            ++linecounter;
        }
        scanBreaks.push_back(linecounter); // last entry is size of the dataset
    }
#pragma endregion "Featurelist"

#pragma region "BinContainer"

    BinContainer::BinContainer() {}
    BinContainer::~BinContainer() {}

    void BinContainer::makeFirstBin(FeatureList *rawdata)
    {
        const Bin firstBin(rawdata);
        binDeque.push_back(firstBin);
    }

    void BinContainer::subsetBins(std::vector<int> dimensions, int scanDiffLimit)
    {
        std::cout << "\nTotal bins: 1";
        // while elements are in the bin deque -> if any bin is not fully subset
        while (!binDeque.empty())
        {
            for (size_t i = 0; i < dimensions.size(); i++) // dimensions vector allows user to choose order of executing subsetting actions. It is also possible to only subset in mz or RT
            {
                switch (dimensions[i])
                {
                case 1:
                { // brackets needed to prevent error
                    // bin in mz
                    int startpoint = binDeque.size(); // startpoint is the first element of the deque that will not be subset, equivalent to do n times
                    for (size_t i = 0; i < startpoint; i++)
                    { // random access not needed, first element -> make bins -> move element or make bins, then remove first entry ; do n times
                        binDeque.front().makeOS();
                        std::cout << "\nIdentical Masses in Featurelist: " << OSzero; // two pairs of identical masses
                        binDeque.front().makeCumError();                                                                          // always after order space, since the features get sorted
                        binDeque.front().subsetMZ(&binDeque, binDeque.front().activeOS, 0, binDeque.front().activeOS.size() - 1); // takes element from binDeque, starts subsetting, appends bins to binDeque
                        binDeque.pop_front();                                                                                     // remove the element that was processed from binDeque
                    }
                    
                    std::cout << "\nmz subsetting done\nTotal bins: " << binDeque.size();
                    break;
                }

                case 2:
                {
                    // bin using scan numbers
                    int startpoint = binDeque.size();
                    for (size_t i = 0; i < startpoint; i++)
                    {
                        binDeque.front().subsetScan(&binDeque, &finishedBins, scanDiffLimit);
                        binDeque.pop_front();
                    }
                    std::cout << "\nClosed bins: " << finishedBins.size() << " perfect bins: " << perfectbins << "\nscan subsetting done\nTotal bins: " << binDeque.size();
                    perfectbins = 0;
                    break;
                }

                default:
                    std::cout << "\nSeparation method " << dimensions[i] << " is not a valid parameter, skipping... \n";
                    break;
                }
            }
        }
    }

    void BinContainer::assignDQSB(const FeatureList *rawdata, int maxdist)
    {
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finishedBins[i].makeDQSB(rawdata, maxdist);
        }
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin(const std::vector<Feature *>::iterator &startBin, const std::vector<Feature *>::iterator &endBin) // const std::vector<Feature> &sourceList,
    {
        featurelist = std::vector<Feature *>(startBin, endBin);
    }
    Bin::Bin(FeatureList *rawdata)
    {
        featurelist = rawdata->allFeatures; // pointers ßßß
    }
    Bin::~Bin() {}

    void Bin::makeOS()
    {
        // sort features by mz
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->mz < rhs->mz; });

        activeOS.resize(featurelist.size());             // OS = Order Space
        for (size_t i = 0; i + 1 < activeOS.size(); ++i) // +1 to prevent accessing outside of vector
        {
            activeOS[i] = featurelist[i + 1]->mz - featurelist[i]->mz;
            if (activeOS[i] == 0)
            {
                ++OSzero;
            }
            
        }
        activeOS.back() = -225; // -225 to signify last element in OS
    }

    void Bin::makeCumError()
    {
        cumError.reserve(featurelist.size());
        std::transform(featurelist.begin(), featurelist.end(), back_inserter(cumError), [](Feature *F)
                       { return F->mzError; });                               // transfer error values from feature objects
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::subsetMZ(std::deque<Bin> *bincontainer, const std::vector<double> &OS, int startBin, int endBin) // bincontainer is binDeque of BinContainer // OS cannot be solved with pointers since index has to be transferred to frature list
    {
        ++subsetcount;
        // std::cout << subsetcount << " ";
        const int n = endBin - startBin; // size is equal to n+1
        if (n < 5)
        {
            return;
        }

        auto pmax = std::max_element(OS.begin() + startBin, OS.begin() + endBin - 1); // -1 to not include maximum at which the previous cut occurred

        double vcrit = 3.05037165842070 * pow(log(n + 1), (-0.4771864667153)) * (cumError[endBin] - cumError[startBin]) / n; // critical value for alpha = 0.01. ßßß add functionality for custom alpha?
        double max = *pmax;
        if (max < vcrit) // all values in range are part of one mz bin
        {
            // make bin
            // std::cout << startBin << "," << endBin << "," << subsetcount << "\n"; // not all that useful, since knowledge of position in original order space is always lost
            subsetcount = 0;
            const Bin output(featurelist.begin() + startBin, featurelist.begin() + endBin);
            // append Bin to bin container
            bincontainer->push_back(output);
            return;
        }
        else
        {
            int cutpos = std::distance(OS.begin() + startBin, pmax);
            subsetMZ(bincontainer, OS, startBin, startBin + cutpos);
            subsetMZ(bincontainer, OS, startBin + cutpos + 1, endBin);
            // multithreading queue producer consumers
            // multithreading probably not a good solution, since the objects need to be copied -> copies all mz for every bin ßßß wrong
            return;
        }
    }

    void Bin::subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int &maxdist)
    {
        // sort
        int n = featurelist.size();
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        if (featurelist.front()->scanNo + n + maxdist > featurelist.back()->scanNo) // sum of all gaps is less than max distance, +n since no element will occur twice in one scan
        {
            ++perfectbins;
            finishedBins->push_back(bincontainer->front()); // moves first element of que to vector of complete bins, since bin is good
        }
        else
        {
            std::vector<Feature *>::iterator newstart = featurelist.begin();
            // std::vector<Feature>::iterator newend = featurelist.begin();
            int lastpos = 0;
            for (size_t i = 1; i < n; i++)
            {
                if (featurelist[i]->scanNo - featurelist[i - 1]->scanNo > maxdist) // bin needs to be split
                {
                    // less than five features in bin
                    if (i - lastpos - 1 < 5)
                    {
                        newstart = featurelist.begin() + i; // new start is one behind current i
                    }
                    else
                    {
                        // no bins of size 5 are ever produced when splitting by scans
                        // viable bin, stable in scan dimension
                        Bin output(newstart, featurelist.begin() + i - 1);
                        bincontainer->push_back(output);
                        newstart = featurelist.begin() + i;
                    }
                    lastpos = i; // sets previ to the position one i ahead, since current value is i-1
                }
            }
            // check for open bin at the end
            if (lastpos == 0) // no cut has occurred
            {
                finishedBins->push_back(bincontainer->front());
            }
            else if (n + 1 - lastpos > 5)
            {
                // viable bin, stable in scan dimension
                Bin output(newstart, featurelist.end());
                bincontainer->push_back(output);
            }
            // test condition for the bin being identical to the previous one
        }

        // iterate, compare with previous for gap
        // do split
        // if prev - push back, pop front
    }

    double Bin::findOuterMinmax(std::vector<Feature *>::const_iterator position, const double &innerMinmax, bool direction)
    { // direction TRUE = forward, direction FALSE = backwards
    if (innerMinmax == 110.131907489017) // same exact mass exists II
    {
        std::cout << "\n";
    }
    
        Feature *F = *position; // position is a null pointer
        if (direction)
        {
            if (F->mz > innerMinmax)
            {
                return -225; // there exists no feature in the scan with an mz which is outside the bin at the lower end. Sets to -225 so it is never considered for the smallest distance
            }
            else
            {
                double closestMZ = F->mz;
                while (F->mz < innerMinmax)
                {
                    closestMZ = F->mz;
                    ++position;
                    F = *position;
                }
                return closestMZ;
            }
        }
        else
        {
            if (F->mz < innerMinmax)
            {
                return -225; // as above, but upper end
            }
            else
            {
                double closestMZ = F->mz;
                while (F->mz > innerMinmax)
                {
                    closestMZ = F->mz;
                    --position;
                    F = *position;
                }
                return closestMZ;
            }
        }
    }

    void Bin::makeDQSB(const FeatureList *rawdata, const int &maxdist)
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        // featurelist is sorted by scans and by mz within scans (sort_stable for taking unsorted data)
        // only the minimum scans - k and the maximum scans + k need to be checked
        // if a value m/z is not lower than the minimum of mz or larger than the maximum while being in the allowed scan interval, it is by definition included in the bin
        // check first scan from min and max of bin, next from same position in next scan (add start of scan to both iterators, move inwards if larger than min/max or outwards otherwise)
        int n = featurelist.size();
        DQSB.reserve(n);
        int minScanNo = featurelist.front()->scanNo - maxdist;
        if (minScanNo < 1)
        {
            minScanNo = 1;
        }
        int maxScanNo = featurelist.back()->scanNo + maxdist;
        if (maxScanNo > rawdata->scanBreaks.size())
        {
            maxScanNo = rawdata->scanBreaks.size();
        }
        // sort featurelist by mz
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->mz < rhs->mz; });
        double inMin = featurelist.front()->mz;
        double inMax = featurelist.back()->mz;

        std::vector<Feature *>::const_iterator goToMinOut;
        std::vector<Feature *>::const_iterator goToMaxOut;
        std::vector<double> compspace; // even index: minOut, odd index: maxOut

        if (n == 64)
        {
            std::cout << inMin <<"," << inMax << "\n"; // exception at the second bin of size 64 (segfault)
        }
        

        for (int i = minScanNo; i < maxScanNo; i++) // iterate over all scans viable for entire bin
        {
            goToMinOut = rawdata->allFeatures.begin() + rawdata->scanBreaks[i];         // start of the currently cheked scan
            goToMaxOut = rawdata->allFeatures.begin() + rawdata->scanBreaks[i + 1] - 1; // end of the currently checked scan
            double minOut = findOuterMinmax(goToMinOut, inMin, true);
            double maxOut = findOuterMinmax(goToMaxOut, inMax, false);
            compspace.push_back(minOut);
            compspace.push_back(maxOut);
        }

        // create vector of mean distance to other features
        std::vector<double> meanDist(n);
        for (size_t i = 0; i < n - 1; i++)
        {
            for (int j = i + 1; j < n; ++j)
            {
                double dist = abs(featurelist[i]->mz - featurelist[j]->mz);
                meanDist[i] = +dist;
                meanDist[j] = +dist;
            }
            meanDist[i] == meanDist[i] / (n - 1);
        }

        // find closest match in mz for min and max mz
        std::vector<double> minOuter(n); // contains lowest elegible distance for every point
        int scanMin;
        int scanMax;
        double lowestDist;
        double dist;
        for (size_t i = 0; i < n; i++)
        {
            lowestDist = 225;

            for (size_t j = 2 * i; j < 2 * i + 4 * maxdist + 1; j++) // 2*i since every two elements is one new scan. From this i, the range is traversed until two max distances (*2 per scan). +1 to include both elements of the last scan
            {
                dist = std::abs(featurelist[i]->mz - compspace[j]);
                if (dist < lowestDist)
                {
                    lowestDist = dist;
                }
            }
            minOuter[i] = lowestDist; // minOuter redundant ßßß
            DQSB.push_back(calcDQS(meanDist[i], minOuter[i]));
        }
    }

    double Bin::calcDQS(double MID, double MOD) // mean inner distance, minimum outer distance
    {
        double dqs = MID;
        if (dqs < MOD)
        {
            dqs = MOD;
        }
        dqs = (MID - MOD) * 0.5 * (1 + 1 / (1 + MID)) / dqs; // sm(i) term
        dqs = (dqs + 1) / 2;                                 // interval transform
        return dqs;
    }

#pragma endregion "Bin"
}

int main()
{
    std::cout << "starting...\n";
    // std::ofstream result("../../qbinning_ßßß.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    // std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!
    // std::cout << "scan,position\n";

    q::FeatureList testdata;
    testdata.readcsv("../../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv", 0, 7, 5, 6); // ../../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv ../test/test.csv
    // for (size_t i = 0; i < testdata.scanBreaks.size(); i++)
    // {
    //     std::cout << i << "," << testdata.scanBreaks[i] << "\n";
    // }

    std::cout << "created feature list with scan index\n";
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin(&testdata);
    std::vector<int> dim = {1, 2};    // last element must be the number of scans ßßß implement outside of the switch statement ßßß endless loop if scan terminator is not included
    testcontainer.subsetBins(dim, 7); // 7 = max dist in scans

    testcontainer.assignDQSB(&testdata, 7);

    // std::cout.rdbuf(coutbuf); // reset to standard output again
    std::cout << "\n\nDone!\n\n";
    return 0;
}
