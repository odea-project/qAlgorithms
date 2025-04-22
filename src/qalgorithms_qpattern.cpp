// #include <../include/qalgorithms_qbin.h>
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_qpattern.h"
#include "qalgorithms_datatypes.h"
#include "qalgorithms_global_vars.h"

#include "./../external/CDFlib/cdflib.hpp"

#include <algorithm> // sorting
#include <cmath>     // pow and log
#include <numeric>   // partial sum (cumError)
#include <vector>
#include <iostream>
#include <cassert>

// group peaks identified from bins by their relation within a scan region

int VALLEYS_1 = 0;
int VALLEYS_other = 0;

namespace qAlgorithms
{
    void findComponents(const std::vector<FeaturePeak> *peaks,
                        const std::vector<EIC> *bins,
                        const std::vector<float> *convertRT)
    {
        // peaks must be sorted here since they arrive out of order
#pragma region "Pre-Group"
        std::vector<GroupLims> limits = preGroup(peaks);
        std::vector<PreGrouping> components;
        components.reserve(limits.size());
        for (size_t groupIdx = 0; groupIdx < limits.size(); groupIdx++)
        {
            // only test if the matrix thing works for now
            PreGrouping newComponent;
            size_t groupsize = limits[groupIdx].end - limits[groupIdx].start + 1;
            std::cout << groupsize << ", ";
            // @todo skip for groups with size 1?
            if (groupsize == 1)
            {
                if ((peaks->at(limits[groupIdx].end).coefficients.b2 > 0) ||
                    (peaks->at(limits[groupIdx].end).coefficients.b3 > 0))
                {
                    VALLEYS_1++;
                }

                continue;
            }
            for (size_t j = limits[groupIdx].start; j < limits[groupIdx].end + 1; j++)
            {
                if ((peaks->at(j).coefficients.b2 > 0) || (peaks->at(j).coefficients.b3 > 0))
                {
                    VALLEYS_other++;
                }
            }

            if (groupsize > 10) // this is only to speed up testing - @todo remove!
            {
                continue;
            }
            newComponent.features.reserve(groupsize);
            unsigned int maxScan = 0;
            unsigned int minScan = 4294967295; // max value of unsigned int
            for (size_t j = limits[groupIdx].start; j < limits[groupIdx].end + 1; j++)
            { // @todo this block can probably be discarded
                auto test = &peaks->at(j);
                auto movedTest = moveAndScaleReg(test);
                const qAlgorithms::EIC *bin = &bins->at(test->idxBin);
                calcRSS(&movedTest, bin);
                auto binRTs = (*bins)[test->idxBin].rententionTimes;
                auto scans = (*bins)[test->idxBin].scanNumbers;
                movedTest.binIdxStart = test->idxPeakStart;
                movedTest.binIdxEnd = test->idxPeakEnd;
                movedTest.limit_L = binRTs[test->idxPeakStart];
                movedTest.limit_R = binRTs[test->idxPeakEnd];
                maxScan = std::max(maxScan, scans[test->idxPeakEnd]); // @todo scans should be their own type, same with indices
                minScan = std::min(minScan, scans[test->idxPeakStart]);
                assert(scans[test->idxPeakEnd] - scans[test->idxPeakStart] >= 4);
                assert(maxScan - minScan >= 4);
                newComponent.features.push_back(movedTest);
            }

            // create a vector of unified RTs for interpolation in the harmonised EICs
            std::vector<float> unifiedRT(maxScan - minScan + 1, 0);
            for (size_t i = 0; i < unifiedRT.size(); i++)
            {
                unifiedRT[i] = convertRT->at(minScan + i);
            }

            std::vector<ReducedEIC> eics; // @todo the maximum size of a reduced EIC is the global maxscale + 1!
            // std::vector<MovedRegression> *selectPeaks;
            for (size_t j = 0; j < groupsize; j++)
            {
                const auto feature = newComponent.features[j];
                const qAlgorithms::EIC *bin = &bins->at(feature.binID);
                eics.push_back(harmoniseEIC(&feature, bin, &unifiedRT, minScan, maxScan, feature.binIdxStart, feature.binIdxEnd));
                assert(eics[j].intensity.size() == eics[0].intensity.size());
            }
            // at this stage, the EICs are in the correct shape for performing a multi-regression
#pragma endregion "Pre-Group"
            newComponent.calcScores();
            // produce a subset of bins with uniform RT axis for this component

            // auto test = getCompareOrder(&newComponent); // not the best way to go about this

#pragma region "Compare Pairs"
            // first, calculate the pairwise RSS in a sparse matrix. The RSS is set to INFINITY if it is worse
            // than the sum of both individual RSS values.
            // size: (x^2 -x) / 2 ; access: smaller Idx + (larger Idx * (larger Idx - 1)) / 2
            std::vector<float> pairRSS((groupsize * groupsize - groupsize) / 2, -1);
            // multi match all pairs
            for (size_t idx_S = 0; idx_S < groupsize - 1; idx_S++)
            {
                auto feature_A = newComponent.features[idx_S];
                auto EIC_A = eics[idx_S];
                for (size_t idx_L = idx_S + 1; idx_L < groupsize; idx_L++)
                {
                    auto feature_B = newComponent.features[idx_L];
                    auto EIC_B = eics[idx_L];
                    // pairwise merge to check if two features can be part of the same component
                    float rss_complex = feature_A.RSS + feature_B.RSS;

                    // merge the EICs that are relevant to both
                    static const std::vector<size_t> select{0, 1};
                    // uses an anonymous vector to skip the additional variable, this may be a bad idea
                    size_t idxStart = std::min(EIC_A.featLim_L, EIC_B.featLim_L);
                    size_t idxEnd = std::min(EIC_A.featLim_R, EIC_B.featLim_R);
                    std::vector<ReducedEIC> eics{EIC_A, EIC_B};
                    auto mergedEIC = mergeEICs(&eics, &select, idxStart, idxEnd);

                    // @todo consider adding a special case function for a combination of two regressions
                    float rss_simple = rss_complex * 1.05; // this is just a standin, perform a regression here
                    // we can be certain that the absolute rss is at most identical (for the case where feature_A == feature_B).
                    // this check is also needed since the function we use for the f distribution later exits with error if a
                    // negative  value is supplied
                    assert(rss_simple > rss_complex);
                    // F-test to check if the merge is a good idea
                    size_t numPoints = feature_A.binIdxEnd - feature_A.binIdxStart + feature_B.binIdxEnd - feature_B.binIdxStart + 2;
                    size_t params_both = 8;  // four coefficients each
                    size_t params_combo = 5; // shared b1, b2, b3, two b0

                    bool merge = preferMerge(rss_complex, rss_simple, numPoints, params_both, params_combo);
                    size_t access = idx_S + (idx_L * (idx_L - 1)) / 2;
                    pairRSS[access] = merge ? rss_simple : INFINITY;
                }
            }
            // pairRSS serves as an exclusion matrix and priorisation tool. The component assignment is handled through
            // a group vector and stored assignment information

            int componentGroup = -1; // the ID is incremented before assignment
            std::vector<CompAssignment> groupings;
            groupings.reserve(groupsize);
            for (size_t idx = 0; idx < groupsize; idx++)
            {
                groupings.push_back({idx, -1});
            }

            // multiFit(&eics, &newComponent.features);

            // if two features are not improved by being combined individually, they may never be part of the same group

            // assign features to groups by forming the best-fit pairs

            // at this point, the group must be subdivided such that the total RSS is minimal
            // special case for groupsize 2 might be a good idea, since it seems to occur very often with the
            // current pre-gruping strategy

            // all ungrouped features are -1 at this point, loop over them and add a correct component number
            for (size_t ID = 0; ID < groupsize; ID++)
            {
                if (groupings[ID].component == -1)
                {
                    componentGroup++;
                    groupings[ID].component = componentGroup;
                }
            }
            assert(componentGroup > -1);
            assert((size_t(componentGroup)) <= groupsize);
        }
        std::cout << std::endl;
        std::cout << "1: " << VALLEYS_1 << " ; other: " << VALLEYS_other << "\n"; // at least for one dataset, features with a valley point are much more likely
        // to be groups of size 1 than to be included in larger groups (ca. twice as likely)
    }

