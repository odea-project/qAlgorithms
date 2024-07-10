// qalgorithms_qpeaks.cpp
//
// internal
#include "../include/qalgorithms_qpeaks.h"

// external

namespace q
{
  namespace Algorithms
  {
#pragma region Constructors and Destructor
    // Constructor
    qPeaks::qPeaks() {}

    qPeaks::qPeaks(const q::MeasurementData::varDataType &dataVec)
    {
      std::visit(
          [this](auto &&arg)
          {
            // push two nullptr to the designMatrices, inverseMatrices, and psuedoInverses vectors to store the matrices for scale 2
            for (int i = 0; i < 2; i++)
            {
              this->designMatrices.push_back(nullptr);
              this->inverseMatrices.push_back(nullptr);
              this->psuedoInverses.push_back(nullptr);
            }

            this->global_maxScale = 15;
            // iterate over the range of the maxScale to create Matrices for each scale
            for (int scale = 2; scale <= this->global_maxScale; scale++)
            {
              createDesignMatrix(scale);
              createInverseAndPseudoInverse(*(designMatrices.back()));
            }
          },
          dataVec);
    }

    // Destructor
    qPeaks::~qPeaks() {}
#pragma endregion Constructors and Destructor

#pragma region "peakListToCSV"
    void
    qPeaks::peakListToCSV(
        const std::vector<std::vector<std::unique_ptr<DataType::Peak>>> &allPeaks,
        const std::string &filename) const
    {
      // check if the file already exists
      if (std::ifstream file(filename); file)
      {
        // if the file exists, remove the file
        std::remove(filename.c_str());
      }
      // open the output file
      std::ofstream file(filename);
      // write the header
      file << "SampleID,Position,Height,Area,PositionUncertainty,HeightUncertainty,AreaUncertainty,DQS,Beta0,Beta1,Beta2,Beta3,DF,X0,dx\n";
      // iterate over the allPeaks vector
      for (const auto &peaks : allPeaks)
      {
        // iterate over the peaks vector
        for (const auto &peak : peaks)
        {
          // write the peak information to the file
          file << peak->sampleID << ","
               << peak->position << ","
               << peak->height << ","
               << peak->area << ","
               << peak->positionUncertainty << ","
               << peak->heightUncertainty << ","
               << peak->areaUncertainty << ","
               << peak->dqsPeak << ","
               << peak->beta0 << ","
               << peak->beta1 << ","
               << peak->beta2 << ","
               << peak->beta3 << ","
               << peak->df << ","
               << peak->x0 << ","
               << peak->dx << "\n";
        }
      }
      // close the output file
      file.close();
    }
#pragma endregion "peakListToCSV"

#pragma region "plotPeaksToPython"
    void
    qPeaks::plotPeaksToPython(
        const std::string &filename_input,
        const std::string &filename_output,
        const bool includeFits,
        const bool featureMap) const
    {
      // check if python is installed
      if (system("python --version"))
      {
        std::cerr << "Python is not installed on this system. Please install Python to use this function.\n";
        return;
      }
      // check if python in the path
      if (system("python -c \"import sys\""))
      {
        std::cerr << "Python is not in the path. Please add Python to the path to use this function.\n";
        return;
      }
      std::string command = "python python/plotPeaksToPython.py " + filename_input + " " + filename_output + " " + std::to_string(includeFits) + " " + std::to_string(featureMap);
      std::string result = exec(command.c_str());
      std::cout << result;
    }
#pragma endregion "plotPeaksToPython"

#pragma region "initialize"
    alignas(16) float q::Algorithms::qPeaks::x_square[128];   // array to store the square of the x values
    alignas(16) float q::Algorithms::qPeaks::invArray[64][6]; // array to store the 6 unique values of the inverse matrix for each scale
    __m128 q::Algorithms::qPeaks::ZERO_128;                   // [0., 0., 0., 0.]
    __m256 q::Algorithms::qPeaks::ZERO_256;                   // [0., 0., 0., 0., 0., 0., 0., 0.]
    __m128 q::Algorithms::qPeaks::KEY_128;                    // [0., 4., 2., 1.]
    __m256 q::Algorithms::qPeaks::LINSPACE_UP_POS_256;        // [7., 6., 5., 4., 3., 2., 1., 0.]
    __m256 q::Algorithms::qPeaks::LINSPACE_UP_NEG_256;        // [-7., -6., -5., -4., -3., -2., -1., 0.]
    __m256 q::Algorithms::qPeaks::LINSPACE_DOWN_NEG_256;      // [0., -1., -2., -3., -4., -5., -6., -7.]
    __m256i q::Algorithms::qPeaks::LINSPACE_UP_INT_256;       // [7, 6, 5, 4, 3, 2, 1, 0]
    __m256i q::Algorithms::qPeaks::LINSPACE_DOWN_INT_256;     // [0, 1, 2, 3, 4, 5, 6, 7]
    __m256 q::Algorithms::qPeaks::MINUS_ONE_256;              // [-1., -1., -1., -1., -1., -1., -1., -1.]
    void qPeaks::initialize()
    {

      ZERO_128 = _mm_setzero_ps();
      ZERO_256 = _mm256_setzero_ps();
      KEY_128 = _mm_set_ps(1.f, 2.f, 4.f, 0.f);
      LINSPACE_UP_POS_256 = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
      LINSPACE_UP_NEG_256 = _mm256_set_ps(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
      LINSPACE_DOWN_NEG_256 = _mm256_set_ps(0.f, -1.f, -2.f, -3.f, -4.f, -5.f, -6.f, -7.f);
      LINSPACE_UP_INT_256 = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
      LINSPACE_DOWN_INT_256 = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
      MINUS_ONE_256 = _mm256_set1_ps(-1.f);
      for (int i = 0; i < 128; ++i)
      {
        x_square[i] = i * i;
      }
      // init invArray
      float XtX_00 = 1.f;
      float XtX_02 = 0.f;
      float XtX_11 = 0.f;
      float XtX_12 = 0.f;
      float XtX_13 = 0.f;
      float XtX_22 = 0.f;
      for (int i = 1; i < 64; ++i)
      {
        XtX_00 += 2.f;
        XtX_02 += x_square[i];
        XtX_11 = XtX_02 * 2.f;
        XtX_13 += x_square[i] * i;
        XtX_12 = -XtX_13;
        XtX_22 += x_square[i] * x_square[i];

        float L_00 = std::sqrt(XtX_00);
        float L_11 = std::sqrt(XtX_11);
        float L_20 = XtX_02 / L_00;
        float L_21 = XtX_12 / L_11;
        float L_20sq = L_20 * L_20;
        float L_21sq = L_21 * L_21;
        float L_22 = std::sqrt(XtX_22 - L_20sq - L_21sq);
        float L_32 = 1 / L_22 * (-L_20sq + L_21sq);
        float L_33 = std::sqrt(XtX_22 - L_20sq - L_21sq - L_32 * L_32);

        float inv_00 = 1.f / L_00;
        float inv_11 = 1.f / L_11;
        float inv_22 = 1.f / L_22;
        float inv_33 = 1.f / L_33;
        float inv_20 = -L_20 * inv_00 / L_22;
        float inv_30 = -(L_20 * inv_00 + L_32 * inv_20) / L_33;
        float inv_21 = -L_21 * inv_11 / L_22;
        float inv_31 = -(-L_21 * inv_11 + L_32 * inv_21) / L_33;
        float inv_32 = -L_32 * inv_22 / L_33;

        invArray[i][0] = inv_00 * inv_00 + inv_20 * inv_20 + inv_30 * inv_30; // cell: 0,0
        invArray[i][1] = inv_22 * inv_20 + inv_32 * inv_30;                   // cell: 0,2
        invArray[i][2] = inv_11 * inv_11 + inv_21 * inv_21 + inv_31 * inv_31; // cell: 1,1
        invArray[i][3] = -inv_31 * inv_33;                                    // cell: 1,2
        invArray[i][4] = inv_33 * inv_33;                                     // cell: 2,2
        invArray[i][5] = inv_32 * inv_33;                                     // cell: 2,3
      }
    }
#pragma endregion "initialize"

#pragma region findPeaks
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
    qPeaks::findPeaks(
        const q::MeasurementData::varDataType &dataVec)
    {
      std::vector<std::vector<std::unique_ptr<DataType::Peak>>> all_peaks;
      std::visit(
          [&all_peaks, this](auto &&arg)
          {
          all_peaks.resize(arg->size());
        // iterate over the map of varDataType datatype objects
#pragma omp parallel for // use parallel for loop to iterate over the dataVec
          for (size_t i = 0; i < arg->size(); i++)
          {
            // de-reference the unique pointer of the object
            auto &pair =  (*arg)[i];         // pair is a unique pointer to a datatype object
            auto &dataObj = *(pair.get());   // dataObj is a datatype object
            auto &data = dataObj.dataPoints; // data is a vector of unique pointers to data points structures
            int n = data.size();
            if (n < 5)
            {
              continue; // not enough data points to fit a quadratic regression model
            }

            // store x and y values in RefMatrix objects
            alignas(32) q::Matrices::Vector Y(n);
            alignas(32) q::Matrices::Vector X(n);
            q::Matrices::BoolVector df(n);
            for (int i = 0; i < n; i++)
            {
              X[i] = data[i]->x();
              df[i] = data[i]->df;
              Y[i] = data[i]->y();
            }
            const bool *df_start = df.begin();
            const float *y_start = Y.begin();
            const float *x_start = X.begin();
            std::vector<std::unique_ptr<validRegression>> validRegressions;
            runningRegression(y_start, df_start, n, validRegressions);
            if (validRegressions.empty())
            {
              continue; // no valid peaks
            }
            all_peaks[i] = createPeaks(validRegressions, y_start, x_start, dataObj.getScanNumber());
          } // end parallel for loop
        ; },
          dataVec); // end visit
      // create the sorted peak list
      return createPeakList(all_peaks);
      return all_peaks;
    } // end findPeaks
#pragma endregion findPeaks

#pragma region runningRegression
    void qPeaks::runningRegression(
        const float *y_start,
        const bool *df_start,
        const int n,
        std::vector<std::unique_ptr<validRegression>> &validRegressions)
    {
      alignas(32) q::Matrices::Vector Ylog = logn(y_start, y_start+n); // perform log-transform on Y
      const float *ylog_start = Ylog.begin();
      int maxScale = std::min(this->global_maxScale, (int)(n - 1) / 2);
      validRegressions.reserve(calculateNumberOfRegressions(n));
      alignas(16) float ylog_array[512];
      if (n <= 512)
      {
        for (size_t i = 0; i < n; i++)
        {
          ylog_array[i] = Ylog[i];
        }
      }
      for (int scale = 2; scale <= maxScale; scale++)
      {
        const size_t k = 2 * scale + 1;                    // window size
        const size_t n_segments = n - k + 1;               // number of segments, i.e. regressions
        alignas(16) __m128 *beta = new __m128[n_segments]; // coefficients matrix

        (n <= 512) ? convolve_static(scale, ylog_array, n, beta) : convolve_dynamic(scale, Ylog.elements, n, beta);
        validateRegressions(beta, n_segments, y_start, ylog_start, df_start, scale, validRegressions);
        delete[] beta;
      } // end for scale loop
      mergeRegressionsOverScales(validRegressions, y_start, ylog_start, df_start);
    } // end runningRegression
#pragma endregion runningRegression

#pragma region validateRegressions
    void qPeaks::validateRegressions(
        const __m128 *beta,                // coefficients matrix
        const size_t n_segments,           // number of segments, i.e. regressions
        const float *y_start,              // pointer to the start of the Y matrix
        const float *ylog_start,           // pointer to the start of the Ylog matrix
        const bool *df_start, // degree of freedom vector, 0: interpolated, 1: measured
        const int scale,                   // scale, i.e., the number of data points in a half window excluding the center point
        std::vector<std::unique_ptr<validRegression>> &validRegressions)
    {
      // declare constants
      const double inverseMatrix_2_2 = invArray[scale][4]; // variance of the quadratic term left side of the peak

      // declare variables
      std::vector<std::unique_ptr<validRegression>> validRegressionsTmp; // temporary vector to store valid regressions <index, apex_position>

      // iterate columwise over the coefficients matrix beta
      for (size_t i = 0; i < n_segments; i++)
      {
        const __m128 &coeff = beta[i]; // coefficient register from beta @ i

        /*
          Degree of Freedom Filter:
          This block of code implements the degree of freedom filter. It calculates the degree of freedom based df vector. If the degree of freedom is less than 5, the loop continues to the next iteration. The value 5 is chosen as the minimum number of data points required to fit a quadratic regression model.
        */
        int df_sum = calcDF(df_start, i, 2 * scale + i); // calculate the sum of the degree of freedom (df_sum)
        if (df_sum < 5)
        {
          continue; // degree of freedom less than 5; i.e., less then 5 measured data points
        }

        /*
          Apex and Valley Position Filter:
          This block of code implements the apex and valley position filter. It calculates the apex and valley positions based on the coefficients matrix B. If the apex is outside the data range, the loop continues to the next iteration. If the apex and valley positions are too close to each other, the loop continues to the next iteration.
        */
        double apex_position, valley_position;
        if (!calculateApexAndValleyPositions(coeff, scale, apex_position, valley_position))
        {
          continue; // invalid apex and valley positions
        }

        /*
          Degree of Freedom Filter:
          This block of code implements the degree of freedom filter. It calculates the degree of freedom based df vector. If the degree of freedom is less than 5, the loop continues to the next iteration. The value 5 is chosen as the minimum number of data points required to fit a quadratic regression model.
        */
        double left_limit = (valley_position < 0) ? std::max((double)i, valley_position + i + scale) : i;
        double right_limit = (valley_position > 0) ? std::min((double)i + 2 * scale, valley_position + i + scale) : i + 2 * scale;
        df_sum = calcDF(df_start, left_limit, right_limit); // update the degree of freedom considering the left and right limits
        if (df_sum < 5)
        {
          continue; // degree of freedom less than 5; i.e., less then 5 measured data points
        }

        /*
          Area Pre-Filter:
          This test is used to check if the later-used arguments for exp and erf functions are within the valid range, i.e., |x^2| < 25. If the test fails, the loop continues to the next iteration. x is in this case -apex_position * b1 / 2 and -valley_position * b1 / 2.
        */
        if (apex_position * ((float *)&coeff)[1] > 50 || valley_position * ((float *)&coeff)[1] < -50)
        {
          continue; // invalid area pre-filter
        }

        /*
          Apex to Edge Filter:
          This block of code implements the apex to edge filter. It calculates the ratio of the apex signal to the edge signal and ensures that the ratio is greater than 2. This is a pre-filter for later signal-to-noise ratio checkups.
        */
        float apexToEdge = 0.f;
        if (!isValidApexToEdge(apex_position, scale, static_cast<int>(i), y_start, apexToEdge))
        {
          continue; // invalid apex to edge ratio
        }

        /*
          Quadratic Term Filter:
          This block of code implements the quadratic term filter. It calculates the mean squared error (MSE) between the predicted and actual values. Then it calculates the t-value for the quadratic term. If the t-value is less than the corresponding value in the tValuesArray, the quadratic term is considered statistically insignificant, and the loop continues to the next iteration.
        */
        const float mse = calcSSE(-scale, scale, coeff, ylog_start + i) / (df_sum - 4); // mean squared error

        if (!isValidQuadraticTerm(coeff, inverseMatrix_2_2, inverseMatrix_2_2, mse, df_sum))
        {
          continue; // statistical insignificance of the quadratic term
        }
        /*
          Height Filter:
          This block of code implements the height filter. It calculates the height of the peak based on the coefficients matrix B. Then it calculates the uncertainty of the height based on the Jacobian matrix and the variance-covariance matrix of the coefficients. If the height is statistically insignificant, the loop continues to the next iteration.
        */
        double uncertainty_height = 0.0; // at this point without height, i.e., to get the real uncertainty multiply with height later. This is done to avoid exp function at this point
        if (!isValidPeakHeight(mse, i, scale, apex_position, valley_position, df_sum, apexToEdge, uncertainty_height))
        {
          continue; // statistical insignificance of the height
        }

        /*
          Area Filter:
          This block of code implements the area filter. It calculates the Jacobian matrix for the peak area based on the coefficients matrix B. Then it calculates the uncertainty of the peak area based on the Jacobian matrix. If the peak area is statistically insignificant, the loop continues to the next iteration.
          NOTE: this function does not consider b0: i.e. to get the real uncertainty and area multiply both with Exp(b0) later. This is done to avoid exp function at this point
        */
        double area = 0.0;
        double uncertainty_area = 0.0;
        if (!isValidPeakArea(coeff, mse, scale, df_sum, area, uncertainty_area))
        {
          continue; // statistical insignificance of the area
        }

        /*
          Chi-Square Filter:
          This block of code implements the chi-square filter. It calculates the chi-square value based on the weighted chi squared sum of expected and measured y values in the exponential domain. If the chi-square value is less than the corresponding value in the chiSquareArray, the loop continues to the next iteration.
        */
        float chiSquare = calcSSE(-scale, scale, coeff, y_start + i, true, true);
        if (chiSquare < chiSquareArray[df_sum - 5])
        {
          continue; // statistical insignificance of the chi-square value
        }

        // at this point, the peak is validated
        /*
          Add to a temporary vector of valid regressions:
          This block of code adds the valid peak to a temporary vector of valid regressions. It calculates the left and right limits of the peak based on the valley position. Then it stores the index of the valid regression in the temporary vector of valid regressions.
        */
        validRegressionsTmp.push_back(
            std::make_unique<validRegression>(
                i + scale,                 // index of the center of the window (x==0) in the Y matrix
                scale,                     // scale
                df_sum - 4,                // df
                apex_position + i + scale, // apex position in x-axis 0:n
                0,                         // initial MSE
                coeff,                     // coefficients register
                true,                      // isValid
                left_limit,                // left_limit
                right_limit,               // right_limit
                area,                      // peak area
                uncertainty_height,        // uncertainty of the peak height
                uncertainty_area           // uncertainty of the peak area
                ));
      } // end for loop
      // early return if no or only one valid peak
      if (validRegressionsTmp.size() < 2)
      {
        if (validRegressionsTmp.empty())
        {
          return; // no valid peaks
        }
        validRegressions.push_back(std::move(validRegressionsTmp[0]));
        return; // not enough peaks to form a group
      }
      /*
        Grouping:
        This block of code implements the grouping. It groups the valid peaks based on the apex positions. Peaks are defined as similar, i.e., members of the same group, if they fullfill at least one of the following conditions:
        - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
        - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
      */

      std::vector<std::vector<std::unique_ptr<validRegression>>> regressionGroups; // vector of vectors of valid regressions, i.e., groups of valid regressions

      auto it_peak = validRegressionsTmp.begin(); // iterator for the peak
      auto it_peak_next = std::next(it_peak);     // iterator for the next peak

      while (it_peak_next != validRegressionsTmp.end()) // iterate over the temporary vector of valid regressions
      {
        // check if the difference between two peak apexes is less than 4 (Nyquist Shannon Sampling Theorem, separation of two maxima), or if the apex of a peak is within the window of the other peak (Overlap of two maxima)
        if (
            std::abs((*it_peak)->apex_position - (*it_peak_next)->apex_position) > 4 && // Nyquist Shannon Sampling Theorem, separation of two maxima
            (*it_peak)->apex_position < (*it_peak_next)->left_limit &&                  // Left peak is not within the window of the right peak
            (*it_peak_next)->apex_position > (*it_peak)->right_limit                    // Right peak is not within the window of the left peak
        )
        {                                                                                                     // the two regressions differ, i.e. create a new group and move all regressions from start to it_peak to this group
          regressionGroups.push_back(std::vector<std::unique_ptr<q::Algorithms::qPeaks::validRegression>>()); // create a new group
          std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back()));  // move all regressions from start to it_peak to this group
          it_peak_next = validRegressionsTmp.erase(validRegressionsTmp.begin(), it_peak_next);                // erase the moved regressions from the temporary vector of valid regressions
        }
        it_peak = it_peak_next;
        ++it_peak_next;
        if (it_peak_next == validRegressionsTmp.end())
        {                                                                                                     // the last peak is reached
          regressionGroups.push_back(std::vector<std::unique_ptr<q::Algorithms::qPeaks::validRegression>>()); // create a new group
          std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back()));  // move all regressions from start to it_peak to this group
        }
      } // end while loop

      /*
        Survival of the Fittest Filter:
        This block of code implements the survival of the fittest filter. It selects the peak with the lowest mean squared error (MSE) as the representative of the group. If the group contains only one peak, the peak is directly pushed to the valid regressions. If the group contains multiple peaks, the peak with the lowest MSE is selected as the representative of the group and pushed to the valid regressions.
      */
      for (auto &group : regressionGroups)
      {
        if (group.size() == 1)
        { // already isolated peak => push to valid regressions
          validRegressions.push_back(std::move(group.front()));
        }
        else
        { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
          calcExtendedMse(y_start, group, df_start);
          for (auto &regression : group)
          {
            if (regression->isValid)
            {
              validRegressions.push_back(std::move(regression));
            }
          }
        } // end if; single item or group with multiple members
      } // end for loop (group in vector of groups)
    } // end validateRegressions
