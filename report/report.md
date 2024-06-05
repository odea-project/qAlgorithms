Presenting an implemetation of the qBinning-algorithm developed by Reuschenbach et al. in c++ <!-- omit in toc -->
-----------------------------------------------------------------------------------------------
Report concluding the analytical practical by Daniel Höhn, supervised by Gerrit Renner <!-- omit in toc -->

***
- [Abstract](#abstract)
- [Abbreviations](#abbreviations)
- [Introduction](#introduction)
- [Implementation](#implementation)
	- [Algorithm](#algorithm)
	- [Module Requirements](#module-requirements)
	- [Data Organisation](#data-organisation)
	- [Core Functions](#core-functions)
	- [User-made Subset Functions](#user-made-subset-functions)
	- [Diagnostic Functions](#diagnostic-functions)
- [Evaluation](#evaluation)
	- [Result Comparison with R and Julia Implementation](#result-comparison-with-r-and-julia-implementation)
	- [Performance Evaluation](#performance-evaluation)
	- [Test Criteria for Bin Correctness](#test-criteria-for-bin-correctness)
- [Discussion](#discussion)
	- [Test Results](#test-results)
	- [Future Improvements and Additions](#future-improvements-and-additions)
- [Conclusion](#conclusion)
- [Software and Data](#software-and-data)

## Abstract

The purpose of the presented project is to provide a user-friendly and
programmer-friendly implementation of the qBinning algorithm introduced by Reuschenbach et al.
This is realized through thoroughly commented code, included
diagnostic functions which can be rewritten for specific use
cases with minimal effort, flexible data input, and comparatively
high performance. 
With the newly written code, it is possible to introduce the
qBinning algorithm to existing, open-source analysis platforms
like MZmine, PatRoon, and StreamFind.

The qBinning algorithm uses a statistical test to create
extracted ion chromatograms, removing the need for at least partially
subjective user parameters. Additionally, an estimator of bin
quality is provided.

This approach provides a greater understanding of the data generated
by non-target screening in general and in particular of the
binning process, thus serving as a step towards its standardized,
large-scale processing.

## Abbreviations
* NTS - Non-Target Screening
* (HR)MS - (High Resolution) Mass Spectrometry 
* m/z, mz - Mass to Charge Ratio
* RT - Retention Time
* GC - Gas Chromatography
* (HP)LC - (High Performance) Liquid Chromatography
* EIC - Extracted Ion Chromatogram
* OS - Order Space
* DQS(B/C) - Data Quality Score (of a Bin/Centroid)
* MID - Mean Inner Distance: The average distance in mz of an element to all other elements in a bin
* MOD - Minimum Outer Distance: The shortest distance in mz to an element not in the bin
* CPU - Central Processing Unit
* ppm - Parts Per Million (10e-6)
* gcc - GNU compiler collection (always referring to version 13.2.0)

## Introduction

With the increasing application of NTS-based methods for toxicological
analysis of environmental samples and the -omics disciplines, it
has become critical to assure comparability and quality of results.
The data processing step is a significant area of improvement here 
since it often depends on user-specified parameters or uses vendor-specific, 
closed-source software. Both properties make it difficult
to achieve comparability, the first if it isn't fully reported and
the latter due to a lack of insight into the concrete data transformations
performed. Rigorous quality assurance practices are necessary
to maximize the gain from NTS workflows [@vosoughNontargetScreeningWater2024].

It is expected that, in addition to more measurements being
performed, the size of datasets will increase. Both factors combined result
in very large amounts of data that require highly performant
data analysis software.

With the emergence of central databases for (analyzed) NTS data like InSpectra [@feraudInSpectraPlatformIdentifying2023],
there is a need to ensure result comparability through standardized
feature detection algorithms. The fact that different algorithms
lead to very different results when employed on the same dataset
poses a major challenge here [@hohrenkComparisonSoftwareTools2020],
however, it could be demonstrated that the correlation is much
higher when prioritizing high-quality data [@reuschenbachQBinningDataQualityBased2023].

The general steps of processing data obtained by HPLC-HRMS
in NTS workflows are centroiding, generating EICs, and identifying 
chromatographic peaks. The peaks are then summarised as features
and annotated, with the ultimate aim of relating all measured 
ions to their source molecule.
During each of these steps, quality parameters are needed
to obtain a comprehensive overview of the final result
reliability on a per-feature basis. For the centroiding 
step, such a parameter was already introduced with the DQSC [@reuschenbachDevelopmentScoringParameter2022].
Established algorithms, such as centWave [@tautenhahnHighlySensitiveFeature2008],
do not provide such a parameter for generated EICs.

The centWave algorithm identifies regions of interest (ROI)
by starting at the first scan and elongating present masses
with points from the second, third, etc. As tolerance, the mass accuracy
of the mass spectrometer is used. New additions are always
checked against the mean mz of all points already in a region.
ROIs must have a minimum size to be accepted [@tautenhahnHighlySensitiveFeature2008].
Mass tolerance (in ppm) and minimum size are general parameters
that must be optimized by measuring reference compounds.
If a filter by centroid intensity is used, it must also
be optimized. While some recommendations for field-specific 
settings exist, these are still quite vague and not
necessarily suited to environmental NTS [@forsbergDataProcessingMultiomic2018].
CentWave also directly fits a peak to the EIC and produces a feature.

These problems are directly addressed by reducing the amount of parameters
present in the actual algorithm and minimizing the steps during the analysis
which involve subjective decisions when setting mass tolerances or filter
thresholds. To this end, Reuschenbach et al. (2023) presented the qBinning
algorithm, which aims to resolve centroided NTS data using only information
already present in the dataset [@reuschenbachQBinningDataQualityBased2023].

Accordingly, qBinning does not depend on user input but instead utilizes 
statistical principles' parameters. These consider the essential
mathematical properties of a peak and sidestep the subjective approach
of manual parameter selection and optimization. The chosen approach allows
bins to be non-continuous in the scan dimension, which maximizes the
likelihood of obtaining features for compounds with a very low concentration.

The removal of this multitude of user-parameters
also removes the time investment needed to optimize them and provides 
a uniform base for other steps toward feature detection and annotation. [@gerrit: the time is only a side-problem with this. It is more like no established target value for optimization so far.]

A further advantage of the qBinning algorithm compared with other
binning methods is that it provides a quality parameter specific
to the processing step. Implemented as the DQSB, it provides direct
and simple feedback towards the degree to which a given bin could
be distinguished from its surroundings. This parameter
can also be used as an assessor for common error cases during binning.

In comparison to other algorithms, some of which require excessive amounts
of computation time upwards of multiple hours [@samanipourSelfAdjustingAlgorithm2019], qBinning is substantially faster. 
This enables its use in high-throughput analysis pipelines.

However, the existing versions of the qBinning algorithm require a
correctly formatted .csv file to function and cannot be run over
multiple datasets. This results in a large amount of manual
file organization and prevents the easy addition to pipelines
which have already been introduced by potential users.
To allow for the introduction of qBinning into existing, 
open-source analysis platforms like MZmine, PatRoon, and StreamFind
[@pluskalMZmineModularFramework2010; @helmusPatRoonImprovedNontarget2022; @helmusPatRoonOpenSource2021; @cunhaStreamFindFrameworkWorkflows2024], 
it was necessary to write a new implementation in the C++ programming language.
 
## Implementation

### Algorithm
From a high-level perspective, the algorithm searches for groups of similar masses
in a given dataset. It ensures no group has too large gaps in the chromatographic
dimension between included points. Where the grouping criteria do not match,
groups are split until a good grouping is achieved.
This results in many small groups with high internal similarity being created
out of one group containing the entire dataset and, as such, a very high-level
of internal dissimilarity. 

From this point, a group of data points will be referred to as
a *closed bin* if the algorithm determines that it cannot be subdivided further
and as an *open bin* if one or more of the [subsetting functions](#core-functions)
still has to check the grouping. Bins always refers to a grouping of centroided
MS data with the parameter mz and at least one additional dimension of separation, e.g., retention time.
Groups are only referred to as bins if they have not yet been handed to the peak 
fitting algorithm, meaning they still contain additional information like their cumulative 
error and a check if two binned points have the same scan number.
[@gerrit: unclear what you mean with metadata // addressed // still not really clear? - we need to discuss this]. 
Creating one or more new bins from an existing one while discarding all
points not present in one of the new bins is called subsetting. Only 
open bins can be subject to subsetting.

The user specifies which subsetting methods should be used and, if applicable, 
the parameters of these methods. The selection must be made before compiling the program.
The following description only concerns the grouping by mz and scan number of centroids.
Other subsetting routines are not included in the program at the time of writing.
It is assumed centroiding has been performed with the qCentroids algorithm [@reuschenbachDevelopmentScoringParameter2022],
meaning all centroids have a specified mass error.[@gerrit: what is the reason for this mass error?]

Subsetting is performed iteratively over all open bins. Since the algorithm requires a
bin to start, all centroids in the dataset are collected and merged into one bin initially.[@gerrit: this should be moved to the front of this section.]

First, open bins are subsets considering mz. Grouping in mz utilizes the order spaces of
neighboring data points, which are calculated after sorting points in the bin by mz.
Assuming that members of an ideal bin are normally distributed in mz, a test statistic
can be calculated based on the statistical alpha level and bin size. This statistic describes the largest
difference between two order spaces still within bounds for points of
the same distribution. This distance decreases exponentially with group size, meaning it
is much less likely for a dissimilar mass to belong to a large bin with a high 
density towards the edges of the probability distribution than a small bin with
low coverage of the entire distribution. 
The order spaces are normalized to the centroid error, which estimates the standard deviation of the assumed normal distribution. This cannot
be empirically determined before binning is complete.
The detailed derivation of the test statistic can be found in the supporting
information provided by Reuschenbach et al. (2023)[@reuschenbachQBinningDataQualityBased2023].
This criterion is then checked for the largest order space of every bin. When the
critical distance is surpassed, the two resulting fragments are controlled.
If no order space is greater than the critical distance and there are more than
five points in a fragment, it is turned into a new open bin. Five points are limited since no peak can be fitted to smaller ones, considering a peak model that uses four coefficients, e.g., the qPeaks model by Reuschenbach et al., 2024.
Once no fragment can be subset further, the resulting bins are subset by scans.

For subsetting a bin by the separation dimension like *scans*, it is sorted by scans and then subset at every 
position where the difference between two neighboring points is greater than six.
The distance results from the assumption that if a point is at the detection limit, it is detected with a probability of 0.5. If a point has not been
detected for six consecutive scans, the probability that the sixth point was 
present but at the detection limit amounts to 1.6%. 
Created fragments are turned into open bins unless they have less than
five members. It is considered closed if the bin was not changed during this subsetting step.

Both functions are executed alternating until no open bins exist.

After all bins are closed, the DQS is calculated for all bin members.
It uses the modified silhouette score [@starczewskiModificationSilhouetteIndex2016]
to assess the separation of a bin from the surrounding points. The MID in mz is
calculated per point while the MOD is searched for in all scans no
more than six scans away from the centroid. For the calculation, refer
to calcDQS in the Functions region of qalgorithms_qbin.cpp.
The DQS is scaled to the interval [0,1] to allow for weighing in later steps. The DQSB
is the mean DQS of all bin members.


### Module Requirements
The module has few requirements, only depending on centroided data with values
for mz, the scan number, and each centroid's mass error. 
While these values are processed from raw data in the qCentroiding step, if used as part of the 
qAlgorithms pipeline, the program also allows the user to specify a 
per-centroid error when reading in a .csv or assuming a static error of x ppm. 


### Data Organisation
Data is represented as DataPoint objects, which are handled in one of two ways:
* one RawData object
* multiple Bin objects in one BinContainer object  

<b> DataPoint objects: </b>
One DataPoint contains mz, RT, intensity, scan number, and the centroid error.
Data points are never modified during the binning process. 
DataPoints are Structs as defined in the c++ 21 standard.

<b> The RawData object: </b>
Centroided data is stored as individual scans, i.e., mass spectra, with one vector per scan. Within the scans, 
data points are sorted by mz. Scans are accessed through a vector containing all scans. 
It is possible to account for empty scans.

<b> Bin objects: </b>
Every Bin stores all data points determined to belong to the same EIC. All subsetting 
methods are called on the bin without requiring knowledge of data points outside of the bin.
The main difference between Bins and EICs is that a Bin only operates with pointers to data
points in RawData, while the EIC can be used independently of the RawData it was generated from. 
[@gerrit: so, what is the reason for using the structure of these bins instead of just EICs?-> this should be explained // addressed // no, we need to discuss]
Additionally, bins keep track of information that can later be used for filtering or weighing
purposes, such as if any scans appear more than once in the data and median values
for scans and masses. These details are irrelevant to the peak finding process and
are not passed up the pipeline to minimize memory allocations.
Since Bins contain superfluous methods and variables while additionally depending on the RawData
object for their content, they are not suited for passing forward to the peak fitting
algorithm. Instead, all necessary data is copied, and the binning infrastructure is freed.
A new bin is created by specifying its start and end within an existing bin. As such, the
program starts by creating a bin that contains every data point in the dataset.

<b> The BinContainer object: </b>
The BinContainer supplies wrapper functions to access all bins. The access for subsetting 
is organized so that the user specifies all methods and the order in which they are to be applied. 
While currently only two methods exist, this makes the program easy to
expand for further parameters. 
Bins are organized in two different data structures: Bins that were newly created are added to 
a deque object containing only Bins requires further subsetting (open Bins). 
The subsetting methods are applied to all bins sequentially, with every iteration following the 
user-specified order. If the last subsetting step does not change the bin, it is considered closed. In the current program, there are no significant gaps in the bin and bins that can no longer be a subset (closed Bins) are removed from the deque. 
Once a Bin is closed, it is added to a vector that contains all closed Bins. Once binning is 
complete, the DQSB is calculated for all bin members in all closed bins.


### Core Functions
The following functions, not including constructors and other utility functions, are included 
as part of the qBinning module:
* RawData: readcsv
* Bin: makeOS
* Bin: makeCumError
* Bin: subsetMZ
* Bin: subsetScan
* Bin: makeDQSB
* BinContainer: subsetBins


<b> readcsv: </b>
This function parses data points from a text file (.csv) and constructs a vector containing 
individual scans. Within each scan, data points are ordered by m/z in increasing order. These functions
primarily exist for testing since the qBinning module is designed to accept centroided data 
from the previous step in the qAlgorithms pipeline. 
If used to read data, an error has to be specified for each centroid. If the error is supposed to
be a certain ppm of the mass, a dummy value suffices since this is handled in **makeCumError**.
This function returns a boolean to indicate if it executed successfully. 

<b> makeOS: </b>
This function sorts a bin by mz and calculates adjacent elements' differences. It is 
required by subsetMZ.

<b> makeCumError: </b>
This function creates a vector that contains the cumulative error of all data points in a bin.
It is called after makeOS and requires the bin to be sorted by mz. 
If specified, makeCumError overrides the centroid error with an error in ppm of the
centroid m/z. This setting requires recompiling the program.

<b> subsetMZ : </b>
This function takes a bin after it was sorted by mz and splits it according to the approach 
described in [@reuschenbachQBinningDataQualityBased2023].
It starts by taking the OS of the bin that calls it and searching for the maximum, corresponding
to the largest difference between two neighboring mz values in the sorted bin. If the maximum
is greater than the critical value (as explained in the [introduction](#algorithm)), 
the bin is split at this position, and subsetMZ is called for both resulting halves. 
The critical value is calculated for an &alpha;
of 0.01 and normalized to the mean centroid error. The mean error is derived from the cumulative
error by dividing the difference between the cumulative error at the start and end of the bin by the 
number of data points in the bin. This is identical to normalizing the order space for every point.
(@gerrit: why mean error?->please explain // addressed // we need to discuss) 
Since a bin created as a subset of another bin shares OS and
the cumulative error with its precursor, both do not need to be calculated more than once.
The function terminates if the bin it is called on has less than five data points.
These points are then added to a vector, which can later be used by diagnostic functions. 
If a bin cannot be split further, it is added to the container for open bins.

<b> subsetScan: </b>
This function takes a bin, sorts it by the scan number of its data points, and splits it if the
difference in scans exceeds a given number. This parameter can be adjusted only when
recompiling the program. It is set to six following the above-mentioned reasoning.
After splitting the bin, only subsets with more than five elements are returned as bins to
the container for open bins. If a bin is not split, it can be considered complete and is 
closed. Closed bins are added to a separate container.

<b> makeDQSB: </b>
This function generates the element-wise DQS for all bin members. It calls the meanDistance and
calcDQS functions. makeDQSB assumes the input is a closed bin sorted by scans, from which it takes the 
first and last element. The range from the smallest scan minus maxdist
and the largest scan plus maxdist is the range that contains all data points
needed to calculate the DQS. Afterward, the bin is sorted by mz, and the mean distances per 
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
executed are listed in the order to apply. Since closing a bin is an action 
integrated into the subsetScan function, it must be the last function specified by the user
unless an alternative function with equivalent code exists. This design also allows for use 
cases in which a specific subsetting operation is only necessary for bins not marked
as closed by a different subsetting function. subsetBins can take an error (in ppm) as an
optional argument. If none is supplied, the centroid error is used. 

### User-made Subset Functions
For a user to specify a subset function, the only hard condition to be fulfilled
is that the function takes the vector of open bins to write to, some criteria of
subsetting, and the subset counter as arguments. Return values can be specified 
but are not expected. If the function serves as a terminator, it also needs to
take the vector of closed bins as an argument.
Within the function, isolate some properties of the data and use that to define
the beginning and end position of new bins within the old. Control for a size greater
than five before continuing the function if it is recursive and does not pass subsets
smaller than five to the bin vectors. After the bin cannot be subset by
the chosen criteria further, the beginning and end position of the sequence 
relative to the pointsInBin vector of the originating bin must be passed
to the bin constructor as a reference.
Always write open or closed bins within the function using .push_back(//NEWBIN//) to the respective container.
Once the function is confirmed to work correctly, it can be implemented in
the subsetBins function. This requires a new case to be defined for the central
switch statement. First, all relevant data that must be calculated outside
the bin is created. Then, binDeque.front().subset//SUBSETNAME//(...) is called.
After the subset function, add binDeque.pop_front() to delete the just processed bin.
Lastly, add the name of the new subset method to the SubsetMethods enum
defined in the BinContainer region's first line. 
Now, the new subset method can be specified in the measurementDimensions
vector, which is passed as an argument to subsetBins().

### Diagnostic Functions
To provide a fast way to assess binning quality to a greater 
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
as a std::tuple. This makes them usable by other user-written analysis
functions.
The limit to eight values is a design choice to encourage the user to
focus on essential properties and limit the amount of high-overlap bins.
The makeBinSelection function uses the test byte to return a vector
containing all bins for which at least one criterion is true. 
Alternatively, only bins not fulfilling any criteria can be returned.
printBinSelection then returns all bins specified by the vector and
writes their members to a .csv file. Optionally, the summary information
can be written in a separate file. 

## Evaluation
As already concluded in the original paper, the binning process results
in a dataset of EICs, the vast majority of which possess very good
separation from their environment. While there are still cases where a 
bin does not conform to separately developed [quality tests](#test-criteria-for-bin-correctness),
it is also important to note that this binning step is only part of
a larger analysis process. As such, even if the intermediate results 
contain false positives or negatives, the peak detection step will likely

### Result Comparison with R and Julia Implementation
All three implementations result in identical bins after completed subsetting.
Bin identity was controlled by matching the result frames by m/z. No false positives or negatives exist since
all points could be matched this way.
For a small test dataset, subsetting results were confirmed by hand.
The test data generation is detailed [here](#software-and-data). Both
"monobin" and "artbin" were controlled by sorting all contained points
by mz, identifying the bin created by the program, and checking if
the normalized order spaces between points were less than the critical value.
Both points at the bin's edges had a normalized order space greater than the
critical value. After sorting the bin by scans, no distances greater than
six scans existed between neighboring points.

The DQS calculation was not identical between any of the approaches. 
To determine a correct result for the DQS, it was calculated
by hand using two reduced datasets. To locate the source of the discrepancy,
intermediate results were also compared. Here, the calculation of the
MID was identical between all approaches. All
function implementations that calculate the DQS obtained identical
results for identical parameters. 

Significant differences exist between the two original implementations, with the
Julia script frequently producing wrong estimates. To account for floating
point errors, a DQS is considered identical if it is identical when rounded
to nine digits. Consequently, multiple cases exist where a DQS
is considered identical despite the MID differing by upwards of 30. 
The error is located in the function nearestNeighbourDistanceWindow!(),
where only the scan an element is placed in is considered for finding the 
MOD distances. For both reduced datasets, a total of one distance 
was determined correctly. Due to the small effect even large absolute 
differences in distance have, three DQS total matched the verified result for both 
(to nine decimal digits). When applied to real data, which is bound to have 
smaller MOD distances than the test data, this effect likely leads to better
matching with the correct results. 

When compared with the implementation in R, the DQS is sometimes identical.
For the entire dataset, 79.1 % of the DQS matched when rounded to nine digits.
This result does not change significantly if bins that
contain two points from the same scan are excluded (134 total). Even when rounded
to three digits, only 93.2% of the DQS match. (@todo image)
This behavior is independent of Bin association, which results in lower 
agreement when only comparing DQS per bin. (@todo image)
Testing with an artificial bin showed that the results generated by R
were not always correct for unknown reasons.
Correctness of the calculated DQS was controlled manually for both
heavily reduced real data and an artificial bin with three very
different bins and two points that do not belong to any bin.

A significant advantage over R and Julia implementations is that a 
single executable can be provided. This means the user is not
required to install a complete programming language and scripting
environment on his device or troubleshoot problems caused by
future package changes. This is untrue, but automated package installation
and version control could be implemented for both R and Julia. As such, a significant 
improvement in terms of user-friendliness was achieved.


### Performance Evaluation
Performance evaluation was performed using the following CPUs: 
* Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz (referred to as i5)
* AMD Ryzen 3 3250U @2.6GHz (referred to as Ryzen)
Writing the results to disk was not included in the profiling tests,
since this is not the main task of the presented program and was
as such not optimised beyond the point of sufficient efficiency.

All tests were performed with one dataset containing 3.5 million
centroids. Scaling of performance criteria with data set size was
not investigated.

For R, the Rprof function was used to profile code execution.
The program never utilized the full CPU.
Total execution times of 299 seconds on the i5 and 762
seconds on the Ryzen were measured. in both tests, 46% of the execution time 
was spent in the function responsible for binning, 10% on copying
data, and 6% on calculating the MID. The remaining time was spent 
on various r internal data representations, largely table management.
Especially, the MID calculation, implemented as fast_mean_distance,
is massively slower than necessary due to creating four additional
tables, each with a length of the bin size to the power of two.
The total time spent on reading data from the disk was 1.3 seconds.
Additionally, the R code required up to 2 GB of memory during 
execution. This is roughly four times the size of the dataset
as plain text.

For Julia, the code was run once before timing to not measure time spent
on compiling instead of executing code. Since no data is written to
disk during code execution, the entire code is considered for timing.
The total execution time on the i5 was 15 seconds. 
further testing was not performed due to the DQSB calculation giving
wrong results for almost all bins. Fixing this error, where the MOD
is only determined in relation to the scan a point is in, would
likely result in vastly increased calculation time due to requiring
a completely different data organization.

The verysleepy tool was used for performance profiling of the c++ implementation.
Here, the majority of the time (~30 - 60 s) is spent on reading the
data from the disk. This is due to the data structure being constructed
and, to a greater degree, because the parsing is very inefficient.
No attempt to optimize this part of the program was made since it only
exists as a debugging tool. 
Profiling showed most of the calculation time was spent on creating the 
order spaces. Here, in turn, almost all the time fell to sorting the data.
Similarly, sorting is one of the main timesinks when calculating the DQS
and during subsetting.
For the Ryzen, at optimization level 2 (the highest possible optimization when compiling
with gcc), 3.12 seconds were spent on subsetting in mz, 0.83 s on subsetting
in scans, 1.92 s on calculating the DQS, and less than 0.1 s on other tasks, 
mainly deleting the first element of the bin deque.
For the i5, The respective times are 1.29 s, 0.35 s and 0.65 s, with
a negligible amount of time spent on other tasks.
Both processors are years out of date in terms of power and serve as
a reasonable benchmark for the computers used within labs. 

Performance is massively superior to the R script, even when including the
inefficient csv parsing. The Julia script has comparable speeds for subsetting
but does not provide any of the additional functions, nor does the DQSB calculation
function correctly. The goal of providing an optimized tool was met to the degree
that work with very large datasets should be possible even on limited hardware
within a reasonable timeframe, provided the entire dataset fits into RAM.

During development, it was observed that using long doubles when calculating
the DQS has an effect up to the fifth digit. Since this change did not affect
the obtained bins by any metric, it was reverted for an increase in performance.

### Test Criteria for Bin Correctness
Not all bins conform to a set of "ideal" parameters. These parameters 
are defined such that a bin that adheres to all of them is as close
as possible to the theoretical model of a Peak-forming EIC.
Since all bins exist independently, only parameters that can be
calculated from the bin members are considered.
Criteria are implemented using the summaiseBin function and
as such are only evaluated in terms of being true or false.
Results of the tests are presented [here](#test-results).

The two main ways a bin can deviate from the expected ideal
is by (1) containing data points that should not be in the bin
and (2) not containing points that belong to the bin.
In the first scenario, the separation was not enough, and the bin can be enhanced by narrowing it down with stricter tolerances for its dimensions or more limitations. An example of such a bin includes two "true" bins and some noisy data that acts as a kind of data bridge. (@todo add image here?)
In the second case, a bin contained a gap that was greater than
the critical distance at the moment of separation, but after this
the bin was split further into the previously discarded points
are within the critical distance. This case also includes situations
in which the discarded points form their own bin.

Both cases are important to detect and, where possible, to prevent
so that the generated features reflect reality as accurately
as possible. Inversely, feature detection could be set to
only consider bins that fulfill strict quality parameters.

To control both cases from both angles, seven different quality tests
were developed. They utilize only data that can be obtained
either directly from the bin, previous binning steps, or
with minimal calculation effort. This has the advantage of
being fast and not introducing additional parameters into the algorithm.

The following tests are numbered according to the bit position
that encodes them, as in 0x8)7)6)5)4)3)2)1).

They can be grouped by the above criteria and how the problem is
approached. Either a specific point is identified as the cause or the
general behavior of the bin is observed (Table 1). Test 6) is 
listed twice since it can be applied in both cases.

| Approach    	  | Bin contains wrong point | Bin does not contain right point |
| ----------:     | :----------------------: | :------------------------------: |
| **Find point**  | 1)						 | 2), 7)							|
| **Observe Bin** | 6), 3), 5)				 | 6)								|

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
The worst-case DQS is calculated for the entire bin, meaning it is the
average of all MIDs. This stems from the necessity to determine
a per-bin criterion, where a bin could also have been flagged for one
member fulfilling this condition. It was decided against this on grounds
of partial matches necessitating an additional completeness check for the newly formed
bin and further decisions in regards to how such a merge leading to even 
worse scores for the newly included point should be handled.
While this leads to a blind spot in the detection of bad bins,
a complete analysis would require the development of a new clustering
algorithm, which functions by increasing bin size and merging
similar bins in accordance with the detailed statistical criteria.

This test reliably detects some, but probably not all, bins
which do not contain all the points that they should. To gain
a comparison value, in a follow-up test, all bins which
have a point within the critical distance of the bin
size that would be removed from the bin if it was added
to it were marked.

**7) Correctly Separated Point Within Critical Distance**

This condition depends on test condition 2). It performs the same
operation but calculates the critical value using the bin size instead
of the bin size plus one. Only bins that do not fulfill condition 2)
are considered. If this condition is positive, there exists a point
with a distance to any other point in the bin below the critical distance.
However, upon adding this point to the bin, the change in critical
value will lead to this distance being too great and the bin being
split again. A bin for which this condition is true could possibly
benefit from slightly increasing the estimated centroid error but
should not be considered an undesirable grouping.

---

All following tests detect bins where the separation was not strict enough.

**1) Duplicates in Scans**

The theoretical bin contains one element for every scan it extends
over. Since the declared purpose of this algorithm is to detect bins
even when single points are missing from a series, only a scan being
represented twice can be counted as an error (as opposed to the 
absence of an expected scan). 
If a scan appears twice at a different mass, it is possible that two 
very similar bins were combined into one due to the centroid error
being too large. As such, duplicate scans can serve as an indicator
of the separation not being strict enough.

**3) Deviation of Median and Mean**

In a perfect normal distribution, the mean and median are identical.
To account for deviations, error thresholds are defined based on
the established process parameters.
Test 3) controls this in the mz dimension, with a tolerance of 
the mean centroid error times two. It was chosen since 
the (hypothetical) centroids at the position of median and mean
can be counted as identical if they overlap, and both are assumed
to have the same error. For consistency, especially between large bins,
the actual centroid error of the median is not considered.
If this condition is not fulfilled, it is possible that the generated
bin encompasses multiple bins and/or noise, leading to a skewed distribution.
This asymmetry is less relevant for small bins with large gaps between
points since the mean and median deviate more strongly here.
It is questionable whether those bins contain real peaks to begin with.

**5) Points Outside the 3-&sigma; Interval**

This function is a simple test for normality and too-heavy scattering.
At 3 sigma, the likelihood of a point being part of a normally distributed
sample is similar to the 1% error margin used to calculate the critical 
value during binning. Since this estimate does not consider the absolute
deviation of the outliers, it should only be used as a component of 
estimating the correctness of bins that show heavy scattering.

**6) DQSB below 0.5**

If the DQSB is less than 0.5, points in the bin are on average closer 
to a point outside of the bin than other bin members. Since the DQSB
is calculated using the internal deviation and distances to points 
outside of the bin, either very high internal deviation or very high
noise levels (or both) can be the cause of a very low DQSB.


The slots four and eight were not used for this evaluation.

## Discussion
{
Centroid error is underestimated, leading to more strict binning than ideal [@gerrit: we will discuss]
kontext existierender literatur ; vergleichbare Ergebnisse? [@gerrit: mostly not applicable, but we can compare with adap or roi]
(@daniel: both points redundant, binning strictness is too much work to characterise
the problem properly and result comparability requires more than one dataset => significantly more time. // adressed)
}

### Test Results
During testing, the median and mean of the scan numbers were
also compared. This test did not select bins with a notably
worse distribution and was removed. A replacement has 
not been introduced, but the peak fitting step will likely
remove all cases that could have been discovered in this stage.

The tests presented [here](#test-criteria-for-bin-correctness) were performed on the
[Warburg-Dataset](#software-and-data) after binning. It contains 55910 total bins, 3053 of which
(5.5%) fulfilled at least one test condition. The points in these
bins make up 24% of the entire dataset or 35% of all binned points. 

Due to the lack of automated evaluation, most bins were checked
visually for "good" distribution. While not entirely arbitrary,
there is a bias to this method.

**1) Duplicates in Scans**

There were 134 bins with duplicate points and 613 duplicates found in the
dataset. Making a distinction between the number of duplicates might
be of further interest when trying to eliminate this condition.
There are some bins with duplicates that do not have an even distribution.
Especially smaller bins (> 30 points) are affected.
Larger bins, especially those with 70 or more members, tend to have an even, but
very broad distribution. These bins are up to 0.01 mz broad, with
some unevenly distributed ones surpassing 0.1 mz broadness.
While the resulting bins might be overall worse, the case of a good
bin being connected to a noise region by a duplicate scan was not
observed. While one or more duplicates could be removed, it is
unlikely such an approach would increase in quality.


**2) Too Strict Separation** 

1.3% of all bins fulfilled this criterion. Notably, they were smaller
than average with a majority having a bin size of 5. This resulted in 0.1% of all points being affected.
For multiple bins, it was possible to find at least one point in the vector of
discarded points that could be associated with the bin in question.
There is very little overlap between this condition and others, with over
97% of cases are exclusively "too close" to another point.
Of these, some are two correctly separated bins where one includes a point that 
lies within the tolerated distance of the other. However, if they were combined,
the critical distance would be reduced and the gap between them too large.
While no method to count them is in place, such occurrences should be 
counted to case 7.

A probable cause for wrong separations is that during the subsetting in
mz, bins start out very large. This leads to extremely small critical distances,
and if the largest order space is within a bin, this bin is first cut internally
before the remainder is cut off. It is also possible that by this 
mechanism, otherwise complete bins were fully eliminated if none 
of the resulting fragments had more than five elements.
In-depth reconstruction of the binning process would be necessary
to gain a definite answer here.

**3) Difference Between Median and Mean in mz**

Bins that fulfill this condition tend to be misshapen
to some extent. Many show trends or are composed of two 
(probable) bins which are connected by a thin line of noise data.
While an indicator for bad bins, this condition also applies
to bins that display no obviously unusual or undesirable behavior.

**5) Points Outside the 3-&sigma; Interval**

The vast majority of bins for this condition were fulfilled and did not 
overlap with the other tests. Bins for which only points outside the
3-Sigma intervals were detected to account for 47% of all bins that were 
selected due to a positive test and 83% of all points in selected bins.
The average bin length if this condition applies is 403. It is a reasonable
assumption that this condition does not imply sub-ideal binning, since for
large bins the likelihood of at least one outlier is increased.
For the test to have proper weight, it should test for the likelihood
of the furthest value to belong to the bin using, for example, a t-test.

**6) DQSB below 0.5**

Notable for this test is that it occurs very rarely, with only ten cases
total, six of which also contain a duplicate scan (condition 2)). Bins have an average size of 10. 
Due to bins with this low a score likely not being considered in the first place,
and the low count, no specific corrective measures for this property
seem sensible. It is likely that the majority will be removed if condition 2)
is addressed either way.

A manual control of the affected bins showed that most of them 
had the majority of their points at the edges of the bin. One
bin showed a very even distribution of points in mz.
While there is a subjective bias to such a judgment,
all bins with a DQS less than 0.5 do not show the behavior
that would be expected of good bins. 

**7) Correctly Separated Point Within Critical Distance**

