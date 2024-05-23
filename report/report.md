Presenting an implemetation of the qBinning-algorithm developed by Reuschenbach et al. in c++ <!-- omit in toc -->
-----------------------------------------------------------------------------------------------
Report concluding the analytical practical by Daniel Höhn, supervised by Gerrit Renner <!-- omit in toc -->

- [Abbreviations](#abbreviations)
- [Introduction](#introduction)
	- [Why is binning necessary?](#why-is-binning-necessary)
	- [Generation of EIC in other software](#generation-of-eic-in-other-software)
	- [Advantages of qBinning](#advantages-of-qbinning)
- [Implementation](#implementation)
	- [Algorithm](#algorithm)
	- [Module Requirements](#module-requirements)
	- [Data Organisation](#data-organisation)
	- [Core Functions](#core-functions)
	- [Diagnostic Functions](#diagnostic-functions)
- [Evaluation](#evaluation)
	- [Result Comparison with R and Julia Implementation](#result-comparison-with-r-and-julia-implementation)
	- [Performance Evaluation](#performance-evaluation)
	- [Test Criteria for Bin Correctness](#test-criteria-for-bin-correctness)
- [Discussion](#discussion)
	- [Test Results](#test-results)
	- [Future Improvements and Additions](#future-improvements-and-additions)
- [Conclusion](#conclusion)
- [peak quality or result correcness to justify being made.](#peak-quality-or-result-correcness-to-justify-being-made)
- [Software and Data](#software-and-data)


## Abbreviations
* (HR)MS - (High Resolution) Mass Spectrometry 
* m/z, mz - Mass to Charge Ratio
* RT - Retention Time
* GC - Gas Chromatography
* (HP)LC - (High Performance) Liquid Chromatography
* EIC - Extracted Ion Chromatogram
* OS - Order Space
* DQS(B) - Data Quality Score (of a Bin)
* MID - Mean Inner Distance: The average distance in mz of an element to all other elements in a bin
* MOD - Minimum Outer Distance: The shortest distance in mz to an element not in the bin
* CPU - Central Processing Unit
* ppm - Parts Per Million (10e-6)
* gcc - GNU compiler collection (always referring to version 13.2.0)

## Introduction

The purpose of the presented project is to provide a user-friendly and
programmer-friendly implementation of the qBinning algorithm [@reuschenbachQBinningDataQualityBased2023].
This is realized through thoroughly commented code, included
diagnostic functions which can be rewritten for specific use
cases with minimal effort, flexible data input, and comparatively
high performance. 
With the newly written code, it is possible to introduce the
qBinning algorithm to existing, open-source analysis platforms
like MZmine, PatRoon and StreamFind [@pluskalMZmineModularFramework2010; @helmusPatRoonImprovedNontarget2022; @helmusPatRoonOpenSource2021; @cunhaStreamFindFrameworkWorkflows2024]. 

Idee: Implementieren des Algorithmus in einer größeren Plattform
Fokus auch auf größerer Datensätze; Datenmenge nimmt wahrscheinlich zu
funktionsweise
Feature: Nur bins weitergeben, wenn in Serie gemessen
(@gerrit: pls translate and complete this part)

// What is HRMS? 
Importance of NTS, Datenprozessierung und Analyse entscheidende Größe bei NTS; ergebnisse verschiedeneer
Workflows haben keine Vergleichbarkeit; hoher Einfluss durch den Nutzer
QA unterscheidet nicht zwischen falschen parametern und instrumentation
Es gibt keine spezifische QA für NTS
Neuerung: qualitätsparameter für einzelne Schritte in der Prouessierung
Steps of finding a feature, mention qCentroids / describe pipeline in general

was ist binning

(@gerrit: not yet completed)

### Why is binning necessary?
(@gerrit: not yet completed)

### Generation of EIC in other software
(@gerrit: not yet completed)

### Advantages of qBinning
parameter-free, quality score for feature priorisation, ideally faster (time) @todo measure
(@gerrit: not yet completed)

## Implementation

### Algorithm
From a high-level perspective, the algorithm searches for groups of similar masses
in a given dataset and ensures no group has too much space in the chromatographic
dimension between included points. 
\ 
From this point, a group will be referred to as
a *closed bin* if the algorithm determines that it cannot be subdivided further
and as *open bin* if one or more of the subsetting functions (@todo link to functions)
still has to check the grouping. Bins always refers to a grouping of centroided
MS data with the parameter mz and at least one additional dimension of separation.
Groups are only referred to as bins if they have not yet been handed to the peak 
fitting algorithm, meaning there are still secondary tests requiring bin metadata. (@todo link to bin object)

The user specifies which subsetting methods should be used and, if applicable, 
parameters of these methods. The selection must be made before compiling the program.
The following description only concerns the grouping by mz and scan number of centroids.
It is assumed centroiding has been performed with the qCentroids algorithm [@reuschenbachDevelopmentScoringParameter2022],
meaning all centroids have a specified mass error.

Grouping is performed iteratively over all open bins. Since the algorithm requires a
bin to start, all centroids in the dataset are collected and merged into one bin.
Grouping always involves separating a bin into two or more smaller bins until no
smaller bin can be created. This process is referred to as subsetting (of an open bin).

First, open bins are subset by mz. Grouping in mz utilises the order spaces of
neighbouring masses, which are calculated after sorting points in the bin by mz.
Assuming that members of a correct bin are normally distributed in mz, a test statistic
can be calculated given an alpha and the bin size. This statistic describes the largest
difference between two order spaces that is still within bounds for points belonging to
the same distribution. This distance decreases exponentially with group size, meaning it
is much less likely for a dissimilar mass to belong into a large bin with a high 
density towards the edges of the probability distribution than a small bin with
low coverage of the entire distribution. 
The order spaces are normaliesd to the centroid error, which serves as an
estimate of the standard deviation of the assumed normal distribution This cannot
be empirically determined before binning is complete.
(@gerrit: This is due to our assumption; however, it is important to mention that the 
centroid error is also just an estimate for the standard deviation of the assumed normal distribution.)
This criteria is then checked for the largest order space of every bin. When the
critical distance is surpassed, the two resulting fragments are controlled.
If no order space is greater than the critical distance and there are more than
five points in a fragment, it is turned into a new open bin. The limitation of five
points is imposed since no peak can be fitted to smaller ones.
Once no fragment can be subset further, the resulting bins are subset by scans.

(@gerrit: here, it is not neighboring masses but the neighboring orders of the masses, 
so it is sorted. However, sorting is not mentioned at all. Moreover, this is based on 
the assumption, that a normally distributed variable provides a characteristic order 
space between first and second or last and last-1 masses. This order space is distributed 
similarly to an exponential distribution with a decreasing probability for increasing 
distances. Here, we assume, that a cumulative probability of 99 % can be used to 
estimate a threshold for an acceptable distance.)

For subsetting a bin by scans, it is sorted by scans and then subset at every 
position where the difference between two neighbouring points is greater than six.
The distance results from the assumption that, if a point is at the limit of
detection, it is detected with a probability of 0.5. If a point has not been
detected for six consecutive scans, the probability that the sixth point was 
present, but at the limit of detection, amounts to 1.6%. 
Created fragments are turned into open bins, unless they would have less than
five members. If the bin was not changed during this subsetting step, it 
is considered closed.

Both functions are executed alternating until no open bins exist.

After all bins are closed, the DQS is calculated for all bin members.
It uses the modified shilouette score [@starczewskiModificationSilhouetteIndex2016]
to assess the separation of a bin from the surrounding points. The MID in mz is
calculated per point while the MOD is searched for in all scans that are no
more than six scans away from the centroid. For the calculation, refer
to calcDQS in the Functions region of qalgorithms_qbin.cpp.
The DQS is scaled to the interval [0,1] to allow for weighing in later steps. The DQSB
is the mean DQS of all bin members.
(@gerrit: this should include the corresponding literature.)


### Module Requirements
The module has few requirements, only depending on centroided data with values
for mz and the scan number, as well as a measurement of each centroid's mass error. 
While these measures are generated in the qCentroiding step if used as part of the 
qAlgorithms pipeline, the program also allows the user to specify a 
per-centroid error when reading in a .csv or to assume a static error of x ppm. 


### Data Organisation
Data is represented as DataPoint objects, which are handled in one of two ways:
* one RawData object
* multiple Bin objects in one BinContainer object  

<b> DataPoint objects: </b>
One DataPoint contains mz, RT, intensity, scan number, the centroid error, and an optional 
control value specifying the correct DQS. Data points are never modified during the binning process. 
=> DataPoints are Structs as defined in the c++ 21 standard.
(@gerrit: here, a small tree would be nice for overview like: 
// puts too much emphasis on implementation detail, just read the code at that point
DataPoint
|
|-- mz (mass-to-charge ratio)
|-- RT (retention time)
|-- intensity
|-- scan number
|-- centroid error
|-- control value (optional)
    |
    |-- bin assignment
)

<b> The RawData object: </b>
Centroided data is stored as individual scans, with one vector per scan. Within the scans, 
data points are sorted by mz. Scans are accessed through a vector containing all scans. 
It is possible to account for empty scans. (@gerrit: a tree would be nice.) // see above

<b> Bin objects: </b>
Every Bin stores all data points that were determined to belong to the same EIC. All subsetting 
methods are called on the bin without requiring knowledge of data points outside of the bin.
The main difference between Bins and EICs is that a Bin only operates with pointers to data
points in RawData, while the EIC can be used independently of the RawData it was generated from. 
(@gerrit: so, what is the reason for using the structure of these bins instead of just EICs?-> this should be explained)
=> Additionally, bins keep track of information which can later be used for filtering or weighing
purposes, such as if any scans appear more than once in the data and median values
for scans and masses. These details are not relevant to the peak finding process and
are consequently not passed up the pipeline to minimise memory allocations.
Since Bins contain superfluous methods and variables while additionally depending on the RawData
object for their content, they are not suited for passing forward to the peak fitting
algorithm. Instead, all necessary data is copied, and the binning infrastructure is freed.
A new bin is created by specifying its start and end within an existing bin. As such, the
program starts by creating a bin that contains every data point in the dataset.

<b> The BinContainer object: </b>
The BinContainer supplies wrapper functions to access all bins. The access for subsetting 
is organized so that the user specifies all methods and the order in which they are to be applied. 
While currently only two methods exist, this makes the program easy to
expand for the use of further parameters. 
Bins are organized in two different data structures: Bins that were newly created are added to 
a deque object that contains only Bins which require further subsetting (open Bins). 
The subsetting methods are applied to all bins sequentially, with every iteration following the 
user-specified order. If the last subsetting step does not change the bin, it is considered
closed. For the present program, this means no significant gaps exist in the bin (refer: @todo link).
and removing bins that can no longer be a subset (closed Bins) from the deque. (@gerrit: it should be mentioned, that closing a bin is based on the criteria: no significant gap in the data group is left.)
Once a Bin is closed, it is added to a vector that contains all closed Bins. Once binning is 
complete, the DQSB is calculated for all bin members in all closed bins.


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
Both functions parse data points from a text file and construct a vector that contains the 
individual scans. Within each scan, data points are ordered by m/z in increasing order. These functions
primarily exist for testing purposes since the qBinning module is designed to accept centroided data 
from the previous step in the qAlgorithms pipeline. 
If used to read in data, an error has to be specified for each centroid. If the error is supposed to
be a certain ppm of the mass, a dummy value suffices since this is handled in **makeCumError**.
This function returns a boolean to indicate if it executed successfully. 
```@todo one line of example in code style, same for others```

<b> makeOS: </b>
This function sorts a bin by mz and calculates adjacent elements' differences. It is 
required by subsetMZ.

<b> makeCumError: </b>
This function creates a vector that contains the cumulative error of all data points in a bin.
It is called after makeOS and requires the bin to be sorted by mz. 
If specified, makeCumError overrides the centroid error with an error in ppm of the
centroid m/z. 

<b> subsetMZ : </b>
This function takes a bin after it was sorted by mz and splits it according to the approach 
described in [@reuschenbachQBinningDataQualityBased2023].
It starts by taking the OS of the bin that calls it and searching for the maximum, corresponding
to the largest difference between two neighboring mz values in the sorted bin. If the maximum
is greater than the critical value (@gerrit: based on the above-mentioned assumption), 
the bin is split at this position, and subsetMZ is called for both resulting halves. 
The critical value is calculated for an @todo unicode 
alpha of 0.05 and normalized to the mean centroid error. The mean error is derived from the cumulative
error by dividing the difference between the cumulative error at the start and end of the bin by the 
number of data points in the bin. This is identical to normalising the order space for every point.
(@gerrit: why mean error?->please explain) 
Since a bin created as a subset of another bin shares OS and
the cumulative error with its precursor, both do not need to be calculated more than once.
The function terminates if the bin it is called on has less than five data points.
These points are then added to a vector which can later be used by diagnostic functions. @todo reasoning (@gerrit: what happens to these small bins?)
If a bin cannot be split further, it is added to the container for open bins.

<b> subsetScan: </b>
This function takes a bin, sorts it by the scan number of its datapoints, and splits it if the
difference in scans exceeds a given number. @todo reasoning for maxdist = 6
After splitting the bin, only subsets with more than five elements are returned as bins to
the container for open bins. If a bin is not split, it can be considered complete and is 
closed. Closed bins are added to a separate container.

<b> makeDQSB: </b>
This function generates the element-wise DQS for all bin members. It calls the meanDistance and
calcDQS functions. makeDQSB assumes the input is a closed bin sorted by scans, from which it takes the 
first and last element. The range from the smallest scan minus maxdist
and the largest scan plus maxdist is the range that contains all data points
needed to calculate the DQS. Afterward, the bin is sorted by mz and the mean distances per 
data point are calculated with meanDistance. The second component needed to calculate the DQS of 
an element is the smallest distance to an element not part of the bin (MOD). To find this
distance for all elements with minimal calculation effort, all scans in the range are reduced
to the largest data point with an mz smaller than the minimum mz in the bin and the smallest
data point with an mz larger than the maximum mz in the bin. For every data point in the bin,
the smallest distance to an element of the reduced scans within maxdist is found. Combined
with the MID, calculated by the utility function meanDistance, the DQS is calculated 
for every element and the result is saved to the DQSB vector of the bin.

<b> subsetBins: </b> 
A wrapper function for applying subsetting functions to bins. All possible subsetting functions
are accessed using a switch statement. The user specifies an order in which the functions to be
executed are listed in the order they are to be applied. Since closing a bin is an action 
integrated into the subsetScan function, it must be the last function specified by the user
unless an alternative function with equivalent code exists. This design also allows for use 
cases in which a specific subsetting operation is only necessary for bins that were not marked
as closed by a different subsetting function. subsetBins can take an error (in ppm) as an
optional argument. If none is supplied, the centroid error is used. 

**user-made subset functions**
For a user to specify a subset function, the only hard condition to be fulfilled
is that the function takes the vector of open bins to write to, some criteria of
subsetting and the subset counter as arguments. Return values can be specified, 
but are not expected. If the function serves as a terminator, it also needs to
take the vector of closed bins as argument.
Within the function, isolate some property of the data and use that to define
beginning and end position of new bins within the old. Control for a size greater
five before continuing the function if it is recursive and do not pass subsets
smaller than five to the bin vectors. After the bin cannot be subset by
the chosen criteria further, beginning and end position of the sequence 
relative to the pointsInBin vector of the originating bin must be passed
to the bin constructor as reference.
Within the function, always write open or closed bins using .push_back(//NEWBIN//).
Once the function is confirmed to work correctly, it can be implemented in
the subsetBins function. This requires a new case to be defined for the central
switch statement. Here, first all relevant data that must be calculated outside
of the bin is created. Then, binDeque.front().subset//SUBSETNAME//(...) is called.
After the subset function, add binDeque.pop_froint() to delete the bin that
was just processed.
Lastly, add the name of the new subset method to the SubsetMethods enum
which is defined in the first line of the BinContainer region. 
Now, the new subset method can be specified in the measurementDimensions
vector which is passed as argument to subsetBins().

### Diagnostic Functions
In order to provide a fast way to assess binning quality to a greater 
extent than just the DQS, the functions **makeBinSelection**, **summariseBin**
and **printBinSelection** were defined. 
In the summariseBin function, up to
eight test properties can be hard-coded. These are, for example, checking
if two points in the bin have the same scan number or if the median and mean
of any dimension deviate by more than a defined value. The tests are limited
to eight because their results are stored in a single byte. If conditions
override other tests, such as 0b11111111 being set if a highest-priority
condition applies, more bin properties can be stored. However, at 256 possible
different results, it is most likely too imprecise to work with more 
simultaneous test criteria.
summariseBin also returns all calculated test results and summary information
as a std::tuple. This makes them usable by other, user-written analysis
functions.
The limit to eight values is a design choice to encourage the user to
focus on essential properties and limit the amount of high-overlap bins.
The makeBinSelection function uses the test byte to return a vector
containing all bins for which at least one criterion is true. 
printBinSelection then returns all bins specified by the vector and
writes their members to a .csv file. Optionally, the summary information
can be written in a separate file. 


## Evaluation
As already concluded in the original paper, the binning process results
in a dataset of EICs the vast majority of which possess very good
separation from their environment. While some errors still persist,
it is also important to note that this binning step is only part of
a larger analysis process. As such, even if the intermediate results 
contain false positives, the peak detection step will likely
heavily reduce their impact through filtering for false positive EICs
and through the fitted regression compensating for single centroids 
being assigned to the wrong bin.

### Result Comparison with R and Julia Implementation
All three implementations result in identical bins after completed subsetting.
Bin identity was controlled by matching the result frames by m/z. Since
all points could be matched this way, no false positives or negatives exist.
For a small test dataset, subsetting results were confirmed by hand.

The DQS calculation was not identical between any of the approaches. 
In order to determine a correct result for the DQS, it was calculated
by hand using two reduced datasets. To locate the source of the discrepancy,
intermediate results were also compared. Here, the calculation of the
MID was identical between all approaches. For identical parameters, all
implementations of the function that calculates the DQS obtained identical
results. 

There exist significant differences between the two original implementations, with the 
julia script producing wrong estimates frequently. To account for floating
point errors, a DQS is considered identical if it is identical when rounded
to nine digits. As a consequence, there exist multiple cases where a DQS
is considered identical despite the MID differing by upwards of 30. 
The error is located in the function nearestNeighbourDistanceWindow!(),
where only the scan an element is placed in is considered for finding the 
MOD distances. For both reduced datasets, a total of one distance 
was determined correctly. Due to the small effect even large absolute 
differences in distance have, three DQS total matched the verified result for both 
(to nine decimal digits). When applied to real data, which is bound to have 
smaller MOD distances than the test data, this effect likely leads to better
matching with the correct results. 
// The bug was successfully fixed following its discovery.

When compared with the implementation in R, the DQS is sometimes identical.
For the entire dataset, 79.1 % of the DQS matched when rounded to nine digits.
This result does not change significantly (ßßß 5. Nachkommastelle) if bins that
contain two points from the same scan are excluded (ßßß total). Even when rounded
to three digits, only 93.2% of the DQS match. (Grafik)
This behavior is independent of Bin association, which results in lower 
agreement when only comparing DQS per bin. (Grafik)

A significant advantage over R and Julia implementations is that a 
single executable can be provided. This means the user is not
required to install a complete programming language and scripting
environment on his device or troubleshoot problems caused by
future changes to packages. While automated package installation
and version control could be implemented for both R and Julia,
this is presently not the case. As such, a significant 
improvement in terms of user-friendliness was achieved.




### Performance Evaluation
Performance evaluation was performed using the following CPUs: 
* Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz (referred to as i5)
* AMD Ryzen 3 3250U (referred to as Ryzen)
@todo SSD speeds
Writing the results to disk was not included in the profiling tests,
since this is not the main task of the presented program and was
as such not optimised beyond the point of sufficient efficiency.

All tests were performed with one dataset containing 3.5 million
centroids. Scaling of performance criteria with data set size was
not investigated.

For R, the Rprof function was used to profile code execution.
A total execution time of 299 seconds on the i5 and @todo
seconds on the Ryzen was measured. in both tests, 46% of the execution time 
was spent in the function responsible for binning, 10% on copying
data, and 6% on calculating the MID. The remaining time was spent 
on various r internal data representations, largely table management.
Especially the MID calculation, implemented as fast_mean_distance,
is massively slower than necessary due to creating four additional
tables, each with a lenght of the bin size to the power of two.
The total time spent on reading data from the disk was 1.3 seconds.
Additionally, the R code required up to 2 GB of memory during 
execution. This is roughly four times the size of the dataset
as plain text.

For Julia, the code was run once before timing to not measure time spent
on compiling instead of executing code. Since no data is written to
disk during code execution, the entire code is considered for timing.
The total execution time on the i5 was 15 seconds. @todo more specific
// further testing was not performed due to a fatal error persisting
// in the code, which would likely change performance if fixed.


For the c++ implementation, the majority of time is spent on reading the
data from disk. This is, in part, due to the data structure being constructed
and to a greater degree because the parsing is very inefficient.
No attempt to optimize this part of the program was made since it only
exists as a debugging tool. 
@todo optimised timing
At optimization level 2, the highest possible optimization when compiling
with gcc, ßßß seconds were spent on subsetting in mz, ßßß s on subsetting
in scans, ßßß s on calculating the DQS, and ßßß s on other tasks, mainly
moving data from the container for open bins to the container for closed bins.

Performance is roughly equal to the Julia script, while more detailed and
easier to access information over the entire process is provided. 

### Test Criteria for Bin Correctness
Not all bins conform to a set of "ideal" parameters. These parameters 
are defined such that a bin that adheres to all of them is as close
as possible to the theoretical model of a Peak-forming EIC.
Since all bins exist independently, only parameters that can be
calculated from the bin members are considered.
Criteria are implemented using the summaiseBin function and
as such are only evaluated in terms of being true or false.
Results of the tests are presented here (@todo add link).

**1) Duplicates in Scans**
The theoretical bin contains one element for every scan it extends
over. Since a declared purpose of this algorithm is to detect bins
even when single points are missing from a series, only a scan being
represented twice can be counted as an error. (@gerrit: what are the consequences of this observation?) 
=> If a scan appears twice at a different mass, it is possible that two 
very similar bins were combined into one due to the centroid error
being too large. As such, duplicate scans can serve as an indicator
for separation not being strict enough.

**2) Too Strict Separation**
To detect too strict separation between either two bins or one bin
and one discarded point, the DQSB is used as a measure of distance
and compared with a hypothetical DQSB. For this hypothetical score,
it is assumed that every point has a nearest neighbor with a 
distance of the critical value for the bin size + 1, normed to the mean
centroid error of the bin. The resulting score should always be lower
than the real DQSB. If it is not, the nearest point outside of the 
bin could be added to it without being discarded again due to lowering
the critical value. 
Ideally, the hypothetical score is above 0.5 but lower than the real
DQS on a per-point basis. This means that every point is, on average,
closer to other bin members than another point would have to be
counted as outside of the bin while being distant enough from another
real point so as to not count it towards the bin in a vacuum. 
The worst-case DQS is calculated for the entire bin, meaning with the
average of all MIDs. This stems from the necessity to determine
a per-bin criteria, where a bin could also have been flagged for one
member fulfilling this condition. It was decided against this on grounds
of partial matches necessitating an additional completeness check for the newly formed
bin and further decisions in regards to how such a merge leading to even 
worse scores for the newly included point should be handled.
While this leads to a blind spot in the detection of bad bins,
a complete analysis would reqire the development of a new clustering
algorithm which functions by increasing bin size and merging
similar bins in accordance with the detailed statistical criteria.
(@gerrit: here you should add if this was observed or not? => moved to separate section)

**3+4) Deviation of Median and Mean**
In a perfect normal distribution, the mean and median are identical.
To account for deviations, error thresholds are defined based on
the established process parameters.
Test 3) checks for deviations in the scan dimension, a discrepancy of up to six
is deemed acceptable. This is equal to the largest tolerated gap
during the binning step. 
Test 4) checks for mz deviations, the mean centroid error times two is accepted.
This allows both values to be at the fringe of their respective
extension and still be counted. (@gerrit: explain what you are investigating with this test. and discuss the results from it)
If either condition is not fulfilled, it is possible that the generated
bin encompasses multiple bins and/or noise, leading to a skewed distribution.
This asymmetry is less relevant for small bins with large gaps between
points, but it is questionable whether those represent real peaks to
begin with. Both deviations being present at once in a large bin
is the case in which an incomplete separation is the most likely.

**5) Points Outside the 3-sigma Interval**
This functions as a simple test for normality and too heavy scattering.
At 3 sigma, the likelihood of a point being part of a normally distributed
sample is similar to the 1% error margin used to calculate the critical 
value during binning. Since this estimate does not consider the absoute
deviation of the outliers, it should only be used as a component of 
estimating high scattering in bins.

**6) DQSB below 0.5**
If the DQSB is less than 0.5, points in the bin are on average closer 
to a point outside of the bin than other bin members. Since the DQSB
is calculated using the internal deviation and distances to points 
outside of the bin, either very high internal deviation or very high
noise levels (or both) can be the cause for a very low DQSB.

**7) Correctly Separated Point Within Critical Distance**
This condition depends on test condition 2). It performs the same
operation, but calculates the critical value using the binsize instead
of the binsize plus one. Only bins which do not fulfill condition 2)
are considered. If this condition is positive, there exists a point
with a distance to any other point in the bin below the critical distance.
However, upon adding this point to the bin, the change in critical
value will lead to this distance being too great and the bin being
split again. A bin for which this condition is true could possibly
benefit from slightly increasing the estimated centroid error, but
should not be considered an undesireable grouping.

The eight test slot was not used for this evaluation.

## Discussion

Centroid error is underestimated, leading to more strict binning than ideal
Differences to the Original Implementation in R
Konkrete Beobachtungen / wertende zusammenfassung + addressat, 
kontext existierender literatur ; vergleichbare Ergebnisse?



### Test Results
The tests presented here (@todo add link) were performed on the
Warburg-Dataset after binning. It contains 55910 total bins, 2205
(4%) fulfilled at least one test condition. The points in these
bins make up 28% of the entire dataset, or 41% of all binned
points. Binsize is the most strongly correlating parameter for
a test being positive, with the detected bins averaging around 
265 members and the remaining bins around 47.

**1) Duplicates in Scans**

Notably, there exists no overlap between test 1) and tests 6) and 7), while 
at least one bin for the criteria 2) to 5) contains a duplicate scan.
This translates to bins with bad separation not containing multiple "true"
bins for test 6), so somehow decreasing the critical value in order to
improve the results is unlikely to work.


**2) Too Strict Separation** 
1.3% of all bins fulfilled this criteria. Notably, they were smaller
than average with a majority having a binsize of 5. The average binsize was larger
when other tests were positive, but still ~10% of the normal binsize
for these groups. This resulted in 0.1% of all points being affected.
For @todo bins it was possible to find at least one point in the vector of
discarded points which could be associated with the bin in question.
@todo control for neighbours in binned dataset
There is very little overlap between this condition and others, with over
97% being exclusively "too close" to another point.
Of these, some are two incorrectly separated bins. // ID 39242 + 39343; 16501 + 16510; check if both form a valid bin
// both cases are close together in operation order

**3) Difference Between Median and Mean in Scans**


**6) DQSB below 0.5**
Notable for this test is that it occurs very rarely with only ten cases
total, six of which also fulfill test condition 2). For the other four,
exclusively case 6) applies. Bins have an average size of 10. There is 