#pragma endregion validateRegressions

#pragma region mergeRegressionsOverScales
    void
    qPeaks::mergeRegressionsOverScales(
        std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const float *y_start,
        const float *ylog_start,
        const bool *df_start)
    {
      if (validRegressions.empty())
      {
        return; // no valid peaks at all
      }

      if (validRegressions.size() == 1)
      {
        return; // only one valid regression, i.e., no further grouping required; validRegressions is already fine.
      }

      /*
        Grouping Over Scales:
        This block of code implements the grouping over scales. It groups the valid peaks based on the apex positions. Peaks are defined as similar, i.e., members of the same group, if they fullfill at least one of the following conditions:
        - The difference between two peak apexes is less than 4. (Nyquist Shannon Sampling Theorem, separation of two maxima)
        - At least one apex of a pair of peaks is within the window of the other peak. (Overlap of two maxima)
      */
      // iterate over the validRegressions vector
      for (auto it_new_peak = validRegressions.begin(); it_new_peak != validRegressions.end(); ++it_new_peak)
      {
        const double left_limit = (*it_new_peak)->left_limit;       // left limit of the current peak regression window in the Y matrix
        const double right_limit = (*it_new_peak)->right_limit;     // right limit of the current peak regression window in the Y matrix
        double grpMSE = 0;                                          // group mean squared error
        int grpDF = 0;                                              // group degree of freedom
        int numPeaksInGroup = 0;                                    // number of peaks in the group
        auto it_ref_peak = validRegressions.begin();                // iterator for the reference peak, i.e., a valid peak to compare with the new peak
        std::vector<decltype(it_ref_peak)> validRegressionsInGroup; // vector of iterators

        // iterate over the validRegressions vector till the new peak
        for (; it_ref_peak < it_new_peak; ++it_ref_peak)
        {
          if (!(*it_ref_peak)->isValid)
          {
            continue; // skip the invalid peaks
          }
          if ( // check for the overlap of the peaks
              (
                  (*it_ref_peak)->apex_position > left_limit &&   // ref peak matches the left limit
                  (*it_ref_peak)->apex_position < right_limit) || // ref peak matches the right limit
              (
                  (*it_new_peak)->apex_position > (*it_ref_peak)->left_limit && // new peak matches the left limit
                  (*it_new_peak)->apex_position < (*it_ref_peak)->right_limit)) // new peak matches the right limit
          {
            if ((*it_ref_peak)->mse == 0.0)
            { // calculate the mse of the ref peak
              (*it_ref_peak)->mse = calcSSE(
                                        ((*it_ref_peak)->left_limit) - (*it_ref_peak)->index_x0,  // left limit of the ref peak
                                        ((*it_ref_peak)->right_limit) - (*it_ref_peak)->index_x0, // right limit of the ref peak
                                        (*it_ref_peak)->coeff,                                    // regression coefficients
                                        y_start + (int)(*it_ref_peak)->left_limit,                // pointer to the start of the Y matrix
                                        true) /
                                    (*it_ref_peak)->df;
            }
            grpDF += (*it_ref_peak)->df;                        // add the degree of freedom
            grpMSE += (*it_ref_peak)->mse * (*it_ref_peak)->df; // add the sum of squared errors
            numPeaksInGroup++;                                  // increment the number of peaks in the group
            // add the iterator of the ref peak to a vector of iterators
            validRegressionsInGroup.push_back(it_ref_peak);
          }
        } // end for loop, inner loop, it_ref_peak

        if (grpDF > 0)
        {
          grpMSE /= grpDF;
        }
        else
        {
          continue; // no peaks in the group, i.e., the current peak stays valid
        }

        if ((*it_new_peak)->mse == 0.0)
        { // calculate the mse of the current peak
          (*it_new_peak)->mse = calcSSE(
                                    ((*it_new_peak)->left_limit) - (*it_new_peak)->index_x0,  // left limit of the new peak
                                    ((*it_new_peak)->right_limit) - (*it_new_peak)->index_x0, // right limit of the new peak
                                    (*it_new_peak)->coeff,                                    // regression coefficients
                                    y_start + (int)(*it_new_peak)->left_limit,                // pointer to the start of the Y matrix
                                    true) /
                                (*it_new_peak)->df;
        }

        if (numPeaksInGroup == 1)
        {
          // calculate the extended MSE using the current peak and the ref peak and set the worse one to invalid
          // create a temporary std::vector<std::unique_ptr<validRegression>> with the new peak and the ref peak
          std::vector<std::unique_ptr<validRegression>> tmpRegressions;
          tmpRegressions.push_back(std::move(*it_new_peak));
          tmpRegressions.push_back(std::move(validRegressionsInGroup[0][0]));
          calcExtendedMse(y_start, tmpRegressions, df_start);
          // Move the unique_ptrs back to validRegressionsInGroup
          validRegressionsInGroup[0][0] = std::move(tmpRegressions[1]);
          *it_new_peak = std::move(tmpRegressions[0]);
          continue;
        }
        if ((*it_new_peak)->mse < grpMSE)
        {
          // Set isValid to false for the candidates from the group
          for (auto it_ref_peak : validRegressionsInGroup)
          {
            (*it_ref_peak)->isValid = false;
          }
        }
        else
        { // Set isValid to false for the current peak
          (*it_new_peak)->isValid = false;
        }
      } // end for loop, outer loop, it_current_peak

      // Remove the peaks with isValid == false from the validRegressions
      validRegressions.erase(std::remove_if(validRegressions.begin(), validRegressions.end(),
                                            [](const auto &peak)
                                            { return !peak->isValid; }),
                             validRegressions.end());
    } // end mergeRegressionsOverScales
