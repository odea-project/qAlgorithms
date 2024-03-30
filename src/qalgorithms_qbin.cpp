#include "../include/qalgorithms_qBin.h"

namespace q
{
    int pt_subsetcount = 0;
    int pt_OSzero = 0;
    int pt_subsets = 0;
    std::vector<double> pt_stepsForDQS;
    std::vector<double> pt_scanRelSteps;
    std::vector<Feature *> pt_outOfBins;

#pragma region "Featurelist"
    FeatureList::FeatureList(int in_numberOfScans)
    {
        numberOfScans = in_numberOfScans;
        scanBreaks = {0, 0}; // two zeros so index 1 is equivalent to scan 1 ßßß superfluous
    }
    FeatureList::~FeatureList() {}
    void FeatureList::readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo, int pt_d_binID)
    {
        allFeatures.resize(numberOfScans, std::vector<Feature *>(0));
        std::ifstream file(user_file);
        if (!file.is_open())
            std::cout << "the file could not be opened\n";

        if (!file.good())
            std::cout << "something is wrong with the input file\n";
        std::string line;
        std::string dummy;
        std::getline(file, dummy); // do not read first row ßßß data is probably not going to be passed with headers
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string cell;
            std::vector<double> row;
            row.reserve(5); // ßßß adjust to input length

            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }
// add conditional here to set feature error to mz* x ppm ßßß
#pragma GCC diagnostic push // do not display the specific warning for rounding a double to integer
#pragma GCC diagnostic ignored "-Wnarrowing"
            int i_scanNo = (int)row[d_scanNo] - 1; // gives warning, conversion functions as intended; -1 so all scans are 0-indexed ßßß control for output
            Feature *F = new Feature{row[d_mz], row[d_mzError], row[d_RT], i_scanNo, row[pt_d_binID]};
