// qalgorithm_measurement_data.cpp

// internal
#include "../include/qalgorithms_measurement_data.h"
#include "../include/qalgorithms_utils.h"

// external
#include <cmath>
#include <fstream>
#include <algorithm>

namespace qAlgorithms
{
  std::vector<std::vector<CentroidPeak>> transferCentroids(
      sc::MZML &data,
      std::vector<int> &indices,
      std::vector<double> &retention_times,
      const int start_index,
      double PPMerror)
  {
    std::vector<std::vector<CentroidPeak>> centroids(indices.size());
    // #pragma omp parallel for
    for (size_t i = 0; i < indices.size(); ++i) // loop over all indices
    {
      const int index = indices[i]; // spectrum index
      if (index < start_index)
      {
        continue; // skip due to index
      }
      // get the spectrum
      alignas(64) std::vector<std::vector<double>> spectrum = data.get_spectrum(index);
      for (size_t j = 0; j < spectrum[0].size(); j++)
      {
        if (spectrum[1][j] < 750)
        {
          continue;
        }

        centroids[i].push_back(CentroidPeak());
        centroids[i].back().scanNumber = index;
        centroids[i].back().mz = spectrum[0][j];
        centroids[i].back().area = spectrum[1][j];
        centroids[i].back().height = spectrum[1][j];
        // centroids[i].back().retentionTime = retention_times[i]; // @todo fix this
        centroids[i].back().dqsCen = -1.0;
        centroids[i].back().mzUncertainty = spectrum[0][j] * PPMerror * 10e-6; // 0.25 ppm default
      }
    }
    return centroids;
  }

  double calcExpectedDiff(std::vector<double> &data)
  {
    const int numPoints = data.size();                 // number of data points
    int upperLimit = numPoints - 95 * numPoints / 100; // check lowest 5% for expected difference
    double expectedDifference = 0.0;
    // static approach: mean of the 8 lowest distances @todo why 8?
    std::vector<double> differences(numPoints - 1);
    for (int i = 0; i < numPoints - 1; i++)
    {
      differences[i] = data[i + 1] - data[i];
    }
    std::sort(differences.begin(), differences.end());
    for (int i = 0; i < upperLimit; i++)
    {
      expectedDifference += differences[i];
    }
    expectedDifference /= upperLimit;

    return expectedDifference;
  }

