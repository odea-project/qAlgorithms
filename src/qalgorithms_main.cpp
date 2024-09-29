// internal
// #include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_measurement_data_tensor.h"
#include "qalgorithms_qpeaks.h"

// external
#include "../external/StreamCraft/src/StreamCraft_mzml.hpp"
#include <iostream>
#include <chrono>
#include <fstream> // write peaks to file
#include <cstdlib>
#include <iomanip>
#include <filesystem> // printing absolute path in case read fails
#include <string>
#include <sstream>   // write peaks to file
#include <algorithm> // remove duplicates from task list

namespace q
{

    // this is set to -5 to track if the argument was set or not. Since a negative
    // number during input causes the program to exit, the warning only triggers
    // if the ppm value is positve.
    double MeasurementData::ppm_for_precentroided_data = -5;

    struct ProfilePoint
    {
        float mz;
        float rt;
        int scan;
        float intensity;
    };

    void printPeaklist(std::vector<std::vector<q::DataType::Peak>> peaktable,
                       std::filesystem::path pathOutput, std::string filename,
                       std::vector<Algorithms::qBinning::EIC> originalBins,
                       bool verbose, bool silent, bool prebinning, bool skipError) // @todo mz range, rt range
    {
        if (verbose)
        {
            filename += "_ex";
        }
        if (prebinning)
        {
            filename += "_centroids.csv";
        }
        else
        {
            filename += "_peaks.csv";
        }

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
            if (skipError)
            {
                return;
            }
            else
            {
                exit(1); // @todo sensible error codes
            }
        }