Condition 7) does not occur in connection with any other condition.
This conforms to the idea that these bins are correctly formed.
Bins are around 6 points large on average, making up 0.2% of all bins.
Centroid errors are not extraordinarily low, ranging from 10^-5
to 10^-3. It is unlikely that decreasing them will compensate for the
large difference between the critical values for 5 and 6.
This effect occurs primarily at low masses, since here the change
in the critical distance is far greater when increasing the bin size by one.
Since such points are less likely to correspond to peaks, efforts to
change the grouping are not warranted.

**Combined effects**

Nine bins show a strong deviation between the median and mean
and contain a point outside of the 3-&sigma; interval. None of
them are evenly distributed, and their average size of 923 members
is much greater than any other grouping. This is mainly due to a 
bin of size 2493 being included, but the other members are
still consistently larger than others. It is possible that
combining the two tests is a good measure to identify
bins that could be subdivided further, since for the nine cases
present in the dataset such a procedure seems viable.

Both conditions also apply to bins whose average
size is up to ten times greater than the average
size of all bins. This is partly owed to the
way these tests work since larger bins are more
likely to have outliers and uneven distribution.
A test selecting the upper percentile of bin size
would be needed to isolate the effect of size on
accuracy.

---

For those bins that fulfill test conditions, the average DQSB
is 0.9. This is slightly skewed due to small bins having the 
same weight as larger ones while accounting mostly for 
cases where bins were selected by having poor separation.
The mean DQS of all points in those bins is 0.97.
The DQSB can as such not be relied upon exclusively to 
identify unwanted bin data.

