// internal
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_input_output.h"

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
#include <numeric>   // infinity macro, sqrt

namespace qAlgorithms
{
    /// ### set global variables ###
    float PPM_PRECENTROIDED = -INFINITY;                                         // -Infinity sets it to the default value if no user input changes it
    float MZ_ABSOLUTE_PRECENTROIDED = -INFINITY;                                 // see above
    std::array<float, 384> INV_ARRAY = initialize();                             // @todo make constexpr
    bool massTraceStable(std::vector<float> massesBin, int idxStart, int idxEnd) // @todo do this in regression
    {
        assert(idxEnd > idxStart);
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

        // float vcrit = 3.05037165842070 * pow(log(peaksize), (TOLERANCE_BINNING)) * stddev;
        float vcrit = binningCritVal(peaksize, stddev);
        for (size_t i = 1; i < peaksize; i++)
        {
            [[unlikely]] if (massesPeak[i] - massesPeak[i - 1] > vcrit)
            {
                return false;
            }
        }
        return true;
    }

}

int main(int argc, char *argv[])
{
    using namespace qAlgorithms; // considered bad practice from what i see online, but i believe it is acceptable for this program

    UserInputSettings userArgs = passCliArgs(argc, argv);

    if (!inputsAreSensible(userArgs))
    {
        exit(1);
    }

    // the final task list contains only unique files, sorted by filesize
    auto tasklist = controlInput(userArgs.inputPaths, userArgs.skipError);

    auto absoluteStart = std::chrono::high_resolution_clock::now();

    // Temporary diagnostics file creation, rework this into the log function?
    std::filesystem::path pathLogging{argv[0]};
    std::string logfileName = "log_qAlgorithms.csv";
    pathLogging = std::filesystem::canonical(pathLogging.parent_path());
    pathLogging /= logfileName;
    std::fstream logWriter;
    if (userArgs.doLogging)
    /// @todo make a separate logging object
    {
        if (std::filesystem::exists(pathLogging))
        {
            std::cerr << "Warning: the processing log has been overwritten\n";
        }
        logWriter.open(pathLogging, std::ios::out);
        logWriter << "filename, numSpectra, numCentroids, meanDQSC, numBins, binsTooLarge, meanDQSB, numFeatures, badFeatures, meanInterpolations, meanDQSF\n";
        logWriter.close();
    }

#pragma region file processing
    std::string filename;
    size_t counter = 1;
    size_t errorCount = 0;
    for (std::filesystem::path pathSource : tasklist)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        if (!userArgs.silent)
        {
            std::cout << "\nreading file " << counter << " of " << tasklist.size() << ":\n"
                      << pathSource << "\n... ";
        }

        StreamCraft::MZML data(std::filesystem::canonical(pathSource));

        if (!data.loading_result)
        {
            std::cerr << "Error: the file is defective.\n";
            if (userArgs.skipError)
            {
                ++counter;
                ++errorCount;
                continue;
            }
            else
            {
                exit(101);
            }
        }

        if (!userArgs.silent)
        {
            std::cout << " file ok\n";
        }
        // @todo find a more elegant solution for polarity switching
        bool oneProcessed = true;
        static bool polarities[2] = {true, false};
        for (bool polarity : polarities)
        {
            filename = pathSource.stem().string();
#pragma region "centroiding"
            std::vector<float> convertRT;
            float diff_rt = 0;
            // @todo add check if set polarity is correct
            std::vector<CentroidPeak> centroids = findCentroids_MZML(data, convertRT, diff_rt, polarity);
            if (centroids.empty())
            {
                if (userArgs.verboseProgress)
                {
                    std::cout << "skipping mode: " << polarity << "\n";
                }
                continue;
            }
            else
            {
                oneProcessed = true;
            }
            // oneProcessed is true if this is the first loop iteration or if centroids were found in the previous iteration
            if (!oneProcessed)
            {
                if (userArgs.skipError)
                {
                    ++counter;
                    ++errorCount;
                    continue;
                }
                else
                {
                    exit(101);
                }
                std::cerr << "error: no centroids were found in the file";
            }
            oneProcessed = false;

            if (!userArgs.silent)
            {
                std::cout << "Processing " << (polarity ? "positive" : "negative") << " peaks\n";
            }

            filename = filename + (polarity ? "_positive" : "_negative");

            if (userArgs.printCentroids)
            {
                printCentroids(centroids, convertRT, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }
            // @todo remove diagnostics later
            auto binThis = passToBinning(centroids);

            exit(1);

            double meanDQSC = 0;
            double meanCenErrorRel = 0;
            double meanCenErrorAbs = 0;

            // index in allDatapoints corresponds to scan number!
            for (size_t i = 1; i < binThis.size() - 1; i++)
            {
                // correlate centroid error with nearest neighbour distance in one scan somehow?
                meanCenErrorRel += binThis[i].mzError / binThis[i].mz;
                meanCenErrorAbs += binThis[i].mzError;
                meanDQSC += binThis[i].DQSCentroid;
            }
            meanCenErrorAbs /= binThis.size();
            meanCenErrorRel /= binThis.size();

            auto timeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

            if (!userArgs.silent)
            {
                std::cout << "    produced " << binThis.size() - 1 << " centroids from " << data.number_spectra
                          << " spectra in " << timePassed.count() << " s\n";
            }

#pragma region "binning"
            timeStart = std::chrono::high_resolution_clock::now();

            std::vector<EIC> binnedData = performQbinning(&binThis, &convertRT, userArgs.verboseProgress);

            timeEnd = std::chrono::high_resolution_clock::now();

            if (binnedData.size() == 0)
            {
                std::cerr << "Error: no bins could be constructed from the data.\n";
                if (!userArgs.skipError)
                {
                    exit(1);
                }
                else
                {
                    ++counter;
                    ++errorCount;
                    continue;
                }
            }

            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    assembled " << binnedData.size() << " bins in " << timePassed.count() << " s\n";
            }
            if (userArgs.printBins)
            {
                printBins(binThis, binnedData, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            // @todo remove diagnostics
            int count = 0;
            int badBinCount = 0;
            double meanDQSB = 0;
            for (auto EIC : binnedData)
            {
                for (double dqsb : EIC.DQSB)
                {
                    if (dqsb == -1)
                    {
                        badBinCount++;
                        break;
                    }
                    ++count;
                    meanDQSB += dqsb;
                }
            }
            meanDQSB /= count;
#pragma region "feature detection"
            timeStart = std::chrono::high_resolution_clock::now();
            // every subvector of peaks corresponds to the bin ID
            auto peaks = findPeaks_QBIN(binnedData, &centroids, diff_rt);
            // make sure that every peak contains only one mass trace
            assert(peaks.size() <= binnedData.size());
            int peaksWithMassGaps = 0;
            double meanDQSF = 0;
            double meanInterpolations = 0;
            for (size_t i = 0; i < peaks.size(); i++)
            {
                int binIdx = peaks[i].idxBin;
                auto massesBin = binnedData[binIdx].mz;
                auto scansBin = binnedData[binIdx].scanNumbers;
                assert(peaks[i].idxPeakStart < massesBin.size() - 4);
                assert(peaks[i].idxPeakEnd < massesBin.size());

                // idxPeakStart/End are the index referring to the bin in which a peak was found
                if (!massTraceStable(massesBin, peaks[i].idxPeakStart, peaks[i].idxPeakEnd))
                {
                    ++peaksWithMassGaps;
                    peaks[i].DQSF *= -1;
                    // @todo consider removing these or add a correction set somewhere later
                    // @todo add some documentation regarding the scores
                }
                else
                {
                    meanDQSF += peaks[i].DQSF;
                }
                meanInterpolations += peaks[i].interpolationCount - 4;
            }
            meanDQSF /= peaks.size() - peaksWithMassGaps;
            meanInterpolations /= peaks.size();
            if (userArgs.verboseProgress)
            {
                std::cout << peaksWithMassGaps << " peaks were erroneously constructed from more than one mass trace\n";
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    found " << peaks.size() << " peaks in " << timePassed.count() << " s\n";
            }
            // no fail condition here, since this case can occur with real data
            if (userArgs.doLogging)
            {

                logWriter.open(pathLogging, std::ios::app);
                logWriter << filename << ", " << centroids.size() << ", " << binThis.size() << ", "
                          << meanDQSC / binThis.size() << ", " << binnedData.size() << ", " << badBinCount << ", " << meanDQSB
                          << ", " << peaks.size() << ", " << peaksWithMassGaps << ", " << meanInterpolations << ", " << meanDQSF << "\n";
                logWriter.close();
            }
            if (userArgs.printFeatures) // @todo revert
            {
                // printFeatureList(peaks, userArgs.outputPath, filename, binnedData,
                //                  userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }
            if (userArgs.printFeatCens)
            {
                printFeatureCentroids(peaks, userArgs.outputPath, filename, binnedData,
                                      userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

#pragma region "Componentisation"

            findComponents(&peaks, &binnedData);
        }
        counter++;
    }

#pragma region "Logging and similar"
    auto absoluteEnd = std::chrono::high_resolution_clock::now();
    if (!userArgs.silent)
    {
        std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(absoluteEnd - absoluteStart);
        std::cout << "Completed data processing on " << tasklist.size() << " files in " << timePassed.count() << " s.\n";
        if (errorCount > 0)
        {
            std::cout << "Skipped " << errorCount << " files which could not be processed.\n";
        }
        std::cout << std::endl;
    }
    if (userArgs.interactive)
    {
        std::cout << "   enter \"exit\" to close this program:\n";
        std::string userInput;
        while (userInput != "exit") // this loop should only terminate upon user input
        {
            std::cin >> userInput;
        }
    }

    return 0;
}