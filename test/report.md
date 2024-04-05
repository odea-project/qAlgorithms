# Presenting an implemetation of the qBinning-algorithm developed by Reuschenbach et al. in c++
## Report concluding the analytical practical by Daniel Höhn, supervised by Gerrit Renner

@todo clickable table of contents in markdown

## Table of Contents
* Abbreviations 
* Area of application (NTS)
* qBinning overview @todo section name
	* Bin Criteria
	* Data Quality Score
* Implementation
	* Differences to the Original Implementation in R
	* Module Requirements
	* Data Organisation
	* Core Functions
* Evaluation
* Future Improvements and Additions

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

## qBinning


## Implementation
Terminology: open/closed Bin; maxdist
@todo capitalisation for data objects?

### Differences to the Original Implementation in R

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
It is called after makeOS and requires the bin to be sorted by mz.

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
as closed by a different subsetting function. 


## Evaluation
Performance evaluation was performed using the following CPUs: 
* Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz (referred to as i5)
* 


runtimes on different processors
performance bottlenecks
effectiveness of binning for different datasets

## Future Improvements and Additions
OpenMS uses binary format for raw data, similar approach (save intermediates of workflow in binary to 
improve processing speeds)
more subsetting
concrete validation
implement into proper pipeline by returning closed bins with DQSB - datapoint basis or as EIC objects?
Implement a way to handle MS^2 / MS^n