The presented test succeeded at identifying both
kinds of errors proposed earlier. Bins that do
not contain all the points that they should be often 
separated so that one or more points that are not 
associated with any bin could be added to them without causing 
a further split. Bins that do not conform to the tests
proposed for finding lacking separation at least partially
returned bins with strong asymmetry.
 Due to the inherent bias of visual selection
when controlling these cases, a better approach based on 
statistical methods are required to determine conclusively
if such a problem exists due to a systematical error and if so,
where it could be fixed.
 
It is generally possible to reassemble premature splitting, which
has the potential to improve result quality. Especially in cases 
where relevant points are not part of any bin, reassembly is relatively 
easy to include as a post-processing module. 

Bins that fulfill one or more conditions show the same distribution
in the mz - scan dimensions as unaffected bins and not binned points.

### Future Improvements and Additions
Here listed are changes to the program that would be beneficial, but
could not be implemented as of now. 

**Performance Improvements**

The first subsetting is not multithreaded, since omp macros
only work on for loops with no greater modification. If, before normal
bin subsetting starts, the bin could be divided by some criteria,
there is some additional time that could be saved. An improvement
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

The critical value, before normalization, is calculated often and 
repeatedly for the same binsize. Including it as a table would
likely save some time during the calculations.

A large portion of calculation time is spent on sorting data. Reducing the
number of sorting steps or implementing a more efficient sorting would dramatically
improve execution time, especially for very large datasets.

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
distance in the scan dimension. This problem could be addressed by 
scaling the MOD during DQS calculation by some factor obtained by a Gaussian
distribution which is 1 for distances in the same scan and greater for every
further scan until it is set to infinity for every scan past the tolerance of six.
Such a modification has not been generalized due to colliding with the 
comparison that is the basis for test condition 2). 
A solution for calculating the scaled DQSB is implemented. It has not
yet been tested, but can be returned with the non-scaled DQSB.
For implementation details, refer to the function **scaleDistancesForDQS_gauss**.
The function compiles and executes without errors or exceptions.

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
obfuscated property, and secondly - assuming any such links can
be established - increase the comparability of results for similar
samples between labs employing different separations.

