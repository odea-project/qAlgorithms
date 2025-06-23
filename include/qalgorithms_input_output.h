#ifndef QALGORITHMS_INPUT_OUTPUT
#define QALGORITHMS_INPUT_OUTPUT

#include <vector>
#include <filesystem>
#include <string>
#include "qalgorithms_datatypes.h"
#include "qalgorithms_qpattern.h" // @todo remove this once MultiRegression is a global type

namespace qAlgorithms
{

#pragma region "command line arguments"

    enum TerminateAfter
    {
        never,
        centroids,
        binning,
        features,
        components,
        never_override
    };

    struct UserInputSettings
    {
        // user input for input and output
        std::vector<std::string> inputPaths;
        std::string outputPath = "";
        size_t skipAhead = 0;        // this is intended to make testing over many files more time efficient
        TerminateAfter term = never; // continue loop execution after the desired result has been printed
        // output options
        bool printCentroids = false;
        bool printBins = false;
        bool printExtended = false;
        bool printFeatures = false;
        bool printSubProfile = false;
        bool printFeatCens = false;
        bool printComponentRegs = false;
        bool printComponentBins = false;
        // progress reporting
        bool silent = false;
        bool verboseProgress = false;
        bool doLogging = false;
        // extended functionality
        bool skipError = false;
        bool noOverwrite = false;
        bool tasklistSpecified = false; // @todo implement
        bool interactive = false;
    };

    UserInputSettings passCliArgs(int argc, char *argv[]);

    UserInputSettings interactiveMode(char *argv[]);

    bool inputsAreSensible(UserInputSettings &args);

#pragma endregion "command line arguments"

#pragma region "file reading"
    // handle polarity switching
    enum Polarities
    {
        positive = 1,
        negative = -1,
        unknown = 0
    };

    struct TaskItem
    {
        std::filesystem::path path;
        Polarities polarity;
        unsigned int replicateGroup; // @todo
        unsigned int fileID;         // count upwards after reading everything in @todo produce a unique identifier
    };

    std::vector<std::filesystem::path> controlInput(const std::vector<std::string> *inputTasks, const bool skipError);

#pragma endregion "file reading"

#pragma region "print functions"
    struct ProfilePoint
    {
        float mz;
        float rt;
        int scan;
        float intensity;
    };

    void printCentroids(const std::vector<CentroidPeak> *peaktable,
                        std::vector<float> *convertRT,
                        std::filesystem::path pathOutput,
                        std::string filename,
                        bool silent, bool skipError, bool noOverwrite);

    void printBins(const std::vector<CentroidPeak> *centroids,
                   const std::vector<EIC> *bins,
                   std::filesystem::path pathOutput,
                   std::string filename,
                   bool silent, bool skipError, bool noOverwrite);

    void printFeatureList(const std::vector<FeaturePeak> *peaktable,
                          std::filesystem::path pathOutput,
                          std::string filename,
                          const std::vector<EIC> *originalBins,
                          bool verbose, bool silent, bool skipError, bool noOverwrite);

    void printFeatureCentroids(const std::vector<FeaturePeak> *peaktable,
                               std::filesystem::path pathOutput,
                               std::string filename,
                               const std::vector<EIC> *originalBins,
                               bool verbose, bool silent, bool skipError, bool noOverwrite);

    void printComponentRegressions(const std::vector<MultiRegression> *compRegs,
                                   std::filesystem::path pathOutput,
                                   std::string filename,
                                   bool verbose, bool silent, bool skipError, bool noOverwrite);

    void printComponentCentroids(const std::vector<MultiRegression> *compRegs,
                                 const std::vector<EIC> *bins,
                                 std::filesystem::path pathOutput,
                                 std::string filename,
                                 bool verbose, bool silent, bool skipError, bool noOverwrite);

    void printLogfile(std::filesystem::path pathLogfile); // @todo

#pragma endregion "print functions"
}

#endif