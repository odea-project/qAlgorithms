#include <vector>
#include <iostream>
#include <fstream> // write peaks to file
#include <cstdlib>
// #include <iomanip>
#include <filesystem> // printing absolute path in case read fails
#include <string>
// #include <sstream>   // write peaks to file
#include <algorithm> // remove duplicates from task list
#include <assert.h>
#include <cmath> // isnan()

#include "qalgorithms_datatypes.h"
#include "qalgorithms_input_output.h"
#include "qalgorithms_global_vars.h"

namespace qAlgorithms
{
#pragma region helpstring

    static std::string helpinfo = " help information:\n\n" // @todo std::format
                                  "    qAlgorithms is a software project for non-target screening using mass spectrometry.\n"
                                  "    For more information, visit our github page: https://github.com/odea-project/qAlgorithms.\n"
                                  "    As of now (2024-10-31), only mzML files are supported. This program accepts the following command-line arguments:\n\n"
                                  "      -h, -help:  open this help menu\n\n"
                                  "    Note that filenames may never start with a \"-\".\n"
                                  "    Input settings:\n"
                                  "      Note that duplicate input files are removed by default, even when they have a different name.\n"
                                  "      -i,  -input <PATH> [PATH]   input files or directories in which to recursively search for .mzML files.\n"
                                  "                                  you can enter any number of targets, as long as no file starts with a \"-\"\n"
                                  "                                  or contains two dots in a row. It is possible to use the -i flag multiple\n"
                                  "                                  times within one execution.\n"
                                  "                                  Note that qAlgorithms is case-sensitive when searching for files recursively. Make\n"
                                  "                                  sure all your files have the correct extension (.mzML) and are not all lowercase (.mzml).\n"
                                  "      -tl, -tasklist <PATH>:      pass a list of file paths to the function. A tasklist can also contain directories\n"
                                  "                                  to search recursively and output directories for different blocks of the input files.\n"
                                  "                                  You can comment out lines by starting them with a \"#\".\n" // @todo update
                                  "    Single-file output settings:\n"
                                  "      The filename is always the original filename extended by the polarity and the processing step.\n"
                                  "      -o,  -output <DIRECTORY>:   directory into which all output files should be printed.\n"
                                  "      -pc, -printcentroids:       print all centroids produced after the first run of qcentroids.\n"
                                  "      -pb, -printbins:            If this flag is set, both bin summary information and\n"
                                  "                                  all binned centroids will be printed to the output location\n"
                                  "                                  in addition to the final peak table. The file ends in _bins.csv.\n"
                                  "      -pf, -printfeatures         print the feature list as csv.\n"
                                  "      -sp, -subprofile:           (not implemented yet) instead of the peaks, print all proflie-mode data points which\n" // @todo
                                  "                                  were used to create the final peaks. This does not return any quality\n"
                                  "                                  scores. Only use this option when reading in prodile mode files.\n"
                                  "      -pa, -printall:             print all availvable resutlts. You will probably not need to do this.\n"
                                  "      -px, -printfeatcen:         print all centroids that are a part of the final feature list, including debug data.\n"
                                  "    Program behaviour:\n"
                                  "      -s, -silent:    do not print progress reports to standard out.\n"
                                  "      -v, -verbose:   print a detailed progress report to standard out.\n"
                                  "      -skip-existing  do not write to files that already exist, even if an output option is set.\n"
                                  "      -skip-error:    if processing fails, the program will not exit and instead start processing\n"
                                  "                      the next file in the tasklist.\n"
                                  "      -log:           This option will create a detailed log file in the program directory.\n"
                                  "                      A name can be supplied with a string following the argument. If this is not\n"
                                  "                      done by the user, the default log will be written or overwritten.\n"
                                  "    Analysis options:\n"
                                  "      -MS2:               also process MS2 spectra (not implemented yet)\n" // @todo
                                  "      -ppm <number>:      this sets the centroid error when reading in pre-centroided data\n"
                                  "                          with qAlgorithms to <number> * 10^-6 * m/z of the centroid. We recommend\n"
                                  "                          you always use the centroiding algorithm implemented in qAlgorithms.\n"
                                  "                          This parameter is significantly different from an EIC standard deviation estimator (XCMS)!\n"
                                  "                          By default, this value is set to 0.25.\n"
                                  "      -mz-abs <number>:   (not implemented yet) add this absolute error (in Dalton) to the relative error specified by -ppm.\n"; //@todo

#pragma endregion helpstring

#pragma region "command line arguments"

