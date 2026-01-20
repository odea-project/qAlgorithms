// internal
#include "qalgorithms_utils.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_input_output.h"
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

    // struct CompoundFilter
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
    CompoundFilter PFPeA{{262.9760, 218.9856, 0}, 10, 11.92 * 60, 20, "PFPeA", "M1", negative};
    CompoundFilter PFOA{{412.9664, 368.9760, 0}, 10, 17.51 * 60, 20, "PFOA", "M1", negative};
    CompoundFilter PFHxA{{312.9728, 268.9824, 0}, 10, 14.22 * 60, 20, "PFHxA", "M1", negative};
    CompoundFilter PFBS{{298.9429, 0}, 10, 12.36 * 60, 20, "PFBS", "M1", negative};
    CompoundFilter PFBA{{212.9792, 168.9888, 0}, 10, 8.3 * 60, 20, "PFBA", "M1", negative};
    CompoundFilter PFOS{{498.9302, 0}, 10, 19.98 * 60, 20, "PFOS", "M1", negative};

    static std::vector<CompoundFilter> pfasFilter = {PFPeA, PFOA, PFHxA, PFBS, PFBA, PFOS};
    // static std::vector<CompoundFilter> pfasFilter = {PFOS};

    struct mzRange
    {
        double min = 0;
        double max = 0;
    };

    struct RangeFilter
    {
        std::string name;
        double minMZ = 0;
        double maxMZ = 0;
        double minRT = 0;
        double maxRT = 0;
    };

    std::vector<RangeFilter> processFilters(std::vector<CompoundFilter> *filter_vec, bool isotopes)
    {
#define MASS_13C 1.003354835
        std::vector<RangeFilter> ranges;

        for (CompoundFilter filter : *filter_vec)
        {
            size_t idx = 0;

            double rt_min = filter.RT - filter.RT_tol;
            double rt_max = filter.RT + filter.RT_tol;

            while (*(filter.mz_expected + idx) > 0) // assume zero-terminated array
            {
                assert(idx < 16);
                double mz = *(filter.mz_expected + idx);
                double tol = mz * filter.mz_tolerance_ppm * 10e-6 + 0.4; // 20 is used as a safety margain, since features are also filtered
                double minMZ = mz - tol;
                double maxMZ = mz + tol;

                std::string filtername = filter.compoundName + " " + std::to_string(idx);
                ranges.push_back({filtername, minMZ, maxMZ, rt_min, rt_max});

                if (isotopes)
                {
                    filtername = filter.compoundName + " 13C " + std::to_string(idx);
                    ranges.push_back({filtername, minMZ + MASS_13C, maxMZ + MASS_13C, rt_min, rt_max});
                }

                idx += 1;
            }
        }

        return ranges;
#undef MASS_13C
    }

    size_t filterCentroids_mz(
        std::vector<CentroidPeak> *centroids,
        std::vector<RangeFilter> *filter_vec)
    {
        size_t removedCount = 0;

        for (size_t i = 1; i < centroids->size(); i++) // @todo remove dummy centroid
        {
            CentroidPeak *cen = &(centroids->at(i));

            float minMZ_feat = cen->mz - cen->mzUncertainty - 0.25;
            float maxMZ_feat = cen->mz + cen->mzUncertainty + 0.25;

            float minRT_feat = cen->RT - 50; // @todo hard-coded window of 1:40
            float maxRT_feat = cen->RT + 50;

            bool featOK = false;

            for (auto filter : *filter_vec)
            {
                if (filter.minMZ > maxMZ_feat)
                    continue;

                if (filter.maxMZ < minMZ_feat)
                    continue;

                if (filter.minRT > maxRT_feat)
                    continue;

                if (filter.maxRT < minRT_feat)
                    continue;

                featOK = true;
                break;
            }

            if (!featOK)
            { // purge bad features, think about markers instead? @todo
                centroids->at(i).mz = 0;
            }
        }
        return removedCount;
    }

    size_t filterBins_mz_rt(
        std::vector<EIC> *bins,
        std::vector<RangeFilter> *filter_vec)
    {
        size_t removedCount = 0;

        for (size_t i = 0; i < bins->size(); i++)
        {
            EIC *bin = &(bins->at(i));
            size_t binLength = bin->mz.size();

            auto mz = bin->mz.data();
            float *minMZ_bin = minVal(mz, binLength);
            float *maxMZ_bin = maxVal(mz, binLength);

            auto rt = bin->RT.data();
            float *minRT_bin = minVal(rt, binLength);
            float *maxRT_bin = maxVal(rt, binLength);

            bool binOK = false;

            for (auto filter : *filter_vec)
            {
                if (filter.minMZ > *maxMZ_bin)
                    continue;

                if (filter.maxMZ < *minMZ_bin)
                    continue;

                if (filter.minRT > *maxRT_bin)
                    continue;

                if (filter.maxRT < *minRT_bin)
                    continue;

                binOK = true;
                break;
            }

            if (!binOK)
                bin->scanNumbers.clear();
        }
        return removedCount;
    }

    size_t filterFeatures_mz_rt(
        std::vector<FeaturePeak> *features,
        std::vector<RangeFilter> *filter_vec)
    {
        size_t removedCount = 0;

        for (size_t i = 0; i < features->size(); i++)
        {
            FeaturePeak *feat = &(features->at(i));

            float minMZ_feat = feat->mz - feat->mzUncertainty;
            float maxMZ_feat = feat->mz + feat->mzUncertainty;

            float minRT_feat = feat->retentionTime - feat->RT_Uncertainty;
            float maxRT_feat = feat->retentionTime + feat->RT_Uncertainty;

            bool featOK = false;

            for (auto filter : *filter_vec)
            {
                if (filter.minMZ > maxMZ_feat)
                    continue;

                if (filter.maxMZ < minMZ_feat)
                    continue;

                if (filter.minRT > maxRT_feat)
                    continue;

                if (filter.maxRT < minRT_feat)
                    continue;

                featOK = true;
                break;
            }

            if (!featOK)
            { // purge bad features, think about markers instead? @todo
                features->erase(features->begin() + i);
                i -= 1;
            }
        }
        return removedCount;
    }
}

