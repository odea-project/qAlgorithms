#include <algorithm> // remove duplicates from task list
#include <assert.h>
#include <filesystem> // printing absolute path in case read fails
#include <fstream>    // write peaks to file @todo remove
#include <iostream>
#include <string>
#include <vector>

#include "qalgorithms_datatypes.h"
#include "qalgorithms_input_output.h"

namespace qAlgorithms
{
#pragma region helpstring

    const char helpinfo[] = " help information:\n\n" // @todo provide an up-to-date version of this and use a normal const char
                            "    qAlgorithms is a software project for non-target screening using mass spectrometry.\n"
                            "    For more information, visit our github page: https://github.com/odea-project/qAlgorithms.\n"
                            "    As of now (2026-06-08), only mzML files are supported. This program accepts the following command-line arguments:\n\n"
                            "      -h, -help:  Open this help menu\n"
                            "\n    Input settings:\n"
                            "      -i,  -input <PATH> [PATH] : Input files or directories in which to recursively search for .mzML\n"
                            "                                  files. You can enter any number of targets, as long as no file starts\n"
                            "                                  with a \"-\" or contains two dots in a row. It is possible to use\n"
                            "                                  the -i flag multiple times within one execution.\n"
                            "                                  Note that:\n"
                            "                                  1) qAlgorithms is case-sensitive when searching for files recursively.\n"
                            "                                     Make sure all your files have the correct extension (.mzML) and are\n"
                            "                                     not all lowercase (.mzml).\n"
                            "                                  2) duplicate input files are removed by default, even when they have\n"
                            "                                     a different name.\n"
                            "                                  3) filenames may never start with a \"-\".\n"
                            //   "      -tl, -tasklist <PATH>:      pass a list of file paths to the function. A tasklist can also contain directories\n"
                            //   "                                  to search recursively and output directories for different blocks of the input files.\n"
                            //   "                                  You can comment out lines by starting them with a \"#\".\n" // @todo update
                            "\n    Single-file output settings:\n"
                            "      Note                      : The filename is always the original filename extended by the polarity\n"
                            "                                  and the processing step. For example, printing features of the file\n"
                            "                                  \"test.mzML\" would produce the output file \"test_pos_features.csv\".\n"
                            "      -o,  -output <DIRECTORY>  : Directory into which all output files should be printed.\n"
                            "      -pc, -printcentroids      : Print all centroids produced after the first run of qcentroids.\n"
                            "      -pb, -printbins:          : If this flag is set, both bin summary information and\n"
                            "                                  all binned centroids will be printed to the output location\n"
                            "                                  in addition to the final peak table. The file ends in _bins.csv.\n"
                            "      -pf, -printfeatures         Print the feature list as csv.\n"
                            //   "      -sp, -subprofile:           (not implemented yet) instead of the peaks, print all proflie-mode data points which\n" // @todo
                            //   "                                  were used to create the final peaks. This does not return any quality\n"
                            //   "                                  scores. Only use this option when reading in prodile mode files.\n"
                            "      -px, -printfeatcen        : Print all centroids that are a part of the final feature list, including debug data.\n"
                            "      -ppf, -printcomponentsF   : Print the component regressions that belong to the generated feature list.\n"
                            "                                  The values for beta 0 are printed in order of the retention time of the\n"
                            "                                  associated features (check the feature list for this) and separated by a semicolon.\n"
                            "                                  The feature list is always printend if this is set.\n"
                            "      -ppb, -printcomponentsB   : Print all centroids that are part of components. Similar to -ppf, but considers the range of the\n"
                            "                                  newly constructed regression for which centroids are relevant. Also prints the components themselves.\n"
                            "      -pa, -printall            : Print all availvable resutlts. You will probably not need to do this.\n"
                            "\n    Program behaviour:\n"
                            "      -s, -silent               : do not print progress reports to standard out.\n" // @todo add an option for printing all process stats without timing and explanations for use with CLI toolchains
                            //   "      -v, -verbose:   print a detailed progress report to standard out.\n"
                            "      -skip-existing            : Do not write to files that already exist, even if an output option is set.\n"
                            "      -skip-error               : If processing fails, the program will not exit and instead start processing\n"
                            "                                  the next file in the tasklist.\n"
                            "      -skipAhead <n>            : Skip the first n entries in the tasklist when starting processing \n"
                            "      -fullLoop                 : Override default behaviour and always perform all possible processing steps,\n"
                            "                                  even if the results are not printed.\n"
                            "      -log                      : This option will create a detailed log file in the program directory.\n"
                            "                                  It will provide an overview for every processed file which can help you find and\n"
                            "                                  reason about anomalous behaviour in the results.\n";
    //   "                      A name can be supplied with a string following the argument. If this is not\n"
    //   "                      done by the user, the default log will be written or overwritten.\n"
    //   "    Analysis options:\n"
    //   "      -MS2:               also process MS2 spectra (not implemented yet)\n" // @todo

#pragma endregion helpstring

#pragma region "command line arguments"

