// internal
#include "qalgorithms_input_output.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_read_file.h"

// other includes are avoided here to reduce header duplication
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <time.h>

int main(int argc, char *argv[])
{
    using namespace qAlgorithms; // considered bad practice from what i see online, but i believe it is acceptable for this program

    volatile bool debug = false;
    if (debug)
    {
        std::vector<float> intensity = {16649.7441, 34118.1367, 46703.2617, 41025.5352, 53125.8086, 140134.125, 402735.812, 689393.625, 907356.938, 728596.75, 432838.844, 156655.438, 43841.4922, 11888.1475, 3812.54126};
        // std::vector<float> intensity = {32, 475, 711, 472, 207, 132, 57, 14};
        // std::vector<float> intensity = {157.883072, 1325.722046, 2188.603760, 5415.137695, 12294.484375, 16239.560547, 13575.218750, 9618.787109, 7654.178223, 20002.025391, 69383.062500, 147876.296875, 233001.171875, 244286.796875, 162216.375000, 82337.882812, 23717.978516, 5968.742676, 2921.130859, 945.386047};
        // std::vector<float> intensity = {1274.10596, 5653.32959, 19341.3398, 51021.3789, 103777.039, 162754.797, 179871.859, 128027.453, 58688.5547, 17326.6309, 3294.46802};
        size_t failcount = 0;
        std::vector<RegressionGauss> validRegressions;

        volatile bool repeat = false;
        while (debug && (validRegressions.empty() || repeat))
        {
            const size_t length = intensity.size();
            const size_t maxscale = 8; // @todo not bound to centroid maxscale
            std::vector<float> logIntensity;
            logIntensity.reserve(length);
            validRegressions.clear();
            runningRegression(
                intensity.data(),
                &logIntensity,
                nullptr,
                length,
                maxscale,
                &validRegressions);

            failcount += 1;
            assert(failcount != 10); // manually turn this into an endless loop by setting failcount > 10
        }
    }

    UserInputSettings userArgs = passCliArgs(argc, argv);

    if (!inputsAreSensible(userArgs))
    {
        exit(1);
    }

    /// @todo add some form of m/z + RT filtering here (although m/z is probably the more relevant one). This should also include MS2

    // the final task list contains only unique files, sorted by filesize
    std::vector<std::filesystem::path> tasklist = controlInput(&userArgs.inputPaths);
    if (tasklist.size() <= userArgs.skipAhead)
    {
        fprintf(stderr, "Error: skipped more entries than were in taks list (%zu).\n", tasklist.size());
        exit(1);
    }

    size_t skipAhead = userArgs.skipAhead;
    if (userArgs.skipAhead != 0)
    {
        fprintf(stderr, "Warning: removing the first %zu elements from the tasklist.\n", userArgs.skipAhead);
    }

    clock_t absoluteStart = clock();

#pragma region file processing
    std::string filename;
    size_t errorCount = 0;
    for (size_t pathIdx = skipAhead; pathIdx < tasklist.size(); pathIdx++)
    {
        std::filesystem::path pathSource = std::filesystem::canonical(tasklist[pathIdx]);
        clock_t timeStart = clock();
        if (!userArgs.silent)
        {
#ifdef _WIN32
            const char format[] = "\nreading file %zu of %zu\n%ls\n";
#else
            const char format[] = "\nreading file %zu of %zu\n%s\n";
#endif
            printf(format, pathIdx + 1, tasklist.size(), pathSource.c_str());
        }

        assert(pathSource.extension() == ".mzML"); // @todo support other filetypes
        XML_File inputFile(pathSource.c_str(), mzML);

        if (inputFile.defective)
        {
            fprintf(stderr, "Error: the file is defective.\n");
            if (userArgs.skipError)
            {
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
            fprintf(stderr, "Error: centroided data is not supported by qAlgorithms for the time being.\n");
            continue;
        }
        if (!userArgs.silent)
        {
            printf(" file in profile mode, ok\n");
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

            if (selectedIndices.empty())
            {
                fprintf(stderr, "Error: No valid spectra exist in the source file %s\n", filename.c_str());
                // @todo better error reporting
                continue;
            }

            if (!userArgs.silent)
            {
                printf("    Processing %s peaks\n", polarity ? "positive" : "negative");
            }
            std::vector<float> retentionTimes;
            inputFile.get_spectra_RT(&selectedIndices, &retentionTimes);

            std::vector<CentroidPeak> *centroids = new std::vector<CentroidPeak>;
            int centroidCount = findCentroids(inputFile,
                                              &selectedIndices,
                                              &retentionTimes,
                                              centroids); // it is guaranteed that only profile mode data is used

            if (centroidCount == 0)
            {
                fprintf(stderr, "Error: no centroids found despite valid indices");
                continue;
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
            for (size_t cenID = 1; cenID < (size_t)centroidCount; cenID++)
            {
                float currentInt = centroids->at(cenID).area;
                minCenArea = minCenArea < currentInt ? minCenArea : currentInt;
            }
            size_t totalScans = centroids->back().number_MS1;

            clock_t timeEnd = clock();
            double timePassed_s = double(timeEnd - timeStart) / CLOCKS_PER_SEC;

            if (!userArgs.silent)
            {
                printf("    produced %d centroids from %zu spectra in %0.3f s\n",
                       centroidCount, totalScans, timePassed_s);
            }

#pragma region "binning"
            timeStart = clock();

            std::vector<EIC> binnedData = performQbinning_old(centroids);

            timeEnd = clock();

            if (binnedData.size() == 0)
            {
                fprintf(stderr, "Error: no bins could be constructed from the data.\n");
                if (!userArgs.skipError)
                {
                    exit(1);
                }
                else
                {
                    ++errorCount;
                    continue;
                }
            }

            if (!userArgs.silent)
            {
                timePassed_s = double(timeEnd - timeStart) / CLOCKS_PER_SEC;
                printf("    assembled %zu bins in %f s\n", binnedData.size(), timePassed_s);
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

#pragma region "feature construction"
            timeStart = clock();

            std::vector<FeaturePeak> features;
            findFeatures(&binnedData, &retentionTimes, &features);

            if (features.size() == 0)
            {
                fprintf(stderr, "Warning: no features were constructed, continuing...\n");
                continue;
            }

            if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
            {
                printFeatureList(&features, userArgs.outputPath, filename, &binnedData, &retentionTimes,
                                 userArgs.silent, userArgs.noOverwrite);
            }

            timeEnd = clock();

            if (!userArgs.silent)
            {
                timePassed_s = double(timeEnd - timeStart) / CLOCKS_PER_SEC;
                printf("    constructed %zu features in %f s\n", features.size(), timePassed_s);
            }

            continue;
#if 0
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

            if (userArgs.term == TerminateAfter::components)
            {
                continue;
            }
#endif
        }
        // @todo this function really needs to be shortened
        inputFile.free_linknodes();
    }

#pragma region "Logging and similar" // @todo add an option for custom logfile names
    if (!userArgs.silent)
    {
        double timePassed_s = double(clock() - absoluteStart) / CLOCKS_PER_SEC;
        printf("Completed data processing on %zu files in %f s.\n", tasklist.size() - userArgs.skipAhead, timePassed_s);
        if (errorCount > 0)
        {
            printf("Skipped %zu files which could not be processed.\n", errorCount);
        }
        printf("\n");
    }
    return 0;
}