    void compDecision()
    {
        // bool incompatible = preferMerge();
        bool incompatible = true;
        if (incompatible)
        {
            return;
        }
        // add the signs of comp assignments
        const int signs = -4;
        switch (signs)
        {
        case -2:
            // both are individual points
            // newComponent();
            break;

        case 0:
            // one component, one loose point
            // addToComponent();
            break;

        case 2:
            // merge both components

            break;

        default:
            assert(false);
            break;
        }
    }

    void mergeComponents(std::vector<CompAssignment> *groupings, size_t *compCount, int ID_A, int ID_B)
    {
        assert(ID_A != ID_B);
        assert(*compCount >= size_t(ID_A) && *compCount >= size_t(ID_B));
        int ID_new = std::min(ID_A, ID_B);
        int ID_max = std::max(ID_A, ID_B);
        compCount -= 1;
        for (size_t i = 0; i < groupings->size(); i++)
        {
            int ID_curr = groupings->at(i).component;
            assert(int(*compCount) >= ID_curr - 1);
            if ((ID_curr == ID_A) || (ID_curr == ID_B))
            {
                groupings->at(i).component = ID_new;
            }
            else if (ID_curr > ID_max)
            {
                groupings->at(i).component -= 1;
            }
        }
    }

    void newComponent(std::vector<CompAssignment> *groupings, int *compCount, size_t member_A, size_t member_B)
    {
        // a new component is only ever made from two unassigned features
        compCount++;
        bool exit = false;
        for (size_t i = 0; i < groupings->size(); i++)
        {
            if ((groupings->at(i).feature == member_A) || (groupings->at(i).feature == member_B))
            {
                groupings->at(i).component = *compCount;
                if (exit)
                {
                    break;
                }
                exit = true;
            }
        }
    }

    void addToComponent(std::vector<CompAssignment> *groupings, const size_t compCount, int ID_add, size_t member)
    {
        // only update assignment of member
        for (size_t i = 0; i < groupings->size(); i++)
        {
            if (groupings->at(i).feature == member)
            {
                groupings->at(i).component = ID_add;
                break;
            }
        }
    }

    MergedEIC mergeEICs(const std::vector<ReducedEIC> *eics,
                        const std::vector<size_t> *selection,
                        size_t idxStart,
                        size_t idxEnd)
    {
        assert(selection->size() <= eics->size());
        assert(idxEnd > idxStart);
        size_t eicSize = eics->front().intensity.size();
        assert(idxEnd < eicSize);
        size_t span = idxEnd - idxStart + 1;
        MergedEIC result;
        result.intensity.reserve(span);
        result.intensity_log.reserve(span);
        result.RSS_cum.reserve(span); // remember to overwrite this! It is no longer accurate after the regression completes
        result.df.reserve(span);
        for (size_t i = 0; i < selection->size(); i++)
        {
            const ReducedEIC *subEIC = &(eics->at(i));
            assert(subEIC->intensity.size() == eicSize);
            for (size_t idx = idxStart; idx <= idxEnd; idx++)
            {
                result.intensity.push_back(subEIC->intensity[idx]);
                result.intensity_log.push_back(subEIC->intensity_log[idx]);
                result.df.push_back(subEIC->df[idx]);
            }
        }

        return result;
    }

    MultiRegression runningRegression_multi( // add function that combines multiplr eics and updates the peak count
        const ReducedEIC *eic,
        const size_t maxScale,
        const size_t numPeaks,
        const size_t peakFrame)
    {
        // regressions for every possible scale and window position
        const std::vector<MultiRegression> regressions = findCoefficients_multi(&(eic->intensity_log), maxScale, numPeaks, peakFrame);
        // bookkeeping: which regressions are viable?
        std::vector<bool> regressionOK(regressions.size(), true);
        // used to decide on the best overall regression
        std::vector<double> sum_MSE(regressions.size(), 0);

        // for every multi-regression, reconstruct the normal regression and validate. If any derived regression
        // is invalid, the multi-regression is also invalid.
        // we know that the correct interpretation of the component always involves only one regression.
        // since the merge over scales is required to isolate multiple peaks within the same block from
        // each other, it would always apply to the special case of all regressions being in the same window here.
        // this allows us to simplify the program such that the best regression regardless of scale is chosen.
        // the validateRegressions function does not account for that special case, but only the validation step
        // is actually required here. as such, this code has been adapted from the implementation in qPeaks.

        // the validation function expects the intensity and degree of freedom data to arrive as a vector
        // for every singular peak.
        std::vector<std::vector<bool>> DF_vecs;
        DF_vecs.reserve(numPeaks);
        std::vector<std::vector<float>> intensity_vecs;
        intensity_vecs.reserve(numPeaks);
        std::vector<std::vector<float>> logInt_vecs;
        logInt_vecs.reserve(numPeaks);

        for (size_t i = 0; i < numPeaks; i++)
        {
            size_t idxBegin = i * peakFrame;
            size_t idxEnd = (i + 1) * peakFrame;
            logInt_vecs[i] = std::vector<float>(eic->intensity_log.begin() + idxBegin, eic->intensity_log.begin() + idxEnd);
            intensity_vecs[i] = std::vector<float>(eic->intensity.begin() + idxBegin, eic->intensity.begin() + idxEnd);
            DF_vecs[i] = std::vector<bool>(eic->df.begin() + idxBegin, eic->df.begin() + idxEnd);
        }

        for (size_t multiReg = 0; multiReg < regressions.size(); multiReg++)
        {
            // create the set of individual regressions (only one b0 each) and provide the individual
            // vectors for EIC data, then test the regression for validity
            auto reg = regressions[multiReg];
            size_t idxRegStart = reg.idxStart;
            size_t scale = reg.scale;
            for (size_t i = 0; i < numPeaks; i++)
            {
                RegCoeffs coeff{reg.b0_vec[i], reg.b1, reg.b2, reg.b3};
                // size_t idxBegin = i * peakFrame;
                // size_t idxEnd = (i + 1) * peakFrame;
                RegressionGauss testCase;
                testCase.newCoeffs = coeff; // @todo do this during initialisation

                size_t df = calcDF(&(DF_vecs[i]), idxRegStart, 2 * scale + idxRegStart);
                if (df < 5)
                {
                    regressionOK[multiReg] = false;
                    break;
                }
                makeValidRegression(&testCase, idxRegStart, scale, &(DF_vecs[i]), &(intensity_vecs[i]), &(logInt_vecs[i]));
                if (!testCase.isValid)
                {
                    regressionOK[multiReg] = false;
                    break;
                }
                // now that only regressions which are logically sensible are present, pick the best one. This uses the sum
                // of all mse values for the given regression, similar to qPeaks. @todo should we also adjust the limits to
                // cover the entire array here?
                double mse = calcSSE_exp(coeff,
                                         &(intensity_vecs[i]),
                                         testCase.left_limit,
                                         testCase.right_limit,
                                         testCase.index_x0);
                sum_MSE[multiReg] += mse / (testCase.df - 4);
            }
        }
        // decide on the best regression @todo do some statistics regarding how different mse values actually are, then remove
        // the storage vector and replace it with a current minimum + index tracker
        double min_MSE = INFINITY;
        int minIdx = -1;
        for (size_t i = 0; i < regressions.size(); i++)
        {
            if (regressionOK[i] && min_MSE > sum_MSE[i])
            {
                min_MSE = sum_MSE[i];
                minIdx = i;
            }
        }
        assert(minIdx >= 0);
        // @todo calculate the RSS at this point?
        return regressions[minIdx];
    }

