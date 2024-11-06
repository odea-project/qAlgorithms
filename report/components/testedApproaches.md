# Development of a clustering algorithm for componentisation

## Approach 1: Binning over RT
Goal: Find distinct distributions of features in RT.
Every Grouping should stem from the same basic distribution. The developed binning
algorithm already fulfills this goal, and could be adapted to search for viable groups
using the retention time uncertainty. 

**non-functional approach**
Problem: All features form a more or less uniform distribution.
After binning in RT, multiple datasets showed the behaviour of forming a few very large bins
(containing more than 40% of all features). Even for the ones where binning resulted in many
smaller groups, the critical distance was often smaller than the uncertainty at the edges of 
these groups. As such, the critical distance was in the region of less than two scans,
from which follows that there was no meaningful difference between the edges of groups.

**further progress**
- While group coherence is a necessary condition for a constructed component, it is not possible
to use it as the sole differentiator for component membership.
- When checking for the difference in RT between two features, the result is only meaningful
if they are further apart than two scans.

## Required condition: Dissimilar mz of all features
The binning algorithm has established that all features result from distinct distributions.
If masses can be grouped, they may not be part of the same component.

## Decision parameter: Uniform regression can be applied
If two separate features can be described by a regression, this regression has more degrees
of freedom and as such a lower base uncertainty. If the combined regression describes both 
features with a lower uncertainty than the individual regressions, both features are considered
to have an identical elution profile and belong to one component. This check is only performed
once all other logical constraints are obeyed.