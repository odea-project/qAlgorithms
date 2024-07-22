#include <../include/qalgorithms_qbin.h>
#include <../include/qalgorithms_qPattern.h>

// group peaks identified from bins by their relation within a scan region

namespace q
{
    namespace qPattern
    {
        PeakConnector::PeakConnector(int *originPoint, int *endPoint) {

        };

    }
}

// find direct matches first, then add all possible one-steps
// as hypothetical points to a separate dataset

/*
starting at the best DQSB, iterate over all other entries,
starting with the lowest mass. Work with two sorted vectors
each containing pointers to the peak objects.

Take the mass error of the peaks as absolute tolerance

*/