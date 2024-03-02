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
    
    // Classes
    class RawData // only contains csv input, find way to make constant
    {
    private:
    public:
        RawData();
        ~RawData();
        void readcsv(std::string path);
        std::vector<std::string> headers;
        std::vector<std::vector<double>> data; // change to matrix-based approach?
        void help();
    };

    class Bin
    {
    private:
    public:
        Bin(std::vector<int> idx);
        ~Bin();
        std::vector<int> index; // link Bin entries to dataset
        int binsize;                         // test criteria
        double DQS_B;                        // Data quality score
    };

    class BinContainer
    {
    private:
        std::vector<Bin> binStorage;  // eventuell unsorted map verwenden -> nein, vektor erübrigt ID-Abfrage
        std::vector<int> mainIndices; // keep indices of sorted m/z for calculating DQS, implemented as deciding on one dimension
        /*calculate DQS for all dimensions and pick the best one?*/
        // std::vector<bool> retain; // T: keep point for further processing, F: Discard ßßß sollte redundant sein, da nur sortierte m/z bekannt sein müssen
        std::vector<double> activeNos; // only one normalised order space is checked at a time ßßß calculate NOS per bin? Possibly less efficient, since bins never need to be reevaluated in one dimension
        // std::vector<int> dataspaceSelect;   //ßßß superfluous user-defined parameters linking to the raw data object
        std::vector<bool> dataspaceDone;    // prevent repeat operations, primarily intended for testing / time intensive calculations that are better performed stepwise
        std::vector<int> orderOfImportance; // first element is primary dimension
        int errorCol;
        struct OrderIndices                 // https://stackoverflow.com/questions/25921706/creating-a-vector-of-indices-of-a-sorted-vector
        {
            const std::vector<double> &target;
            OrderIndices(const std::vector<double> &target) : target(target) {}
            bool operator()(int a, int b) const { return target[a] < target[b]; }
        };

    public:
        BinContainer(const RawData &user_data); // , RawData rawData, std::optional<std::vector<int>> orderInt, std::optional<std::vector<std::string>>& orderString accept arguments as column number or name
        ~BinContainer();
        // void selectDataspace(std::vector<int> selection); // adapt function as to accept different inputs (name), add option to ignore incomplete? ßßß superfluous
        void startProcessing();                                                  // wrapper function, iterate over all selected dataspaces
        void makeNOS(std::vector<double> activeDim); // set mainIndices if dataspace[0], set activeNos, only call once per dataspace
        void initBinning(int dataspace, RawData &user_data);                                         // generate Bins for any one dimension. dataspace is given as all dimensions to bin for as their column number in the imported csv. Binning is performed by iterating over dataspace
        std::vector<int> selectBin(int idx);
        std::vector<int> allOfSize(std::vector<int> size);                // include mod for 2x int -> range entry
        std::vector<int> byScore(double score, bool invert);      // only output bins over a given score, invert for below
        void t_binsizes();
        void subsetBin(const std::vector<double> &nos, const std::vector<double> &error, int beginBin, int endBin); // begin and end of range in nos
        /* fürs Subsetting ist es nicht notwendig, die nos zu verändern -
        pointer lösen das Problem, solange der nos-Vektor im Speicher bleiben kann.
        Erst am Schluss müssen die Indices zurück auf die Daten gemappt werden */
        const std::vector<double> meanDistances(const std::vector<double> &x); // calculate the mean Distances of one to other elements in vector
        void fragmentBin(Bin, const std::vector<int> elutionOrder, int tolerance); // remove bins if separation in non-mass dimension is too great ßßß implement in subsetting routine?
        void help(); // include documentation for bin object
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