# Detailed Explanation of the qPeaks Algorithm

## Motivation
Like the entire qAlgorithms project, qPeaks is primarily motivated from the desire
for a fully (user)parameter-free processing workflow for non-target screening.
Our model workflow centroids profile-mode MS1 data, groups it into EICs, and then
finds features within the EICs. qPeaks is employed for both centroiding and feature
detection. In addition to the peak coordinates, quality metrics for further processing
of the peak are returned by the implemented algorithm.

## Short Description
qPeaks is a linear regression for the peak model described [here](./moments_peakmodel.md).
It pre-calculates the pseudoinverse of a generic design matrix for computational efficency,
which allows many regressions to be performed in sequence. These are then filtered through
statistical tests for validation and finally, conflicting peak descriptions are resolved.
The final peaks are described by position, height, width and area.

## Processing Stages

### Stage 0: Prerequisites
Any data that is used with qPeaks must fulfill the following criteria for the algorithm to function correctly:
* The data must be present as a set of two-dimensional points
* Expected peaks must be roughly gaussian-shaped, asymmetry can be modeled
* The x-axis must consist of equidistant points for the width of a peak
* All values of y must be greater than zero
* If a relevant baseline exists, it has to be substracted before applying qPeaks

Note that the "y > 0" condition is mostly based on the fact that instruments will
generally use zeroes to separate blocks internally. Since the sensitivity is never
high enough to fully detect all ions of a peak, including these zeroes during 
processing will lead to the peak being distorted. Something to keep in mind is that
this results in the measured points, especially during centroiding, not including
the full signal. This also means that the ground-truth peak area will be higher
than that obtained from naively taking the area under the measured points.

The axis used for later transforms is calculated for all integer values between the
maximum scales (-scale to +scale). For the used peak model, this results in a design
matrix `X` with four columns. The first represents the intercept and is one (`x^0`)
for all values. The second is the x-axis. The quadratic coefficent is only defined
for `x < 0` for b2 and `x > 0` for b3 (`x = 0`)

### Stage 1: Data Transform
Initially, the y-axis is log-transformed using the natural logarithm. This is so
that the exponential function can be approximated using a quadratic model. The 
x-axis is also transformed, although this is not a visible mathematical operation.
Under the assumption that all x are equidistant, we can divide every x by this distance
to recieve an axis where all numbers are integer values with the distance one to each other.
Additionally, the degrees of freedom each point has are stored in an array with the same
length as x and y. If a point was interpolated to ensure equidistance, it has zero degrees
of freedom. Conversely, a point could have more than one degree of freedom if it represents
more than one spectrum.

### Stage 2: Moving Regression
The regression is performed stepwise in a nested loop. For every point in x, a linear regression
is performed with that point set to x = 0 relative to the precalculated matrix. This
results in no regression being performed for the outermost two points on each side, since the 
smallest defined matrix requires x to range from -2 t0 2. This distance to 0 on the x-axis is 
called "scale". The smallest possible scale is 2, while the largest possible scale is defined
as a constant of the algorithm. This is purely for reasons of computational practicality, in
theory the upper limit is `(length(x) - 1) / 2`. A convolution with the precalculated matrix
is performed to obtain four regression coefficients describing the peak. The last two coefficents,
b2 and b3, both describe the quadratic term. At x = 0, the peak switches from being defined as
`exp(b2 x^2 + b1 x + b0)` to `exp(b3 x^2 + b1 x + b0)`. Regressions are stored with coefficients,
scale and position of x = 0 relative to the first element of the x-axis.

Since the regression is performed on log-transformed data, the height is adjusted using the
untransformed values of y. Since b1, b2 and b3 are not affected, this can be solved with
linear regression.

### Stage 3: Statistical Plausibility Tests
All regressions are individually checked for plausibility using established statistical tests.
Where logical impossibilities are evident (example: the peak has no apex), no tests need to be
performed. The tests are set for an alpha of 0.05 by default, which cannot be adjusted in the 
present implementation.

In order of execution, the employed checks are:
* is the apex of the peak defined?
* does the peak have at least five degrees of freedom?
* does each peak halve have at least two points?
* is the description of the data as just a straight line preferable (using the F-test)?
* are b2 and b3 significantly different from 0?
* is the area statistically significant?
* is the height statistically significant?
* chi-squared test for goodness of fit

### Stage 4: Optimal Regression Selection
Since for a well-described peak, multiple regressions will be defined, the optimal description of a 
data region has to be selected. This is realised by comparing regressions to one or more smaller
regressions, so that cases where a very large regression is covering two smaller "true" peaks
can be selected against. This is especially relevant for not fully separated signals with an
intersect at a high intensity.

### Stage 5: Retransformation
All peak properties are adjusted for the original x-axis using the step length for the
region in which a peak was defined. 

## Expansion for Multiple x-axes
@todo