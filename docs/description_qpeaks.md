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

### Stage 3: Plausibility Tests

The two types of tests employed here are those for logical contradictions, which always have 
to be true if a real peak exists, and those for statistical plausibility, which test with
a hard-coded significance level of 0.05. Regressions are considered individually and 
independently during testing.

The following conditions must hold for a peak to be defined:
* The peak must have exactly one maximum (the apex)
* every half of the peak must be described by at least two points
* at least five points, not counting eventual interpolations, must be within the regression window
* the observed maximum within the regression window must be at least twice as large as the smallest value at the edge of the regression window

If these conditions are fulfilled, statitical tests (t-test and F-test) are used to ensure
that the calculated individual peak properties are significant. If one of these tests fails,
the regression is considered to be invalid:
* F-test against a two-coefficient model y = b1 x + b0 prefers the simple model
* Either b2 or b3 is significantly different from 0
* The area that is fully described by the regression is significant when assuming the two outermost points are baseline
* The height is significant
* The total area, including outside of regression bounds, is significant
* The entire regression fulfills minimal goodness-of-fit criteria via the chi-squared test

### Stage 4: Optimal Regression Selection
Since for a well-described peak, multiple regressions will be defined, the optimal description of a 
data region has to be selected. This is realised by comparing regressions to one or more smaller
regressions, so that cases where a very large regression is covering two smaller "true" peaks
can be selected against. This is especially relevant for not fully separated signals with an
intersect at a high intensity.

### Stage 5: Retransformation
All peak properties are adjusted for the original x-axis using the step length for the
region in which a peak was defined. 

The regression properties, including uncertainties, we calculate are as follows:
* Position
* Height
* Full Width at Half Maximum (FWHM)
* Area

Further, goodness-of-fit parameters are calculated per regression.

**Calculation of Position and Height**

The position is the derivative at y = 0 for the apex-containing part of the peak. This depends on 
the coefficient b1 -- if b1 < 0, the left half describes the apex (and vice versa). The height
is obtained by evaluating the peak at x = apex.


**Calculation of FWHM**

The width is calculated from the regression and not empirically. For this, we solve every peak half for
x and calculate the difference when setting y to h / 2.

**Calculation of the Area**

The area of the peak is calculated from -infinity to infinity for the common case of both quadratic
coefficients being negative. Here, the area is first calculated for the transformed x-axis and then
multiplied with the (uniform) distance in x between points to scale it to the original axis. 
The used antiderivative contains the imaginary error function erfi(x), but the normal error function
applies if the quadratic coefficient is negative since we take its square root. If the coefficient is
positive, erfi() is used. The initial area is first calculated for every peak half and then summed up.





## Expansion for Multiple x-axes
@todo