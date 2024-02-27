// qalgorithms_qBin.cpp

#include "../include/qalgorithms_qBin.h"

namespace q
{
    // Functions

    // RawData Class
    RawData::RawData() {}
    RawData::~RawData() {}
    void RawData::readcsv(std::string path)
    {
        std::ifstream file(path); // test.csv must inclide a header line specifying the contents of each column

        static bool initialised;

        std::istringstream stream{};

        for (std::string line{}; std::getline(file, line); stream.clear())
        {
            stream.str(line);
            std::string number{};
            std::size_t position = 0;

            if (!initialised) // only execute this part once - uses same naming as numbers to remove header
            {
                initialised = true;
                while (std::getline(stream, number, ','))
                {
                    headers.push_back(number);
                }
                data.resize(headers.size());
            }

            while (std::getline(stream, number, ',')) // every subvector contains one column
            {
                data[position].push_back(stod(number));
                ++position;
            }
        }
    }
    void RawData::help()
    {
        std::cout << "\n Your documentation here! \n"
                  << "readcsv: The input must include at least one column consisting of a header and one or more numbers written as decimals. Only commas are accepted as deliminators. Headers are case-sensitive. \n";
    }

    // BinContainer class
    BinContainer::BinContainer(const RawData &user_data)
    {
        std::cout << "Select the columns you wish to use for binning in decreasing order of resolution by entering the numbers assigned to them comma-separated."
                  << "\n The availvable columns are:\n";
        for (size_t i = 0; i < user_data.headers.size(); i++)
        {
            std::cout << user_data.headers[i] << " - " << i << " // ";
        }
        std::cout << "\n";
        // ßßß include user input here - include option to skip user input if OoI is given as argument to BinContainer()
        orderOfImportance = {0, 5};
        dataspaceDone.resize(orderOfImportance.size()); // default value of bool is false
        std::cout << "Select the column which contains the standard error for your primary parameter by entering the number assigned to it.";
        errorCol = 7;
    }
    BinContainer::~BinContainer() {}

    void BinContainer::initBinning(int dataspace, RawData &user_data)
    {
        if (dataspaceDone[dataspace])
        {
            std::cout << "Dataspace " << dataspace << " was already used for binning, skipping ahead...\n";
            return;
        }
        else
        {
            // create order space
            makeNOS(user_data.data[dataspace]); // dataspace is one element of orderOfImportance vector
            subsetBin(activeNos, user_data.data[errorCol], 0, activeNos.size());
            dataspaceDone[dataspace] = true;
        }
    }

    void BinContainer::makeNOS(std::vector<double> activeDim)
    {

        activeNos.reserve(activeDim.size());
        //
        std::vector<int> index(activeDim.size());
        std::iota(index.begin(), index.end(), 0);
        // sort index by size of the active dataspace
        std::sort(index.begin(), index.end(), OrderIndices(activeDim));

        mainIndices = index; // change if binning in more than one dimension / add function to subset bins with vector of bins as input

        for (size_t i = 0; i + 1 < index.size(); i++) // +1 since difference vector is one short
        {
            double diff = activeDim[index[i + 1]] - activeDim[index[i]];
            activeNos.push_back(diff);
        }

        activeNos.push_back(-225); // NOS vector has same length as data, -225 taken from existing code
    }

    void BinContainer::subsetBin(const std::vector<double> &nos, const std::vector<double> &error, int beginBin, int endBin) // idx not a pointer to enable pausing ßßß requires error list
    {                                                                                                                        // give start and end coordinates of bin instead of whole range
        double vcrit;
        const int n = endBin - beginBin;    // size 0 not possible, since then no cut would occur (in the previous step) -> saved as Bin
        auto pmax = std::max_element(nos.begin() + beginBin, nos.begin() + endBin - 1); // -1 to not include maximum at which the previous cut occurred
        int cutpos = std::distance(nos.begin() + beginBin, pmax);
        // int iterator must be implemented outside of the recursive function
        if (n < 5) // terminate function if Bin too small
        {
            std::cout << "terminate at length" << n << "\n"; // ßßß testing only
            return;
        }
        double meanerror = 0;
        for (size_t i = beginBin; i == endBin; i++)
        {
            meanerror += error[mainIndices[i]]; // segmentation fault in this line - fails to access error col through indices?
        }

        vcrit = 3.05037165842070 * pow(log(n + 1), (-0.4771864667153)) * meanerror / n; // integrate calculation of mean mz error - norm would be mz / error > critval, equivalent to mz > critval * error

        if (double max = *pmax < vcrit)
        {
            // construct vector containing indices in relation to raw data
            std::vector<int> idx(&mainIndices[beginBin], &mainIndices[endBin]);
            // append Bin to bin container
            Bin output = Bin(idx);
            binStorage.push_back(output);
            return;
        }
        else
        {
            subsetBin(nos, error, beginBin, beginBin + cutpos);
            subsetBin(nos, error, beginBin + cutpos + 1, endBin);
            return;
        }
        std::cout << "I"; // count total number of function calls ßßß remove
    }

