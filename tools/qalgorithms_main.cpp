// internal
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_global_vars.h"
#include "qalgorithms_input_output.h"
#include "qalgorithms_metafiles.h" // new organisation of program - this and the library header should be the only two qalgo includes!
#include "qalgorithms_read_file.h"
// #include "qalgorithms_measurement_data.h"

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

    // {
    //     // this struct is used to limit the amount of operations performed by the program to
    //     // a selection of masses and RTs that is relevant to the analysis at hand.
    //     double mz_expected[16] = {0};
    //     double mz_tolerance_ppm = 0;

    //     double RT = -1;
    //     double RT_tol = -1; // tolerance to either side, assumes symmetrical peaks

    //     std::string compoundName = "";
    //     std::string methodName = "";
    //     Polarities polarity = Polarities::unknown_polarity;
    // };

    // @todo this vector is a compiled-in version of the filter to skip file reading for now
    // CompoundFilter PFPeA{{262.9760, 218.9856, 0}, 10, 11.92 * 60, 20, "PFPeA", "M1", negative}; //
    // CompoundFilter PFOA{{412.9664, 368.9760, 0}, 10, 17.51 * 60, 20, "PFOA", "M1", negative};
    // CompoundFilter PFHxA{{312.9728, 268.9824, 0}, 10, 14.22 * 60, 20, "PFHxA", "M1", negative};
    CompoundFilter PFBS{{298.9429, 0}, 10, 12.36 * 60, 10, "PFBS", "M1", negative};
    // CompoundFilter PFBA{{212.9792, 168.9888, 0}, 10, 8.3 * 60, 20, "PFBA", "M1", negative};
    // CompoundFilter PFOS{{498.9302, 0}, 10, 19.98 * 60, 20, "PFOS", "M1", negative};

    // static std::vector<CompoundFilter> pfasFilter = {PFPeA, PFOA, PFHxA, PFBS, PFBA, PFOS};
    static std::vector<CompoundFilter> pfasFilter = {PFBS};

    struct mzRange
    {
        double min = 0;
        double max = 0;
    };

    // isotope option controls if first 13C isotopologue is also searched for
    std::vector<mzRange> mz_regions(std::vector<CompoundFilter> *filter_vec, double margain, bool isotopes)
    {
#define MASS_13C 1.003354835
        std::vector<mzRange> regions;
        // problem: multiple ion masses (up to 16) per compound
        for (CompoundFilter filter : *filter_vec)
        {
            size_t idx = 0;
            while (*(filter.mz_expected + idx) > 0) // assume zero-terminated array
            {
                assert(idx < 16);
                double mz = *(filter.mz_expected + idx);
                double tol = mz * filter.mz_tolerance_ppm * 10e-6 + margain; // 20 is used as a safety margain, since features are also filtered
                double minMZ = mz - tol;
                double maxMZ = mz + tol;

                printf("%s %zu: min: %f, max: %f | ", filter.compoundName.c_str(), idx, minMZ, maxMZ);

                regions.push_back({minMZ, maxMZ});

                if (isotopes)
                    regions.push_back({minMZ + MASS_13C, maxMZ + MASS_13C});

                idx += 1;
            }
        }
        return regions;
    }
