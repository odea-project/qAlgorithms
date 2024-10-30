# Abstract Master Thesis

Contaminants of emergent concern threaten the quality of drinking and surface water worldwide.
Often, these substances are not described, which means they cannot be reliably measured 
with conventional analytical approaches. To this end, non-target screening
serves as a methodology through which a large amount of unknown compounds can 
be detected and responded to without indentification. Many challenges remain unil
non-target analysis is easily reproducible applicable without expert knowledge, 
especially regarding the data processing in NTS. Processing consists of a series of algorithms
which aim at removing chemical and instrument noise from measurement data until
only that information which is related to compounds within the sample remains, all while
minimising the amount of falsely removed signals. The process of feature list construction
has already been implemented within the qAlgorithms project. A feature describes
a signal whose intensity follows a gaussian shape in the time dimension.

Componentisation is the process by which features in a feature list acquired
through non-target screening are grouped by their analytical behaviour.
Ideally, all features within a component originate from the same
chemical compound found in a sample. As such, it adds chemical information
into the feature list and helps reduce the data complexity caused by in-source fragmentation.

Multiple independent groups in the fields of metabolomics and environmental science 
developed and sucessfully employed different approaches for grouping feature 
lists within one measurement, between different measurements, or both. These often
rely on user-supplied parameters or are otherwise not consistent between labs,
for example by depending pre-trained models for machine learning solutions.

The goal of this thesis is to develop an approach to clustering feature 
lists which operates without user parameters, runs on a consumer laptop 
and provides a measure of certainty for every grouping.
The program will be written in C++ and directly integrated with qAlgorithms.
This also means that executing other componentisation systems, which are often 
written in interpreted languages like R, MATLAB or julia, is not possible while 
keeping the program maintainable for research and industry use. An additional
functionality which does not exist in algorithms developed thus far is the delayed
resolution of uncertain component assignments at a later point in the program.
For example, if a feature could be assigned to two different groups, this decision
is only made when comparing multiple measurements by choosing the arrangement which
maximises replicability, provided the base statistical requirements are met.

To compare the new componentisation with other tools a measurement series with
increasingly strong coelution of different compounds will be measured. All steps up to
the feature list will be performed by qAlgorithms. The generated
data will be archived on the institute server and the most relevant test series uploaded
to a data archive with comprehensive descritions regarding the experimental procedures.
Links to these datasets will be published to the qAlgorithms github page and distributed 
with the source code. The comparison will focus on how reproducible groupings are in
relation to the developed quality score.

The task of remaining completely parameter-free and providing the novel strategy
of resolving uncertainties only when reaching a point where a decision is necessary
provides an answer to relevant issues in the field of non-target screening and
expands the space of what current analysis tools can do by a new concept to 
approach componentisation problems with. Such new tooling stands to benefit
all potential and current users of non-target screening. Should the processing
give new insight into how a good componentisation differs from a bad one in terms of
the precise calculation steps taken to produce it, existing algorithms could be
better compared. The open nature of the GPL3-licensed qAlgorithms codebase also 
enables anyone to use the supplied project as a starting point for other,
unaffiliated componentisation strategies or to improve existing ones.