    UserInputSettings passCliArgs(int argc, char *argv[])
    {
        volatile bool debug = false;
        // this function processes all cli arguments supplied by the user

        // return this struct
        UserInputSettings args;
        assert(args.inputPaths.empty());
        assert(args.outputPath.empty());

        if (argc == 1 && !debug)
        {
            // run in interactive mode
            args = interactiveMode(argv);
        }
        else if (debug)
        {
            argc = 0;
            // args.inputPaths.push_back("C:/Users/unisys/Documents/Studium/Messdaten/Wasser2_neg.mzML");
            args.inputPaths.push_back("/home/terry/Work/Messdaten/LC_orbitrap_kali/");
        }
        for (int i = 1; i < argc; i++)
        {
            std::string argument = argv[i];
            if ((argument == "-h") || (argument == "-help"))
            {
                std::cout << "\n    " << argv[0] << helpinfo;
                exit(0);
                // return args;
            }
            else if ((argument == "-s") || (argument == "-silent"))
            {
                args.silent = true;
            }
            else if ((argument == "-v") || (argument == "-verbose"))
            {
                args.verboseProgress = true;
            }
            else if ((argument == "-i") || (argument == "-input"))
            {
                ++i;
                if (i == argc)
                {
                    std::cerr << "Error: argument -input was set, but no valid file supplied.\n";
                    return args;
                }
                std::string inputString = argv[i];
                if (inputString[0] == '-')
                {
                    std::cerr << "Error: argument -input was set, but no valid file supplied.\n";
                    return args;
                }
                for (; i < argc; i++)
                {
                    inputString = argv[i];
                    if (inputString[0] == '-')
                    {
                        break;
                    }
                    args.inputPaths.push_back(inputString);
                }
                --i;
            }
            else if ((argument == "-tl") || (argument == "-tasklist")) // @todo test this
            {
                std::cerr << "Error: tasklist support is not integrated at the moment.\n";

                args.tasklistSpecified = true;
                ++i;
                if (i == argc)
                {
                    std::cerr << "Error: argument -tasklist was set, but no valid file supplied.\n";
                    return args;
                }
                std::string inputString = argv[i];
                if (inputString[0] == '-')
                {
                    std::cerr << "Error: argument -tasklist was set, but no valid file supplied.\n";
                    return args;
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
                args.inputPaths.push_back(inputString);
            }

            else if ((argument == "-o") || (argument == "-output"))
            {
                // @todo
                if (args.outputPath != "")
                {
                    std::cerr << "Error: two output locations specified. For complex output location "
                              << "structures, it is recommended you use the tasklist input (not implemented yet).\n";
                    args.outputPath = "";
                    return args;
                }

                ++i;
                if (i == argc)
                {
                    std::cerr << "Error: no output directory specified.\n";
                    return args;
                }
                args.outputPath = argv[i];
            }
            else if (argument == "-ppm") // @todo remove
            {
                ++i;
                if (i == argc)
                {
                    std::cerr << "Error: -ppm set, but no centroid error specified.\n";
                    return args;
                }

                try
                {
                    args.newPPM = std::stod(argv[i]);
                }
                catch (std::invalid_argument const &)
                {
                    std::cerr << "Error: the centroid error cannot be set to \"" << argv[i] << "\" ppm.\n";
                    return args;
                }
                // @todo move to control function or remove entirely
                if (args.newPPM <= 0)
                {
                    std::cerr << "Error: the assumed centroid error must be greater than 0.\n";
                    return args;
                }
                if (std::isnan(args.newPPM))
                {
                    std::cerr << "Error: the assumed centroid error must be a number, but was set to NaN.\n";
                    return args;
                }
                if (args.newPPM > 1000000)
                {
                    std::cerr << "Error: the assumed centroid error is set to 100% or greater (\"" << argv[i] << "\").\n";
                    return args;
                }
                if (args.newPPM > 0.5 || args.newPPM < 0.05)
                {
                    std::cerr << "Warning: you have set the centroid error to \"" << argv[i] << "\", with the expected range \n"
                              << "being between 0.05 and 0.5 ppm. Are you sure this is what you want?\n"
                              << "The centroid error is not the mz tolerance from ROI-type approaches.\n";
                }
            }
            else if ((argument == "-pc") || (argument == "-printcentroids"))
            {
                args.printCentroids = true;
            }
            else if ((argument == "-pb") || (argument == "-printbins"))
            {
                args.printBins = true;
            }
            else if ((argument == "-pf") || (argument == "-printfeatures"))
            {
                args.printFeatures = true;
            }
            else if ((argument == "-sp") || (argument == "-subprofile"))
            {
                args.printSubProfile = true;
            }
            else if ((argument == "-pa") || (argument == "-printall"))
            {
                args.printCentroids = true;
                args.printBins = true;
                args.printFeatures = true;
                args.printSubProfile = true;
                args.printFeatCens = true;
            }
            else if ((argument == "-px") || (argument == "-printfeatcen"))
            {
                args.printFeatCens = true;
            }
            else if (argument == "-log")
            {
                args.doLogging = true;
                // @todo is this functionality necessary?
                // if (i + 1 != argc)
                // {
                //     std::string logName = argv[i + 1];
                //     if (logName[0] != '-')
                //     {
                //         logfileName = logName;
                //     }
                // }
            }
            else if (argument == "-skip-error")
            {
                std::cerr << "Warning: processing will ignore defective files.\n";
                args.skipError = true;
            }
            else
            {
                std::cerr << "Warning: unknown argument \"" << argument << "\".\n";
            }
        } // end of reading in command line arguments
        // assert(!args.outputPath.empty());
        assert(!args.inputPaths.empty());
        assert(!args.inputPaths[0].empty());
        return args;
    }

    UserInputSettings interactiveMode(char *argv[])
    {
        // this function is called if qAlgorithms is executed without arguments
        std::cout << "    ### qAlgorithms interactive terminal interface ###\n"
                  << "relative paths are not supported in this mode\n"
                  << "drag the folder or file you want to process into this window and press \"enter\" to continue:\n";
        std::string inputPath;
        std::cin >> inputPath;

        std::cout << "drag the folder you want the output files written to into this window and press \"enter\" to continue.\n"
                  << "enter \"#\" to write to the input path.\n";
        std::string outputPath;
        std::cin >> outputPath;

        if (outputPath[0] == '#')
        {
            if (std::filesystem::status(inputPath).type() != std::filesystem::file_type::directory)
            {
                outputPath = std::filesystem::path(inputPath).parent_path().string();
            }
            else
            {
                outputPath = inputPath;
            }
        }
        std::cout << outputPath;
        return UserInputSettings{
            // user input for input and output
            std::vector<std::string>{inputPath},
            outputPath,
            false,
            false,
            false,
            true, // only print standard feature list
            false,
            false,
            false,
            false,
            false,
            false,
            0,
            false,
            true};
    }

    bool inputsAreSensible(UserInputSettings &args)
    {
        // program exits if an error is found, but only after displaying all warnings
        bool goodInputs = true;

        if (args.inputPaths.empty())
        {
            std::cerr << "Error: no input file supplied. Specify a file or directorey using the -i or "
                      << "-tl flag. Execute qAlgorithms with the -h flag for more information.\n";
            goodInputs = false;
        }
        if (args.outputPath.empty())
        {

            if (args.printCentroids || args.printFeatures || args.printBins)
            {
                std::string badOptions = "";
                bool prevBad = false;
                if (args.printCentroids)
                {
                    prevBad = true;
                    badOptions += "-printcentroids";
                }
                if (args.printBins)
                {
                    if (prevBad)
                    {
                        badOptions += ", ";
                    }
                    prevBad = true;
                    badOptions += "-printbins";
                }
                if (args.printFeatures)
                {
                    if (prevBad)
                    {
                        badOptions += ", ";
                    }
                    badOptions += "-printfeatures";
                }
                bool pluralSet = false;
                if (badOptions.size() > 15) // more than one option was set
                {
                    pluralSet = true;
                }

                if (pluralSet)
                {
                    std::cerr << "Error: output flags \"" << badOptions << "\" were set, but no output path supplied.\n";
                }
                else
                {
                    std::cerr << "Error: output flag \"" << badOptions << "\" was set, but no output path supplied.\n";
                }
                goodInputs = false;
            }
            else
            {
                std::cerr << "Warning: no output directory specified. No output files will be written.\n";
            }
        }
        else if (!std::filesystem::exists(args.outputPath))
        {
            std::cerr << "Error: the specified output path \"" << args.outputPath << "\" does not exist.\n";
            goodInputs = false;
        }
        else if (std::filesystem::status(args.outputPath).type() != std::filesystem::file_type::directory)
        {
            std::cerr << "Error: the output location must be a directory.\n";
            goodInputs = false;
        }
        // if (inSpecified && tasklistSpecified)
        // {
        //     std::cerr << "Warning: Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        //     // @todo
        // }
        if (args.silent && args.verboseProgress)
        {
            std::cerr << "Warning: -verbose overrides -silent.\n";
            args.silent = false;
        }
        if (!((args.printCentroids || args.printBins) ||
              (args.printFeatures || args.printFeatCens)) &&
            !(args.outputPath.empty()))
        {
            std::cerr << "Warning: no output files will be written.\n";
        }

        if (args.newPPM < 0)
        {
            std::cerr << "Error: invalid value for ppm error supplied.\n";
            goodInputs = false;
        }
        if (args.newPPM > 0)
        {
            std::cerr << "Notice: the changed centroid certainty will only affect pre-centroided data.\n";
            PPM_PRECENTROIDED = args.newPPM;
        }
        if (!goodInputs && args.interactive)
        {
            std::cout << "   inputs are incorrect - enter \"exit\" to close this program:\n";
            std::string userInput;
            while (userInput != "exit") // this loop should only terminate upon user input
            {
                std::cin >> userInput;
            }
        }

        return goodInputs;
    }

#pragma endregion "command line arguments"

#pragma region "file reading"

    std::vector<std::filesystem::path> controlInput(const std::vector<std::string> inputTasks, const bool skipError)
    {
        const std::string filetype = ".mzML"; // @todo change this if other filetpes should be supported
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
                continue;
            }
            currentPath = fs::canonical(currentPath);
            auto status = fs::status(currentPath);

            if (status.type() == fs::file_type::regular)
            {
                if (currentPath.extension() != filetype)
                {
                    std::cerr << "Warning: only " << filetype << " files are supported. The file \""
                              << inputPath << "\" has been skipped.\n";
                    if (currentPath.extension() == ".mzml")
                    {
                        std::cerr << "Warning: qAlgorithms is case-sensitive. Please change the file extension to \".mzML\"";
                    }
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
                const size_t readLength = 25000;
                std::ifstream fileConflict;
                char firstChars[readLength];
                char secondChars[readLength];
                fileConflict.open(tasklist[i - 1].path);
                fileConflict.read(firstChars, readLength);
                fileConflict.close();

                fileConflict.open(tasklist[i].path);
                fileConflict.read(secondChars, readLength);
                fileConflict.close();

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
        // make polarity switching to two files
        return (outputTasks);
    }

#pragma endregion "file reading"

#pragma region "print functions"
    /// @todo make universal print function, fix out-of-bounds access, chi squared test for binning
    void printCentroids(const std::vector<std::vector<CentroidPeak>> peaktable,
                        std::vector<float> convertRT, std::filesystem::path pathOutput,
                        std::string filename, bool silent, bool skipError, bool noOverwrite)
    {
        filename += "_centroids.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
                std::cerr << "Warning: " << pathOutput << " already exists and will not be overwritten\n";
                return;
            }
            std::filesystem::remove(pathOutput);
        }
        if (!silent)
        {
            std::cout << "writing peaks to: " << pathOutput << "\n";
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during peaklist printing. No files have been written.\n"
                      << "Filename: " << pathOutput << "\n";
            return;
        }
        output << "cenID,mz,mzUncertainty,scanNumber,retentionTime,area,areaUncertainty,"
               << "height,heightUncertainty,degreesOfFreedom,DQSC\n";
        unsigned int counter = 0;
        for (size_t i = 0; i < peaktable.size(); i++)
        {
            if (!peaktable[i].empty())
            {
                for (size_t j = 0; j < peaktable[i].size(); ++j)
                {
                    auto peak = peaktable[i][j];
                    char buffer[256];
                    sprintf(buffer, "%d,%0.6f,%0.6f,%d,%0.4f,%0.4f,%0.4f,%0.4f,%0.4f,%u,%0.5f\n",
                            counter, peak.mz, peak.mzUncertainty, peak.scanNumber, convertRT[peak.scanNumber],
                            peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, peak.df, peak.dqsCen);
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
                            bool silent, bool skipError, bool noOverwrite)
    {
        // @todo should this be added to the program?
        std::cout << "subprofiles might not be a valid concept, exiting.\n";
        exit(1);
        filename += "_subprofiles.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
                std::cerr << "Warning: " << pathOutput << " already exists and will not be overwritten\n";
                return;
            }
            std::filesystem::remove(pathOutput);
        }

        if (!silent)
        {
            std::cout << "writing profile information of peaks to: " << pathOutput << "\n";
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during peaklist printing. No files have been written.\n"
                      << "Filename: " << pathOutput << "\n";
            return;
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

    void printBins(const std::vector<EIC> bins, std::filesystem::path pathOutput, std::string filename,
                   bool silent, bool skipError, bool noOverwrite)
    {
        filename += "_bins.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
                std::cerr << "Warning: " << pathOutput << " already exists and will not be overwritten\n";
                return;
            }
            std::filesystem::remove(pathOutput);
        }

        if (!silent)
        {
            std::cout << "writing bins to: " << pathOutput << "\n";
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during bin printing. No files have been written.\n"
                      << "Filename: " << pathOutput << "\n";
            return;
        }
        // @todo consider if the mz error is relevant when checking individual bins
        output << "binID,cenID,mz,mzError,retentionTime,scanNumber,area,height,degreesOfFreedom,DQSC,DQSB\n";
        for (size_t binID = 0; binID < bins.size(); binID++)
        {
            for (size_t i = 0; i < bins[binID].mz.size(); i++)
            {
                char buffer[128];
                sprintf(buffer, "%zu,%u,%0.8f,%0.8f,%0.4f,%d,%0.6f,%0.6f,%u,%0.4f,%0.4f\n",
                        binID, bins[binID].cenID[i], bins[binID].mz[i], bins[binID].predInterval[i],
                        bins[binID].rententionTimes[i], bins[binID].scanNumbers[i], bins[binID].ints_area[i],
                        bins[binID].ints_height[i], bins[binID].df[i], bins[binID].DQSC[i], bins[binID].DQSB[i]);
                output << buffer;
            }
        }
        file_out << output.str();
        file_out.close();
        return;
    }

    void printFeatureList(const std::vector<FeaturePeak> peaktable,
                          std::filesystem::path pathOutput, std::string filename,
                          const std::vector<EIC> originalBins,
                          bool verbose, bool silent, bool skipError, bool noOverwrite)
    {
        filename += "_features.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
                std::cerr << "Warning: " << pathOutput << " already exists and will not be overwritten\n";
                return;
            }
            std::filesystem::remove(pathOutput);
        }
        if (!silent)
        {
            std::cout << "writing peaks to: " << pathOutput << "\n";
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during peaklist printing. No files have been written.\n"
                      << "Filename: " << pathOutput << "\n";
            return;
        }

