// internal
// #include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_measurement_data_tensor.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qPattern.h"
#include "qalgorithms_global_vars.h"

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

namespace qAlgorithms
{
    /// ### set global variables ###
    float PPM_PRECENTROIDED = -INFINITY;               // -Infinity sets it to the default value if no user input changes it
    float MZ_ABSOLUTE_PRECENTROIDED = -INFINITY;       // see above
    const double TOLERANCE_BINNING = -0.4771864667153; // precalculated tolerance for alpha = 0.05
    const int GLOBAL_MAXSCALE = 15;                    // @todo is this a sensible maximum?
    std::array<float, 384> INV_ARRAY = initialize();   // @todo make constexpr
    constexpr float T_VALUES[256] = {6.313751515, 2.91998558, 2.353363435, 2.131846781, 2.015048373, 1.94318028, 1.894578605, 1.859548038, 1.833112933, 1.812461123, 1.795884819, 1.782287556, 1.770933396, 1.761310136, 1.753050356, 1.745883676, 1.739606726, 1.734063607, 1.729132812, 1.724718243, 1.720742903, 1.717144374, 1.713871528, 1.71088208, 1.708140761, 1.70561792, 1.703288446, 1.701130934, 1.699127027, 1.697260894, 1.695518789, 1.693888742, 1.692360304, 1.690924251, 1.689572454, 1.688297711, 1.687093617, 1.685954458, 1.684875119, 1.683851011, 1.682878, 1.681952356, 1.681070702, 1.680229975, 1.679427392, 1.678660413, 1.677926721, 1.677224195, 1.676550892, 1.675905025, 1.67528495, 1.674689153, 1.674116236, 1.673564906, 1.673033965, 1.672522303, 1.672028888, 1.671552762, 1.671093032, 1.670648865, 1.670219484, 1.669804162, 1.669402222, 1.669013025, 1.668635976, 1.668270514, 1.667916114, 1.667572281, 1.667238549, 1.666914479, 1.666599658, 1.666293696, 1.665996224, 1.665706893, 1.665425373, 1.665151353, 1.664884537, 1.664624644, 1.664371409, 1.664124579, 1.663883913, 1.663649184, 1.663420175, 1.663196679, 1.6629785, 1.662765449, 1.662557349, 1.662354029, 1.662155326, 1.661961084, 1.661771155, 1.661585397, 1.661403674, 1.661225855, 1.661051817, 1.66088144, 1.66071461, 1.660551217, 1.660391156, 1.660234326, 1.66008063, 1.659929976, 1.659782273, 1.659637437, 1.659495383, 1.659356034, 1.659219312, 1.659085144, 1.658953458, 1.658824187, 1.658697265, 1.658572629, 1.658450216, 1.658329969, 1.65821183, 1.658095744, 1.657981659, 1.657869522, 1.657759285, 1.657650899, 1.657544319, 1.657439499, 1.657336397, 1.65723497, 1.657135178, 1.657036982, 1.656940344, 1.656845226, 1.656751594, 1.656659413, 1.656568649, 1.65647927, 1.656391244, 1.656304542, 1.656219133, 1.656134988, 1.65605208, 1.655970382, 1.655889868, 1.655810511, 1.655732287, 1.655655173, 1.655579143, 1.655504177, 1.655430251, 1.655357345, 1.655285437, 1.655214506, 1.655144534, 1.6550755, 1.655007387, 1.654940175, 1.654873847, 1.654808385, 1.654743774, 1.654679996, 1.654617035, 1.654554875, 1.654493503, 1.654432901, 1.654373057, 1.654313957, 1.654255585, 1.654197929, 1.654140976, 1.654084713, 1.654029128, 1.653974208, 1.653919942, 1.653866317, 1.653813324, 1.653760949, 1.653709184, 1.653658017, 1.653607437, 1.653557435, 1.653508002, 1.653459126, 1.6534108, 1.653363013, 1.653315758, 1.653269024, 1.653222803, 1.653177088, 1.653131869, 1.653087138, 1.653042889, 1.652999113, 1.652955802, 1.652912949, 1.652870547, 1.652828589, 1.652787068, 1.652745977, 1.65270531, 1.652665059, 1.652625219, 1.652585784, 1.652546746, 1.652508101, 1.652469842, 1.652431964, 1.65239446, 1.652357326, 1.652320556, 1.652284144, 1.652248086, 1.652212376, 1.652177009, 1.652141981, 1.652107286, 1.65207292, 1.652038878, 1.652005156, 1.651971748, 1.651938651, 1.651905861, 1.651873373, 1.651841182, 1.651809286, 1.651777679, 1.651746359, 1.65171532, 1.65168456, 1.651654074, 1.651623859, 1.651593912, 1.651564228, 1.651534805, 1.651505638, 1.651476725, 1.651448062, 1.651419647, 1.651391475, 1.651363544, 1.65133585, 1.651308391, 1.651281164, 1.651254165, 1.651227393, 1.651200843, 1.651174514, 1.651148402, 1.651122505, 1.65109682, 1.651071345, 1.651046077, 1.651021013, 1.650996152, 1.65097149, 1.650947025, 1.650922755, 1.650898678, 1.650874791, 1.650851092, 1.650827579};
    constexpr float CHI_SQUARES[256] = {3.84146, 5.99146, 7.81473, 9.48773, 11.0705, 12.59159, 14.06714, 15.50731, 16.91898, 18.30704, 19.67514, 21.02607, 22.36203, 23.68479, 24.99579, 26.29623, 27.58711, 28.8693, 30.14353, 31.41043, 32.67057, 33.92444, 35.17246, 36.41503, 37.65248, 38.88514, 40.11327, 41.33714, 42.55697, 43.77297, 44.98534, 46.19426, 47.39988, 48.60237, 49.80185, 50.99846, 52.19232, 53.38354, 54.57223, 55.75848, 56.94239, 58.12404, 59.30351, 60.48089, 61.65623, 62.82962, 64.00111, 65.17077, 66.33865, 67.50481, 68.66929, 69.83216, 70.99345, 72.15322, 73.31149, 74.46832, 75.62375, 76.7778, 77.93052, 79.08194, 80.2321, 81.38102, 82.52873, 83.67526, 84.82065, 85.96491, 87.10807, 88.25016, 89.39121, 90.53123, 91.67024, 92.80827, 93.94534, 95.08147, 96.21667, 97.35097, 98.48438, 99.61693, 100.74862, 101.87947, 103.00951, 104.13874, 105.26718, 106.39484, 107.52174, 108.64789, 109.77331, 110.898, 112.02199, 113.14527, 114.26787, 115.38979, 116.51105, 117.63165, 118.75161, 119.87094, 120.98964, 122.10773, 123.22522, 124.34211, 125.45842, 126.57415, 127.68931, 128.80391, 129.91796, 131.03146, 132.14442, 133.25686, 134.36878, 135.48018, 136.59107, 137.70146, 138.81136, 139.92077, 141.0297, 142.13816, 143.24615, 144.35367, 145.46074, 146.56736, 147.67353, 148.77926, 149.88456, 150.98943, 152.09388, 153.1979, 154.30152, 155.40472, 156.50752, 157.60992, 158.71193, 159.81355, 160.91478, 162.01563, 163.1161, 164.2162, 165.31593, 166.4153, 167.5143, 168.61295, 169.71125, 170.8092, 171.9068, 173.00406, 174.10098, 175.19757, 176.29382, 177.38975, 178.48535, 179.58063, 180.6756, 181.77025, 182.86458, 183.95861, 185.05233, 186.14575, 187.23887, 188.33169, 189.42422, 190.51646, 191.6084, 192.70007, 193.79144, 194.88254, 195.97336, 197.06391, 198.15418, 199.24418, 200.33391, 201.42337, 202.51258, 203.60152, 204.6902, 205.77863, 206.8668, 207.95472, 209.04239, 210.12981, 211.21698, 212.30391, 213.3906, 214.47705, 215.56326, 216.64924, 217.73498, 218.82049, 219.90577, 220.99082, 222.07565, 223.16025, 224.24462, 225.32878, 226.41272, 227.49644, 228.57994, 229.66323, 230.7463, 231.82917, 232.91182, 233.99427, 235.07651, 236.15855, 237.24038, 238.32201, 239.40344, 240.48467, 241.56571, 242.64654, 243.72719, 244.80764, 245.8879, 246.96797, 248.04784, 249.12754, 250.20704, 251.28636, 252.3655, 253.44445, 254.52322, 255.60182, 256.68023, 257.75847, 258.83653, 259.91441, 260.99212, 262.06966, 263.14702, 264.22422, 265.30124, 266.3781, 267.45479, 268.53131, 269.60767, 270.68387, 271.7599, 272.83577, 273.91148, 274.98703, 276.06242, 277.13765, 278.21272, 279.28764, 280.36241, 281.43702, 282.51148, 283.58578, 284.65994, 285.73394, 286.80779, 287.8815, 288.95506, 290.02847, 291.10174, 292.17486, 293.24784, 294.32067};
    const double SQRTPI_2 = std::sqrt(3.141592653589793) / 2;

