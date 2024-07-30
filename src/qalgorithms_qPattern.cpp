#include <../include/qalgorithms_qbin.h>
#include <../include/qalgorithms_qPattern.h>

// group peaks identified from bins by their relation within a scan region

namespace q
{
    namespace qPattern
    {

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