#pragma GCC diagnostic pop
            allFeatures[i_scanNo].push_back(F); // every subvector in allFeatures is one complete scan - does not require a sorted input file!
        }
        // ßßß remove if no benefit
        lengthAllFeatures = 0;
        for (size_t i = 0; i < numberOfScans; i++)
        {
            int size = allFeatures[i].size();
            lengthAllFeatures += size;
            allFeatures[i].resize(size);
        }
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
        auto timeStart = std::chrono::high_resolution_clock::now();
        auto timeEnd = std::chrono::high_resolution_clock::now();
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
                        binDeque.front().makeCumError();                                                                          // always after order space, since the features get sorted
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
                    int startpoint = binDeque.size();
                    for (size_t i = 0; i < startpoint; i++)
                    {
                        binDeque.front().subsetScan(&binDeque, &finishedBins, scanDiffLimit);
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

    void BinContainer::assignDQSB(const FeatureList *rawdata, int maxdist)
    {
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            finishedBins[i].makeDQSB(rawdata, maxdist);
        }
    }

    void BinContainer::printAllBins(std::string path, FeatureList *rawdata)
    {
        std::fstream file_out;
        file_out.open(path);
        if (!file_out.is_open())
        {
            throw;
        }
        // possible symbols
        char shapes[] = "acGuxnoQzRsTvjIEf";

        file_out << "mz,rt,ID,color,shape\n";
        // for (size_t i = 0; i < rawdata->allFeatures.size(); i++)
        // {
        //     file_out << std::setprecision(15) << rawdata->allFeatures[i]->mz << "," << rawdata->allFeatures[i]->scanNo << "," << 0 << "\n";
        // }

        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            char x = shapes[rand() % 17];
            int colour = i % 9; // 10 colours total
            std::vector<Feature *> features = finishedBins[i].featurelist;
            for (size_t j = 0; j < features.size(); j++)
            {
                file_out << std::setprecision(15) << features[j]->mz << "," << features[j]->scanNo << "," << i + 1 << "," << colour << "," << x << "\n";
            }
            // file_out << finishedBins[i].featurelist.size() << "\n";
        }
    }

    void BinContainer::printBinSummary(std::string path)
    {
        std::fstream file_out;
        file_out.open(path);
        if (!file_out.is_open())
        {
            std::cout << " ";
        }

        file_out << "mzmin,mzmax,scanmin,scanmax\n";
        for (size_t i = 0; i < finishedBins.size(); i++)
        {
            file_out << finishedBins[i].pt_mzmin << "," << finishedBins[i].pt_mzmax << "," << finishedBins[i].pt_scanmin << "," << finishedBins[i].pt_scanmax << "\n";
        }
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin(const std::vector<Feature *>::iterator &startBin, const std::vector<Feature *>::iterator &endBin) // const std::vector<Feature> &sourceList,
    {
        featurelist = std::vector<Feature *>(startBin, endBin);
    }
    Bin::Bin(FeatureList *rawdata) // relatively time intensive, better solution?
    {
        featurelist.reserve(rawdata->lengthAllFeatures);
        for (size_t i = 0; i < rawdata->numberOfScans; i++)
        {
            featurelist.insert(featurelist.end(), rawdata->allFeatures[i].begin(), rawdata->allFeatures[i].end());
            // std::copy(rawdata->allFeatures[i].begin(), rawdata->allFeatures[i].end(), featurelist.end());
            // featurelist.insert( // move vector
            //     featurelist.end(),
            //     std::make_move_iterator(rawdata->allFeatures[i].begin()),
            //     std::make_move_iterator(rawdata->allFeatures[i].end()));
        }
    }
    Bin::~Bin() {}

    void Bin::makeOS()
    {
        // sort features by mz
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->mz < rhs->mz; });

        activeOS.reserve(featurelist.size());            // OS = Order Space
        for (size_t i = 0; i + 1 < activeOS.size(); ++i) // +1 to prevent accessing outside of vector
        {
            activeOS[i] = featurelist[i + 1]->mz - featurelist[i]->mz;
            // if (activeOS[i] == 0)
            // {
            //     ++OSzero;
            // }
        }
        // std::cout << OSzero; // there are two pairs of identical masses in the dataset
        activeOS.back() = -225; // -225 to denote last element in OS
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
        ++pt_subsetcount;
        const int n = endBin - startBin + 1; // +1 to avoid lenth zero
        if (n < 5)
        {
            if (n == 1)
            {
                Feature *F = *(featurelist.begin() + startBin);
                pt_outOfBins.push_back(F);
            }
            else
            {
                for (size_t i = 0; i < n; i++)
                {
                    Feature *F = *(featurelist.begin() + startBin + i);
                    pt_outOfBins.push_back(F);
                }

                // pt_outOfBins.insert(pt_outOfBins.end(), featurelist.begin() + startBin, featurelist.begin() + endBin + 1); // ßßß place discarded features in separate table
            }
            return;
        }

        auto pmax = std::max_element(OS.begin() + startBin, OS.begin() + endBin); // ´ßßß -1 to not include maximum at which the previous cut occurred

        double vcrit = 3.05037165842070 * pow(log(n), (-0.4771864667153)) * (cumError[endBin] - cumError[startBin]) / (n - 1); // critical value for alpha = 0.01. ßßß add functionality for custom alpha?
        double max = *pmax;
        if (max < vcrit) // all values in range are part of one mz bin
        {
            // make bin
            // std::cout << startBin << "," << endBin << "," << subsetcount << "\n"; // not all that useful, since knowledge of position in original order space is always lost
            pt_subsetcount = 0;
            const Bin output(featurelist.begin() + startBin, featurelist.begin() + endBin + 1); // endBin+1 since the iterator has to point behind the last element to put into the vector
            // append Bin to bin container
            bincontainer->push_back(output);
            return;
        }
        else
        {
            int cutpos = std::distance(OS.begin() + startBin, pmax);
            subsetMZ(bincontainer, OS, startBin, startBin + cutpos);
            subsetMZ(bincontainer, OS, startBin + cutpos + 1, endBin);
            return;
        }
    }

    void Bin::subsetScan(std::deque<Bin> *bincontainer, std::vector<Bin> *finishedBins, const int &maxdist)
    {
        // sort
        int n = featurelist.size();
        double tmp_pt_mzmin = featurelist.front()->mz;
        double tmp_pt_mzmax = featurelist.back()->mz;
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->scanNo < rhs->scanNo; });
        std::vector<Feature *>::iterator newstart = featurelist.begin();
        int lastpos = 0;
        for (size_t i = 1; i < n; i++)
        {
            if (featurelist[i]->scanNo - featurelist[i - 1]->scanNo > maxdist) // bin needs to be split
            {
                // less than five features in bin
                if (i - lastpos - 1 < 5)
                {
                    pt_outOfBins.insert(pt_outOfBins.end(), newstart, featurelist.begin() + i - 1); // ßßß place discarded features in separate table
                    newstart = featurelist.begin() + i;                                             // new start is one behind current i
                }
                else
                {
                    // no bins of size 5 are ever produced when splitting by scans - ßßß try reduced maxdist
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
            bincontainer->front().pt_mzmin = tmp_pt_mzmin;
            bincontainer->front().pt_mzmax = tmp_pt_mzmax;
            bincontainer->front().pt_scanmin = featurelist.front()->scanNo;
            bincontainer->front().pt_scanmax = featurelist.back()->scanNo;
            bincontainer->front().pt_subsetcount = pt_subsets; // ßßß rm
            finishedBins->push_back(bincontainer->front());
        }
        else if (n + 1 - lastpos > 5)
        {
            // viable bin, stable in scan dimension
            Bin output(newstart, featurelist.end());
            bincontainer->push_back(output);
        }
        else
        {
            pt_outOfBins.insert(pt_outOfBins.end(), newstart, featurelist.end()); // ßßß place discarded features in separate table
        }
    }

    void Bin::makeDQSB(const FeatureList *rawdata, const int &maxdist)
    {
        // assumes bin is saved sorted by scans, since the result from scan gap checks is the final control
        const int n = featurelist.size();
        DQSB.reserve(n);
        // determine start and end of relevant scan section, used as repeats for the for loop; -1 since accessed vector is zero-indexed
        int minScanNo = featurelist.front()->scanNo - maxdist - 1;
        int maxScanNo = featurelist.back()->scanNo + maxdist;
        std::vector<double> minMaxOuter; // contains both values (per scan) next or closest to all m/z in the bin
        minMaxOuter.reserve((n + 2 * maxdist) * 2);
        int maxScansReduced = 0;
        if (minScanNo < 0)
        {
            minScanNo = 0;
        }
        else if (maxScanNo > rawdata->allFeatures.size())
        {
            maxScansReduced = maxScanNo - rawdata->allFeatures.size(); // dummy values have to be added later
            maxScanNo = rawdata->allFeatures.size();
        }
        // determine min and max in mz - sort, since then calculation of inner distances is significantly faster
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->mz < rhs->mz; });
        const int minInnerMZ = featurelist.front()->mz;
        const int maxInnerMZ = featurelist.back()->mz;
        std::vector<double> meanInnerDistances = meanDistance(featurelist);

        // for all scans relevant to the bin
        int accessPoint = 0; // position in scan where a value was found - starts at 0 for first scan
        for (size_t i = minScanNo; i < maxScanNo; i++)
        {
            bool minFound = false; // only execute search if min or max is present in the scan
            bool maxFound = false;
            int scansize = rawdata->allFeatures[i].size() - 1; // -1 since 0-indexed, last element of scan
            if (accessPoint > scansize)
                accessPoint = scansize; // prevent searching outside of the valid scan region

            int minMaxFound = 0; // only advance to next scan once both min and max have been found
            // check begin of bin for possible segfault
            if (rawdata->allFeatures[i][0]->mz > minInnerMZ)
            {
                minMaxOuter.push_back(-225);
                minFound = true;
                if (rawdata->allFeatures[i][0]->mz > maxInnerMZ)
                {
                    minMaxOuter.push_back(rawdata->allFeatures[i][0]->mz);
                    maxFound = true;
                }
            }
            // check end of bin
            if (rawdata->allFeatures[i][scansize]->mz < maxInnerMZ)
            {
                minMaxOuter.push_back(-225);
                maxFound = true;
                if (rawdata->allFeatures[i][scansize]->mz < minInnerMZ)
                {
                    minMaxOuter.push_back(rawdata->allFeatures[i][scansize]->mz);
                    minFound = true;
                }
            }
            // ßßß consider starting at a place other than the first element of a scan, zB (minMZ/maxMZ(scan)) * number of features in scan
            if (!minFound)
            {
                while (rawdata->allFeatures[i][accessPoint]->mz < minInnerMZ)
                {
                    ++accessPoint; // steps through the dataset and increments until accessPoint is the first value >= minInnerMZ
                }
                while (rawdata->allFeatures[i][accessPoint]->mz > minInnerMZ)
                {
                    --accessPoint; // steps through the dataset and decrements until accessPoint is the desired mz value
                }
                minMaxOuter.push_back(rawdata->allFeatures[i][accessPoint]->mz);
            }

            if (!maxFound)
            {
                while (rawdata->allFeatures[i][accessPoint]->mz > maxInnerMZ)
                {
                    --accessPoint;
                }
                while (rawdata->allFeatures[i][accessPoint]->mz < maxInnerMZ)
                {
                    ++accessPoint;
                }
                minMaxOuter.push_back(rawdata->allFeatures[i][accessPoint]->mz);
            }
        }
        // minMaxOuter contains the relevant distances in order of scans, with both min and max per scan being stored for comparison
        if (maxScansReduced != 0)
        {
            for (size_t i = 0; i < 2 * maxScansReduced; i++)
            {
                minMaxOuter.push_back(-225);
            }
        }

        // find min distance in minMaxOuter, assign to minOuterDistances in order of the featurelist
        for (size_t i = 0; i < n; i++)
        {
            double minDist = 225;
            double currentMZ = featurelist[i]->mz;
            int currentRange = featurelist[i]->scanNo - minScanNo;  // minScanNo already includes -maxdist necessary for complete range ßßß does not work
            for (size_t j = currentRange; j < maxdist * 2 + 1; j++) // from lowest scan to highest scan relevant to feature, +1 since scan no of feature has to be included
            {
                double dist = abs(currentMZ - minMaxOuter[j]);
                if (dist < minDist)
                    minDist = dist;
            }
            // calculate DQSB on a per-feature basis
            DQSB[i] = calcDQS(meanInnerDistances[i], minDist);
        }
    }

    std::string Bin::summarisePerf()
    {
        // ID, size, mean(mz), mz range, mean(scans), scan range, mean(DQS), subsets, time (DQSB), \n
        std::stringstream buffer;
        int n = featurelist.size();
        double meanMZ = 0;
        double meanScan = 0;
        double meanDQS = 0;
        for (size_t i = 0; i < n; i++)
        {
            meanMZ += featurelist[i]->mz;
            meanScan += featurelist[i]->scanNo;
            meanDQS += DQSB[i];
        }
        std::sort(featurelist.begin(), featurelist.end(), [](const Feature *lhs, const Feature *rhs)
                  { return lhs->mz < rhs->mz; });
        meanMZ = meanMZ / n;
        meanScan = meanScan / n;
        meanDQS = meanDQS / n;
        buffer << n << "," << meanMZ << "," << pt_mzmin << "," << meanScan << "," << pt_scanmin << "," << meanDQS << "," << pt_subsetcount << "," << pt_MakeDQSB;
        std::string output = buffer.str();
        return output;
    }