It could be demonstrated that during specific steps of the algorithm,
members of a group are erroneously split off. By identifying these
bins and revising their members, ca. 2% of all bins found in the
test dataset could be improved. One possible approach for this is to
re-add all "bad" bins to the dataset of discarded points and redo
the entire binning. This results in an operation that iterates over about
1/3 of the entire dataset again, and does not guarantee that this error
will not occur once more.
An alternative approach is binning a mass window. Given a more 
the elaborate structure of discarded points, for example, organizing them
in steps of 0.5 mz, the area needed for binning always spans 1 mz. 
This massively reduces the amount of subsets that need to be performed
per control.

While it already functions as a measure of separation, a secondary
statistic to the DQSB would allow to control of the generated values.
This test would, depending on calculation effort, not be included
as part of the standard DQSB function. Instead it serves as a control
of clustering quality unrelated to the test statistic used for binning.
One possible measure for this is the Quadratic Renyi’s Negentropy Separation [@martinsNewClusteringSeparation2015],
which is interesting since it decides if two generated clusters should
be combined. Since it does not depend on the error of a point,
bins with suboptimal separation could be detected even if the grouping
itself is correct.

The Algorithm is only insofar truly user-parameter free as there exist
no more than two different subsetting methods, where subsetting by scans
first will simply return the whole dataset as one bin. The moment a third
parameter is introduced, if it first causes a subset before the data
is subset by mz the final results might vary. The effect of such a choice 
depends on how large the resulting bins are since large bins lead to 
potentially more cases where a complete bin was erroneously subset.