#pragma endregion mergeRegressionsOverScales

#pragma region createPeaks
    std::vector<std::unique_ptr<DataType::Peak>>
    qPeaks::createPeaks(
        const std::vector<std::unique_ptr<validRegression>> &validRegressions,
        const float *y_start,
        const float *x_start,
        const int scanNumber)
    {
      std::vector<std::unique_ptr<DataType::Peak>> peaks; // peak list
      // iterate over the validRegressions vector
      for (auto &regression : validRegressions)
      {
        // re-scale the apex position to x-axis
        const double x0 = *(x_start + (int)std::floor(regression->apex_position));
        const double dx = *(x_start + (int)std::ceil(regression->apex_position)) - x0;
        const double apex_position = x0 + dx * (regression->apex_position - std::floor(regression->apex_position));

        const __m128 coeff = regression->coeff;
        const float exp_b0 = exp_approx_d(((float *)&coeff)[0]); // exp(b0)
        // create a new peak object and push it to the peaks vector; the peak object is created using the scan number, the apex position and the peak height
        peaks.push_back(std::make_unique<DataType::Peak>(
            scanNumber,
            apex_position,
            exp_approx_d(((float *)&coeff)[0] + (regression->apex_position - regression->index_x0) * ((float *)&coeff)[1] * .5))); // peak height (exp(b0 - b1^2/4/b2)) with position being -b1/2/b2

        // add additional information to the peak object
        peaks.back()->area = regression->area * exp_b0;
        peaks.back()->areaUncertainty = regression->uncertainty_area * exp_b0;
        peaks.back()->heightUncertainty = regression->uncertainty_height;
        // peaks.back()->positionUncertainty = regression->uncertainty_position * dx;
        // peaks.back()->dqsPeak = regression->dqs;

        peaks.back()->beta0 = ((float *)&coeff)[0];
        peaks.back()->beta1 = ((float *)&coeff)[1];
        peaks.back()->beta2 = ((float *)&coeff)[2];
        peaks.back()->beta3 = ((float *)&coeff)[3];
        peaks.back()->x0 = *(x_start + regression->index_x0);
        peaks.back()->dx = dx;
      } // end for loop

      return peaks;
    } // end createPeaks