        output << "ID,binID,binIdxStart,binIdxEnd,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
               << "lowestRetentionTime,highestRetentionTime,area,areaUncertainty,height,heightUncertainty,"
               << "DQSC,DQSB,DQSF,apexLeft,b0,b1,b2,b3\n";

        unsigned int counter = 1;
        for (size_t i = 0; i < peaktable.size(); i++)
        {
            auto peak = peaktable[i];
            int binID = peak.idxBin;
            std::vector<float> RTs = originalBins[binID].rententionTimes;

            char buffer[256];
            sprintf(buffer, "%d,%d,%d,%d,%0.6f,%0.6f,%0.4f,%0.4f,%0.4f,%0.4f,%0.3f,%0.3f,%0.3f,%0.3f,%0.5f,%0.5f,%0.5f,%s,%0.8f,%0.8f,%0.8f,%0.8f\n",
                    counter, binID, peak.idxPeakStart, peak.idxPeakEnd, peak.mz, peak.mzUncertainty,
                    peak.retentionTime, peak.retentionTimeUncertainty, RTs[peak.idxPeakStart], RTs[peak.idxPeakEnd],
                    peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty,
                    peak.dqsCen, peak.dqsBin, peak.dqsPeak,
                    // properties relevant for componentisation, remove this later
                    peak.apexLeft ? "T" : "F", peak.coefficients.b0, peak.coefficients.b1, peak.coefficients.b2, peak.coefficients.b3);
            output << buffer;
            ++counter;
        }

