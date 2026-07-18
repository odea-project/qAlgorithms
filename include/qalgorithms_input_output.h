#ifndef QALGORITHMS_INPUT_OUTPUT
#define QALGORITHMS_INPUT_OUTPUT

#include "qalgorithms_datatypes.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

// when printing filepaths, on windows we have to use %ls in format strings while on linux,
// %s is used. This is annoying and leads to string duplication. Instead, we can use a macro
// and accept the slightly worse readability of writing "text" MACRO "text\n" for the format
// strings in printf. Since this is mainly abstracted into a logging function, that is acceptable.

namespace qAlgorithms
{

#pragma region "command line arguments"

    enum TerminateAfter : uint8_t
    {
        never,
        profilesec,
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
        std::string outputPath;
        size_t skipAhead = 0;        // this is intended to make testing over many files more time efficient
        TerminateAfter term = never; // continue loop execution after the desired result has been printed
        // limits for printing profile region
        float prof_lim_mz_lower = 0;
        float prof_lim_mz_upper = 0;
        float prof_lim_rt_lower = 0;
        float prof_lim_rt_upper = 0;
        // output options
        bool printCentroids = false;
        bool printCentroidsMZML = false;
        bool printBins = false;
        bool printExtended = false;
        bool printFeatures = false;
        bool printSubProfile = false;
        bool printFeatCens = false;
        bool printComponentRegs = false;
        bool printComponentBins = false;
        bool printProfileSection = false;
        // progress reporting
        bool silent = false;
        bool verboseProgress = false;
        bool doLogging = false;
        // extended functionality
        bool time_seconds = false;
        bool skipError = false;
        bool noOverwrite = false;
        bool tasklistSpecified = false; // @todo implement
        bool interactive = false;
    };

    UserInputSettings passCliArgs(int argc, char *argv[]);

    UserInputSettings interactiveMode();

    bool inputsAreSensible(UserInputSettings &args);

#pragma endregion "command line arguments"

#pragma region "file reading"
    struct TaskItem
    {
        std::filesystem::path path;
        Polarities polarity = Polarities::unknown_polarity;
        unsigned int replicateGroup = 0; // @todo
        unsigned int fileID = 0;         // count upwards after reading everything in @todo produce a unique identifier
    };

    std::vector<std::filesystem::path> controlInput(const UserInputSettings *args);

#pragma endregion "file reading"

#pragma region "print functions"

    class XML_File;
    void printProfileSections(const XML_File *infile,
                              const UserInputSettings *inargs,
                              const std::vector<unsigned int> *selectedIndices,
                              std::string filename);

    void printSpectrumAndCens(const std::vector<CentroidPeak> *peaktable,
                              std::filesystem::path pathOutput,
                              std::string filename,
                              size_t spectrumNo);

    void printCentroids(const std::vector<CentroidPeak> *peaktable,
                        std::vector<float> *convertRT,
                        std::filesystem::path pathOutput,
                        std::string filename,
                        bool silent, bool noOverwrite);

    void printBins(const std::vector<CentroidPeak> *centroids,
                   const std::vector<EIC> *bins,
                   //    const std::vector<float> *convertRT,
                   std::filesystem::path pathOutput,
                   std::string filename,
                   bool silent, bool noOverwrite);

    void printFeatureList(const std::vector<FeaturePeak> *peaktable,
                          std::filesystem::path pathOutput,
                          std::string filename,
                          const std::vector<EIC> *originalBins,
                          const std::vector<float> *convertRT,
                          bool silent, bool noOverwrite);

    void printFeatureCentroids(const std::vector<FeaturePeak> *peaktable,
                               std::filesystem::path pathOutput,
                               std::string filename,
                               const std::vector<EIC> *originalBins,
                               //    const std::vector<float> *convertRT,
                               bool silent, bool noOverwrite);

    void printLogfile(std::filesystem::path pathLogfile); // @todo

#pragma endregion "print functions"
} // namespace qAlgorithms

#endif