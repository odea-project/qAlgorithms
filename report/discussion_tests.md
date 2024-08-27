## Abbreviations and definitions:
[@todo] remove irrelevant entries
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
[@todo] move all definitions for within the workflow here

# Notes on the design of visual process quality estimators
An emerging and highly relevant problem in the field of NTS is
the fast and accurate estimation of total data quality or total
measurement quality through an operator. Especially for routine 
measurements, replicates and other series of very similar samples
this involves comparing the different steps of a series with 
each other, ideally over a large amount of samples at once.

These requirements place significant restrictions on the possible
ways to communicate the deluge of data that comes with (possibly)
years of measurements. Here, two types of approaches can be viable:
1) Machine-assisted or machine-led processing and search for regions of high change
2) A reductive approach that produces metadata which is still sensitive to shifts, but human-comprehensive

While these options are not exclusive, the final step of any
automated evaluation still needs to be that of communicating relevant
results to the instrument operator. 

## Differentiating process quality from result quality
Previous work by https://doi.org/10.1016/j.trac.2020.116063 highlighted
quality assurance as a core issue for nontargeted mass spectrometry and
presented different dimensions for quality during an analysis.
Expanding on the step of "result reporting", a distinction should be
made between reporting the entire measurement for purposes of replication
and controlling through peer review and the reporting of just the 
measurement results. @todo et. al. only consider reporting of the
results after interpretation, with raw data and the processing
steps being provided in addition. Besides being impractical to replicate
and thus very unlikely for this vital control to take place during peer 
review, it is not possible to estimate the algorithmic performance 
relating to result quality from just the algorithm on a per-sample 
basis accurately during NTA.

Result quality, as understood here, is the reliability and completeness
of all features at the end of any given processing workflow. For a
feature to be reliable, it has to correspond to a real molecule
that can be found within the sample in terms of mass and behaviour
in the chromatographic dimension or other dimensions of separation.
Furtermore, a reliable feature must conform to the MS2 spectrum,
if such a spectrum was produced by the instrument. After componentisation,
the source feature [@todo better name] should include only features
which have a high degree of certainty. The highest reliability in
a dataset is achieved if no false positives or negatives exist,
meaning the final results are a complete description of the sample contents.

Completeness is the degree to which all molecules in the sample that arrived at
the mass detector could be reconstructed from the final list of components.

Since both measures depend on ground truth, it is not always sensible [@todo] very similar measures, change
to provide an estimator for result quality. The central difference 
between results and process as proposed here is that results serve
as the basis for some decision, be it through a researcher deciding
on the next experiment, a water treatment plant worker taking preventative
measures against a detected contaminant or a sampling system repeating
an operation. In all such cases, the process does not factor into
the final decision, unless it is as an abstract measure of confidence.

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

### Current means of stability / quality monitoring
-consistency in mass accuracy of some internal standards
[https://www.sciencedirect.com/science/article/pii/S0165993621000236#bib63]
Caballero-Casero et Al. proposed guidelines on analytical
performance reporting in the context of human metabolomics.
The suggested criteria are sensitivity, selectivity, 
accuracy, stability and representativeness[]. All five largely
depend on internal standards, with the excpection of stability.
The authors propose hard limit values for these, 

### Validation criteria
For the purposes of this evaluation, measurements withing a
series are ranked based on their median DQSF. A characteristic
found to correlate with the median DQSF is considered to be an
indicator for that process to have performed well. [rework]

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

## Bin quality score (DQSB)
The DQSB in its current implementation [@todo] (paper binning, report 1) gives a measure of how well separated
a bin is from its environment by comparing the distances within a bin with
the distance to the closest point outside of it. In its original conception,
this only takes the retention time into account when searching for the closest
points not in the bin. To achieve greater locality of the score, the program
was modified such that only points within the maximal gap distance are used
to calculate the mean in-group differences in mz. This change led to, on
average, slightly lower scores. Furthermore, scaling was introduced for 
the outer distance. (see [section])

# General Notes on Tests - Binning:
The tests presented here were developed as assessors for the quality
of the binning process. As such, they do not intend to make predictions
on the final peak quality, if a peak will be found, or how 

# Discarded or Modified Criteria:

## point within maxdist but not within maxdist + 1
The test was removed due to not providing actionable, non-redundant
information. With the addition of a standard-deviation corrected
estimate for the critical distance, potentially interesting outliers
are already covered. The ones which aren't included are represented
through the DQSB.

## one-sided intensity profile
Originally, this test only checked if the maximum intensity of the
bin was also the first or last scan which is covered by the bin. 
This would prevent a gaussian fit with a maximum within the bin.
A related case, which relatively frequently occurred in bins without
positive tests, is the maximum being the second to first/last scan.
This is especially likely for small bins purely containing noise.
As such, the test condition was changed to the maximum being the smallest,
the smallest + 1, the largest or the largest - 1 scan.

## test for asymmetry in mz
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

## test for points outside three sigma interval
This test was implemented to identify those bins that
were too broad (in mz) to form visually identifiable ion traces.
It was removed due to outliers not directly corelating with
good bins - often, points with very low intensity at the edges of
the relevant scan interval triggered the detection.

# Bin Categories:

## Bin with duplicate scan:
These bins should not be passed to the centroiding algorithm.
The information that a duplicate was included is passed with the
errorcode, while the centroid with the higher DQSC is retained.

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
[@todo] DQSB, DQSF


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

# Software and Data
All calculations in R were performed with R 4.4.0[@rcoreteamLanguageEnvironmentStatistical2023], 
using the packages tidyverse 2.0.0[@wickhamWelcomeTidyverse2019].
Images were generated using ggplot2 3.5.1[ggplot2].

qAlgorithms is written in base C++, using only standard library functions[@ISOInternationalStandard].
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