// internal
#include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_measurement_data_tensor.h"
#include "qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <filesystem> // printing absolute path in case read fails
#include <string>
#include <sstream>

int main(int argc, char *argv[])
{
    std::string filename_input;
    bool inSpecified;
    std::filesystem::path pathInput;
    bool outSpecified;
    std::filesystem::path pathOutput;
    std::string filename;
    std::vector<std::filesystem::path> tasklist;
    // ask for file if none are specified
    if (argc == 1)
    {
        // @todo check standard in and use that if it isn't empty
        std::cout << "Enter a filename (replace every backslash with a forward slash) "
                  << "to process that file. You must select an .mzML file.  ";
        std::cin >> filename_input;
        // filename_input = "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090901_H2O_1_pos.mzML";
        pathInput = filename_input;
        if (!std::filesystem::exists(pathInput))
        {
            std::cerr << "Error: The selected file does not exist.\n"
                      << std::filesystem::canonical(pathInput);
        }
        if (pathInput.extension() != ".mzML")
        {
            std::cerr << "Error: the selected file has the type " << pathInput.extension() << ", but only \".mzML\" is supported\n";
            exit(101); // @todo sensible exit codes
        }
        filename = pathInput.filename().string();
        std::cout << "\nfile accepted, enter the output directory or \"#\" to use the input directory:  ";
        std::cin >> filename_input;
        if (filename_input == "#")
        {
            pathOutput = pathInput.parent_path();
        }
        else
        {
            pathOutput = filename_input;
            if (pathOutput == pathInput)
            {
                std::cerr << "Error: use \"#\" if you want to save your results to the location of your input file\n";
                exit(101);
            }
        }
        inSpecified = true;
        outSpecified = true;
        tasklist.push_back(pathInput);
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
                      << "-f, -files: specifies the input files. More than one file can be passed. Use as -f FILEPATH FILEPATH etc.\n" // @todo allow piping of file list
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
                      << "-e, -extended: print additional information into the final peak list.\n"
                      << "-log: This option will create a detailed log file in the program directory.\n";

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
        if ((argument == "-f") | (argument == "-files"))
        {
            // @todo loop until the first string which contains a "-" and add those files to the task list
            inSpecified = true;
            ++i;
            int control = i;
            pathInput = argv[i];
            while (std::filesystem::exists(pathInput))
            {
                if (pathInput.extension() != ".mzML")
                {
                    std::cerr << "Error: only .mzML files are supported. The file input has been skipped.\n";
                }
                else
                {
                    tasklist.push_back(pathInput);
                }
                ++i;
                pathInput = argv[i];
            }
            if (i == control)
            {
                std::cerr << "Error: argument -files was set, but no valid file supplied.\n";
                inSpecified = false;
            }

            continue;
        }
        if ((argument == "-tl") | (argument == "-tasklist"))
        {
            tasklistSpecified = true;
            ++i;
            pathInput = argv[i];
            if (!std::filesystem::exists(pathInput))
            {
                std::cerr << "Error: The selected tasklist file does not exist.\n"
                          << std::filesystem::canonical(pathInput);
            }
            std::ifstream file(pathInput);
            std::filesystem::path pathTemp;
            std::string line;
            bool firstline = true;
            while (std::getline(file, line))
            {
                if (firstline)
                {
                    if (line != "TASKFILE")
                    {
                        std::cerr << "Error: The taskfile is not formatted correctly. The first line of the taskfile must be \"TASKFILE\"\n";
                        exit(101);
                    }
                    firstline = false;
                    continue;
                }
                if (line[0] == char('#'))
                {
                    continue;
                }
                if (line[0] == char('-'))
                {
                    continue;
                }

                pathTemp = line;

                if (pathTemp.extension() != ".mzML")
                {
                    std::cerr << "Error: only .mzML files are supported. The tasklist entry has been skipped.\n";
                    continue;
                }
                if (!std::filesystem::exists(pathInput))
                {
                    std::cerr << "Error: a specified file does not exist. I has been removed from the tasklist.\n";
                }

                tasklist.push_back(pathTemp);
            }
            /*@todo
            the task file should contain a list of all files the program
            should run over. It should give access to additional settings,
            for example per-group output location and multiple recursion points.
            Lines can be commented out with a "#". empty lines are ignored.
            The file must start with a line containing only "TASKFILE" and
            should accept some standardised user data like name and project.
            Suggested options:
                ADDDATE: The current date (YYYYMMDD) is added with an underscore
                to the front of the filename
            */
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
            pathOutput = argv[i];
            if (!std::filesystem::exists(pathOutput))
            {
                std::cerr << "Error: the specified";
            }
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
        std::cerr << "Error: no input file supplied. Specify a file using the -f, -r or -tl flag. Execute qAlgorithms with the -h flag for more information\n";
        exit(1);
    }

    if (inSpecified & recursiveSearch)
    {
        std::cerr << "Error: Recursive search and target file are incompatible\n";
        exit(100);
    }
    if (tasklistSpecified & recursiveSearch)
    {
        std::cerr << "Error: Recursive search and task list are incompatible\n";
        exit(100);
    }
    if (inSpecified & tasklistSpecified)
    {
        std::cerr << "Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        // @todo
    }

#pragma region file processing
    for (std::filesystem::path pathSource : tasklist)
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