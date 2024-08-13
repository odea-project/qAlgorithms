// internal
// #include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_measurement_data_tensor.h"
#include "qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.h"
#include <iostream>
#include <chrono>
#include <fstream> // write peaks to file
#include <cstdlib>
#include <iomanip>
#include <filesystem> // printing absolute path in case read fails
#include <string>
#include <sstream>   // write peaks to file
#include <algorithm> // remove duplicates from task list
#include <map>       // remove duplicates from task list
#include <ranges>    // comparison of char arrays during duplicate filtering

namespace q
{
    void printPeaklist(std::vector<std::vector<q::DataType::Peak>> peaktable,
                       std::filesystem::path pathOutput, std::string filename, bool verbose, bool silent)
    {
        if (verbose)
        {
            filename += "_ex";
        }
        filename += "_peaks.csv";
        pathOutput /= filename;
        if (!silent)
        {
            std::cout << "writing peaks to: " << pathOutput << "\n\n";
        }

        std::fstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during peaklist printing. Program terminated.\n";
            exit(1); // @todo sensible error codes
        }

        if (verbose)
        {

            output << "ID,binID,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                   << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            int counter = 1;
            for (size_t i = 0; i < peaktable.size(); i++)
            {
                if (peaktable[i].empty())
                {
                    continue;
                }

                for (size_t j = 0; j < peaktable[i].size(); ++j)
                {
                    auto peak = peaktable[i][j];
                    char buffer[128];
                    sprintf(buffer, "%d,%d,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f,%0.8f\n",
                            counter, int(i + 1), peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
                            peak.area, peak.areaUncertainty, peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                    output << buffer;
                    ++counter;
                }
            }
        }
        else
        {
            output << "mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
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
        return;
    }

}

const std::string helpinfo = " help information:\n\n" // @todo std::format
                             "    qAlgorithms is a software project for non-target screening using mass spectrometry.\n"
                             "    For more information, visit our github page: https://github.com/odea-project/qAlgorithms.\n"
                             "    As of now (30.07.2024), only mzML files are supported. This program accepts the following command-line arguments:\n\n"
                             "      -h, -help:  open this help menu\n\n"
                             "    Input settings:\n"
                             "      Note that duplicate input files are removed by default.\n"
                             "      -f,  -files <PATH>:          specifies the input files. More than one file can be passed. Use as -f FILEPATH FILEPATH etc.\n" // @todo allow piping of file list
                             "      -tl, -tasklist <PATH>:       pass a list of file paths to the function. A tasklist can also contain directories\n"
                             "                                   to search recursively and output directories for different blocks of the input files.\n"
                             "                                   You can comment out lines by starting them with a \"#\"\n" // @todo update
                             "      -r,  -recursive <DIRECTORY>: recursive search for .mzML files in the specified directory.\n\n"
                             "    Output settings:\n"
                             "      The filename is always the original filename extended by the polarity.\n"
                             "      -o,  -output <DIRECTORY>:   directory into which all output files should be printed.\n"
                             "                                  If you want to print all results into the folder which contains the\n"
                             "                                  .mzML file, write \"#\". The default output is standard out,\n" // @todo is this a good idea?
                             "                                  unless you did not specify an input file. in that case, you will\n"
                             "                                  be prompted to enter the output location. If the specified\n"
                             "                                  location does not exist, a new directory is created.\n"
                             "      -ps, -printsummary:         print summarised information on the bins in addition to\n"
                             "                                  the peaktable. It is saved to your output directory\n"
                             "                                  under the name FILENAME_summary.csv\n"
                             "      -pb, -printbins:            If this flag is set, both bin summary information and\n"
                             "                                  all binned centroids will be printed to the output location\n"
                             "                                  in addition to the final peak table. The file ends in _bins.csv.\n"
                             "      -pp, -printpeaks:           print the peak tables as csv.\n"
                             "      -e,  -extended:             print additional information into the final peak list. You do not\n"
                             "                                  have to also set the -pp flag. Currently, only bin ID and peak ID\n"
                             "                                  are added to the output.\n"
                             "    \nProgram behaviour:\n"
                             "      -s, -silent:    do not print progress reports to standard out\n"
                             "      -v, -verbose:   print a detailed progress report to standard out\n"
                             "      -log:           This option will create a detailed log file in the program directory.\n"; // @todo

int main(int argc, char *argv[])
{

    std::string filename_input;
    volatile bool inSpecified = false;
    std::filesystem::path pathInput;
    volatile bool outSpecified = false;
    std::filesystem::path pathOutput;

    std::vector<std::filesystem::path> tasklist;

    volatile bool printPeaks = false;
    // ask for file if none are specified
    if (argc == 1)
    {
        // @todo check standard in and use that if it isn't empty
        std::cout << "Enter \"-h\" for a complete list of options.\n"
                     "Enter a filename to process that file. You must select a .mzML file:    ";
        std::cin >> filename_input;
        if ((filename_input == "-h") | (filename_input == "-help"))
        {
            std::cout << "    " << argv[0] << helpinfo;
            exit(0);
        }
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
        std::cout << "\nfile accepted, enter the output directory or \"#\" to use the input directory:  ";
        std::cin >> filename_input;
        printPeaks = true;
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

    volatile bool silent = false;
    volatile bool verboseProgress = false;
    volatile bool recursiveSearch = false;
    volatile bool tasklistSpecified = false;
    volatile bool printSummary = false;
    volatile bool printBins = false;
    volatile bool printExtended = false;

#pragma region cli arguments
    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") | (argument == "-help"))
        {
            std::cout << "\n    " << argv[0] << helpinfo;
            exit(0);
        }
        if ((argument == "-s") | (argument == "-silent"))
        {
            silent = true;
        }
        else if ((argument == "-v") | (argument == "-verbose"))
        {
            verboseProgress = true;
        }
        else if ((argument == "-f") | (argument == "-files"))
        {
            // loop until the first string which contains a "-" and add those files to the task list
            inSpecified = true;
            ++i;
            if ((i == argc))
            {
                std::cerr << "Error: argument -files was set, but no valid file supplied.\n";
                exit(101);
            }
            pathInput = std::filesystem::canonical(argv[i]);
            while (std::filesystem::exists(pathInput))
            {
                if (pathInput.extension() != ".mzML")
                {
                    std::cerr << "Warning: only .mzML files are supported. The file input has been skipped.\n";
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
                pathInput = std::filesystem::weakly_canonical(argv[i]);
                if (!std::filesystem::exists(pathInput))
                {
                    --i;
                    break;
                }
            }
        }
        else if ((argument == "-tl") | (argument == "-tasklist")) // @todo test this
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
                }

                if (line[0] == char('#'))
                {
                    continue;
                }
                else if (line[0] == char('$'))
                //@todo add argument checker
                {
                    continue;
                }

                pathTemp = line;

                if (pathTemp.extension() != ".mzML")
                {
                    std::cerr << "Warning: only .mzML files are supported. The tasklist entry " << pathTemp << " has been skipped.\n";
                    continue;
                }
                else if (!std::filesystem::exists(pathTemp))
                {
                    std::cerr << "Warning: the file " << pathTemp << " does not exist. I has been removed from the tasklist.\n";
                    continue;
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
        }
        else if ((argument == "-r") | (argument == "-recursive"))
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
            else if (std::filesystem::exists(pathInput.filename()))
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
            pathInput = "";
        }
        else if ((argument == "-o") | (argument == "-output"))
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
            else if (std::filesystem::exists(pathOutput.filename()))
            {
                std::cerr << "Error: the output directory cannot be a file.\n";
                exit(101);
            }
        }
        else if ((argument == "-ps") | (argument == "-printsummary"))
        {
            printSummary = true;
        }
        else if ((argument == "-pb") | (argument == "-printbins"))
        {
            printSummary = true;
            printBins = true;
        }
        else if ((argument == "-pp") | (argument == "-printpeaks"))
        {
            printPeaks = true;
        }
        else if ((argument == "-e") | (argument == "-extended"))
        {
            printExtended = true;
            printPeaks = true;
        }
        else if (argument == "-log")
        {
            //@todo write the executed command into the logfile
        }
        else
        {
            std::cerr << "Error: unknown argument " << argument << ", terminating program.\n";
            exit(1);
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
        silent = false;
    }