**7) Correctly Separated Point Within Critical Distance**
Condition 7) does not occur in connection with any other condition.
This conforms to the idea that these bins are correctly formed.
Bins are around 6 points large on average, making up 0.2% of all bins.
Centroid errors are not extraordinarily low, ranging from 10^-5
to 10^-3. It is unlikely that decreasing them will compensate for the
large difference between the critical values for 5 and 6.
Notable is that this effect occurs primarily at low masses
and at the beginning and end of the chromoatographic dimension (@todo picture).
Since such points are less likely to correspond to peaks, efforts to
change the grouping are not warranted.

Throughout all tests, the presence of "too large" bins; those being
groupings which show traits that could be caused by the bin either 
including a lot of noise or multiple other bins, could be observed.
They generally have a high member count and high deviation in the mz
dimension, the scan dimension less so. @todo 

Different from these are bin which were subset at the wrong position
due to how the algorithm works. @todo finish julia test code
It is generally possible to reassemble these cases, potentially improving


### Future Improvements and Additions
Here listed are changes to the program that would be beneficial, but
could not be implemented as of now. 

**Performance Improvements**
The first subsetting is not multithreaded, since omp macros
only work on for loops with no greater modification. If, before normal
bin subsetting starts, the bin could be divided by some criteria,
there is some additional time which could be saved. An improvement
in this direction would also enable datasets to be binned if they are
too large to fit into RAM. 