#pragma endregion createPeaks

#pragma region "createPeakList"
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>>
    qPeaks::createPeakList(std::vector<std::vector<std::unique_ptr<DataType::Peak>>> &allPeaks)
    {
      // create a temporary unordered map of peaks based on sampleID
      std::unordered_map<int, std::vector<std::unique_ptr<DataType::Peak>>> peakMap;
      // iterate over the peaks vector
      for (size_t i = 0; i < allPeaks.size(); ++i)
      {
        auto &peaks = allPeaks[i];
        // iterate over the peaks vector
        for (size_t j = 0; j < peaks.size(); ++j)
        {
          auto &peak = peaks[j];
          if (peakMap.find(peak->sampleID) == peakMap.end())
          {
            peakMap[peak->sampleID] = std::vector<std::unique_ptr<DataType::Peak>>();
          }
          peakMap[peak->sampleID].push_back(std::move(peak));
        }
      }

      // Store the keys in a separate vector
      std::vector<int> keys;
      for (const auto &pair : peakMap)
      {
        keys.push_back(pair.first);
      }

      // Now iterate over the keys to move the values
      std::vector<std::vector<std::unique_ptr<DataType::Peak>>> peakList(keys.size());
      int i = 0;
      for (const auto &key : keys)
      {
        // sort peaks based on the position
        std::sort(
            peakMap[key].begin(),
            peakMap[key].end(),
            [](const std::unique_ptr<DataType::Peak> &a, const std::unique_ptr<DataType::Peak> &b)
            { return a->position < b->position; });
        // add the vector of peaks to the peakList
        peakList[i] = std::move(peakMap[key]);
        i++;
      }

      // sort peakList based on the sampleID
      std::sort(
          peakList.begin(),
          peakList.end(),
          [](const std::vector<std::unique_ptr<DataType::Peak>> &a, const std::vector<std::unique_ptr<DataType::Peak>> &b)
          { return a.front()->sampleID < b.front()->sampleID; });

      return peakList;
    }
#pragma endregion "createPeakList"