    // remove duplicates from tasklist
    // this will only remove duplicate file paths
    size_t prevsize = tasklist.size();
    std::sort(tasklist.begin(), tasklist.end());
    tasklist.erase(std::unique(tasklist.begin(), tasklist.end()), tasklist.end());

    int duplicateCount = prevsize - tasklist.size();

    std::multimap<size_t, std::filesystem::path> tasklist2;
    for (auto path : tasklist)
    {
        size_t filesize = std::filesystem::file_size(path);
        tasklist2.insert({filesize, path});
    }

    // remove duplicate files
    // create vector of unique keys (file sizes)
    // multimap is sorted by keys in ascending order
    // size_t prevsize = tasklist2.size();
    std::vector<size_t> keys;
    size_t prevkey = 0;
    for (const auto &pair : tasklist2)
    {
        size_t key = pair.first;
        if (key != prevkey)
        {
            prevkey = key;
            keys.push_back(key);
        }
    }

    tasklist.resize(0);

    for (size_t key : keys)
    {
        // files are hashed by filesize. First, search for sizes with more than one entry
        size_t numDubs = tasklist2.count(key);
        if (numDubs != 1)
        {
            // for every file, read the first 25 kb and then compare strings
            // this will work because most metadata should be covered by this, while not
            // reading in the first mass spectrum
            auto reduce = tasklist2.find(key);
            std::ifstream file1;
            const size_t readLength = 25000;

            if (numDubs == 2)
            {
                char first[readLength];
                char second[readLength];
                file1.open(reduce->second);
                file1.read(first, readLength);
                file1.close();
                std::advance(reduce, 1);
                file1.open(reduce->second);
                file1.read(second, readLength);
                file1.close();
                if (std::ranges::equal(first, second))
                {
                    // both entries are identical, only add one
                    ++duplicateCount;
                    tasklist.push_back(reduce->second);
                }
                else
                {
                    // files have the same size, but are different - add both
                    tasklist.push_back(reduce->second);
                    std::advance(reduce, -1);
                    tasklist.push_back(reduce->second);
                }
            }
            else
            {

                std::vector<std::filesystem::path> findDoubles;
                findDoubles.reserve(numDubs);
                std::vector<char[readLength]> firstChars(numDubs);
                bool keepIndex[numDubs] = {true};
                // transfer all relevant values to a different vector
                for (size_t i = 0; i < numDubs; i++)
                {
                    std::advance(reduce, i);
                    findDoubles.push_back(reduce->second);
                    // read the first 25 kb per file into the comparison vector
                    file1.open(reduce->second);
                    file1.read(firstChars[i], readLength);
                    file1.close();
                }
                // compare all values and note the duplicates
                for (size_t i = 0; i < numDubs; i++)
                {
                    for (size_t j = i + 1; j < numDubs; j++)
                    {
                        if (std::ranges::equal(firstChars[i], firstChars[j]))
                        {
                            // marks all duplicates exept the last as false
                            keepIndex[i] = false;
                            ++duplicateCount;
                            break;
                        }
                    }
                }
                // add all unique files to the tasklist
                for (size_t i = 0; i < numDubs; i++)
                {
                    if (keepIndex[i])
                    {
                        tasklist.push_back(findDoubles[i]);
                    }
                }
            }
        }
        else
        {
            // the file is unique
            tasklist.push_back(tasklist2.find(key)->second);
        }
    }

