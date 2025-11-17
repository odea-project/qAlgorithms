## Usage Notice
At this moment, `qAlgorithms` is provided purely for reasons of scientific transparency. There are
still known flaws in the concrete implementation, and we are steadily working on finding a useful
way to vaidate the rest in a sensible manner. For this reason, **you should not use `qAlgorithms`
to answer your research questions** (yet). 

## Introduction

`qAlgorithms` is a standalone non-target screening analysis workflow for processing Liquid Chromatography
High-Resolution Mass Spectrometry (LC-HRMS) data. Our goal is to provide a solution to single-file 
processing pipeline that does not require any additional user input, delivers error estimates for results
and is very fast / computationally efficient.

While `qAlgorithms` started as a library for processing, the high interdependency of every
step from profile mode mass spectrum to feature means it requires extensive work from the
end user to use only one "step" of the processing algorithm. As such, we decided to provide
a pre-compiled executeable with a low-complexity user interface. It is a low-priority goal of
ours to provide library bindings to more widespread programming environments like R and python.

We are always open for suggestions and feedback regarding your useage of our algorithms,
so do not hesitate to open an issue on our github page. You can also find us on [Codeberg](https://codeberg.org/ODEA-Project/qAlgorithms).

Additionally, we are interested in measurement data that allows us to test our quality
parameters with some degree of confidence. If you have performed an experiment which
resulted in data that allows you to infer its general quality (on basis of the experimental
conditions) or measured a large amount of replicates of one sample, we would appreciate your 
support. You can contact us by opening an issue or sending a request to our [Zenodo community](https://zenodo.org/communities/nontarget). 
Here, we have also provided example files for trying out `qAlgorithms`.

## Installation and Usage

### Windows
The entire `qAlgorithms` workflow is provided as an executable under ["Releases"](https://github.com/odea-project/qAlgorithms/releases) 
on our github repository. **There is no need to download or compile the source code.**

On windows, double-clicking the .exe will open `qAlgorithms` in interactive mode. This mode
is extremely primitive and not officially supported. 
We recommend all users to start `qAlgorithms.exe` using powershell, which is pre-installed on all windows PCs.
If you are unfamiliar with using the shell, refer to [the basic powershell demonstration for `qAlgorithms`](https://github.com/odea-project/qAlgorithms/wiki/Tutorial-for-Novice-Users).

To build from source, follow the instructions for linux after installing [mingw](https://www.msys2.org/). After installation, run the command
```sh
pacman -Syu mingw-w64-x86_64-zlib
```
from the installed UCRT environment. 

### Linux / Mac
Currently, no Linux releases are provided. We recommend you to clone the repository
and compile from source using CMake and GCC. Since we do not depend on anything besides
zlib, GCC and cmake being installed, we consider this a reasonable policy.

We require CMake 3.25 or later and GCC 15 or later.

Build `qAlgorithms` by executing these commands:
```sh
git clone https://github.com/odea-project/qAlgorithms.git
cd ./qAlgorithms
mkdir build
cmake -S . -B ./build
cd ./build
cmake --build . -j
```

## Usage

To use `qAlgorithms` for processing mass spectrometry data, you need to convert your
measurements into .mzML files, for example with [msconvert](https://proteowizard.sourceforge.io/) or
[ThermoRawFileParser](https://github.com/compomics/ThermoRawFileParser). 
Currently, only MS1 data can be used, so you save some disk space if you filter them out at this stage.

You can find pre-converted files which are confirmed to be processed correctly [here](https://zenodo.org/records/14046512).

`qAlgorithms` is a command line utility which reads mzML files and outputs them
as csv. You can select individual files or an entire directory to search for
.mzML files recursively. All output is written into one directory, which you also
must specify. Below are some commands you will likely use (replace example paths with your system paths):

Display the help menu, listing all availvable options:
```sh
  ./qAlgorithms.exe -h
```
Process the file measurement.mzML and write a complete feature list 
into the directory "results":
```sh
  ./qAlgorithms.exe -i C:/example/path/measurement.mzML -o ../my/results -printfeatures
```
searches the directory "allMeasurements" and all subdirectories for files ending in .mzML and process them.
All intermediate results, those being centroids, bins and features, are written to a .csv 
file and saved to the "results" directory:
```sh
  ./qAlgorithms.exe -i ./allMeasurements -o ./results -printall
```

Some things to keep in mind:
* `qAlgorithms` can only process profile mode data at this point. We rely on uncertainty data 
  generated during centroiding for following steps and have not found a way to esitmate them so
  far. The ability to process centroids will be added once that problem is solved.
  
* Check out the [Wiki page](https://github.com/odea-project/qAlgorithms/wiki/Questions-regarding-the-use-of-qAlgorithms) for more details on using `qAlgorithms`.
* If you do not specify which results you want, no output will be written when using the command line interface.
* If the program crashes, check if your problem matches one of the known program errors on our
  [issues page](https://github.com/odea-project/qAlgorithms/issues). We will not fix known 
  problems before the program is considered to be stable.
* If multiple copies of the same file are found during recursive search, only one of them will be processed.
* The different quality scores do not serve as a way to remove peaks from
  your results. They only indicate how well the data at every step fit our
  model assumptions regarding the mathematical properties of real peaks.
  All peaks which are provided in the peak table are statistically significant.
  The best current usage for quality scores is priorisation of peaks 
  during further analysis.

## Design Philosophy

The algorithms within `qAlgorithms` are rooted in well-established statistical 
tests and employ standard linear regression as the main problem solving strategy.
This allows us to be fully deterministic without requiring the user to supply 
and optimise algorithm parameters, which is a time-consuming and error-prone
process even for domain experts.

While this design allows users to treat processing as a "black box", we always
provide additional data which communicates which points a feature actually covers
and data quality scores, which summarise the confidence in a step as a number between
zero and one.

If you want to go one step further and read out code, it is organised such that almost
all mathematical operations related to a procedure are localised within that procedure.
Since code readability is only tested by a very limited number of individuals, we also
appreciate questions and comments in that regard.

We are licensed under GPL-3, which means that there is no risk of you being unable to use `qAlgorithms`
even for commercial purposes. The current and all future versions are distributed with source code,
and you are free to include our libraries in other projects (provided the terms of the license are fulfilled).

Additionally, `qAlgorithms` is develped with computational performance as a core concern. With `qAlgorithms`,
we aim for the fastest time-to-insight possible. If a long time is spent waiting for results, 
the entire process is more prone to insufficient validation and more sensitive to potential
hardware failures. 

The choice of language being C++ is so for two reasons: Firstly the mentioned desire for fast
processing, secondly the need to write software for NTS in a manner that does not need 
maintainance besides occasional recompilation.
The use of "exotic" language features is kept to a minimum and largely limited to precomputation.

We hope that by demonstrating the effectiveness of our approach, more software written by researchers 
for non-target questions will adopt or improve on these ideals. 

## Documentation
No documentation beyond the commented source code exists at this point. Please refer to the
publications linked below for details on how the algorithms function.

We provide documentation on what the output parameter for the respective files mean on our
[wiki page](https://github.com/odea-project/qAlgorithms/wiki/Result-File-Guide) (currently incomplete).

## Current Offerings

### qCentroids Algorithm
`qCentroids` by Reuschenbach, Renner et al. [https://doi.org/10.1007/s00216-022-04224-y],
has been superceded by a modified variant of `qPeaks`. Using the new peak model, even
more centroids can be described accurately. The core goal of providing a reliable centroiding
algorithm without user parameters remains fulfilled.

### qBinning Algorithm
The `qBinning` algorithm utilises the centroids generated by `qCentroids` to
produce extracted ion chromatograms. Like `qCentroids`, it requires no user
parameters. Binning allows you to reduce the amount of centroids considered
in future analysis by roughly 30%. The current `qBinning` program is based
on the algorithm presented by Reuschenbach, Renner et al. [https://doi.org/10.1021/acs.analchem.3c01079],
but implements additional steps for finding the highest amount of statistically
sound bins. Additionally, the prediction interval is used instead of the mass error.
The function fitted to the simulated data has also been changed to provide even higher accuracy.
For details on the implementation, refer to `src/qalgorithms_qbin.cpp`.

### qPeaks Algorithm
`qPeaks` uses a comprehensive peak model 
developed by Renner et al. [https://doi.org/10.1021/acs.analchem.4c00494] to 
identify peaks within the bins generated by qBinning. Every peak is statistically
significant, sidestepping the need for further filtering steps like a minimum
intensity requirement. The scores generated provide you with information about
how well every step of the process to your peak worked, and allow you to make
a statement about the confidence of your results. Like all parts of the `qAlgorithms`
project, `qPeaks` requires no user parameters.

### qPattern Algorithm
The `qPattern` algorithm (as of now unpublished, name subject to change) is a newly
developed componentisation strategy to group features produced with qPeaks. It uses 
linear regressions to estimate shape similarity of features and group related features
into components. 

## Development Roadmap
We are consistently optimising and expanding `qAlgorithms` to meet the evolving needs of data analysis
in non-target screening applications. 
Our goal is to provide a specialised, high-precision tool for analytical data processing
of HRMS data. All current and future additions to `qAlgorithms` are developed with the goal of reducing
the potential for human error and increasing result reliability.

---

**qAlgorithms** - Transforming data into insight.
