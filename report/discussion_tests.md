Evaluation of different approaches for determining and visualising <!-- omit in toc -->
process quality in LC-HRMS nontarget measurement series as part of the qAlgorithms project <!-- omit in toc -->
-----------------------------------------------------------------------------------------------
Report concluding the research practical by Daniel Höhn, supervised by Felix Drees and Gerrit Renner <!-- omit in toc -->

- [Abstract](#abstract)
- [Abbreviations and definitions:](#abbreviations-and-definitions)
- [Notes on the design of visual process quality estimators](#notes-on-the-design-of-visual-process-quality-estimators)
  - [Differentiating process quality from result quality](#differentiating-process-quality-from-result-quality)
  - [Measuring process quality](#measuring-process-quality)
    - [Validation criteria](#validation-criteria)
- [Preformance criteria - Fourier Transform:](#preformance-criteria---fourier-transform)
- [Performance criteria - File Conversion](#performance-criteria---file-conversion)
- [Established methods for fault detection](#established-methods-for-fault-detection)
- [Performance criteria - Centroiding:](#performance-criteria---centroiding)
  - [Centroid quality score (DQSC)](#centroid-quality-score-dqsc)
  - [signal point retention](#signal-point-retention)
  - [centroid count / density](#centroid-count--density)
- [Performance criteria - Binning:](#performance-criteria---binning)
  - [Bin quality score (DQSB)](#bin-quality-score-dqsb)
  - [Bin property tests:](#bin-property-tests)
    - [Two or more centroids of the same scan](#two-or-more-centroids-of-the-same-scan)
    - [Unbinned points within the critical distance](#unbinned-points-within-the-critical-distance)
    - [Signs of a halved peak](#signs-of-a-halved-peak)
    - [Drastic mass shifts](#drastic-mass-shifts)
    - [Asymmetric distribution of mz](#asymmetric-distribution-of-mz)
    - [Maximum intensity at edge of bin](#maximum-intensity-at-edge-of-bin)
    - [At least two possible maxima in intensity](#at-least-two-possible-maxima-in-intensity)
  - [Discarded Criteria:](#discarded-criteria)
    - [point within maxdist but not within maxdist + 1](#point-within-maxdist-but-not-within-maxdist--1)
    - [one-sided intensity profile](#one-sided-intensity-profile)
    - [test for asymmetry in mz](#test-for-asymmetry-in-mz)
    - [test for points outside three sigma interval](#test-for-points-outside-three-sigma-interval)
- [Performance criteria - Peak Finding:](#performance-criteria---peak-finding)
  - [DQSF](#dqsf)
  - [Validity of the identified peaks](#validity-of-the-identified-peaks)
- [Development of a consistency parameter from process statistics](#development-of-a-consistency-parameter-from-process-statistics)
  - [Consistency of the centroiding algorithm](#consistency-of-the-centroiding-algorithm)
  - [Consistency of the Binning algorithm](#consistency-of-the-binning-algorithm)
- [Error traceability](#error-traceability)
- [Viability test for simple process parameters](#viability-test-for-simple-process-parameters)
- [Further research](#further-research)
- [planned Expansions to qAlgorithms](#planned-expansions-to-qalgorithms)
- [performed modifications to qAlgorithms](#performed-modifications-to-qalgorithms)
  - [modified centroiding](#modified-centroiding)
  - [DQSB calculation](#dqsb-calculation)
- [Conclusion](#conclusion)
- [Software and Data](#software-and-data)


# Abstract
One important property of nontarget measurements is the comparability of different
measurements of a series with each other. This is especially relevant when
routine measurements of, for example, wastewater treatment plant effluent are
taken to monitor the overall water quality. 
The decision of whether a system is stable enough for measurements to be comparable
is non-standardised and often depending on manual data review. The amount of
manhours needed drastically reduces the amount of measurements which can be
processed as part of a routine series, while introducing a subjective bias
into the type of discrepancies that would lead to the exclusion of a measurement.
Additionally, such decisions are often made based on standard mixtures, which 
only provide limited coverage of a dataset and have the potential of masking
trace compounds with very similar chromatographic behaviour and mass.

The goal of this project is to suggest and, to a limited degree, validate multiple
different approaches to describing and quantifying system stability as one
central component of result comparability. To this end, statistics generated
within the qAlgorithms data evaluation pipeline are collected for multiple
different measurement series. The process is designed to be fully automated,
requiring no user input during data processing.

# Abbreviations and definitions:
[@todo] remove irrelevant entries
* NTS - Non-Target Screening
* (HR)MS - (High Resolution) Mass Spectrometry 
* m/z, mz - Mass to Charge Ratio
* RT - Retention Time
* GC - Gas Chromatography
* (HP)LC - (High Performance) Liquid Chromatography
* EIC - Extracted Ion Chromatogram
* DQS(B/C/F) - Data Quality Score (of a Bin/Centroid/Feature)
* ppm - Parts Per Million (10e-6)
[@todo] move all definitions for within the workflow here

# Notes on the design of visual process quality estimators
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
the most relevant measurement from a series of technical replicates

Another point of application is the stability of a pooled sample
over the course of very long measurement series. Here, an easy way
of assessing the total stability while the instrument is still in 
operation allows the operator to react quickly to urgent problems
and gives limited insight into regions of the dataset not covered
by the detailed stablility assessment.

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
ist compatible with spreadsheet software like Microsoft Excel, or,
for more complex data, some relevant subset which is human-readable. 

## Differentiating process quality from result quality
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
Besides it being impractical to replicate
without all raw data being provided and thus very unlikely for this vital control to take place during peer 
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
every component should include only features
which have a high degree of certainty. The highest reliability in
a dataset is achieved if no false positives or negatives exist,
meaning the final results are a complete description of the sample contents.

Completeness is the degree to which all molecules in the sample that arrived at
the mass detector could be reconstructed from the final list of components.
[@todo] very similar measures, change

Since both measures depend on ground truth, it is not always sensible 
to provide an estimator for result quality. The central difference 
between results and process as proposed here is that results serve
as the basis for some decision, be it through a researcher deciding
on the next experiment, a water treatment plant worker taking preventative
measures against a detected contaminant. In all such cases, the process does not factor into
the final decision, unless it is as an abstract measure of confidence.
The only decision based on a process parameter would be whether to
repeat a measurement de to a suspected error during sampling or analysis.

Process quality consists of the data processing workflow and the
(inferred) quality of the different steps from sampling to
retrieving the final result data set. The process can be split
into the two general steps of data acquisition and data processing.
The former presents a complex system that is impossible to perfectly
reproduce and very difficult to monitor in a quantitative manner.
While practices can be standardised, identification of errors 
with robust statistical methods is not easily realised. [@todo]
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
be converted into each other or otherwise combined without
losing relevant information or producing distorted estimations.

Under these conditions, deriving any indication of quantitative
result certainty on a per-result basis from the entire process 
is not possible. Instead, process quality can either serve
as a trigger for computation intensive corrections while
processing is active or as a way to provide non-quantitative
insight into the programs doing the processing to the operator.

## Measuring process quality
Performance criteria must be robust, especially if further processing
actions are taken based on them. As such, established tests must
be validated using a representative sample of errors in the analysis
process.
Current techniques to this end rely on a large amount and variety
of standard substances, which can then be described in terms
of recovery and mass accuracy. Internal standards always come with
the problem of only covering a limited region of the dataset and 
potentially masking similar compounds at low intensities.
[https://www.sciencedirect.com/science/article/pii/S0165993621000236]
Caballero-Casero et Al. proposed guidelines on analytical
performance reporting in the context of human metabolomics.
The suggested criteria are sensitivity, selectivity, 
accuracy, stability and representativeness. All five largely
depend on internal standards, with the excpection of stability, which
also proposes consistency in the number of features as an addition 
to the recovery of internal standards.
The authors propose hard limit values for these, 
In addition, internal standards never achieve full coverage
of a given chemical space in terms of retention time, mass
or ionisation behaviour. This limits the general applicability
of quality control by means of (exclusively) internal standards.

### Validation criteria
For the purposes of this evaluation, measurements withing a
series are ranked based on their median DQSF. A characteristic
found to correlate with the median DQSF is considered to be an
indicator for that process to have performed well. [rework]

Due to the lack of established quality parameters, a comparison
with some "true" measure of quality is not possible. While the
quality scores implemented in qAlgorithms provide some information
regarding process quality, no correlation with result quality has
been established. Furthermore, errors persisting in the program
itself are possible. These factors reduce the viability of qAlgorithms
itself for controlling process quality variables. Since it could
be shown that the DQSF correlates with reproducibility [qPeaks],
The DQSF could function as an indicator for a process variable
not providing correct information should features with very high
scores contribute significantly to a very low process score.

To resolve this issue, two datasets - one for a case with no errors,
one for a case with a lot of errors - are used. The error-free series
consists of ten replicates of a matrix-free standard solution, measured 
in succession, with no noticeable deviation from each other. It is assumed
that this dataset and similar ones are fully free of data which would 
cause issues during processing with qAlgorithms. As such, any test that would
identify one of the ten measurements as defective is considered to be 
unreliable.
[@todo] recovery of standards
As a positive control, data from an UV degradation experiment was used.
During the measurement, the column pressure was highly unstable.
It is assumed that the individual measurements are both highly inconsistent
for replicates and generally contain bad peak assignments due to
the elution profile no longer being gaussian. A test which identifies
these measurements as defective is considered to be viable.

# Preformance criteria - Fourier Transform:
The fourier transform employed in FT-ICR and Orbitrap type mass
spectrometers is not something the instrument operator has access to.
With techniques like phase-constrained deconvolution [@todo paper]
being possible, 
Without vendor cooperation, it will not be possible to estimate 
sensible quality criteria or to implement quality reporting
at this stage of the data processing.

# Performance criteria - File Conversion
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

# Established methods for fault detection
process monitoring
 SECTION IV.
Signal-Based Data-Driven FDD
Established fault detection methods utilise practices that fall under the
broader category of signal-based data driven fault detection.

# Performance criteria - Centroiding:
It should be noted that not all approaches to mass spectrometric
data processing necesarily utilise centroiding of the raw spectra. @todo MCR approach
To differentiate between the data at different steps of processing,
a group of m/z value, retention time and intensity before any processing
has taken place is referred to as "signal point".

During centroiding, large amounts of signal points are reduced to
centroids, which encompass multiple signal points, while other signal
points are not added to centroids and consequently removed from the dataset.
A traditional estimation of false positive or false negatives is
not possible at this step, since any per-signal point information is 
lost during the following steps towards componentisation.

## Centroid quality score (DQSC)
The DQSC currently implemented through the qAlgorithms workflow
gives the percentage to which any given centroid conforms to the
expected gaussian shape. [@todo paper] 
The following test utilising the DQSC was considered:
    For every measurement, the mean DQSC and the mean DQSCs of
    upper and lower quartile were calculated with the compared 
    criteria being the relative change between the three.
    This test was discarded because the difference between
    six samples of a DoE measurement series was less than 0.005
    in every category. The same observation was made when testing
    blank, standard and samples of a different measurement run.

## signal point retention
Another perspective on process quality is the unifomity of intermediate
results when comparing replicates or time series that should be 
consistent, for example drinking water. In these cases, under 
ideal circumstances, all processing steps should have very similar
results, not just the final componentisation.
One such process parameters that can be obtained is the percentage
of signal points that are included in centroids, for every mass
spectrum in a measurement.


## centroid count / density
It is assumed that generally, more initial signals equal a lower chance
of false negatives. As such, the amount of centroids produced - irrespective
of the DQSC - is also a measure of process quality during centroiding.

# Performance criteria - Binning:

## Bin quality score (DQSB)
The DQSB in its current implementation [@todo] (paper binning, report 1) gives a measure of how well separated
a bin is from its environment by comparing the distances within a bin with
the distance to the closest point outside of it. In its original conception,
this only takes the retention time into account when searching for the closest
points not in the bin. To achieve greater locality of the score, the program
was modified such that only points within the maximal gap distance are used
to calculate the mean in-group differences in mz. This change led to, on
average, slightly lower scores. Furthermore, scaling was introduced for 
the outer distance. (see [here](#dqsb-calculation))

## Bin property tests:
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

### Two or more centroids of the same scan
This test is necessary because the peakfinder cannot utilise more than one 
centroid per scan. All bins where this applies are marked. Before finding
peaks, only the centroid with the highest DQSC is retained. For most 
datasets, the rate of bins with duplicates is below 0.1%, with many
affected bins being ones that contain a hundred or more centroids.

### Unbinned points within the critical distance 
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

### Signs of a halved peak
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

### Drastic mass shifts
A bin is considered to undergo a drastic mass shift if median and
mean in mz are more than two times the mean centroid error apart.
Bins display this behaviour if at the end of a bin, few points 
undergo a drastic increase or decrease in mz.
This test is not used for corrective action.

### Asymmetric distribution of mz
Assuming a normal distribution, 80% of points should be within the
1.3 sigma interval. 80% were chosen as the limit so that one point
of deviation is still within tolerance for bins of size 5.
This test is not used for corrective action.

### Maximum intensity at edge of bin
If the highest intensity in a bin is at the first or last centroid of
the bin, it can not be the apex of an interpolated peak. This goes
against the model behaviour of a correct bin.
This test is not used for corrective action, but could be employed 
as another sign of a halved peak.

### At least two possible maxima in intensity
If the highest and second-highest intensity in a bin are more than
three scans apart, more than one regression for this peak is expected.
If only one peak was found, this could justify a closer look at the bin.
This test is not used for corrective action yet.


## Discarded Criteria:

### point within maxdist but not within maxdist + 1
The test was removed due to not providing actionable, non-redundant
information. With the addition of a standard-deviation corrected
estimate for the critical distance, potentially interesting outliers
are already covered. The ones which aren't included are represented
through the DQSB.

### one-sided intensity profile
Originally, this test only checked if the maximum intensity of the
bin was also the first or last scan which is covered by the bin. 
This would prevent a gaussian fit with a maximum within the bin.
A related case, which relatively frequently occurred in bins without
positive tests, is the maximum being the second to first/last scan.
This is especially likely for small bins purely containing noise.
As such, the test condition was changed to the maximum being the smallest,
the smallest + 1, the largest or the largest - 1 scan.

### test for asymmetry in mz
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

### test for points outside three sigma interval
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
consistency or anything else. It is assumed that a relevant difference
in DQSF would be noticeably more strongly through the other observed peak properties.

## Validity of the identified peaks
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
regions of the scan which did not suffice for centroid representation
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
profile signal exists at the point


# Development of a consistency parameter from process statistics
When assessing the entire process from profile-mode spectra to 
components, it is important to find parameters that are responsive
to a broad number of deviations within the system. Multiple of the 
presented categories are highly selective regarding the types of
properties they give insight into, and as such not well-suited to
form the basis of a sum parameter for process quality.

Tested parameters:
* mean DQSC
* mean DQSB
* mean DQSF
* Number of centroids
* Number of bins
  * Number of bins with no features
  * Number of bins with one feature
  * number of bins with more than one features
* Number of features

The three quality parameters (DQSC, DQSB, DQSF) calculated during the
different steps of qAlgorithms describe the model consistency of that
step to a limited degree. When taking their mean value at the end of their
respective step, they can indicate large differences between datasets
and make a statement about when during processing such a shift took place.
Tests with different datasets showed that their correlation is inconsistent,
sometimes (anti)correlating strongly and other times being roughly orthogonal.
This implies that they contain information which can not be coerced into
one sum parameter without losing predictive power.

## Consistency of the centroiding algorithm

It was observed that, when ordered by time of recording, the differences
between scores of neighbouring measurements are close to zero for defect-free
measurement series. Two different series of similar samples which were
measured one month apart had very similar DQSCs, barring one exception.
DQSB and DQSF showed a similar jump in difference, but tend to have a higher 
base fluctuation in the used datasets.
In the tested data, the first measurement of the continued series had 
abnormally many spectra recorded. This lead to two noticeable changes
in DQSC (refer image). To use the DQSC as a consistency parameter, the
display algorithm would have to account for such outliers and add an
explanation for the operator to the marked region. The threshold for an
out-of-control situation should be dynamically generated out of the 
dataset. The greatest observed difference was 0.0017, which is ca. 2.5
times geater than the mean fluctuation. The standard deviation within
an outlier-free measurement series is 0.00036. The established three-sigma
interval for denoting outliers would be applcable to this use case,
and it is worth considering as the least sensitive stability measure
for a running system.

Similar effects can be observed when viewing the differences between
the total number of centroids per measurement. To display both graphs together,
they were normalised so that their greatest point, respectively, was 1.
The number of recorded spectra is inconsistent between measurements,
usually deviating by around four. Whether this had an influence on the
displayed results was tested by calculating the differences between
all points before and after normalising them to the number of recorded
spectra. Both series were normalised to their maximum value afterwards.
The greatest absolute difference between the two series was 1.1 10e-16.
This is within the floating point error, and both results can be
considered identical. The most visible difference to the DQSC variation
is that here, blanks do appear as a visible decrease.

The solution chosen for confidence limits uses a separately recorded
series of ten technical replicates. Their similarity is confirmed via
PCA before asserting them as the best-case scenario for a given set
of instrument conditions. 

The combined view shows a change in both parameters if the series changes,
both being largely constant within the group of known good measurements.
An increase in DQSC does not necessarily coincide with an increase in 
the centroid count. Within a series, some cases exist where only one of the
two experiences a major shift. Neither of the two has a consistently higher 
noise level. 

For an eventual user-facing implementation, blanks could be removed 
when displaying the differences in count.

## Consistency of the Binning algorithm

The consistency of binning should be evaluated without taking the 
consistency assessment during centroiding into account. This has the
advantage that it can still be used if the centroiding was performed 
using another algorithm, while also not amplifying potential distortions
introduced in the previous step.



[@todo] DQSB, DQSF

# Error traceability
An important part of communicating an error to the operator is to
establish complete traceability from the questionable result to
the raw data. This allows users to control the output with a 
different tool and, ideally, compare the original data which both used 
to generate their final result. qAlgorithms provides the user with
the option to view only those parts of the input data that was used
to construct peaks. @todo
This option also enables the processing of uncertain, but highly
relevant masses by means of especially computationally expensive
methods without demanding very powerful processors, a lot of
energy or a lot of time. 

# Viability test for simple process parameters
Tested parameters:
* mean DQSC
* mean DQSB
* mean DQSF
* Number of centroids
* Number of bins
* Number of Features

A sample dataset consisting of 32 different analysis runs with qAlgorithms
was assembled from six SFC-HRMS polarity switching measurements (12 processes),
six LC-HRMS blank measurements and 14 normal LC-HRMS measurements, two of
which were measured with negative polarity.

The process parameters were controlled for normal distribution using a one-sided
t-test with an alpha of 0.01. No parameter was normally distributed. 

A PCA grouped by polarity was performed to find relevant parameters for 
process performance. In both cases, the numbers of centroids, bins and features
were highly correlating. In the positive case mean scores for bins and
features correlated, which did not apply for negative measurements. For both,
DQSC was roughly orthogonal to DQSF. When using only nine measurements from 
the same series, the quality scores changed significantly.

The consistency of both the centroids to bin and the bin to feature ratio
could be a good criteria for finding process errors and a general measure
of effectiveness. When checking these parameters against the nine-sample
series, results were very consistent with the exception of one measurement. 
The deviant measurement was notable in that (roughly) the first 800 scans
contained a total of three centroids. It was taken as an example of an
undesireable measurement, which a given test would have to detect.
This finding was controlled against the measurements with polarity switching,
with the expectation that negative mode measurements would perform worse
than positive mode ones. 

For the criteria "centroids to bins", the deviant measurement had values
25% lower than the series. Negative mode scores were also consistently lower.
There were no significant differences within positive and negative series.
A third series, which was selected for containing many poorly separated mass
traces in the EIC, had generally higher values, but showed significant deviation.

For the criteria "bins to feature", the undesireable measurement had a significantly
greater (factor six) value than the rest of its series. Positive measurements in
the SFC data had slightly higher values than negatives, both showing little in-group
deviation. The third series had very high in-group deviance.

The criteria "centroids to features" displayed similar behaviour to "centroids to bin".
It is not considered further, since it depends on both other criteria being
applicable for its validity.

"centroids to bin" covers both the number of discarded centroids and the average
binsize. It measures the total data reduction as a result of binning. Preliminary
tests imply that it is largely dependent on system conditions, although more exhaustive
measurement series are needed here. 

"bins to feature" combines the number of bins with no features with the average number of
bins with one or more features. Here, the ideal number is one. A high number of features 
per bin implies generally poor separation of ion traces, and thus less reliable results.
The amount of bins without any features could function as an estimator for the noise
level in the measurement. While this needs to be validated, a temporally resolved noise
indicator could also serve a valuable role in combining different measurements. For
the present samples, bins which do not produce features are primarily found 
at low masses as well as the beginning and end of the measurement. This conforms
to general expectations.

The criteria of having close to one feature per bin, only counting bins that 
contain at least one feature, was additionally tested using data which was 
recorded during a highly unstable pressure profile in the LC. It is
assumed that this negatively effects all steps of data evaluation. This step was 
taken due to the previously used measurement now displaying a ratio close to one,
which contradicted the previously assumed behaviour. The data recorded during 
unstable pressure did have a high number of features being detected in each bin,
as well as a high degree of variance between individual measurements.
The SFC measurements in positive and negative mode had between one and 1.5 
features per bin. 
A relalted property is the fraction of bins which contain only one feature
in relation to bins which contain at least one feature. This is anticorrelated
with the above mentioned criteria for obvious reasons, but shows slightly higher
in-group deviation. This lowers its viability as a test parameter.

In terms of general utility, the two most promising tests for stability of
measurement conditions and process quality are 

Notably, blanks and samples produce very similar results for all criteria 
tested. This could be explained by noise making up the absolute majority
of signals, which implies these citeria only respond to irregularities
during chromatography.

When using the different quality scores for general assessments of data
quality over a dataset, for example choosing instrument settings on basis
of a design of experiment, it should be controlled that the differences
which form the basis for a decision are greater than the random deviation
which is present in a series of technical replicates.

# Further research
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

The implemented visualisation tools should be validated using realistic
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

# planned Expansions to qAlgorithms
Data visualisation is currently (19.08.2024) not implemented for qAlgorithms.
Furthermore, it does not have an internal representation of measurement 
series. This results in the user having to invest additional time to create 
such control charts and manually select which files to include.

Adding data visualisation could be handled through a third-party library
and would decrease the time and skill needed to use qAlgorithms in
this regard. While advantageous, defining out-of-control situations
and detecting them based on statistical approaches could remove the
need for human interaction in this regard, too.

Useful data visualisation requires implementing some way of declaring
a relation between different measurements by the user. Since such
a function is also required for componentisation and assigning MS2 spectra,
it should be the next step in expanding the project. 
Since DDA MS2 spectra will not be binned, separate methods of establishing 
process quality will have to be devised here. It could be sensible 
decision to include templates for common applications, such as monitoring
the stability of a system over time. This would ideally include a way to check 
the general system stability without re-processing potentially thousands of files again.

During development of this tool, some regressions covered less than five
real datapoints. While it is unknown what causes this issue at the moment,
it must be adressed. The root cause is possibly another inherent property
of the peak which could in turn be used to describe the overall data quality.
The detectable errors (as percentage of affected features) show no clear pattern
and deviate roughly as much within a measurement series as between series.
As such, they were not included as process parameters.
They seem to affect bins which contain multiple mass traces disproportionally
often when compared to other regressions. The effect is likely not bin-specific, since
good regressions also exist within the affected large bins.

# performed modifications to qAlgorithms 
## modified centroiding
An advantage of the qCentroids approach [@todo] is that peaks can also
be found if "gaps" exsist, meaning either through instrument behaviour
or a processing error no intensity was measured where it should have 
been. These missing points are interpolated, previously bridging gaps
of up to seven points. This accepted gap size has been lowered to three
points, since at a greater number of unknowns the risk of connecting two
unrelated peaks and lowering the overall data quality was too great. 
Accordingly, the greatest tolerated gap in scans within a bin has been
changed to three, which corresponds to a 12.5% chance of three real
signals being overlooked.

## DQSB calculation
Two elements of the DQSB calculation were changed:
* The calculation of the mean inner distance (MID)
* The scaling factor for minimum outer distance (MOD)
Previously, the MID was calculated per point by averaging the
distances from one point to every other member point of a given bin.
This was changed, since a large part of the bin which this distance
was calculated for will, in many cases, later be discarded by 
isolating the feature region. In order for the DQSB to better reflect
its later use case, now only those points within the maximum tolerated
distance (currently three, see above) are considered. This led to a slight
reduction in the average quality scores, although the exact effects still
need to be evaluated. 

With a similar consideration of the relevant scan range, now scaling is
introduced based on the likelihood with which a relevant point was not 
detected. In the worst case scenario, this results in a 50% likelihood
that the point was not measured erroneously [@qBinning]. To reflect this
behaviour, the MOD is doubled if it connects to a point with a one-scan
difference, quadrupled for a two scan difference etc.. A previously implemented
scaling approach doubled the MOD if it was to a point with the greatest 
possible scan distance and fell off to no increase in distance for a point
in the same scan. It was not extensively tested and discarded because the
reasoning behind it did not translate into assumptions of the grouping model.
If more lenient gaps are considered again in the future, this scaling 
will likely have to be reworked to not automatically exclude remote but
relevant points from influencing the DQSB.

An edge case was addressed which led to incorrect peaks being returned.
The error occurred when, during peak finding within a bin, the selected
centroids no longer fulfilled the statistical criteria for belonging to the
same mass trace. A test was added which controlled the maximum order space,
normalised to the standard deviation of masses within the peak, against the
critical distance as described for qBinning [@qBinning]. These Peaks are
now returned with a negative DQSF to indicate them being incorrect.

# Conclusion
It could be shown that, using exclusively very broad statistics generated 
as part of the qAlgorithms pipeline, some statements about sample similarity
are possible. This could be applied to monitor system stability, and 
methods for effectively visualising such changes were developed.


# Software and Data
All calculations in R were performed with R 4.4.0[@rcoreteamLanguageEnvironmentStatistical2023], 
using the packages tidyverse 2.0.0[@wickhamWelcomeTidyverse2019].
Images were generated using ggplot2 3.5.1[ggplot2].

qAlgorithms is written in base C++ 20, using only standard library functions[@ISOInternationalStandard].
For reading in files, the StreamCraft library is used[https://github.com/odea-project/StreamCraft].
It was compiled with gcc 13.2.0[@freesoftwarefoundationinc.UsingGNUCompiler1988].

UV degredation of ibuprofen with peracetic acid:
This dataset was measured while a pump defect on the LC caused the
column pressure to increase and decrease rapidly. It is used because
these conditions should cause the produced data to be no longer
binned reliably, leading to the majority of features being wrong.

SFC-HRMS data:
This dataset is from a design of experiment series trying out different
ion source parameters. The source was set to polarity switching, 
resulting in two datasets per measurement after evaluation.

Aquaflow data:[Non-target Analysis and Chemometric Evaluation of a Passive Sampler Monitoring of Small Streams]
Three blanks, three standards and three samples from the Aquaflow 
project, all measured in positive mode. One sample measurement
is notable for not containing any centroids for the first 800 scans.
Naming: C = control, T = treatment, S = sampling time
Another sample contained 5000 MS1 spectra, while usually only 1300 were recorded.

Other reference data:
Excerpt of a measurement series containing four blanks, and seven
ozonation experiments with indigo, two of which were measured in
negative mode. These measurements were primarily chosen for their 
small filesize and poor chromatographic separation.

PFAS data:
HPLC-orbitrap measurements

High-consistency standards
As a reference for a stable system, ten replicates of a standard mixture
in pure water were mesasured on an HPLC-orbitrap system. They were 
measured at the end of a longer run and are assumed to represent 
samples which do not significantly differ from each other in terms
of instrument conditions or sample content.