    constexpr auto INV_ARRAY = initialize();

    std::vector<MultiRegression> findCoefficients_multi( // @todo add option for a minimum scale
        const std::vector<float> *intensity_log,
        const size_t max_scale, // maximum scale that will be checked. Should generally be limited by peakFrame
        const size_t numPeaks,  // only > 1 during componentisation (for now? @todo)
        const size_t peakFrame) // how many points are covered per peak? For single-peak data, this is the length of intensity_log
    {
        /*
  This function performs a convolution with the kernel: (xTx)^-1 xT and the data array: intensity_log.
  (xTx)^-1 is pre_calculated and stored in the inv_array and contains 7 unique values per scale.
  xT is the transpose of the design matrix X that looks like this:
  for scale = 2:
  xT = | 1  1  1  1  1 |    : all ones
       |-2 -1  0  1  2 |    : from -scale to scale
       | 4  1  0  0  0 |    : x^2 values for x < 0
       | 0  0  0  1  4 |    : x^2 values for x > 0

  It contains one additional row of all ones for every additional peak that is added into the model

  When adding multiple peaks to the regression model, we need to adjust the inverse values.
  This will change the number of unique values in the inv_values array from 6 to 7.
  Here we use the inv_array[1] position and shift all values from that point onwards to the right.
  example for num_peaks = 2:
  original matrix with the unique values [a, b, c, d, e, f] (six unique values)
  | a  0  b  b |
  | 0  c  d -d |
  | b  d  e  f |
  | b -d  f  e |

  new matrix with the unique values [A1, A2, B, C, D, E, F] (seven unique values)
  | A1  A2  0  B  B |
  | A2  A1  0  B  B |
  | 0   0   C  D -D |
  | B   B   D  E  F |
  | B   B  -D  F  E |

  for num_peaks = 3:
  new matrix with the unique values [A1, A2, B, C, D, E, F] (the same seven unique values)
  | A1  A2  A2  0  B  B |
  | A2  A1  A2  0  B  B |
  | A2  A2  A1  0  B  B |
  | 0   0   0   C  D -D |
  | B   B   B   D  E  F |
  | B   B   B  -D  F  E |

  Note that no more than seven different values are needed per scale.

  In general, we have two moving actions:
  1) step right through the intensity_log and calculate the convolution with the kernel
  2) expand the kernel to the left and right of the intensity_log (higher scale)

  The workflow is organized in nested loops:
  1) outer loop: move along the intensity_log AND calculate the convolution with the scale=2 kernel
  2) inner loop: expand the kernel to the left and right of the intensity_log (higher scale)

  The pattern used for both loops looks like:
  e.g. for n(y) = 16
  outer loop: i = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
  inner loop: range from:
   for i=0: 3 to 3 => i.e. loop is not executed
   for i=1: 3 to 4 => i.e. loop is executed once for scale=3
   for i=2: 3 to 5 => i.e. loop is executed twice for scale=3,4
   for i=3: 3 to 6 => i.e. loop is executed three times for scale=3,4,5
   for i=4: 3 to 7 => i.e. loop is executed four times for scale=3,4,5,6
   for i=5: 3 to 8 => i.e. loop is executed five times for scale=3,4,5,6,7
   for i=6: 3 to 8 => i.e. loop is executed five times for scale=3,4,5,6,7
   for i=7: 3 to 7 => i.e. loop is executed four times for scale=3,4,5,6
   for i=8: 3 to 6 => i.e. loop is executed three times for scale=3,4,5
   for i=9: 3 to 5 => i.e. loop is executed twice for scale=3,4
   for i=10: 3 to 4 => i.e. loop is executed once for scale=3
   for i=11: 3 to 3 => i.e. loop is not executed
   */
        assert(max_scale > 1);
        assert(numPeaks > 1); // this is necessary since the A1 INV_ARRAY value is
        const size_t minScale = 2;
        const size_t steps = intensity_log->size() - 2 * minScale; // iteration number at scale 2

        std::vector<float> y_array_sum = *intensity_log; // sum of each index across multiple dimensions of intensity_log
        // only relevant for more than one peak

        //   this vector is for the inner loop and looks like:
        //   [scale_min, scale_min +1 , .... scale_max, ... scale_min +1, scale_min]
        //   length of vector: num_steps
        // the vector starts and ends with minScale and increases by one towards the middle until it reaches the scale value
        std::vector<size_t> maxInnerLoop(steps, max_scale);
        for (size_t i = 0; i + 2 < max_scale; i++) // +2 since smallest scale is 2
        {
            // @todo somewhat inefficient, design better iteration scheme
            size_t newVal = i + 2;
            maxInnerLoop[i] = newVal;
            size_t backIdx = maxInnerLoop.size() - i - 1;
            maxInnerLoop[backIdx] = newVal;
        }

        size_t iterationCount = std::accumulate(maxInnerLoop.begin(), maxInnerLoop.end(), 0) - maxInnerLoop.size(); // no range check necessary since every entry > 1

        constexpr MultiRegression empty{std::vector<float>{}, 0, 0, NAN, NAN, NAN, NAN};
        MultiRegression localEmpty = empty;
        localEmpty.b0_vec.reserve(numPeaks);
        std::vector<MultiRegression> coeffs(iterationCount, localEmpty);

        // these arrays contain all coefficients for every loop iteration
        std::vector<long double> beta_0(iterationCount * numPeaks, NAN); // one per iteration per peak

        // one per iteration
        // std::vector<long double> beta_1(iterationCount, NAN);
        // std::vector<long double> beta_2(iterationCount, NAN);
        // std::vector<long double> beta_3(iterationCount, NAN);

        // the product sums are the rows of the design matrix (xT) * intensity_log[i:i+4] (dot product)
        // The first n entries are contained in the b0 vector, one for each peak the regression is performed
        // over.
        std::vector<long double> tmp_product_sum_b0(numPeaks, NAN); // one fully separate b0 per peak
        long double tmp_product_sum_b1;
        long double tmp_product_sum_b2;
        long double tmp_product_sum_b3;

        long double sum_tmp_product_sum_b0 = 0; // sum of all elements in the tmp_product_sum_b0 vector

        size_t k = 0;
        for (size_t i = 0; i < steps; i++)
        {

            // move along the intensity_log (outer loop)
            // calculate the convolution with the kernel of the lowest scale (= 2), i.e. xT * intensity_log[i:i+4]
            // tmp_product_sum_b0 = intensity_log[i:i+5].sum(axis=0) // numPeaks rows of xT * intensity_log[i:i+4]
            for (size_t b0_elems = 0; b0_elems < numPeaks; b0_elems++)
            {
                size_t x = b0_elems * peakFrame + i;
                tmp_product_sum_b0[b0_elems] = intensity_log->at(x) + intensity_log->at(x + 1) +
                                               intensity_log->at(x + 2) + intensity_log->at(x + 3) +
                                               intensity_log->at(x + 4); // b0 = 1 for all elements
            }

            tmp_product_sum_b1 = 2 * (y_array_sum[i + 4] - y_array_sum[i]) + y_array_sum[i + 3] - y_array_sum[i + 1];
            tmp_product_sum_b2 = 4 * y_array_sum[i] + y_array_sum[i + 1];
            tmp_product_sum_b3 = 4 * y_array_sum[i + 4] + y_array_sum[i + 3];

            sum_tmp_product_sum_b0 = std::accumulate(tmp_product_sum_b0.begin(), tmp_product_sum_b0.end(), 0.0);

            // A1 is not modified if numPeaks is 1. It is, however, modified by -2 * b / numPeaks and not just /numPeaks if numPeaks > 1
            // use [12 + ...] since the array is constructed for the accession arry[scale * 6 + (0:5)]
            const long double S2_A1 = INV_ARRAY[12 + 0] - 2 * INV_ARRAY[12 + 1] / numPeaks;
            // A2 is only defined for numPeaks > 1, set it to 0 to avoid conditional in loops
            // @todo replace the array with a struct and an accessor function
            const long double S2_A2 = -2 * INV_ARRAY[12 + 1] / numPeaks;
            const long double S2_B = INV_ARRAY[12 + 1] / numPeaks;
            const long double S2_C = INV_ARRAY[12 + 2] / numPeaks;
            const long double S2_D = INV_ARRAY[12 + 3] / numPeaks;
            const long double S2_E = INV_ARRAY[12 + 4] / numPeaks;
            const long double S2_F = INV_ARRAY[12 + 5] / numPeaks;

            // this line is: a*t_i + b * sum(t without i)
            // inv_array starts at scale = 2
            for (size_t peak = 0; peak < numPeaks; peak++)
            {
                coeffs[k].b0_vec[peak] = S2_A2 * sum_tmp_product_sum_b0 + (S2_A1 - S2_A2) * tmp_product_sum_b0[peak] +
                                         S2_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
            }

            coeffs[k].b1 = S2_C * tmp_product_sum_b1 + S2_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
            coeffs[k].b2 = S2_B * sum_tmp_product_sum_b0 + S2_D * tmp_product_sum_b1 + S2_E * tmp_product_sum_b2 + S2_F * tmp_product_sum_b3;
            coeffs[k].b3 = S2_B * sum_tmp_product_sum_b0 - S2_D * tmp_product_sum_b1 + S2_F * tmp_product_sum_b2 + S2_E * tmp_product_sum_b3;

            k += 1;       // update index for the productsums array
            size_t u = 1; // u is the expansion increment
            for (size_t scale = 3; scale < maxInnerLoop[i] + 1; scale++)
            { // minimum scale is 2. so we start with scale + 1 = 3 in the inner loop
                size_t scale_sqr = scale * scale;
                // expand the kernel to the left and right of the intensity_log.
                sum_tmp_product_sum_b0 = 0;
                for (size_t peak = 0; peak < numPeaks; peak++)
                {
                    tmp_product_sum_b0[peak] += intensity_log->at(i - u) + intensity_log->at(i + 4 + u);
                    sum_tmp_product_sum_b0 += tmp_product_sum_b0[peak];
                }

                tmp_product_sum_b1 += scale * (y_array_sum[i + 4 + u] - y_array_sum[i - u]);
                tmp_product_sum_b2 += scale_sqr * y_array_sum[i - u];
                tmp_product_sum_b3 += scale_sqr * y_array_sum[i + 4 + u];

                // A1 is not modified if numPeaks is 1. It is, however, modified by -2 * b / numPeaks and not just /numPeaks if numPeaks > 1
                const long double inv_A1 = numPeaks == 1 ? INV_ARRAY[12 + u * 6 + 0] : INV_ARRAY[12 + u * 6 + 0] - 2 * INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                // A2 is only defined for numPeaks > 1, set it to 0 to avoid conditional in loops
                const long double inv_A2 = numPeaks == 1 ? 0 : -2 * INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                const long double inv_B = INV_ARRAY[12 + u * 6 + 1] / numPeaks;
                const long double inv_C = INV_ARRAY[12 + u * 6 + 2] / numPeaks;
                const long double inv_D = INV_ARRAY[12 + u * 6 + 3] / numPeaks;
                const long double inv_E = INV_ARRAY[12 + u * 6 + 4] / numPeaks;
                const long double inv_F = INV_ARRAY[12 + u * 6 + 5] / numPeaks;

                for (size_t peak = 0; peak < numPeaks; peak++)
                {
                    coeffs[k].b0_vec[peak] = inv_A2 * sum_tmp_product_sum_b0 + (inv_A1 - inv_A2) * tmp_product_sum_b0[peak] +
                                             inv_B * (tmp_product_sum_b2 + tmp_product_sum_b3);
                }

                coeffs[k].b1 = inv_C * tmp_product_sum_b1 + inv_D * (tmp_product_sum_b2 - tmp_product_sum_b3);
                coeffs[k].b2 = inv_B * sum_tmp_product_sum_b0 + inv_D * tmp_product_sum_b1 + inv_E * tmp_product_sum_b2 + inv_F * tmp_product_sum_b3;
                coeffs[k].b3 = inv_B * sum_tmp_product_sum_b0 - inv_D * tmp_product_sum_b1 + inv_F * tmp_product_sum_b2 + inv_E * tmp_product_sum_b3;

                u += 1; // update expansion increment
                k += 1; // update index for the productsums array
            }
        }

        return coeffs;
    }

