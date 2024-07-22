# Discarded or Modified Criteria:

## point within maxdist but not within maxdist + 1
The test was removed due to not providing actionable, non-redundant
information. With the addition of a standard-deviation corrected
estimate for the critical distance, potentially interesting outliers
are already covered. The ones which aren't included are represented
through the DQSB.

## one-sided intensity profile
Originally, this test only checked if the maximum intensity of the
bin was also the first or last scan which is covered by the bin. 
This would prevent a gaussian fit with a maximum within the bin.
A related case, which relatively frequently occurred in bins without
positive tests, is the maximum being the second to first/last scan.
This is especially likely for small bins purely containing noise.
As such, the test condition was changed to the maximum being the smallest,
the smallest + 1, the largest or the largest - 1 scan.

## test for asymmetry in mz
A relatively frequent problem with the binning results is that
sometimes, either a low-deviation mass trace identified visually
has a lot of noise on one side or is binned together with a different
mass trace. Such bins would, ideally, be subjected to a stricter 
binning and correctly separated.
The test was designed to find bins in which the region in mz around the 
mean mz has a lower density than the bin borders. The test divided 
the bin in three sub-bins in mz, assigned points and compared sizes.
It was positive if the middle region did not contain the most points.
The test was discarded due to having no statistical basis. To obtain
useable results, the expected difference between regions had to be
scaled, while the division into thirds had no basis in the expected
behaviour of normally distributed data and was extremely sensitive to
outliers.

## test for points outside three sigma interval
This test was implemented to identify those bins that
were too broad (in mz) to form visually identifiable ion traces.
It was removed due to outliers not directly corelating with
good bins - often, points with very low intensity at the edges of
the relevant scan interval triggered the detection.

# Bin Categories:

## Bin with duplicate scan:
These bins should not be passed to the centroiding algorithm.
The information that a duplicate was included is passed with the
errorcode, while the centroid with the higher DQSC is retained.

## 