    // NOLINTBEGIN(concurrency-mt-unsafe)
    UserInputSettings passCliArgs(int argc, char *argv[])
    {
        UserInputSettings args;
        assert(args.inputPaths.empty());
        assert(args.outputPath.empty());

        volatile bool debug = false;
        if (argc == 1 && !debug)
        {
            args = interactiveMode();
        }

        for (int i = 1; i < argc; i++)
        {
            std::string argument = argv[i];
            if ((argument == "-h") || (argument == "-help"))
            {
                printf("\n    %s%s", argv[0], helpinfo);
                exit(0);
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
                    fprintf(stderr, "Error: argument -input was set, but no valid file supplied.\n");
                    return args;
                }
                std::string inputString = argv[i];
                if (inputString[0] == '-')
                {
                    fprintf(stderr, "Error: argument -input was set, but no valid file supplied.\n");
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
                fprintf(stderr, "Error: tasklist support is not integrated at the moment.\n");

                args.tasklistSpecified = true;
                ++i;
                if (i == argc)
                {
                    fprintf(stderr, "Error: argument -tasklist was set, but no valid file supplied.\n");
                    return args;
                }
                std::string inputString = argv[i];
                if (inputString[0] == '-')
                {
                    fprintf(stderr, "Error: argument -tasklist was set, but no valid file supplied.\n");
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
                    fprintf(stderr, "Error: two output locations specified. For complex output location "
                                    "structures, it is recommended you use the tasklist input (not implemented yet).\n");
                    args.outputPath = "";
                    return args;
                }

                ++i;
                if (i == argc)
                {
                    fprintf(stderr, "Error: no output directory specified.\n");
                    return args;
                }
                args.outputPath = argv[i];
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
            else if ((argument == "-ppf") || (argument == "-printcomponentsF"))
            {
                args.printComponentRegs = true;
                args.printFeatures = true;
            }
            else if ((argument == "-ppb") || (argument == "-printcomponentsB"))
            {
                args.printComponentRegs = true;
                args.printComponentBins = true;
            }
            else if ((argument == "-px") || (argument == "-printfeatcen"))
            {
                args.printFeatCens = true;
            }
            else if ((argument == "-pa") || (argument == "-printall"))
            {
                args.printCentroids = true;
                args.printBins = true;
                args.printFeatures = true;
                args.printSubProfile = true;
                args.printFeatCens = true;
                args.printComponentRegs = true;
                args.printComponentBins = true;
            }
            else if (argument == "-log")
            {
                args.doLogging = true;
            }
            else if (argument == "-skip-error")
            {
                fprintf(stderr, "Warning: processing will ignore defective files.\n");
                args.skipError = true;
            }
            else if (argument == "-skipAhead" || argument == "-skip-ahead")
            {
                ++i;
                if (i == argc)
                {
                    fprintf(stderr, "Error: no value to skip ahead specified.\n");
                    return args;
                }

                char *str = argv[i];
                char **endptr = &str - 1; // the position one ahead of str is always defined and non-null
                int skip = strtol(str, endptr, 10);
                if (*endptr == str)
                {
                    fprintf(stderr, "Error: you cannot skip ahead by \"%s\" entries.\n", argv[i]);
                    return args;
                }
                else if (skip < 0)
                {
                    fprintf(stderr, "Error: you cannot skip ahead a negative number of entries.\n");
                }

                args.skipAhead = (size_t)skip;
            }
            else
            {
                fprintf(stderr, "Error: unknown argument \"%s\".\n", argument.c_str());
                exit(1);
            }
        } // end of reading in command line arguments
        // assert(!args.inputPaths.empty());
        // assert(!args.inputPaths[0].empty());
        return args;
    }
    // NOLINTEND(concurrency-mt-unsafe)

    UserInputSettings interactiveMode() // @todo this should be replaced by a better debug interface
    {
        // this function is called if qAlgorithms is executed without arguments
        printf("    ### qAlgorithms interactive terminal interface ###\n"
               "relative paths are not supported in this mode\n"
               "drag the folder or file you want to process into this window and press \"enter\" to continue:\n");
        std::string inputPath;
        while (true)
        {
            std::cin >> inputPath; // @todo replace this
            if (std::filesystem::exists(inputPath))
            {
                break;
            }
            printf("Error: The path does not exist.\n");
        }

        UserInputSettings res = UserInputSettings{
            std::vector<std::string>{inputPath},
            ".",
            0,
            never,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            0,
            false,
            true};

        return res;
    }

    bool inputsAreSensible(UserInputSettings &args)
    {
        // program exits if an error is found, but only after displaying all warnings
        bool goodInputs = true;

        if (args.inputPaths.empty())
        {
            fprintf(stderr, "Error: no input file supplied. Specify a file or directorey using the -i or "
                            "-tl flag. Execute qAlgorithms with the -h flag for more information.\n");
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
                    fprintf(stderr, "Error: output flags \"%s\" were set, but no output path supplied.\n", badOptions.c_str());
                }
                else
                {
                    fprintf(stderr, "Error: output flag \"%s\" was set, but no output path supplied.\n", badOptions.c_str());
                }
                goodInputs = false;
            }
            else
            {
                fprintf(stderr, "Warning: no output directory specified. No output files will be written.\n");
            }
        }
        else if (!std::filesystem::exists(args.outputPath))
        {
            fprintf(stderr, "Error: the specified output path \"%s\" does not exist.\n", args.outputPath.c_str());
            goodInputs = false;
        }
        else if (std::filesystem::status(args.outputPath).type() != std::filesystem::file_type::directory)
        {
            fprintf(stderr, "Error: the output location must be a directory.\n");
            goodInputs = false;
        }
        if (args.silent && args.verboseProgress)
        {
            fprintf(stderr, "Warning: -verbose overrides -silent.\n");
            args.silent = false;
        }