  treatedData pretreatData(
      std::vector<dataPoint> &dataPoints,
      std::vector<unsigned int> &binIdx,
      float expectedDifference,
      const bool updateExpectedDifference)
  {

    // lambda function to calculate the coefficients b0, b1, and b2 for the quadratic extrapolation
    // @todo move this to where it is called
    auto calculateCoefficients = [](const float *x, const float *y, float &b0, float &b1, float &b2)
    {
      float x1 = x[0], y1 = y[0];
      float x2 = x[1], y2 = y[1];
      float x3 = x[2], y3 = y[2];

      float denom = (x1 - x2) * (x1 - x3) * (x2 - x3);

      float a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
      float b = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
      float c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

      b0 = c;
      b1 = b;
      b2 = a;
    };

    treatedData treatedData = {std::vector<dataPoint>(), std::vector<int>()}; // treated data
    treatedData.dataPoints.reserve(dataPoints.size() * 2);                    // reserve memory for the new data points
    auto it_dataPoint = dataPoints.begin();                                   // iterator for the data points
    const auto it_dataPoint_end = dataPoints.end() - 1;                       // end of the data points (last element is infinty point)
    auto it_maxOfBlock = dataPoints.begin();                                  // maximum of the block (y-axis)
    int blockSize = 0;                                                        // size of the current block

    binIdx.reserve(dataPoints.size() * 2);

    unsigned int realIdx = 0;
    // add the first to zeros to the dataPoints_new vector
    for (int i = 0; i < 2; i++)
    {
      treatedData.addDataPoint(0.f, 0.f, false, 0.f, 0.f, 0, 0.f);
      binIdx.push_back(realIdx);
      assert(binIdx.size() == treatedData.dataPoints.size());
    }
    treatedData.addSeparator(0); // add the first separator

    // iterate over the data points
    for (; it_dataPoint != it_dataPoint_end; it_dataPoint++)
    {
      blockSize++;
      treatedData.dataPoints.push_back(*it_dataPoint);
      binIdx.push_back(realIdx);
      assert(binIdx.size() == treatedData.dataPoints.size());
      ++realIdx;
      const float dx = (it_dataPoint + 1)->x - it_dataPoint->x;
      if (dx > 1.75 * expectedDifference)
      { // gap detected

        const int gapSize = static_cast<int>(dx / expectedDifference) - 1;
        if (gapSize < 4)
        {
          // add gapSize interpolated datapoints
          const float dy = std::pow((it_dataPoint + 1)->y / it_dataPoint->y, 1.0 / (gapSize + 1)); // dy for log interpolation
          for (int i = 1; i <= gapSize; i++)
          {
            binIdx.push_back(realIdx);
            treatedData.addDataPoint(
                it_dataPoint->x + i * expectedDifference, // x-axis
                it_dataPoint->y * std::pow(dy, i),        // y-axis
                false,                                    // df
                0.f,                                      // dqsCentroid
                0.f,                                      // dqsBinning
                0,                                        // scanNumber
                0.f);                                     // mz
          }
          assert(binIdx.size() == treatedData.dataPoints.size());
        }
        else
        { // END OF BLOCK, EXTRAPOLATION STARTS
          // add 4 datapoints (two extrapolated [end of current block] and two zeros
          // [start of next block]) extrapolate the first two datapoints of this block
          if (blockSize < 5)
          {
            // delete all data points of the block in treatedData.dataPoints except the
            // first two zeros marked by the separator.back()+2
            auto it_startOfBlock = treatedData.dataPoints.begin() + treatedData.separators.back() + 2;
            treatedData.dataPoints.erase(it_startOfBlock, treatedData.dataPoints.end());
            while (binIdx.size() != treatedData.dataPoints.size())
            {
              binIdx.pop_back();
            }
            assert(binIdx.size() == treatedData.dataPoints.size());
          }
          else
          {
            const dataPoint dp_startOfBlock = treatedData.dataPoints[treatedData.separators.back() + 2];
            // check if the maximum of the block is the first or last data point
            if (it_maxOfBlock == it_dataPoint || it_maxOfBlock->x == dp_startOfBlock.x)
            {
              // extrapolate the left side using the first non-zero data point (i.e, the start of the block)
              for (int i = 0; i < 2; i++)
              {
                // LEFT SIDE
                dataPoint &dp_left = treatedData.dataPoints[treatedData.separators.back() + i];
                dp_left.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                dp_left.y = dp_startOfBlock.y;

                // RIGHT SIDE
                treatedData.addDataPoint(
                    it_dataPoint->x + (i + 1) * expectedDifference, // x-axis
                    it_dataPoint->y,                                // y-axis
                    false,                                          // df
                    0.f,                                            // dqsCentroid
                    0.f,                                            // dqsBinning
                    0,                                              // scanNumber
                    0.f);                                           // mz

                binIdx.push_back(realIdx);
                assert(binIdx.size() == treatedData.dataPoints.size());
              }
            }
            else
            {
              const float x[3] = {0.f, it_maxOfBlock->x - dp_startOfBlock.x, it_dataPoint->x - dp_startOfBlock.x};
              const float y[3] = {std::log(dp_startOfBlock.y), std::log(it_maxOfBlock->y), std::log(it_dataPoint->y)};
              float b0, b1, b2;
              calculateCoefficients(x, y, b0, b1, b2);
              // extrapolate the left side of the block
              for (int i = 0; i < 2; i++)
              {
                dataPoint &curr_dp = treatedData.dataPoints[treatedData.separators.back() + i];
                curr_dp.x = dp_startOfBlock.x - (2 - i) * expectedDifference;
                const float x = curr_dp.x - dp_startOfBlock.x;
                curr_dp.y = std::exp(b0 + x * (b1 + x * b2));
              }
              // add the extrapolated data points to the right side of the block
              for (int i = 0; i < 2; i++)
              {
                const float dp_x = it_dataPoint->x + (i + 1) * expectedDifference;
                const float x = dp_x - dp_startOfBlock.x;
                treatedData.addDataPoint(
                    dp_x,                             // x-axis
                    std::exp(b0 + x * (b1 + x * b2)), // y-axis
                    false,                            // df
                    0.f,                              // dqsCentroid
                    0.f,                              // dqsBinning
                    0,                                // scanNumber
                    0.f);                             // mz
                binIdx.push_back(realIdx);
                assert(binIdx.size() == treatedData.dataPoints.size());
              }
            }
            // add the zeros to the treatedData.dataPoints vector to start the next block
            for (int i = 0; i < 2; i++)
            {
              treatedData.addDataPoint(0.f, 0.f, false, 0.f, 0.f, 0, 0.f);
              binIdx.push_back(realIdx);
              assert(binIdx.size() == treatedData.dataPoints.size());
            }
            treatedData.addSeparator(treatedData.dataPoints.size() - 2); // add the separator
            it_maxOfBlock = it_dataPoint + 1;                            // update the maximum of the block
          }
          blockSize = 0; // reset the block size
        }
      } //
      else
      {
        // binIdx.push_back(realIdx);
        // realIdx++;
        if (it_maxOfBlock->y < it_dataPoint->y)
        {
          it_maxOfBlock = it_dataPoint; // update the maximum of the block
        }
        if (updateExpectedDifference && dx > .75 * expectedDifference)
        {
          expectedDifference = (expectedDifference + dx) * .5; // update the expected difference
        }
      }
    } // end of for loop
    assert(binIdx.size() == treatedData.dataPoints.size());

    binIdx.pop_back();
    binIdx.pop_back();

    // delete the last two zeros // @todo why?
    treatedData.dataPoints.pop_back();
    treatedData.dataPoints.pop_back();
    // change the last separator to the end of the dataPoints vector
    treatedData.separators.back() = treatedData.dataPoints.size();
    assert(binIdx.size() == treatedData.dataPoints.size());

    return treatedData;
  } // end of pretreatData
}