    ReducedEIC harmoniseEIC(const MovedRegression *feature,
                            const EIC *bin,
                            const std::vector<float> *RTs,
                            const unsigned int minScan, // minimum overall scan in the subgroup
                            const unsigned int maxScan, // maximum overall scan in the subgroup
                            const size_t minIdx,
                            const size_t maxIdx)
    {
        assert(maxIdx < bin->ints_area.size());
        assert(minScan < bin->scanNumbers.back());
        assert(maxScan > bin->scanNumbers.front());
        // assert(minScan >= bin->scanNumbers.front());
        // assert(maxScan <= bin->scanNumbers.back());
        // this function creates harmonised EICs, meaning that it can be used for the running regression
        // within a component subgroup without further modification. The process is as follows:
        // 1) create a results vector for every element which is of the size known at the time of function
        //    call
        unsigned int length = maxScan - minScan + 1;
        assert(length > 4);
        assert(RTs->size() == length);
        assert(bin->ints_area[minIdx] != 0);
        assert(bin->ints_area[maxIdx] != 0);

        // scan relates to the complete measurement and idx to the position within the bin

        ReducedEIC reduced{
            // std::vector<float>(length, 0),  // RT
            std::vector<float>(length, 0),   // intensity
            std::vector<float>(length, 0),   // log intensity
            std::vector<size_t>(length, 0),  // scan number
            std::vector<float>(length, 0),   // RSS_cum
            std::vector<bool>(length, true), // degrees of freedom
            0,                               // feature ID
            0,                               // bin ID
            // these limits are relating to the fully interpolated EIC
            bin->scanNumbers[minIdx] - minScan,
            bin->scanNumbers[maxIdx] - minScan};

        std::iota(reduced.scanNo.begin(), reduced.scanNo.end(), minScan);

        // 2) transfer all points that will not be interpolated. At this stage, the log values are used already to
        //    minimise the big number problems with RSS and avoid all the exp() functions. Note that points outside
        //    the regression window will only be transferred if they do not increase again seen from the feature
        //    region outwards. This is chosen to account for cases where the initial regression window was chosen too
        //    small. @todo this will only be integrated in the future since i am unsure as to how good of an idea that
        //    is at the moment

        for (size_t i = 0; i < bin->ints_area.size(); i++) // indices in relation to bin without interpolations
        {
            size_t scan = bin->scanNumbers[i];
            if (scan < minScan)
            {
                continue;
            }
            else if (scan > maxScan)
            {
                break;
            }

            size_t relIdx = scan - minScan;
            // outerLeft = std::min(relIdx, outerLeft);
            // outerRight = std::max(relIdx, outerRight);
            reduced.intensity[relIdx] = bin->ints_area[i];
            reduced.intensity_log[relIdx] = log(bin->ints_area[i]);
        }
        // add points from outside the window?

        // 3) interpolate all intensities at 0 (log only for now)
        // this is combined with the RSS calculation
        // 4) calculate the RSS (log) for all transferred points against the moved regression and write them into
        //    the 0-filled vector for cumRSS. Then, take the cumsum over the vector.
        for (unsigned int i = 0; i < length; i++)
        {
            float predictedInt = logIntAt(feature, RTs->at(i));
            // special case positive coefficient in b2 or b3: Here, the model will lead to very high results
            // (10e500) relatively quickly. As such, the prediction past the outer limit is corrected to be
            // always the last real value.
            // this will lead to a very high improvement if a positive b23 is part of a double negative component
            if (feature->b2 > 0) [[unlikely]] // @todo this is a really slow loop to execute, remove branching
            {
                if (i < reduced.featLim_L)
                {
                    predictedInt = reduced.intensity_log[reduced.featLim_L];
                }
            }
            if (feature->b3 > 0) [[unlikely]]
            {
                if (i > reduced.featLim_R)
                {
                    predictedInt = reduced.intensity_log[reduced.featLim_R];
                }
            }

            if (reduced.intensity_log[i] == 0)
            { // value needs to be interpolated
                reduced.intensity_log[i] = predictedInt;
                reduced.intensity[i] = std::exp(predictedInt);
                reduced.df[i] = false;
            }
            else
            {
                float diff = reduced.intensity_log[i] - predictedInt;
                reduced.RSS_cum[i] = diff * diff;
            }
        }

        // form the cumulative sum over RSS
        for (unsigned int i = 1; i < length; i++)
        {
            reduced.RSS_cum[i] += reduced.RSS_cum[i - 1];
            assert(reduced.RSS_cum[i] != INFINITY); // @todo this should not happen in the log case
        }

        return reduced;
    }