The transferred data always contains a control DQS and centroid error,
even if those are not needed. By implementing them as optional parameters,
the total memory allocation that needs to be performed is reduced.

When performing the splitting in the m/z dimension, for small bins the 
greatest order space is often at the border. This leads to many redundant
calculations, especially toward the end of a subsetting step. If it is
possible to implement a check that does not require a lot of processing
time, a medium to large boost in efficiency can be assumed. For reference,
A test dataset with 261 centroids took 98 subsets in m/z total to identify 
one bin with ten members. 

The critical value, before normalisation, is calculated often and 
repeaqtedly for the same binsize. Including it as a table would
likely save some time during the calculations.

**Feature Additions**
While the algorithm works well for LC-MS data, MS^2 datasets can only be 
binned according to the precursor ion. An additional method to subset
closed bins further according to MS^2 or MS^n data could be implemented
with relative ease as a separate step following the existing subset methods.

Besides single-dimension chromatography, separation by Ion Mobility Spectrometry
or two-dimensional chromatography is employed for NTS. Accounting for this
instrumentation during the subsetting step requires additional methods to
be written. Here, the way the DQS is calculated also needs to be revised.

The DQS currently does not discriminate between points according to their
distance in the scan dimension. This problem could be adressed by 
scaling the MOD during DQS calculation by some facor obtained by a gaussian
distribution which is 1 for distances in the same scan and less for every
further scan, until it is set to 0 for every scan past the tolerance of six.
Such a modification has not yet been implemented due to colliding with the 
comparison that is the basis for test condition 2). 

