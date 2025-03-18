## Introduction

`qAlgorithms` is a standalone non-target screening analysis workflow for processing Liquid Chromatography
High-Resolution Mass Spectrometry (LC-HRMS) data. We focus on ensuring precise and reliable processing
in this complex field, while providing fast processing without depending on user parameters.

While `qAlgorithms` started as a library for processing, the high interdependency of every
step from profile mode mass spectrum to feature means it requires extensive work from the
end user to use only one "step" of the processing algorithm. As such, we decided to provide
a pre-compiled executeable with a low-complexity user interface.

In the spirit of open data evaluation, as little encapsulation as possible is used in the
source code itself. Functions are written with the goal of communicating the entirety of
their mathematical operations to the reader, provided he has some understanding of functions and structs.
We encourage you, the end user, to read our source code when working with `qAlgorithms`. This
increases the chance for errors to be found and keeps the project maintainable.

We are always open for suggestions and feedback regarding your useage of our algorithms,
so do not hesitate to open an issue on our github page.

Additionally, we are interested in measurement data that allows us to test our quality
parameters with some degree of confidence. If you have performed an experiment which
resulted in data that allows you to infer its general quality (on basis of the experimental
conditions) or measured a large amount of replicates of one sample, we would appreciate your 
support. You can contact us by opening an issue or sending a request to our [Zenodo community](https://zenodo.org/communities/nontarget). 
Here, we have also provided example files for trying out `qAlgorithms`.

## Installation and Usage
Please note that `qAlgorithms` is still in active development and result accuracy
cannot be guaranteed at this stage. 

### Windows
The entire `qAlgorithms` workflow is provided as an executable under ["Releases"](https://github.com/odea-project/qAlgorithms/releases) 
on our github repository. Note that `qAlgorithms` requires the libraries 
[`libgcc_s_seh-1.dll`](https://github.com/odea-project/qAlgorithms/releases/download/v0.1.1.beta/libgcc_s_seh-1.dll)
and [`libwinpthread-1.dll`](https://github.com/odea-project/qAlgorithms/releases/download/v0.1.1.beta/libwinpthread-1.dll). If they are not present on your system already, you can also download them under "Releases" 
or by clicking on the filenames above. There is no need to download the source code.

To run `qAlgorithms`, the three .dll files and `qAlgorithms.exe` must be in the same directory.

On windows, double-clicking the .exe will open `qAlgorithms` in interactive mode. Follow the prompts
in the terminal window to produce a feature list for the file or directory of .mzML files.
Exclusively use [ASCII characters](https://en.wikipedia.org/wiki/ASCII) in 
filenames. If a directory or filename has a space in it, you need to enter the absolute
path with quotes or escape the space character with "\\" to read in everything correctly.

However, we recommend all users to start `qAlgorithms.exe` using powershell, which is pre-installed on all windows PCs.
If you are unfamiliar with using the shell, refer to [the basic powershell demonstration for `qAlgorithms`](https://github.com/odea-project/qAlgorithms/wiki/Tutorial-for-Novice-Users).

You can also drag a directory into the powershell window to copy its path into the command line. 

### Linux / Mac
Currently, no Linux releases are provided. We recommend you to clone the repository
and compile from source using cmake and GCC.

On linux, you can use special characters like "µ" in filenames, provided your terminal supports
them. We use the [std::filesystem library](https://en.cppreference.com/w/cpp/filesystem), so you are limited by that.

## Usage

To use `qAlgorithms` for processing mass spectrometry data, you need to convert your
measurements into .mzML files, for example with [msconvert](https://proteowizard.sourceforge.io/) or [ThermoRawFileParser](https://github.com/compomics/ThermoRawFileParser). 
Currently, only MS1 data can be used, so you save some disk space if you filter them out at this stage.

`qAlgorithms` is a command line utility which reads mzML files and outputs them
as csv. You can select individual files or an entire directory to search for
.mzML files recursively. All output is written into one directory, which you also
must specify. Below are some commands you will likely use (replace example paths with your system paths):

`./qAlgorithms.exe -h` - Display the help menu, listing all availvable options.

`./qAlgorithms.exe -i C:/example/path/measurement.mzML -o ../my/results -printfeatures` - 
Process the file measurement.mzML and write a feature list with every detected peak
into the directory "results".

`./qAlgorithms.exe -i ./allMeasurements -o ./results -printall` - searches the directory
"allMeasurements" and all subdirectories for files ending in .mzML and process them.
All intermediate results, those being centroids, bins and peaks, are written to a .csv 
file and saved to the "results" directory. 

Some things to keep in mind:
* `qAlgorithms` can only process profile mode data at this point. While we did implement functionality
  for processing centroided data, the results are significantly less reliable. Additionally, we did not
  yet find a good process for estimating the mass accuracy of a given centroid for input from different
  mass spectrometers. As such, processing centroided data is only possible if you recompile the program
  yourself at this point in time (which would still lead to inaccurate results).
* Check out the [Wiki page](https://github.com/odea-project/qAlgorithms/wiki/Questions-regarding-the-use-of-qAlgorithms) for more details on using `qAlgorithms`.
* If you do not specify which results you want, no output will be written when using the command line interface.
* If the program crashes, check if your problem matches one of the known bugs on our
  [issues page](https://github.com/odea-project/qAlgorithms/issues). If a workaround exists,
  we will provide it there while a proper solution is being worked on.
* If multiple copies of the same file are found during recursive search, only one of them will be processed.
* The different quality scores do not serve as a way to remove peaks from
  your results. They only indicate how well the data at every step fit our
  model assumptions regarding the mathematical properties of real peaks.
  All peaks which are provided in the peak table are statistically significant.
  The best current usage for quality scores is priorisation of peaks 
  during further analysis.
* A negative DQSpeak means that the m/z given for that feature is more uncertain than the DQSpeak 
  implies. This does not mean that the feature does not exist, or that the calculated
  mass is inaccurate. While we were able to revise the binning algorithm to eliminate a majority
  of these cases, some remain. Until we have decided on a sensible way to handle these cases, this is given
  as a warning to not trust the mass of this peak. 
* Contrary to the original publication (see below) the DQSbin now ranges from -1 to 1. This was
  chosen to put additional focus on the case where a point is more similar to the point outside of
  the bin than the bin subset within a relevant number of scans. Additionally, bin scores are now only
  calculated from points that are not part of bins after the algorithm completes.


## Philosophy
Our core principles can be summarized as three "No"s: 
* **No User Parameters**
* **No Unclear Process**
* **No Usage Restrictions**

Unlike many traditional data processing tools, our algorithms do not rely on manual user input 
parameters such as thresholds. Instead, they intelligently leverage the inherent properties of 
the measurement data itself. This approach allows the algorithms to dynamically assess and 
utilize the quality of the data, ensuring robust and reproducible results every time.

The algorithms within `qAlgorithms` are rooted in well-established statistical 
tests. Our primary goal is to deliver results that aren't just accurate but also 
statistically significant, providing confidence in every analysis. 

We provide insight into the individual steps that led to a feature throug the use of quality
scores. Every score preserves information on one of the concrete steps that are necessary to
transform a series of mass spectra into a feature list. Every step is programmed to be clearly
separated in the source code, with the relevant procedures being as independent from each other as possible. This makes it
more feasible for a C++ literate user to understand the specifics of how a result came to be.

We are licensed under GPL-3, which means that there is no risk of you being unable to use `qAlgorithms`
even for commercial purposes. The current and all future versions are distributed with source code,
and you are free to include our libraries in other projects (provided the terms of the license are fulfilled).

Additionally, `qAlgorithms` is develped with computational performance as a core concern. With `qAlgorithms`,
we aim for the fastest time-to-insight possible. As scientists, we are dedicated to provinding a smooth
research experience with rich output data that fits even highly sophisticated questions in your non-target analysis.

We hope that by demonstrating the effectiveness of our approach, more software written by researchers 
for non-target questions will adopt or improve on these ideals. 

## Documentation
No documentation beyond the commented source code exists at this point. Please refer to the
publications linked below for details on how the algorithms function.

We provide documentation on what the output parameter for the respective files mean on our
(wiki page) [https://github.com/odea-project/qAlgorithms/wiki/Result-File-Guide]. (currently incomplete)

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
As the current end point of `qAlgorithms`, `qPeaks` uses a comprehensive peak model 
developed by Renner et al. [https://doi.org/10.1021/acs.analchem.4c00494] to 
identify peaks within the bins generated by qBinning. Every peak is statistically
significant, sidestepping the need for further filtering steps like a minimum
intensity requirement. The scores generated provide you with information about
how well every step of the process to your peak worked, and allow you to make
a statement about the confidence of your results. Like all parts of the `qAlgorithms`
project, `qPeaks` requires no user parameters.

## Development Roadmap
The next step for `qAlgorithms` is an integrated componentisation workflow. By leveraging the power of
our dynamic peak model, we can analyse features by shape similarity and adjust the regression to describe
multiple features at once. Following a successful implementation, we aim to provide a fully automated
strategy for combining replicate measurements.

Our goal is to provide a specialised, high-precision tool for analytical data processing
of HRMS data. All current and future additions to `qAlgorithms` are developed with the goal of reducing
the potential for human error and increasing result reliability.

---

**qAlgorithms** - Transforming data into insights.