#pragma region calcSSE
    float
    qPeaks::calcSSE(
        const int left_limit,
        const int right_limit,
        const __m128 &coeff,
        const float *y_start,
        const bool calc_EXP,
        const bool calc_CHISQ) const
    {
      // exception handling if the right limits is negative or the left limit is positive
      if (right_limit < 0 || left_limit > 0)
      {
        throw std::invalid_argument("right_limit must be positive and left_limit must be negative");
      }

      // exception if nullptr is passed
      if (y_start == nullptr)
      {
        throw std::invalid_argument("y_start must not be nullptr");
      }

      float result = 0.0f; // result variable

      const int n = right_limit - left_limit + 1;      // length of the result vector
      const int nFullSegments_left = -left_limit / 8;  // calculate the number of full segments of 8 elements per side
      const int nFullSegments_right = right_limit / 8; // calculate the number of full segments of 8 elements per side
      const int nRemaining_left = -left_limit % 8;     // calculate the number of remaining elements
      const int nRemaining_right = right_limit % 8;    // calculate the number of remaining elements

      // Load the coefficients
      const __m256 b0 = _mm256_set1_ps(((float *)&coeff)[0]);
      const __m256 b1 = _mm256_set1_ps(((float *)&coeff)[1]);
      const __m256 b2 = _mm256_set1_ps(((float *)&coeff)[2]);
      const __m256 b3 = _mm256_set1_ps(((float *)&coeff)[3]);

      // LEFT SIDE
      // Calculate the full segments
      int j = 0;
      float i = static_cast<float>(left_limit);
      for (int iSegment = 0; iSegment < nFullSegments_left; ++iSegment, i += 8.0f, j += 8)
      {
        // Load 8 values of i directly as float
        __m256 x_left = _mm256_add_ps(_mm256_set1_ps(i), LINSPACE_UP_POS_256); // x vector : -k to -k+7
        // Calculate the yhat values
        __m256 yhat_left = _mm256_fmadd_ps(_mm256_fmadd_ps(b2, x_left, b1), x_left, b0); // b0 + b1 * x + b2 * x^2
        if (calc_EXP)
        {
          yhat_left = exp_approx_vf(yhat_left); // calculate the exp of the yhat values (if needed)
        }
        __m256 y_vec = _mm256_load_ps(y_start + j);    // Load 8 values from y considering the offset j
        __m256 diff = _mm256_sub_ps(y_vec, yhat_left); // Calculate the difference between y and yhat
        __m256 diff_sq = _mm256_mul_ps(diff, diff);    // Calculate the square of the difference
        if (calc_CHISQ)
        {
          diff_sq = _mm256_div_ps(diff_sq, yhat_left); // Calculate the weighted square of the difference
        }
        result += q::sum8(diff_sq); // Calculate the sum of the squares and add it to the result
      }

      // Calculate the yhat values for the remaining elements
      if (nRemaining_left > 0)
      {
        __m256 x_left = _mm256_add_ps(_mm256_set1_ps(-static_cast<float>(nRemaining_left)), LINSPACE_UP_POS_256); // x vector : -nRemaining_left to -nRemaining_left+7
        __m256 yhat_left = _mm256_fmadd_ps(_mm256_fmadd_ps(b2, x_left, b1), x_left, b0);                          // b0 + b1 * x + b2 * x^2
        if (calc_EXP)
        {
          yhat_left = exp_approx_vf(yhat_left); // calculate the exp of the yhat values (if needed)
        }
        // Load the remaining values from y
        alignas(32) float y_left[8] = {0.0f};
        std::copy(y_start - left_limit - nRemaining_left, y_start - left_limit, y_left);
        __m256 y_vec = _mm256_load_ps(y_left);

        __m256i mask = _mm256_cmpgt_epi32(_mm256_set1_epi32(nRemaining_left), LINSPACE_UP_INT_256); // mask for the remaining elements
        yhat_left = _mm256_blendv_ps(y_vec, yhat_left, _mm256_castsi256_ps(mask));                  // set the remaining elements to zero

        __m256 diff = _mm256_sub_ps(y_vec, yhat_left); // calculate the difference between y and yhat
        __m256 diff_sq = _mm256_mul_ps(diff, diff);    // calculate the square of the difference
        if (calc_CHISQ)
        {
          diff_sq = _mm256_div_ps(diff_sq, yhat_left);                                         // calculate the weighted square of the difference
          diff_sq = _mm256_blendv_ps(_mm256_setzero_ps(), diff_sq, _mm256_castsi256_ps(mask)); // set the nan values to zero
        }
        result += q::sum8(diff_sq); // calculate the sum of the squares and add it to the result
      }

      // RIGHT SIDE
      // Calculate the yhat values for the full segments
      int k = n - 8;
      i = static_cast<float>(-right_limit);
      for (int iSegment = 0; iSegment < nFullSegments_right; ++iSegment, i += 8.0f, k -= 8)
      {
        // Load 8 values of i directly as float
        __m256 x_right = _mm256_add_ps(_mm256_set1_ps(-i), LINSPACE_DOWN_NEG_256); // x vector : k-7 to k
        // Calculate the yhat values
        __m256 yhat_right = _mm256_fmadd_ps(_mm256_fmadd_ps(b3, x_right, b1), x_right, b0); // b0 + b1 * x + b3 * x^2
        if (calc_EXP)
        {
          yhat_right = exp_approx_vf(yhat_right); // calculate the exp of the yhat values (if needed)
        }
        __m256 y_vec = _mm256_load_ps(y_start + k);     // Load 8 values from y considering the offset j
        __m256 diff = _mm256_sub_ps(y_vec, yhat_right); // Calculate the difference between y and yhat
        __m256 diff_sq = _mm256_mul_ps(diff, diff);     // Calculate the square of the difference
        if (calc_CHISQ)
        {
          diff_sq = _mm256_div_ps(diff_sq, yhat_right); // Calculate the weighted square of the difference
        }
        result += q::sum8(diff_sq); // Calculate the sum of the squares and add it to the result
      }

      // Calculate the yhat values for the remaining elements including center
      if (nRemaining_right > 0)
      {
        __m256 yhat_right = _mm256_fmadd_ps(_mm256_fmadd_ps(b3, LINSPACE_UP_POS_256, b1), LINSPACE_UP_POS_256, b0); // b0 + b1 * x + b3 * x^2
        if (calc_EXP)
        {
          yhat_right = exp_approx_vf(yhat_right); // calculate the exp of the yhat values (if needed)
        }
        // Load the remaining values from y
        alignas(32) float y_right[8] = {0.0f};
        std::copy(y_start - left_limit, y_start - left_limit + nRemaining_right + 1, y_right);

        __m256 y_vec = _mm256_load_ps(y_right);

        __m256i mask = _mm256_cmpgt_epi32(_mm256_set1_epi32(nRemaining_right + 1), LINSPACE_UP_INT_256); // mask for the remaining elements
        yhat_right = _mm256_blendv_ps(y_vec, yhat_right, _mm256_castsi256_ps(mask));                     // set the remaining elements to zero

        __m256 diff = _mm256_sub_ps(y_vec, yhat_right); // calculate the difference between y and yhat
        __m256 diff_sq = _mm256_mul_ps(diff, diff);     // calculate the square of the difference
        if (calc_CHISQ)
        {
          diff_sq = _mm256_div_ps(diff_sq, yhat_right);                                        // calculate the weighted square of the difference
          diff_sq = _mm256_blendv_ps(_mm256_setzero_ps(), diff_sq, _mm256_castsi256_ps(mask)); // set the nan values to zero
        }
        result += q::sum8(diff_sq); // calculate the sum of the squares and add it to the result
      }
      return result;
    } // end calcSSE
#pragma endregion calcSSE

#pragma region calcExtendedMse
    void
    qPeaks::calcExtendedMse(
        const float *y_start,                                             // start of the measured data
        const std::vector<std::unique_ptr<validRegression>> &regressions, // regressions to compare
        const bool *df_start)                                // degrees of freedom
    {
      /*
        The function consists of the following steps:
        1. Identify left and right limit of the grouped regression windows.
        2. Calculate the mean squared error (MSE) between the predicted and actual values.
        3. Identify the best regression based on the MSE and return the MSE and the index of the best regression.
      */
      // declare variables
      float best_mse = std::numeric_limits<float>::infinity();
      auto best_regression = regressions.begin();

      // step 1: identify left (smallest) and right (largest) limit of the grouped regression windows
      int left_limit = static_cast<int>(regressions.front()->left_limit);
      int right_limit = static_cast<int>(regressions.front()->right_limit);
      for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
      {
        left_limit = std::min(left_limit, static_cast<int>((*regression)->left_limit));
        right_limit = std::max(right_limit, static_cast<int>((*regression)->right_limit));
      }

      const int df_sum = calcDF(df_start, left_limit, right_limit);
      if (df_sum <= 4)
      {
        // set isValid to false for all regressions
        for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
        {
          (*regression)->isValid = false;
        }
        return; // not enough degrees of freedom
      }

      for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
      {
        // step 2: calculate the mean squared error (MSE) between the predicted and actual values
        const float mse = calcSSE(
                              left_limit - (*regression)->index_x0,  // left limit of the regression window (normalized scale)
                              right_limit - (*regression)->index_x0, // right limit of the regression window (normalized scale)
                              (*regression)->coeff,                  // regression coefficients
                              y_start + left_limit,                  // start of the measured data
                              true) /                                // calculate the exp of the yhat values
                          (df_sum - 4);

        // step 3: identify the best regression based on the MSE and return the MSE and the index of the best regression
        if (mse < best_mse)
        {
          best_mse = mse;
          (*regression)->mse = mse;
          best_regression = regression;
        }
      } // end for loop (index in groupIndices)
      // set isValid to false for all regressions except the best one
      for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
      {
        if (regression != best_regression)
        {
          (*regression)->isValid = false;
        }
      }
    } // end qPeaks::calcExtendedMse
