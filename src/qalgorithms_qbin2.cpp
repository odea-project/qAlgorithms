#include "../include/qalgorithms_qBin2.h"

namespace q
{
#pragma region "Featurelist"
    FeatureList::FeatureList() {}
    FeatureList::~FeatureList() {}
    void FeatureList::readcsv(std::string user_file, int d_mz, int d_mzError, int d_RT, int d_scanNo)
    {
        std::ifstream file(user_file);
        std::string line;
        std::string dummy;
        std::getline(file, dummy); // do not reas first row ßßß data is probably not going to be passed with headers
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string cell;
            std::vector<double> row;

            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
            Feature F{row[d_mz], row[d_mzError], row[d_RT], round(row[d_scanNo])}; // round gives warning, conversion functions as intended
#pragma GCC diagnostic pop
            allFeatures.push_back(F);
        }
    }
#pragma endregion "Featurelist"

#pragma region "BinContainer"

    BinContainer::BinContainer() {}
    BinContainer::~BinContainer() {}

    void BinContainer::makeFirstBin(FeatureList rawdata)
    {
        Bin firstBin(rawdata);
        binDeque.push_back(firstBin);
    }

    void BinContainer::subsetBins(std::vector<int> dimensions)
    {
        // while
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
                    binDeque.front().makeCumError();                                                               // always after order space, since the features get sorted
                    binDeque.front().subsetMZ(binDeque, binDeque.front().activeOS, 0, binDeque.front().activeOS.size() - 1); // takes element from binDeque, starts subsetting, appends bins to binDeque
                    binDeque.pop_front();                                                                          // remove the element that was processed from binDeque
                    // ßßß bad solution, needless copy if bin is ok - only relevant for controlling in rt, so should be fine here
                }

                break;
            }

            case 2:
            {
                // bin using scan numbers
                int startpoint = binDeque.size();
                for (size_t i = 0; i < startpoint; i++)
                {
                    binDeque.front().subsetScan(binDeque, 5);
                    binDeque.pop_front();
                }

                break;
            }

            default:
                std::cout << "\nSeparation method " << dimensions[i] << " is not a valid parameter, skipping... \n";
                break;
            }
        }
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin(const std::vector<Feature>::iterator &startBin, const std::vector<Feature>::iterator &endBin) // const std::vector<Feature> &sourceList,
    {
        std::copy(startBin, endBin, featurelist.begin());
    }
    Bin::Bin(FeatureList rawdata)
    {
        featurelist = rawdata.allFeatures; // pointers ßßß
    }
    Bin::~Bin() {}

    void Bin::makeOS()
    {
        // sort features by mz
        std::sort(featurelist.begin(), featurelist.end(), [](const auto &lhs, const auto &rhs)
                  { return lhs.mz < rhs.mz; });

        activeOS.resize(featurelist.size());             // OS = Order Space
        for (size_t i = 0; i + 1 < activeOS.size(); i++) // +1 to prevent accessing outside of vector
        {
            activeOS[i] = featurelist[i + 1].mz - featurelist[i].mz;
        }
        activeOS.back() = -225; // -225 to signify last element in OS
    }

    void Bin::makeCumError()
    {
        cumError.reserve(featurelist.size());
        std::transform(featurelist.begin(), featurelist.end(), back_inserter(cumError), [](Feature f)
                       { return f.mzError; });                                // transfer error values from feature objects
        std::partial_sum(cumError.begin(), cumError.end(), cumError.begin()); // cumulative sum
    }

    void Bin::subsetMZ(std::deque<Bin> bincontainer, const std::vector<double> &OS, int startBin, int endBin) // bincontainer is binDeque of BinContainer // OS cannot be solved with pointers since index has to be transferred to frature list
    {
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
            Bin output(featurelist.begin() + startBin, featurelist.begin() + endBin);
            // append Bin to bin container
            bincontainer.push_back(output);
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

    void Bin::subsetScan(std::deque<Bin> bincontainer, const int &maxdist)
    {
        // sort
        int n = featurelist.size();
        std::sort(featurelist.begin(), featurelist.end(), [](const auto &lhs, const auto &rhs)
                  { return lhs.scanNo < rhs.scanNo; });
        if (featurelist.front().scanNo + n + maxdist > featurelist.back().scanNo) // sum of all gaps is less than max distance, +n since no element will occur twice in one scan
        {
            bincontainer.push_back(bincontainer.front()); // moves first element of que to last, since bin is good
        }
        else
        {
            std::vector<Feature>::iterator newstart = featurelist.begin();
            // std::vector<Feature>::iterator newend = featurelist.begin();
            int previ = 0;
            for (size_t i = 1; i < n; i++)
            {
                if (featurelist[i].scanNo - featurelist[i - 1].scanNo > maxdist) // bin needs to be split
                {
                    // less than five features in bin
                    if (i - previ - 1 < 5)
                    {
                        newstart = featurelist.begin() + i; // new start is one behind current i
                    }
                    else
                    {
                        // viable bin, stable in scan dimension
                        Bin output(newstart, featurelist.begin() + i - 1);
                        bincontainer.push_back(output);
                    }
                    previ = i; // sets previ to the position one i ahead, since current value is i-1
                }
            }
            // check for open bin at the end
            if (n + 1 - previ > 5)
            {
                // viable bin, stable in scan dimension
                Bin output(newstart, featurelist.end());
                bincontainer.push_back(output);
            }
        }

        // iterate, compare with previous for gap
        // do split
        // if prev - push back, pop front
    }

#pragma endregion "Bin"
}

int main()
{
    // std::ofstream result("../../qbinning_results.csv");
    // std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    // std::cout.rdbuf(result.rdbuf()); //redirect std::cout to out.txt!
    q::FeatureList testdata;
    testdata.readcsv("../../rawdata/qCentroid_Warburg_pos_171123_01_Zu_01.csv", 0, 7, 5, 6);
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin(testdata);
    std::vector<int> dim = {1};
    testcontainer.subsetBins(dim);
    // std::cout.rdbuf(coutbuf); //reset to standard output again
    return 0;
}