#undef MASS_13C

    std::vector<mzRange> mergeRegions_mz(std::vector<mzRange> *toBeMerged)
    {
        std::vector<mzRange> regions;
        // problem: multiple ion masses (up to 16) per compound
        for (mzRange range : *toBeMerged)
        {
            size_t idx = 0;
            bool novelRange = true;
            // iterate through existing filters and check if two ranges can be merged
            for (size_t i = 0; i < regions.size(); i++)
            {
                if ((regions[i].min > range.min) && (regions[i].min < range.max))
                {
                    novelRange = false;
                    if (regions[i].max < range.max)
                        regions[i].max = range.max;
                }
                if ((regions[i].max > range.min) && (regions[i].max < range.max))
                {
                    novelRange = false;
                    if (regions[i].min > range.min)
                        regions[i].min = range.min;
                }
            }

            if (novelRange)
            {
                regions.push_back(range);
            }
            idx += 1;
        }

        return regions;
    }

    size_t filterCentroids_mz(
        std::vector<CentroidPeak> *centroids,
        std::vector<CompoundFilter> *filter_vec)
    {
        // modifies recieved centroids to only include relevant masses. Returns number of removed centroids

        // make relevant regions for filter

        std::vector<mzRange> regions = mz_regions(filter_vec, 1, true);
        // regions = mergeRegions_mz(&regions);

        // at this point, overlapping mz regions should largely be merged.
        // the filter block is extremely inefficient, but since it only runs
        // once that should be fine performance wise

        size_t removedCount = 0;
        for (size_t i = 1; i < centroids->size(); i++)
        {
            bool filtered = true;
            double mz = centroids->at(i).mz;

            for (auto r : regions)
            {
                if ((r.min < mz) && (mz < r.max))
                {
                    filtered = false;
                    break;
                }
            }

            if (filtered)
            {
                centroids->at(i) = centroids->back();
                centroids->pop_back();
                removedCount += 1;
                assert(centroids->at(i).mz > 1);

                if (i + 1 < centroids->size())
                    i -= 1; // this is necessary since the last element would otherwise be checked forever
            }
        }

        for (size_t i = 0; i < centroids->size(); i++)
        {
            centroids->at(i).ID = i + 1;
        }

        return removedCount;
    }

    size_t filterFeatures_mz_rt(
        std::vector<FeaturePeak> *features,
        std::vector<CompoundFilter> *filter_vec)
    {
        // restrict features to specified mz and rt
        std::vector<mzRange> regions_mz = mz_regions(filter_vec, 0.5, true);
        // regions_mz = mergeRegions_mz(&regions_mz);

        size_t removedCount = 0;
        for (size_t i = 0; i < features->size(); i++)
        {
            bool filtered = true;
            double mz = features->at(i).mz;

            for (auto r : regions_mz)
            {
                if ((r.min < mz) && (mz < r.max))
                {
                    filtered = false;
                    break;
                }
            }

            for (auto filter : *filter_vec)
            {
                double rt_min = filter.RT - filter.RT_tol;
                double rt_max = filter.RT + filter.RT_tol;
                if ((rt_min < features->at(i).retentionTime) && (features->at(i).retentionTime < rt_max))
                {
                    filtered = false;
                    break;
                }
            }

            if (filtered)
            {
                features->at(i) = features->back();
                features->pop_back();
                removedCount += 1;

                if (i + 1 < features->size())
                    i -= 1; // this is necessary since the last element would otherwise be checked forever
            }
        }

        return removedCount;
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

    Polarities polarity_file = unknown_polarity;
    if (polarity_file == Polarities::positive || polarity_file == Polarities::mixed)
    {
        /* this does the processing for positive files - keep it to the function call and handle logging / output through master function arguments */
    }
    if (polarity_file == Polarities::negative || polarity_file == Polarities::mixed)
    {
        // see above
    }

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
    std::vector<std::filesystem::__cxx11::path> tasklist = controlInput(&userArgs.inputPaths);
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

        XML_File inputFile(std::filesystem::canonical(pathSource));

        if (inputFile.defective)
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

        std::vector<bool> spectrum_mode = inputFile.get_spectra_mode(); // get spectrum mode (centroid or profile)

        // CHECK IF CENTROIDED SPECTRA
        size_t num_centroided_spectra = std::count(spectrum_mode.begin(), spectrum_mode.end(), false);
        if (num_centroided_spectra > spectrum_mode.size() / 2) // in profile mode sometimes centroided spectra appear as well @todo is 2 a good idea?
        {
            std::cout << " file centroided, error\n";
            std::cerr << "Error: centroided data is not supported by qAlgorithms for the time being.\n";
            counter++;
            continue;
        }
        if (!userArgs.silent)
        {
            std::cout << " file in profile mode, ok\n";
        }

        auto polarity_file = inputFile.get_polarity_mode(100); // checks first 100 spectra

        // @todo single access function into qAlgorithms

        // @todo find a more elegant solution for polarity switching, this one trips up clang-tidy
        bool oneProcessed = true;
        static bool polarities[2] = {true, false};
        for (bool polarity : polarities)
        {
            filename = pathSource.stem().string();
#pragma region "centroiding"

            // @todo add check if set polarity is correct
            // const std::vector<unsigned int> selectedIndices = inputFile.filter_spectra(true, polarity, false); // @todo MS2 support here!

            const std::vector<unsigned int> selectedIndices = inputFile.filter_spectra_suspects(&pfasFilter, true, polarity, false);

            if (selectedIndices.empty())
            {
                // std::cerr << "Error: No valid spectra exist in the source file " << filename << "\n";
                // @todo better error reporting
                continue;
            }
            std::vector<float> retentionTimes = inputFile.get_spectra_RT(&selectedIndices);
            RT_Converter rt_index = interpolateScanNumbers(&retentionTimes);

            std::vector<CentroidPeak> *centroids = new std::vector<CentroidPeak>;
            *centroids = findCentroids(inputFile, &selectedIndices); // it is guaranteed that only profile mode data is used

            filterCentroids_mz(centroids, &pfasFilter);

            if (centroids->size() == 1) // one empty element is always pushed back.
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
                printCentroids(centroids, &retentionTimes, userArgs.outputPath, filename, userArgs.silent, userArgs.noOverwrite);

                if (userArgs.term == TerminateAfter::centroids)
                {
                    continue;
                }
            }

            assert(!centroids->empty());
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
            meanDQSC /= double(centroidCount);

            auto timeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

            if (!userArgs.silent)
            {
                std::cout << "    produced " << centroidCount - 1 << " centroids from " << totalScans
                          << " spectra in " << timePassed.count() << " s\n";
            }

#pragma region "binning"
            timeStart = std::chrono::high_resolution_clock::now();

            // RT_Converter rt_index = interpolateScanNumbers(&retentionTimes);
            std::vector<EIC> binnedData = performQbinning_old(centroids, &rt_index);

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
                printBins(centroids, &binnedData, &retentionTimes, userArgs.outputPath, filename, userArgs.silent, userArgs.noOverwrite);

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
                assert(EIC.scanNumbers.back() <= retentionTimes.size());
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
            auto features = findFeatures(binnedData, &rt_index);

            filterFeatures_mz_rt(&features, &pfasFilter);

            if (features.size() == 0)
            {
                std::cerr << "Warning: no features were constructed, continuing...\n";
                continue;
            }

            // make sure that every peak contains only one mass trace
            // assert(features.size() <= binnedData.size());
            // @todo this is not intrinsically true, a very crowded spectrum might contain multiple features per Bin

            int peaksWithMassGaps = 0;
            double meanDQSF = 0;
            double meanInterpolations = 0;
            for (size_t i = 0; i < features.size(); i++)
            {
                assert(features[i].scanPeakEnd < retentionTimes.size());
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
            meanDQSF /= double(features.size() - peaksWithMassGaps);
            meanInterpolations /= double(features.size());
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
                // printFeatureCentroids(&features, userArgs.outputPath, filename, &binnedData, // &retentionTimes, // @todo this is incomplete
                //                       userArgs.silent, userArgs.noOverwrite);
            }

            if (userArgs.term == TerminateAfter::features)
            {
                if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
                {
                    printFeatureList(&features, userArgs.outputPath, filename, &binnedData, &retentionTimes,
                                     userArgs.silent, userArgs.noOverwrite);
                }
                continue;
            }

            continue; // @todo ensure feature constrction works correctly before redoing componentisation

#pragma region "Componentisation"
            timeStart = std::chrono::high_resolution_clock::now();

            size_t featuresInComponents = 0; // only used as a count statistic
            const auto components = findComponents(&features, &binnedData, &retentionTimes, minCenArea, &featuresInComponents);

            timeEnd = std::chrono::high_resolution_clock::now();
            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    grouped " << featuresInComponents << " features into " << components.size() << " components in " << timePassed.count() << " s\n";
            }

            if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
            {
                printFeatureList(&features, userArgs.outputPath, filename, &binnedData, &retentionTimes,
                                 userArgs.silent, userArgs.noOverwrite);
            }

            if (userArgs.printComponentRegs)
            {
                printComponentRegressions(&components, userArgs.outputPath, filename,
                                          userArgs.silent, userArgs.noOverwrite);
            }

            if (userArgs.printComponentBins)
            {
                printComponentCentroids(&components, &binnedData, userArgs.outputPath, filename,
                                        userArgs.silent, userArgs.noOverwrite);
            }

            if (userArgs.term == TerminateAfter::components)
            {
                continue;
            }

            if (userArgs.doLogging) // @todo logging should account for early terminate
            {
                logWriter.open(pathLogging, std::ios::app);
                logWriter << filename << ", " << inputFile.number_spectra << ", " << centroidCount << ", "
                          << meanDQSC << ", " << binnedData.size() << ", " << badBinCount << ", " << meanDQSB
                          << ", " << features.size() << ", " << peaksWithMassGaps << ", " << meanInterpolations << ", " << meanDQSF
                          << components.size() << ", " << featuresInComponents << "\n";
                logWriter.close();
            }
        }
        // @todo this function really needs to be shortened
        inputFile.freeLinknodes();
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