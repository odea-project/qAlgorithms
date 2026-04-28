# planned actions for the qAlgorithms project

The todos are roughly organised by category. Adding more and more 
detailed todos, or removing outdated ones, is also a todo item.

Abbreviations (sorted alphabetically):
* LoC - Lines of Code
* MS - mass spectrometry
* NTS - Non-Target Screening
* RT - Retention Time 
* SoA - Structure of Arrays

## Refactoring
Reduce code complexity of existing modules and find generic operations that
could be moved into a more generic library for mass spectra processing.

* clean up the main loop to be less LoC
* extract logging into its own code blocks throughout the project
* change binning to utilise an index vector and use arrays per property as centroiding output
* generic print method for centroids / bins / features, move all three to SoA
* full rewrite of componentisation
* add more raw formats besides mzML (mzXML is similar, that should have priority)
* good isolation of core functionality such as peak detection and binning
* find a way to multithread parts of the application
* extract the long matrix multiplication chains where present and show the concrete calculation in the code as a comment
* improve file reading performance, check if https://doi.org/10.1371/journal.pone.0125108 can be used partially or fully
* general performance improvements
* rework code so that the warnings -Wdouble-promotion, -Wconversion and -Weffc++ can be turned on without causing too much noise
* ensure clang-tidy conditions are fulfilled
* ensure uniform terminology throughout the codebase (keep a record of correct terms somewhere?)
* remove the RT transform mess currently implemented in favour of the regression-local delta_x estimation introduced in the retransformPeaks function
* Optimisation: a lot of time is spent computing exponentials. Identify where these are performance-
critical and check if they can be replaced with a less accurate estimation, ex. https://github.com/nadavrot/fast_log
* Optimisation: Ensure that all large computation chains of exponentials / logarithms are vectorised
* Cleanup: Large parts of the code just pass array pointers downwards, make that part of the code nicer using
scratch spaces or similar techniques of avoiding a lot of malloc/free
* check if special functions as implemented here (http://ab-initio.mit.edu/faddeeva/) are faster 
without being less accurate and replace the currently used slow functions if possible
* use the PeakFit struct and generic additional data instead of having two separate peak output models for centroids and features 
* fully separate filtering based on critical test statistics from the calculation of these statistics for all regressions
* Binning: make algorithm more flexible by sorting by mass and then only binning the first n points initially. (how is n determined?)
Then, return all bins with points where the cutting point is within m/z uncertainty (or uncertainty * 2?) to the unbinned values, extract EICs
and discard noise. Repeat until all points have been binned. Safeguard against very long traces: always take n new points (check if possible first)

## Expansion
Additional functionality of the core library that should be added at some point.

* Support fragment spectra
* fully functional componentisation
* component-based matching of repeat injections
* automated estimation of the upper limit of the regression search window
* conceptual compatibility with retention indices for comparing multiple injections
* add post-hoc correction for binning falsely separating mass traces
* design a qAlgorithms-specific binary storage format for mass spectra 
* deconvolution of (probable) double-peak systems like isomeric compounds or other strong coelution of similar masses 
* add functionality for extracting pressure, UV, temperature from the mzML files
* include the pressure profile and other column parameters as data for describing a feature
* devise a better quality score for binning
* add functionality to read vendor formats
* work with already centrodied data as input (mass error estimation)
* RT distance harmoniser should work within a specific window if the distance changes at one point in the measurement (make it local to every bin? Check potential performance impact)
* consider adding the artifact removal presented here: https://chemrxiv.org/doi/pdf/10.26434/chemrxiv.10001734/v1
* add a function to calculate an exact fit using the model and the complete x axis via the pseudoinverse (fast matrix: https://gist.github.com/nadavrot/5b35d44e8ba3dd718e595e40184d03f0)
* write output data of centroiding to mzML (centroids only, introduce as dedicated option) and other MS formats
* add support for multithreading
* peak parameters: theoretical plate height
* Function that takes a feature and file path as input and produces only the profile points / centroids specified by the feature
* comparison of peaks: can the F-test be used with the different number of points as degrees of freedom for the model? In the current system of comparison, the bigger regression is generally preferred even if it incorporates too much baseline
* add pre-filled error reporting template that is given to the user when the program fails to encourage bug reporting -> depends on working logger
* different binning approach growing outward from high intensity signals, but still utilising the order space. Context: We expect the mass spread to be larger when moving towards the edges of a given signal. This could also be useful when trying to bin data without known mass uncertainty (?)

## Interface to high-level languages
* Expose core functions to R, python, java, julia (?), C# (?), Matlab <- this is relevant because many companies only allow matlab
* potentially also add an interface for openMS
* Function: qpeaks_find(): Basic functionality already implemented, takes a single contiguous spectrum and returns all peaks therein
* Function to process a single spectrum with non-equidistant x axis and potential gaps
* Function to process a complete file based on a filepath and returns a feature list
* integration of full qalgo functionality to patroon
* (maybe) add qalgo to openchrom and mzmine

## Testing
Any code that is not run when processing data and concerned with logical correctness of the program.

* Formulate precisely how to test functions with non-trivial input data
* rework tests to use randomised inputs where sensible
* add a system that standardises how to test a pre-supplied peak
* add tests for all used functions
* find data input that only fails in one of the statistical plausibility tests for all these tests
* refine assertions used in the code
* check that apex is close to local maximum in data
* also test against Voigt-function as it approaches the lorentzian function for peak shape

## Algorithm Characterisation
The goal of this is to describe correctness based on input data for a generic peak detection algorithm.

* general writing of documentation
* Implement many of the functions used to describe peaks (https://doi.org/10.1021/ac970481d, https://doi.org/10.1016/S0021-9673(01)01136-0)
* Research if such a system already exists
* write documentation for unpublished parts and more exotic bits, like separate handling of RT
* expand documentation for qPeaks, remember to also document the kernel for matrix expansion

## PINTS
Todos related to the Pipelines In NTS initiative.

* Authorship tool - input names / classifiers of the programs used and get a bibtex file of the complete 
relevant references. This should be similar to the 'citation()' function in R, but have it open a pop-up
window with a "click to copy to clipboard" button. CLI only suffices for a proof-of-concept.
* (related) central catalog of existing software tools for NTS
* Output feature list into a standardised database

## Other
Other todos.

* central repo that catalogs all published (raw) data related to NTS
* plotting utility directly in qAlgorithms (use raylib?)
* marketing materials 
* Finish website (https://odea-project.org/)
* Feedback form for bug reporting on website
* Raw data viewer that isn't seeMS, using implot (https://github.com/epezent/implot/)
* own raw data conversion utility