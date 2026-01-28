This Page lists every property of an entry in our result files with a description of 
what it represents, how it was calculated and how you should interpret it. (WIP)

**All masses are in Dalton, assuming a charge of 1, and all retention times are in seconds! The intensity is in the unit supplied by the mass spectrometer. Scores do not have a unit.**

For the file "example.mzML":

First, the polarity is checked. The polarity is added as "positive" or "negative" to the output file name. If polarity switching is used, two result files are generated.

## example_positive_centroids.csv
Every centroid is a regression determined using the [`qPeaks`algorithm](https://pubs.acs.org/doi/10.1021/acs.analchem.4c00494).
- `ID`: Integer number counting upwards by scans and m/z within scans.
- `mz`: m/z value for the centroid, calculated as the apex position of the regression.
- `mzUncertainty`: Prediction interval for the distribution of m/z that was centroided.
- `scanNumber`: Which scan of the mass spectrometer this centroid exists in.
- `retentionTime`: Retention time, including the void time (the first scan is at t = 0).
- `area`: Area under the gaussian curve described by the intensity profile.
- `areaUncertainty`: 
- `height`: Maximum of the regression fitted to the centroid.
- `heightUncertainty`
- `degreesOfFreedom`: Degrees of freedom of the regression. They are calculated as the number of points in the centroid - 4.
- `dqsCen`: Data quality score of the centroid. Calculated as the error function of `areaUncertainty` divided by `area`.

## example_positive_bins.csv
Every bin is a group of centroids that was determined to have very similar m/z values over a range of continuous scans, with a tolerance for gaps no greater than 3. Since the output of qCentroids is used, only those properties added by the binning step are explained.
- `binID`: Integer number counting upwards by bin generation point. It does not hold chemical information.
- `DQSB`: Quality score of the centroid in the bin. It is calculated as the ratio of the mean distance of a centroid to all other bin members within four scans and the smallest difference to a centroid that is not part of any bin. It ranges from -1 to 1, with values > 0 meaning that this point is more similar to the bin than its surroundings.

## example_positive_features.csv
Every feature is a regression over a bin.
- `ID`: Integer number counting upwards by binID.
- `binID`: The bin in which this feature was found.
- `binIdxStart`: First point within the bin that is part of the region over which the feature was determined.
- `binIdxEnd`: Last point within the bin that is part of the region over which the feature was determined.
- `mz`: m/z value for the feature, calculated as the weighted mean by intensity.
- `mzUncertainty`: 
- `retentionTime`: Apex position of the regression in RT.
- `retentionTimeUncertainty`:
- `lowestRetentionTime`: RT at the point `binIdxStart`.
- `highestRetentionTime`: RT at the point `binIdxEnd`.
- `area`: Area under the gaussian curve described by the intensity profile.
- `areaUncertainty`: 
- `height`: Maximum of the regression fitted to the bin.
- `heightUncertainty`
- `dqsCen`: Weighed mean by intensity of all values for dqsCen within the regression window.
- `dqsBin`: Weighed mean by intensity of all values for dqsBin within the regression window.
- `dqsPeak`: Data quality score of the Feature. Calculated as the error function of `areaUncertainty` divided by `area`.

---