    float logIntAt(const MovedRegression *feature, float RT)
    {
        // this function interpolates the logarithmic intensity of a given regression at the retention time RT.
        assert(RT >= 0);
        bool leftHalf = RT < feature->RT_switch;
        float b0 = leftHalf ? feature->b0_L : feature->b0_R;
        float b1 = leftHalf ? feature->b1_L : feature->b1_R;
        float b23 = leftHalf ? feature->b2 : feature->b3;
        return b0 + RT * b1 + RT * RT * b23;
    }

    float PreGrouping::score(size_t idx1, size_t idx2)
    { // @todo we could use a more complex structure here to save space, but with how small these groups should be it is probably pointless
        size_t size = shapeScores.size();
        assert(idx1 < size && idx2 < size);

        if (idx1 == idx2)
        {
            return 1;
        }
        size_t idx = size * idx1 + idx2;
        return shapeScores[idx];
    }

    void PreGrouping::calcScores()
    {
        shapeScores.reserve(features.size() * features.size());
        for (size_t i = 0; i < features.size(); i++)
        {
            for (size_t j = 0; j < features.size(); j++)
            {
                float tanimoto = calcTanimoto_reg(&features[i], &features[j]);
                shapeScores.push_back(tanimoto);
            }
        }
    }

    std::vector<GroupLims> preGroup(const std::vector<FeaturePeak> *peaks)
    {
        // base assumption: for all peaks that constitute a component, the two apexes
        // are no further apart than the smallest uncertainty of the pair
        // This assumes that peaks is sorted by retention time!
        std::vector<GroupLims> initialGroups;
        initialGroups.push_back({0, 0});
        for (size_t i = 1; i < peaks->size(); i++)
        {
            float apex_L = (*peaks)[i - 1].retentionTime;
            float apex_R = (*peaks)[i].retentionTime;
            assert(apex_L <= apex_R);
            float uncert = std::min((*peaks)[i - 1].retentionTimeUncertainty, (*peaks)[i].retentionTimeUncertainty);
            if (apex_R - apex_L > uncert)
            {
                initialGroups.back().end = i - 1;
                initialGroups.push_back({i, 0});
            }
        }
        initialGroups.back().end = peaks->size() - 1;
        return initialGroups;
    }

    MovedRegression moveAndScaleReg(const FeaturePeak *feature)
    {
        float b0 = feature->coefficients.b0;
        float b1 = feature->coefficients.b1;
        float b2 = feature->coefficients.b2;
        float b3 = feature->coefficients.b3;

        const bool apexLeft = feature->apexLeft;
        float b23 = apexLeft ? b2 : b3;
        float apexDist = -b1 / (2 * b23);
        // float height = b0 + b1 * apexDist + b23 * apexDist * apexDist;
        // const float scaleTo = 1;
        // b0 += log(scaleTo) - height; // scale to height 1. The log is 0 for case height = 1, but we might have to adjust later
        // move regression to RT of feature
        float offset = feature->retentionTime - apexDist;
        MovedRegression movedReg;
        movedReg.origin = feature;
        movedReg.binID = feature->idxBin;
        movedReg.limit_L = 0; // @todo
        movedReg.limit_R = 0; // @todo
        movedReg.b0_L = b0 - b1 * offset + b2 * offset * offset;
        movedReg.b0_R = b0 - b1 * offset + b3 * offset * offset;
        movedReg.b1_L = b1 - 2 * b2 * offset;
        movedReg.b1_R = b1 - 2 * b3 * offset;
        movedReg.b2 = b2;
        movedReg.b3 = b3;
        movedReg.RT_switch = (apexLeft ? movedReg.b1_L : movedReg.b1_R) / (-2 * b23); // -1 would be in front of the b1, switched for readability
        return movedReg;
    }

