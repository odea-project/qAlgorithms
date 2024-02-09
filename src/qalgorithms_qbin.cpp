// qalgorithms_qBin.cpp

#include "../include/qalgorithms_qBin.h"

namespace q
{
    // Functions
    // mean Distances to other elements in vector
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

    // Classes
    // raw data class
    RawData::RawData() {}
    RawData::RawData(std::string path) {}
    RawData::~RawData() {}
    double RawData::getmz(int i) const
    {
        return mz[i];
    }
    std::vector<double> RawData::getmz(){
        return mz;
    }
    void RawData::setmz(double value, int index) {}
    void RawData::setmz(const std::vector<double> &vec)
    {
        mz = vec;
    }
    double RawData::getrt(int i) const { return 0.; }
    // std::vector<double> RawData::getrt(){}
    void RawData::setrt(double value, int index) {}
    void RawData::setrt(const std::vector<double> &vec)
    {
        rt = vec;
    }
    double RawData::getintensity(int i) const { return 0.; }
    // std::vector<double> RawData::getintensity(){}
    void RawData::setintensity(double value, int index) {}
    void RawData::setintensity(const std::vector<double> &vec)
    {
        intensity = vec;
    }
    void RawData::print()
    {
        std::cout << "success";
    }
    void RawData::readcsv(std::string path)
    {
        std::ifstream file(path);
        std::string line;
        std::vector<std::vector<double>> df;

        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string cell;
            std::vector<double> row;

            while (std::getline(ss, cell, ','))
            {
                row.push_back(std::stod(cell));
            }

            df.push_back(row);
        }
        mz = df[0];
        rt = df[1];
        intensity = df[2];
    }
    void RawData::help()
    {
        std::cout << "\n Your documentation here! \n";
    }
    const int RawData::size(){
        return mz.size();
    }

    // BinContainer class
    BinContainer::BinContainer(RawData rawData)
    {
        const int n = rawData.size();
        std::vector<int> index(n);
        std::iota(index.begin(), index.end(), 1);
        indices = index;
    }
    void BinContainer::appendBin(Bin bin){
        binStorage.push_back(bin);
        return;
    }

    // Bin class
    Bin::Bin() {}
    Bin::~Bin() {}

    void subsetBin(const std::vector<double> &nos, std::vector<int> idx) // idx als pointer
    { // void ausgabe, extern vektor initialisieren und mit push_back() anfügen
        double vcrit;
        const int n = idx.size();
        auto pmax = std::max_element(idx.begin(), idx.end()-2); // iterator with the position of maximum. -2 to not include maximum at which the previous cut occurred
        // int iterator must be implemented outside of the recursive function
        if (n < 5) // terminate function if Bin too small
        {
            std::cout << "terminate at length" << n << "\n"; //ßßß testing only
            return;
        }
        if (n <= 100) // precalculate crit values if bottleneck
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
                std::cout << idx[i] << " "; //ßßß testing only
                std::cout << "\n";
             // append Bin to bin container here
        }
        else
        {

            std::vector<int> range1(idx.begin(), pmax);
            std::vector<int> range2(pmax, idx.end());
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

    // return subsetBin(nos,);

}

int main()
{
    // test run: create raw data (mz only)


    const std::vector<double> nos = {0.0178,0.0179,0.0169,0.0175,0.0172,0.0173,0.5580,0.9373,0.2089,0.7187,0.8188,0.7409,0.5495,0.7000,0.7565,0.4286,0.4682,0.1984,0.3768,0.1503,0.2685,0.6151,0.8555,0.4497,0.4177,0.8574,0.2988,0.0278,0.6537,0.0783,0.6358,0.2581,0.7298,0.0919,0.2276,0.3038,0.7050,0.6696,0.7409,0.3830};
    std::vector<int> index(40); // function runs 12 times for the given dataset
    std::iota(index.begin(), index.end(), 1);

    q::subsetBin(nos, index);

    std::vector<std::vector<int>> binContainer;
    binContainer.push_back(index);
    // std::cout << binContainer[1][20]; // segmentation fault

    // std::vector<double> x {1,2,3,4};
    // q::RawData test;
    // test.readcsv("../test/test.csv");
    // std::cout << test.getmz(0);
    // test.help();

    // std::vector<double> y = q::fastMeanDistances(x);
    // int c = 0;
    // for (int i = 0; i < 10; i++) {
    //    if (i == 5)
    //    {
    //     c = i;
    //    }
    // }
    // std::cout << c;
    return 0;
}
