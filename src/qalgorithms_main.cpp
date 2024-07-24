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

int main(int argc, char *argv[])
{
    std::string filename_input;
    bool inSpecified;
    std::filesystem::path pathSource;
    bool outSpecified;
    std::filesystem::path pathOutput;
    std::string filename;
    std::vector<std::filesystem::path> tasklist;
    // ask for file if none are specified
    if (argc == 1)
    {
        std::cout << "Enter a filename (replace every backslash with a forward slash) "
                  << "to process that file. You must select an .mzML file.  ";
        std::cin >> filename_input;
        // filename_input = "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090901_H2O_1_pos.mzML";
        pathSource = filename_input;
        if (!std::filesystem::exists(pathSource))
        {
            std::cout << "Error: The selected file does not exist.\nSupplied path: " << std::filesystem::canonical(pathSource)
                      << "\nCurrent directory: " << std::filesystem::current_path() << "\n\nTerminated Program.\n\n";
            exit(101); // @todo sensible exit codes
        }
        if (pathSource.extension() != ".mzML")
        {
            std::cout << "Error: the selected file has the type " << pathSource.extension() << ", but only \".mzML\" is supported";
            exit(101); // @todo sensible exit codes
        }
        filename = pathSource.filename().string();
        std::cout << "\nfile accepted, enter the output directory or \"#\" to use the input directory:  ";
        std::cin >> filename_input;
        if (filename_input == "#")
        {
            pathOutput = pathSource.parent_path();
        }
        else
        {
            pathOutput = filename_input;
            if (pathOutput == pathSource)
            {
                std::cout << "Error: use \"#\" if you want to save your results to the location of your input file";
                exit(101);
            }
        }
        inSpecified = true;
        outSpecified = true;
    }

    // if only one argument is given, check if it is any variation of help
    // if this is not the case, try to use the string as a filepath

    bool silent = false;
    bool wordyProgress = false;
    bool recursiveSearch = false;
    bool tasklistSpecified = false;
    bool printSummary = false;
    bool printBins = false;
    bool printExtended = false;

#pragma region cli arguments
    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") | (argument == "-help"))
        {
            std::cout << argv[0] << " help information:\n"
                      << "qAlgorithms is a software project for non-target screening using mass spectrometry."
                      << "For more information, visit our github page: https://github.com/odea-project/qAlgorithms\n"
                      << "As of now (23.07.2024), only mzML files are supported. This program accepts the following command-line arguments:"
                      << "-h, -help: open this help menu\n"
                      << "-s, -silent: do not print progress reports to standard out\n"
                      << "-w, -wordy: print a detailed progress report to standard out"
                      << "-f, -file: specifies the input file. Use as -f FILEPATH\n"
                      << "-tl, -tasklist: pass a list of file paths to the function"
                      << "-r, -recursive: recursive search for .mzML files in the specified directory. "
                      << "Use as -r DIRECTORY\n"
                      << "-o, -output: directory into which all output files should be printed. "
                      << " Use as -o DIRECTORY. If you want to print all results into the folder which "
                      << "contains the .mzML file, write \"#\". The default output is standard out, "
                      << "unless you did not specify an input file. in that case, you will be prompted to "
                      << "enter the output location. If the specified location does not exist, a new directory is created\n"
                      << "-ps, -printsummary: print summarised information on the bins in addition to "
                      << "the peaktable. It is saved to your output directory using as FILENAME_summary.csv\n"
                      << "-pb, -printbins: If this flag is set, both bin summary information and "
                      << "all binned centroids will be printed to the output location in addition "
                      << "to the final peak table. The files end in _summary.csv and _bins.csv.\n"
                      << "-e, -extended: print additional information into the final peak list."
                      << "-log: This option will create a detailed log file in the program directory.";

            exit(0);
        }
        if ((argument == "-s") | (argument == "-silent"))
        {
            silent = true;
            continue;
        }
        if ((argument == "-w") | (argument == "-wordy"))
        {
            wordyProgress = true;
            continue;
        }
        if ((argument == "-f") | (argument == "-file"))
        {
            // @todo
            inSpecified = true;
            ++i;
            continue;
        }
        if ((argument == "-tl") | (argument == "-tasklist"))
        {
            // @todo
            tasklistSpecified = true;
            ++i;
            continue;
        }
        if ((argument == "-r") | (argument == "-recursive"))
        {
            // @todo
            recursiveSearch = true;
            ++i;
            continue;
        }
        if ((argument == "-o") | (argument == "-output"))
        {
            // @todo
            recursiveSearch = true;
            ++i;
            continue;
        }
        if ((argument == "-ps") | (argument == "-printsummary"))
        {
            printSummary = true;
            continue;
        }
        if ((argument == "-pb") | (argument == "-printbins"))
        {
            printSummary = true;
            printBins = true;
            continue;
        }
        if ((argument == "-e") | (argument == "-extended"))
        {
            printExtended = true;
            continue;
        }
        if (argument == "-log")
        {
            //@todo write the executed command into the logfile
            continue;
        }
    }

    if ((!inSpecified & !recursiveSearch) & (!tasklistSpecified))
    {
        std::cout << "Error: no input file supplied. Specify a file using the -f, -r or -tl flag. Execute qAlgorithms with the -h flag for more information\n";
        exit(1);
    }

    if (inSpecified & recursiveSearch)
    {
        std::cout << "Error: Recursive search and target file are incompatible\n";
        exit(100);
    }
    if (tasklistSpecified & recursiveSearch)
    {
        std::cout << "Error: Recursive search and task list are incompatible\n";
        exit(100);
    }
    if (inSpecified & tasklistSpecified)
    {
        std::cout << "Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        // @todo
    }

#pragma region file processing
    {
        if (!outSpecified)
        {
            pathOutput = pathSource.parent_path(); // @todo include route to standard out
        }

        // initialize qPeaks static variables and lcmsData object @todo constexpr
        q::Algorithms::qPeaks::initialize();

        auto timeStart = std::chrono::high_resolution_clock::now();
        std::cout << "reading file...";

        sc::MZML data(std::filesystem::canonical(pathSource).string()); // create mzML object @todo change to use filesystem::path
        std::cout << " ok\n";
        q::Algorithms::qPeaks qpeaks;              // create qPeaks object
        q::MeasurementData::TensorData tensorData; // create tensorData object
        // @todo add check if set polarity is correct
        std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> centroids =
            tensorData.findCentroids_MZML(qpeaks, data, true, "positive", 10); // read mzML file and find centroids via qPeaks
        // std::cout << "centroided\n";
        q::Algorithms::qBinning::CentroidedData testdata = qpeaks.passToBinning(centroids);
        volatile auto checkthis = testdata;
        auto timeEnd = std::chrono::high_resolution_clock::now();

        std::cout << "produced " << testdata.lengthAllPoints << " centroids from " << centroids.size()
                  << " spectra in " << (timeEnd - timeStart).count() << " ns\n\n";

        bool silentBinning = false;
        timeStart = std::chrono::high_resolution_clock::now();
        std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(
            testdata, pathOutput, filename, 6, silentBinning, printSummary, printBins);
        timeEnd = std::chrono::high_resolution_clock::now();

        if (silentBinning)
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
        exit(10);
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
    }
    return 0;
}