// qalgorithms_qbin.cpp

#include "../include/qalgorithms_qbin.h"

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
    rawData::rawData() {}
    rawData::rawData(std::string path) {}
    rawData::~rawData() {}
    double rawData::getmz(int i) const
    {
        return mz[i];
    }
    // std::vector<double> rawData::getmz(){}
    void rawData::setmz(double value, int index) {}
    void rawData::setmz(const std::vector<double> &vec)
    {
        mz = vec;
    }
    double rawData::getrt(int i) const { return 0.; }
    // std::vector<double> rawData::getrt(){}
    void rawData::setrt(double value, int index) {}
    void rawData::setrt(const std::vector<double> &vec)
    {
        rt = vec;
    }
    double rawData::getintensity(int i) const { return 0.; }
    // std::vector<double> rawData::getintensity(){}
    void rawData::setintensity(double value, int index) {}
    void rawData::setintensity(const std::vector<double> &vec)
    {
        intensity = vec;
    }
    void rawData::print()
    {
        std::cout << "success";
    }
    void rawData::readcsv(std::string path)
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
    void rawData::help()
    {
        std::cout << "\n Your documentation here! \n";
    }

    // bin class
    bin::bin() {}
    bin::~bin() {}

    void subsetBin(const std::vector<double> &nos, std::vector<int> idx)
    { // void ausgabe, extern vektor initialisieren und mit push_back() anfügen
        double vcrit;
        const int n = idx.size();
        auto pmax = std::max_element(idx.begin(), idx.end()-2); // iterator with the position of maximum. -2 to not include maximum at which the previous cut occurred
        // int iterator must be implemented outside of the recursive function
        if (n < 5) // terminate function if bin too small
        {
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
            idx.push_back(-1);
            for (int i = 0; i < n; ++i)
                std::cout << idx[i] << " ";
            return; // create bin here
        }
        else
        {

            std::vector<int> range1(idx.begin(), pmax);
            std::vector<int> range2(pmax, idx.end());
            subsetBin(nos, range1);
            subsetBin(nos, range2);
            // int binstart = idx[0];
            // for (int i = idx[0]; i == idx.back(); i++) //
            // {
            //     if (nos[i] > vcrit)
            //     {
            //         std::vector<int> num = std::iota(binstart, i, 1);
            //         subsetBin(&nos, num);
            //         int binstart = i+1;
        }
        std::cout << "I";
    }

    // return subsetBin(nos,);

}

int main()
{
    const std::vector<double> nos = {0.0178,0.0179,0.0169,0.0175,0.0172,0.0173,0.5580,0.9373,0.2089,0.7187,0.8188,0.7409,0.5495,0.7000,0.7565,0.4286,0.4682,0.1984,0.3768,0.1503,0.2685,0.6151,0.8555,0.4497,0.4177,0.8574,0.2988,0.0278,0.6537,0.0783,0.6358,0.2581,0.7298,0.0919,0.2276,0.3038,0.7050,0.6696,0.7409,0.3830};
    std::vector<int> index(40); // function runs 12 times for the given dataset
    std::iota(index.begin(), index.end(), 1);

    q::subsetBin(nos, index);

    // std::vector<double> x {1,2,3,4};
    // q::rawData test;
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
