// qalgorithms_qbin.cpp

#include "../include/qalgorithms_qbin.h"

namespace q
{
    // Functions
    // mean Distances to other elements in vector
    const std::vector<double> fastMeanDistances(const std::vector<double> &x)
    {
        // const int n = static_cast<int>(x.size()); // always convert to int std::vector<double>
        // // generate a vector vec_i of lenght n^2 with elements of 1:n repeating n times
        // std::vector<int> vec_i;
        // vec_i.reserve(n * n);
        // for (int i = 1; i < n+1; i++)
        // {
        //     std::vector<int> vec_temp(n,i);
        //     vec_i.insert(vec_i.end(), vec_temp.begin(), vec_temp.end());
        // }
        // // generate a vector vec_j of lenght n^2 with the series 1:n repeated n times
        // std::vector<int> vec_j;
        // vec_j.reserve(n * n);
        // for (int i = 1; i < n + 1; i++) // ßßß loop necessary?
        // {
        //     std::vector<int> vec_temp(n);
        //     std::iota(vec_temp.begin(), vec_temp.end(), 1);
        //     vec_j.insert(vec_j.end(), vec_temp.begin(), vec_temp.end());
        // }
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
    bin::bin(){}
    bin::~bin(){}
}

int main()
{
    // std::vector<double> x {1,2,3,4};
    q::rawData test;
    test.readcsv("../test/test.csv");
    std::cout << test.getmz(0);
    test.help();

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