While the use of a set maximum distance between scans of a bin is in relation
to the chance a real signal has of being a false negative, the result of
this reasoning is still a more or less arbitrary value a certain estimate
of the false negative rate can be attached to. Ideally, this distance
could be derived from the dataset itself or the instrumentation used.
Since it is required to be an integer number, this parameter is limited
in the degree, it can be optimized.

Similarly, the critical value is always calculated for an alpha of 0.01.
There is no analytical solution to the parameters of this value, but
it is possible some links to the instrumentation itself can be made.
Further research into this area and a decision table for the right
alpha could help firstly draw the user's attention to this slightly
obfuscated property and secondly - assuming any such links can
be established - increase the comparability of results for similar
samples between labs employing different separation.

It could be demonstrated that during specific steps of the algorithm,
members of a group are erroneously split off. By identifying these
bins and revising their members, ca. 2% of all bins found in the
test dataset could be improved. One possible approach for this is to
re-add all "bad" bins to the dataset of discarded points and redo
the entire binning. This results in an operation which iterates over about
1/3 of the entire dataset again, and does not guarantee that this error
will not occur once more.
An alternative approach is binning is a mass window. Given a more 
elaborate structure of of discarded points, for example organising them
in steps of 0.5 mz, the area needed for binning always spans 1 mz. 
This massively reduces the amount of subsets that need to be performed
per control.