        if (verbose)
        {

            if (prebinning)
            {
                output << "ID,binID,startIndex,endIndex,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                       << "area,areaUncertainty,height,heightUncertainty,dqsCen,dqsBin,dqsPeak\n";
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
                        sprintf(buffer, "%d,%d,%d,%d,%0.6f,%0.6f,%0.4f,%0.4f,%0.3f,%0.3f,%0.5f,%0.5f,%0.5f\n",
                                counter, int(i + 1), peak.idxPeakStart, peak.idxPeakEnd, peak.mz, peak.mzUncertainty,
                                peak.retentionTime, peak.retentionTimeUncertainty, peak.area, peak.areaUncertainty,
                                peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                        output << buffer;
                        ++counter;
                    }
                }
            }
            else
            {
                // @todo add peak borders
                output << "ID,binID,binIdxStart,binIdxEnd,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                       << "lowestRetentionTime,highestRetentionTime,area,areaUncertainty,height,heightUncertainty,"
                       << "binTestCode,dqsCen,dqsBin,dqsPeak\n";
                unsigned int counter = 1;
                for (size_t i = 0; i < peaktable.size(); i++)
                {
                    if (!peaktable[i].empty())
                    {
                        std::vector<float> RTs = originalBins[i].rententionTimes;
                        for (size_t j = 0; j < peaktable[i].size(); ++j)
                        {

                            auto peak = peaktable[i][j];
                            char buffer[256];
                            sprintf(buffer, "%d,%d,%d,%d,%0.6f,%0.6f,%0.4f,%0.4f,%0.4f,%0.4f,%0.3f,%0.3f,%0.3f,%0.3f,%d,%0.5f,%0.5f,%0.5f\n",
                                    counter, int(i + 1), peak.idxPeakStart, peak.idxPeakEnd, peak.mz, peak.mzUncertainty,
                                    peak.retentionTime, peak.retentionTimeUncertainty, RTs[peak.idxPeakStart], RTs[peak.idxPeakEnd],
                                    peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, int(originalBins[i].errorcode),
                                    peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                            output << buffer;
                            ++counter;
                        }
                    }
                }
                // std::advance(currentBin, 1);
            }
        }
        else
        {
            output << "ID,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                   << "area,areaUncertainty,dqsCen,dqsBin,dqsPeak\n ";
            unsigned int counter = 1;
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
                    sprintf(buffer, "%d,%0.6f,%0.6f,%0.4f,%0.4f,%0.3f,%0.3f,%0.5f,%0.5f,%0.5f\n",
                            counter, peak.mz, peak.mzUncertainty, peak.retentionTime, peak.retentionTimeUncertainty,
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

    void printProfilePoints(std::vector<std::vector<ProfilePoint>> peakComponents,
                            std::filesystem::path pathOutput, std::string filename,
                            bool silent, bool skipError)
    {
        filename += "_subprofiles.csv";
        pathOutput /= filename;
        if (!silent)
        {
            std::cout << "writing profile information of peaks to: " << pathOutput << "\n\n";
        }

        std::fstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during peaklist printing. Program terminated.\n";
            if (skipError)
            {
                return;
            }
            else
            {
                exit(1); // @todo sensible error codes
            }
        }
        output << "peakID,mz,retentionTime,scanNumber,intensity\n";
        for (size_t i = 0; i < peakComponents.size(); i++)
        {
            for (auto point : peakComponents[i])
            {
                char buffer[64];
                sprintf(buffer, "%zu,%0.8f,%0.8f,%d,%0.8f\n",
                        i, point.mz, point.rt, point.scan, point.intensity);
                output << buffer;
            }
        }
        file_out << output.str();
        file_out.close();
        return;
    }

    std::vector<std::filesystem::path> controlInput(std::vector<std::string> inputTasks, const std::string filetype, bool skipError)
    {
        namespace fs = std::filesystem;
        std::vector<fs::path> outputTasks;
        outputTasks.reserve(inputTasks.size());
        struct TaskEntry
        {
            fs::path path;
            uintmax_t filesize;
        };
        std::vector<TaskEntry> tasklist;
        tasklist.reserve(inputTasks.size());
        // find all valid inputs and add them to the task list
        for (std::string inputPath : inputTasks)
        {
            fs::path currentPath{inputPath};
            if (!fs::exists(currentPath))
            {
                std::cerr << "Warning: the file \"" << inputPath << "\" does not exist.\n";
                if (skipError)
                {
                    continue;
                }
                else
                {
                    exit(1);
                }
            }
            currentPath = fs::canonical(currentPath);
            auto status = fs::status(currentPath);

            if (status.type() == fs::file_type::regular)
            {
                if (currentPath.extension() != filetype)
                {
                    std::cerr << "Warning: only " << filetype << " files are supported. The file \""
                              << inputPath << "\" has been skipped.\n";
                    continue;
                    // @todo consider terminating the program here
                }
                tasklist.push_back(TaskEntry{currentPath, fs::file_size(currentPath)});
            }
            else if (status.type() == fs::file_type::directory)
            {
                // recursively search for relevant files
                for (auto const &dir_entry : fs::recursive_directory_iterator(currentPath))
                {
                    fs::path recursivePath = fs::canonical(dir_entry);
                    if (recursivePath.extension() == filetype)
                    {
                        tasklist.push_back(TaskEntry{recursivePath, fs::file_size(recursivePath)});
                    }
                }
            }
            else
            {
                std::cerr << "Warning: \"" << inputPath << "\" is not a supported file or directory. The file has been skipped.\n";
            }
        }
        // remove duplicate files
        size_t tasknumber = tasklist.size();
        unsigned int removedEntries = 0;
        if (tasknumber == 0)
        {
            std::cerr << "Error: no valid files selected.\n";
            exit(1);
        }
        std::sort(tasklist.begin(), tasklist.end(), [](const TaskEntry lhs, const TaskEntry rhs)
                  { return lhs.filesize < rhs.filesize; });
        size_t prevsize = tasklist[0].filesize;
        for (size_t i = 1; i < tasknumber; i++)
        {
            if (tasklist[i].filesize == prevsize)
            {
                size_t readLength = 25000;
                std::ifstream fileConflict;
                char firstChars[readLength];
                char secondChars[readLength];
                fileConflict.open(tasklist[i - 1].path);
                fileConflict.read(firstChars, readLength);
                fileConflict.close();
                // std::string s = firstChars;
                // auto hash1 = std::hash<std::string>{}(s);

                fileConflict.open(tasklist[i].path);
                fileConflict.read(firstChars, readLength);
                fileConflict.close();
                // s = firstChars;
                // auto hash2 = std::hash<std::string>{}(s);
                // if (hash1 == hash2)
                // {
                //     tasklist[i - 1].filesize = 0; // always keeps last so comparison by file size works
                //     ++removedEntries;
                // }
                bool sameFiles = true;
                for (size_t j = 0; j < readLength; j++)
                {
                    if (firstChars[j] != secondChars[j])
                    {
                        sameFiles = false;
                        break;
                    }
                }
                if (sameFiles)
                {
                    tasklist[i - 1].filesize = 0; // always keeps last so comparison by file size works
                    ++removedEntries;
                }
            }
            prevsize = tasklist[i].filesize;
        }
        if (removedEntries > 0)
        {
            std::cerr << "Warning: removed " << removedEntries << " duplicate input files from processing queue.\n";
        }
        for (auto entry : tasklist)
        {
            if (entry.filesize > 0)
            {
                outputTasks.push_back(entry.path);
            }
        }
        return (outputTasks);
    }

    bool massTraceStable(std::vector<float> massesBin, int idxStart, int idxEnd)
    {
        size_t peaksize = idxEnd - idxStart + 1;
        // std::cout << idxStart << ", " << idxEnd << ", " << peaksize << "\n";
        std::vector<float> massesPeak;
        for (size_t i = 0; i < peaksize; i++)
        {
            massesPeak.push_back(massesBin[idxStart + i]);
        }
        std::sort(massesPeak.begin(), massesPeak.end());

        // critval @todo make this one function
        float mean = 0;
        float stddev = 0;
        for (size_t i = 0; i < peaksize; i++)
        {
            mean += massesPeak[i];
        }
        mean /= peaksize;
        for (size_t i = 0; i < peaksize; i++)
        {
            stddev += (massesPeak[i] - mean) * (massesPeak[i] - mean);
        }
        stddev = sqrt(stddev / (peaksize - 1));

        float vcrit = 3.05037165842070 * pow(log(peaksize), (-0.4771864667153)) * stddev;
        for (size_t i = 1; i < peaksize; i++)
        {
            [[unlikely]] if (massesPeak[i] - massesPeak[i - 1] > vcrit)
            {
                // std::cerr << "Bad mass trace found\n"; // @todo remove error message
                return false;
            }
        }
        return true;
    }

    const std::string helpinfo = " help information:\n\n" // @todo std::format
                                 "    qAlgorithms is a software project for non-target screening using mass spectrometry.\n"
                                 "    For more information, visit our github page: https://github.com/odea-project/qAlgorithms.\n"
                                 "    As of now (2024-09-03), only mzML files are supported. This program accepts the following command-line arguments:\n\n"
                                 "      -h, -help:  open this help menu\n\n"
                                 "    Input settings:\n"
                                 "      Note that duplicate input files are removed by default, even when they have a different name.\n"
                                 "      -i,  -input <PATH> [PATH]   input files or directories in which to recursively search for .mzML files.\n"
                                 "                                  you can enter any number of targets, as long as no file starts with a \"-\"\n"
                                 "                                  or contains two dots in a row. It is possible to use the -i flag multiple\n"
                                 "                                  times within one execution.\n"
                                 "      -tl, -tasklist <PATH>:      pass a list of file paths to the function. A tasklist can also contain directories\n"
                                 "                                  to search recursively and output directories for different blocks of the input files.\n"
                                 "                                  You can comment out lines by starting them with a \"#\".\n" // @todo update
                                 "    Output settings:\n"
                                 "      The filename is always the original filename extended by the polarity and the processing step.\n"
                                 "      -o,  -output <DIRECTORY>:   directory into which all output files should be printed.\n"
                                 "      -pc, -printcentroids:       print all centroids produced after the first run of qcentroids.\n"
                                 "      -ps, -printsummary:         print summarised information on the bins in addition to\n"
                                 "                                  the peaktable. It is saved to your output directory\n"
                                 "                                  under the name FILENAME_summary.csv.\n"
                                 "      -pb, -printbins:            If this flag is set, both bin summary information and\n"
                                 "                                  all binned centroids will be printed to the output location\n"
                                 "                                  in addition to the final peak table. The file ends in _bins.csv.\n"
                                 "      -pp, -printpeaks:           print the peak tables as csv.\n"
                                 "      -e,  -extended:             print additional information into the final peak list. You do not\n"
                                 "                                  have to also set the -pp flag. The extended output includes the\n"
                                 "                                  ID of the bin a given peak was found in, its start and end\n"
                                 "                                  position (by index) within the bin, the lowest and highest retenetion \n"
                                 "                                  times in the peak and the intensity as apex height.\n"
                                 "      -sp, -subprofile:           instead of the peaks, print all proflie-mode data points which\n"
                                 "                                  were used to create the final peaks. This does not return any quality\n"
                                 "                                  scores. Only use this option when reading in prodile mode files.\n"
                                 "      -pa, -printall:             print all availvable resutlts. You will probably not need to do this.\n"
                                 "    Program behaviour:\n"
                                 "      -s, -silent:    do not print progress reports to standard out.\n"
                                 "      -v, -verbose:   print a detailed progress report to standard out.\n"
                                 "      -skip-error:    if processing fails, the program will not exit and instead start processing\n"
                                 "                      the next file in the tasklist.\n"
                                 "      -log:           This option will create a detailed log file in the program directory.\n" // @todo
                                 "    Analysis options:\n"
                                 "      -MS2: also process MS2 spectra (not implemented yet)\n" // @todo
                                 "      -ppm <number>:  this sets the centroid error when reading in pre-centroided data\n"
                                 "                      with qAlgorithms to <number> * 10^-6 * m/z of the centroid. We recommend\n"
                                 "                      you always use the centroiding algorithm implemented in qAlgorithms.\n"
                                 "                      By default, this value is set to 5.\n";
}

int main(int argc, char *argv[])
{

    std::string filename_input;
    volatile bool inSpecified = false;
    volatile bool outSpecified = false;
    std::filesystem::path pathOutput;
    std::vector<std::string> suppliedPaths;

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
            std::cout << "    " << argv[0] << q::helpinfo << "\n\n";
            exit(0);
        }
        std::cout << filename_input << "\n";
        suppliedPaths.push_back(filename_input);
        // filename_input = "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090901_H2O_1_pos.mzML";
        std::cout << "\nEnter the output directory or \"#\" to use the input directory:  ";
        std::string filename_output;
        std::cin >> filename_output;
        printPeaks = true;
        if (filename_output == "#")
        {
            std::filesystem::path outDir = suppliedPaths[0];
            pathOutput = outDir.parent_path();
        }
        else if ((filename_output == "-h") | (filename_output == "-help"))
        {
            std::cout << "    " << argv[0] << q::helpinfo << "\n\n";
            exit(0);
        }