Since a possible correlation between bad centroid scores and non-ideal 
binning was observed, integrating the score into the subsetting itself
might be a viable strategy for improving overall result quality.

**Usability Changes**

The program does not feature error handling in any capacity. Additionally,
only one dataset 
has been used to confirm error-free operation on two different
computers, both running Windows 10. When running without an attached debugger,
complete error management will be impossible for a possible end user. For the qAlgorithms
tool to find widespread adoption, possible error sources need to be identified,
addressed, or effectively communicated. This task requires 
extensive testing with a broad sample group of potential users and more varied data sets.

Reading in csv files is very slow. This was not a priority for optimization,
since it is not the intended main function of this program. However, for
general-purpose use a fast file reading is sensible. 

Contributions to open-source software like this are more likely to occur
if automated tests exist that can verify the validity of results quickly
after modification. Such tests should cover all stages of the program
at which potential errors can occur, such as after reading data,
after forming the initial bin, and completing a subsetting operation,
when adding a bin to the vector of closed bins and after the DQS calculation.
The test dataset for this purpose should contain multiple bins which display 
different behavior, that is, bins with good scores, bins with correctly
assigned bad scores and bins with bad scores due to either being too large
and including more than one ion trace or one bin being split in two. 
These cases are in some part covered by the existing controls, but a
tool to judge accurately if a change leads to actually better results
should be created. A possible implementation of this is a manual selection
of such cases for a test dataset. With such measures in place, 
a potential improvement of the algorithm is not discarded
for not obtaining exactly matching results.
Also to reduce the effort needed for a substantial contribution, the entire
project should compile and run correctly on a Linux distribution. For the
qBinning program, this is the case.