#pragma endregion calcExtendedMse

#pragma region calcDF
    int qPeaks::calcDF(
        const bool *df_start,     // start of the degrees of freedom
        const size_t left_limit,  // left limit
        const size_t right_limit) // right limit
    {
      return std::accumulate(
          df_start + left_limit,
          df_start + right_limit + 1,
          0,
          [](int sum, bool p)
          { return sum + p; });
    } // end calcDF
#pragma endregion calcDF

#pragma region calculateApexAndValleyPositions
    bool
    qPeaks::calculateApexAndValleyPositions(
        const __m128 &coeff,
        const int scale,
        double &apex_position,
        double &valley_position) const
    {
      // calculate key by checking the signs of coeff
      __m128 res = _mm_set1_ps(-.5f);               // res = -0.5
      __m128 signs = _mm_cmplt_ps(coeff, ZERO_128); // compare the coefficients with zero, results will have the following values: 0xFFFFFFFF if the value is negative, 0x00000000 if the value is positive
      signs = _mm_and_ps(signs, KEY_128);           // multiply a key value if the value of the coefficient is negative, i.e., b0 * 0, b1 * 4, b2 * 2, b3 * 1
      signs = _mm_hadd_ps(signs, signs);            // horizontal add of the signs
      signs = _mm_hadd_ps(signs, signs);            // horizontal add of the signs, now all values are the same, i.e. the sum
      int key = _mm_cvtss_si32(signs);

      switch (key)
      {
      case 7:                                                            // Case 1a: apex left
        res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101)); // res = -0.5 * b1
        res = _mm_div_ps(res, coeff);                                    // res = -0.5 * b1 / b2
        apex_position = ((float *)&res)[2];                              //-B1 / 2 / B2;  // is negative
        valley_position = 0;                                             // no valley point
        return apex_position > -scale + 1;                               // scale +1: prevent apex position to be at the edge of the data

      case 3:                                                            // Case 1b: apex right
        res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101)); // res = -0.5 * b1
        res = _mm_div_ps(res, coeff);                                    // res = -0.5 * b1 / b2
        apex_position = ((float *)&res)[3];                              //-B1 / 2 / B3;     // is positive
        valley_position = 0;                                             // no valley point
        return apex_position < scale - 1;                                // scale -1: prevent apex position to be at the edge of the data

      case 6:                                                                     // Case 2a: apex left | valley right
        res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101));          // res = -0.5 * b1
        res = _mm_div_ps(res, coeff);                                             // res = -0.5 * b1 / b2
        apex_position = ((float *)&res)[2];                                       //-B1 / 2 / B2;                                             // is negative
        valley_position = ((float *)&res)[3];                                     //-B1 / 2 / B3;                                           // is positive
        return apex_position > -scale + 1 && valley_position - apex_position > 2; // scale +1: prevent apex position to be at the edge of the data

      case 1:                                                                    // Case 2b: apex right | valley left
        res = _mm_mul_ps(res, _mm_shuffle_ps(coeff, coeff, 0b01010101));         // res = -0.5 * b1
        res = _mm_div_ps(res, coeff);                                            // res = -0.5 * b1 / b2
        apex_position = ((float *)&res)[3];                                      //-B1 / 2 / B3;                                            // is positive
        valley_position = ((float *)&res)[2];                                    //-B1 / 2 / B2;                                          // is negative
        return apex_position < scale - 1 && apex_position - valley_position > 2; // scale -1: prevent apex position to be at the edge of the data

      default:
        return false; // invalid case
      } // end switch
    }
#pragma endregion calculateApexAndValleyPositions

#pragma region "multiplyVecMatrixVecTranspose"
    float
    qPeaks::multiplyVecMatrixVecTranspose(
        const float vec[4],
        const int scale) const
    {
      // Prefetch the inverse matrix to improve cache performance
      _mm_prefetch(reinterpret_cast<const char *>(&invArray[scale][0]), _MM_HINT_T0);

      // Load the vector values
      __m128 vec_values = _mm_loadu_ps(vec);

      // Calculate the products of the vector values
      __m128 v0_v0 = _mm_mul_ps(vec_values, vec_values);                              // v0_v0 = vec[0] * vec[0]
      __m128 temp1 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(1, 0, 0, 0)); // temp1 = vec[1], vec[0], vec[0], vec[0]
      __m128 temp2 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(2, 2, 1, 1)); // temp2 = vec[2], vec[2], vec[1], vec[1]
      __m128 v0_v1 = _mm_mul_ps(temp1, temp2);                                        // v0_v1 = vec[0] * vec[1], vec[1] * vec[2], etc.

      __m128 temp3 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(3, 3, 3, 3)); // temp3 = vec[3], vec[3], vec[3], vec[3]
      __m128 v0_v2 = _mm_mul_ps(vec_values, temp3);                                   // v0_v2 = vec[0] * vec[2], vec[1] * vec[3], etc.

      __m128 temp4 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(1, 1, 1, 1)); // temp4 = vec[1], vec[1], vec[1], vec[1]
      __m128 v1_v1 = _mm_mul_ps(temp4, temp4);                                        // v1_v1 = vec[1] * vec[1]

      __m128 temp5 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(2, 2, 2, 2)); // temp5 = vec[2], vec[2], vec[2], vec[2]
      __m128 v1_v2 = _mm_mul_ps(temp5, temp4);                                        // v1_v2 = vec[2] * vec[1]

      __m128 temp6 = _mm_shuffle_ps(vec_values, vec_values, _MM_SHUFFLE(3, 3, 3, 3)); // temp6 = vec[3], vec[3], vec[3], vec[3]
      __m128 v1_v3 = _mm_mul_ps(temp6, temp4);                                        // v1_v3 = vec[3] * vec[1]

      __m128 v2_v2 = _mm_mul_ps(temp5, temp5); // v2_v2 = vec[2] * vec[2]
      __m128 v2_v3 = _mm_mul_ps(temp5, temp6); // v2_v3 = vec[2] * vec[3]
      __m128 v3_v3 = _mm_mul_ps(temp6, temp6); // v3_v3 = vec[3] * vec[3]

      // Calculate the result
      float result = _mm_cvtss_f32(v0_v0) * invArray[scale][0] +
                     _mm_cvtss_f32(v1_v1) * invArray[scale][2] +
                     invArray[scale][4] * (_mm_cvtss_f32(v2_v2) + _mm_cvtss_f32(v3_v3)) +
                     2 * (invArray[scale][1] * (_mm_cvtss_f32(v0_v1) + _mm_cvtss_f32(v0_v2)) +
                          invArray[scale][3] * (_mm_cvtss_f32(v1_v2) - _mm_cvtss_f32(v1_v3)) +
                          _mm_cvtss_f32(v2_v3) * invArray[scale][5]);

      return result;
    }
#pragma endregion "multiplyVecMatrixVecTranspose"

#pragma region "isValidApexToEdge"
    bool
    qPeaks::isValidApexToEdge(
        const double apex_position,
        const int scale,
        const int index_loop,
        const float *y_start,
        float &apexToEdge) const
    {
      int idx_apex = (int)std::round(apex_position) + scale + index_loop; // index of the apex
      int idx_left = index_loop;                                          // index of the left edge
      int idx_right = 2 * scale + index_loop;                             // index of the right edge
      float apex = *(y_start + idx_apex);                                 // apex value
      float left = *(y_start + idx_left);                                 // left edge value
      float right = *(y_start + idx_right);                               // right edge value
      apexToEdge = (left < right) ? (apex / left) : (apex / right);       // difference between the apex and the edge
      return apexToEdge > 2;                                              // return true if the ratio is greater than 2
    }