#pragma endregion "Bin"

#pragma region "Functions"

    std::vector<double> meanDistance(std::vector<Feature *> featurelistBin)
    {
        // assumes bin is sorted by mz
        int n = featurelistBin.size();
        double totalSum = 0;
        std::vector<double> output(n);
        for (size_t i = 0; i < n; i++)
        {
            totalSum = +featurelistBin[i]->mz;
        }
        double beforeSum = 0;
        for (size_t i = 0; i < n; i++)
        {
            double v1 = (totalSum - featurelistBin[i]->mz * (n - i));
            double v2 = (featurelistBin[i]->mz * i - beforeSum);
            beforeSum += featurelistBin[i]->mz;
            totalSum -= featurelistBin[i]->mz;
            output[i] = (v1 + v1) / (n - 1);
        }
        return output;
    }

    double calcDQS(double MID, double MOD) // mean inner distance, minimum outer distance
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

#pragma endregion "Functions"

}

int main()
{
    std::cout << "starting...\n";
    // std::ofstream result("../../qbinning_ßßß.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    // std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!
    // std::cout << "scan,position\n";

    q::FeatureList testdata(2533);                                                              // 2533 scans in Warburg dataset
    testdata.readcsv("../../rawdata/qBinnings_Warburg_pos_171123_01_Zu_01.csv", 0, 4, 1, 2, 7); // ../../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv ../test/test.csv
    // for (size_t i = 0; i < testdata.scanBreaks.size(); i++)
    // {
    //     std::cout << i << "," << testdata.scanBreaks[i] << "\n";
    // }
    q::pt_outOfBins;
    std::cout << "created feature list with scan index\n";
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin(&testdata);
    std::vector<int> dim = {1, 2};    // last element must be the number of scans ßßß implement outside of the switch statement ßßß endless loop if scan terminator is not included
    testcontainer.subsetBins(dim, 7); // 7 = max dist in scans
    std::cout << "\ncalculating DQSBs...\n";

    // std::ofstream result("../../qbinning_binposition.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    // std::cout.rdbuf(result.rdbuf());             // redirect std::cout to out.txt!
    // testcontainer.assignDQSB(&testdata, 7);

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
        q::Feature *F = q::pt_outOfBins[i];
        file_out << std::setprecision(15) << q::pt_outOfBins[i]->mz << "," << q::pt_outOfBins[i]->scanNo << "," << -1 << "," << -1 << ","
                 << "Y\n";
    }
    // std::vector<double> ratio;
    // int oneCount = 0;
    // int otherCount = 0;
    // int meanSteps = 0;
    // for (size_t i = 0; i < q::pt_stepsForDQS.size(); i++)
    // {
    //     if (q::pt_stepsForDQS[i] == 1)
    //     {
    //         ++oneCount;
    //     }
    //     else
    //     {
    //         ++otherCount;
    //         ratio.push_back(q::pt_stepsForDQS[i] / q::pt_scanRelSteps[i]);
    //         meanSteps += q::pt_stepsForDQS[i];
    //     }
    // }
    // meanSteps = meanSteps / otherCount;
    // std::cout << oneCount << ", " << meanSteps;

    // std::cout.rdbuf(coutbuf); // reset to standard output again
    std::cout << "\n\nDone!\n\n";
    // testcontainer.printBinSummary("G:\\_Studium\\Analytik-Praktikum\\qbinning\\qAlgorithms\\binstats.csv");
    return 0;
}
