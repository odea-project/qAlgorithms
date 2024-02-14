#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <optional>

namespace q
{
    // Functions
    const std::vector<double> fastMeanDistances(const std::vector<double> &x); // calculate the mean Distances of one to other elements in vector
    // q::Bin extractBin(const q::rawData &data);
    // q::Bin makeBins(const q::Bin &Bin);
    //q::resultFrame ;
    //void appendBin(Bin resultBin, BinContainer target); // implemented as separate function since BinContainer name is unknown

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
        double getval_single(int col, int i) const;
        std::vector<double> getval(int col); 
        void setval(int col, int i, double value);
        // double getmz(int i) const;
        // std::vector<double> getmz(); 
        // void setmz(double value, int index);
        // void setmz(const std::vector<double> &vec);
        // double getrt(int i) const;
        // std::vector<double> getrt();
        // void setrt(double value, int index);
        // void setrt(const std::vector<double> &vec);
        // double getintensity(int i) const;
        // std::vector<double> getintensity();
        // void setintensity(double value, int index);
        // void setintensity(const std::vector<double> &vec);
        void print();
        void readcsv(std::string path);
        const int size();
        void help();
    };

    class Bin
    {
    private:
        std::vector<int> index; // link Bin entries to dataset
        // int t_iterations; // test criteria ßßß not feasible with current approach
        int BinID; // random number, hash, checksum? ßßß redundant due to consistent position 
        std::string dataspace; // type of data used ßßß resolved using bin container
        double DQS_B; // Data quality score
        int binsize;

    public:
        Bin(std::vector<int> idx);
        ~Bin();
        double getindex(int i);
        std::vector<int> getIndex();
        std::vector<double> getIDs();
        void setindex(std::vector<int> vec);
        
       
    };

    class BinContainer
    {
    private:
        std::string description;
        std::vector<Bin> binStorage; // eventuell unsorted map verwenden
        std::vector<int> mainIndices; // keep indices of sorted m/z for calculating DQS, implemented as deciding on one dimension
        /*calculate DQS for all dimensions and pick the best one?*/
        // std::vector<bool> retain; // T: keep point for further processing, F: Discard ßßß sollte redundant sein, da nur sortierte m/z bekannt sein müssen
        std::vector<double> activeNos; // only one normalised order space is checked at a time
        std::vector<bool> dataspaceDone; // prevent repeat operations, use order as determined by raw data
        std::vector<int> orderOfImportance; // first element is primary dimension´
        int threadOpen; // opening a thread with subsetBin substracts 1, terminating it (bin < 5 or bin accepted) adds 1
        struct OrderIndices // https://stackoverflow.com/questions/25921706/creating-a-vector-of-indices-of-a-sorted-vector
        {
            const std::vector<int>& target;
            OrderIndices(const std::vector<int>& target): target(target) {}
            bool operator()(int a, int b) const { return target[a] < target[b]; }
        }; 
    public:
        BinContainer(std::string user_desc); // , RawData rawData, std::optional<std::vector<int>> orderInt, std::optional<std::vector<std::string>>& orderString accept arguments as column number or name
        ~BinContainer();
        void makeNOS(int dimension, bool mostImportant);
        void initBinning(RawData data, std::string dataspace); // generate Bins for any one dimension
        std::vector<int> selectBin(int idx);
        std::vector<int> largestBin();
        std::vector<std::vector<int>> allOfSize(std::vector<int> size); // include mod for 2x int -> range entry
        std::vector<std::vector<int>> byScore(double score, bool invert); // only output bins over/below a given score
        std::vector<int> t_binsizes();
        void subsetBin(const std::vector<double> &nos, std::vector<int> idx, int dataspace); // index vector to pointers
        /* fürs Subsetting ist es nicht notwendig, die nos zu verändern - 
        pointer lösen das Problem, solange der nos-Vektor im Speicher bleiben kann. 
        Erst am Schluss müssen die Indices zurück auf die Daten gemappt werden */
// testing purposes only
        const std::vector<double> mz;
        const std::vector<double> rt;
    };
    
    
}

/* Targeted usage (for testing)
- create bin container (user input: name)
- define mz and rt
- use OoI to select mz as first dimension
- create NOS for mz
- set first element of dataspaceDone to false
- bin dataset
- subset bins using rt
- calculate DQS from full dataset
- 
*/