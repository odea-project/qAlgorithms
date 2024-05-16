Presenting an implemetation of the qBinning-algorithm developed by Reuschenbach et al. in c++ <!-- omit in toc -->
-----------------------------------------------------------------------------------------------
Report concluding the analytical practical by Daniel Höhn, supervised by Gerrit Renner <!-- omit in toc -->

- [Abbreviations](#abbreviations)
- [Introduction](#introduction)
- [qBinning @todo better name](#qbinning-todo-better-name)
	- [why is binning necessary?](#why-is-binning-necessary)
	- [generation of EIC in other software](#generation-of-eic-in-other-software)
	- [advantages of qBinning](#advantages-of-qbinning)
- [Implementation](#implementation)
	- [Differences to the Original Implementation in R -\> discussion / conclusion; Konkrete Beobachtungen / wertende zusammenfassung + addressat, kontext existierender literatur ; vergleichbare Ergebnisse?](#differences-to-the-original-implementation-in-r---discussion--conclusion-konkrete-beobachtungen--wertende-zusammenfassung--addressat-kontext-existierender-literatur--vergleichbare-ergebnisse)
	- [Module Requirements](#module-requirements)
	- [Data Organisation](#data-organisation)
	- [Core Functions](#core-functions)
	- [Diagnostic Functions](#diagnostic-functions)
- [Evaluation](#evaluation)
	- [Result Comparison with R and julia implementation](#result-comparison-with-r-and-julia-implementation)
	- [Performance Evaluation](#performance-evaluation)
- [Discussion](#discussion)
	- [Error Criteria](#error-criteria)
	- [Future Improvements and Additions](#future-improvements-and-additions)
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
// What is HRMS? 
Importance of NTS, Datenprozessierung und Analyse entscheidende Größe bei NTS; ergebnisse verschiedeneer
Workflows haben keine Vergleichbarkeit; hoher Einfluss durch den Nutzer
QA unterscheidet nicht zwischen flaschen parametern und instrumentation
Es gibt keine spezifische QA für NTS
Neuerung: qualitätsparameter für einzelne Schritte in der Prouessierung
Steps of finding a feature, mention qCentroids / describe pipeline in general

was ist binning

--
specific goal of this project
The purpose of the presented project is to provide a user-friendly and
programmer-friendly implementation of the qBinning algorithm [@reuschenbachQBinningDataQualityBased2023].
This is realised through thoroughly commented code, included
diagnostic functions which can be rewritten for specific use
cases with minimal effort, flexible data input and comparatively
high performance. 
With the newly written code, it is possible to introduce the
qBinning algorithm to existing, open-source analysis platforms
like MZmine @todo citation.


Idee: Implementieren des Algorithmus in einer größeren Plattform
Fokus auch auf größerer Datensätze; Datenmenge nimmt wahrscheinlich zu
funktionsweise
Feature: Nur bins weitergeben, wenn in Serie gemessen

## qBinning @todo better name
wie funktioniert der algorithmus?

### why is binning necessary?

### generation of EIC in other software

### advantages of qBinning
parameter-free, quality score for feature priorisation, ideally faster (time) @todo measure

## Implementation
Terminology: open/closed Bin; maxdist
A bin is a data construct with associated data points and defined operations, an EIC is just a set of data points.
@todo capitalisation for data objects?

### Differences to the Original Implementation in R -> discussion / conclusion; Konkrete Beobachtungen / wertende zusammenfassung + addressat, kontext existierender literatur ; vergleichbare Ergebnisse?
R: 1-2 GB (factor 2 to 4 of in-place operation) of memory needed for Warburg, 
30% CPU use on Ryzen + 10% from RStudio, very long runtime (640 s)
writing to csv is significantly slower in R -> probably main timesink

The most significant difference to the original implementation is that 
while previously a dataframe object was used to handle binning by
changing the order of data points, now a bin 

### Module Requirements
The modlue has few requirements, only requiring centroided data with values
for mz and the scan number and a measurement of the mass error each centroid has. 
While, if used as part of the qAlgorithms pipeline, these measures are generated 
in the qCentroiding step, the program also allows the user to specify a 
per-centroid error when reading in a .csv or to assume a static error of x ppm. 


### Data Organisation
Data is represented as DataPoint objects, which are handled in one of two ways:
* one RawData object
* multiple Bin objects in one BinContainer object  

<b> DataPoint objects: </b>
One DataPoint contains mz, RT, intensity, scan number, the centroid error and an optional 
control value which assigns it to a bin. Data points are never modified during the binning process.

<b> The RawData object: </b>
Centroided data is stored as individual scans, with one vector per scan. Within the scans, 
data points are sorted by mz. Scans are accessed through a vector containing all scans. 
It is possible to account for empty scans.

<b> Bin objects: </b>
Every Bin stores all data points which were determined to belong to the same EIC. All subsetting 
methods are called on the bin without requiring knowledge of data points outside of the bin.
The main difference between Bins and EICs is that a Bin only operates with pointers to data
points in RawData while the EIC can be used independently of the RawData it was generated from.
A new bin is created by specifying start and end of it within an existing bin. As such, the
program starts by creating a bin which contains every data point in the dataset.
Bins also keep track of information which can later be used for filtering or weighing
purposes, such as if any scans appear more than once in the data and median values
for scans.

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

Since Bins contain superfluous methods and variables, while additionally depending on the RawData
object for their content, they are not suited for passing forward to the peak fitting
algorithm. Instead, all necessary data is copied and the binning infrastructure freed.

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
This function returns a boolean to indicate if it executed successfully. 
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

### Diagnostic Functions
In order to provide a fast way to assess binning quality to a greater 
extent than just the DQS, the functions **makeBinSelection**, **summariseBin**
and **printBinSelection** were written. 
In the summariseBin function, up to
eight test properties can be hard-coded. These are, for example, checking
if two points in the bin have the same scan number or if median and mean
of any dimension deviate by more than a defined value. The tests are limited
to eight because their results are stored in a single byte. If conditions
override other tests, such as 0b11111111 being set if a highest-priority
condition applies, more bin properties can be stored. However, at 256 possible
different results, it is most likely too imprecise to work with more 
simultaneous test criteria.
The limit to eight values is a design choice to encourage the user to
focus on essential properties and limit the amount of high-overlap bins.
The makeBinSelection function uses the test byte to return a vector
containing all bins for which at least one criteria is true. 
printBinSelection then returns all bin specified by the vector and
writes their members to a .csv file. Optionally, the summary information
can be written to a separate file. 

## Evaluation
As already concluded in the original paper, the binning process results
in a dataset of EICs the vast majority of which possess very good
separation from their enviroment. While some errors still persist,
it is also important to note that this binning step is only part of
a larger analysis process. As such, even if the intermediate result 
table contains false positives, the peak detection step will likely
heavily reduce their impact through filtering for false positive EICs
and through the fitted regression compensating for single centroids 
being assigned to the wrong bin.

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

When comparing with the implementation in R, the DQS is sometimes identical.
For the entire dataset, 79.1 % of the DQS matched when rounded to nine digits.
This result does not change significantly (ßßß 5. Nachkommastelle) if bins which
contain two points from the same scan are excluded (ßßß total). Even when rounded
to three digits, only 93.2% of the DQS match. (Grafik)
This behaviour is independent from Bin association, which results in lower 
agreement when only comparing DQS per bin. (Grafik)

A significant advantage over R and julia implementations is that a 
single executable can be provided. This means the user is not
required to install a complete programing language and scripting
environment on his device or troubleshoot problems caused by
future changes to packages. While automated package installation
and version control could be implemented for both R and julia,
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

For R, the Rprof function was used to profile code execution.
A total execution time of 299 seconds on the i5 and @todo
seconds on the Ryzen was measured. in both tests, 46% of the execution time 
were spent in the function responsible for binning, 10% on copying
data and 6% on calculating the MID. The remaining time was spent 
on various r internal data representation, largely table management.
Especially the MID calculation, implemented as fast_mean_distance,
is massively slower than necessary due to creating four additional
tables, each with a lenght of the bin size to the power of two.
The total time spent on reading data from disk was 1.3 seconds.

For julia, code was run once before timing to not measure time spent
on compiling instead of executing code. Since no data is written to
disk during code execution, the entire code is considered for timing.
The total execution time on the i5 was 15 seconds. @todo more specific
// further testing was not performed due to a fatal error persisting
// in the code, which would likely change performance if fixed.


For the c++ implementation, the majority of time is spent on reading the
data from disk. This is, in part, due to the data structure being constructed
and to a greater degree because the parsing is very inefficient.
No attempt to optimise this part of the program was made, since it only
exists as a debugging tool. 
@todo optimised timing
At optimisation level 2, the highest possible optimisation when compiling
with gcc, ßßß seconds were spent on subsetting in mz, ßßß s on subsetting
in scans, ßßß s on calculating the DQS and ßßß s on other tasks, mainly
moving data from the container for open bins to the container for closed bins.

Performance is roughly equal to the julia script, while more detailed and
easier to access information over the entire process is provided. 


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
Specify error cases
Large bins > 100 are overrepresented in error crits


@todo runtime comparison

## Discussion

Centroid error is underestimated, leading to more strict binning than ideal

### Error Criteria
Not all bins conform to a set of "ideal" parameters. These parameters 
are defined such that a bin which adheres to all of them is as close
as possible to the theoretical model of a Peak-forming EIC.
Since all bins exist independently, only parameters which can be
calculated from the bin members are considered.
Criteria are implemented using the summaiseBin function and
as such are only evaluated in terms of being true or false.

**1) Duplicates in Scans**
The theoretical bin contains one element for every scan it extends
over. Since a declared puropse of this algorithm is to detect bins
even when single points are missing from a series, only a scan being
represented twice can be counted as an error.

**2) Too Strict Separation**
To detect too strict separation between either two bins or one bin
and one discarded point, the DQSB is used as a measure of distance
and compared with a hypothetical DQSB. For this hypothetical score,
it is assumed that every point has a nearest neighbour with a 
distance the critical value for the binsize + 1 normed to the mean
centroid error of the bin. The resulting score should always be lower
than the real DQSB. If it is not, the nearest point outside of the 
bin could be added to it without being discarded again due to lowering
the critical value. 
Ideally, the hypothetical score is above 0.5 but lower than the real
DQS on a per-point basis. This means that every point is, on average,
closer to other bin members than another point would have to be
counted as outside of the bin while being distant enough to another
real point as to not count it towards the bin in a vacuum.


// passage outdated
When using the DQS as the singular descriptor of bin quality, it is
difficult to draw conclusions beyond some points being more similar
to points outside the bin than other bin members. When taking the 
mean of all DQS for example as an exclusion criteria, such information 
is lost. Without knowledge of the data environment, borders other than
0.5 are not readily apparent. At DQS = 0.5, MID and MOD are equal for a
given point. It follows that a bin with a mean DQS at or below 0.5 contains
more datapoints that are less similar to their group than the surrounding
data. Even above 0.5, it is possible for a large percentage of the bin to
have very low scores. When given a bin the masses of which show drift,
meaning it is not necessarily one ion trace, large enough distances to
other points can compensate for massive discrepancies within the bin. 
While such an error is easily identified with a test for normality, 
the reverse situation is more difficult to detect. Here, a bin is formed @todo image here
by points with a very low centroid error, leading to other points which
represent the same mass being placed in a seperate bin. These two bins 
will have a very low MID, which makes the MOD largely irrelevant as 
long as it is ~2 orders of magnitude greater than the MID. To identify
such cases, there needs to be a reference value for the DQS past which 
bins need to be reevaluated.
As a first step towards this, it is possible to calculate a lower limit
for the DQS on a per-bin basis. This limit follows two assumptions:
* The bin follows a single normal distriution in mz
* The MOD is equal for all points and equal to the critical value used for binning
Heuristic tests with the MID function show that the mean of all MIDs for any normal
distribution is 1.128 * sigma. Since sigma and critical value (normalised to centroid
error) of any given bin are known, the worst DQS possible for these data points
can be calculated. This additional criteria is especially sensitive to bins 
like (image ...), because here the critical value will be very similar to the 
distance between neighbours. 
@todo relation of DQSmin to DQS
// end outdated

**3) Deviation of Median and Mean**
In a perfect normal distribution, mean and median are identical.
To account for deviations, error thresholds are defined based on
the established process parameters.
For deviations in the scan dimension, a discrepancy of up to six
is deemed acceptable. This is equal to the largest tolerated gap
during the binning step.
For mz deviations, the mean centroid error times two is accepted.
This allows both values to be at the fringe of their respective
extension and still be counted.

**4) Points Outside the 4-sigma Interval**
This functions as a simple test for normality and too heavy scattering.
At 4 sigma, the likelihood of a point being part of a normally distributed
sample is orders of magnitude lower than the 1% error margin used to
calculate the critical value during binning.

### Future Improvements and Additions
**Performance Improvements**
As of now, the algorithm is not multithreaded. Doing so would provide a 
significant increase in execution speed. Multithreading is possible
to use after the first bin is split, since every bin is treated independently.

The transferred data always contains a control DQS and centroid error,
even if those are not needed. By implementing them as optional parameters,
the total memory allocation which needs to be performed is reduced.

When performing the splitting in the m/z dimension, for small bins the 
greatest order space is often at the border. This leads to many redundant
calculations, especially towards the end of a subsetting step. If it is
possible to implement a check that does not require a lot of processing
time, a medium to large boost in efficiency can be assumed. For reference,
A test dataset with 261 centroids took 98 subsets in m/z total to identify 
one bin with ten members. 

**Feature Additions**
While the algorithm works well for LC-MS data, MS^2 datasets can only be 
binned according to the precursor ion. An additional method to subset
closed bins further according to MS^2 or MS^n data could be implemented
with relative ease as a separate step following the existing subset methods.

Besides single-dimension chromatography, separation by Ion Mobility Spectrometry
or two dimensional chromatography is employed for NTS. Accounting for this
instrumentation during the subsetting step requires additional methods to
be written. Here, the way the DQS is calculated also needs to be revised.

**Usability Changes**
The program does not feature error handling in any capacity. Additionally,
only one dataset @todo adjust if testing SFC/GC
has been used to confirm error-free operation on two different @todo test on linux
computers, both running Windows 10. When running without an attached debugger,
error management will be impossible for a possible end user. For the qAlgorithms
tool to find widespread adoption, possible error sources need to be identified,
adressed or effectively communicated to the end user. This task requires 
extensive testing with a broad sample group of potential users.

Contributions to open source software like this are more likely to occur
if automated tests exist which can verify the validity of results quickly
after modification. Such tests should cover all stages of the program
at which potential errors can occur, such as after reading data,
after forming the initial bin, after completing a subsetting operation,
when adding a bin to the vector of closed bins and after the DQS calculation.
The test dataset for this purpose should contain multiple bins which display 
different behaviour, that is bins with good scores, bins with correctly
assigned bad scores and bins with bad scores due to either being too large
and including more than one ion trace (case @todo mention in discussion)
or one bin being split in two (@todo see above). These cases need to be
included so that a potential improvement of the algorithm is not discarded
for not obtaining exactly matching results.


OpenMS uses binary format for raw data, similar approach (save intermediates of workflow in binary to 
improve processing speeds)
more subsetting
implement into proper pipeline by returning closed bins // almost implemented
Implement a way to handle MS^2 / MS^n
automatic tests to verify program after user modifies it
Implement error handling, especially for cases where nonsense data can be detected - ideally implemented 
	with qCentroids for data. Error handling for new subsetting: control that bins are valid before DQS 
	calculation (sorted by scans) - best included in readcsv()
Add compile-time toggle for creating list of non-binned objects using macros
Cache optimisation
include an example for creating a new subsetting method

## Software and Data
R + package versions
Julia + package versions
C++ and compiler version
Describe Warburg dataset?