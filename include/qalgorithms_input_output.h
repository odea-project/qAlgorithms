#ifndef QALGORITHMS_INPUT_OUTPUT
#define QALGORITHMS_INPUT_OUTPUT

#include <vector>
#include <filesystem>
#include <string>
#include "qalgorithms_datatypes.h"

namespace qAlgorithms
{

#pragma region "command line arguments"
    struct UserInputSettings
    {
        // user input for input and output
        std::vector<std::string> inputPaths;
        std::string outputPath = "";
        // output options
        bool printCentroids = false;
        bool printBins = false;
        bool printExtended = false;
        bool printFeatures = false;
        bool printSubProfile = false;
        // progress reporting
        bool silent = false;
        bool verboseProgress = false;
        bool doLogging = false;
        // extended functionality
        bool skipError = false;
        bool noOverwrite = false;
        float newPPM = 0;               // @todo not a good idea
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

    std::vector<std::filesystem::path> controlInput(const std::vector<std::string> inputTasks, const bool skipError);

#pragma endregion "file reading"

#pragma region "print functions"
    struct ProfilePoint
    {
        float mz;
        float rt;
        int scan;
        float intensity;
    };

    void printCentroids(std::vector<std::vector<CentroidPeak>> peaktable,
                        std::vector<float> convertRT, std::filesystem::path pathOutput,
                        std::string filename, bool silent, bool skipError, bool noOverwrite);

    void printProfilePoints(std::vector<std::vector<ProfilePoint>> peakComponents,
                            std::filesystem::path pathOutput, std::string filename,
                            bool silent, bool skipError, bool noOverwrite);

    void printBins(std::vector<EIC> bins, std::filesystem::path pathOutput, std::string filename,
                   bool silent, bool skipError, bool noOverwrite);

    void printFeatureList(std::vector<FeaturePeak> peaktable,
                          std::filesystem::path pathOutput, std::string filename,
                          std::vector<EIC> originalBins,
                          bool verbose, bool silent, bool skipError, bool noOverwrite);

    void printLogfile(std::filesystem::path pathLogfile); // @todo

#pragma endregion "print functions"
}

#endif