## Conclusion

It could be shown that the qBinning algorithm creates largely good results,
and more importantly, provides quality criteria that can be used to improve
the clustering results.  

Furthermore, an error in calculating the DQS was identified in both original
implementations. The alternative approach chosen here avoids incorrect results
and was verified under different edge cases. As such, the comparison with
other algorithms as described in the original publication should be redone [@reuschenbachDevelopmentScoringParameter2022].

A significant expansion of the program gives access to many optional tests for bin quality,
which enables a sophisticated pre-selection of bins before the peak fitting
the algorithm begins work. For example, only those bins that do not contain duplicate scans,
are completely separated from the dataset and do not have points outside of 3 sigma 
(tests 1, 2, and 4 are negative) could be returned by this program.
The tests also grant better insight into the weaknesses of the algorithm.
Through them, a probable cause for incorrect splits in bins could be identified
as an acceptable gap in a small bin which was considered too large while the correct bin
was still part of another, orders of magnitude greater bin.
Such insights are essential for providing a standardized NTS workflow
which allows its users to judge performance to at least a semi-quantitative level.

The program is now flexible and accessible for future expansions. 
This is expanded by comprehensive comments in the code It is now
possible to pass centroided data as an object that will not be modified,
and data can be returned in a reduced form, skipping the reading from and
writing to csv steps. Through other differences in implementation, a massive
improvement in terms of execution time was achieved in comparison to the R code,
while performing additional calculations for bin metadata and test conditions.
A full binning procedure taking just a few seconds is very important for
analyzing large datasets with hundreds of measurements. Such demands are
met by the presented implementation, provided a connection with the 
centroiding program is established.
The next steps include the complete implementation of the StreamFind platform,
which requires some further work to connect the existing code to the
standardized interface.

