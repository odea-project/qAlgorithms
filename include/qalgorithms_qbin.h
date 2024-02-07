#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

namespace q
{
    // Functions
    const std::vector<double> fastMeanDistances(const std::vector<double> &x); // calculate the mean Distances of one to other elements in vector
    // q::bin extractBin(const q::rawData &data);
    // q::bin makeBins(const q::bin &bin);
    //q::resultFrame ;

    // Classes
    class rawData
    {
    private:
        std::vector<double> mz; // mass/charge ratio
        std::vector<double> rt; // retention time
        std::vector<double> intensity;
        std::vector<bool> retain; // T: keep point for further processing, F: Discard

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
        std::vector<double> index; // link bin entries to dataset
        std::vector<double> os;
        double critVal; // ist es sinnvoll, für jeden bin ein neues Objekt zu erstellen?
        int groupsize;
        int t_iterations; // test criteria
        int binid; // checksum
        std::string dataspace;   // type of data used

    public:
        bin();
        ~bin();
        double getindex(int i);
        std::vector<int> getindex();
        void setindex(std::vector<int> vec);
        double getos(int i);
        std::vector<int> getos();
        void setos(std::vector<int> vec);
        int getcritVal();
        void subsetBin(const std::vector<double> &nos,std::vector<int> idx);
        /* fürs Subsetting ist es nicht notwendig, die nos zu verändern - 
        aus den nos wird ein index-vektor erzeugt und rekursiv bearbeitet. 
        Erst am Schluss müssen die Indices zurück auf die Daten gemappt werden */

    };

}