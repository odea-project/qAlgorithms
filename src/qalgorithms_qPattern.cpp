// #include <../include/qalgorithms_qbin.h>
#include <../include/qalgorithms_qPattern.h>

#include <string>
#include <vector>

// group peaks identified from bins by their relation within a scan region

namespace q
{
    namespace qPattern
    {
        Peaklist readcsv(std::string user_file, int d_mz, int d_mzError, int d_scanNo, int d_intensity, int d_DQScentroid)
        { // @todo stop segfaults when reading empty lines; use buffers for speedup
            int lengthAllPoints = 0;
            rawData->allDatapoints.push_back(std::vector<qCentroid>(0)); // first element empty since scans are 1-indexed
            std::ifstream file(user_file);
            if (!file.is_open())
            {
                std::cerr << "the file could not be opened\n";

                return false;
            }

            if (!file.good())
            {
                std::cerr << "something is wrong with the input file\n";
                return false;
            }
            std::string line;
            std::string dummy;
            std::getline(file, dummy); // do not read first row @todo check if first row starts with a number; parralelise?
            while (std::getline(file, line))
            {
                std::istringstream ss(line);
                std::string cell;
                std::vector<double> row;
                while (std::getline(ss, cell, ','))
                {
                    row.push_back(std::stod(cell));
                }
                const int i_scanNo = (int)row[d_scanNo];
                if (size_t(i_scanNo) > rawData->allDatapoints.size() - 1)
                {
                    for (size_t i = rawData->allDatapoints.size() - 1; i < size_t(i_scanNo); i++)
                    {
                        rawData->allDatapoints.push_back(std::vector<qCentroid>(0));
                    }
                }
                qCentroid F = qCentroid{row[d_mz], row[d_mzError], -1, i_scanNo, row[d_intensity],
                                        row[d_DQScentroid]}; // @todo add rt reading back in

                ++lengthAllPoints;
                rawData->allDatapoints[i_scanNo].push_back(F); // every subvector in allDatapoints is one complete scan - does not require a sorted input file!
            }
            for (size_t i = 1; i < rawData->allDatapoints.size(); i++) // make sure data conforms to expectations
            {
                std::sort(rawData->allDatapoints[i].begin(), rawData->allDatapoints[i].end(), [](const qCentroid lhs, const qCentroid rhs)
                          { return lhs.mz < rhs.mz; });
            }
            std::cout << "Read " << lengthAllPoints << " datapoints in " << rawData->allDatapoints.size() - 1 << " scans\n";
            return true;
            // CentroidedData is always a vector of vectors where the first index is the scan number (starting at 1) and every scsn is sorted by mz
        }

    }
}

// find direct matches first, then add all possible one-steps
// as hypothetical points to a separate dataset

/*
starting at the best DQSB, iterate over all other entries,
starting with the lowest mass. Work with two sorted vectors
each containing pointers to the peak objects.

Construct a chain of carbon isotopes first. Only the root
of such a carbon chain can be the start of an adduct network.
During this step, chains cannot branch.
Calculate the likelihood of coelution from the centroids that
make up both peaks and store this data in the connector
Priority of adduct assignment is given to points high up in
a carbon isotope structure

Take the mass error of the peaks as absolute tolerance

*/