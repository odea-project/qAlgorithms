#include "../include/qalgorithms_qBin2.h"

namespace q
{
#pragma region "BinContainer"

    BinContainer::BinContainer() {}
    BinContainer::~BinContainer() {}

    void BinContainer::makeFirstBin(std::string input_file)
    {
        std::vector<Feature> features;
        readcsv(input_file, features, 0, 7, 5, 6); // ßßß replace hard-coded cols with flexible input
        Bin firstBin(features.begin(), features.end(), 0);
        binDeque.push_back(firstBin);
    }

    void BinContainer::readcsv(std::string user_file, std::vector<Feature> output, int d_mz, int d_mzError, int d_RT, int d_scanNo)
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
            Feature F;
            F.mz = row[d_mz];
            F.mzError = row[d_mzError];
            F.RT = row[d_RT];
            F.scanNo = round(row[d_scanNo]); // should work without issue
            output.push_back(F);
        }
    }

    void BinContainer::subsetBins(std::vector<int> dimensions)
    {
        for (size_t i = 0; i < dimensions.size(); i++) // dimensions vector allows user to choose order of executing subsetting actions. It is also possible to only subset in mz or RT
        {
            switch (dimensions[i])
            {
            case 1:
            { // brackets needed to prevent error
                // bin in mz
                int startpoint = binDeque.size();
                for (size_t i = 0; i < startpoint; i++) // random access not needed, first element -> make bins -> move element or make bins, then remove first entry ; do n times
                {
                    binDeque[i].makeOS();
                    binDeque[i].makeCumError();
                    binDeque[i].subsetMZ(binDeque, binDeque[i].activeOS, 0, binDeque[i].activeOS.size() - 1); // takes element from binDeque, starts subsetting, appends bins to binDeque
                    binDeque.pop_front();                                                                     // remove the element that was processed from binDeque
                    // ßßß bad solution, needless copy if bin is ok - only relevant for controlling in rt, so should be fine here
                }

                break;
            }

            case 2:
                /* code */
                break;

            default:
                std::cout << "\nSeparation method " << dimensions[i] << " is not a valid parameter, skipping... \n";
                break;
            }
        }
    }

#pragma endregion "BinContainer"

#pragma region "Bin"

    Bin::Bin(const std::vector<Feature>::iterator &startBin, const std::vector<Feature>::iterator &endBin, const double &max) // const std::vector<Feature> &sourceList,
    {
        prevmax = max;
        std::copy(startBin, endBin, featurelist.begin());
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
        makeCumError();         // included here to avoid sorting twice
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
            Bin output(featurelist.begin() + startBin, featurelist.begin() + endBin, max); // featurelist,
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
            // multithreading probably not a good solution, since the objects need to be copied -> copies all mz for every bin
            return;
        }
    }

    void Bin::subsetRT(const Bin &target, const int &maxdist)
    {
    }

#pragma endregion "Bin"
}

int main()
{
    q::BinContainer testcontainer;
    testcontainer.makeFirstBin("../test/test.csv");
    return 0;
}