        file_out << output.str();
        file_out.close();
        return;
    }

    void printFeatureCentroids(const std::vector<FeaturePeak> peaktable,
                               std::filesystem::path pathOutput, std::string filename,
                               const std::vector<EIC> originalBins,
                               bool verbose, bool silent, bool skipError, bool noOverwrite)
    {
        filename += "_featCen.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
                std::cerr << "Warning: " << pathOutput << " already exists and will not be overwritten\n";
                return;
            }
            std::filesystem::remove(pathOutput);
        }
        if (!silent)
        {
            std::cout << "writing peaks to: " << pathOutput << "\n";
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
            std::cerr << "Error: could not open output path during featCen printing. No files have been written.\n"
                      << "Filename: " << pathOutput << "\n";
            return;
        }

        output << "featureID,binID,cenID,mz,mzUncertainty,retentionTime,"
               << "area,height,DQSC,DQSB,DQSF,apexLeft,b0,b1,b2,b3\n";

        unsigned int counter = 1;
        for (size_t i = 0; i < peaktable.size(); i++)
        {
            auto peak = peaktable[i];
            int binID = peak.idxBin;
            auto bin = originalBins[binID];
            char buffer[256];
            for (size_t cen = peak.idxPeakStart; cen < peak.idxPeakEnd + 1; cen++)
            {
                sprintf(buffer, "%d,%d,%d,%0.6f,%0.6f,%0.4f,%0.3f,%0.3f,%0.5f,%0.5f,%0.5f,%s,%0.8f,%0.8f,%0.8f,%0.8f\n",
                        counter, binID, bin.cenID[cen], bin.mz[cen], bin.predInterval[cen], bin.rententionTimes[cen],
                        bin.ints_area[cen], bin.ints_height[cen], bin.DQSC[cen], bin.DQSB[cen], peak.dqsPeak,
                        peak.apexLeft ? "T" : "F", peak.coefficients.b0, peak.coefficients.b1, peak.coefficients.b2, peak.coefficients.b3);
                output << buffer;
            }
            ++counter;
        }

        file_out << output.str();
        file_out.close();
        return;
    }
#pragma endregion "print functions"
}