        // set termination point here so that the program will only process data when necessary
        if (args.printComponentRegs || args.printComponentBins)
        {
            args.term = TerminateAfter::components;
        }
        else if (args.printFeatures || args.printFeatCens || args.printSubProfile)
        {
            args.term = TerminateAfter::features;
        }
        else if (args.printBins)
        {
            args.term = TerminateAfter::binning;
        }
        else if (args.printCentroids)
        {
            args.term = TerminateAfter::centroids;
        }
        else
        {
            args.term = TerminateAfter::never;
        }

        if (args.term == TerminateAfter::never &&
            !(args.outputPath.empty()))
        {
            fprintf(stderr, "Warning: no output files will be written.\n");
        }

        return goodInputs;
    }

#pragma endregion "command line arguments"

#pragma region "file reading"

    // NOLINTBEGIN(concurrency-mt-unsafe)
    std::vector<std::filesystem::path> controlInput(const std::vector<std::string> *inputTasks)
    {
        const std::string filetype = ".mzML"; // @todo change this if other filetpes should be supported
        namespace fs = std::filesystem;
        std::vector<fs::path> outputTasks;
        outputTasks.reserve(inputTasks->size());
        struct TaskEntry
        {
            fs::path path;
            uintmax_t filesize;
        };
        std::vector<TaskEntry> tasklist;
        tasklist.reserve(inputTasks->size());
        // find all valid inputs and add them to the task list
        for (std::string inputPath : *inputTasks)
        {
            fs::path currentPath{inputPath};
            if (!fs::exists(currentPath))
            {
                fprintf(stderr, "Warning: the file \"%s\" does not exist.\n", inputPath.c_str());
                continue;
            }
            currentPath = fs::canonical(currentPath);
            auto status = fs::status(currentPath);

            if (status.type() == fs::file_type::regular)
            {
                if (currentPath.extension() != filetype)
                {
                    fprintf(stderr, "Warning: only %s files are supported. The file \"%s\" has been skipped.\n",
                            filetype.c_str(), inputPath.c_str());
                    if (currentPath.extension() == ".mzml")
                    {
                        fprintf(stderr, "Warning: qAlgorithms file reading is case-sensitive. Please change the file extension to \".mzML\"");
                    }
                    continue;
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
                fprintf(stderr, "Warning: \"%s\" is not a supported file or directory. The file has been skipped.\n", inputPath.c_str());
            }
        }
        // remove duplicate files
        size_t tasknumber = tasklist.size();
        unsigned int removedEntries = 0;
        if (tasknumber == 0)
        {
            fprintf(stderr, "Error: no valid files selected.\n");
            exit(1);
        }
        std::sort(tasklist.begin(), tasklist.end(),
                  [](const TaskEntry &lhs, const TaskEntry &rhs) { return lhs.filesize < rhs.filesize; });
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
            fprintf(stderr, "Warning: removed %u duplicate input files from processing queue.\n", removedEntries);
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
    // NOLINTEND(concurrency-mt-unsafe)

#pragma endregion "file reading"

#pragma region "print functions"
    // @todo use macros to move the boilderplate out of the function body

    static void errorMsg(std::filesystem::path *pathOutput, bool noOverwrite)
    {
        if (std::filesystem::exists(*pathOutput))
        {
            if (noOverwrite)
            {
#ifdef _WIN32
                const char format[] = "Warning: \"%ls\" already exists and will not be overwritten\n";
#else
                const char format[] = "Warning: \"%s\" already exists and will not be overwritten\n";
#endif
                fprintf(stderr, format, pathOutput->c_str());
                return;
            }
            std::filesystem::remove(*pathOutput);
        }
    }

    // void printSpectrumAndCens(const std::vector<CentroidPeak> *peaktable,
    //                           std::filesystem::path pathOutput,
    //                           std::string filename,
    //                           size_t spectrumNo)
    // {
    //     assert(false);
    // }

    void printCentroids(const std::vector<CentroidPeak> *peaktable,
                        std::vector<float> *convertRT,
                        std::filesystem::path pathOutput,
                        std::string filename,
                        bool silent, bool noOverwrite)
    {
        filename += "_centroids.csv";
        pathOutput /= filename;
        errorMsg(&pathOutput, noOverwrite);

        if (!silent)
        {
#ifdef _WIN32
            const char format[] = "writing centroids to: %ls\n";
#else
            const char format[] = "writing centroids to: %s\n";
#endif
            printf(format, pathOutput.c_str());
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
#ifdef _WIN32
            const char format[] = "Error: could not open output path during centroid printing. No files have been written.\nFilename: %ls\n";
#else
            const char format[] = "Error: could not open output path during centroid printing. No files have been written.\nFilename: %s\n";
#endif
            fprintf(stderr, format, pathOutput.c_str());
            return;
        }
        output << "cenID,mz,mzUncertainty,number_MS1,retentionTime,area,areaUncertainty,"
               << "height,heightUncertainty,scale,DQSC,competitors\n";
        for (size_t j = 0; j < peaktable->size(); ++j)
        {
            const CentroidPeak peak = peaktable->at(j);
            char buffer[256];
            snprintf(buffer, 256, "%d,%0.6f,%0.6f,%d,%0.4f,%0.4f,%0.4f,%0.4f,%0.4f,%d,%0.5f,%d\n",
                     peak.ID, peak.mz, peak.mzUncertainty, peak.number_MS1, convertRT->at(peak.number_MS1),
                     peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, peak.scale, peak.DQSC,
                     //  peak.interpolations,
                     peak.numCompetitors);
            output << buffer;
        }

        file_out << output.str();
        file_out.close();
        return;
    }

    void printBins(const std::vector<CentroidPeak> *centroids,
                   const std::vector<EIC> *bins,
                   //    const std::vector<float> *convertRT,
                   std::filesystem::path pathOutput,
                   std::string filename,
                   bool silent, bool noOverwrite)
    {
        filename += "_bins.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
#ifdef _WIN32
                const char format[] = "Warning: %ls already exists and will not be overwritten\n";
#else
                const char format[] = "Warning: %s already exists and will not be overwritten\n";
#endif
                fprintf(stderr, format, pathOutput.c_str());

                return;
            }
            std::filesystem::remove(pathOutput);
        }

        if (!silent)
        {
#ifdef _WIN32
            const char format[] = "writing bins to: %ls\n";
#else
            const char format[] = "writing bins to: %s\n";
#endif
            printf(format, pathOutput.c_str());
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
#ifdef _WIN32
            const char format[] = "Error: could not open output path during bin printing. No files have been written.\nFilename: %ls\n";
#else
            const char format[] = "Error: could not open output path during bin printing. No files have been written.\nFilename: %s\n";
#endif
            fprintf(stderr, format, pathOutput.c_str());
            return;
        }
        // @todo consider if the mz error is relevant when checking individual bins
        output << "binID,cenID,mz,mzUncertainty,retentionTime,number_MS1,area,height,degreesOfFreedom,DQSC\n";
        for (unsigned long binID = 0; binID < bins->size(); binID++)
        {
            const EIC bin = bins->at(binID);
            if (bin.scanNumbers.empty())
                continue;

            for (size_t i = 0; i < bin.mz.size(); i++)
            {
                const CentroidPeak cen = centroids->at(bin.cenID[i]);
                char buffer[128];
                snprintf(buffer, 128, "%lu,%u,%0.8f,%0.8f,%0.4f,%d,%0.6f,%0.6f,%u,%0.4f\n", // @todo re-add the dqsb once that works
                         binID, cen.ID, bin.mz[i], bin.predInterval[i],
                         bin.RT[i], bin.scanNumbers[i], bin.ints_area[i],
                         bin.ints_height[i], bin.df[i], bin.DQSC[i]);
                output << buffer;
            }
        }
        file_out << output.str();
        file_out.close();
        return;
    }

    void printFeatureList(const std::vector<FeaturePeak> *peaktable,
                          std::filesystem::path pathOutput,
                          std::string filename,
                          const std::vector<EIC> *originalBins,
                          const std::vector<float> *convertRT,
                          bool silent, bool noOverwrite)
    {
        filename += "_features.csv";
        pathOutput /= filename;

        if (std::filesystem::exists(pathOutput))
        {
            if (noOverwrite)
            {
#ifdef _WIN32
                const char format[] = "writing bins to: %ls\n";
#else
                const char format[] = "writing bins to: %s\n";
#endif
                printf(format, pathOutput.c_str());
                return;
            }
            std::filesystem::remove(pathOutput);
        }
        if (!silent)
        {
#ifdef _WIN32
            const char format[] = "writing features to: %ls\n";
#else
            const char format[] = "writing features to: %s\n";
#endif
            printf(format, pathOutput.c_str());
        }

        std::ofstream file_out;
        std::stringstream output;
        file_out.open(pathOutput, std::ios::out);
        if (!file_out.is_open())
        {
#ifdef _WIN32
            const char format[] = "Error: could not open output path during feature printing. No files have been written.\nFilename: %ls\n";
#else
            const char format[] = "Error: could not open output path during feature printing. No files have been written.\nFilename: %s\n";
#endif
            fprintf(stderr, format, pathOutput.c_str());
            return;
        }

        constexpr std::string_view header = "ID,binID,mz,mzUncertainty,retentionTime,RT_Uncertainty,"
                                            "lowestRetentionTime,highestRetentionTime,area,areaUncertainty,height,heightUncertainty,"
                                            "scale,DQSC,DQSB,DQSF,b0,b1,b2,b3\n";
        output << header;

        unsigned int ID = 1;
        for (size_t i = 0; i < peaktable->size(); i++)
        {
            const FeaturePeak peak = peaktable->at(i);
            unsigned long binID = peak.idxBin;
            assert(binID < originalBins->size());
            float RT_start = 0;
            float RT_end = 0;

            char buffer[256];
            snprintf(buffer, 256, "%d,%lu,%0.6f,%0.6f,%0.4f,%0.4f,%0.4f,%0.4f,%0.3f,%0.3f,%0.3f,%0.3f,%lu,%0.5f,%0.5f,%0.5f,%0.8f,%0.8f,%0.8f,%0.8f\n",
                     ID, binID, peak.mz, peak.mzUncertainty,
                     peak.retentionTime, peak.RT_Uncertainty, RT_start, RT_end,
                     peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, (unsigned long)peak.coefficients.scale,
                     peak.DQSC, peak.DQSB, peak.DQSF,
                     // properties relevant for componentisation, remove this later
                     peak.coefficients.b0, peak.coefficients.b1, peak.coefficients.b2, peak.coefficients.b3);
            output << buffer;
            ++ID;
        }

        file_out << output.str();
        file_out.close();
        return;
    }

#pragma endregion "print functions"
} // namespace qAlgorithms