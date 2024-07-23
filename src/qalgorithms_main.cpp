// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_measurement_data_tensor.h"
#include "../include/qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <filesystem> // printing absolute path in case read fails

// console output
#define PRINT_DONE                         \
    SetConsoleTextAttribute(hConsole, 10); \
    std::cout << "done\n";                 \
    SetConsoleTextAttribute(hConsole, 15);

// console output
#define PRINT_DONE_no_n                    \
    SetConsoleTextAttribute(hConsole, 10); \
    std::cout << "done";                   \
    SetConsoleTextAttribute(hConsole, 15);

int main(int argc, char *argv[])
{
    std::string filename_input;
    // ask for file if none are specified
    if (argc < 2)
    {
        std::cout << "Enter a filename (replace every backslash with a forward slash) "
                  << "to process that file ";
    }

    std::cout << "Enter the filename (*.mzML): ";
    std::cin >> filename_input;

    // if only one argument is given, check if it is any variation of help
    // if this is not the case, try to use the string as a filepath

    bool silent = false;
    bool recursiveSearch = false;
    bool fileSpecified = false;

    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") | (argument == "-help"))
        {
            std::cout << argv[0] << " help options:\n"
                      << "-h, -help: open this help menu\n"
                      << "-s, -silent: do not print progress reports to standard out\n"
                      << "-f, -file: specifies the input file. Use as -f FILEPATH\n"
                      << "-r, -recursive: recursive search for .mzML files in the specified directory. "
                      << "Use as -r DIRECTORY\n"
                      << "-o, -output: directory into which all output files should be printed. "
                      << " Use as -o DIRECTORY. If you want to print all results into the folder which "
                      << "contains the .mzML file, write \"#\". The default output is standard out, "
                      << "unless you did not specify an input file. in that case, "
                      << "you will be prompted to enter the output location.\n";
        }
        if ((argument == "-s") | (argument == "-silent"))
        {
            silent = true;
        }
        if ((argument == "-f") | (argument == "-file"))
        {
            // @todo
            fileSpecified = true;
            ++i;
        }
        if ((argument == "-r") | (argument == "-recursive"))
        {
            // @todo
            recursiveSearch = true;
            ++i;
        }
        if ((argument == "-o") | (argument == "-output"))
        {
            // @todo
            recursiveSearch = true;
            ++i;
        }
    }

    if (fileSpecified & recursiveSearch)
    {
        std::cout << "Error: Recursive search and task file are incompatible\n";
        exit(100);
    }

    // initialize qPeaks static variables and lcmsData object
    q::Algorithms::qPeaks::initialize();

    auto timeStart = std::chrono::high_resolution_clock::now();
    // check if file exists; if not, repeat the input until a valid file is entered
    if (!std::ifstream(filename_input))
    {
        std::cout << "Error: file not found\n"
                  << std::endl;
        exit(0);
    }

    std::cout << "reading file... ";

    // check if the input file a mzML file
    if (!(filename_input.find(".mzML") != std::string::npos))
    {
        std::cout << "Error: you must supply a .mzML file\n";
        // break;
        exit(0);
    }

    sc::MZML data(filename_input);             // create mzML object
    q::Algorithms::qPeaks qpeaks;              // create qPeaks object
    q::MeasurementData::TensorData tensorData; // create tensorData object
    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> centroids =
        tensorData.findCentroids_MZML(qpeaks, data, true, 10); // read mzML file and find centroids via qPeaks

    q::Algorithms::qBinning::CentroidedData testdata = qpeaks.passToBinning(centroids, centroids.size());
    std::string summary_output_location = "summary_output_location";
    auto timeEnd = std::chrono::high_resolution_clock::now();

    std::cout << "produced " << testdata.lengthAllPoints << " centroids from " << centroids.size()
              << " spectra in " << (timeEnd - timeStart).count() << " ns\n\n";

    bool verboseQbinning = false;
    timeStart = std::chrono::high_resolution_clock::now();
    std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(testdata, summary_output_location, 6, verboseQbinning);
    timeEnd = std::chrono::high_resolution_clock::now();

    if (verboseQbinning)
    {
        std::cout << "assembled " << binnedData.size() << " bins in " << (timeEnd - timeStart).count() << " ns\n\n";
    }

    timeStart = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks =
        tensorData.findPeaks_QBIN(qpeaks, binnedData);
    timeEnd = std::chrono::high_resolution_clock::now();
    std::cout << "found " << peaks.size() << " peaks in " << (timeEnd - timeStart).count() << " ns\n\n";

    // write peaks to csv file
    std::cout << "writing peaks to file... ";
    std::string output_filename = "output.csv";
    std::ofstream output_file(output_filename);
    output_file << "mz,rt,int,mzUncertainty,rtUncertainty,intUncertainty,dqs_cen,dqs_bin,dqs_peak\n";
    for (size_t i = 0; i < peaks.size(); ++i)
    {
        for (size_t j = 0; j < peaks[i].size(); ++j)
        {
            output_file << peaks[i][j]->mz << "," << peaks[i][j]->retentionTime << "," << peaks[i][j]->area << ","
                        << peaks[i][j]->mzUncertainty << "," << peaks[i][j]->retentionTimeUncertainty << ","
                        << peaks[i][j]->areaUncertainty << "," << peaks[i][j]->dqsCen << "," << peaks[i][j]->dqsBin
                        << "," << peaks[i][j]->dqsPeak << "\n";
        }
    }
    output_file.close();

    return 0;
}