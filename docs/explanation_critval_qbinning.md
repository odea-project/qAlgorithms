# Summary of Critical Order Space Analysis and Statistical Test

## Overview
This analysis investigates the critical order space, a statistical metric, and its use in determining whether a dataset belongs to a single normal distribution. The method leverages the maximum empirical order space (largest gap between neighboring sorted data points) to identify natural breaks in a dataset. These breaks can indicate potential splits in the data.

## Definition of Order Space
The order space is the gap between neighboring values in a sorted dataset. For a given dataset, these gaps provide information about the data distribution. The largest order space, the maximum empirical order space, is particularly important as it can reveal significant deviations from a uniform structure, which is expected under normality.

### Critical Order Space
The critical order space is the 99th percentile of the maximum order space observed over multiple Monte Carlo simulations. This value provides a threshold to determine whether the observed maximum gap in a dataset is statistically significant under the normality assumption.

## Statistical Test Based on Critical Order Space
A statistical test was implemented to assess if a dataset belongs to a single normal distribution:
- Data is sorted, and the differences between neighboring points (order spaces) are calculated.
- The largest gap (maximum order space) is compared against the critical order space.
- If the largest order space exceeds the critical value, the dataset is split at that gap, indicating it may not originate from a single normal distribution.

This approach allows for identifying natural breaks and evaluating the normality of datasets.

## Methodology
1. **Simulation Setup:**
   - Normal distributions with standard deviation `s = 25.0` were simulated.
   - Sample sizes ranged from `N = 5` to `N = 500`.
   - For each sample size, `m = 100,000` Monte Carlo simulations were performed to compute the empirical maximum order spaces.

2. **Modeling Critical Values:**
   - Critical values for each sample size were normalized by dividing by the standard deviation (`s`).
   - A model function $$os_{crit}(n) = a + \frac{b}{\sqrt{\log(n)}}$$ was fitted to these values.
   - The parameters $$a$$ and $$b$$ were estimated using non-linear least squares.

3. **Repetition for Parameter Distribution:**
   - The entire procedure was repeated 500 times to assess the variability of $$a$$ and $$b$$.
   - Normality tests were performed on the parameter distributions to compute confidence intervals.

## Results
### Fitted Model
The fitted model for the critical order space is:

$$os_{crit} = a + \frac{b}{\sqrt{\log(n)}}$$

### Estimated Parameters
- \( a \): Mean = 0.144, 95% CI = (0.116, 0.173)
- \( b \): Mean = 3.241, 95% CI = (3.187, 3.296)

Both parameters \( a \) and \( b \) passed normality tests with p-values > 0.05, validating the use of parametric confidence intervals.

### Critical Value Lookup Table
A lookup table was generated for sample sizes \( N \) ranging from 2 to 500, providing critical values at the 99th percentile.

Example:
- \( N = 2 \): Critical value = 4.037
- \( N = 10 \): Critical value = 2.280
- \( N = 500 \): Critical value = 1.445

## Implications and Applications
This methodology provides a robust framework for testing normality and identifying natural breaks in datasets. The critical order space serves as an objective criterion to split datasets that deviate significantly from normality, aiding in the segmentation of heterogeneous data into homogenous subsets.

This approach is particularly valuable in exploratory data analysis, quality control, and scenarios requiring a statistical validation of distributional assumptions.

[The full simulated data, as well as a copy of this page, are available here.](https://github.com/GeRe87/OS_critVal)