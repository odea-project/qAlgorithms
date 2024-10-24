Evaluation of different, process statistic based approaches for describing and visualising <!-- omit in toc -->
measurement similarity in LC-HRMS nontarget measurement series as part of the qAlgorithms project <!-- omit in toc -->
-----------------------------------------------------------------------------------------------
Report concluding the research practical by Daniel Höhn, supervised by Felix Drees and Gerrit Renner <!-- omit in toc -->

- [Abstract](#abstract)
- [Abbreviations and Definitions:](#abbreviations-and-definitions)
- [Process Quality Estimators](#process-quality-estimators)
  - [Differentiating Process Quality from Result Quality](#differentiating-process-quality-from-result-quality)
  - [Measuring Process Quality](#measuring-process-quality)
- [Selected Process Statistics](#selected-process-statistics)
  - [Test Dataset](#test-dataset)
- [Calculations not Covered by qAlgorithms](#calculations-not-covered-by-qalgorithms)
  - [Preformance Criteria - Fourier Transform:](#preformance-criteria---fourier-transform)
  - [Performance Criteria - File Conversion](#performance-criteria---file-conversion)
- [Established Methods for Fault Detection](#established-methods-for-fault-detection)
- [Performance Criteria - Centroiding:](#performance-criteria---centroiding)
  - [Centroid Quality Score (DQSC)](#centroid-quality-score-dqsc)
  - [Signal Point Retention](#signal-point-retention)
  - [Centroid Count](#centroid-count)
- [Performance Criteria - Binning:](#performance-criteria---binning)
  - [Bin Quality Score (DQSB)](#bin-quality-score-dqsb)
  - [Bin Property Tests:](#bin-property-tests)
    - [Two or More Centroids of the Same Scan](#two-or-more-centroids-of-the-same-scan)
    - [Unbinned Points Within the Critical Distance](#unbinned-points-within-the-critical-distance)
    - [Signs of a Halved Peak](#signs-of-a-halved-peak)
    - [Drastic Mass Shifts](#drastic-mass-shifts)
    - [Asymmetric Distribution of MZ](#asymmetric-distribution-of-mz)
    - [Maximum Intensity at Edge of Bin](#maximum-intensity-at-edge-of-bin)
  - [Discarded Criteria:](#discarded-criteria)
    - [At Least Two Possible Maxima in Intensity](#at-least-two-possible-maxima-in-intensity)
    - [Point Within Maxdist but not Within Maxdist + 1](#point-within-maxdist-but-not-within-maxdist--1)
    - [One-Sided Intensity Profile](#one-sided-intensity-profile)
    - [Test for Asymmetry in MZ](#test-for-asymmetry-in-mz)
    - [Test for Points Outside Three Sigma Interval](#test-for-points-outside-three-sigma-interval)
- [Performance criteria - Peak Finding:](#performance-criteria---peak-finding)
  - [DQSF](#dqsf)
  - [Replicate Features](#replicate-features)
  - [Validity of the Identified Peaks](#validity-of-the-identified-peaks)
- [Development of Consistency Parameters from Process Statistics](#development-of-consistency-parameters-from-process-statistics)
  - [Consistency of the Centroiding Algorithm](#consistency-of-the-centroiding-algorithm)
  - [Consistency of the Binning Algorithm](#consistency-of-the-binning-algorithm)
  - [Consistency of the Feature-finding Algorithm](#consistency-of-the-feature-finding-algorithm)
  - [Result Differences Between Centroid and Binning / Feature Test](#result-differences-between-centroid-and-binning--feature-test)
  - [Combined Binning and Feature Test](#combined-binning-and-feature-test)
- [Applying the Developed Tests](#applying-the-developed-tests)
  - [Applied Test: Centroid Stability](#applied-test-centroid-stability)
  - [Applied Test: Bin Stability](#applied-test-bin-stability)
  - [Applied Test: Feature Stability](#applied-test-feature-stability)
  - [Applied Test: Feature-Bin Ratio](#applied-test-feature-bin-ratio)
- [Further Research](#further-research)
- [Conclusion](#conclusion)
- [Software and Data](#software-and-data)
  - [Software](#software)
  - [Data](#data)
- [References](#references)

# Abstract

Untargeted analysis using mass spectrometry generates large amounts of data
that are difficult to store, share and process in large quantities. To adress
this issue, information regarding the processing performed with qAlgorithms
is examined regarding its chemical informativeness. The property chosen as 
representative for chemical information in general is the similarity of 
measurements taken using the same or similar system conditions, where very
low similarity without an evident reason rooted in the sample is taken as
a reason to consider it non-comparable to the other samples.

Such an estimate is especially relevant when routine measurements of, for example, 
wastewater treatment plant effluent are taken to monitor the overall water quality. 
The decision of whether a system is stable enough for measurements to be comparable
is non-standardised and often depending on manual data review. The amount of
manhours needed drastically reduces the amount of measurements which can be
processed as part of a routine series, while introducing a subjective bias
into the type of discrepancies that would lead to the exclusion of a measurement.

The goal of this project is to find, describe and test different, dataset-wide
properties which do not depend on knowledge regarding specific features.
System stability is chosen as one central component of result comparability
which is independent of detailed information regarding the samples used. 
To this end, statistics generated within the qAlgorithms data evaluation 
pipeline are collected for multiple different measurement series. The process 
is designed to be fully automated, requiring no user input during data processing.

Three complimentary approaches, each covering one of centroiding, binning and 
feature creation, were developed and tested using seven different measurement
series. The resulting graphs allowed a visual distinction between different
groups of data, as well as the identification of potential outliers within
groups. These methods can be used for (limited) applications where the question
of broad sample similarity arises and demonstrate the general utility of the 
chosen process statistics. This puts a future research focus on deriving
insights from high-resolution mass spectra that is not related to specific
features or components, but rather their distribution and relations.

The utility of the developed tests could be demostrated using a dataset
where the pump pressure fluctuation gradually increased, with the
measurements becoming unusable after a certain point. Additionally,
a generally applicable model of result consistency is presented for the qAlgorithms workflow

# Abbreviations and Definitions:
* NTS - Non-Target Screening
* (HR)MS - (High Resolution) Mass Spectrometry 
* m/z, mz - Mass to Charge Ratio
* RT - Retention Time
* GC - Gas Chromatography
* (HP)LC - (High Performance) Liquid Chromatography
* EIC - Extracted Ion Chromatogram
* DQS(B/C/F) - Data Quality Score (of a Bin/Centroid/Feature)
* ppm - Parts Per Million (10e-6)
* PCA - Principal Component Analysis
* DDA - Data Dependent Acquisition

# Process Quality Estimators
An emerging and highly relevant problem in the field of NTS is
the fast and accurate estimation of total data quality or total
measurement quality through an operator. Especially for routine 
measurements, replicates and other series of very similar samples
this involves comparing the different steps of a series with 
each other, ideally over a large amount of samples at once.
The higher the amount of routine meansurements per sampling
interval, the higher the time needed for evaluation. This limits
the granularity of data and potentially restricts the analysis 
throughput to a number of measurements below the upper maximum
dictated through the instrumentation and selected chromatographic
method. Even if existing procedures need to be kept in place due
to contractual obligations, automated classification could select
the most relevant or representative measurement from a series of technical replicates.

Another point of application is the stability of a pooled sample
over the course of very long measurement series. Here, an easy way
of assessing the total stability while the instrument is still in 
operation allows the operator to react quickly to urgent problems
and gives limited insight into regions of the dataset not covered
by a more detailed stablility assessment using internal standards or 
the intensity of selected peaks chosen from a previously recorded
measurement of the pooled sample. Similar problems arise when
modifying the instrumentation while still requiring comparability
with the previous series.

These requirements place significant restrictions on the possible
ways to communicate the deluge of data that comes with (possibly)
years of measurements. Here, two types of approaches can be viable:
1) Machine-assisted or machine-led processing and search for regions of high change
2) A reductive approach that produces metadata which is still sensitive to shifts, but human-comprehensive

While these options are not exclusive, the final step of any
automated evaluation still needs to be that of communicating relevant
results to the instrument operator, ideally filtering out all irrelevant
information in the process. Such communication is especially
relevant to the qAlgorithms project, since the quality parameters supplied
can cause confusion with users.
For example, one user of the project asked which threshold to set
for selecting peaks generated with qAlgorithms. This misunderstanding
could be prevented by including an explicit descriptor that states
the viability of the measurement and communicates a point up to
which all data generated is reliable.
Similarly, (final) output data should always be provided in a format that
is compatible with spreadsheet software like Microsoft Excel, or,
for more complex data, some relevant subset which is human-readable. 

## Differentiating Process Quality from Result Quality
Previous work by Schulze et. al. (https://doi.org/10.1016/j.trac.2020.116063) highlighted
quality assurance as a core issue for nontargeted mass spectrometry and
discussed different dimensions for quality during an analysis.
Expanding on the step of "result reporting", a distinction should be
made between reporting the entire measurement for purposes of replication
and controlling through peer review and the reporting of just the 
measurement results. Schulze et. al. only consider reporting of the
results after interpretation, with the processing parameters and (unspecified)
statistical parameters being provided in addition. 
Confidence is also understood as the confidence of a correct identification,
not confidence in the peak being distinct from noise.
Besides it being impractical to replicate without all raw data being provided 
and thus very unlikely for this vital control to take place during peer 
review, it is not possible to estimate the algorithmic performance 
relating to result quality from just the algorithm settings on a per-sample 
basis accurately during NTA.

Result quality, as understood here, is the reliability and completeness
of all features at the end of any given processing workflow. For a
feature to be reliable, it has to correspond to a real molecule
that can be found within the sample in terms of mass and behaviour
in the chromatographic dimension or other dimensions of separation.
Furtermore, a reliable feature must conform to the MS2 spectrum,
if such a spectrum was produced by the instrument. After componentisation,
every component should include only features which have a high degree of 
certainty. The highest reliability in
a dataset is achieved if no false positives or negatives exist,
meaning the final results are a complete description of the sample contents.

Since this measure depends on ground truth, it is not always sensible 
to provide an estimator for result quality. The central difference 
between results and process as proposed here is that results serve
as the basis for some decision, be it through a researcher deciding
on the next experiment or a water treatment plant worker taking preventative
measures due to a detected contaminant. In all such cases, the process does not factor into
the final decision, unless it is as an abstract measure of confidence.
In contrast, the only decision based on a process parameter would be whether to
repeat a measurement due to a suspected error during sampling or analysis.

Process quality consists of the data processing workflow and the
(inferred) quality of the different steps from sampling to
retrieving the final result data set. The process can be split
into the two general steps of data acquisition and data processing.
The former presents a complex system that is impossible to perfectly
reproduce and very difficult to monitor in a quantitative manner.
While practices can be standardised, identification of errors 
with robust statistical methods is not easily realised once
sample processing has been performed, especially after prolonged periods of time. 
Here, error prevention is the most sensible practice, strategies for which have
been discussed by Schulze et. Al [https://doi.org/10.1016/j.trac.2020.116063] among others.
On the other hand, data processing must be deterministic
and fully quantifiable in order to enable automation. As such,
it is a viable point for introducing measures of process quality
both into existing systems and retroactively assessing the process
quality of past measurement series, provided the unmodified data was archived.

In terms of the fully formulated mathematical operations from
unprocessed data to the result table, quality describes the 
overlap between the assumed model that dictates selection,
combination and normalisation of data and the produced 
results. An example for such a quality parameter was developed
by Reuschenbach et al. [qcentroids]: The quality score describes
the overlap between a fitted gaussian curve and the measured 
data points. While such parameters give insight into the 
reliability of data, be it on a per-EIC, per-feature
or per-component level, this information does not directly 
derive from or relate to the result quality.

More importantly, if multiple steps feature a parameter of 
process quality, the different qualities likely can not
be converted into each other, or otherwise combined, without
losing relevant information or producing distorted estimations.

Under these conditions, deriving any indication of quantitative
result certainty on a per-result basis from the entire process 
is not possible. Instead, process quality can either serve
as a trigger for computation intensive corrections while
processing is active or as a way to provide non-quantitative
insight into the programs doing the processing to the operator.
Such insights are not limited to result certainty estimators,
but can give concrete feedback regarding the effectiveness
of a change in the process or the impact of a known disruption.

## Measuring Process Quality
Performance criteria must be robust, especially if further processing
actions are taken based on them. As such, established tests must
be validated using a representative sample of errors in the analysis
process. Current techniques to this end rely on a large amount and variety
of standard substances, which can then be described in terms
of recovery and mass accuracy of internal standards. [https://www.doi.org/10.1016/j.aca.2016.06.030]

Caballero-Casero et Al. [https://www.sciencedirect.com/science/article/pii/S0165993621000236] proposed guidelines on analytical
performance reporting in the context of human metabolomics.
The suggested criteria are sensitivity, selectivity, 
accuracy, stability and representativeness. All five largely
depend on internal standards, with the excpection of stability, which
also proposes consistency in the number of features as an addition 
to the recovery of internal standards.
The authors propose hard limit values for these, based on parameters
reported by questioned labs. These parameters 
In addition, internal standards never achieve full coverage
of a given chemical space in terms of retention time, mass
or ionisation behaviour. This limits the general applicability
of quality control by means of (exclusively) internal standards.
If very high amounts of standards are used in high concentrations
to facilitate high recovery, this also has the potential to mask
relevant trace compounds with similar behaviour.

# Selected Process Statistics
For the purposes of this analysis, characterisitic properties of
the qAlgorithms process were selected. These parameters are supposed
to cover the entire runtime of the algorithm and be significantly
different between different datasets. An advantage of only considering
process statistics is that the amount of data to be considered
is massively reduced, which in turn removes limits imposed by data storage
and transportation for almost all applications. Furthermore, if meaningful
information can be derived from such broad measures, this could sidestep
the non-technical restrictions imposed on sharing data by industry and government.

When assessing the entire process from profile-mode spectra to 
the final feature list, it is important to find parameters that 
are responsive to a broad number of deviations within the system,
while still being specific enough so that no change prevents another
from being percieved. An additional desireable quality is the tracability
from one detected error to the root cause or, in lieu of that, the point
in the algorithm where such an error occured.

Tested parameters:
* mean DQSC
* mean DQSB
* mean DQSF
* Number of centroids
* Number of bins
  * Bins with no features
  * Bins with one feature
  * Bins with more than one features
* Number of Features
  * Normal Features
  * Features with highly divergent mass trace

The three quality parameters (DQSC, DQSB, DQSF) calculated during the
different steps of qAlgorithms describe the model consistency of that
step to a limited degree. When taking their mean value at the end of their
respective step, they can indicate large differences between datasets
and make a statement about when during processing such a shift took place.
Here, an increase or decrease in quality is not necessarily related to an
actual increase in process quality for DQSC and DQSB. With the current
peak model, some noise tends to result in very low-intensity centroids 
with a very high score. Real signals have generally lower scores.
Similarly, bins with a real peak tend to have worse scores than very
small bins of data which does not contain a peak.
The DQSF is the only score which makes a statement about result quality,
since it serves as a measure of repeatability. [qPeaks]

The means of DQSC and DQSB were calculated from all produced scores. 
An alternative would be to only consider the scores of those 
points that were included in a feature, since both exist
as centroid-specific numbers at one point of the processing. This was 
decided against to mitigate the impact potential unknown problems with 
the processing would have on such a selection. A complimentary approach
which only considers such "useable" data should also incorporate the
as of now not implemented processes of componentisation and alignment.

The different counts were selected since they are not calculation
intensive or dependent on mathematical transformations to obtain. The latter
was avoided due to the possibility of introducing additional uncertainty
or some unknown bias into the parameters.

The centroid count is taken for all centroids with at least five profile
points. Depending on further additions to qAlgorithms, the count of 
centroids which do not fulfill this criteria in isolation, but are still
part of some regressions, could also be considered.

The three different bin counts were selected with the reasoning that 
many bins with no features are indicative of a noisy dataset and having
multiple peaks in the same bin implies poor chromatographic performance.
One bin containing one peak is the desired outcome.

For feature count and mean DQSF, only those features which do not 
fulfill the error condition were used. This is because the mechanism 
behind "unstable" mass traces is not understood to the extent that certainty
regarding their validity exist. The count of error-laden features is
not used as a process statistic, since it is unclear whether they represent
an oversight in the algorithm, a behaviour that can be corrected robustly
or an emergent property of the dataset not explained through other, more direct influences.

All three count statistics were normalised to the number of scans within a 
measurement to remain comparable between different measurement series.
This number changes within the individual series, but only by small amounts.
It could also be considered to remove those scans which are not shared between
datasets, provided a measure of analytical viablility on a 
per-spectrum basis is established.

## Test Dataset

The test dataset consists of eleven different groups from seven different
measurement series, all recorded using an orbitrap mass spectrometer and
electrospray ionisation, but differing chromatography and ion source conditions. For further details,
see [Software and Data](#software-and-data). Positive and negative mode 
measurements of the same sample are considered two groups, including 
one series using polarity switching.

The process statistics were controlled for normal distribution using a one-sided
t-test with an alpha of 0.01. No parameter was normally distributed. This is
taken as sufficient reason for them being meaningfully distinct between
datasets, under the assumption that algorithm-induced noise follows a
normal distribution. As such, all can be included in the PCA. 

Tests with different datasets showed that their correlation is inconsistent,
sometimes (anti)correlating strongly and other times being roughly orthogonal.
This implies that they contain information which can not be coerced into
one sum parameter without losing predictive power.

A PCA grouped by polarity was performed to find relevant parameters for 
process performance. In both cases, the numbers of centroids, bins and features
were highly correlating. In the positive case mean scores for bins and
features correlated, which did not apply for negative measurements. For both,
DQSC was roughly orthogonal to DQSF. When using only nine measurements from 
the same series, the relation and impact of the quality scores changed significantly.
One GC-HRMS dataset was also processed, but not included in the test dataset due to 
being too different from the LC-HRMS data and distorting the PCA. 

For further confirmation, one PCA containing all 129 processed samples was performed.
In the combined view, the different counts correlate, while mean DQSB and DQSF
roughly anticorrelate with the counts. the mean DQSC is orthogonal to both.
Four separate groups can be observed for the pump error, all with large 
distance in the dominant principal component. Similarly, one outlier can
be observed for the aquaflow D2 series. The ten replicates used as negative
control cluster very closely together. The total coverage of 85% leaves the
possibility for a relevant property to not influence this plot.

[@todo] add plotly interactive graph here

Due to the lack of established quality parameters, a comparison
with some "true" measure of quality is not possible. While the
quality scores implemented in qAlgorithms provide some information
regarding process quality, no correlation with result quality has
been established. Furthermore, errors persisting in the program
itself are possible. These factors reduce the viability of qAlgorithms
itself for controlling process quality variables. Since it could
be shown that the DQSF correlates with reproducibility [qPeaks],
The DQSF could function as an indicator for a process variable
not providing correct information should features with very data quality
scores contribute significantly to a very low process score.

To resolve this issue, two datasets - one for a case with no errors,
one for a case with a lot of errors - are used. The error-free series
consists of ten replicates of a matrix-free standard solution, measured 
in succession, with no noticeable deviation from each other. It is assumed
that this dataset and similar ones are fully free of data which would 
cause issues during processing with qAlgorithms. As such, any test that would
identify one of the ten measurements as defective is considered to be 
unreliable.

As a positive control, data from a UV degradation experiment was used.
During the measurement, the column pressure was highly unstable.
It is assumed that the individual measurements are both highly inconsistent
for replicates and generally contain bad peak assignments due to
the elution profile no longer being gaussian. A test which identifies
these measurements as defective is considered to be viable.

The mean DQSF values for the positive control average around 0.71,
while those for the standard solution average 0.73. With one exception,
the negative control measurements have the lowest DQSFs present in the
dataset. The four highest DQSFs in the test data belong to blank measurements
from two different series.

# Calculations not Covered by qAlgorithms 
// Diesen Abschnitt entfernen / kürzen? Ich halte es zwar für sinnvoll,
// auf die Schritte hinzuweisen, die wir nicht abdecken, bin mir mit der
// Umsetzung hier aber nicht sicher

## Preformance Criteria - Fourier Transform:
The fourier transform employed in FT-ICR and Orbitrap type mass
spectrometers is not something the instrument operator has access to.
With techniques like phase-constrained deconvolution [https://pubs.acs.org/doi/10.1021/acs.analchem.6b03636]
being possible, the potential negative influence of processing performed
in the instrument should be quantified and given with high granularity.
Without vendor cooperation, it will not be possible to estimate 
sensible quality criteria or to implement means of quality reporting
at this stage of the data processing.

## Performance Criteria - File Conversion
Due to most instruments providing measurements in proprietary formants,
they have to be converted using other software tools before processing.
At this stage, there is the potential for floating-point imprecision
to lead to different results for the same input file when using 
different software or different versions of the same software. [https://github.com/ProteoWizard/pwiz/issues/2407]
While the current accuracy of mass instruments does not usually
require more than 15 digits of precision, as the required accuracy
increases this could start affecting analysis results.
The maximum mass precision should be reported in the output file.
Conversion tools which depend on file transfer and user parameters
are additionally a potential source of user error. These cannot be
detected when using the vendor supplied instrument software for
visual inspection of the mass spectra.

# Established Methods for Fault Detection

// hier hab ich überlegt, ein Review-Paper für Data-Driven
// Fehlerdetektion  zu benutzen - findest du das zu weit hergeholt?
// https://ieeexplore.ieee.org/document/6423903

process monitoring
 SECTION IV.
Signal-Based Data-Driven FDD
Established fault detection methods utilise practices that fall under the
broader category of signal-based data driven fault detection.

# Performance Criteria - Centroiding:
It should be noted that not all approaches to mass spectrometric
data processing necesarily utilise centroiding of the raw spectra.
For these cases, measures of process quality are unlikely to be comparable 
to those presented here.
To differentiate between the data at different steps of processing,
a group of m/z value, retention time and intensity before any processing
has taken place is referred to as "signal point".

During centroiding, large amounts of signal points are reduced to
centroids, which encompass multiple signal points, while other signal
points are not added to centroids and consequently removed from the dataset.
A traditional estimation of false positive or false negatives is
not possible at this step, since any per-signal point information is 
lost during the following steps towards componentisation.

## Centroid Quality Score (DQSC)
The DQSC currently implemented through the qAlgorithms workflow
gives the percentage to which any given centroid conforms to the
expected gaussian shape. This correlates with the reliability
of the generated centroid. [qCentroids] 
The following test utilising the DQSC was considered:

For every measurement, the mean DQSC and the mean DQSCs of
upper and lower quartile were calculated with the compared 
criteria being the mean relative change between the three.
This test was discarded because the measure of difference was
smaller than 50% of the standard deviation of the DQSC for
all samples of the negative control series.

## Signal Point Retention

// Ich bin mir hier nicht sicher, wie sinnvoll ein entsprechender 
// Parameter tatsächlich ist. Müsste man wahrscheinlich ausprobieren.

Another perspective on process quality is the unifomity of intermediate
results when comparing replicates or time series that should be 
consistent, for example drinking water. In these cases, under 
ideal circumstances, all processing steps should have very similar
results, not just the final componentisation.
One such process parameters that can be obtained is the percentage
of signal points that are included in centroids, for every mass
spectrum in a measurement.

This parameter is not yet implemented in qAlgorithms at the time
of writing, while further questions as to the concrete implementation
remain. Notably, points at the edges of the observed mass region are
generally less relevant to the dataset, and the first few scans
encompass the void time of the chromatography. 
Since development of the centroiding algorithm is not fully completed,
future changes stand to enable this and similar measures with a more
represenatative coverage of the dataset than currently possible.

## Centroid Count 

// Ich hab eine ähnliche Argumentation für die Menge an Features in
// einem Paper gelesen, halte das aber im Rückblick nicht für zwangsweise sinnvoll.

It is assumed that generally, more initial signals equal a lower chance
of false negatives. As such, the amount of centroids produced - irrespective
of the DQSC - is also a measure of process quality during centroiding.

# Performance Criteria - Binning:

## Bin Quality Score (DQSB)
The DQSB in its current implementation [@todo] (paper binning, report 1) gives a measure of how well separated
a bin is from its environment by comparing the distances within a bin with
the distance to the closest point outside of it. In its original conception,
this only takes the retention time into account when searching for the closest
points not in the bin. To achieve greater locality of the score, the program
was modified such that only points within the maximal gap distance are used
to calculate the mean in-group differences in mz. This change led to, on
average, slightly lower scores. Furthermore, scaling was introduced for 
the outer distance (see [here](#dqsb-calculation)).

## Bin Property Tests:
// Es lohnt sich wahrscheinlich nicht, diesen Abschnitt beizubehalten. Ich halte die
// Tests zwar für erwähnenswert, aber sie tragen nicht wirklich zum Gesamtkunstwerk bei.
In addition to the quality score, multiple property tests are performed on 
each bin. These serve the purpose of descibing commonly observed patterns
during binning that often occur in distorted bins. These secondary tests
are used to take corrective action where this is possible, or to relax the
strictness of some binning parameters. This is sometimes necessary because
the peak finding algorithm assumes all centroids to have the same mz. 

While not all tests are applicable for improving the data passed to qPeaks, 
they give further insight into how the process functioned. Since all tests
aim to identify points of failure in the bins without knowledge about the
regression, the number of bins which did not fail any test, the number of
these bins that contained at least one peak and the number of bins with 
at least one positive test that contained one or more peaks are also taken
as parameters for the binning / peak finding process.

A complete overview of all currently used tests and those tests that were
implemented at one point and then removed is given below. Tests can be
used both for corrective action and to assess bin quality, since a correction
is not always possible. Every bin which shows positive for these could
not be improved through the described process.
As of now, the found peaks do not stem from any bins that did not trigger 
any test during binning. The additional criteria are not considered to
carry any degree of certainty for that reason.

### Two or More Centroids of the Same Scan
This test is necessary because the peakfinder cannot utilise more than one 
centroid per scan. All bins where this applies are marked. Before finding
peaks, only the centroid with the highest DQSC is retained. For most 
datasets, the rate of bins with duplicates is below 0.1%, with many
affected bins being ones that contain a hundred or more centroids.

### Unbinned Points Within the Critical Distance 
During binning, sometimes points are not included in a bin
despite - taken in isolation - fulfilling the criteria for
inclusion. However, they cannot be part of the bin without 
also including more points. Since the critical distance
shrinks with the number of points, the bin is split
into the one or two bins that fulfill this test.
This test does not count points that would increase
the critical distance such that the bin would be split
by themselves.

This test is used in conjunction with the next to 
combine two bins which show signs of being split in
the middle of a peak.

### Signs of a Halved Peak
If a peak shows a uniform monotonous rise in intensities,
which is defined as at least every other centroid having 
a higher intensity than the previously highest intensity,
it is considered to be one half of a peak. Bins which 
fulfill this criteria generally contain few data points.

Both this and the previous test are specific to the beginning
and end of a peak, although this information is lost after 
completed binning.

In some cases, peaks that are part of one very large bin
are cut, despite the mass differences being within tolerance
if they are viewed without considering the large amount of noise
data that is also within the bin. If the other half ends up as a 
viable bin, one correct peak more can be found if both halves 
are merged again. Since they were split due to too many points
lowering the critical distance below what is reasonable for the
peak, if a bin that could contain half a peak has a point of another
bin in range (determined by the previous test), both bins have 
a distance of no more than three scans and the assumed peak apex is
in the same direction (in scans) as the bin that is considered for
a merge, Both are combined even if this leads to the critical distance
being surpassed. If this merge is correct, one more peak can be 
characterised. If it is incorrect, either no peak is found in the
region and the merge has no effect or, in the worst case, the mass
of a correct peak is slightly distorted by including some 
otherwise avoidable noise.

### Drastic Mass Shifts
A bin is considered to undergo a drastic mass shift if median and
mean in mz are more than two times the mean centroid error apart.
Bins display this behaviour if at the end of a bin, few points 
undergo a drastic increase or decrease in mz.
This test is not used for corrective action.

### Asymmetric Distribution of MZ
Assuming a normal distribution, 80% of points should be within the
1.3 sigma interval. 80% were chosen as the limit so that one point
of deviation is still within tolerance for bins of size 5.
This test is not used for corrective action.

### Maximum Intensity at Edge of Bin
If the highest intensity in a bin is at the first or last centroid of
the bin, it can not be the apex of an interpolated peak. This goes
against the model behaviour of a correct bin.
This test is not used for corrective action, but could be employed 
as another sign of a halved peak.


## Discarded Criteria: 
[@todo] Is this a sensible category? 

### At Least Two Possible Maxima in Intensity
If the highest and second-highest intensity in a bin are more than
three scans apart, more than one regression for this peak is expected.
If only one peak was found, this could justify a closer look at the bin.
This test has been discarded since at bin-stage, this information is
not useable for corrective action and the test condition is fulfilled
too often where no feature was found. A similar test on a per-feature 
basis is preferable.

### Point Within Maxdist but not Within Maxdist + 1
The test was removed due to not providing actionable, non-redundant
information. With the addition of a standard-deviation corrected
estimate for the critical distance, potentially interesting outliers
are already covered. The ones which aren't included are represented
through the DQSB.

### One-Sided Intensity Profile
Originally, this test only checked if the maximum intensity of the
bin was also the first or last scan which is covered by the bin. 
This would prevent a gaussian fit with a maximum within the bin.
A related case, which relatively frequently occurred in bins without
positive tests, is the maximum being the second to first/last scan.
This is especially likely for small bins purely containing noise.
As such, the test condition was changed to the maximum being the smallest,
the smallest + 1, the largest or the largest - 1 scan.

### Test for Asymmetry in MZ
A relatively frequent problem with the binning results is that
sometimes, either a low-deviation mass trace identified visually
has a lot of noise on one side or is binned together with a different
mass trace. Such bins would, ideally, be subjected to a stricter 
binning and correctly separated.
The test was designed to find bins in which the region in mz around the 
mean mz has a lower density than the bin borders. The test divided 
the bin in three sub-bins in mz, assigned points and compared sizes.
It was positive if the middle region did not contain the most points.
The test was discarded due to having no statistical basis. To obtain
useable results, the expected difference between regions had to be
scaled, while the division into thirds had no basis in the expected
behaviour of normally distributed data and was extremely sensitive to
outliers.

### Test for Points Outside Three Sigma Interval
This test was implemented to identify those bins that
were too broad (in mz) to form visually identifiable ion traces.
It was removed due to outliers not directly corelating with
good bins - often, points with very low intensity at the edges of
the relevant scan interval triggered the detection.

# Performance criteria - Peak Finding:
## DQSF
The DQSF describes how well the chromatographic peak underlying a 
feature fits the peak model. It already serves as a way for the user
to priorise peaks for further analysis and make a qualified decision 
regarding conflicts in componentisation or assignemnt of MS2 spectra. [qpeaks]
However, the feature-specific nature of the DQSF makes it unwieldy as
a tool for assessing the entire dataset. When taking the mean or median,
The few features of extremely high quality lose impact, whereas a more
selective subset of peaks loses its validity concerning the entire 
dataset. While the mean DQSF is used for sum parameter characterisation
of a dataset, it is not used as the foundation for a test regarding
consistency or anything else. Based on preliminary tests it is assumed 
that a relevant difference in DQSF does not provide information not 
covered by any of the other presented tests.

## Replicate Features
While not implemented at this point in time, the number of times
a feature is found in a set of technical replicates is also of 
interest for evaluating process quality. If process statistics 
can be shown to correlate with high replicability, this provides both
further insight into the importance different processing steps have
and a way to optimise instrument conditions towards good data
while reducing the measurements performed to this end.

## Validity of the Identified Peaks
During development, it was found that some generated peaks will not
fulfill the condition of being part of only one mass trace. Around two
percent of all peaks found displayed this behaviour. This property
of a peak implies poorly resolved mass traces during the binning step,
while the fact that a peak could be constructed points towards mostly singular 
noise datapoints being included. Peak scores in these "contaminated" bins are
not noticeably distinct from those in regular bins.
It is possible that these bad inclusions are a result of the "correct" centroid,
that being the real signal of this ion trace in the given scan, not
being detected by the centroiding algorith while still being measured in
the instrument. Such issues could be addressed by finding a way to utilise
regions of the scan which contain real peaks with a width less than 
five and as such not constituting a viable peak during centroiding
or by interpolating such points instead when fitting the regression.
It could also be possible to introduce more region specific filters during
binning to find these points and search for a better fitting centroid.
The total number of these peaks is also a process statistic that can be
used for generalised performance assessment, since every peak with these 
characteristics represents an error during processing.
It is, however, not directly clear where in processing this error occurred.
It could be that in the profile data, a better fitting signal was overlooked
because it did not fulfill the minimum standards for a peak. The possibility
exists that after binning, if the relevant point was a duplicate, the 
wrong choice was made. This cannot be the main reason for such errors due
to the rarity of duplicate scans in bins. It is also possible that no
profile signal exists at the point.

Another frequent cause of this behaviour is the centroid mass showing a
rapid change in either direction once the signal intensity surpasses 
some value. The mass returns to its previous position once the intensity
is below this value again. Examples of this effect could be observed in all
controlled samples, which makes the hypothesis that this is an orbitrap-specific
effect plausible. 

Corrective measures will have to differentiate between both cases.


# Development of Consistency Parameters from Process Statistics

// Ich habe hier nur Bilder für Aquaflow und die 
// Positivkontrolle (PC) im Verzeichnis hinterlegt.

## Consistency of the Centroiding Algorithm

It was observed that, when ordered by time of recording, the differences
between scores of neighbouring measurements are close to zero for defect-free
measurement series. Two different series of similar samples which were
measured one month apart had very similar DQSCs, barring one exception.
DQSB and DQSF showed a similar jump in difference, but tend to have a higher 
base fluctuation in the used datasets.
In the tested data, the last measurement of the continued series had 
abnormally many spectra recorded. This lead to two noticeable changes
in DQSC (refer image aquaflow). 

Similar effects can be observed when viewing the differences between
the total number of centroids per measurement. To display both graphs together,
they were normalised so that their greatest point, respectively, was 1.
The number of recorded spectra is inconsistent between measurements,
usually deviating by around four. Whether this had an influence on the
displayed results was tested by calculating the differences between
all points before and after normalising them to the number of recorded
spectra. Both series were scales afterwards such that their maximum value was one.
The greatest absolute difference between the two series was 1.1 10e-16.
This is within the floating point error, and both results can be
considered identical. The most visible difference to the DQSC variation
is that here, blanks do appear as a visible decrease.

Normalising to the number of spectra is done primarily to use the ten
replicate measurements as a baseline for series measured using another method.
In a real application, normalisation is not necessary.

The solution chosen for confidence limits uses a separately recorded
series of ten technical replicates. Their similarity is confirmed via
PCA before asserting them as the best-case scenario for a given set
of instrument conditions. In the shown series, this reference data
is included as a separate series using the naming SP_DDA... and
separated using a dashed black line.
The warning limit is set to ten times the greatest relative standard
deviation of the control series regarding scaled DQSC and centroid count,
the exception limit to twenty times that.
The exception limit was chosen such that the difference between different
measurement series always surpassed this value with at least one of
the two differences, while the warning limit was set to 50% of that.
The warning limit does not serve a clear purpose in its current implementation,
since all known outliers massively exceeded the exception limit. For
a better value, a measurement displaying relevant drift in this parameter is
required for a heuristic estimation of sensible limits.

The combined view shows a change in both parameters if the series changes,
both being largely constant within the group of known good measurements.
An increase in DQSC does not necessarily coincide with an increase in 
the centroid count. Within a series, some cases exist where only one of the
two experiences a major shift. Neither of the two has a consistently higher 
noise level. Black, dashed lines denote the border between two series.
At these points, it is expected to see a major shift in both score and
count once. This combination exists primarily to reduce the number of
images which constitute the final assessment and is not required for
an automated evaluation.

For an eventual user-facing implementation, blanks could be removed 
when displaying the differences in count and score. Additionally, the
display algorithm should account for outliers and add an explanation for 
the operator to the relevant region. This could also be realised by two 
independent tests for consistency on mean DQSC and centroid count.

For the positive control, this visualisation is made hard to evaluate
visually due to the very large change in centroid count to the 
replicates. For better visibility, these are not included in the graph. (centroid_test_PC)
Despite one point over the warning limit and two others visibly
different from 0, with the current configuration it is not obvious
that this data is not sensible to consider for further analysis.

In the Aquaflow data, the previously known outlier can be identified by a 
large change in both parameters. The three blank measurements also
exceed the exception limit. While the centroid count stays largely 
constant, the DQSC varies enough to surpass the warning limit for
cases in which differing sampling points are next to each other.

The SFC data shows much stronger deviation than the control series.
The change from positive to negative data causes the centroid count to
go over the exception limit, but not the DQSC. This behaviour is expected,
since the different points have differing settings for the ion source.

In the calibration series, negative measurements are more similar than 
positive ones. The change from positive to negative mode causes both 
parameters to exceed the exception limit. One possible outlier exists,
although it is not detected through one of the other tests.

## Consistency of the Binning Algorithm

The consistency of binning is evaluated without taking data unrelated
to the binning step into account. This has the
advantage that it can still be used if the centroiding was performed 
using another algorithm, while also not amplifying potential distortions
introduced in the previous step. It also allows, to a limited extent,
a localised view on the binning process. As such, only the properties
number of bins with no feature, number of bins with one feature and 
number of bins with more than one feature were considered for consistency.
The DQSB was not included since it is not considered to provide valuable 
information on a dataset-wide basis yet.

The base reasoning behind the developed visualisation is that the three categories
should show as little deviation as possible. Since no meaningful difference
between bins with one or more than one regressions in them could be found,
both are combined through summation. Consistency for both groups is expressed
relative to the same properties of the reference dataset. For both groups, the 
data is centered to the mean of the reference data. The absolute difference in 
average bins per spectrum is called "bin deviation" in the following.

For the ten replicates, it was observed that both groups show very low bin deviation.
In the datasets with obvious outliers shown in the PCA, but especially the Aquaflow
series, it could be observed that for those bin deviations, the two measures were
much further apart than in the other reference data. From this, it was assumed that
both deviations being highly dissimilar indicates an undesireable property of the 
data. For all datasets, the total count of bins with no features was higher than the
count of bins with features for all measurements. The case of one  deviation width 
being above and the other below zero was observed in the standard replicates 
and some other measurements.

To express both properties in a plot, the two bin deviations are visualised as a
bar connecting the two points. This serves to ease the comparison between distances
to the observer. This distance is referred to as deviation width in the following.

Using the Aquaflow dataset as a test case (image), it can be observed that
the two different types of samples (control and treatment) can be distinguished 
visually from the bin deviation, without this property being considered 
during plot construction. This was not the
case for the consistency using centroiding data. The blanks (D1_B+)
have a very high similarity, while the two days (D1+ and D2+) are mainly
distinguished by their in-group deviation. The wave pattern observed in
D1+ matches the sampling time. Deviation width is significantly greater than
the standards used as a baseline.

The measurement already identified with the centroid parameters is also
noticeably different in this visualisation.

The positive control using the pump error dataset (image) shows massive deviation
from the series mean between different measurements and generally very large
deviation widths. Due to the scaling, the baseline is only barely visible in some
cases. Not a single measurement has a bin deviation lower than 25.
It is easily visible that there is no internal consistency for this series.

The SFC data (image) shows generally similar bin deviation for both positive and
negative mode, with the deviation width being larger for negative mode.
No clear optimum is visible. 

The calibration series (image) is in the same range as the standard samples 
for samples with and without indigo, as well as the blanks. Negative mode 
measurements are not noticeably different from positie mode. Notably,
the measurement kaliHCO3_0.02_indigo_p, which has very low values for both
centroid parameters, does not show noteable bin deviation or greater 
deviation width than the standards. The blank measurement 01_H2O_1_p shows very 
high deviation in all four parameters discussed.

The PFAS data shows much greater inconsistency for the positive mode measurements
than the negative mode measurements. A similar behaviour is observed in the DQSC,
which fluctuates slightly more in positive than negative mode. This effect is 
similar for real samples and instrument blanks, so potential matrix effects on
the stability parameter likely have a strongly reduced effect compared to the 
ionisation.

While the chosen visualisation allows the identification of strong outliers,
decision limits are not implemented. The main challenge regarding those is
the observed sensitivity to different data properties. If the use case is 
exclusively one type of sample that is expected to have no seasonal deviations
like drinking water, an approach using the standard deviation of some preceding
time period could be viable. For other sample types, another possible use case
is control of another test which concluded similarity, but with a high degree of
uncertainty. 

## Consistency of the Feature-finding Algorithm
The similarity parameter for the feature generation test is the number of features
normalised to the number of scans. This measure was chosen due to the high
consistency observed in the ten replicates. As control limit, the three
sigma interval of the standard replicate is used. A line is used to mark the 
group mean, with the out of control condition being fulfilled if the tolerance
interval of a point does not include the group mean (image).
Scaling the means of different groups is unlikely to be useful, although it 
causes minor distortion in the present case due to the reference data being
from a different measurement series.

Notably, blanks and samples produce very similar results for all criteria 
tested. This could be explained by noise making up the absolute majority
of signals, which implies these citeria only respond to irregularities
during chromatography.

When using the different quality scores for general assessments of data
quality over a dataset, for example choosing instrument settings on basis
of a design of experiment, it should be controlled that the differences
which form the basis for a decision are greater than the random deviation
which is present in a series of technical replicates. For the present
measures of difference, one singular decision parameter is difficult to
construct. 

One issue with taking the mean as a baseline is heavy distortion through
large outliers. A more robust measure, like the median, has the potential
to introduce a bias when two separate groups exist in the data. Both cases
are fulfilled for the aquaflow dataset (feature_test_aquaflow), where one 
outlier distorts the mean and the not recognised groups of control and 
treatment. Here, a possible solution is to remove a detected outlier from 
the mean and repeat the process until only non-outliers remain. Such an
approach requires limits to the amount of points that can be discarded.

## Result Differences Between Centroid and Binning / Feature Test
It is noteable that the centroid evaluation does not correlate with the
other tests in terms of probable outliers or signal fluctuation. This
implies that some changes in the instrument condition or the sample behaviour 
are noteable, but made undetectable through the processing that follows 
centroiding. Series 4) demonstrates this well, since by all other comparison methods,
including PCA, the series shows a very high consistency, with the exception
of one outlier. This outlier is still cleraly visible in the bin count
consistency, but additionally a fluctuation in the centroid count between two
states (seemingly) independent from anything but sample order can be observed.
If such an effect exists in the binning or feature test, it is not noticeable.


## Combined Binning and Feature Test
For all datasets, redundancy between the binning test and the feature test 
exist. In terms of non-quantitative results, the relative height of the two
counts is very similar, with the main difference being the different measures
of quality. The feature test provides a rudimentary decision limit for outliers,
while the less concrete deviation width is only useful if the difference 
between two measurements is large. The similarity is reasonable, since the
amount of features in total directly depends on the amount of bins with features.
This suggests another way of describing consistency or general usability,
in that the degree with which a measurement conforms to a linear relationship 
between total bin count and total feature count (or some otherwise determined
subset of the two). As with the other tests, the counts are normalised to the
number of scans. There is no distortion introduced here, since both axis are 
scaled by the same value. This makes different series comparable.

[@todo] image relation_bins_features.jpg
Shown is the line of the linear regression calculated from all points not part of the
two pump error series. The one known outlier in Aquaflow_D1+ is noticeably 
further away from the regression line and much lower in terms of bin and feature
count, but still much more similar to the other data than the measurements
performed while the pressure fluctuated. The standard samples cluster very close 
together on the line. For the tested data, it appears possible to express the 
similarity between two samples, after features have been determined, as the geometric
distance between them in this plot. The distance from the regression line 
serves as an equivalent to the regression width, with previously detected
outliers having a greater distance than the rest of their dataset.

It is noteable that the different datasets follow (seemingly) the same function,
which also almost passes through the point (0, 0). More data is required to establish 
a proper test based on adherence to a linear relation which uses a heuristic
function, and to determine if such a test is sensible. For longer series, 
an approach utilising a set of reference measurements to create a starting
point and then modifying the reference function as new data is recorded
is also thinkable. An important issue to solve here is that only data
which does not display undesireable properties is included in the regression.


# Applying the Developed Tests
The similarity measures as descibed so far were tested on dataset 7),
which was not included in their design. Here, the most interesting 
property is that the pump pressure started being irregular gradually,
with early measurements showing a stable pressure profile and the last
measurements being performed under highly irregular pressure. 
The expected test behaviour is that the dissimilarity increases primarily 
as a function of measurement time. 

From visual comparison of the pressure profile, it is expected that 
measurement 9, but 10 at the latest, are seen as outliers by the tests.
While arbitrary, the data has not been utilised at the time of writing
and no empiric measure of usefulness exists.

[@todo] images

## Applied Test: Centroid Stability
A general increase in disorder for both centroid parameters can be 
observed, but the exception limit is only passed for the last two measurements.
The warning limit is only surpassed once (at measurement 10) before that.
While the general principles are viable, the decided limit values are
not sensitive enough to respond to the problems in this dataset.
[@todo] deviation relative to the standards.

## Applied Test: Bin Stability
Here, the first anomaly is again measurement 10. Notably, the distinction
is that the deviation width is very low, similar to the standards.
The following measurements show a large increas in both deviation and deviation width.
The general curve conforms to expectations, with a gradual increase that escalates
towards the end. 

It is questionable how sensible it is to set the deviation zero to a 
baseline defined by standards, especially if those standards belong to a 
different series. Assuming the point with lower than expected deviation
width to be a random outlier, in this case the general trend is that
a higher deviation means worse data, while a larger than average deviation
with is added for especially severe outliers.

Such an assumption, of course, needs to be validated using comparable datasets.

## Applied Test: Feature Stability
This test shows similar results to the bin stability test, with a 
noticeable increse starting with measurement 10 and 12, 13 and 14 having
a much greater feature count. Again, the issue of a distorted mean arises
through the severe outliers.

## Applied Test: Feature-Bin Ratio
It can be observed that the three obvious outliers have drastically different
values from the regression. Starting at the sixth measurement, a clear upwards
trend in the relative feature count can be observed. Starting at the first
measurement, the bin count increases with time. This way of displaying data
expresses everything the bin stability test intended to communicate more clearly
and with the added benefit of a (semi) quantitative comparison. The deviation
width is also communicated, but without actually including the distinction 
between bin types in the data. Very similar, but not necessarily identical 
information is included through the relative feature count. This supports both the 
relevance of such a measure and makes the bin consisteny test itself superfluous.

# Further Research
While the criteria found through exploratory data analysis are seemingly able
to make broad statements about process quality, they need to be related to
result quality for certainty in this regard. Such an analysis depends on
functioning and well-tested componentisation being implemented for the
qAlgorithms project and should consider factoring in only relevant regions
of the measurement. This relevance could be estimated over the proximity
to a bin which did not contain any features, which would de-priorise 
features still affected by void-time effluent. 
The degree to which such a method is oversensitive could be reduced by
Implementing different warning levels with independent criteria.

Depending on the instrument mode, i.e. just recording MS1 spectra, 
data-dependent or data-independent MS2 spectra, the time between MS1 
scans and as such the binning results are likely to differ. It should
be ensured that an implemented test does not mark these as non-comparable,
if necessary by implementing a different test for these cases.

The implemented visualisation tools should be validated using additional realistic
conditions under which instrument performance is reduced, like measurements
with an old column, or measurements after the system has been inactive for
a prolonged period of time. 

For being useful tools in decisionmaking, the control chart must also include
critical values for out of control situations. These should be identified
systematically by provoking cases that produce the smallest error which
invalidates the measurement.

Finding the root cause of detected problems purely through analysing the
data is unlikely to work for cases which do not result from an error in
the processing pipeline. Here is a possible application for machine learning
systems, which are trained on a relevant subset of the processing data and 
then used once an out of control situation occurs. For this approach to
be viable, it must be established that all used variables are largely
independent of instrument-specific conditions first. Such a confirmation
would have to involve many different groups contributing data and using
multiple models, the results of which would also have to be evaluated
separately. Such a control period would massively increase the workload
per analysis for a likely unsustainable amount of time. As such, 
instrument-specific models for routine measurements could be a better 
focus of development, if machine learning is to be employed at all.

The only mass analyser used for performance evaluation was the orbitrap.
To which degree the results are transferable to, for example, a QTOF,
would require a series being measured on both. At the time of writing,
no series of profile TOF data was availvable for comparisons.

Some applications for system stability visualisation in the environmental
sciences will deal with data that is subject to seasonal changes. 
Here, critical values for out-of-control sitautions should not be based
on the entirety of the data but only a relatively recent subset.
Similarly, if a change to the instrumentation is made, there needs to
be a user friendly way to exclude previous measurements from indfluencining
the baseline of new measurements.

For some possible outliers, only one of the three tests resulted in a
detection. Here, it is necessary to establish concrete decision parameters
for when to consider such a measurement an outlier.

All process parameters were taken as means. This is probably not the best
possible approach to characterise the general structure of a dataset, since
it ignores the way these parameters exist in relation to the three 
dimensions mz, RT and intensity. The development of a relational model
mapping these to the selected process variables and offering a more precise
comparison between datasets was out of scope for the present work.
Progress towards this could be made by demonstrating a linear relation
between bin count and feature count which is consistent for all measurements
in the test dataset which did not stem from a series with known defects.

While the relative bin count worked well for most presented tests, it
does not accurately convey the true average bin count per scan.
The average bin count as understood here is the amount of bins which contain
one point from this scan, and then the average being taken over this
count for all scans. It would be interesting to see if the relative bin
count can serve as a substitute for such a parameter or if sognificant 
differences exist between the two.

# Conclusion
It could be shown that, using exclusively very broad statistics generated 
as part of the qAlgorithms pipeline, some statements about sample similarity
are possible. This could be applied to monitor system stability, and 
methods for effectively visualising such changes were developed.
Nontheless, siginficantly more measurements using different instrumentation
are necessary to confirm the universal applicability of such measures. Additionally,
the proposed linear relation between relative bin count and relative feature
count should be tested using directed measurements.

The next step of this project is establishing boundary conditions on basis of
a model or robust heuristic. This should be established for all control charts
and validated using different datasets.

While it was not possible to test in a real application, the developed
visualisations did fulfill the expectations regarding their behaviour
when used for clearly defective data and data that did not contain any
known issues. The control limits derived from the standard deviation
of a good measurement replicate present a viable solution for identifying 
outliers for the measurements that were tested, with the exception of the 
aquaflow data. Here, the problem was grouping multiple different samples
together, which is not a problem with the intended use case.

The main advantages of process parameters as used here are a massive reduction
in the data, both in terms of size and complexity, as well as the ability
to compare two or more datasets without depending on the produced feature lists.
In consequence, data sharing is radically simplified and the processing time
for large datasets drastically reduced. The addition of a set of identifying
properties to datasets could also be used to find generally similar data
over thousands to millions of other measurements, provided a database
is established. For example, such efforts could make it easier to find 
relevant optimisation strategies from otherwise unrelated studies.

Since basic functionality could be demonstrated, further work focusing on 
deriving measurement-specific information on, for example, suspected representativeness 
regarding the sample, is a promising direction for the qAlgorithms project to take.
The generalised test for consistency further opens up the possibility of a
standardised comparison between different processing algorithms, where 
a low deviation denotes good datasets. Through this, the presented work makes
a relevant contribution to the future of Non-Target Screening methods and
their optimisation towards result reliability.


# Software and Data
## Software
All calculations in R were performed with R 4.4.0[@rcoreteamLanguageEnvironmentStatistical2023], 
using the packages tidyverse 2.0.0[@wickhamWelcomeTidyverse2019].
Images were generated using ggplot2 3.5.1[ggplot2].

qAlgorithms is written in base C++ 20, using only standard library functions[@ISOInternationalStandard].
For reading in files, the StreamCraft library is used[https://github.com/odea-project/StreamCraft].
It was compiled with gcc 13.2.0[@freesoftwarefoundationinc.UsingGNUCompiler1988].

The scripts used to generate the presented images and associated calculations 
are availvable on the [qAlgorithms github](https://github.com/odea-project/qAlgorithms/tree/qBinning_beta/test).

## Data
1) UV degredation of ibuprofen with peracetic acid:
This dataset was measured while a pump defect on the LC caused the
column pressure to increase and decrease rapidly. It is used because
these conditions should cause the produced data to be no longer
binned reliably, leading to the majority of features being wrong.

2) SFC-HRMS data:
This dataset is from a design of experiment series trying out different
ion source parameters. The source was set to polarity switching, 
resulting in two datasets per measurement after evaluation.

3) Aquaflow data: 
Three blanks, three standards and three samples from the Aquaflow 
project, all measured in positive mode. One sample measurement
is notable for not containing any centroids for the first 800 scans.
Naming: C = control, T = treatment, S = sampling time
Another sample contained 5000 MS1 spectra, while usually only 1300 were recorded.
The samples are wastewater effluent comparing the effects an
additional treatment step had on the water quality.
The measurements were performed one day apart, but the two runs contained samples
from different sampling points. As such, discrepancies are not 
assured to stem from the daily variation in instrument conditions.

4) Calibration series:
Excerpt of a measurement series containing four blanks, and seven
ozonation experiments with indigo, two of which were measured in
negative mode. These measurements were primarily chosen for their 
small filesize and poor chromatographic separation. No information
regarding the type of sample exist, but a standard solution in 
millipore water is likely from the naming scheme.

5) PFAS data:
HPLC-orbitrap DDA measurements consisting of blank measurements and
water samples containing PFAS. Unisensor samples were filtered
to remove matrix before measurement. It is only a small subsample
of a larger series which was selected by filesize for ease of
data transfer and processing.

1) High-consistency standards
As a reference for a stable system, ten replicates of a standard mixture
in pure water were mesasured on an HPLC-orbitrap system. They were 
measured at the end of a longer run and are assumed to represent 
samples which do not significantly differ from each other in terms
of instrument conditions or sample content.
Of the eight compounds in the sample, 2-4 (average 2.5) were recovered
using the currently existing limits of mz +/- mzUncertainty and lowest /
highest retention time. These tolerances are subject to change and as 
such, recovery is not considered as a way to determine one measurement
as better than another.

1) A repeat of the failed series that makes up dataset 1), performed
at the same conditions and with equivalent samples. Here, the same
error of varying pump pressure occurred, but with an observable, gradual 
increase from a ca. 15 bar mean amplitude to up to 70 bar amplitudes in the
pump pressure over a four-hour timeframe. 
13 complete measurements were performed and the instrument stopped halfway 
through the 14th. Of these, four measurements could not be converted to
mzML files successfully. For an overview, see the attached image. Note that
the filenames in the LC results are incorrect. 
[@todo] image
For this dataset, the mean DQSF shows a clear decreasing trend over time.

# References

[@todo] bibtex export

Special thanks to David Kniesel for suffering extended periods of instrument 
failure and supplying his data.