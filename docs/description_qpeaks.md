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

## Limitations
qPeaks uses a regression model with four coefficients. This means that any sparsely sampled
signal will incur a massive increase in relative error from a lack of degrees of freedom.
When fitting five points, even a very poor fit of a straight line will be preferable to the
"obvious" peak shape since it has more degrees of freedom by a factor of four. This and 
similar effects could cause a lower than desired peak recognition rate.

## Processing Stages

### Stage 0: Prerequisites
Any data that is used with qPeaks must fulfill the following criteria for the algorithm to function correctly:
* The data must be present as a set of two-dimensional points
* Expected peaks must be roughly gaussian-shaped, asymmetry can be modeled
* The x-axis must consist of equidistant points for the width of a peak
* All values of y must be greater than zero
* If a relevant baseline exists, it has to be substracted before applying qPeaks

Note that the "$y > 0$" condition is mostly based on the fact that instruments will
generally use zeroes to separate blocks internally. Since the sensitivity is never
high enough to fully detect all ions of a peak, including these zeroes during 
processing will lead to the peak being distorted. Something to keep in mind is that
this results in the measured points, especially during centroiding, not including
the full signal. This also means that the ground-truth peak area will be higher
than that obtained from naively taking the area under the measured points.

The axis used for later transforms is calculated for all integer values between the
maximum scales (-scale to +scale). For the used peak model, this results in a design
matrix $X$ with four columns. The first represents the intercept and is one ($x^0$)
for all values. The second is the x-axis. The quadratic coefficent is only defined
for $x < 0$ for b2 and $x > 0$ for b3 (no quadratic term is relevant at $x = 0$).

Example matrices for scale = 2 and scale = 3:
```math
A = \begin{bmatrix}
1 & -2 & -4 & 0 \cr
1 & -1 & -1 & 0\cr
1 &  0 &  0 &0\cr
1 &  1 &  0 &1\cr
1 &  2 &  0&4
\end{bmatrix}
;
B = \begin{bmatrix}
1 & -3 & -9 &0 \cr
1 & -2 & -4 &0\cr
1 & -1 & -1 &0\cr
1 &  0 &  0 &0\cr
1 &  1 &  0 &1\cr
1 &  2 &  0 &4\cr
1 &  3 &  0&9
\end{bmatrix}
```

The convolution (for scale = 2) $(X^T X)^{-1}$ is then:
```math
\left(
\begin{bmatrix}
1 & 1 & 1 & 1 & 1 \cr
2 & 1 & 0 & 1 & 2 \cr
-4 & -1 & 0 & 0 & 0 \cr
0 & 0 & 0 & 1 & 4 
\end{bmatrix} 
\cdot
\begin{bmatrix}
1 & -2 & -4 & 0 \cr
1 & -1 & -1 & 0\cr
1 &  0 &  0 &0\cr
1 &  1 &  0 &1\cr
1 &  2 &  0&4
\end{bmatrix}
\right)^{-1}
=
\begin{bmatrix}
0.48 & 0 & -0.14 & -0.14 \cr
0 & 2.12 & 1.12 & -1.12\cr
-0.14 &  1.12 & 0.69 & -0.55\cr
-0.14 &  -1.12 &  -0.55 & 0.69
\end{bmatrix}
```

As can be seen from the numbers, there are symmetries on the axis and for all interactions with the quadratic coefficients. This has the concrete effect that the full matrix can be expressed using just six unique values (not counting zero):

```math
\begin{bmatrix}
A & 0 & B & B \cr
0 & C & D & -D\cr
B &  D & E & F\cr
B & -D & F & E
\end{bmatrix}
```

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
is performed with that point set to $x = 0$ relative to the precalculated matrix. This
results in no regression being performed for the outermost two points on each side, since the 
smallest defined matrix requires x to range from $-2$ t0 $2$. This distance to $0$ on the x-axis is 
called "scale". The smallest possible scale is 2, while the largest possible scale is defined
as a constant of the algorithm. This is purely for reasons of computational practicality, in
theory the upper limit is $(\text{length}(x) - 1) / 2$. A convolution with the precalculated matrix
is performed to obtain four regression coefficients describing the peak. The last two coefficents,
b2 and b3, both describe the quadratic term. At $x = 0$, the peak switches from being defined as
$\exp(\text{b2} x^2 + \text{b1} x + \text{b0})$ to $\exp(\text{b3} x^2 + \text{b1} x + \text{b0})$. Regressions are stored with coefficients,
scale and position of x = 0 relative to the first element of the x-axis.

