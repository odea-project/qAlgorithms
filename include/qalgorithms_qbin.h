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
    // q::Bin extractBin(const q::rawData &data);
    // q::Bin makeBins(const q::Bin &Bin);
    //q::resultFrame ;

    // Classes
    class RawData
    {
    private:
        std::vector<double> mz; // mass/charge ratio
        std::vector<double> rt; // retention time
        std::vector<double> intensity;
        

    public:
        RawData();
        RawData(std::string path);
        ~RawData();
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
        const int size();
        void help();
    };

    class Bin
    {
    private:
        std::vector<double> index; // link Bin entries to dataset
        // double critVal; // ist es sinnvoll, für jeden Bin ein neues Objekt zu erstellen? - Nein
        // int groupsize; // redundant
        int t_iterations; // test criteria
        int Binid; // random number ?
        std::string dataspace; // type of data used

    public:
        Bin();
        ~Bin();
        double getindex(int i);
        std::vector<int> getIndex();
        std::vector<double> getIDs();
        void setindex(std::vector<int> vec);
        double getnos(int i);
        std::vector<int> getnos();
        void setos(std::vector<double> vec);
        void subsetBin(const std::vector<double> &nos, std::vector<int> idx); // index vector to pointers
        /* fürs Subsetting ist es nicht notwendig, die nos zu verändern - 
        pointer lösen das Problem, solange der nos-Vektor im Speicher bleiben kann. 
        Erst am Schluss müssen die Indices zurück auf die Daten gemappt werden */
        bool discardFeature(int idx);

    };

    class BinContainer
    {
    private:
        std::vector<Bin> binStorage; // eventuell unsorted map verwenden
        std::vector<int> indices; // 
        std::vector<bool> retain; // T: keep point for further processing, F: Discard
        std::vector<double> activeNos; // only one normalised order space is checked at a time
        std::vector<char> dataspaceDone; // prevent repeat operations
    public:
        BinContainer(RawData rawData);
        ~BinContainer();
        void initBinning(RawData data, std::string dataspace); // generate Bins for any one dimension
        void appendBin(Bin);
        std::vector<int> selectBin(int idx);
        std::vector<int> largestBin();
        std::vector<std::vector<int>> allOfSize(std::vector<int> size); // include mod for 2x int -> range entry
        std::vector<std::vector<int>> byScore(double score, bool invert); // only output bins over/below a given score
        std::vector<int> t_binsizes();
    };
    
    
}