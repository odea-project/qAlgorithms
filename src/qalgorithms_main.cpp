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

namespace qAlgorithms
{
    /// ### set global variables ###
    float PPM_PRECENTROIDED = -INFINITY;                                         // -Infinity sets it to the default value if no user input changes it
    float MZ_ABSOLUTE_PRECENTROIDED = -INFINITY;                                 // see above
    std::array<float, 384> INV_ARRAY = initialize();                             // @todo make constexpr
    bool massTraceStable(std::vector<float> massesBin, int idxStart, int idxEnd) // @todo do this in regression

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

        // float vcrit = 3.05037165842070 * pow(log(peaksize), (TOLERANCE_BINNING)) * stddev;
        float vcrit = binningCritVal(peaksize, stddev);
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
        logWriter << "filename, numSpectra, numCentroids, meanDQSC, numBins, binsTooLarge, meanDQSB, numFeatures, badFeatures, meanDQSF\n";
        logWriter.close();
    }

#pragma region file processing
    std::string filename;
    const std::vector<std::string> polarities = {"positive", "negative"}; // @todo make bool
    int counter = 1;
    // @todo add dedicated diagnostics function
    // std::cout << "numCens,numBins,numPeaks,numLoadedBins,numNarrowBin,\n";
    // #pragma omp parallel for // there is a memory leak in findCentroids_MZML
    for (std::filesystem::path pathSource : tasklist)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();
        if (!userArgs.silent)
        {
            std::cout << "\nreading file " << counter << " of " << tasklist.size() << ":\n"
                      << pathSource << "\n... ";
        }

        sc::MZML data(std::filesystem::canonical(pathSource).string()); // create mzML object @todo change to use filesystem::path

        if (!data.loading_result)
        {
            std::cerr << "Error: the file is defective.\n";
            if (userArgs.skipError)
            {
                ++counter;
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
        // implement way to try out ppm values
        // std::vector<float> ppmValues{0.01, 0.05, 0.1, 0.2, 0.25, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 7.5, 10, 15, 20};

        // for (auto setPPM : ppmValues)
        for (auto polarity : polarities)
        {
            // std::string polarity = "positive";
            filename = pathSource.stem().string();
            std::vector<unsigned int> addEmptyScans; // make sure the retention time interpolation does not add unexpected points to bins
            std::vector<float> convertRT;
            float diff_rt = 0;
            // @todo add check if set polarity is correct
            std::vector<std::vector<CentroidPeak>> centroids =
                findCentroids_MZML(data, addEmptyScans, convertRT, diff_rt, true, polarity, 10); // read mzML file and find centroids via qPeaks
            if (centroids.empty())
            {
                if (userArgs.verboseProgress)
                {
                    std::cout << "skipping mode: " << polarity << "\n";
                }
                continue;
            }
            if (!userArgs.silent)
            {
                std::cout << "Processing " << polarity << " peaks\n";
            }
            // continue;
            // adjust filename to include polarity here
            // filename += ("_" + std::to_string(PPM_PRECENTROIDED) + "pos");
            filename = filename + "_" + polarity;
            if (userArgs.printCentroids)
            {
                printCentroids(centroids, convertRT, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            // @todo remove diagnostics later
            auto binThis = passToBinning(centroids, addEmptyScans);

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
            //

            if (userArgs.printSubProfile)
            {
                // create a subset of the profile data which only contains binned points
                // organisation: bin ID -> vector<vector<profile point>>
                // after peak finding, use the peak borders to annotate which profile
                // points were part of which peak.
                // @todo consider adding the option to also print surrounding profile points
            }

            auto timeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

            if (!userArgs.silent)
            {
                std::cout << "    produced " << binThis.size() << " centroids from " << centroids.size()
                          << " spectra in " << timePassed.count() << " s\n";
            }

            timeStart = std::chrono::high_resolution_clock::now();

            std::vector<EIC> binnedData = performQbinning(
                &binThis, convertRT, 3, // set maxdist here - reasoned as likelihood of real trace being overlooked being at worst 50%
                userArgs.verboseProgress);

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
                printBins(binnedData, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
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
                auto scansBin = binnedData[binIdx].scanNumbers;

                // idxPeakStart/End are the index referring to the bin in which a peak was found
                if (!massTraceStable(massesBin, peaks[i].idxPeakStart, peaks[i].idxPeakEnd))
                {
                    ++peaksWithMassGaps;
                    peaks[i].dqsPeak *= -1;
                    // @todo consider removing these or add a correction set somewhere later
                    // @todo add some documentation regarding the scores
                }
                else
                {
                    meanDQSF += peaks[i].dqsPeak;
                }
            }

            if (userArgs.verboseProgress)
            {
                std::cout << peaksWithMassGaps << " peaks were erroneously constructed from more than one mass trace\n";
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            meanDQSF /= peaks.size() - peaksWithMassGaps;
            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    found " << peaks.size() << " peaks in " << timePassed.count() << " s\n";
            }
            if (userArgs.doLogging)
            {

                logWriter.open(pathLogging, std::ios::app);
                logWriter << filename << ", " << centroids.size() << ", " << binThis.size() << ", "
                          << meanDQSC / binThis.size() << ", " << binnedData.size() << ", " << badBinCount << ", " << meanDQSB
                          << ", " << peaks.size() << ", " << peaksWithMassGaps << ", " << meanDQSF << "\n";
                logWriter.close();
            }
            if (userArgs.printFeatures)
            {
                printFeatureList(peaks, userArgs.outputPath, filename, binnedData,
                                 userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            // @todo add peak grouping here
        }
        counter++;
    }
    auto absoluteEnd = std::chrono::high_resolution_clock::now();
    if (!userArgs.silent)
    { // @todo add number of errors
        std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(absoluteEnd - absoluteStart);
        std::cout << "Completed data processing on " << tasklist.size() << " files in " << timePassed.count() << " s.\n\n";
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