While it already functions as a measure of separation, a secondary
statistic to the DQSB would allow to control the generated values.
This test would, depending on calculation effort, not be included
as part of the standard DQSB function. Instead it serves as a control
of clustering quality unrelated to the test statistic used for binning.
One possible measure for this is the Quadratic Renyi’s Negentropy Separation [@martinsNewClusteringSeparation2015],
which is interesting since it decides if two generated clusters should
be combined. Since it does not depend on the error of a point,
bins with suboptimal separation could be detected even if the grouping
itself is correct.

Isotope ratio priorisation during binning possible?

**Usability Changes**
The program does not feature error handling in any capacity. Additionally,
only one dataset @todo adjust if testing SFC/GC
has been used to confirm error-free operation on two different @todo test on linux
computers, both running Windows 10. When running without an attached debugger,
error management will be impossible for a possible end user. For the qAlgorithms
tool to find widespread adoption, possible error sources need to be identified,
addressed, or effectively communicated to the end user. This task requires 
extensive testing with a broad sample group of potential users.

Contributions to open-source software like this are more likely to occur
if automated tests exist that can verify the validity of results quickly
after modification. Such tests should cover all stages of the program
at which potential errors can occur, such as after reading data,
after forming the initial bin, and completing a subsetting operation,
when adding a bin to the vector of closed bins and after the DQS calculation.
The test dataset for this purpose should contain multiple bins which display 
different behavior, that is bins with good scores, bins with correctly
assigned bad scores and bins with bad scores due to either being too large
and including more than one ion trace (case @todo mention in discussion)
or one bin being split in two (@todo see above). These cases need to be
included so that a potential improvement of the algorithm is not discarded
for not obtaining exactly matching results.