    if (duplicateCount > 0)
    {
        std::cerr << "Warning: removed " << duplicateCount << " duplicate files\n";
    }

    // check for duplicate file names and rename them automatically
    // extract filenames as strings to a vector for comparison

    bool duplicate_Name_different_Content = false;
    std::vector<std::string> itemNames;
    size_t tlSize = tasklist.size();
    for (size_t i = 0; i < tlSize; i++)
    {
        itemNames.push_back(tasklist[i].filename().string());
    }

    for (size_t i = 0; i < tlSize; i++)
    {
        std::vector<size_t> duplicates;

        for (size_t j = i + 1; j < tlSize; j++)
        {
            if (itemNames[j] != "")
            {
                duplicates.push_back(j);
                itemNames[j] = "";
            }
        }

        if (duplicates.size() != 0)
        {
            duplicate_Name_different_Content = true;
            std::string dupliName = itemNames[i];
            duplicates.push_back(i); // i was not added during the loop
            for (size_t j = 0; j < duplicates.size(); j++)
            {
                itemNames[duplicates[j]] = dupliName + "_" + std::to_string(j + 1);
            }
        }
    }
    if (duplicate_Name_different_Content)
    {
        std::cerr << "Warning: files with the same name but different content exist. "
                     "The output files have been renamed to <FILENAME>_1, <FILENAME>_2, etc.\n";
    }

#pragma endregion cli arguments

#pragma region file processing
    std::string filename;
    const std::vector<std::string> polarities = {"positive", "negative"}; // @todo make bool
    int counter = 0;
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
        else if (!silent && (printPeaks | printSummary))
        {
            std::cout << "printing output to: " << pathOutput;
        }

