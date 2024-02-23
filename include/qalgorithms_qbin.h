#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <optional>
#include <thread> // wahrscheinlich nicht notwendig

namespace q
{
    // Functions
    const std::vector<double> fastMeanDistances(const std::vector<double> &x); // calculate the mean Distances of one to other elements in vector
    // q::Bin extractBin(const q::rawData &data);
    // q::Bin makeBins(const q::Bin &Bin);
    // q::resultFrame ;
    // void appendBin(Bin resultBin, BinContainer target); // implemented as separate function since BinContainer name is unknown

    // Classes
    class RawData // only contains csv input, find way to make constant
    {
    private:
    public:
        RawData();
        ~RawData();
        void readcsv(std::string path);
        std::vector<std::string> headers;
        std::vector<std::vector<double>> data; // ßßß aufbau
        void help();
    };

    class Bin
    {
    private:
        std::vector<int> index; // link Bin entries to dataset
        // int t_iterations; // test criteria ßßß not feasible with current approach
        // int BinID;             // random number, hash, checksum? ßßß redundant due to unique position in container
        // std::string dataspace; // type of data used ßßß resolved using bin container
        double DQS_B;          // Data quality score
        int binsize;    // test criteria

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
        std::string description;      // generate automatically?
        std::vector<Bin> binStorage;  // eventuell unsorted map verwenden -> nein, vektor erübrigt ID-Abfrage
        std::vector<int> mainIndices; // keep indices of sorted m/z for calculating DQS, implemented as deciding on one dimension
        /*calculate DQS for all dimensions and pick the best one?*/
        // std::vector<bool> retain; // T: keep point for further processing, F: Discard ßßß sollte redundant sein, da nur sortierte m/z bekannt sein müssen
        std::vector<double> activeNos;      // only one normalised order space is checked at a time ßßß calculate NOS per bin? Possibly less efficient, since bins never need to be reevaluated in one dimension
        std::vector<int> dataspaceSelect;   // user-defined parameters linking to the raw data object
        std::vector<bool> dataspaceDone;    // prevent repeat operations, primarily intended for testing / time intensive calculations that are better performed stepwise
        std::vector<int> orderOfImportance; // first element is primary dimension
        struct OrderIndices                 // https://stackoverflow.com/questions/25921706/creating-a-vector-of-indices-of-a-sorted-vector
        {
            const std::vector<double> &target;
            OrderIndices(const std::vector<double> &target) : target(target) {}
            bool operator()(int a, int b) const { return target[a] < target[b]; }
        };

    public:
        BinContainer(RawData user_data); // , RawData rawData, std::optional<std::vector<int>> orderInt, std::optional<std::vector<std::string>>& orderString accept arguments as column number or name
        ~BinContainer();
        // void selectDataspace(std::vector<int> selection); // adapt function as to accept different inputs (name), add option to ignore incomplete? ßßß superfluous
        void startProcessing();                           // wrapper function, iterate over all selected dataspaces
        void makeNOS(int dataspace, RawData user_data); // set mainIndices if dataspace[0], set activeNos, only call once per dataspace
        void initBinning(int dataspace); // generate Bins for any one dimension. dataspace is given as all dimensions to bin for as their column number in the imported csv. Binning is performed by iterating over dataspace
        std::vector<int> selectBin(int idx);
        std::vector<int> largestBin();
        std::vector<std::vector<int>> allOfSize(std::vector<int> size);   // include mod for 2x int -> range entry
        std::vector<std::vector<int>> byScore(double score, bool invert); // only output bins over/below a given score
        std::vector<int> t_binsizes();
        void subsetBin(const std::vector<double> &nos, std::vector<int> idx); // index vector to pointers
        /* fürs Subsetting ist es nicht notwendig, die nos zu verändern -
        pointer lösen das Problem, solange der nos-Vektor im Speicher bleiben kann.
        Erst am Schluss müssen die Indices zurück auf die Daten gemappt werden */
        void help(); // include documentation for bin object
        // testing purposes only
        const std::vector<double> mz;
        const std::vector<double> rt;
    };

}

/* Targeted usage (for testing)
- read csv containing binable dataset
- create bin container 
- define order of binning (mz -> RT) // no user input for the time being 
- create index vector for mz
- create NOS for mz
- set first element of dataspaceDone to false
- bin dataset with mz
- subset bins using rt
- calculate DQS from full dataset
-
-- general --
re-binning in any dimension should be unnecessary, since the critical value rises with smaller bin size
multithreading is not automatic, should not be necessary to begin with - if yes, implement with separate function from make bins
*/