struct Filepath
{
    std::filesystem::path path;
};

int main(int argc, char *argv[])
{
    using namespace qAlgorithms; // considered bad practice from what i see online, but i believe it is acceptable for this program

    volatile bool debug = true;
    if (debug)
    {
        // std::vector<float> intensity = {32, 475, 711, 472, 207, 132, 57, 14};
        // std::vector<float> mz = {205.120056, 205.125031, 205.130005, 205.134979, 205.139954, 205.144928, 205.149902, 205.154877};
        // std::vector<float> intensity = {157.883072, 1325.722046, 2188.603760, 5415.137695, 12294.484375, 16239.560547, 13575.218750, 9618.787109, 7654.178223, 20002.025391, 69383.062500, 147876.296875, 233001.171875, 244286.796875, 162216.375000, 82337.882812, 23717.978516, 5968.742676, 2921.130859, 945.386047};
        std::vector<float> intensity = {1274.10596, 5653.32959, 19341.3398, 51021.3789, 103777.039, 162754.797, 179871.859, 128027.453, 58688.5547, 17326.6309, 3294.46802};
        std::vector<float> mz = {};
        std::vector<unsigned int> df = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        ProfileBlock block = {
            intensity.data(),
            mz.data(),
            0,
            intensity.size()};
        size_t failcount = 0;
        std::vector<RegressionGauss> validRegressions;

        volatile bool repeat = false;
        while (validRegressions.empty() || repeat)
        {
            const size_t length = block.length;
            const size_t maxScale = 8; // @todo not bound to centroid maxscale
            std::vector<float> logIntensity;
            logIntensity.reserve(length);
            validRegressions.clear();
            runningRegression(
                block.intensity,
                &logIntensity,
                &df,
                &validRegressions,
                length,
                maxScale);
            assert(failcount < 1000);
            assert(!validRegressions.empty());

            double predict[8] = {0};
            RegCoeffs test = validRegressions.front().coeffs;

            for (int i = 0; i < 8; i++)
            {
                int x = i - test.x0;
                predict[i] = regExpAt(&test, x);
            }
            double diffs[8] = {0};
            double totallErr = 0;
            for (int i = 0; i < 8; i++)
            {
                diffs[i] = block.intensity[i] - predict[i];
                totallErr += abs(diffs[i]);
            }
        }
    }

    UserInputSettings userArgs = passCliArgs(argc, argv);

    if (!inputsAreSensible(userArgs))
    {
        exit(1);
    }

    // auto filterRanges = processFilters(&pfasFilter, true); // @todo make optional

    // the final task list contains only unique files, sorted by filesize
    std::vector<std::filesystem::path> tasklist = controlInput(&userArgs.inputPaths);
    if (tasklist.size() <= userArgs.skipAhead)
    {
        std::cerr << "Error: skipped more entries than were in taks list (" << tasklist.size() << ").\n";
        exit(1);
    }

    size_t skipAhead = userArgs.skipAhead;
    if (userArgs.skipAhead != 0)
    {
        std::cerr << "Warning: removing the first " << userArgs.skipAhead << " elements from the tasklist.\n";
    }

    FILE *queue = fopen("./processedFiles.csv", "a");
    fprintf(queue, "ID, Name\n");
    for (size_t pathIdx = skipAhead; pathIdx < tasklist.size(); pathIdx++)
    {
        int ID = pathIdx - skipAhead;
        std::string name = tasklist[pathIdx].filename().string();
        fprintf(queue, "%d, %s\n", ID, name.c_str());
    }
    fclose(queue);

    // hacked in logging part, @todo make this better
    FILE *log = fopen("./logdata.csv", "a");
    fprintf(log, "type, regions, peaks, passed, no_apex, invalid_apex, no_df, invalid_apexToEdge, f_test_fail, invalid_quadratic, invalid_area, invalid_height, invalid_chisq\n");
    fclose(log);

    auto absoluteStart = std::chrono::high_resolution_clock::now();

#pragma region file processing
    std::string filename;
    size_t counter = 1;
    size_t errorCount = 0;
    for (size_t pathIdx = skipAhead; pathIdx < tasklist.size(); pathIdx++)
    {
        std::filesystem::path pathSource = tasklist[pathIdx];
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

        if (inputFile.isCentroided) // in profile mode sometimes centroided spectra appear as well @todo is 2 a good idea?
        {
            std::cerr << "Error: centroided data is not supported by qAlgorithms for the time being.\n";
            counter++;
            continue;
        }
        if (!userArgs.silent)
        {
            std::cout << " file in profile mode, ok\n";
        }

        // @todo single access function into qAlgorithms

        // @todo find a more elegant solution for polarity switching, this one trips up clang-tidy
        static bool polarities[2] = {true, false};
        int check_true = 0;
        int check_false = 2;

        switch (inputFile.polarityMode)
        {
        case positive:
            check_false = 1; // loop will terminate after pol == 0
            break;

        case negative:
            check_true = 1; // loop starts at pol == 1
            break;

        case mixed:
            // does nothing since both is default
            break;

        default:
            assert(false);
            break;
        }

        for (int pol = check_true; pol < check_false; pol++)
        {
            bool polarity = polarities[pol];

            filename = pathSource.stem().string();
#pragma region "centroiding"

            // @todo add check if set polarity is correct
            const std::vector<unsigned int> selectedIndices = inputFile.filter_spectra(true, polarity, false); // @todo MS2 support here!

            // const std::vector<unsigned int> selectedIndices = inputFile.filter_spectra_suspects(&pfasFilter, true, polarity, false);

            if (selectedIndices.empty())
            {
                std::cerr << "Error: No valid spectra exist in the source file " << filename << "\n";
                // @todo better error reporting
                continue;
            }

            if (!userArgs.silent)
            {
                printf("    Processing %s peaks\n", polarity ? "positive" : "negative");
            }

            std::vector<float> retentionTimes = inputFile.get_spectra_RT(&selectedIndices);
            RT_Converter rt_index = interpolateScanNumbers(&retentionTimes);

            std::vector<CentroidPeak> *centroids = new std::vector<CentroidPeak>;
            int centroidCount = findCentroids(inputFile, &selectedIndices, centroids); // it is guaranteed that only profile mode data is used

            if (centroidCount == 0)
            {
                fprintf(stderr, "Error: no centroids found despite valid indices");
                continue;
            }

            for (int cenID = 1; cenID < centroidCount; cenID++) // dummy value at idx 0 @todo
            {
                size_t scanNumber = centroids->at(cenID).number_MS1;
                size_t realRT_idx = rt_index.indexOfOriginalInInterpolated[scanNumber];
                centroids->at(cenID).RT = rt_index.groups[realRT_idx].trueRT;
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

            // find lowest intensity among all centroids to use as baseline during componentisation
            float minCenArea = INFINITY;
            for (int cenID = 1; cenID < centroidCount; cenID++)
            {
                float currentInt = centroids->at(cenID).area;
                minCenArea = minCenArea < currentInt ? minCenArea : currentInt;
            }
            size_t totalScans = centroids->back().number_MS1;

            auto timeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

            if (!userArgs.silent)
            {
                printf("    produced %d centroids from %zu spectra in %0.3f s\n",
                       centroidCount, totalScans, timePassed.count());
            }

#pragma region "binning"
            timeStart = std::chrono::high_resolution_clock::now();

            // RT_Converter rt_index = interpolateScanNumbers(&retentionTimes);
            std::vector<EIC> binnedData = performQbinning_old(centroids, &rt_index);

            // filterBins_mz_rt(&binnedData, &filterRanges);

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
                printBins(centroids,
                          &binnedData,
                          // &retentionTimes,
                          userArgs.outputPath,
                          filename,
                          userArgs.silent,
                          userArgs.noOverwrite);

                if (userArgs.term == TerminateAfter::binning)
                {
                    continue;
                }
            }
            delete centroids;

            // continue; // @todo ensure centroiding and binning work as best they can first

#pragma region "feature construction"
            timeStart = std::chrono::high_resolution_clock::now();
            // every subvector of peaks corresponds to the bin ID

            auto features = findFeatures(binnedData, &rt_index);

            // filterFeatures_mz_rt(&features, &filterRanges);

            if (features.size() == 0)
            {
                std::cerr << "Warning: no features were constructed, continuing...\n";
                continue;
            }

            if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
            {
                printFeatureList(&features, userArgs.outputPath, filename, &binnedData, &retentionTimes,
                                 userArgs.silent, userArgs.noOverwrite);
            }

            timeEnd = std::chrono::high_resolution_clock::now();

            if (!userArgs.silent)
            {
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                std::cout << "    constructed " << features.size() << " features in " << timePassed.count() << " s\n";
            }

            continue;

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
        }
        // @todo this function really needs to be shortened
        inputFile.free_linknodes();
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