        // initialize qPeaks static variables and lcmsData object @todo constexpr
        q::Algorithms::qPeaks::initialize();

        auto timeStart = std::chrono::high_resolution_clock::now();
        if (!silent)
        {
            std::cout << "\nreading file " << counter + 1 << " of " << tasklist.size() << ":\n"
                      << pathSource << "\n... ";
        }

        sc::MZML data(std::filesystem::canonical(pathSource).string()); // create mzML object @todo change to use filesystem::path

        if (!data.loading_result)
        {
            std::cerr << "Error: the file is defective.\n";
            exit(101);
        }

        if (!silent)
        {
            std::cout << " file ok\n\n";
        }
        // update filename to name without duplicates
        pathSource.filename() = itemNames[counter];

        for (auto polarity : polarities)
        {
            filename = pathSource.stem().string();
            q::Algorithms::qPeaks qpeaks;              // create qPeaks object
            q::MeasurementData::TensorData tensorData; // create tensorData object
            // @todo add check if set polarity is correct
            std::vector<std::vector<q::DataType::Peak>> centroids =
                tensorData.findCentroids_MZML(qpeaks, data, true, polarity, 10); // read mzML file and find centroids via qPeaks
            // std::cout << "centroided\n";
            if ((centroids.size() < 5) & !silent)
            {
                std::cout << "skipping mode: " << polarity << "\n";
                continue;
            }
            std::cout << "Processing " << polarity << " peaks\n";
            // adjust filename to include polarity here
            filename += ("_" + polarity);

            q::Algorithms::qBinning::CentroidedData binThis = qpeaks.passToBinning(centroids);

            auto timeEnd = std::chrono::high_resolution_clock::now();

            if (!silent)
            {
                std::cout << "    produced " << binThis.lengthAllPoints << " centroids from " << centroids.size()
                          << " spectra in " << (timeEnd - timeStart).count() << " ns\n";
            }

            timeStart = std::chrono::high_resolution_clock::now();
            std::vector<q::Algorithms::qBinning::EIC> binnedData = q::Algorithms::qBinning::performQbinning(
                binThis, pathOutput, filename, 3, !verboseProgress, printSummary, printBins); // set maxdist here
            timeEnd = std::chrono::high_resolution_clock::now();

            if (!silent)
            {
                std::cout << "    assembled " << binnedData.size() << " bins in " << (timeEnd - timeStart).count() << " ns\n";
            }

            timeStart = std::chrono::high_resolution_clock::now();
            // every subvector of peaks corresponds to the bin ID
            auto peaks = tensorData.findPeaks_QBIN(qpeaks, binnedData);

            timeEnd = std::chrono::high_resolution_clock::now();
            if (!silent)
            {
                int peakCount = 0;
                for (size_t i = 0; i < peaks.size(); i++)
                {
                    peakCount += peaks[i].size();
                }

                std::cout << "    found " << peakCount << " peaks in " << (timeEnd - timeStart).count() << " ns\n";
            }

            // std::vector<std::vector<q::DataType::Peak>> processPeaks = tensorData.remove_unique_ptr(qpeaks, peaks);

            // continue;

            if (printPeaks)
            {
                q::printPeaklist(peaks, pathOutput, filename, printExtended, silent);
            }
            // @todo add peak grouping here

            counter++;
        }
    }
    if (!silent)
    {
        std::cout << "Completed data processing on " << tasklist.size() << " files.\n\n";
    }

    return 0;
}