#pragma endregion "isValidApexToEdge"

#pragma region isValidQuadraticTerm
    bool qPeaks::isValidQuadraticTerm(
        const __m128 &coeff,
        const double inverseMatrix_2_2,
        const double inverseMatrix_3_3,
        const float mse,
        const int df_sum) const
    {
      double tValue = std::max(                                                 // t-value for the quadratic term
          std::abs(((float *)&coeff)[2]) / std::sqrt(inverseMatrix_2_2 * mse),  // t-value for the quadratic term left side of the peak
          std::abs(((float *)&coeff)[3]) / std::sqrt(inverseMatrix_3_3 * mse)); // t-value for the quadratic term right side of the peak
      return tValue > tValuesArray[df_sum - 5];                                 // statistical significance of the quadratic term
    }
#pragma endregion isValidQuadraticTerm

#pragma region isValidPeakHeight
    bool
    qPeaks::isValidPeakHeight(
        const float mse,
        const size_t index,
        const int scale,
        const double apex_position,
        const double valley_position,
        const int df_sum,
        const float apexToEdge,
        double &uncertainty_height) const
    {
      float Jacobian_height[4];           // Jacobian matrix for the height
      Jacobian_height[0] = 1.f;           // height;
      Jacobian_height[1] = apex_position; // apex_position * height;
      if (apex_position < 0)
      {
        Jacobian_height[2] = apex_position * apex_position; // apex_position * Jacobian_height[1];
        Jacobian_height[3] = 0;
      }
      else
      {
        Jacobian_height[2] = 0;
        Jacobian_height[3] = apex_position * apex_position; // apex_position * Jacobian_height[1];
      }
      uncertainty_height = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale)); // at this point without height, i.e., to get the real uncertainty multiply with height later. This is done to avoid exp function at this point

      if (1 / uncertainty_height <= tValuesArray[df_sum - 5]) // statistical significance of the peak height
      {
        return false;
      }

      // check if the peak height is significantly greater than edge signal

      float edge_position = (apex_position < 0) // position of the egde not on the same side like the peak apex
                                ?               // apex on the left
                                (valley_position != 0) ? valley_position : static_cast<float>(-scale)
                                : // apex on the right
                                (valley_position != 0) ? valley_position : static_cast<float>(scale);

      Jacobian_height[0] = 0.f;            // adjust for uncertainty calculation of apex to edge ratio
      Jacobian_height[1] -= edge_position; // adjust for uncertainty calculation of apex to edge ratio
      if (apex_position < 0)
      {
        Jacobian_height[2] -= edge_position * edge_position; // adjust for uncertainty calculation of apex to edge ratio
      }
      else
      {
        Jacobian_height[3] -= edge_position * edge_position; // adjust for uncertainty calculation of apex to edge ratio
      }
      float uncertainty_apexToEdge = std::sqrt(mse * multiplyVecMatrixVecTranspose(Jacobian_height, scale));
      return (apexToEdge - 2) / (apexToEdge * uncertainty_apexToEdge) > tValuesArray[df_sum - 5];
    }
#pragma endregion isValidPeakHeight