        else
        {
            pathOutput = filename_input;
        }
        inSpecified = true;
        outSpecified = true;
    }

    // if only one argument is given, check if it is any variation of help
    // if this is not the case, try to use the string as a filepath
#pragma region cli arguments
    volatile bool silent = false;
    volatile bool verboseProgress = false;
    volatile bool skipError = false;
    volatile bool tasklistSpecified = false;
    volatile bool printSummary = false;
    volatile bool printBins = false;
    volatile bool printExtended = false;
    volatile bool printCentroids = false;
    volatile bool printSubProfile = false;

    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") | (argument == "-help"))
        {
            std::cout << "\n    " << argv[0] << q::helpinfo;
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
        else if ((argument == "-i") | (argument == "-input"))
        {
            ++i;
            if (i == argc)
            {
                std::cerr << "Error: argument -input was set, but no valid file supplied.\n";
                exit(101);
            }
            std::string inputString = argv[i];
            if (inputString[0] == '-')
            {
                std::cerr << "Error: argument -input was set, but no valid file supplied.\n";
                exit(101);
            }
            while ((inputString[0] != '-') && (i < argc))
            {
                suppliedPaths.push_back(inputString);
                ++i;
                inputString = argv[i];
                if (inputString[0] == '-')
                {
                    --i;
                }
            }

            inSpecified = true;
        }
        else if ((argument == "-tl") | (argument == "-tasklist")) // @todo test this
        {
            std::cerr << "The tasklist functionality is not implemented currently.Please use -i.\n";
            exit(0);
            tasklistSpecified = true;
            ++i;
            if (i == argc)
            {
                std::cerr << "Error: no task list specified.\n";
                exit(101);
            }
            std::filesystem::path pathInput = argv[i];
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
        else if ((argument == "-o") | (argument == "-output"))
        {
            // @todo
            if (outSpecified)
            {
                std::cerr << "Error: two output locations specified. For complex output location "
                          << "structures, it is recommended you use the tasklist input.";
                exit(101); // @todo
            }

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
            else if (std::filesystem::status(pathOutput).type() != std::filesystem::file_type::directory)
            {
                std::cerr << "Error: the output location must be a directory.\n";
                exit(101);
            }
        }
        else if (argument == "-ppm")
        {
            ++i;
            if (i == argc)
            {
                std::cerr << "Error: -ppm set, but no centroid error specified.\n";
                exit(1);
            }

            double modifiedPPM;
            try
            {
                modifiedPPM = std::stod(argv[i]);
            }
            catch (std::invalid_argument const &)
            {
                std::cerr << "Error: the centroid error cannot be set to \"" << argv[i] << "\" ppm.\n";
                exit(1);
            }
            if (modifiedPPM <= 0)
            {
                std::cerr << "Error: the centroid error must be greater than 0.";
                exit(1);
            }
            if (modifiedPPM > 1000000)
            {
                std::cerr << "Error: the centroid error is set to 100% or greater (\"" << argv[i] << "\").";
                exit(1);
            }
            if (std::isnan(modifiedPPM))
            {
                std::cerr << "Error: the centroid error must be a number, but was set to NaN.";
                exit(1);
            }

            q::MeasurementData::ppm_for_precentroided_data = modifiedPPM;
        }
        else if ((argument == "-pc") | (argument == "-printcentroids"))
        {
            printCentroids = true;
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
        else if ((argument == "-sp") | (argument == "-subprofile"))
        {
            printSubProfile = true;
        }
        else if ((argument == "-pa") | (argument == "-printall"))
        {
            printCentroids = true;
            printBins = true;
            printSummary = true;
            printExtended = true;
            printPeaks = true;
            printSubProfile = true;
        }
        else if (argument == "-log")
        {
            std::cerr << "Logging is not implemented yet.\n";
            //@todo write the executed command into the logfile
        }
        else if (argument == "-skip-error")
        {
            std::cerr << "Warning: processing will ignore defective files.\n";
            skipError = true;
        }
        else
        {
            std::cerr << "Error: unknown argument " << argument << ", terminating program.\n";
            exit(1);
        }
    }

    if (!inSpecified & !tasklistSpecified)
    {
        std::cerr << "Error: no input file supplied. Specify a file or directorey using the -i or "
                  << "-tl flag. Execute qAlgorithms with the -h flag for more information.\n";
        exit(1);
    }
    if (!outSpecified)
    {
        std::cerr << "Error: You must specify an output directory.\n";
        exit(100);
    }
    if (inSpecified & tasklistSpecified)
    {
        std::cerr << "Warning: Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        // @todo
    }
    if (silent & verboseProgress)
    {
        std::cerr << "Warning: -verbose overrides -silent.\n";
        silent = false;
    }
    if (!((printCentroids | printSummary) | printPeaks))
    {
        std::cerr << "Warning: no output files will be written.\n";
    }

    // the final task list contains only unique files, sorted by filesize
    tasklist = q::controlInput(suppliedPaths, ".mzML", skipError);

#pragma endregion cli arguments

    // Temporary diagnostics file creation, rework this into the log function?
    std::filesystem::path pathLogging{argv[0]};
    // pathLogging.filename() = "log_qBinning_beta.csv";
    pathLogging = std::filesystem::canonical(pathLogging.parent_path());
    pathLogging /= "log_qBinning_beta.csv";
    if (std::filesystem::exists(pathLogging))
    {
        std::cerr << "Warning: the processing log has been overwritten\n";
    }
    std::fstream logWriter;
    logWriter.open(pathLogging, std::ios::out);
    logWriter << "filename, numSpectra, numCentroids, meanDQSC, numBins_empty, numBins_one,"
                 " numBins_more, meanDQSB, numFeatures, badFeatures, impossibleRegs, meanDQSF\n"; // , testFailedPeak, testPassedPeak, testPassedTotal
    logWriter.close();

#pragma region file processing
    std::string filename;
    const std::vector<std::string> polarities = {"positive", "negative"}; // @todo make bool
    int counter = 1;
    // @todo add dedicated diagnostics function
    // std::cout << "numCens,numBins,numPeaks,numLoadedBins,numNarrowBin,\n";
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
            std::cout << "\nreading file " << counter << " of " << tasklist.size() << ":\n"
                      << pathSource << "\n... ";
        }

        sc::MZML data(std::filesystem::canonical(pathSource).string()); // create mzML object @todo change to use filesystem::path

        if (!data.loading_result)
        {
            std::cerr << "Error: the file is defective.\n";
            if (skipError)
            {
                ++counter;
                continue;
            }
            else
            {
                exit(101);
            }
        }

        if (!silent)
        {
            std::cout << " file ok\n";
        }
        // update filename to name without duplicates @todo find better solution / should this happen?

        for (auto polarity : polarities)
        {
            filename = pathSource.stem().string();
            q::Algorithms::qPeaks qpeaks;              // create qPeaks object
            q::MeasurementData::TensorData tensorData; // create tensorData object
            std::vector<unsigned int> addEmptyScans;   // make sure the retention time interpolation does not add unexpected points to bins
            // @todo add check if set polarity is correct
            std::vector<std::vector<q::DataType::Peak>> centroids =
                tensorData.findCentroids_MZML(qpeaks, data, addEmptyScans, true, polarity, 10); // read mzML file and find centroids via qPeaks
            if (centroids.size() < 5)
            {

                if (!silent)
                {
                    std::cout << "skipping mode: " << polarity << "\n";
                }
                continue;
            }
            if (!silent)
            {
                std::cout << "Processing " << polarity << " peaks\n";
            }
            // adjust filename to include polarity here
            filename += ("_" + polarity);
            if (printCentroids)
            {
                std::vector<q::Algorithms::qBinning::EIC> dummy;
                q::printPeaklist(centroids, pathOutput, filename, dummy, printExtended, silent, true, skipError);
            }

            // @todo remove diagnostics later
            double meanDQSC = 0;
            for (auto spectrum : centroids)
            {
                for (auto point : spectrum)
                {
                    meanDQSC += point.dqsCen;
                }
            }

            q::Algorithms::qBinning::CentroidedData binThis = qpeaks.passToBinning(centroids, addEmptyScans);

            if (printSubProfile)
            {
                // create a subset of the profile data which only contains binned points
                // organisation: bin ID -> vector<vector<profile point>>
                // after peak finding, use the peak borders to annotate which profile
                // points were part of which peak.
                // @todo consider adding the option to also print surrounding profile points
            }

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

            // @todo remove diagnostics
            int count = 0;
            double meanDQSB = 0;
            for (auto EIC : binnedData)
            {
                for (double dqsb : EIC.DQSB)
                {
                    ++count;
                    meanDQSB += dqsb;
                }
            }
            meanDQSB /= count;

            timeStart = std::chrono::high_resolution_clock::now();
            // every subvector of peaks corresponds to the bin ID
            auto peaks = tensorData.findPeaks_QBIN(qpeaks, binnedData);
            // make sure that every peak contains only one mass trace
            assert(peaks.size() == binnedData.size());
            size_t peaksWithMassGaps = 0;
            size_t testsPassedTotal = 0;
            size_t testsPassedPeak = 0;
            size_t testsFailedPeak = 0;
            double meanDQSF = 0;
            int impossibleRegression = 0;
            for (size_t i = 0; i < peaks.size(); i++)
            {
                if (binnedData[i].errorcode == std::byte{0b01000000})
                {
                    ++testsPassedTotal;
                }
                if (!peaks[i].empty())
                {
                    if (binnedData[i].errorcode == std::byte{0b01000000})
                    {
                        ++testsPassedPeak;
                    }
                    else
                    {
                        ++testsFailedPeak;
                    }

                    auto massesBin = binnedData[i].mz;
                    // auto rtsBin = binnedData[i].rententionTimes;
                    auto scansBin = binnedData[i].scanNumbers;
                    for (size_t j = 0; j < peaks[i].size(); j++)
                    {
                        // idxPeakStart/End are the index referring to the bin in which a peak was found
                        if (!q::massTraceStable(massesBin, peaks[i][j].idxPeakStart, peaks[i][j].idxPeakEnd))
                        {
                            ++peaksWithMassGaps;
                            // std::cerr << peaks[i][j].mz << ", " << peaks[i][j].retentionTime << ", "
                            //           << peaks[i][j].area << ", " << peaks[i][j].dqsPeak << "\n";
                            // problems do not only occur at very low masses or the edges of out chromoatography
                            // recommendation: do not pass these peaks to result table
                            peaks[i][j].dqsPeak *= -1;

                            // @todo consider removing these or add a correction set somewhere later
                            // @todo add some documentation regarding the scores
                        }
                        else
                        {
                            meanDQSF += peaks[i][j].dqsPeak;
                        }
                    }
                }
            }
            std::cout << testsPassedTotal << ", " << testsPassedPeak << ", " << testsFailedPeak << ", " << peaks.size() - testsPassedTotal << "\n";

            if (verboseProgress)
            {
                std::cout << peaksWithMassGaps << " peaks were erroneously constructed from more than one mass trace\n";
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            int peakCount = 0;
            int dudBins = 0;
            int onlyone = 0;
            int overfullBins = 0;
            for (size_t i = 0; i < peaks.size(); i++)
            {
                peakCount += peaks[i].size();
                if (!peaks[i].empty())
                {
                    if (peaks[i].size() == 1)
                    {
                        ++onlyone;
                    }
                    else
                    {
                        overfullBins++;
                    }
                }
                else
                {
                    dudBins++;
                    // double binMeanMZ = 0;
                    // double binMeanRT = 0;
                    // int binsize = binnedData[i].mz.size();
                    // for (int j = 0; j < binsize; j++)
                    // {
                    //     binMeanMZ += binnedData[i].mz[j];
                    //     binMeanRT += binnedData[i].rententionTimes[j];
                    // }
                    // std::cout << binMeanMZ / binsize << ", " << binMeanRT / binsize << "\n";
                }
            }
            meanDQSF /= peakCount - peaksWithMassGaps - impossibleRegression;
            if (!silent)
            {
                if (impossibleRegression != 0)
                {
                    std::cerr << impossibleRegression << " bad regressions\n";
                }

                std::cout << "    found " << peakCount << " peaks in " << (timeEnd - timeStart).count() << " ns\n";
            }
            // @todo remove diagnostics
            // std::cerr << meanDQSF << ", " << binThis.lengthAllPoints << ", " << binnedData.size() << ", "
            //           << peakCount << ", " << meanDQSC << ", " << meanDQSB << ", " << polarity << ", " << filename << "\n";
            // continue;
            // std::cerr << peakCount << ", " << truebins << ", " << onlyone << ", " << filename << "\n";

            //@todo , numBins_empty, numBins_one, numBins_more, meanDQSB
            logWriter.open(pathLogging, std::ios::app);
            logWriter << filename << ", " << centroids.size() << ", " << binThis.lengthAllPoints << ", "
                      << meanDQSC / binThis.lengthAllPoints << ", " << dudBins << ", " << onlyone << ", "
                      << overfullBins << ", " << meanDQSB << ", " << peakCount << ", " << peaksWithMassGaps << ", " << impossibleRegression
                      << ", " << meanDQSF << /*testFailedPeak << ", " << testsPassedPeak << ", " << testsPassedTotal <<*/ "\n";
            logWriter.close();

            if (printPeaks)
            {
                q::printPeaklist(peaks, pathOutput, filename, binnedData, printExtended, silent, false, skipError);
            }
            // if (printSubProfile)
            // {
            //     for (size_t i = 0; i < peaks.size(); i++)
            //     {
            //         if (!peaks[i].empty())
            //         {
            //             auto massesBin = binnedData[i].mz;
            //             for (size_t j = 0; j < peaks[i].size(); j++)
            //             {
            //             }
            //         }
            //     }
            // }

            // @todo add peak grouping here
        }
        counter++;
    }
    if (!silent)
    {
        std::cout << "Completed data processing on " << tasklist.size() << " files.\n\n";
    }

    return 0;
}