Since the regression is performed on log-transformed data, the height is adjusted using the
untransformed values of y. Since b1, b2 and b3 are not affected, this can be solved with
linear regression. When only adjusting b0, the peak model equation can be factored as:
```math
f(x) = \exp(\text{b0}) * exp(\text{b1} x + \text{b2} x^2)
```
Key observation here is that the second exponential term is constant, since all values of x
within the regression window are known. Accordingly, the function to be minimised is
```math
\sum{(y_i - \exp(\text{b0}) * c_i)^2}
```
where $y_i$ is the observed intensity and $c_i$ the constant part of the peak equation shown above
at position $i$. The following will substitute $\exp(\text{b0})$ with $x$.

```math
\sum{(y_i - x * c_i)^2} = \sum{y_i^2 - 2 * x * c_i * y_i + x^2 * c_i^2} 
```
Now take the derivative of the sum:
```math
\frac{d}{dx} = \sum{-2 * c_i * y_i + 2 * x * c_i^2} = \sum{2 * c_i * (x * c_i - y_i)} 
```


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

For ease of reading, the following equations use b0, b1, b2, b3 instead of $\beta_0$ etc.

**Calculation of Position and Height**

The position is the derivative at $y = 0$ for the apex-containing part of the peak. This depends on 
the coefficient b1 -- if $b1 < 0$, the left half describes the apex (and vice versa). The height
is obtained by evaluating the peak at x = apex. Since the apex in exponential form is always at the
same position as in logarithmic form, we do not need the exponential function in this step. 

Assuming the apex is in the region $x < 0$:
```math
    \frac{d}{dx} \text{b0} + \text{b1} x + \text{b2} x^2 = \text{b1} + 2 \text{b2} x \\
    0 = \text{b1} + 2 \text{b2} x \\
    x = \frac{-\text{b1}}{2 \text{b2}}
```
For the height, evaluate the full function at the apex (note that b1 and b2 are factored into the fraction
already in the term below):
```math
    h(x) = \exp(\text{b0} + \frac{-\text{b1}^2}{2 \text{b2}} + \frac{\text{b1}^2}{4 \text{b2}}) \\
    h(x) = \exp(\text{b0} + \frac{-\text{b1}^2}{2 \text{b2}} * \frac{2}{2} + \frac{\text{b1}^2}{4 \text{b2}}) \\
    h(x) = \exp(\text{b0} + \frac{\text{b1}^2}{4 \text{b2}})
```
These calculations are only relevant for the half that contains the regression apex. If a regression
has a valley (one of the quadratic coefficients is positive), its position is calculated in the same manner as the apex.

**Calculation of FWHM**

The width is calculated from the regression and not empirically. For this, we solve every peak half for
x and calculate the difference when setting y to h / 2.

**Calculation of the Area**

The area of the peak is calculated from -infinity to infinity for the common case of both quadratic
coefficients being negative. Here, the area is first calculated for the transformed x-axis and then
multiplied with the (uniform) distance in x between points to scale it to the original axis. 
The used antiderivative contains the imaginary error function erfi(x), but the normal error function
applies if the quadratic coefficient is negative since we take its square root. If the coefficient is
positive, erfi(x) is used. The initial area is first calculated for every peak half and then summed up.
Since the integral for an open parabola is infinite towards infinity, in those cases the valley of the
relevant peak half is taken as the outer limit of the peak. This introduces additional systematic 
uncertainty in area estimates of peaks with tailing or similar properties.

An example for the case of one positive coefficient is provided here: [(external link)](https://www.desmos.com/calculator/pgac3buccd)

Calculation for the simple case of two negative quadratic coefficients:

The antiderivative of one peak half is:
```math
    F(x) = \sqrt{\pi} * \exp(\text{b0} - \frac{\text{b1}^2}{4 \text{b2}}) * \text{erfi}( (\text{b1} + 2 \text{b2} x) * \frac{1}{2 \sqrt{\text{b2}}} ) * \frac{1}{2 \sqrt{\text{b2}}}
```
where $\text{erfi}(x) = i * \text{erf}(i x)$ is the imaginary error function. Using $\sqrt{b2} = i \sqrt{-b2}$, the imaginary values
cancel each other out. Thus, we substitute b2 -> $-$b2 and use erf() instead of erfi().

All terms except the error function are constant. Note that there is a sign change for x since no square root is used in that place.
For $x = 0$, $(\text{b1} + 2 \text{b2} x) = \text{b1}$. The error function at minus infinity is $-1$ and $+1$ for positive infinity. This means
that the full term is multiplied with $-1$ if b1 is positive.

To get the area, evaluate $F(x)$ at zero for both terms and plus or minus infinity depending on the half, substract
the leftmost result from the rightmost result and sum up the halves for the total area.

If there is a valley point, the outermost limit of integration for that half area is at $x = -\text{b1} / (2 \text{b3})$.
Since the coefficient is positive, the square root does not take an imaginary part and erfi() is used instead of erf().

## Expansion for Multiple x-axes

The presented regression model can also be used to fit more than one signal at the same time. This is because
the height of a regression is 

@todo