Also to reduce the effort needed for a substantial contribution, the entire
project should compile and run correctly on a Linux distribution. For the
qBinning program, this is the case.


## Conclusion

It could be shown that the qBinning algorithm creates largely good results,
and more importantly provides quality criteria that can be used to improve
the clustering results.  

Furthermore, an error in calculating the DQS was identified in both original
implementations. The alternative approach chosen here avoids incorrect results
and was verified under different edge cases.

Expansion of the program gives access to many optional tests for bin quality,
which enable a sophisticated pre-selection of bins before the peak fitting
algorithm begins work. For example, only those bins which do not contain duplicate scans,
are well-separated from the dataset and do not have points outside of 2 sigma 
(tests 1, 2 and 4 are negative) could be returned by this program.
The tests also grant better insight into the weaknesses of the algorithm.
Through them, a probable cause for incorrect splits in bins could be identified
as an acceptable gap in a small bin which was considered too large while the correct bin
was still part of another, orders of magnitude greater bin.

The program is now flexible and accessible for future expansions. It is now
possible to pass centroided data as an object which will not be modified,
and data can be returned in a reduced form, skipping the reading from and
writing to csv steps. Through other differences in implementation, a massive
improvement in terms of execution time was achieved in comparison to the R code,
while performing additional calculations for bin metadata and test conditions.

An important restriction of the presented test for too strict separation is that it
only detects bins where a majority of points is affected. It is a
reasonable assumption that such cases also exist in plenty at the
ends of larger bins, where they are not detected by the current test.
Developing further tests for similar conditions could present a way
to improve existing large bins by re-binning their edges with smaller,
neighbouring bins and "free" points.

For maximising result correctness, the approach of exclusive subdivision
cannot be used alone. The development of additional steps, however,
should not happen in isolation from the peak finding algorithm.
An approach focused on merging existing bins with "free" points or
other, similar bins presents a promising supplement to subset-based
binning if the formation of very large bins containing more than one
ion trace can be effectively prevented. Such a method would further 
benefit the binning procedure by adressing an issue that persists
in the subsetting-based approach.
The search for an efficient detection and correction of non-ideal
bins should be the main focus when improving and expanding the 
presented program. Such improvements must always be shown to improve
peak quality or result correcness to justify being made.
--
Images of common undesired outputs/elements that need to be cleaned up: 
runtimes on different processors
performance bottlenecks
effectiveness of binning for different datasets
comparison between centroid error and ppm
compare results for different alpha when calculating the critical value

## Software and Data
R + package versions
Julia + package versions
C++ and compiler version
Describe Warburg dataset?
