// internal
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_input_output.h"
#include "qalgorithms_metafiles.h" // new organisation of program - this and the library header should be the only two qalgo includes!
#include "qalgorithms_read_file.h"
#include "qalgorithms_measurement_data.h"

// external
#include <cassert>
#include <cmath>
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
    struct logger
    {
        std::string name;
        std::string series;
        unsigned int centroidCount;
        float meanDQSC;
        unsigned int binCount;
        unsigned int binCentroidCount;
        float meanDQSB;
        unsigned int featureCount;
        unsigned int positive_b23;
        unsigned int massGapsTotal;
        unsigned int meanINterpolations;
        float meanDQSF;
        unsigned int numComponents;
        unsigned int componentMembers;
        float meanDQSG;
    };

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

int main2(int argc, char *argv[])
{
    using namespace qAlgorithms; // considered bad practice from what i see online, but i believe it is acceptable for this program

    UserInputSettings userArgs = passCliArgs(argc, argv);

    if (!inputsAreSensible(userArgs))
    {
        exit(1);
    }

    bool argument_for_making_a_tasklist = false;
    if (argument_for_making_a_tasklist)
    {
        // make the tasklist
        std::cout << "### qAlgorithms successfully created a tasklist at " << "TASKLIST PATH HERE!" << " ###\n";
        exit(0);
    }

    std::vector<TaskItem_action> actions;
    std::vector<TaskItem_data> data;

    return 0;
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
    std::vector<std::filesystem::__cxx11::path> tasklist = controlInput(&userArgs.inputPaths, userArgs.skipError);
    if (tasklist.size() <= userArgs.skipAhead)
    {
        std::cerr << "Error: skipped more entries than were in taks list (" << tasklist.size() << ").\n";
        exit(1);
    }
    if (userArgs.skipAhead != 0)
    {
        std::cerr << "Warning: removing the first " << userArgs.skipAhead << " elements from the tasklist.\n";
        tasklist.erase(tasklist.begin(), tasklist.begin() + userArgs.skipAhead);
    }

    auto absoluteStart = std::chrono::high_resolution_clock::now();

    // Temporary diagnostics file creation, rework this into the log function?
    std::filesystem::path pathLogging{argv[0]};
    // std::string logfileName = "log_qAlgorithms.csv";
    // pathLogging = std::filesystem::canonical(pathLogging.parent_path());
    // pathLogging = logfileName;
    std::string logfileName = "_log.csv";
    pathLogging = std::filesystem::canonical(pathLogging);
    pathLogging += logfileName;
    std::fstream logWriter;
    if (userArgs.doLogging)
    /// @todo make a separate logging object
    {
        if (std::filesystem::exists(pathLogging))
        {
            std::cerr << "Warning: the processing log has been overwritten\n";
        }
        logWriter.open(pathLogging, std::ios::out);
        logWriter << "filename, numSpectra, numCentroids, meanDQSC, numBins, binsTooLarge, meanDQSB, numFeatures, badFeatures, meanInterpolations, meanDQSF, numComponentRegs, numComponentFeatures\n";
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

        XML_File data(std::filesystem::canonical(pathSource));

        if (data.defective)
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
        // the check for centroided data is set here, will likely require different approach down the line
        // accessor contains the indices of all spectra that should be fetched
        std::vector<unsigned int> accessor(data.number_spectra, 0);
        std::iota(accessor.begin(), accessor.end(), 0);

        const auto linkNodes = link_vector_spectra_nodes(data.mzml_root_node);
        std::vector<bool> spectrum_mode = data.get_spectra_mode(&accessor, &linkNodes); // get spectrum mode (centroid or profile)

        // CHECK IF CENTROIDED SPECTRA
        size_t num_centroided_spectra = std::count(spectrum_mode.begin(), spectrum_mode.end(), false);
        if (num_centroided_spectra > spectrum_mode.size() / 2) // in profile mode sometimes centroided spectra appear as well @todo is 2 a good idea?
        {
            std::cout << " file centroided, error\n";
            std::cerr << "Error: centroided data is not supported by qAlgorithms for the time being.\n";
            continue;
        }
        if (!userArgs.silent)
        {
            std::cout << " file in profile mode, ok\n";
        }

        auto polarity_file = data.get_polarity_mode(100, &linkNodes); // checks first 100 spectra

        // @todo single access function into qAlgorithms

        if (polarity_file == Polarities::positive || polarity_file == Polarities::mixed)
        {
            /* this does the processing for positive files - keep it to the function call and handle logging / output through master function arguments */
        }
        if (polarity_file == Polarities::negative || polarity_file == Polarities::mixed)
        {
            // see above
        }

        // @todo find a more elegant solution for polarity switching, this one trips up clang-tidy
        bool oneProcessed = true;
        static bool polarities[2] = {true, false};
        for (bool polarity : polarities)
        {
            filename = pathSource.stem().string();
#pragma region "centroiding"

            float diff_rt = 0;
            // @todo add check if set polarity is correct
            std::vector<float> convertRT = data.get_spectra_RT(&accessor, &linkNodes);
            auto relIndex = interpolateScanNumbers(&convertRT);

            std::vector<CentroidPeak> *centroids = new std::vector<CentroidPeak>;
            *centroids = findCentroids(data, &linkNodes, diff_rt, polarity); // it is guaranteed that only profile mode data is used

            if (centroids->empty())
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
            { // @todo this is really hard to follow, change it
                if (userArgs.skipError)
                {
                    ++counter;
                    ++errorCount;
                    continue;
                }
                else
                {
                    std::cerr << "error: no centroids were found in the file" << std::endl;
                    exit(101);
                }
            }
            oneProcessed = false;

            if (!userArgs.silent)
            {
                std::cout << "Processing " << (polarity ? "positive" : "negative") << " peaks\n";
            }

            filename = filename + (polarity ? "_positive" : "_negative");

            if (userArgs.printCentroids)
            {
                printCentroids(centroids, &convertRT, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);

                if (userArgs.term == TerminateAfter::centroids)
                {
                    continue;
                }
            }

            size_t centroidCount = centroids->size() - 1; // added noise value

            // find lowest intensity among all centroids to use as baseline during componentisation
            float minCenArea = INFINITY;
            for (size_t cenID = 1; cenID < centroids->size(); cenID++)
            {
                float currentInt = centroids->at(cenID).area;
                minCenArea = minCenArea < currentInt ? minCenArea : currentInt;
            }
            size_t totalScans = centroids->back().number_MS1;

            double meanDQSC = 0;

            for (size_t i = 1; i < centroids->size() - 1; i++)
            {
                // correlate centroid error with nearest neighbour distance in one scan somehow?
                meanDQSC += centroids->at(i).DQSC;
            }
            meanDQSC /= centroidCount;

            auto timeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

            if (!userArgs.silent)
            {
                std::cout << "    produced " << centroidCount - 1 << " centroids from " << totalScans
                          << " spectra in " << timePassed.count() << " s\n";
            }

#pragma region "binning"
            timeStart = std::chrono::high_resolution_clock::now();

            std::vector<EIC> binnedData = performQbinning(centroids, &convertRT, userArgs.verboseProgress);

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
                printBins(centroids, &binnedData, userArgs.outputPath, filename, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);

                if (userArgs.term == TerminateAfter::binning)
                {
                    continue;
                }
            }
            delete centroids;

            // @todo remove diagnostics
            int count = 0;
            int badBinCount = 0;
            double meanDQSB = 0;
            for (auto EIC : binnedData)
            {
                assert(EIC.scanNumbers.back() <= convertRT.size());
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

            // continue; // @todo ensure centroiding and binning work as best they can first

#pragma region "feature construction"
            timeStart = std::chrono::high_resolution_clock::now();
            // every subvector of peaks corresponds to the bin ID
            auto features = findFeatures(binnedData, diff_rt, convertRT.size());

            if (features.size() == 0)
            {
                std::cerr << "Warning: no features were constructed, continuing...\n";
                continue;
            }

            // make sure that every peak contains only one mass trace
            assert(features.size() <= binnedData.size());
            int peaksWithMassGaps = 0;
            double meanDQSF = 0;
            double meanInterpolations = 0;
            for (size_t i = 0; i < features.size(); i++)
            {
                assert(features[i].scanPeakEnd < convertRT.size());
                int binIdx = features[i].idxBin;
                auto massesBin = binnedData[binIdx].mz;
                auto scansBin = binnedData[binIdx].scanNumbers;
                unsigned int binStart = features[i].idxBinStart;
                unsigned int binEnd = features[i].idxBinEnd;
                assert(binStart < massesBin.size() - 4);
                assert(binEnd < massesBin.size());

                // idxPeakStart/End are the index referring to the bin in which a peak was found
                if (!massTraceStable(massesBin, binStart, binEnd))
                {
                    ++peaksWithMassGaps;
                    // peaks[i].DQSF *= -1;
                    // @todo consider removing these or add a correction set somewhere later
                    // @todo add some documentation regarding the scores
                }
                else
                {
                    meanDQSF += features[i].DQSF;
                }
                meanInterpolations += features[i].interpolationCount - 4;
            }
            meanDQSF /= features.size() - peaksWithMassGaps;
            meanInterpolations /= features.size();
            if (userArgs.verboseProgress)
            {
                std::cout << peaksWithMassGaps << " peaks were erroneously constructed from more than one mass trace\n";
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    constructed " << features.size() << " features in " << timePassed.count() << " s\n";
            }
            // no fail condition here, since this case can occur with real data

            if (userArgs.printFeatCens)
            {
                printFeatureCentroids(&features, userArgs.outputPath, filename, &binnedData,
                                      userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            if (userArgs.term == TerminateAfter::features)
            {
                if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
                {
                    printFeatureList(&features, userArgs.outputPath, filename, &binnedData,
                                     userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
                }
                continue;
            }

            continue; // @todo ensure feature constrction works correctly before redoing componentisation

#pragma region "Componentisation"
            timeStart = std::chrono::high_resolution_clock::now();

            size_t featuresInComponents = 0; // only used as a count statistic
            const auto components = findComponents(&features, &binnedData, &convertRT, minCenArea, &featuresInComponents);

            timeEnd = std::chrono::high_resolution_clock::now();
            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    grouped " << featuresInComponents << " features into " << components.size() << " components in " << timePassed.count() << " s\n";
            }

            if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
            {
                printFeatureList(&features, userArgs.outputPath, filename, &binnedData,
                                 userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            if (userArgs.printComponentRegs)
            {
                printComponentRegressions(&components, userArgs.outputPath, filename,
                                          userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            if (userArgs.printComponentBins)
            {
                printComponentCentroids(&components, &binnedData, userArgs.outputPath, filename,
                                        userArgs.printExtended, userArgs.silent, userArgs.skipError, userArgs.noOverwrite);
            }

            if (userArgs.term == TerminateAfter::components)
            {
                continue;
            }

            if (userArgs.doLogging) // @todo logging should account for early terminate
            {
                logWriter.open(pathLogging, std::ios::app);
                logWriter << filename << ", " << data.number_spectra << ", " << centroidCount << ", "
                          << meanDQSC << ", " << binnedData.size() << ", " << badBinCount << ", " << meanDQSB
                          << ", " << features.size() << ", " << peaksWithMassGaps << ", " << meanInterpolations << ", " << meanDQSF
                          << components.size() << ", " << featuresInComponents << "\n";
                logWriter.close();
            }
        }
        counter++;
    }

#pragma region "Logging and similar" // @todo add an option for custom logfile names
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