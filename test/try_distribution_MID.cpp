#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <random>
#include <ctime>
#include <span>

double meanDistance(std::vector<double> pointsInBin)
{
    // assumes bin is sorted by mz
    const int binsize = pointsInBin.size();
    std::vector<double> output(binsize);
    double totalSum = std::accumulate(pointsInBin.begin(), pointsInBin.end(), 0.0); // totalSum is the sum of all mz ahead of the current element
    double beforeSum = 0;                                                           // beforeSum is the sum of all mz which had their distance calculated already
    for (int i = 0; i < binsize; i++)
    {
        const double v1 = fmal(pointsInBin[i], -(binsize - i), totalSum); // sum of all distances to mz ahead of the current element
        // (totalSum - pointsInBin[i]->mz * (binsize - i));
        const double v2 = fmal(pointsInBin[i], i, -beforeSum); // sum of all distances to mz past the current element (starts at 0)
        // (pointsInBin[i]->mz * i - beforeSum);
        beforeSum += pointsInBin[i];
        totalSum -= pointsInBin[i];
        output[i] = (v1 + v2) / (binsize - 1); // -1 since the distance of an element to itself is not factored in
    }
    double meanOfMean = std::accumulate(output.begin(), output.end(), 0.0);
    return meanOfMean / binsize;
}

int main()
{

    // size_t binsize = 5;
    std::vector<double> MIDsum(500);
    for (size_t n = 3; n < 500; n++)
    {
        size_t binsize = n;
        int repeats = 10000;
        std::vector<double> MIDs(repeats);
        std::vector<double> sample(binsize);

        std::default_random_engine generator(time(0));
        std::normal_distribution<double> normDist(0.0, 0.1);

        for (int j = 0; j < repeats; j++)
        {
            for (size_t i = 0; i < binsize; i++)
            {
                sample[i] = normDist(generator);
            }
            std::sort(sample.begin(), sample.end());
            for (size_t i = binsize; i > 0; i--)
            {
                sample[i - 1] += abs(sample[0]);
            }

            MIDs[j] = meanDistance(sample);
            // std::cout << MIDs[j] << "\n";
        }
        MIDsum[n] = std::accumulate(MIDs.begin(), MIDs.end(), 0.0);
    }

    // std::vector<double> MIDflat(1000);
    // double increment = 0.0345;

    // for (size_t i = 0; i < 1000; i++)
    // {
    //     std::vector<double> flatDist(i);
    //     for (size_t j = 0; j < i; j++)
    //     {
    //         flatDist[j] = increment * j;
    //     }

    //     MIDflat[i] = meanDistance(flatDist) / (increment * (i + 1)); // 1/3 für alle Werte von increment
    // }

    return 0;
}
// Hypothese: für die Normalverteilung 0,1 geht MID immer gegen 1.128..., unabhängig von Bingröße
// MID einer beliebigen Stichprobe ist immer sigma * 1.128... => unabhängig von µ

// MID einer flachen Verteilung = n * Abstand * 1/3