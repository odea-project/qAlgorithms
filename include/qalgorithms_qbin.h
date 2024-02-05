#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>

namespace q
{
    // Functions
    const std::vector<double> fastMeanDistances(const std::vector<double> &x);

    // Classes
    class rawData
    {
    private:
        std::vector<double> mz;
        std::vector<double> rt;
        std::vector<double> intensity;

    public:
        rawData();
        rawData(std::string path);
        ~rawData();
        double getmz(int i) const;
        std::vector<double> getmz();
        void setmz(double value, int index);
        void setmz(const std::vector<double> &vec);
        double getrt(int i) const;
        std::vector<double> getrt();
        void setrt(double value, int index);
        void setrt(const std::vector<double> &vec);
        double getintensity(int i) const;
        std::vector<double> getintensity();
        void setintensity(double value, int index);
        void setintensity(const std::vector<double> &vec);
        void print();
        void readcsv(std::string path);
        void help();
    };

    class bin
    {
    private:
        std::vector<double> index;
        std::vector<double> os;
        double critVal; // ist es sinnvoll, für jeden bin ein neues Objekt zu erstellen?
        int groupsize;

    public:
        bin();
        ~bin();
    };

}