An important restriction of the presented test for too strict separation is that it
only detects bins where a majority of points are affected. It is a
reasonable assumption that such cases also exist in plenty at the
ends of larger bins, where they are not detected by the current test.
Developing further tests for similar conditions could present a way
to improve existing large bins by re-binning their edges with smaller,
neighboring bins and "free" points.

For maximising result correctness, the approach of exclusive subdivision
cannot be used alone. The development of additional steps, however,
should not happen in isolation from the peak finding algorithm.
An approach focused on merging existing bins with "free" points or
other, similar bins present a promising supplement to subset-based
binning if the formation of very large bins containing more than one
ion trace can be effectively prevented. Such a method would further 
benefit the binning procedure by addressing an issue that persists
in the subsetting-based approach.
The search for efficient detection and correction of non-ideal
bins should be the main focus when improving and expanding the 
presented program. Such improvements must always be shown to improve
peak quality or result correctness to justify being made.

---


## Software and Data
All calculations in R were performed with R 4.3.1[@rcoreteamLanguageEnvironmentStatistical2023], 
using the packages tidyverse 2.0.0[@wickhamWelcomeTidyverse2019] 
and pracma 2.4.4[@borchersPracmaPracticalNumerical2023].

The julia language 1.10.0[@bezansonJuliaFreshApproach] was used for executing the julia script.

