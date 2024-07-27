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
namespace q
{
    bool printPeaklist(std::vector<std::vector<q::DataType::Peak>> peaktable,
                       std::filesystem::path pathOutput, std::string filename, bool verbose)
    {
        if (verbose)
        {
            filename += "_ex";
        }
        pathOutput.filename() = filename;
        pathOutput.extension() = ".csv";
        std::fstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            return 1;
        }

        if (verbose)
        {

            output << "ID,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                   << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            int counter = 0;
            for (size_t i = 0; i < peaktable.size(); i++)
            {
                for (size_t j = 0; j < peaktable[i].size(); ++j)
                {
                    auto peak = peaktable[i][j];
                    char buffer[128];
                    sprintf(buffer, "%d,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f\n",
                            counter, peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
                            peak.area, peak.areaUncertainty, peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                    output << buffer;
                    ++counter;
                }
            }
        }
        else
        {
            output << "ID,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                   << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            int counter = 0;
            for (size_t i = 0; i < peaktable.size(); i++)
            {
                for (size_t j = 0; j < peaktable[i].size(); ++j)
                {
                    auto peak = peaktable[i][j];
                    char buffer[128];
                    sprintf(buffer, "%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f\n",
                            peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
                            peak.area, peak.areaUncertainty, peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                    output << buffer;
                    ++counter;
                }
            }
        }

        file_out << output.str();
        file_out.close();
        return 0;
    }

}

const std::string helpinfo = " help information:\n"
                             "qAlgorithms is a software project for non-target screening using mass spectrometry."
                             "For more information, visit our github page: https://github.com/odea-project/qAlgorithms\n"
                             "As of now (23.07.2024), only mzML files are supported. This program accepts the following command-line arguments:"
                             "-h, -help: open this help menu\n"
                             "-s, -silent: do not print progress reports to standard out\n"
                             "-v, -verbose: print a detailed progress report to standard out"
                             "-f, -files: specifies the input files. More than one file can be passed. Use as -f FILEPATH FILEPATH etc.\n" // @todo allow piping of file list
                             "-tl, -tasklist: pass a list of file paths to the function. A tasklist can also contain directories "
                             "to search recursively and output directories for different blocks of the input files. "
                             "You can comment out lines by starting them with a \"#\"\n" // @todo update
                             "-r, -recursive <DIRECTORY>: recursive search for .mzML files in the specified directory.\n"
                             "-o, -output <DIRECTORY>: directory into which all output files should be printed. "
                             "If you want to print all results into the folder which "
                             "contains the .mzML file, write \"#\". The default output is standard out, "
                             "unless you did not specify an input file. in that case, you will be prompted to "
                             "enter the output location. If the specified location does not exist, a new directory is created\n"
                             "-ps, -printsummary: print summarised information on the bins in addition to "
                             "the peaktable. It is saved to your output directory using as FILENAME_summary.csv\n"
                             "-pb, -printbins: If this flag is set, both bin summary information and "
                             "all binned centroids will be printed to the output location in addition "
                             "to the final peak table. The files end in _summary.csv and _bins.csv.\n"
                             "-pp, -printpeaks: print the peak tables as csv.\n"
                             "-e, -extended: print additional information into the final peak list. You do not"
                             " have to also set the -pp flag.\n"
                             "-log: This option will create a detailed log file in the program directory.\n";

int main(int argc, char *argv[])
{
    std::string filename_input;
    bool inSpecified = false;
    std::filesystem::path pathInput;
    bool outSpecified = false;
    std::filesystem::path pathOutput;
    std::string filename;
    std::vector<std::filesystem::path> tasklist;
    // ask for file if none are specified
    if (argc == 1)
    {
        // @todo check standard in and use that if it isn't empty
        std::cout << "Start qAlgorithms with the -h flag for a complete list of options. "
                     "Enter a filename to process that file. You must select a .mzML file:    ";
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
    bool verboseProgress = false;
    bool recursiveSearch = false;
    bool tasklistSpecified = false;
    bool printSummary = false;
    bool printBins = false;
    bool printPeaks;
    bool printExtended = false;

#pragma region cli arguments
    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") | (argument == "-help"))
        {
            std::cout << argv[0] << helpinfo;

            exit(0);
        }
        if ((argument == "-s") | (argument == "-silent"))
        {
            silent = true;
            continue;
        }
        if ((argument == "-w") | (argument == "-wordy"))
        {
            verboseProgress = true;
            continue;
        }
        if ((argument == "-f") | (argument == "-files"))
        {
            // loop until the first string which contains a "-" and add those files to the task list
            inSpecified = true;
            ++i;
            pathInput = argv[i];
            if ((i == argc) | !(std::filesystem::exists(pathInput)))
            {
                std::cerr << "Error: argument -files was set, but no valid file supplied.\n";
                inSpecified = false;
            }
            while (std::filesystem::exists(pathInput))
            {
                pathInput = argv[i];
                if (pathInput.extension() != ".mzML")
                {
                    std::cerr << "Error: only .mzML files are supported. The file input has been skipped.\n";
                }
                else
                {
                    tasklist.push_back(pathInput);
                }
                ++i;
                if (i == argc)
                {
                    break;
                }
            }

            continue;
        }
        if ((argument == "-tl") | (argument == "-tasklist")) // @todo test this
        {
            tasklistSpecified = true;
            ++i;
            if (i == argc)
            {
                std::cerr << "Error: no task list specified.\n";
                exit(101);
            }
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
                if (line[0] == char('$'))
                //@todo add argument checker
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
            if (i == argc)
            {
                std::cerr << "Error: no target directory specified.\n";
                exit(101);
            }
            pathInput = argv[i];
            if (!std::filesystem::exists(pathInput))
            {
                std::cerr << "Error: the target path for recursive search does not exist.\n";
                exit(101);
            }
            if (std::filesystem::exists(pathInput.filename()))
            {
                std::cerr << "Error: the target directory for recursive search cannot be a file.\n";
            }
            int fileCounter = 0;
            for (auto const &dir_entry : std::filesystem::recursive_directory_iterator(pathInput))
            {
                pathInput = dir_entry;

                if (pathInput.extension() == ".mzML")
                {
                    tasklist.push_back(std::filesystem::canonical(pathInput));
                    ++fileCounter;
                }
            }
            if (fileCounter == 0)
            {
                std::cerr << "Warning: no files were found by recursive search.\n";
            }

            continue;
        }
        if ((argument == "-o") | (argument == "-output"))
        {
            // @todo
            outSpecified = true;
            ++i;
            if (i == argc)
            {
                std::cerr << "Error: no output directory specified.\n";
                exit(101);
            }
            pathOutput = argv[i];
            if (!std::filesystem::exists(pathOutput))
            {
                std::cerr << "Error: the specified output path does not exist.\n";
                exit(101);
            }
            if (std::filesystem::exists(pathOutput.filename()))
            {
                std::cerr << "Error: the output directory cannot be a file.\n";
                exit(101);
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
        if ((argument == "-pp") | (argument == "-printpeaks"))
        {
            printPeaks = true;
            continue;
        }
        if ((argument == "-e") | (argument == "-extended"))
        {
            printExtended = true;
            printPeaks = true;
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
        std::cerr << "Error: no input file supplied. Specify a file using the -f, -r or "
                  << "-tl flag. Execute qAlgorithms with the -h flag for more information\n";
        exit(1);
    }
    if (recursiveSearch & !outSpecified)
    {
        std::cerr << "Error: Recursive search requires you to specify an output directory. Set -o to \"#\" "
                  << "to use the location of every processed file. (currently unsupported)\n";
        exit(100);
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
        std::cerr << "Warning: Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        // @todo
    }
    if (silent & verboseProgress)
    {
        std::cerr << "Warning: -verbose overrides -silent\n";
    }
#pragma endregion cli arguments

#pragma region file processing
    const std::vector<std::string> polarities = {"positive", "negative"};
    for (std::filesystem::path pathSource : tasklist)
    {

        bool printOnce = true;
        if (!outSpecified)
        {
            if (printOnce)
            {
                printOnce = false;
                std::cerr << "Warning: no output location specified, printing output to file location\n";
            }

            pathOutput = pathSource.parent_path(); // @todo include route to standard out
        }

        // initialize qPeaks static variables and lcmsData object @todo constexpr
        q::Algorithms::qPeaks::initialize();

        auto timeStart = std::chrono::high_resolution_clock::now();
        if (!silent)
        {
            std::cout << "\nreading file " << pathSource << " ...";
        }

        sc::MZML data(std::filesystem::canonical(pathSource).string()); // create mzML object @todo change to use filesystem::path
        if (!silent)
        {
            std::cout << " file ok\n";
        }
        for (auto polarity : polarities)
        {
            q::Algorithms::qPeaks qpeaks;              // create qPeaks object
            q::MeasurementData::TensorData tensorData; // create tensorData object
            // @todo add check if set polarity is correct
            std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> centroids =
                tensorData.findCentroids_MZML(qpeaks, data, true, polarity, 10); // read mzML file and find centroids via qPeaks
            // std::cout << "centroided\n";
            if ((centroids.size() < 5) & !silent)
            {
                std::cout << "skipping mode: " << polarity << "\n";
                continue;
            }
            // adjust filename to include polarity here
            filename += polarity;

            q::Algorithms::qBinning::CentroidedData binThis = qpeaks.passToBinning(centroids);
            auto timeEnd = std::chrono::high_resolution_clock::now();

            if (!silent)
            {
                std::cout << "produced " << binThis.lengthAllPoints << " centroids from " << centroids.size()
                          << " spectra in " << (timeEnd - timeStart).count() << " ns\n\n";
            }

            timeStart = std::chrono::high_resolution_clock::now();
            std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(
                binThis, pathOutput, filename, 6, !verboseProgress, printSummary, printBins);
            timeEnd = std::chrono::high_resolution_clock::now();

            if (!silent)
            {
                std::cout << "assembled " << binnedData.size() << " bins in " << (timeEnd - timeStart).count() << " ns\n\n";
            }

            timeStart = std::chrono::high_resolution_clock::now();
            auto peaks = tensorData.findPeaks_QBIN(qpeaks, binnedData);
            timeEnd = std::chrono::high_resolution_clock::now();
            if (!silent)
            {
                std::cout << "found " << peaks.size() << " peaks in " << (timeEnd - timeStart).count() << " ns\n\n";
                std::cout << "writing peaks to file... \n";
            }

            // std::vector<std::vector<q::DataType::Peak>> processPeaks = tensorData.remove_unique_ptr(qpeaks, peaks);

            // continue;

            if (printPeaks)
            {
                if (!q::printPeaklist(peaks, pathOutput, filename, printExtended))
                {
                    std::cerr << "Error: could not open output path during peaklist printing. Program terminated.\n";
                    exit(1); // @todo sensible error codes
                }
            }
#pragma region print peaktable
            //@todo understand why this does not work as an individual function
            // if (printPeaks)
            // {
            //     if (printExtended)
            //     {
            //         filename += "_ex";
            //     }
            //     pathOutput.filename() = filename;
            //     pathOutput.extension() = ".csv";
            //     std::fstream file_out;
            //     std::stringstream output;
            //     file_out.open(pathOutput, std::ios::out);
            //     if (!file_out.is_open())
            //     {
            //         return 1;
            //     }

            //     if (printExtended)
            //     {

            //         output << "ID,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
            //                << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            //         int counter = 0;
            //         for (size_t i = 0; i < peaks.size(); i++)
            //         {
            //             for (size_t j = 0; j < peaks[i].size(); ++j)
            //             {
            //                 auto peak = *peaks[i][j];
            //                 char buffer[128];
            //                 sprintf(buffer, "%d,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f\n",
            //                         counter, peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
            //                         peak.area, peak.areaUncertainty, peak.dqsCen, peak.dqsBin, peak.dqsPeak);
            //                 output << buffer;
            //                 ++counter;
            //             }
            //         }
            //     }
            //     else
            //     {
            //         output << "mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
            //                << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            //         int counter = 0;
            //         for (size_t i = 0; i < peaks.size(); i++)
            //         {
            //             for (size_t j = 0; j < peaks[i].size(); ++j)
            //             {
            //                 auto peak = *peaks[i][j];
            //                 char buffer[128];
            //                 sprintf(buffer, "%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f\n",
            //                         peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
            //                         peak.area, peak.areaUncertainty, peak.dqsCen, peak.dqsBin, peak.dqsPeak);
            //                 output << buffer;
            //                 ++counter;
            //             }
            //         }
            //     }

            //     file_out << output.str();
            //     file_out.close();
            // }
#pragma endregion print peaktable
        }
    }
    if (!silent)
    {
        std::cout << "Completed data processing on " << tasklist.size() << " files.\n\n";
    }

    return 0;
}