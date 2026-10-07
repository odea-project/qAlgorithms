# Elevator Pitch for qAlgorightms
(assuming a moderately long elevator ride)

---
## Disclaimer
qAlgorithms is in active development and requires intermediate knowledge of the
C++ programming language to be used. A version that is accessible to 
non-programmers will only be provided once we have high confidence in the
correctness of the produced data. If you are interested, get in touch via github:
https://github.com/odea-project/qAlgorithms


## Problem Statement
Currently (Anno Domini 2026), most established NTS data-processing 
workflows include a step that falls into the category "feature detection".
In this step, a set of data points is summarised to one point with the
dimensions mass-to-charge ratio **m/z**, retention time **RT** and the
signal intensity. This summary is practical in doing further data 
processing, but only sensible to perform if all points included within
a feature are all part of the same chromatographic profile, or *peak*.
In its most general form, such a *peak detection* algorithm can be expressed as:

```
1.1) Identify a region with stable m/z values in the data
1.2) Assume m/z is the same for all points

2.1) taking RT as x-axis and intensity as y-axis,
2.2) identify all peak shapes in the data and mark the apex
2.3) For every apex, determine the intensity and produce one feature

3.1) remove all points processed in 2.1 from the data
3.2) continue at 1.1 until no points are left to process
```

However, a number of publications have demonstrated that the specific processing
tool used has a major influence on the detected features. This effect comes from
both different choices in how data is processed between tools and different values 
for the user parameters set by the operator based on observed results. This means the 
results of data processing depend strongly on **how** and **who**, even if the
exact same measurement file is processed. Obviously, this makes it difficult
to compare the results of non-target screening between labs.


## Solution Approach
With qAlgorithms, the idea is to address the problem of result variability to the
most complete extent possible. Firsty, the user of the software is no longer 
responsible for setting optimal parameters. Instead, the limit values normally
determined through these parameters are replaced by established statistical 
methods and operate with complete determinism. Where required, decisions are made
using the battle-tested approach of accepting a $`5\%`$ false positive rate for 
elimination ($\alpha = 0.05$). To give a concrete example, where usually a fixed
or relative minimum intensity is used to filter feature candidates, qAlgorithms
test whether a peak candidate is significantly different from the baseline. 

All similar tests and decision mechanisms within qAlgorithms are based on a subtle 
change in the problem statement: Instead of asking if a peak is present, we determine
how well a certain region of the data can be described as a peak. The decision if
a certain feature should be included in the final feature list is delayed until
the best possible description can be evaluated, leading to more robust results.

As a consequence, we gain the ability to produce comprehensive uncertainty 
information for our users. Such data could also be utilised by other tools
that refine information within feature lists towards allowing for decision-making,
specifically priorisation. Maybe more importantly, it also makes the software 
trivial to use. By removing parameter selection entirely, the program can
operate seamlessly as part of a larger, autonomous software system. The data
processing as implemented for qAlgorithms can be thought of as a mathematical
operation like taking a sum or the logarithm. The log-function is a fitting 
analogy in another context: Initially, log-tables were published as books
since they are time consuming to calculate by hand and this operation is too 
difficult to perform for a layman. Equivalently, qAlgorithm removes the 
need to define what a fit being "good" means concretely by adjusting program
parameters and gives the user a simple table with the final results. Expanding
one step, a layman is not able to do much with a logarithm table by itself.
It takes a trained scientist or engineer to make use of the knowledge, which
will likely remain true for non-target screening over the coming decades, too.


## Holistic User-Friendlines
As established, qAlgorithms does not require a user to use *directly*. This
allows it to be user-friendly in many ways that commonly, scientific software
is not. Importantly, obtaining the software is simple and highly future-proof.
Nearly all source code required to produce a working program is provided, 
meaning the final executable depends on exactly two software libraries being
installed on the system. The approach of minimal dependencies has two big
advantages:
* It will always be possible to produce a version of qAlgorithms that is 
  compatible with current and future computers running Linux, Windows or 
  an Apple operating system.
* Since it does not depend on code that is not provided as part of qAlgorithms,
  a "supply chain attack", where a cyber attack is made possible because
  a user-installed, uncritical program required a software library which 
  had been compromised in the meantime, is prevented by definition. 

Such guarantees on long-term functionality are difficult or impossible to
make with most scientific software, which is written in python or R and
requires many different third-party libraries. Ensuring that functionality
will not decay without the source code changing further makes adoption 
of qAlgorithms less expensive on a medium to long time frame. Where it might
otherwise be necessary to employ a professional or find a volunteer that
will keep the program up-to-date, a hypothetical compatibility
issue will always be solvable by merely reinstalling the program.

Friendliness towards non-academic end-users in particular is further 
provided by every piece of non-original code being provided under a known 
open-source license and every original piece of code being provided as
**AGPL**, which is a strong copyleft license that ensures that any future
versions of qAlgorithms will also be fully free software.

Especially with the ever-increasing need for a digital laboratory, another
type of user we aim to be friendly towards is a programmer who is
interested in incorporating qAlgorithms into a larger piece of software,
the details of which cannot be known in advance. Here, the qAlgorithms
software library (which is also used in the standalone program) aims
to be maximally compatible by providing its functionality in a way that
is very simple to include from all widely-used programming languages.
This design extends to more fine-grained functionality than just reading
and processing a file. A programmer can use just the function that find
peaks in data (which is used by the main program) or even just the 
function that produces possible regressions from data (which is used by
the peak finder). To facilitate this, the source code is well-documented
at the points that are relevant to a programmatic user.

Lastly, qAlgorithms is written with processing speed as a priority 
secondary to correctness. Besides reducing wait times when processing 
data, this benefits users by reducing the required hardware for
using the software. Any PC made after 2018 should be able to process
a standard mzML file of ~0.5 GB within seconds.