The program is written in base C++, using only standard library functions[@ISOInternationalStandard].
It was compiled with gcc 13.2.0[@freesoftwarefoundationinc.UsingGNUCompiler1988].

Describe Warburg dataset @todo 
(Ich habe keine Infos darüber, was hier genau gemessen wurde)

The two test bins that demonstrated errors in the DQSB calculation for julia and R 
were created using the following R code and the Warburg dataset. 
Note: monobin_base.csv contains the first 42 entries of the full dataset
sorted by mz. There is a correct bin including all elements from 17 to 29.

``` 
library(tidyverse)
library(data.table)

monobin = read.csv("../../rawdata/monobin_base.csv")
monobin = monobin[which(1912 < monobin$scans & monobin$scans < 1958),]

# create one entry for all points from 1913 to 1957
for (i in 1913:1957) {
  r = runif(1, 2, 4)
  a = c(55*r, r*r/1000000, runif(1, 700, 2000), i, 1, -1, 0, -1)
  monobin = rbind(monobin, a)
}
for (i in 1:length(monobin$mz)) {
  t1 = c(monobin$mz[i]*runif(1, 2, 3), monobin$error[i], NA, monobin$scans[i], 1, -1, 0, -1)
  t2 = c(monobin$mz[i]*runif(1, 2, 4), monobin$error[i], NA, monobin$scans[i], 1, -1, 0, -1)
  monobin = rbind(monobin, t1)
  monobin = rbind(monobin, t2)
}
monobin$rt = monobin$scans * 0.65
monobin = monobin[with(monobin, order(scans, mz)), ]

write.csv(monobin, "../../rawdata/monobin.csv", row.names = FALSE)

# artificial data
artmz = rep(c(200,300,400), each = 10)
artmz = artmz + runif(30, 0.0001, 0.0009)
arterror = artmz * 0.000005
artscan = c(1:10, 6:15, 11:20)

artbin = data.frame(mz = artmz, error = arterror, rt = NA, scans = artscan, intensity = 1, ID = c(rep(1:3, each = 10)), DQScen = 0.99, DQSbin = 0.99)

out1 = c(215, 0.001, NA, 2, 1, 0, 0.1, 0.1)
out2 = c(300.5, 0.0015, NA, 20, 1, 0, 0.1, 0.1)

artbin = rbind(artbin, out1, out2)
artbin$rt = 0.65 * artbin$scans + 1

write.csv(artbin, "../../rawdata/artbin.csv", row.names = FALSE)
```
