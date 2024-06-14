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
          /*
          // get the largest number of data points in a single data set within the dataVec
          int maxDataPoints = 0;
          // iterate over the map of varDataType datatype objects
            for (auto &pair : *arg)
            {
              // de-reference the unique pointer of the object
              auto &dataObj = *(pair.get());
              // iterate over data point vector, which is a vector of unique pointers to data points structures
              auto &data = dataObj.dataPoints;
              // compare the size of the data vector to the maxDataPoints variable
              if (data.size() > maxDataPoints)
              {
                // if the size of the data vector is larger than the maxDataPoints variable, set the maxDataPoints variable to the size of the data vector
                maxDataPoints = data.size();
              }
            }
          int maxScale = (int) (maxDataPoints-1)/2;
          */

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
  double q::Algorithms::qPeaks::x_square[100];   // array to store the square of the x values
  double q::Algorithms::qPeaks::invArray[50][6]; // array to store the 6 unique values of the inverse matrix for each scale
  void qPeaks::initialize()
  {
    // init x_square
    for (int i = 0; i < 100; ++i)
    {
      x_square[i] = i * i;
    }
    // init invArray
    double XtX_00 = 1;
    double XtX_02 = 0;
    double XtX_11 = 0;
    double XtX_12 = 0;
    double XtX_13 = 0;
    double XtX_22 = 0;
    for (int i = 1; i < 50; ++i)
    {
      XtX_00 += 2;
      XtX_02 += x_square[i];
      XtX_11 = XtX_02 * 2;
      XtX_13 += x_square[i] * i;
      XtX_12 = -XtX_13;
      XtX_22 += x_square[i] * x_square[i];

      double L_00 = std::sqrt(XtX_00);
      double L_11 = std::sqrt(XtX_11);
      double L_20 = XtX_02 / L_00;
      double L_21 = XtX_12 / L_11;
      double L_20sq = L_20 * L_20;
      double L_21sq = L_21 * L_21;
      double L_22 = std::sqrt(XtX_22 - L_20sq - L_21sq);
      double L_32 = 1 / L_22 * (-L_20sq + L_21sq);
      double L_33 = std::sqrt(XtX_22 - L_20sq - L_21sq - L_32 * L_32);

      double inv_00 = 1 / L_00;
      double inv_11 = 1 / L_11;
      double inv_22 = 1 / L_22;
      double inv_33 = 1 / L_33;
      double inv_20 = -L_20 * inv_00 / L_22;
      double inv_30 = -(L_20 * inv_00 + L_32 * inv_20) / L_33;
      double inv_21 = -L_21 * inv_11 / L_22;
      double inv_31 = -(-L_21 * inv_11 + L_32 * inv_21) / L_33;
      double inv_32 = -L_32 * inv_22 / L_33;

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
            size_t n = data.size();
            if (n < 5)
            {
              continue; // not enough data points to fit a quadratic regression model
            }

            // store x and y values in RefMatrix objects
            q::Matrices::Vector Y(n);
            q::Matrices::Vector X(n);
            q::Matrices::BoolVector df(n);
            for (size_t i = 0; i < n; i++)
            {
              X[i] = data[i]->x();
              df[i] = data[i]->df;
              Y[i] = data[i]->y();
            }
            std::vector<std::unique_ptr<validRegression>> validRegressions;
            runningRegression(Y, df, validRegressions);
            if (validRegressions.empty())
            {
              continue; // no valid peaks
            }
            all_peaks[i] = createPeaks(validRegressions, Y, X, dataObj.getScanNumber());
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
      const q::Matrices::Vector &Y,
      const q::Matrices::BoolVector &df,
      std::vector<std::unique_ptr<validRegression>> &validRegressions)
  {
    size_t n = Y.n;
    q::Matrices::Vector Ylog = logn(Y); // perform log-transform on Y
    int maxScale = std::min(this->global_maxScale, (int)(n - 1) / 2);
    validRegressions.reserve(calculateNumberOfRegressions(n));
    double ylog_array[500];
    if (n <= 500)
    {
      for (size_t i = 0; i < Y.n; i++)
      {
        ylog_array[i] = Ylog[i];
      }
    }
    for (int scale = 2; scale <= maxScale; scale++)
    {
      q::Matrices::Matrix_mc B = (n <= 500) ? convolve_fast(scale, ylog_array, n) : convolve(Ylog, n, *(psuedoInverses[scale]));
      validateRegressions(B, Y, Ylog, df, scale, validRegressions);
    } // end for scale loop
    mergeRegressionsOverScales(validRegressions, Ylog, Y, df);
  } // end runningRegression
#pragma endregion runningRegression

#pragma region validateRegressions
  void qPeaks::validateRegressions(
      const q::Matrices::Matrix_mc &B,
      const q::Matrices::Vector &Y,
      const q::Matrices::Vector &Ylog,
      const q::Matrices::BoolVector &df,
      const int scale,
      std::vector<std::unique_ptr<validRegression>> &validRegressions)
  {
    // declare constants
    // const double inverseMatrix_2_2 = (*(inverseMatrices[scale]))(2, 2); // variance of the quadratic term left side of the peak
    // const double inverseMatrix_3_3 = (*(inverseMatrices[scale]))(3, 3); // variance of the quadratic term right side of the peak
    const double inverseMatrix_2_2 = invArray[scale][4]; // variance of the quadratic term left side of the peak
    // const Matrix &X = *(designMatrices[scale]);                         // design matrix

    // declare variables
    std::vector<std::unique_ptr<validRegression>> validRegressionsTmp; // temporary vector to store valid regressions <index, apex_position>

    // iterate columwise over the coefficients matrix
    for (size_t i = 0; i < B.cols; i++)
    {
      /*
        Degree of Freedom Filter:
        This block of code implements the degree of freedom filter. It calculates the degree of freedom based df vector. If the degree of freedom is less than 5, the loop continues to the next iteration. The value 5 is chosen as the minimum number of data points required to fit a quadratic regression model.
      */
      int df_sum = calcDF(df, i, 2 * scale + i); // calculate the sum of the degree of freedom (df_sum)
      if (df_sum < 5)
      {
        continue; // degree of freedom less than 5; i.e., less then 5 measured data points
      }

      /*
        Apex and Valley Position Filter:
        This block of code implements the apex and valley position filter. It calculates the apex and valley positions based on the coefficients matrix B. If the apex is outside the data range, the loop continues to the next iteration. If the apex and valley positions are too close to each other, the loop continues to the next iteration.
      */
      double apex_position, valley_position;
      if (!calculateApexAndValleyPositions(B, i, scale, apex_position, valley_position))
      {
        continue; // invalid apex and valley positions
      }

      /*
        Quadratic Term Filter:
        This block of code implements the quadratic term filter. It calculates the mean squared error (MSE) between the predicted and actual values. Then it calculates the t-value for the quadratic term. If the t-value is less than the corresponding value in the tValuesArray, the quadratic term is considered statistically insignificant, and the loop continues to the next iteration.
      */
      const q::Matrices::Vector b = extractCol(B, i);                                 // coefficients vector
      const q::Matrices::Vector yhat = calcYhat(-scale, scale, B, i);                 // predicted values
      const double mse = calcSSE(yhat, Ylog, Ylog.begin() + i) / df_sum; // mean squared error
      if (!isValidQuadraticTerm(B, i, inverseMatrix_2_2, inverseMatrix_2_2, mse, df_sum))
      {
        continue; // statistical insignificance of the quadratic term
      }

      /*
        Height Filter:
        This block of code implements the height filter. It calculates the height of the peak based on the coefficients matrix B. Then it calculates the uncertainty of the height based on the Jacobian matrix and the variance-covariance matrix of the coefficients. If the height is statistically insignificant, the loop continues to the next iteration.
      */
      // Matrix C = (*inverseMatrices[scale]) * mse; // variance-covariance matrix of the coefficients
      q::Matrices::Matrix_mc_4x4 C = multiplyScalarTo4x4Matrix(mse, *inverseMatrices[scale]); // variance-covariance matrix of the coefficients
      double height = 0.0;
      double uncertainty_height = 0.0;
      double uncertainty_position = 0.0;
      if (!isValidPeakHeight(B, C, i, scale, apex_position, df_sum, height, uncertainty_height, uncertainty_position))
      {
        continue; // statistical insignificance of the height
      }

      /*
        Area Filter:
        This block of code implements the area filter. It calculates the Jacobian matrix for the peak area based on the coefficients matrix B. Then it calculates the uncertainty of the peak area based on the Jacobian matrix. If the peak area is statistically insignificant, the loop continues to the next iteration.
      */
      double area = 0.0;
      double uncertainty_area = 0.0;
      if (!isValidPeakArea(B, C, i, scale, df_sum, area, uncertainty_area))
      {
        continue; // statistical insignificance of the area
      }

      /*
        Chi-Square Filter:
        This block of code implements the chi-square filter. It calculates the chi-square value based on the weighted chi squared sum of expected and measured y values in the exponential domain. If the chi-square value is less than the corresponding value in the chiSquareArray, the loop continues to the next iteration.
      */
      double chiSquare = calcChiSquareEXP(yhat, Y, Y.begin() + i);
      if (chiSquare < chiSquareArray[df_sum - 5])
      {
        continue; // statistical insignificance of the chi-square value
      }

      /*
        Add to a temporary vector of valid regressions:
        This block of code adds the valid peak to a temporary vector of valid regressions. It calculates the left and right limits of the peak based on the valley position. Then it stores the index of the valid regression in the temporary vector of valid regressions.
      */
      // at this point, the peak is validated
      double left_limit = (valley_position < 0) ? std::max((double)i, valley_position + i + scale) : i;
      double right_limit = (valley_position > 0) ? std::min((double)i + 2 * scale, valley_position + i + scale) : i + 2 * scale;

      df_sum = calcDF(df, left_limit, right_limit); // update the degree of freedom considering the left and right limits
      if (df_sum < 5)
      {
        continue; // degree of freedom less than 5; i.e., less then 5 measured data points
      }
      double dqs = 1 - std::erf(uncertainty_area / area);
      validRegressionsTmp.push_back(
          std::make_unique<validRegression>(
              i + scale,                 // index of the center of the window (x==0) in the Y matrix
              scale,                     // scale
              df_sum - 4,                // df
              apex_position + i + scale, // apex position in x-axis 0:n
              0,                         // initial MSE
              b,                         // coefficients matrix
              true,                      // isValid
              left_limit,                // left_limit
              right_limit,               // right_limit
              left_limit - i,            // X_row_0
              right_limit - i + 1,       // X_row_1
              dqs,                       // dqs
              height,                    // peak height
              area,                      // peak area
              uncertainty_height,        // uncertainty of the peak height
              uncertainty_area,          // uncertainty of the peak area
              uncertainty_position       // uncertainty of the peak position
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
      {                                                                                                    // the two regressions differ, i.e. create a new group and move all regressions from start to it_peak to this group
        regressionGroups.push_back(std::vector<std::unique_ptr<q::Algorithms::qPeaks::validRegression>>());            // create a new group
        std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back())); // move all regressions from start to it_peak to this group
        it_peak_next = validRegressionsTmp.erase(validRegressionsTmp.begin(), it_peak_next);               // erase the moved regressions from the temporary vector of valid regressions
      }
      it_peak = it_peak_next;
      ++it_peak_next;
      if (it_peak_next == validRegressionsTmp.end())
      {                                                                                                    // the last peak is reached
        regressionGroups.push_back(std::vector<std::unique_ptr<q::Algorithms::qPeaks::validRegression>>());            // create a new group
        std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back())); // move all regressions from start to it_peak to this group
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
        calcExtendedMse(Y, group, df);
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
      const q::Matrices::Vector &Ylog,
      const q::Matrices::Vector &Y,
      const q::Matrices::BoolVector &df)
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
            const q::Matrices::Vector yhat = calcYhatExtended(
                (*designMatrices[(*it_ref_peak)->scale]),                                                                // design matrix
                (*it_ref_peak)->B,                                                                                       // coefficients vector
                (*it_ref_peak)->X_row_0,                                                                                 // starting row for the design matrix
                (*it_ref_peak)->X_row_1);                                                                                // ending row for the design matrix
            (*it_ref_peak)->mse = calcSSEexp(yhat, Y, Y.begin() + (int)(*it_ref_peak)->left_limit) / (*it_ref_peak)->df; // calculate the mean squared error (MSE) between the predicted and actual values
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
        const q::Matrices::Vector yhat = calcYhatExtended(
            (*designMatrices[(*it_new_peak)->scale]),                                                                // design matrix
            (*it_new_peak)->B,                                                                                       // coefficients vector
            (*it_new_peak)->X_row_0,                                                                                 // starting row for the design matrix
            (*it_new_peak)->X_row_1);                                                                                // ending row for the design matrix
        (*it_new_peak)->mse = calcSSEexp(yhat, Y, Y.begin() + (int)(*it_new_peak)->left_limit) / (*it_new_peak)->df; // calculate the mean squared error (MSE) between the predicted and actual values
      }

      if (numPeaksInGroup == 1)
      {
        // calculate the extended MSE using the current peak and the ref peak and set the worse one to invalid
        // create a temporary std::vector<std::unique_ptr<validRegression>> with the new peak and the ref peak
        std::vector<std::unique_ptr<validRegression>> tmpRegressions;
        tmpRegressions.push_back(std::move(*it_new_peak));
        tmpRegressions.push_back(std::move(validRegressionsInGroup[0][0]));
        calcExtendedMse(Y, tmpRegressions, df); // @todo is not working that way.
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
      const q::Matrices::Vector &Y,
      const q::Matrices::Vector &X,
      const int scanNumber)
  {
    std::vector<std::unique_ptr<DataType::Peak>> peaks; // peak list
    // iterate over the validRegressions vector
    for (auto &regression : validRegressions)
    {
      // re-scale the apex position to x-axis
      const double x0 = X[(int)std::floor(regression->apex_position)];
      const double dx = X[(int)std::ceil(regression->apex_position)] - x0;
      const double apex_position = x0 + dx * (regression->apex_position - std::floor(regression->apex_position));

      // create a new peak object and push it to the peaks vector; the peak object is created using the scan number, the apex position and the peak height
      peaks.push_back(std::make_unique<DataType::Peak>(
          scanNumber,
          apex_position,
          regression->height));

      // add additional information to the peak object
      peaks.back()->area = regression->area;
      peaks.back()->areaUncertainty = regression->uncertainty_area;
      peaks.back()->heightUncertainty = regression->uncertainty_height;
      peaks.back()->positionUncertainty = regression->uncertainty_position * dx;
      peaks.back()->dqsPeak = regression->dqs;
      peaks.back()->beta0 = regression->B[0];
      peaks.back()->beta1 = regression->B[1];
      peaks.back()->beta2 = regression->B[2];
      peaks.back()->beta3 = regression->B[3];
      peaks.back()->x0 = X[regression->index_x0];
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
  double
  qPeaks::calcSSE(
      const q::Matrices::Vector &yhat,
      const q::Matrices::Vector &y,
      const double *y_start) const
  {
    if (y_start == nullptr)
    {
      y_start = y.begin();
    }
    return std::inner_product(
        yhat.begin(),  // start of yhat
        yhat.end(),    // end of yhat
        y_start,       // start of y
        0.0,           // initial value
        std::plus<>(), // binary operation
        [](double a, double b)
        {
          double diff = a - b; // calculate the difference between a and b
          return diff * diff;  // return the square of the difference
        });
  } // end calcSSE

  double
  qPeaks::calcSSEexp(
      const q::Matrices::Vector &yhat_log,
      const q::Matrices::Vector &Y,
      const double *y_start) const
  {
    if (y_start == nullptr)
    {
      y_start = Y.begin();
    }
    return std::inner_product(
        yhat_log.begin(), // start of yhat_log
        yhat_log.end(),   // end of yhat_log
        y_start,          // start of y
        0.0,              // initial value
        std::plus<>(),    // binary operation
        [this](double a, double b)
        {
          double diff = exp_approx(a) - b; // calculate the difference between exp(a) and b
          return diff * diff;              // return the square of the difference
        });
  } // end calcSSEexp
#pragma endregion calcSSE

#pragma region calcExtendedMse
  void
  qPeaks::calcExtendedMse(
      const q::Matrices::Vector &Y,                                                  // measured data (not log-transformed data)
      const std::vector<std::unique_ptr<validRegression>> &regressions, // regressions to compare
      const q::Matrices::BoolVector &df)                                             // degrees of freedom
  {
    /*
      The function consists of the following steps:
      1. Identify left and right limit of the grouped regression windows.
      2. Calculate yhat based on the coefficients matrix B and the extended design matrix X.
      3. Calculate the mean squared error (MSE) between the predicted and actual values.
      4. Identify the best regression based on the MSE and return the MSE and the index of the best regression.
    */
    // declare variables
    double best_mse = std::numeric_limits<double>::infinity();
    auto best_regression = regressions.begin();

    // step 1: identify left and right limit of the grouped regression windows
    const int left_limit = regressions.front()->left_limit;
    const int right_limit = regressions.back()->right_limit;

    const int df_sum = calcDF(df, left_limit, right_limit);
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
      // step 2: calculate yhat based on the coefficients matrix B and the extended design matrix X
      const q::Matrices::Vector yhat = calcYhat(
          left_limit - (*regression)->index_x0,
          right_limit - (*regression)->index_x0,
          (*regression)->B);
      // step 3: calculate the mean squared error (MSE) between the predicted and actual values
      const double mse = calcSSEexp(yhat, Y, Y.begin() + left_limit) / (df_sum - 4);

      // step 4: identify the best regression based on the MSE and return the MSE and the index of the best regression
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

#pragma region calcChiSquareEXP
  double
  qPeaks::calcChiSquareEXP(
      const q::Matrices::Vector &yhat_log,
      const q::Matrices::Vector &Y,
      const double *y_start) const
  {
    if (y_start == nullptr)
    {
      y_start = Y.begin();
    }

    size_t n = yhat_log.n;
    double sum = 0.0;

    q::Matrices::Vector yhat_exp(n);
    std::transform(yhat_log.begin(), yhat_log.end(), yhat_exp.begin(), exp_approx);

    for (size_t i = 0; i < n; ++i)
    {
      double diff = yhat_exp[i] - *y_start;
      sum += diff * diff / yhat_exp[i];
      ++y_start;
    }
    return sum;
  } // end calcChiSquareEXP
#pragma endregion calcChiSquareEXP

#pragma region calcDF
  int qPeaks::calcDF(
      const q::Matrices::BoolVector &df,     // degrees of freedom
      const size_t left_limit,  // left limit
      const size_t right_limit) // right limit
  {
    return std::accumulate(
        df.begin() + left_limit,
        df.begin() + right_limit + 1,
        0,
        [](int sum, bool p)
        { return sum + p; });
  } // end calcDF
#pragma endregion calcDF

#pragma region calculateApexAndValleyPositions
  bool
  qPeaks::calculateApexAndValleyPositions(
      const q::Matrices::Matrix_mc &B,
      const size_t index,
      const int scale,
      double &apex_position,
      double &valley_position) const
  {
    // extract the coefficients
    const double B1 = B(1, index);
    const double B2 = B(2, index);
    const double B3 = B(3, index);
    int key = (B1 < 0 ? 4 : 0) + (B2 < 0 ? 2 : 0) + (B3 < 0 ? 1 : 0);
    switch (key)
    {
    case 7:                              // Case 1a: apex left
      apex_position = -B1 / 2 / B2;      // is negative
      valley_position = 0;               // no valley point
      return apex_position > -scale + 1; // scale +1: prevent appex position to be at the edge of the data

    case 3:                             // Case 1b: apex right
      apex_position = -B1 / 2 / B3;     // is positive
      valley_position = 0;              // no valley point
      return apex_position < scale - 1; // scale -1: prevent appex position to be at the edge of the data

    case 6:                                                                     // Case 2a: apex left | valley right
      apex_position = -B1 / 2 / B2;                                             // is negative
      valley_position = -B1 / 2 / B3;                                           // is positive
      return apex_position > -scale + 1 && valley_position - apex_position > 2; // scale +1: prevent appex position to be at the edge of the data

    case 1:                                                                    // Case 2b: apex right | valley left
      apex_position = -B1 / 2 / B3;                                            // is positive
      valley_position = -B1 / 2 / B2;                                          // is negative
      return apex_position < scale - 1 && apex_position - valley_position > 2; // scale -1: prevent appex position to be at the edge of the data

    default:
      return false; // invalid case
    } // end switch
  }
#pragma endregion calculateApexAndValleyPositions

#pragma region isValidQuadraticTerm
  bool
  qPeaks::isValidQuadraticTerm(
      const q::Matrices::Matrix_mc &B,
      const size_t index,
      const double inverseMatrix_2_2,
      const double inverseMatrix_3_3,
      const double mse,
      const int df_sum) const
  {
    double tValue = std::max(                                        // t-value for the quadratic term
        std::abs(B(2, index)) / std::sqrt(inverseMatrix_2_2 * mse),  // t-value for the quadratic term left side of the peak
        std::abs(B(3, index)) / std::sqrt(inverseMatrix_3_3 * mse)); // t-value for the quadratic term right side of the peak
    return tValue > tValuesArray[df_sum - 5];                        // statistical significance of the quadratic term
  }
#pragma endregion isValidQuadraticTerm

#pragma region isValidPeakHeight
  bool
  qPeaks::isValidPeakHeight(
      const q::Matrices::Matrix_mc &B,
      const q::Matrices::Matrix_mc_4x4 &C,
      const size_t index,
      const int scale,
      const double apex_position,
      const int df_sum,
      double &height,
      double &uncertainty_height,
      double &uncertainty_position) const
  {
    const double b0 = B(0, index);
    const double b1 = B(1, index);
    const double b2 = B(2, index);
    const double b3 = B(3, index);
    double Jacobian_height[4]; // Jacobian matrix for the height
    height = std::exp(b0 + apex_position * b1 * .5);
    Jacobian_height[0] = height;
    Jacobian_height[1] = apex_position * height;
    double Jacobian_position[4]; // Jacobian matrix for the position
    Jacobian_position[0] = 0;
    Jacobian_position[1] = apex_position / b1;
    if (apex_position < 0)
    {
      Jacobian_height[2] = apex_position * Jacobian_height[1];
      Jacobian_height[3] = 0;
      Jacobian_position[2] = -apex_position / b2;
      Jacobian_position[3] = 0;
    }
    else
    {
      Jacobian_height[2] = 0;
      Jacobian_height[3] = apex_position * Jacobian_height[1];
      Jacobian_position[2] = 0;
      Jacobian_position[3] = -apex_position / b3;
    }
    uncertainty_height = std::sqrt(multiplyVecMatrixVecTranspose(Jacobian_height, C));
    uncertainty_position = std::sqrt(multiplyVecMatrixVecTranspose(Jacobian_position, C));

    if (height / uncertainty_height <= tValuesArray[df_sum - 5]) // statistical significance of the peak height
    {
      return false;
    }

    // check if the peak height is significantly greater than base signal
    const double B1_2_B2 = b1 / 2 / b2;
    const double B1_2_B3 = b1 / 2 / b3;
    double x_left = -scale;
    double x_right = scale;

    // adjust x limits
    if (b2 > 0 && -B1_2_B2 > -scale)
    { // valley point is inside the window, use valley point as limit
      x_left = -B1_2_B2;
    }
    if (b3 > 0 && -B1_2_B3 < scale)
    { // valley point is inside the window, use valley point as limit
      x_right = -B1_2_B3;
    }
    double base_signal = (std::exp(b0 + b1 * x_left + b2 * x_left * x_left) + std::exp(b0 + b1 * x_right + b3 * x_right * x_right)) * .5;
    // std::cout << "base_signal: " << base_signal << " height: " << height << " uncertainty_height: " << uncertainty_height << " tValue: " << (height - 2* base_signal) / uncertainty_height << std::endl;
    return (height - 2 * base_signal) / uncertainty_height > tValuesArray[df_sum - 5];
  }
#pragma endregion isValidPeakHeight

#pragma region isValidPeakArea
  bool
  qPeaks::isValidPeakArea(
      const q::Matrices::Matrix_mc &B,
      const q::Matrices::Matrix_mc_4x4 &C,
      const size_t index,
      const int scale,
      const int df_sum,
      double &area,
      double &uncertainty_area) const
  {
    // predefine expressions
    const double b0 = B(0, index);
    const double b1 = B(1, index);
    const double b2 = B(2, index);
    const double b3 = B(3, index);
    const double SQRTB2 = std::sqrt(std::abs(-b2));
    const double SQRTB3 = std::sqrt(std::abs(-b3));
    const double EXP_B0 = std::exp(b0);
    const double B1_2_B2 = b1 / 2 / b2;
    const double EXP_B12 = std::exp(-b1 * B1_2_B2 / 2);
    const double B1_2_B3 = b1 / 2 / b3;
    const double EXP_B13 = std::exp(-b1 * B1_2_B3 / 2);

    // initialize variables
    double J[4];            // Jacobian matrix
    double J_covered[4];    // Jacobian matrix for the covered peak area
    double x_left = -scale; // left limit due to the window
    double x_right = scale; // right limit due to the window
    double y_left = 0;      // y value at the left limit
    double y_right = 0;     // y value at the right limit

    // here we have to check if there is a valley point or not
    const double err_L =
        (b2 < 0)
            ? 1 - std::erf(b1 / 2 / SQRTB2) // ordinary peak
            : erfi(b1 / 2 / SQRTB2);        // peak with valley point;

    const double err_R =
        (b3 < 0)
            ? 1 + std::erf(b1 / 2 / SQRTB3) // ordinary peak
            : -erfi(b1 / 2 / SQRTB3);       // peak with valley point ;

    // calculate the Jacobian matrix terms
    const double J_1_common_L = SQRTPI_2 * EXP_B0 * EXP_B12 / SQRTB2;
    const double J_1_common_R = SQRTPI_2 * EXP_B0 * EXP_B13 / SQRTB3;
    const double J_2_common_L = B1_2_B2 * EXP_B0 / b1;
    const double J_2_common_R = B1_2_B3 * EXP_B0 / b1;
    const double J_1_L = J_1_common_L * err_L;
    const double J_1_R = J_1_common_R * err_R;
    const double J_2_L = J_2_common_L - J_1_L * B1_2_B2;
    const double J_2_R = -J_2_common_R - J_1_R * B1_2_B3;

    J[0] = J_1_R + J_1_L;
    J[1] = J_2_R + J_2_L;
    J[2] = -B1_2_B2 * (J_2_L + J_1_L / b1);
    J[3] = -B1_2_B3 * (J_2_R + J_1_R / b1);

    area = J[0];
    uncertainty_area = std::sqrt(multiplyVecMatrixVecTranspose(J, C));

    if (area / uncertainty_area <= tValuesArray[df_sum - 5])
    {
      return false;
    }

    const double err_L_covered = ///@todo : need to be revised
        (b2 < 0)
            ? // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            std::erf((b1 - 2 * b2 * scale) / 2 / SQRTB2) + err_L - 1
            : // valley point, i.e., check position
            (-B1_2_B2 < -scale)
                ? // valley point is outside the window, use scale as limit
                err_L - erfi((b1 - 2 * b2 * scale) / 2 / SQRTB2)
                : // valley point is inside the window, use valley point as limit
                err_L;

    const double err_R_covered = ///@todo : need to be revised
        (b3 < 0)
            ? // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
            err_R - 1 - std::erf((b1 + 2 * b3 * scale) / 2 / SQRTB3)
            : // valley point, i.e., check position
            (-B1_2_B3 > scale)
                ? // valley point is outside the window, use scale as limit
                erfi((b1 + 2 * b3 * scale) / 2 / SQRTB3) + err_R
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
    y_left = std::exp(b0 + b1 * x_left + b2 * x_left * x_left);
    y_right = std::exp(b0 + b1 * x_right + b3 * x_right * x_right);
    const double dX = x_right - x_left;

    // calculate the trapzoid correction terms for the jacobian matrix
    const double trpzd_b0 = (y_right + y_left) * dX / 2;
    const double trpzd_b1 = (x_right * y_right + x_left * y_left) * dX / 2;
    const double trpzd_b2 = (x_left * x_left * y_left) * dX / 2;
    const double trpzd_b3 = (x_right * x_right * y_right) * dX / 2;

    const double J_1_L_covered = J_1_common_L * err_L_covered;
    const double J_1_R_covered = J_1_common_R * err_R_covered;
    const double J_2_L_covered = J_2_common_L - J_1_L_covered * B1_2_B2;
    const double J_2_R_covered = -J_2_common_R - J_1_R_covered * B1_2_B3;

    J_covered[0] = J_1_R_covered + J_1_L_covered - trpzd_b0;
    J_covered[1] = J_2_R_covered + J_2_L_covered - trpzd_b1;
    J_covered[2] = -B1_2_B2 * (J_2_L_covered + J_1_L_covered / b1) - trpzd_b2;
    J_covered[3] = -B1_2_B3 * (J_2_R_covered + J_1_R_covered / b1) - trpzd_b3;

    const double area_uncertainty_covered = std::sqrt(multiplyVecMatrixVecTranspose(J_covered, C));

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

#pragma region "yhat"
  q::Matrices::Vector
  qPeaks::calcYhat(
      const int left_limit,
      const int right_limit,
      const q::Matrices::Matrix_mc &beta,
      const size_t idx)
  {
    size_t n = right_limit - left_limit + 1;
    size_t j = 0;
    q::Matrices::Vector yhat(n);
    for (int i = left_limit; i <= right_limit; i++)
    {
      if (i < 0)
        yhat[j] = beta(0, idx) + beta(1, idx) * i + beta(2, idx) * x_square[std::abs(i)];
      else
        yhat[j] = beta(0, idx) + beta(1, idx) * i + beta(3, idx) * x_square[i];
      j++;
    }
    return yhat;
  }

  q::Matrices::Vector
  qPeaks::calcYhat(
      const int left_limit,
      const int right_limit,
      const q::Matrices::Vector &beta)
  {
    size_t n = right_limit - left_limit + 1;
    size_t j = 0;
    q::Matrices::Vector yhat(n);
    for (int i = left_limit; i <= right_limit; i++)
    {
      if (i < 0)
        yhat[j] = beta[0] + beta[1] * i + beta[2] * x_square[std::abs(i)];
      else
        yhat[j] = beta[0] + beta[1] * i + beta[3] * x_square[i];
      j++;
    }
    return yhat;
  }
#pragma endregion "yhat"

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
  q::Matrices::Matrix_mc
  qPeaks::convolve_fast(
      const size_t scale,
      const double (&vec)[500],
      const size_t n)
  {
    if (n < 2 * scale + 1)
    {
      throw std::invalid_argument("n must be greater or equal to 2 * scale + 1");
    }

    size_t k = 2 * scale + 1;
    size_t n_segments = n - k + 1;
    size_t centerpoint = k / 2;

    // double result[4][n_segments];
    // Matrix_mc result(4, n_segments);
    double result[n_segments][4] = {0};
    double products[500][4];
    // define the kernel offsets
    double kernel_r0 = invArray[scale][0];
    double kernel_r1 = 0;
    double kernel_r2 = invArray[scale][1];
    double kernel_r3 = invArray[scale][1];

    // define the kernel increments
    double kernel_r0_incr = -invArray[scale][1];
    double kernel_r1_incr = -invArray[scale][2] - invArray[scale][3];
    double kernel_r2_incr = -invArray[scale][3] - invArray[scale][4];
    double kernel_r3_incr = invArray[scale][3] - invArray[scale][5];

    // define increment updates
    double kernel_r0_incr_update = 2 * invArray[scale][1];
    double kernel_r1_incr_update = 2 * invArray[scale][3];
    double kernel_r2_incr_update = 2 * invArray[scale][4];
    double kernel_r3_incr_update = 2 * invArray[scale][5];
    // calculation of the center terms
    for (size_t i = 0; i < n_segments; i++)
    {
      result[i][0] += vec[i + centerpoint] * kernel_r0;
      result[i][2] += vec[i + centerpoint] * kernel_r2;
      result[i][3] = result[i][2];
    }
    // calculation from center to right (excluding center)
    for (size_t i = 1; i < scale + 1; i++)
    {
      int u = 0;
      // update kernel increments
      kernel_r0_incr += kernel_r0_incr_update;
      kernel_r1_incr += kernel_r1_incr_update;
      kernel_r2_incr += kernel_r2_incr_update;
      kernel_r3_incr += kernel_r3_incr_update;

      // update kernel values
      kernel_r0 += kernel_r0_incr;
      kernel_r1 += kernel_r1_incr;
      kernel_r2 += kernel_r2_incr;
      kernel_r3 += kernel_r3_incr;

      for (size_t j = scale - i; j < (n - scale + i); j++)
      {
        products[u][0] = vec[j] * kernel_r0;
        products[u][1] = vec[j] * kernel_r1;
        products[u][2] = vec[j] * kernel_r2;
        products[u][3] = vec[j] * kernel_r3;
        u++;
      }

      for (size_t j = 0; j < n_segments; j++)
      {
        if (2 * i + j >= 500)
        {
          throw std::out_of_range("Index out of range for products array: n=" + std::to_string(n) + " i=" + std::to_string(i) + " j=" + std::to_string(j));
        }
        result[j][0] += products[j][0] + products[2 * i + j][0];
        result[j][1] -= products[j][1] - products[2 * i + j][1];
        result[j][2] += products[j][3] + products[2 * i + j][2];
        result[j][3] += products[j][2] + products[2 * i + j][3];
      }
    }
    // create Matrix_mc object
    q::Matrices::Matrix_mc resultMatrix(4, n_segments);
    for (size_t i = 0; i < n_segments; i++)
    {
      resultMatrix(0, i) = result[i][0];
      resultMatrix(1, i) = -result[i][1];
      resultMatrix(2, i) = result[i][3];
      resultMatrix(3, i) = result[i][2];
    }

    return resultMatrix;
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
