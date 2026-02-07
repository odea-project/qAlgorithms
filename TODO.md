# planned actions for the qAlgorithms project

The todos are roughly organised by category. Adding more and more 
detailed todos, or removing outdated ones, is also a todo item.

Abbreviations (sorted alphabetically):
* LoC - Lines of Code
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

## Expansion
Additional functionality of the core library that should be added at some point.

* Support fragment spectra
* fully functional componentisation
* component-based matching of repeat injections
* automated estimation of the upper limit of the regression search window
* conceptual compatibility with retention indices for comparing multiple injections
* expose core functions to R, python and other languages (java / C# for compatibility with openchrom and mzmine)
* add post-hoc correction for binning falsely separating mass traces
* design a qAlgorithms-specific binary storage format for mass spectra 
* deconvolution of (probable) isomeric compounds forming double-peaks 
* include the pressure profile and other column parameters as data for describing a feature
* devise a better quality score for binning
* add functionality to read vendor formats
* work with already centrodied data as input (mass error estimation)
* RT distance harmoniser should work within a specific window if the distance changes at one point in the measurement (make it local to every bin? Check potential performance impact)

## Testing
Any code that is not run when processing data and concerned with logical correctness of the program.

* Formulate precisely how to test functions with non-trivial input data
* rework tests to use randomised inputs where sensible
* add a system that standardises how to test a pre-supplied peak
* add tests for all used functions
* find data input that only fails in one of the statistical plausibility tests for all these tests
* refine assertions used in the code

## Algorithm Characterisation
The goal of this is to describe correctness based on input data for a generic peak detection algorithm.

* Implement many of the functions used to describe peaks (https://doi.org/10.1021/ac970481d, https://doi.org/10.1016/S0021-9673(01)01136-0)
* Research if such a system already exists
* write documentation for unpublished parts and more exotic bits, like separate handling of RT

## PINTS
Todos related to the Pipelines In NTS initiative.

* Authorship tool - input names / classifiers of the programs used and get a bibtex file of the complete relevant references
* (related) central catalog of existing software tools for NTS
* Output feature list into a standardised database

## Other
Other todos.

* central repo that catalogs all published (raw) data related to NTS
* plotting utility directly in qAlgorithms (use raylib?)
* marketing materials (website?)