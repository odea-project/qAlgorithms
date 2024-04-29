# Presenting an implemetation of the qBinning-algorithm developed by Reuschenbach et al. in c++ <!-- omit in toc -->
## Report concluding the analytical practical by Daniel Höhn, supervised by Gerrit Renner <!-- omit in toc -->

- [Abbreviations](#abbreviations)
- [general NTS concepts](#general-nts-concepts)
- [qBinning @todo better name](#qbinning-todo-better-name)
	- [why is binning necessary?](#why-is-binning-necessary)
	- [generation of EIC in other software](#generation-of-eic-in-other-software)
	- [advantages of qBinning](#advantages-of-qbinning)
- [Implementation](#implementation)
	- [Differences to the Original Implementation in R](#differences-to-the-original-implementation-in-r)
	- [Module Requirements](#module-requirements)
	- [Data Organisation](#data-organisation)
	- [Core Functions](#core-functions)
- [Evaluation](#evaluation)
	- [Result Comparison with R and julia implementation](#result-comparison-with-r-and-julia-implementation)
	- [Performance Evaluation](#performance-evaluation)
	- [Effect of Different Error Thresholds (ppm)](#effect-of-different-error-thresholds-ppm)
- [Future Improvements and Additions](#future-improvements-and-additions)


## Abbreviations
* (HR)MS - (High Resolution) Mass Spectrometry 
* m/z, mz - Mass to Charge Ratio
* RT - Retention Time
* GC - Gas Chromatography
* (HP)LC - (High Performance) Liquid Chromatography
* EIC - Extracted Ion Chromatogram
* OS - Order Space
* DQS(B) - Data Quality Score (of a Bin)
* MIN - Mean Inner Distance: The average distance in mz of an element to all other elements in a bin
* MOD - Minimum Outer Distance: The shortest distance in mz to an element not in the bin
* CPU - Central Processing Unit
* ppm - Parts Per Million (10e-6)

## general NTS concepts
necessary?

## qBinning @todo better name
### why is binning necessary?

### generation of EIC in other software

### advantages of qBinning
parameter-free, quality score for feature priorisation, ideally faster (time) @todo measure

## Implementation
Terminology: open/closed Bin; maxdist
A bin is a data construct with associated data points and defined operations, an EIC is just a set of data points.
@todo capitalisation for data objects?

### Differences to the Original Implementation in R
necessary?
R: 1-2 GB (factor 2 to 4 of in-place operation) of memory needed for Warburg, 
30% CPU use on Ryzen + 10% from RStudio, very long runtime (640 s)
writing to csv is significantly slower in R -> probably main timesink
@todo Why do centroids not in bins get a centroid error?

The most significant difference to the original implementation is that 
while previously a dataframe object was used to handle binning by
changing the order of data points, now a bin 

### Module Requirements
The modlue has few requirements, only requiring centroided data and a measurement of the 
error each centroid @todo rename data point to centroid?
has. The error is supplied with the centroid by the qCentroids module. 

### Data Organisation
Data is represented as DataPoint objects, which are handled in one of two ways:
* one RawData object
* multiple Bin objects in one BinContainer object  

<b> DataPoint objects: </b>
One DataPoint contains mz, RT, intensity, scan number, the centroid error and an optional 
control value which assigns it to a bin. Data points are never modified during the binning process.

<b> The RawData object: </b>
Centroided data is stored as individual scans. Within the scans, data points are sorted by mz.
Scans are accessed through a vector containing all scans.

<b> Bin objects: </b>
Every Bin stores all data points which were determined to belong to the same EIC. All subsetting 
methods are called on the bin without requiring knowledge of data points outside of the bin.
The main difference between Bins and EICs is that a Bin only operates with pointers to data
points in RawData while the EIC can be used independently of the RawData it was generated from.
Creating a bin requires all of its members to be aligned sequentially in an existing array.

<b> The BinContainer object: </b>
The BinContainer supplies wrapper functions to access all bins. The access for subsetting is 
organised such that all methods which should be applied and the order they are to be applied in
is specified by the user. While currently only two methods exist, this makes the program easy to
expand for use of further parameters. 
Bins are organised in two different data structures: Bins which were newly created are added to 
a deque object, which contains only Bins that require further subsetting (open Bins). 
The subsetting methods are applied to all bins sequentially, with every iteration following the 
user-specified order and removing bins which can no longer be subset (closed Bins) from the deque.
Once a Bin is closed, it is added to a vector which contains all closed Bins. Once binning is 
complete, the DQSB is calculated for all bin members in all bins.

### Core Functions
The following functions, not including constructors and other utility functions, are included 
as part of the qBinning module:
@todo clickable function names
* RawData: readcsv
* RawData: readtxt
* Bin: makeOS
* Bin: makeCumError
* Bin: subsetMZ
* Bin: subsetScan
* Bin: makeDQSB
* BinContainer: subsetBins


<b> readcsv, readtxt: </b>
Both functions parse data points from a text file and construct a vector which contains the 
individual scans. Within each scan, datapoints are ordered by m/z in increasing order. These functions
primarily exist for testing purposes, since the qBinning module is designed to accept centroided data 
from the previous step in the qAlgorithms pipeline. However, when the user specifies an error (in ppm), 
this is used in place of the centroid error for all following operations.
```@todo one line of example in code style, same for others```

<b> makeOS: </b>
This function sorts a bin by mz and calculates the difference between adjacent elements. It is 
required by subsetMZ.

<b> makeCumError: </b>
This function creates a vector which contains the cumulative error of all datapoints in a bin.
It is called after makeOS and requires the bin to be sorted by mz. In case data without a 
centroid error is used, makeCumError can take an arbitrary error in ppm. 

<b> subsetMZ : </b>
This function takes a bin after it was sorted by mz and splits it according to approach 
described in @todo reference
It starts by taking the OS of the bin that calls it and searching for the maximum, which corresponds
to the largest difference between two neighbouring mz values in the sorted bin. If the maximum
is greater than the critical value, the bin is split at this position and subsetMZ called for both
resulting halves. 
The critical value is calculated for an @todo unicode 
alpha of 0.05 and normalised to the mean centroid error. The mean error is derived from the cumulative
error by dividing the difference between the cumulative error at start and end of the bin by the 
number of data points in the bin. Since a bin created as a subset of another bin shares OS and
the cumulative error with its precursor, both do not need to be calculated more than once.
The function terminates if the bin it is called on has less than five data points. @todo reasoning
If a bin cannot be split further, it is added to the container for open bins.

<b> subsetScan: </b>
This function takes a bin, sorts it by the scan number of its datapoints and splits it if the
difference in scans exceeds a given number. @todo reasoning for maxdist = 7
After splitting the bin, only subsets with more than five elements are returned as bins to
the container for open bins. If a bin was not split, it can be considered complete and is 
closed. Closed bins are added to a separate container.

<b> makeDQSB: </b>
This function generates the element-wise DQS for all bin members. It calls meanDistance and
calcDQS. makeDQSB assumes the input is a closed bin sorted by scans, from which it takes the 
first and last element. The range from the smallest scan minus maxdist
and the largest scan plus maxdist are the range which contains all data points
needed to calculate the DQS. Afterwards, the bin is sorted by mz and the mean distances per 
data point calculated with meanDistance. The second component needed to calculate the DQS of 
an element is the smallest distance to an element not part of the bin (MOD). To find this
distance for all elements with minimal calculation effort, all scans in the range are reduced
to the largest data point with an mz smaller than the minimum mz in the bin and the smallest
data point with an mz larger than the maximum mz in the bin. For every data point in the bin,
the smallest distance to an element of the reduced scans within maxdist is found. Combined
with the MID, which is calculated by the utility functions meanDistance, the DQS is calculated 
for every element and the result saved to the DQSB vector of the bin.

<b> subsetBins: </b> 
A wrapper function for applying subsetting functions to bins. All possible subsetting functions
are accessed using a switch statement. The user specifies an order in which the functions to be
exrcuted are listed in the order they are to be applied. Since closing a bin is an action 
integrated into the subsetScan function, it must be the last function specified by the user
unless an alternative function with equivalent code exists. This design also allows for use 
cases in which a specific subsetting operation is only necessary for bins which were not marked
as closed by a different subsetting function. subsetBins can take an error (in ppm) as an
optional argument. If none is supplied, the centroid error is used.


## Evaluation

### Result Comparison with R and julia implementation
All three implementations result in identical bins after completed subsetting.
Bin identity was controlled by matching the result frames by m/z. Since
all points could be matched this way, no false positives or negatives exist.
For a small test dataset, subsetting results were confirmed by hand.

The DQS calculation was not identical between any of the approaches. 
In order to determine a correct result for the DQS, it was calculated
by hand using two reduced datasets. To locate the source of the discrepancy,
intermediate results were also compared. Here, the calculation of the
MID was identical between all approaches. For identical parameters, all
implementations of the function which calculates the DQS obtained identical
results. 

There exist significant differences between the two original implementations, with the 
julia script producing wrong estimates frequently. To account for floating
point errors, a DQS is considered identical if it is identical when rounded
to nine digits. 
The error is located in the method used for finding the data point not in 
the bin which is most similar to the data point for which the DQS is to be 
determined. More precisely, the function nearestNeighbourDistanceWindow!
gives wrong results for a tolerance window of greater than zero. With the
window parameter set to zero, only points in the same scan are considered 
when calculating the DQS. It is also possible that some error is present in the
idxDict struct, but since it is also used for binning errors are less likely here.
Both possible errors result in the definite error of wrong values for NND 
(Nearest Neighbour Distance). For both reduced datasets, a total of one distance 
was determined correctly. Due to the small effect even large absolute 
differences in distance have, three DQS total matched the verified result for both 
(to nine decimal digits). When applied to real data, which is bound to have 
smaller MOD distances than the test data. This effect likely leads to better
matching with the correct results. distance window bezog sich nur auf eigenens bin - 
The bug was successfully fixed by ßßß

When comparing with the implementation in R, the DQS is sometimes identical.
For the entire dataset, 79.1 % of the DQS matched when rounded to nine digits.
This result does not change significantly (ßßß 5. Nachkommastelle) if bins which
contain two points from the same scan are excluded (ßßß total). Even when rounded
to three digits, only 93.2% of the DQS match. (Grafik)





### Performance Evaluation
Performance evaluation was performed using the following CPUs: 
* Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz (referred to as i5)
* AMD Ryzen 3 3250U (referred to as Ryzen)

@todo all performance tests were conducted with the compiled program and 
optimisation level 2 (gcc)

<b> Issues after binning: </b>
Images of common undesired outputs / elements that need to be cleaned up: 
Centroid error too small, points were not binned
	-> negative effect on DQS
Very small bins from noise (shoud get removed by peak fitting)

runtimes on different processors
performance bottlenecks
effectiveness of binning for different datasets
comparison between centroid error and ppm
compare results for different alpha when calculating the critical value
Less cases where DQS matches, identify cause for this



@todo runtime comparison

### Effect of Different Error Thresholds (ppm)

## Future Improvements and Additions
OpenMS uses binary format for raw data, similar approach (save intermediates of workflow in binary to 
improve processing speeds)
more subsetting
concrete validation
implement into proper pipeline by returning closed bins with DQSB - datapoint basis or as EIC objects?
Implement a way to handle MS^2 / MS^n
automatic tests to verify program after user modifies it
Implement error handling, especially for cases where nonsense data can be detected - ideally implemented 
with qCentroids for data. Error handling for new subsetting: control that bins are valid before DQS 
calculation (sorted by scans)
How certain is scan number as a good parameter?
Add compile-time toggle for creating list of non-binned objects using macros
Cache optimisation
include an example for creating a new subsetting method