// internal
#include "qalgorithms_datatypes.h"
#include "qalgorithms_input_output.h"
#include "qalgorithms_qbin.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_read_file.h"

// other includes are avoided here to reduce header duplication
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>

using namespace qAlgorithms;

int main(int argc, char *argv[]) // NOLINTBEGIN(concurrency-mt-unsafe)
{
    UserInputSettings userArgs = passCliArgs(argc, argv);

    if (!inputsAreSensible(userArgs))
    {
        exit(1);
    }

    // the final task list contains only unique files, sorted by filesize
    const std::vector<std::filesystem::path> tasklist = controlInput(&userArgs);

    clock_t absoluteStart = clock();

#pragma region file processing
    std::string filename;
    size_t errorCount = 0;
    for (size_t pathIdx = 0; pathIdx < tasklist.size(); pathIdx++)
    {
        std::filesystem::path pathSource = std::filesystem::canonical(tasklist[pathIdx]);
        clock_t timeStart = clock();
        if (!userArgs.silent)
        {
            const char format[] = "\nreading file %zu of %zu\n" _STR "\n";
            printf(format, pathIdx + 1, tasklist.size(), pathSource.c_str());
        }

#pragma region "centroid to mzml"
        if (userArgs.printCentroidsMZML)
        {
            // This function handles everything from input validation to processing and file saving.
            // We should probably make it a separate program with its own specialised input validation
            centroids_to_mzml(&pathSource);

            exit(1);
        }

#pragma endregion "centroid to mzml"

        XML_File inputFile(pathSource.c_str(), mzML);

        if (inputFile.defective)
        {
            inputFile.free_linknodes();
            fprintf(stderr, "Error: the file is defective.\n");
            if (userArgs.skipError)
            {
                ++errorCount;
                continue;
            }
            exit(101);
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
        filename = pathSource.stem().string();

        // start with positive scans when evaluating mixed mode spectra
        bool polarity_selected = (inputFile.polarityMode == Polarities::positive) ||
                                 (inputFile.polarityMode == Polarities::mixed);

    // because there are only three cases, the logic is handled like this:
    // 1) negative or positive only: set the argument for filter_spectra correctly and proceed. There is no looping.
    // 2) mixed mode: start with positive scans, then set polarity to negative and goto the the next line.
    //    This is a bit messy, but everything else i can think of involves an abused loop construct that
    //    has horrible readability and requires more variables to handle the switching.
    EVALUATE_MIXED_POLARITY:
        // @todo MS2 support here!
        const std::vector<unsigned int> selectedIndices = filter_spectra(&inputFile, true, polarity_selected, false);

        if (selectedIndices.empty())
        {
            fprintf(stderr, "Error: No valid spectra exist in the source file %s\n", filename.c_str());
        }

        if (!userArgs.silent)
        {
            printf("    Processing %s peaks\n", polarity_selected ? "positive" : "negative");
        }
        if (inputFile.polarityMode == Polarities::mixed)
        {
            filename += (polarity_selected ? "_positive" : "_negative");
        }

        if (userArgs.printProfileSection)
        {
            // This is a somewhat crude solution to print a section of the data in profile mode.
            // Really, this should be a separate program within the qAlgorithms project @todo
            printProfileSections(&inputFile,
                                 &userArgs,
                                 &selectedIndices,
                                 filename);
        }

#pragma region "centroiding"
        std::vector<float> retentionTimes;
        get_spectra_RT(&inputFile, &selectedIndices, &retentionTimes);

        std::vector<CentroidPeak> *centroids = new std::vector<CentroidPeak>;
        size_t centroidCount = findCentroids(&inputFile,
                                             &selectedIndices,
                                             centroids);

        assert(centroidCount < UINT32_MAX);
        if (centroidCount == 0)
        {
            fprintf(stderr, "Error: no centroids found despite valid indices");
        }

        if (userArgs.printCentroids)
        {
            printCentroids(centroids, &retentionTimes, userArgs.outputPath, filename, userArgs.silent, userArgs.noOverwrite);
        }

        assert(!centroids->empty());

        // find lowest intensity among all centroids to use as baseline during componentisation
        float minCenArea = INFINITY;
        for (size_t cenID = 1; cenID < centroidCount; cenID++)
        {
            float currentInt = centroids->at(cenID).area;
            minCenArea = minCenArea < currentInt ? minCenArea : currentInt;
        }
        size_t totalScans = centroids->back().number_MS1;

        clock_t timeEnd = clock();
        double timePassed_s = double(timeEnd - timeStart) / CLOCKS_PER_SEC;

        if (!userArgs.silent)
        {
            printf("    produced %zu centroids from %zu spectra in %0.3f s\n",
                   centroidCount, totalScans, timePassed_s);
        }

#pragma region "binning"
        timeStart = clock();

        std::vector<EIC> binnedData = performQbinning(centroids, &retentionTimes);

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
        }
        delete centroids;

#pragma region "feature construction"
        timeStart = clock();

        std::vector<FeaturePeak> features;
        findFeatures(&binnedData, &retentionTimes, &features);

        if (features.size() == 0)
        {
            fprintf(stderr, "Warning: no features were constructed, continuing...\n");
        }

        if (userArgs.printFeatures) // this is here so we can incorporate the component ID into the output
        {
            printFeatureList(&features,
                             userArgs.outputPath,
                             filename,
                             &binnedData,
                             userArgs.silent,
                             userArgs.noOverwrite);
        }

        timeEnd = clock();

        if (!userArgs.silent)
        {
            timePassed_s = double(timeEnd - timeStart) / CLOCKS_PER_SEC;
            printf("    constructed %zu features in %f s\n", features.size(), timePassed_s);
        }

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
        }

#endif
        if (inputFile.polarityMode == Polarities::mixed)
        {
            assert(polarity_selected);
            polarity_selected = false;
            goto EVALUATE_MIXED_POLARITY;
        }

        // @todo this function really needs to be shortened
        inputFile.free_linknodes();
    }

#pragma region "Logging and similar" // @todo add an option for custom logfile names
    if (!userArgs.silent)
    {
        double timePassed_s = double(clock() - absoluteStart) / CLOCKS_PER_SEC;
        printf("Completed data processing on %zu files in %f s.\n", tasklist.size(), timePassed_s);
        if (errorCount > 0)
        {
            printf("Skipped %zu files which could not be processed.\n", errorCount);
        }
        printf("\n");
    }
    return 0;
}
// NOLINTEND(concurrency-mt-unsafe)