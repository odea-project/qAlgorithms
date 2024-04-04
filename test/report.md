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

## qBinning


## Implementation
Terminology: open/closed Bin
@todo capitalisation for data objects?

### Differences to the Original Implementation in R

### Module Requirements

### Data Organisation
Data is represented as DataPoint objects, which are handled in one of two ways:
* one RawData object
* multiple Bin objects

### - DataPoint objects
One DataPoint contains mz, RT, intensity, scan number, the centroid error and an optional 
control value which assigns it to a bin. Data points are never modified during the binning process.

### - The RawData object
Centroided data is stored as individual scans. Within the scans, data points are sorted by mz.
Scans are accessed through a vector containing all scans.

### - Bin objects
Every Bin stores all data points which were determined to belong to the same EIC

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
* BinContainer: assignDQSB
* BinContainer: PrintAllBins
* BinContainer: PrintBinSummary
* meanDistance
* calcDQS


### - readcsv, readtxt
Both functions parse data points from a text file and construct a vector which contains the 
individual scans. Within each scan, datapoints are ordered by m/z in increasing order. These functions
primarily exist for testing purposes, since the qBinning module is designed to accept centroided data 
from the previous step in the qAlgorithms pipeline. However, when the user specifies an error (in ppm), 
this is used in place of the centroid error for all following operations.
```@todo one line of example in code style, same for others```

### - makeOS
This function sorts a bin by mz and calculates the difference between adjacent elements. It is 
required by subsetMZ.

### - makeCumError
This function creates a vector which contains the cumulative error of all datapoints in a bin.
It is called after makeOS and requires the bin to be sorted by mz.

### - subsetMZ 
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

### - subsetScan
This function takes a bin, sorts it by the scan number of its datapoints and splits it if the
difference in scans exceeds a given number. @todo reasoning for maxdist = 7
After splitting the bin, only subsets with more than five elements are returned as bins to
the container for open bins. If a bin was not split, it can be considered complete and is 
closed. Closed bins are added to a separate container.


## Evaluation
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

