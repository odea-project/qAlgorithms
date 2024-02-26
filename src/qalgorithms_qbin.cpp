// qalgorithms_qBin.cpp

#include "../include/qalgorithms_qBin.h"

namespace q
{
    // Functions
    // mean Distances to other elements in vector ßßß implement as part of bin container class
    const std::vector<double> fastMeanDistances(const std::vector<double> &x)
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
    BinContainer::BinContainer(const RawData user_data) 
    {
        std::cout << "Select the columns you wish to use for binning in decreasing order of resolution by entering the numbers assigned to them comma-separated."
                  << "\n The availvable columns are:\n";
        for (size_t i = 0; i < user_data.headers.size(); i++)
        {
            std::cout << user_data.headers[i] << " - " << i << " // ";
        }
        // ßßß include user input here - include option to skip user input if OoI is given as argument to BinContainer()
        orderOfImportance = {0, 1};
        dataspaceDone.resize(orderOfImportance.size()); // default value of bool is false
        std::cout << "Select the column which contains the standard error for your primary parameter by enterin the number assigned to it.";
        activeNos.reserve(user_data.data[0].size());
    }

    void BinContainer::initBinning(int dataspace)
    {
        if (dataspaceDone[dataspace])
        {
            std::cout << "Dataspace " << dataspace << " was already used for binning, skipping ahead...\n";
            return;
        }
        else
        {
            dataspaceDone[dataspace] = true; // erst ganz am Ende ßßß
            // makeNOS(dataspace);
        }
    }

    void BinContainer::makeNOS(int dataspace, std::vector<std::vector<double>> activeDim)
    {
        //ßßß remove
        std::vector<int> orderOfImportance = {0, 1};
        std::vector<double> activeNos;
        //
        std::vector<int> index(activeDim[0].size()); // .data
        std::iota(index.begin(), index.end(), 1);
        // sort index by size of the active dataspace
        std::sort(index.begin(), index.end(), OrderIndices(activeDim[dataspace])); // .data

        if (dataspace == orderOfImportance[0])
        {
            mainIndices = index;
        }

        for (size_t i = 0; i + 1 < index.size(); i++) // +1 since difference vector is one short
        {
            double diff = activeDim[dataspace][index[i + 1]] - activeDim[dataspace][index[i]];
            activeNos.push_back(diff);
        }

        activeNos.push_back(-1); // NOS vector has same length as data
    }

    void BinContainer::subsetBin(const std::vector<double> &nos, std::vector<int> idx) // idx not a pointer to enable pausing
    {                                                                                  
        double vcrit; 
        const int n = idx.size();
        auto pmax = std::max_element(idx.begin(), idx.end() - 2); // iterator with the position of maximum. -2 to not include maximum at which the previous cut occurred
        // int iterator must be implemented outside of the recursive function
        if (n < 5) // terminate function if Bin too small
        {
            std::cout << "terminate at length" << n << "\n"; // ßßß testing only
            return;
        }
        if (n <= 100) // precalculate crit values if bottleneck ßßß removed due to dynamic mz error calc
        {
            vcrit = 3.05037165842070 * pow(log(n + 1), (-0.4771864667153));
        }
        else
        {
            vcrit = 3.05037165842070 * pow(log(n + 1), (-0.4771864667153));
        }
        if (double max = *pmax < vcrit)
        {
            // idx.push_back(-1); // not necessary with bin container
            for (int i = 0; i < n; ++i)
                std::cout << idx[i] << " "; // ßßß testing only
            std::cout << "\n";
            // append Bin to bin container
            Bin output = Bin(idx);
            binStorage.push_back(output);
            return;
        }
        else
        {
            std::vector<int> range1(idx.begin(), pmax);
            std::vector<int> range2(pmax + 1, idx.end() - 1); // idx.end-1 hier richtig? Sollte eigentlich auf letztes Element des Vektors zeigen
            subsetBin(nos, range1);
            subsetBin(nos, range2);
            // int Binstart = idx[0];
            // for (int i = idx[0]; i == idx.back(); i++) //
            // {
            //     if (nos[i] > vcrit)
            //     {
            //         std::vector<int> num = std::iota(Binstart, i, 1);
            //         subsetBin(&nos, num);
            //         int Binstart = i+1;
            return;
        }
        std::cout << "I"; // verstehe hier nicht, warum nach return trotzdem I ausgegeben wird
    }

    std::vector<int> BinContainer::allOfSize(std::vector<int> size){
        std::vector<int> output;
        for (size_t i = 0; i < binStorage.size(); i++)
        {
            if(std::find(size.begin(), size.end(), binStorage[i].binsize) != size.end()){
                output.push_back(i);
            }
        }
        return output;
    }

    std::vector<int> BinContainer::byScore(double score, bool invert = false){
        std::vector<int> output;
        for (size_t i = 0; i < binStorage.size(); i++)
        {
            if((binStorage[i].DQS_B < score)*invert){
                output.push_back(i);
            }
        }
        return output;
    }

    // BinContainer::largestBin

    // BinContainer::selectBin

    // BinContainer::


    // Bin class
    Bin::Bin(std::vector<int> idx)
    {
        index = idx;
        binsize = index.size();
    }
    Bin::~Bin() {}
}

int main()
{
    q::RawData test;
    test.readcsv("../test/test.csv");
    // q::BinContainer testCont(test); // compile error if not every function
    



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