    struct ProfilePoint
    {
        float mz;
        float rt;
        int scan;
        float intensity;
    };

    struct TaskItem
    {
        std::filesystem::path path;
        unsigned int replicateGroup; // @todo
        unsigned int fileID;         // count upwards after reading everything in
    };

    /// @todo make universal print function, fix out-of-bounds access, chi squared test for binning
    void printPeaklist(std::vector<std::vector<CentroidPeak>> peaktable,
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
            std::cout << "writing peaks to: " << pathOutput << "\n\n";
        }

        std::ofstream file_out;
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

        output << "ID,mz,mzUncertainty,scanNumber,retentionTime,area,areaUncertainty,"
               << "height,heightUncertainty,dqsCen\n";
        unsigned int counter = 1;
        for (size_t i = 0; i < peaktable.size(); i++)
        {
            if (!peaktable[i].empty())
            {
                for (size_t j = 0; j < peaktable[i].size(); ++j)
                {
                    auto peak = peaktable[i][j];
                    char buffer[128];
                    sprintf(buffer, "%d,%0.6f,%0.6f,%d,%0.4f,%0.4f,%0.4f,%0.4f,%0.4f,%0.5f\n",
                            counter, peak.mz, peak.mzUncertainty, peak.scanNumber, convertRT[peak.scanNumber],
                            peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, peak.dqsCen);
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
            std::cout << "writing profile information of peaks to: " << pathOutput << "\n\n";
        }

        std::ofstream file_out;
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

    void printFeatureList(std::vector<FeaturePeak> peaktable,
                          std::filesystem::path pathOutput, std::string filename,
                          std::vector<EIC> originalBins,
                          bool verbose, bool silent, bool skipError, bool noOverwrite)
    {
        if (false) // @todo
        {
            filename += "_ex";
        }

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
            std::cout << "writing peaks to: " << pathOutput << "\n\n";
        }

        std::ofstream file_out;
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
            output << "ID,binID,binIdxStart,binIdxEnd,mz,mzUncertainty,retentionTime,retentionTimeUncertainty,"
                   << "lowestRetentionTime,highestRetentionTime,area,areaUncertainty,height,heightUncertainty,"
                   << "binTestCode,dqsCen,dqsBin,dqsPeak\n";
            unsigned int counter = 1;
            for (size_t i = 0; i < peaktable.size(); i++)
            {
                auto peak = peaktable[i];
                int binID = peak.idxBin;
                std::vector<float> RTs = originalBins[binID].rententionTimes;

                char buffer[256];
                sprintf(buffer, "%d,%d,%d,%d,%0.6f,%0.6f,%0.4f,%0.4f,%0.4f,%0.4f,%0.3f,%0.3f,%0.3f,%0.3f,%d,%0.5f,%0.5f,%0.5f\n",
                        counter, binID, peak.idxPeakStart, peak.idxPeakEnd, peak.mz, peak.mzUncertainty,
                        peak.retentionTime, peak.retentionTimeUncertainty, RTs[peak.idxPeakStart], RTs[peak.idxPeakEnd],
                        peak.area, peak.areaUncertainty, peak.height, peak.heightUncertainty, int(originalBins[binID].errorcode),
                        peak.dqsCen, peak.dqsBin, peak.dqsPeak);
                output << buffer;
                ++counter;
            }
        }
        else
        {
            /// code @todo should this exist?
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
                fileConflict.read(firstChars, readLength);
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

        float vcrit = 3.05037165842070 * pow(log(peaksize), (TOLERANCE_BINNING)) * stddev;
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
                                 "                          By default, this value is set to 5.\n"
                                 "      -mz-abs <number>:   add this absolute error (in Dalton) to the relative error specified by -ppm.\n"
                                 "      -divination:        process a profile mode dataset and return the best estimates for -ppm and -mz-abs.\n"
                                 "                          This is intended for cases in which some data is only available in centroided form,\n"
                                 "                          but an equivalent measurement in profile mode exists. As the name implies, this\n"
                                 "                          is only intended for giving a realistic estimate and in no way statistically sound.\n"; //@todo
}

int main(int argc, char *argv[])
{
    using namespace qAlgorithms; // considered bad practice from what i see online, but i believe it is acceptable for this program

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
        if ((filename_input == "-h") || (filename_input == "-help"))
        {
            std::cout << "    " << argv[0] << helpinfo << "\n\n";
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
        else if ((filename_output == "-h") || (filename_output == "-help"))
        {
            std::cout << "    " << argv[0] << helpinfo << "\n\n";
            exit(0);
        }

        else
        {
            pathOutput = filename_output;
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
    volatile bool doLogging = false;
    volatile bool noOverwrite = false;
    volatile bool predictFuture = false; // @todo implement
    std::string logfileName = "log_qAlgorithms.csv";

    for (int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];
        if ((argument == "-h") || (argument == "-help"))
        {
            std::cout << "\n    " << argv[0] << helpinfo;
            exit(0);
        }
        else if (argument == "-divination")
        {
            predictFuture = true;
        }
        else if ((argument == "-s") || (argument == "-silent"))
        {
            silent = true;
        }
        else if ((argument == "-v") || (argument == "-verbose"))
        {
            verboseProgress = true;
        }
        else if ((argument == "-i") || (argument == "-input"))
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
        else if ((argument == "-tl") || (argument == "-tasklist")) // @todo test this
        {
            std::cerr << "The tasklist functionality is not implemented currently. Please use -i.\n";
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
                    // this argument is set to increment the series
                    if (line.find("$SERIES") != line.npos)
                    {
                        /* code */
                    }
                    else if (line.find("$REPLICATES") != line.npos)
                    {
                        // this argument denotes a series of replicates (repeat injection)
                    }

                    continue;
                }

                suppliedPaths.push_back(line);
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
        else if ((argument == "-o") || (argument == "-output"))
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
            if (std::isnan(modifiedPPM))
            {
                std::cerr << "Error: the centroid error must be a number, but was set to NaN.";
                exit(1);
            }
            if (modifiedPPM > 1000000)
            {
                std::cerr << "Error: the centroid error is set to 100% or greater (\"" << argv[i] << "\").";
                exit(1);
            }
            if (modifiedPPM > 0.5 || modifiedPPM < 0.05)
            {
                std::cerr << "Warning: you have set the centroid error to \"" << argv[i] << "\", with the expected range \n"
                          << "being between 0.05 and 0.5 ppm. Are you sure this is what you want?\n"
                          << "The centroid error is not the mz tolerance from ROI-type approaches.\n";
            }

            std::cerr << "Notice: the changed centroid certainty will only affect pre-centroided data.\n";

            PPM_PRECENTROIDED = modifiedPPM;
        }
        else if ((argument == "-pc") || (argument == "-printcentroids"))
        {
            printCentroids = true;
        }
        else if ((argument == "-ps") || (argument == "-printsummary"))
        {
            printSummary = true;
        }
        else if ((argument == "-pb") || (argument == "-printbins"))
        {
            printSummary = true;
            printBins = true;
        }
        else if ((argument == "-pp") || (argument == "-printpeaks"))
        {
            printPeaks = true;
        }
        else if ((argument == "-e") || (argument == "-extended"))
        {
            printExtended = true;
            printPeaks = true;
        }
        else if ((argument == "-sp") || (argument == "-subprofile"))
        {
            printSubProfile = true;
        }
        else if ((argument == "-pa") || (argument == "-printall"))
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
            doLogging = true;
            if (i + 1 != argc)
            {
                std::string logName = argv[i + 1];
                if (logName[0] != '-')
                {
                    logfileName = logName;
                }
            }
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
    } // end of reading in command line arguments

    if (predictFuture)
    {
        // if (tasklistSpecified)
        // {
        //     // set params for the following measurements dynamically @todo
        // }

        std::cerr << "qAlgorithms will try to predict the best settings for user-supplied mass inaccuracies.\n"
                  << "This is an estimate - uniform parameters for every centroid do not accurately represent reality!\n";
        silent = false;
        verboseProgress = false;
        skipError = false;
        printSummary = false;
        printBins = false;
        printExtended = false;
        printCentroids = false;
        printSubProfile = false;
        doLogging = false;
    }

    if (!inSpecified && !tasklistSpecified)
    {
        std::cerr << "Error: no input file supplied. Specify a file or directorey using the -i or "
                  << "-tl flag. Execute qAlgorithms with the -h flag for more information.\n";
        exit(1);
    }
    if (!outSpecified)
    {
        std::cerr << "Error: You must specify an output directory.\n";
        std::cerr.flush();
        exit(100);
    }
    if (inSpecified && tasklistSpecified)
    {
        std::cerr << "Warning: Both an input file and a tasklist were specified. The file has been added to the tasklist.\n";
        // @todo
    }
    if (silent && verboseProgress)
    {
        std::cerr << "Warning: -verbose overrides -silent.\n";
        silent = false;
    }
    if (!((printCentroids || printSummary) || printPeaks))
    {
        std::cerr << "Warning: no output files will be written.\n";
    }

    // the final task list contains only unique files, sorted by filesize
    tasklist = controlInput(suppliedPaths, ".mzML", skipError);

    if (predictFuture && tasklist.size() > 1)
    {
        std::cerr << "Error: prediction of good centroid error estimation only works on one file!\n";
        exit(1);
    }

#pragma endregion cli arguments

    // Temporary diagnostics file creation, rework this into the log function?
    std::filesystem::path pathLogging{argv[0]};
    // pathLogging.filename() = "log_qBinning_beta.csv";
    pathLogging = std::filesystem::canonical(pathLogging.parent_path());
    pathLogging /= logfileName;
    std::fstream logWriter;
    if (doLogging)
    /// @todo make a separate logging object
    {
        if (std::filesystem::exists(pathLogging))
        {
            std::cerr << "Warning: the processing log has been overwritten\n";
        }
        logWriter.open(pathLogging, std::ios::out);
        logWriter << "filename, numSpectra, numCentroids, meanDQSC, numBins, meanDQSB, numFeatures, badFeatures, meanDQSF\n";
        logWriter.close();
    }

#pragma region file processing
    std::string filename;
    const std::vector<std::string> polarities = {"positive", "negative"}; // @todo make bool
    int counter = 1;
    // @todo add dedicated diagnostics function
    // std::cout << "numCens,numBins,numPeaks,numLoadedBins,numNarrowBin,\n";
    for (std::filesystem::path pathSource : tasklist)
    {

        if (!outSpecified)
        {
            std::cerr << "Warning: no output location specified, printing output to file location\n";
            pathOutput = pathSource.parent_path(); // @todo include route to standard out
        }
        else if (!silent && (printPeaks || printSummary))
        {
            std::cout << "printing output to: " << pathOutput;
        }

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
        // implement way to try out ppm values
        // std::vector<float> ppmValues{0.01, 0.05, 0.1, 0.2, 0.25, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 7.5, 10, 15, 20};

        // for (auto setPPM : ppmValues)
        for (auto polarity : polarities)
        {
            // std::string polarity = "positive";
            // PPM_PRECENTROIDED = setPPM;
            filename = pathSource.stem().string();
            std::vector<unsigned int> addEmptyScans; // make sure the retention time interpolation does not add unexpected points to bins
            std::vector<float> convertRT;
            float diff_rt = 0;
            // @todo add check if set polarity is correct
            std::vector<std::vector<CentroidPeak>> centroids =
                findCentroids_MZML(data, addEmptyScans, convertRT, diff_rt, true, polarity, 10); // read mzML file and find centroids via qPeaks
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
            // filename += ("_" + std::to_string(PPM_PRECENTROIDED) + "pos");
            filename += polarity;
            if (printCentroids)
            {
                printPeaklist(centroids, convertRT, pathOutput, filename, silent, skipError, noOverwrite);
            }

            // @todo remove diagnostics later

            CentroidedData binThis = passToBinning(centroids, addEmptyScans);

            double meanDQSC = 0;
            double meanCenErrorRel = 0;
            double meanCenErrorAbs = 0;

            // index in allDatapoints corresponds to scan number!
            for (size_t i = 1; i < binThis.allDatapoints.size() - 1; i++)
            {
                if (binThis.allDatapoints[i].empty())
                {
                    continue;
                }

                for (size_t j = 0; j < binThis.allDatapoints[i].size() - 1; j++)
                {
                    // correlate centroid error with nearest neighbour distance in one scan somehow?
                    meanCenErrorRel += binThis.allDatapoints[i][j].mzError / binThis.allDatapoints[i][j].mz;
                    meanCenErrorAbs += binThis.allDatapoints[i][j].mzError;
                    meanDQSC += binThis.allDatapoints[i][j].DQSCentroid;
                }
            }
            meanCenErrorAbs /= binThis.lengthAllPoints;
            meanCenErrorRel /= binThis.lengthAllPoints;
            //

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
            std::vector<EIC> binnedData = performQbinning(
                binThis, convertRT, pathOutput, filename, 3, !verboseProgress, printSummary, printBins); // set maxdist here
            timeEnd = std::chrono::high_resolution_clock::now();

            if (binnedData.size() == 0)
            {
                std::cerr << "Error: no bins could be constructed from the data.\n";
                if (!skipError)
                {
                    exit(1);
                }
                else
                {
                    continue;
                }
            }

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
            auto peaks = findPeaks_QBIN(binnedData, diff_rt);
            // make sure that every peak contains only one mass trace
            assert(peaks.size() < binnedData.size());
            int peaksWithMassGaps = 0;
            double meanDQSF = 0;
            for (size_t i = 0; i < peaks.size(); i++)
            {
                int binIdx = peaks[i].idxBin;
                auto massesBin = binnedData[binIdx].mz;
                // auto rtsBin = binnedData[binIdx].rententionTimes;
                auto scansBin = binnedData[binIdx].scanNumbers;

                // idxPeakStart/End are the index referring to the bin in which a peak was found
                if (!massTraceStable(massesBin, peaks[i].idxPeakStart, peaks[i].idxPeakEnd))
                {
                    ++peaksWithMassGaps;
                    // std::cerr << peaks[i][j].mz << ", " << peaks[i][j].retentionTime << ", "
                    //           << peaks[i][j].area << ", " << peaks[i][j].dqsPeak << "\n";
                    // problems do not only occur at very low masses or the edges of out chromoatography
                    // recommendation: do not pass these peaks to result table
                    peaks[i].dqsPeak *= -1;

                    // @todo consider removing these or add a correction set somewhere later
                    // @todo add some documentation regarding the scores
                }
                else
                {
                    meanDQSF += peaks[i].dqsPeak;
                }
            }

            // std::cout << testsPassedTotal << ", " << testsPassedPeak << ", " << testsFailedPeak << ", " << peaks.size() - testsPassedTotal << "\n";

            if (verboseProgress)
            {
                std::cout << peaksWithMassGaps << " peaks were erroneously constructed from more than one mass trace\n";
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            meanDQSF /= peaks.size() - peaksWithMassGaps;
            if (!silent)
            {
                std::cout << "    found " << peaks.size() << " peaks in " << (timeEnd - timeStart).count() << " ns\n";
            }
            if (doLogging)
            {

                logWriter.open(pathLogging, std::ios::app);
                logWriter << filename << ", " << centroids.size() << ", " << binThis.lengthAllPoints << ", "
                          << meanDQSC / binThis.lengthAllPoints << ", " << binnedData.size() << ", " << meanDQSB
                          << ", " << peaks.size() << ", " << peaksWithMassGaps << ", " << meanDQSF << "\n";
                logWriter.close();
            }
            if (printPeaks)
            {
                printFeatureList(peaks, pathOutput, filename, binnedData, printExtended, silent, skipError, noOverwrite);
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

            // auto peakpointer = collapseFeaturelist(peaks);
            // initialComponentBinner(peaks, 1);
        }
        counter++;
    }
    if (!silent)
    {
        std::cout << "Completed data processing on " << tasklist.size() << " files.\n\n";
    }

    return 0;
}