    std::vector<int> BinContainer::allOfSize(std::vector<int> size)
    {
        std::vector<int> output;
        for (size_t i = 0; i < binStorage.size(); i++)
        {
            if (std::find(size.begin(), size.end(), binStorage[i].binsize) != size.end())
            {
                output.push_back(i);
            }
        }
        return output;
    }

    std::vector<int> BinContainer::byScore(double score, bool invert)
    {
        std::vector<int> output;
        if (invert)
        {
            for (size_t i = 0; i < binStorage.size(); i++)
            {
                if (binStorage[i].DQS_B <= score)
                {
                    output.push_back(i);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < binStorage.size(); i++)
            {
                if (binStorage[i].DQS_B >= score)
                {
                    output.push_back(i);
                }
            }
        }
        return output;
    }

    void BinContainer::t_binsizes()
    {
        std::cout << "\n";
        for (size_t i = 0; i < binStorage.size(); i++)
        {
            std::cout << binStorage[i].binsize << " ; ";
        }
        std::cout << "\n";
    }

    std::vector<int> BinContainer::selectBin(int idx)
    {
        return binStorage[idx].index;
    }

    const std::vector<double> BinContainer::meanDistances(const std::vector<double> &x)
    {
        const int n = static_cast<int>(x.size());
        std::vector<double> vec_dist;
        vec_dist.reserve(n);
        // if not sorted / not sortable: Calculate absolute distance between i and j, add it to the position of i and j.
        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 1; j <= n; ++j)
            {
                double dist = abs(x[i] - x[j]);
                vec_dist[i] = +dist;
                vec_dist[j] = +dist;
            }
            vec_dist[i] == vec_dist[i] / (n - 1);
        }

        for (int i = 0; i < n; ++i)
            std::cout << vec_dist[i] << " ";

        // std::cout << x.size() << std::endl;
        return vec_dist;
    }

    // Bin class
    Bin::Bin(std::vector<int> idx)
    {
        index = idx;
        binsize = index.size();
    }
    Bin::~Bin() {}

}

// main
int main()
{
    q::RawData test;
    test.readcsv("../test/test.csv");
    q::BinContainer testCont(test);
    testCont.initBinning(0, test); // sorting and nos creation works well

    // leave

    const std::vector<double> nos = {0.0178, 0.0179, 0.0169, 0.0175, 0.0172, 0.0173, 0.5580, 0.9373, 0.2089, 0.7187, 0.8188, 0.7409, 0.5495, 0.7000, 0.7565, 0.4286, 0.4682, 0.1984, 0.3768, 0.1503, 0.2685, 0.6151, 0.8555, 0.4497, 0.4177, 0.8574, 0.2988, 0.0278, 0.6537, 0.0783, 0.6358, 0.2581, 0.7298, 0.0919, 0.2276, 0.3038, 0.7050, 0.6696, 0.7409, 0.3830};
    std::vector<int> index(40); // function runs 12 times for the given dataset
    std::iota(index.begin(), index.end(), 1);

    std::vector<std::vector<int>> binContainer{{-1}};
    binContainer.push_back(index);
    std::cout << binContainer[1][1]; // segmentation fault !! Vektoren fangen mit Index 0 an !!ßßß

    // std::vector<double> x {1,2,3,4};
    // q::RawData test;
    // test.readcsv("../test/test.csv");
    // std::cout << test.getmz(0);
    // test.help();

    // std::vector<double> y = q::fastMeanDistances(x);
    return 0;
}