    // small functions for calcTanimoto_reg
    inline std::vector<float> areas(std::vector<float> *borders, MovedRegression *feature)
    {
        std::vector<float> areas;
        areas.reserve(borders->size() - 1);
        std::vector<float> heights;
        heights.reserve(borders->size());
        { // calcualte the relevant heights for both peak halves
            size_t i = 0;
            for (; i < borders->size(); i++)
            {
                float x = borders->at(i);
                if (x > feature->RT_switch)
                {
                    break;
                }
                heights.push_back(exp(feature->b0_L + feature->b1_L * x + feature->b2 * x * x));
            }
            for (; i < borders->size(); i++)
            {
                float x = borders->at(i);
                heights.push_back(exp(feature->b0_R + feature->b1_R * x + feature->b3 * x * x));
            }
        }
        for (size_t i = 1; i < borders->size(); i++)
        {
            float meanHeight = (heights[i] + heights[i - 1]) / 2;
            float dist = borders->at(i) - borders->at(i - 1);
            // assert(dist > 0); // unsure why this fails, should be fine?
            float area = meanHeight * dist;
            // assert(area > 0); // evidently, this is not sure to be the case
            areas.push_back(area);
        }
        return areas;
    }

    float calcTanimoto_reg(MovedRegression *feature_A, MovedRegression *feature_B)
    {
        // assert(feature_A->RT_switch != feature_B->RT_switch); // @todo turn on once we need a more elegant access pattern
        MovedRegression *feature_L = feature_A->RT_switch < feature_B->RT_switch ? feature_A : feature_B;
        MovedRegression *feature_R = feature_A->RT_switch > feature_B->RT_switch ? feature_A : feature_B;
        if (feature_L->limit_R < feature_R->limit_L) [[unlikely]]
        {
            // the features have no overlap, exclude this case for better correctness checing in the function
            return 0;
        }

        // naming: first  L / R : feature_L or _R; second L / R : left or right half of the regression
        float b0_L_L = feature_L->b0_L;
        float b0_L_R = feature_L->b0_R;
        float b0_R_L = feature_R->b0_L;
        float b0_R_R = feature_R->b0_R;
        float b1_L_L = feature_L->b1_L;
        float b1_L_R = feature_L->b1_R;
        float b1_R_L = feature_R->b1_L;
        float b1_R_R = feature_R->b1_R;
        float b2_L = feature_L->b2;
        float b2_R = feature_R->b2;
        float b3_L = feature_L->b3;
        float b3_R = feature_R->b3;
        float switch_L = feature_L->RT_switch;
        float switch_R = feature_R->RT_switch;

        float limit_L = std::max(feature_L->limit_L, feature_R->limit_L);
        float limit_R = std::max(feature_L->limit_R, feature_R->limit_R);

        // this is a stopgap solution for the case of a positive quadratic exponent when the regression window extends past the local minimum
        // use signbit of -b2 / -b3 to still select the max if both are positive (signbit returns true for set)
        if ((b2_L > 0) | (b2_R > 0))
        {
            limit_L = std::max(std::signbit(-b2_L) * feature_L->limit_L, std::signbit(-b2_R) * feature_R->limit_L);
        }
        if ((b3_L > 0) | (b3_R > 0))
        {
            limit_R = std::max(std::signbit(-b2_R) * feature_L->limit_R, std::signbit(-b3_R) * feature_R->limit_R);
        }

        // find intersects of both regressions
        std::vector<float> intersects; // @todo this can have a fixed size
        intersects.push_back(limit_L);

        // term under the root separate to check for 0 == ((b1A - b1B)^2 - 4* (b23A - b23B) * (b0A - b0B))
        { // both left halves of the regressions
            float root_LL = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) - 4 * (b2_L - b2_R) * (b0_L_L - b0_R_L);
            if (root_LL > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) / (2 * (b2_L - b2_R)) + root_LL / (2 * (b2_L - b2_R));
                if (intersect_pos > limit_L && intersect_pos < switch_L)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_L - b1_L_L) * (b1_R_L - b1_L_L) / (2 * (b2_L - b2_R)) - root_LL / (2 * (b2_L - b2_R));
                if (intersect_neg > limit_L && intersect_neg < switch_L)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }

        intersects.push_back(switch_L);
        { // in the middle part, take the right half of feature_L and the left half of feature_R
            float root_LR = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) - 4 * (b3_L - b2_R) * (b0_L_R - b0_R_L);
            if (root_LR > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) / (2 * (b3_L - b2_R)) + root_LR / (2 * (b3_L - b2_R));
                if (intersect_pos > switch_L && intersect_pos < switch_R)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_L - b1_L_R) * (b1_R_L - b1_L_R) / (2 * (b3_L - b2_R)) - root_LR / (2 * (b3_L - b2_R));
                if (intersect_neg > switch_L && intersect_neg < switch_R)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }

        intersects.push_back(switch_R);
        {
            float root_RR = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) - 4 * (b3_L - b3_R) * (b0_L_L - b0_R_L);
            if (root_RR > 0)
            {
                // if the term is negative, there are no intersects between the two curves
                float intersect_pos = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) / (2 * (b3_L - b3_R)) + root_RR / (2 * (b3_L - b3_R));
                if (intersect_pos > switch_R && intersect_pos < limit_R)
                {
                    intersects.push_back(intersect_pos);
                }
                float intersect_neg = (b1_R_R - b1_L_R) * (b1_R_R - b1_L_R) / (2 * (b3_L - b3_R)) - root_RR / (2 * (b3_L - b3_R));
                if (intersect_neg > switch_R && intersect_neg < limit_R)
                {
                    intersects.push_back(intersect_neg);
                }
            }
        }
        intersects.push_back(limit_R);

        // remove duplicate entries from vector
        std::sort(intersects.begin(), intersects.end());
        const auto &it = std::unique(intersects.begin(), intersects.end());
        intersects.resize(std::distance(intersects.begin(), it));

        // subdivide the regions further in order for the score to be accurate
        size_t subdivisions = 10;
        size_t e = intersects.size();
        for (size_t i = 1; i < e; i++)
        {
            float distance = intersects[i] - intersects[i - 1];
            for (size_t j = 1; j < subdivisions; j++)
            {
                float step = distance * j / subdivisions;
                intersects.push_back(intersects[i - 1] + step);
            }
        }
        std::sort(intersects.begin(), intersects.end());
        // trapezoid integral for both features independently
        std::vector<float> areas_L = areas(&intersects, feature_L);
        std::vector<float> areas_R = areas(&intersects, feature_R);

        // scale by the maximum of either L or R at any given point
        std::vector<float> weights;
        weights.reserve(intersects.size() - 1);
        size_t position = 1;
        for (; position < intersects.size(); position++)
        {
            // both left halves
            if (intersects[position] > switch_L)
            {
                break;
            }
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_L + b1_L_L * xval + b2_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < intersects.size(); position++)
        {
            // middle region
            if (intersects[position] > switch_R)
            {
                break;
            }
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_L + b1_R_L * xval + b2_R * xval * xval)));
        }
        for (; position < intersects.size(); position++)
        {
            // both right halves
            float xval = (intersects[position] + intersects[position - 1]) / 2;
            weights.push_back(std::max(exp(b0_L_R + b1_L_R * xval + b3_L * xval * xval),
                                       exp(b0_R_R + b1_R_R * xval + b3_R * xval * xval)));
        }
        // multiply every area with the weight, then sum up and divide for the coefficent
        // @todo check for squre sum score
        assert(areas_L.size() == weights.size());
        float AuB = 0; // area only covered by either A or B
        float AnB = 0; // area covered by both A and B
        for (size_t i = 0; i < weights.size(); i++)
        {
            areas_L[i] *= weights[i];
            areas_R[i] *= weights[i];
            AnB += std::min(areas_L[i], areas_R[i]);
            AuB += abs(areas_L[i] - areas_R[i]);
        }
        auto score = AuB / AnB;
        // assert(0 <= score && score <= 1);
        // @todo this won't work
        return score;
    }

    std::pair<unsigned int, unsigned int> featNums(size_t idx, const PreGrouping *group)
    {
        // the original two indices of a pair:
        // idx = size * idx1 + idx2
        unsigned int idx2 = idx % group->features.size();
        unsigned int idx1 = (idx - idx2) / group->features.size();
        return std::make_pair(idx1, idx2);
    }

    std::vector<size_t> getCompareOrder(const PreGrouping *group)
    {
        assert(group->shapeScores.size() > 0);
        std::vector<size_t> indices(group->shapeScores.size());
        std::iota(indices.begin(), indices.end(), 0);
        {
            std::sort(indices.begin(), indices.end(),
                      [group](size_t lhs, size_t rhs)
                      { return (group->shapeScores)[lhs] > (group->shapeScores)[rhs]; });
            // remove indices where the score is smaller than sensible
            float lowestScore = 0.01;
            size_t breakpoint = 0;
            for (; group->shapeScores[indices[breakpoint]] > lowestScore; breakpoint++)
                ;
            indices.resize(breakpoint + 1);
        }

        // vector to count how many other regressions the one at this index has been combined with
        std::vector<unsigned int> taken(group->features.size(), 0);

        for (size_t i = 0; i < indices.size() / 2; i++)
        {
            auto pair = featNums(indices[2 * i], group); // at this point, two neighbours always have the same score.
            bool left_is_left = group->features[pair.first].origin->retentionTime < group->features[pair.second].origin->retentionTime;
            if (!left_is_left)
            {
                std::swap(pair.first, pair.second);
            }
            // @todo do the regression comparison here
            bool mergeSuccess = true;
            if (mergeSuccess)
            {
                taken[pair.first]++;
                taken[pair.second]++;
            }
        }

        return indices;
    }

    void calcRSS(MovedRegression *reg, const EIC *bin)
    {
        // mutates the RSS field in MovedRegression
#define PREDICT_L(x) (std::exp(reg->b0_L + x * reg->b1_L + x * x * reg->b2)) // @todo there is an error here
#define PREDICT_R(x) (std::exp(reg->b0_R + x * reg->b1_R + x * x * reg->b3))
        const auto RTs = bin->rententionTimes;
        const auto areas = bin->ints_area;
        size_t idx = 0;
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->limit_L)
            {
                break;
            }
        }
        float RSS = 0;
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->RT_switch)
            {
                break;
            }
            float diff = PREDICT_L(RTs[idx]) - areas[idx];
            RSS += diff * diff; // triggers for positive b2 / b3 and large frame, @todo: rework limits
            assert(RSS != INFINITY);
        }
        for (; idx < RTs.size(); idx++)
        {
            if (RTs[idx] >= reg->limit_R)
            {
                break;
            }
            float diff = PREDICT_R(RTs[idx]) - areas[idx];
            RSS += diff * diff;
            assert(RSS != INFINITY);
        }
        assert(RSS != INFINITY);
        reg->RSS = RSS;
    }

    // MultiMatrix combinedMatrix(std::vector<std::vector<float>> *intensities)
    // {
    //     MultiMatrix matrix;
    //     matrix.n_cols = 3 + intensities->size(); // three for b1, b2, b3 and one b0 for every other member
    //     // omit the last value to ensure uneven number of points - this is a bad solution @todo
    //     for (size_t i = 0; i < intensities->size(); i++)
    //     {
    //         if (!(intensities[i].size() % 2))
    //         {
    //             intensities[i].pop_back();
    //         }
    //     }
    // }

    struct IntensityMatrix
    {
        std::vector<std::vector<float>> intensities;
        std::vector<size_t> scanNo;
    };

    IntensityMatrix combineIntensities(const PreGrouping *group, const std::vector<EIC> *bins)
    {
        // combine all features in the group such that their intensities are all
        // accessible in a per-scan setting

        // get the lowest and highest scan number of all component candidates
        size_t lowerBound = INT64_MAX;
        size_t upperBound = 0;
        for (size_t i = 0; i < group->features.size(); i++)
        {
            size_t binID = group->features[i].binID;
            auto bin = (*bins)[binID];
            size_t lowEnd = bin.scanNumbers[group->features[i].origin->idxPeakStart];
            size_t highEnd = bin.scanNumbers[group->features[i].origin->idxPeakEnd];
            lowerBound = std::min(lowerBound, lowEnd);
            upperBound = std::max(upperBound, highEnd);
        }
        // this vector holds the scan numbers per regression
        size_t totalEntries = upperBound - lowerBound + 1;
        std::vector<size_t> scanNo(totalEntries);
        std::iota(scanNo.begin(), scanNo.end(), lowerBound);

        // add a vector of size = number of groups for every relevant scan
        std::vector<float> insert(group->features.size(), 0);
        std::vector<std::vector<float>> elements;
        elements.reserve(totalEntries);
        for (size_t i = 0; i < totalEntries; i++)
        {
            elements.push_back(insert);
        }

        // fill the sub-vectors with relevant intensities, intensity 0 being equivalent to "not found"
        for (size_t i = 0; i < group->features.size(); i++)
        {
            size_t binID = group->features[i].binID;
            auto bin = (*bins)[binID];
            auto scan = bin.scanNumbers;
            auto area = bin.ints_area;
            for (size_t j = 0; j < scan.size(); j++)
            {
                // iterate through the bin and add only those intensities with relevant scans
                if (scan[j] < lowerBound)
                {
                    continue;
                }
                if (scan[j] > upperBound)
                {
                    break;
                }
                size_t scanIdx = scan[j] - lowerBound;
                elements[scanIdx][i] = area[j];
            }
        }
        return IntensityMatrix{}; // @todo remove this entire function
    }

    std::vector<std::vector<float>> designMat(int scale, size_t k) // , const std::vector<size_t> *eliminate
    {
        // k is the number of features combined, left as k due to similar naming in adapted code
        // eliminate skips all rows whose indices are included.
        assert(scale > 1);
        assert(k > 1);
        size_t colLenght = 2 * scale + 1;
        size_t vecSize = colLenght * k;
        std::vector<std::vector<float>> resultMat;
        // add one vector b0 for every feature. b0 is 0 everywhere except for the region that contains the feature
        for (size_t i = 0; i < k; i++)
        {
            std::vector<float> b0(vecSize);
            for (size_t j = i; j < colLenght; j++)
            {
                b0[j] = 1;
            }
            resultMat.push_back(b0);
        }

        std::vector<float> b1(vecSize, 0);
        for (size_t i = 0; i < k; i++)
        {
            size_t offset = i * colLenght;
            std::iota(b1.begin() + offset, b1.begin() + offset + colLenght, -scale);
        }
        resultMat.push_back(b1);
        std::vector<float> b2(vecSize, 0);
        for (size_t j = 0; j < k; j++)
        {
            for (int i = 0; i < scale + 1; i++)
            {
                size_t offset = j * colLenght;
                b2[i + offset] = b1[i + offset] * b1[i + offset];
            }
        }
        std::vector<float> b3(vecSize, 0);
        for (size_t j = 0; j < k; j++)
        {
            for (size_t i = scale + 1; i < colLenght; i++)
            {
                size_t offset = j * (colLenght - 1);
                b3[i + offset] = b1[i + offset] * b1[i + offset];
            }
        }
        resultMat.push_back(b2);
        resultMat.push_back(b3);
        // @todo add a correctness check
        return resultMat;
    }

    // void singleFit(std::vector<float> *logIntensity) // this was only included for the residuals, replace
    // {
    //     // the model requires an uneven number of points, this is a suboptimal solution
    //     // int scale = bool(logIntensity->size() % 2) ? logIntensity->size() / 2 : logIntensity->size() / 2 - 1;

    //     if (!logIntensity->size() % 2)
    //     {
    //         logIntensity->resize(logIntensity->size() - 1);
    //     }
    //     int scale = logIntensity->size() / 2;
    //     // auto design = designMat(scale);
    // }

    //     void multiFit(const std::vector<ReducedEIC> *fitRegion, const std::vector<MovedRegression> *peaks)
    //     {
    //         assert(fitRegion->size() == peaks->size());
    //         // fit one model with variable b0 over multiple traces
    //         // fitRegion contains all mass traces over which a fit should be performed, including zero padding
    //         // find the scale that fits the non-0 intensity region, -1 at the lowest intensity if an even number is found
    //         size_t k = fitRegion->size();
    //         size_t limit_L = 0;
    //         size_t limit_R = fitRegion->front().intensity.size();
    //         // to decide in the case of an uneven region, keep track of the total intensities at the two outer points
    //         float sumInts_L = -1;
    //         float sumInts_R = -1;
    //         for (size_t i = 0; i < fitRegion->front().intensity.size(); i++)
    //         {
    //             // left half
    //             float sumInts = 0;
    //             for (size_t j = 0; j < k; j++)
    //             {
    //                 sumInts += fitRegion->at(j).intensity_log[i];
    //             }
    //             if (sumInts != 0)
    //             {
    //                 sumInts_L = sumInts;
    //                 limit_L = i;
    //                 break;
    //             }
    //         }
    //         for (size_t i = fitRegion->front().intensity.size() - 1; i > limit_L; i--)
    //         {
    //             // left half
    //             float sumInts = 0;
    //             for (size_t j = 0; j < k; j++)
    //             {
    //                 sumInts += fitRegion->at(j).intensity_log[i];
    //             }
    //             if (sumInts != 0)
    //             {
    //                 sumInts_R = sumInts;
    //                 limit_R = i;
    //                 break;
    //             }
    //         }
    //         assert(limit_R - limit_L >= 4);
    //         // if there is an even number of points, the difference of the limits is uneven
    //         if (((limit_R - limit_L) % 2) == 1)
    //         {
    //             // adjust the window while minimising the intensity loss
    //             if (sumInts_L > sumInts_R)
    //             {
    //                 limit_R -= 1;
    //             }
    //             else
    //             {
    //                 limit_L += 1;
    //             }
    //         }
    //         assert(limit_R - limit_L >= 4);

    //         const size_t scale = (limit_R - limit_L + 1) / 2; // +1 since limits are indices
    //         // produce the intensity vector (without zeroes) while keeping track of which rows to interpolate
    //         // std::vector<size_t> interpolate;
    //         std::vector<float> combined_logInt((2 * scale + 1) * k, 0);
    //         // interpolate.reserve(combined_logInt.size() / 10);
    //         size_t logIdx = 0;
    //         for (size_t i = 0; i < k; i++)
    //         {
    //             ReducedEIC current = (*fitRegion)[i];
    //             auto coeffs = peaks->at(i);
    // #define INTERPOLATE_L(x) (coeffs.b0_L + coeffs.b1_L * x + coeffs.b2 * x * x) // no need for exp since we work with the log for regressions anyway
    // #define INTERPOLATE_R(x) (coeffs.b0_R + coeffs.b1_R * x + coeffs.b3 * x * x)
    //             for (size_t j = limit_L; j < limit_R + 1; j++)
    //             {
    //                 combined_logInt[logIdx] = current.intensity_log[j];
    //                 if (current.intensity_log[j] == 0)
    //                 {
    //                     float rt = current.RTs[j];
    //                     combined_logInt[logIdx] = rt < coeffs.RT_switch ? INTERPOLATE_L(rt) : INTERPOLATE_R(rt);
    //                     // interpolate.push_back(logIdx);
    //                 }
    //                 logIdx++;
    //             }
    // #undef INTERPOLATE_L
    // #undef INTERPOLATE_R
    //         }
    //         // assert(combined_logInt.back() != 0); // this is possible, although hopefully unlikely
    //         auto matrix = designMat(scale, k);
    //         assert(matrix[0].size() == combined_logInt.size());
    //         // set all entries of the matrix were no intensity values exist to 0
    //         // for (size_t i = 0; i < matrix.size(); i++)
    //         // {
    //         //     for (size_t j = 0; j < interpolate.size(); j++)
    //         //     {
    //         //         matrix[i][j] = 0;
    //         //     }
    //         // }
    //     }

    bool preferMerge(float rss_complex, float rss_simple, size_t n_total, size_t p_complex, size_t p_simple)
    {
        assert(rss_complex < rss_simple);
        // @todo consider if this part of the code can be sped up by hashing the computations dependent on dfn and dfd
        float alpha = 0.05; // @todo is a set alpha really the best possible solution?
        // problem: pre-calculation of all relevant f values could result in a very large array
        // possible max size of 20 seems reasonable, maximum observed is 6
        int which = 1; // select mode of library function and check computation result
        double p = 0;  // not required
        double q = 0;  // return value, equals p - 1
        double F = ((rss_simple - rss_complex) / float(p_complex - p_simple)) / (rss_complex / float(n_total - p_complex));
        double dfn = p_complex - p_simple; // numerator degrees of freedom
        double dfd = n_total - p_complex;  // denominator degrees of freedom
        int status = 1;                    // result invalid if this is not 0
        double bound = 0;                  // allows recovery from non-0 status

        cdff(&which, &p, &q, &F, &dfn, &dfd, &status, &bound); // library function, see https://people.math.sc.edu/Burkardt/cpp_src/cdflib/cdflib.html
        assert(status == 0);

        return q > alpha; // the merged model is not worse than the complex version, both can be merged
    }
}