#pragma region isValidPeakArea
    bool
    qPeaks::isValidPeakArea(
        const __m128 &coeff,
        const float mse,
        const int scale,
        const int df_sum,
        double &area,
        double &uncertainty_area) const
    {
      // predefine expressions
      const float b1 = ((float *)&coeff)[1];
      const float b2 = ((float *)&coeff)[2];
      const float b3 = ((float *)&coeff)[3];
      const float _SQRTB2 = 1 / std::sqrt(std::abs(-b2));
      const float _SQRTB3 = 1 / std::sqrt(std::abs(-b3));
      const float B1_2_SQRTB2 = b1 / 2 * _SQRTB2;
      const float B1_2_SQRTB3 = b1 / 2 * _SQRTB3;
      const float B1_2_B2 = b1 / 2 / b2;
      const float EXP_B12 = exp_approx_d(-b1 * B1_2_B2 / 2);
      const float B1_2_B3 = b1 / 2 / b3;
      const float EXP_B13 = exp_approx_d(-b1 * B1_2_B3 / 2);

      // initialize variables
      float J[4]; // Jacobian matrix

      // here we have to check if there is a valley point or not
      const float err_L =
          (b2 < 0)
              ? experfc(B1_2_SQRTB2, -1.0) // 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
              : dawson5(B1_2_SQRTB2);      // erfi(b1 / 2 / SQRTB2);        // peak with valley point;

      const float err_R =
          (b3 < 0)
              ? experfc(B1_2_SQRTB3, 1.0) // 1 + std::erf(b1 / 2 / SQRTB3) // ordinary peak
              : dawson5(-B1_2_SQRTB3);    // -erfi(b1 / 2 / SQRTB3);       // peak with valley point ;

      // calculate the Jacobian matrix terms
      const float J_1_common_L = _SQRTB2; // SQRTPI_2 * EXP_B12 / SQRTB2;
      const float J_1_common_R = _SQRTB3; // SQRTPI_2 * EXP_B13 / SQRTB3;
      const float J_2_common_L = B1_2_B2 / b1;
      const float J_2_common_R = B1_2_B3 / b1;
      const float J_1_L = J_1_common_L * err_L;
      const float J_1_R = J_1_common_R * err_R;
      const float J_2_L = J_2_common_L - J_1_L * B1_2_B2;
      const float J_2_R = -J_2_common_R - J_1_R * B1_2_B3;

      J[0] = J_1_R + J_1_L;
      J[1] = J_2_R + J_2_L;
      J[2] = -B1_2_B2 * (J_2_L + J_1_L / b1);
      J[3] = -B1_2_B3 * (J_2_R + J_1_R / b1);

      area = J[0]; // at this point the area is without exp(b0), i.e., to get the real area multiply with exp(b0) later. This is done to avoid exp function at this point
      uncertainty_area = std::sqrt(mse * multiplyVecMatrixVecTranspose(J, scale));

      if (area / uncertainty_area <= tValuesArray[df_sum - 5])
      {
        return false;
      }

      float J_covered[4];    // Jacobian matrix for the covered peak area
      float x_left = -scale; // left limit due to the window
      float x_right = scale; // right limit due to the window
      float y_left = 0;      // y value at the left limit
      float y_right = 0;     // y value at the right limit

      const float err_L_covered = ///@todo : need to be revised
          (b2 < 0)
              ?                                                                                                   // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
              erf_approx_f((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12 * SQRTPI_2 + err_L - SQRTPI_2 * EXP_B12 // std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
              :                                                                                                   // valley point, i.e., check position
              (-B1_2_B2 < -scale)
                  ? // valley point is outside the window, use scale as limit
                  err_L - erfi((b1 - 2 * b2 * scale) / 2 * _SQRTB2) * EXP_B12
                  : // valley point is inside the window, use valley point as limit
                  err_L;

      const float err_R_covered = ///@todo : need to be revised
          (b3 < 0)
              ?                                                                                                   // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
              err_R - SQRTPI_2 * EXP_B13 - erf_approx_f((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * SQRTPI_2 * EXP_B13 // err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
              :                                                                                                   // valley point, i.e., check position
              (-B1_2_B3 > scale)
                  ? // valley point is outside the window, use scale as limit
                  erfi((b1 + 2 * b3 * scale) / 2 * _SQRTB3) * EXP_B13 + err_R
                  : // valley point is inside the window, use valley point as limit
                  err_R;

      // adjust x limits
      if (b2 > 0 && -B1_2_B2 > -scale)
      { // valley point is inside the window, use valley point as limit
        x_left = -B1_2_B2;
      }

      if (b3 > 0 && -B1_2_B3 < scale)
      { // valley point is inside the window, use valley point as limit
        x_right = -B1_2_B3;
      }

      // calculate the y values at the left and right limits
      y_left = exp_approx_d(b1 * x_left + b2 * x_left * x_left);
      y_right = exp_approx_d(b1 * x_right + b3 * x_right * x_right);
      const float dX = x_right - x_left;

      // calculate the trapzoid correction terms for the jacobian matrix
      const float trpzd_b0 = (y_right + y_left) * dX / 2;
      const float trpzd_b1 = (x_right * y_right + x_left * y_left) * dX / 2;
      const float trpzd_b2 = (x_left * x_left * y_left) * dX / 2;
      const float trpzd_b3 = (x_right * x_right * y_right) * dX / 2;

      const float J_1_L_covered = J_1_common_L * err_L_covered;
      const float J_1_R_covered = J_1_common_R * err_R_covered;
      const float J_2_L_covered = J_2_common_L - J_1_L_covered * B1_2_B2;
      const float J_2_R_covered = -J_2_common_R - J_1_R_covered * B1_2_B3;

      J_covered[0] = J_1_R_covered + J_1_L_covered - trpzd_b0;
      J_covered[1] = J_2_R_covered + J_2_L_covered - trpzd_b1;
      J_covered[2] = -B1_2_B2 * (J_2_L_covered + J_1_L_covered / b1) - trpzd_b2;
      J_covered[3] = -B1_2_B3 * (J_2_R_covered + J_1_R_covered / b1) - trpzd_b3;

      const float area_uncertainty_covered = std::sqrt(mse * multiplyVecMatrixVecTranspose(J_covered, scale));

      return J_covered[0] / area_uncertainty_covered > tValuesArray[df_sum - 5]; // statistical significance of the peak area
    }
#pragma endregion isValidPeakArea

#pragma region createDesignMatrix
    void qPeaks::createDesignMatrix(const int scale)
    {
      // construct matrix
      int m = 4;
      int n = 2 * scale + 1;
      q::Matrices::Matrix X(n, m);

      for (int i = 0; i < n; i++)
      {
        double x = static_cast<double>(i - scale);
        X(i, 0) = 1.0; // first column is always 1
        X(i, 1) = x;   // x vector (centered) from -scale to scale
        if (x < 0)
        {
          X(i, 2) = x * x; // x^2 vector
        }
        else
        {
          X(i, 3) = x * x; // x^2 vector
        }
      }
      // add the matrix to the designMatrices vector
      designMatrices.push_back(std::make_unique<q::Matrices::Matrix>(X));
    }
#pragma endregion createDesignMatrix

#pragma region createInverseAndPseudoInverse
    void qPeaks::createInverseAndPseudoInverse(const q::Matrices::Matrix &X)
    {
      q::Matrices::Matrix Xt = transpose(X);
      q::Matrices::Matrix XtX = Xt * X;
      inverseMatrices.push_back(std::make_unique<q::Matrices::Matrix>(inv(XtX)));
      psuedoInverses.push_back(std::make_unique<q::Matrices::Matrix>(*(inverseMatrices.back()) * Xt));
    }

    int qPeaks::calculateNumberOfRegressions(const int n) const
    {
      /*
      int maxScale = (int) (n - 1) / 2;
      int sumScales = (int) (maxScale * (maxScale + 1) / 2) - 1;
      return n * (maxScale-1) - sumScales*2;
      */
      int sum = 0;
      for (int i = 4; i <= this->global_maxScale * 2; i += 2)
      {
        sum += std::max(0, n - i);
      }
      return sum;
    }
#pragma endregion createInverseAndPseudoInverse

#pragma region info
    void qPeaks::info() const
    {
      std::cout << "qPeaks object\n";
      std::cout << "designMatrices size: " << designMatrices.size() << "\n";
      std::cout << "inverseMatrices size: " << inverseMatrices.size() << "\n";
      std::cout << "psuedoInverses size: " << psuedoInverses.size() << "\n";
    }
#pragma endregion info

#pragma region "convolve regression"

    void
    qPeaks::convolve_static(
        const size_t scale,
        const float (&vec)[512],
        const size_t n,
        __m128 *beta)
    {
      if (n < 2 * scale + 1)
      {
        throw std::invalid_argument("n must be greater or equal to 2 * scale + 1");
      }

      alignas(16) __m128 result[512];
      alignas(16) __m128 products[512];
      const __m128 flipSign = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
      convolve_SIMD(scale, vec, n, result, products, 512);

      for (size_t i = 0; i < n - 2 * scale; i++)
      {
        beta[i] = _mm_mul_ps(_mm_shuffle_ps(result[i], result[i], 0b10110100), flipSign); // swap beta2 and beta3 and flip the sign of beta1 // @todo: this is a temporary solution
      }
    }

    void
    qPeaks::convolve_dynamic(
        const size_t scale,
        const float *vec,
        const size_t n,
        __m128 *beta)
    {
      if (n < 2 * scale + 1)
      {
        throw std::invalid_argument("n must be greater or equal to 2 * scale + 1");
      }

      alignas(16) __m128 *result = new __m128[n - 2 * scale];
      alignas(16) __m128 *products = new __m128[n];
      convolve_SIMD(scale, vec, n, result, products, n);

      for (size_t i = 0; i < n - 2 * scale; i++)
      {
        beta[i] = result[i];
      }
    }

    void
    qPeaks::convolve_SIMD(
        const size_t scale,
        const float *vec,
        const size_t n,
        __m128 *result,
        __m128 *products,
        const size_t buffer_size)
    {
      size_t k = 2 * scale + 1;
      size_t n_segments = n - k + 1;
      size_t centerpoint = k / 2;

      for (size_t i = 0; i < n_segments; ++i)
      {
        result[i] = _mm_setzero_ps();
      }

      for (size_t i = 0; i < n; ++i)
      {
        products[i] = _mm_setzero_ps();
      }

      alignas(16) __m128 kernel[3];
      kernel[0] = _mm_set_ps(invArray[scale][1], invArray[scale][1], 0.0f, invArray[scale][0]);
      kernel[1] = _mm_set_ps(invArray[scale][3] - invArray[scale][5], -invArray[scale][3] - invArray[scale][4], -invArray[scale][2] - invArray[scale][3], -invArray[scale][1]);
      kernel[2] = _mm_set_ps(2.f * invArray[scale][5], 2.f * invArray[scale][4], 2.f * invArray[scale][3], 2.f * invArray[scale][1]);

#pragma GCC ivdep
#pragma GCC unroll 8
      for (size_t i = 0; i < n_segments; i++)
      {
        __m128 vec_values = _mm_set1_ps(vec[i + centerpoint]);
        result[i] = _mm_fmadd_ps(vec_values, kernel[0], result[i]);
      }

      for (size_t i = 1; i < scale + 1; i++)
      {
        int u = 0;
        kernel[1] = _mm_add_ps(kernel[1], kernel[2]);
        kernel[0] = _mm_add_ps(kernel[0], kernel[1]);

#pragma GCC ivdep
#pragma GCC unroll 8
        for (size_t j = scale - i; j < (n - scale + i); j++)
        {
          __m128 vec_values = _mm_set1_ps(vec[j]);
          products[u] = _mm_mul_ps(vec_values, kernel[0]);
          u++;
        }

#pragma GCC ivdep
#pragma GCC unroll 8
        for (size_t j = 0; j < n_segments; j++)
        {
          if (2 * i + j >= buffer_size)
          {
            throw std::out_of_range("Index out of range for products array: n=" + std::to_string(n) + " i=" + std::to_string(i) + " j=" + std::to_string(j));
          }
          __m128 products_temp = _mm_permute_ps(products[j], 0b10110100);
          __m128 sign_flip = _mm_set_ps(1.0f, 1.0f, -1.0f, 1.0f);
          products_temp = _mm_fmadd_ps(products_temp, sign_flip, products[2 * i + j]);
          result[j] = _mm_add_ps(result[j], products_temp);
        }
      }
    }
#pragma endregion "convolve regression"

#pragma region printMatrices
    void qPeaks::printMatrices(int scale) const
    {
      std::cout << "Design Matrix\n";
      print(*(designMatrices[scale]));
      std::cout << "\nInverse Matrix\n";
      print(*(inverseMatrices[scale]));
      std::cout << "\nPseudo-Inverse Matrix\n";
      print(*(psuedoInverses[scale]));
    }
#pragma endregion printMatrices
  } // namespace Algorithms
} // namespace q
