// qalgorithms_qpeaks.cpp
//
// internal
#include "../include/qalgorithms_qpeaks.h"

// external

namespace q
{
#pragma region Constructors
  // Constructor
  qPeaks::qPeaks() {}

  qPeaks::qPeaks(const varDataType &dataVec)
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
          this->global_maxScale = 30;
          // iterate over the range of the maxScale to create Matrices for each scale
          for (int scale = 2; scale <= this->global_maxScale; scale++)
          {
            createDesignMatrix(scale);
            createInverseAndPseudoInverse(*(designMatrices.back()));
          }
        },
        dataVec);
  }
#pragma endregion Constructors

#pragma region Destructor
  // Destructor
  qPeaks::~qPeaks() {}
#pragma endregion Destructor

#pragma region Methods
#pragma region findPeaks
  std::vector<std::vector<std::unique_ptr<DataType::Peak>>> qPeaks::findPeaks(
      const varDataType &dataVec)
  {
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>> all_peaks;
    std::visit(
        [&all_peaks, this](auto &&arg)
        {
          all_peaks.resize(arg->size());
      // iterate over the map of varDataType datatype objects
      // use parallel for loop to iterate over the dataVec
#pragma omp parallel for
          for (int i = 0; i < arg->size(); i++)
          {
            // de-reference the unique pointer of the object
            auto &pair =  (*arg)[i];
            auto &dataObj = *(pair.get());
            auto &data = dataObj.dataPoints;
            int n = data.size();
            // store x and y values in RefMatrix objects
            // RefMatrix X(n, 1);
            RefMatrix Y(n, 1);
            std::vector<double*> X(n);
            std::vector<int*> df(n);
            for (size_t i = 0; i < n; i++)
            {
              X[i] = &data[i]->x();
              df[i] = &data[i]->df;
              Y.assignRef(i, 0, data[i]->y());
            }
            std::vector<std::unique_ptr<validRegression>> validRegressions; // index in B, scale, apex_position and MSE
            runningRegression(Y, df, validRegressions);
            if (validRegressions.empty())
            {
              continue; // no valid peaks
            }
            all_peaks[i] = createPeaks(validRegressions, Y, X, dataObj.getScanNumber());
          } // end parallel for loop
        ; },
        dataVec); // end visit
    return all_peaks;
  } // end findPeaks
#pragma endregion findPeaks

#pragma region runningRegression
  void qPeaks::runningRegression(
      const RefMatrix &Y,
      const std::vector<int *> &df,
      std::vector<std::unique_ptr<validRegression>> &validRegressions)
  {
    // perform log-transform on Y
    size_t n = Y.getRows();
    Matrix Ylog = Y.log();

    int maxScale = std::min(this->global_maxScale, (int)(n - 1) / 2);
    validRegressions.reserve(calculateNumberOfRegressions(n));
    for (int scale = 2; scale <= maxScale; scale++)
    {
      // coeffiencts matrix B of the running regression
      Matrix b = Ylog.convoleCombiend(*(psuedoInverses[scale - 2]));
      validateRegressions(b, Y, Ylog, df, scale, validRegressions);
    } // end for scale loop
    mergeRegressionsOverScales(validRegressions, Ylog, Y, df);
  } // end runningRegression
#pragma endregion runningRegression

#pragma region validateRegressions
  void qPeaks::validateRegressions(
      const Matrix &B,
      const RefMatrix &Y,
      const Matrix &Ylog,
      const std::vector<int *> &df,
      const int scale,
      std::vector<std::unique_ptr<validRegression>> &validRegressions)
  {
    // declare constants
    const double index_offset = (scale - 2) * Ylog.numRows() - scale * (scale - 1) + 2;
    // declare variables
    int df_sum = 0;
    double valley_position;
    double apex_position;
    double inverseMatrix_2_2 = (*(inverseMatrices[scale - 2]))(2, 2);  // variance of the quadratic term left side of the peak
    double inverseMatrix_3_3 = (*(inverseMatrices[scale - 2]))(3, 3);  // variance of the quadratic term right side of the peak
    Matrix &X = *(designMatrices[scale - 2]);                          // design matrix
    Matrix Jacobian_height(1, 4);                                      // Jacobian matrix for the height
    std::vector<std::unique_ptr<validRegression>> validRegressionsTmp; // temporary vector to store valid regressions <index, apex_position>

    // iterate columwise over the coefficients matrix
    for (size_t i = 0; i < B.numCols(); i++)
    {
      /*
        Degree of Freedom Filter:
        This block of code implements the degree of freedom filter. It calculates the degree of freedom based df vector. If the degree of freedom is less than 5, the loop continues to the next iteration. The value 5 is chosen as the minimum number of data points required to fit a quadratic regression model.
      */
      df_sum = std::transform_reduce(
          df.begin() + i,
          df.begin() + 2 * scale + i,
          0,
          std::plus<>(),
          [](int *p)
          { return *p; });

      if (df_sum < 5)
      {
        continue; // degree of freedom less than 5; i.e., less then 5 measured data points
      }

      /*
        Apex and Valley Position Filter:
        This block of code implements the apex and valley position filter. It calculates the apex and valley positions based on the coefficients matrix B. If the apex is outside the data range, the loop continues to the next iteration. If the apex and valley positions are too close to each other, the loop continues to the next iteration.
      */
      // calculate the checksum for case differentiation
      int key = (B(1, i) < 0 ? 4 : 0) + (B(2, i) < 0 ? 2 : 0) + (B(3, i) < 0 ? 1 : 0);
      switch (key)
      {
      case 7:                                   // Case 1a: apex left
        apex_position = -B(1, i) / 2 / B(2, i); // is negative
        valley_position = 0;                    // no valley point
        if (apex_position <= -scale + 1)        // scale +1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex position
        }
        break;
      case 3:                                   // Case 1b: apex right
        apex_position = -B(1, i) / 2 / B(3, i); // is positive
        valley_position = 0;                    // no valley point
        if (apex_position >= scale - 1)         // scale -1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex position
        }
        break;
      case 6:                                                                    // Case 2a: apex left | valley right
        apex_position = -B(1, i) / 2 / B(2, i);                                  // is negative
        valley_position = -B(1, i) / 2 / B(3, i);                                // is positive
        if (apex_position <= -scale + 1 || valley_position - apex_position <= 2) // scale +1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex and valley positions
        }
        break;
      case 1:                                                                   // Case 2b: apex right | valley left
        apex_position = -B(1, i) / 2 / B(3, i);                                 // is positive
        valley_position = -B(1, i) / 2 / B(2, i);                               // is negative
        if (apex_position >= scale - 1 || apex_position - valley_position <= 2) // scale -1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex and valley positions
        }
        break;
      default:
        continue; // invalid case
      } // end switch

      /*
        Quadratic Term Filter:
        This block of code implements the quadratic term filter. It calculates the mean squared error (MSE) between the predicted and actual values. Then it calculates the t-value for the quadratic term. If the t-value is less than the corresponding value in the tValuesArray, the quadratic term is considered statistically insignificant, and the loop continues to the next iteration.
      */
      double mse = calcMse( // mean squared error
          X * B.col(i),
          (Ylog.subMatrix(i, i + 2 * scale + 1, 0, 1)));
      // adjust mse by considering the real df
      mse *= (2 * scale - 3) / (df_sum - 4);

      double tValue = std::max(                                    // t-value for the quadratic term
          std::abs(B(2, i)) / std::sqrt(inverseMatrix_2_2 * mse),  // t-value for the quadratic term left side of the peak
          std::abs(B(3, i)) / std::sqrt(inverseMatrix_3_3 * mse)); // t-value for the quadratic term right side of the peak
      if (tValue < tValuesArray[df_sum - 5])                       // df is df_sum-4 but tValuesArray is zero-based
      {
        continue; // statistical insignificance of the quadratic term
      }

      /*
        Height Filter:
        This block of code implements the height filter. It calculates the height of the peak based on the coefficients matrix B. Then it calculates the uncertainty of the height based on the Jacobian matrix and the variance-covariance matrix of the coefficients. If the height is statistically insignificant, the loop continues to the next iteration.
      */
      double height = std::exp(B(0, i) + apex_position * B(1, i) * .5);
      Jacobian_height(0, 0) = height;
      Jacobian_height(0, 1) = apex_position * height;
      if (apex_position < 0)
      {
        Jacobian_height(0, 2) = apex_position * Jacobian_height(0, 1);
        Jacobian_height(0, 3) = 0;
      }
      else
      {
        Jacobian_height(0, 2) = 0;
        Jacobian_height(0, 3) = apex_position * Jacobian_height(0, 1);
      }
      Matrix C = (*inverseMatrices[scale - 2]) * mse; // variance-covariance matrix of the coefficients
      double h_uncertainty = std::sqrt((Jacobian_height * C * Jacobian_height.T()).sumElements());
      if (height / h_uncertainty < tValuesArray[df_sum - 5])
      {
        continue; // statistical insignificance of the height
      }

      /*
        Area Filter:
        This block of code implements the area filter. It calculates the Jacobian matrix for the peak area based on the coefficients matrix B. Then it calculates the uncertainty of the peak area based on the Jacobian matrix. If the peak area is statistically insignificant, the loop continues to the next iteration.
      */
      std::pair<Matrix, Matrix> Jacobian_area = jacobianMatrix_PeakArea(B.col(i), scale); // first: Jacobian matrix, second: Jacobian matrix for the covered peak area
      double area_uncertainty = std::sqrt(                                                // uncertainty of the peak area based on the Jacobian matrix and the non-covered peak area
          (Jacobian_area.first * C * Jacobian_area.first.T()).sumElements());

      if (Jacobian_area.first(0, 0) / area_uncertainty < tValuesArray[df_sum - 5])
      {
        continue; // statistical insignificance of the peak area
      }

      /*
        Covered Area Filter:
        This block of code implements the covered area filter. It calculates the covered peak area. Then it calculates the uncertainty of the peak area based on the Jacobian matrix. If the area is statistically insignificant, the loop continues to the next iteration.
      */
      double area_uncertainty_covered = std::sqrt( // uncertainty of the peak area based on the Jacobian matrix and the covered peak area
          (Jacobian_area.second * C * Jacobian_area.second.T()).sumElements());
      if (Jacobian_area.second(0, 0) / area_uncertainty_covered < tValuesArray[df_sum - 5])
      {
        continue; // statistical insignificance of the peak area
      }

      /*
        Add to a temporary vector of valid regressions:
        This block of code adds the valid peak to a temporary vector of valid regressions. It calculates the left and right limits of the peak based on the valley position. Then it stores the index of the valid regression in the temporary vector of valid regressions.
      */
      // at this point, the peak is validated
      double left_limit = (valley_position < 0) ? std::max((double)i, valley_position + i + scale) : i;
      double right_limit = (valley_position > 0) ? std::min((double)i + 2 * scale, valley_position + i + scale) : i + 2 * scale;

      validRegressionsTmp.push_back(
          std::make_unique<validRegression>(
              i + scale,                                            // index of the center of the window (x==0) in the Y matrix
              scale,                                                // scale
              df_sum - 4,                                           // df
              apex_position + i + scale,                            // apex position in x-axis 0:n
              0,                                                    // initial MSE
              B.col(i),                                             // coefficients matrix
              true,                                                 // isValid
              left_limit,                                           // left_limit
              right_limit,                                          // right_limit
              left_limit - i,                                       // X_row_0
              right_limit - i + 1,                                  // X_row_1
              area_uncertainty_covered / Jacobian_area.second(0, 0) // dqs
              ));
      // std::erf(                  // dqs (experimental)
      //     (1 - std::erf(area_uncertainty / Jacobian_area.first(0, 0))) / (1 - std::erf(1 / tValuesArray[df_sum - 5])) - 1)));
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
      { // the two regressions differ, i.e. create a new group and move all regressions from start to it_peak to this group

        regressionGroups.push_back(std::vector<std::unique_ptr<q::qPeaks::validRegression>>());            // create a new group
        std::move(validRegressionsTmp.begin(), it_peak_next, std::back_inserter(regressionGroups.back())); // move all regressions from start to it_peak to this group
        it_peak_next = validRegressionsTmp.erase(validRegressionsTmp.begin(), it_peak_next);               // erase the moved regressions from the temporary vector of valid regressions
      }
      it_peak = it_peak_next;
      ++it_peak_next;
      if (it_peak_next == validRegressionsTmp.end())
      {                                                                                                    // the last peak is reached
        regressionGroups.push_back(std::vector<std::unique_ptr<q::qPeaks::validRegression>>());            // create a new group
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
  void qPeaks::mergeRegressionsOverScales(
      std::vector<std::unique_ptr<validRegression>> &validRegressions,
      const Matrix &Ylog,
      const RefMatrix &Y,
      const std::vector<int *> &df)
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
      double left_limit = (*it_new_peak)->left_limit;             // left limit of the current peak regression window in the Y matrix
      double right_limit = (*it_new_peak)->right_limit;           // right limit of the current peak regression window in the Y matrix
      double grpMSE = 0;                                          // group mean squared error
      int grpDF = 0;                                              // group degree of freedom
      int numPeaksInGroup = 0;                                    // number of peaks in the group
      auto it_ref_peak = validRegressions.begin();                // iterator for the reference peak, i.e., a valid peak to compare with the new peak
      std::vector<decltype(it_ref_peak)> validRegressionsInGroup; // vector of iterators

      // iterate over the validRegressions vector till the new peak
      for (it_ref_peak; it_ref_peak < it_new_peak; ++it_ref_peak)
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
            (*it_ref_peak)->mse = calcMseEXP(
                ((*designMatrices[(*it_ref_peak)->scale - 2]).subMatrix((*it_ref_peak)->X_row_0, (*it_ref_peak)->X_row_1, 0, 4) * (*it_ref_peak)->B),
                (Y.subMatrix(
                    (*it_ref_peak)->left_limit,      // start row index
                    (*it_ref_peak)->right_limit + 1, // end row index
                    0,                               // start column index
                    1))                              // end column index
            );
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
        (*it_new_peak)->mse = calcMseEXP(
            ((*designMatrices[(*it_new_peak)->scale - 2]).subMatrix((*it_new_peak)->X_row_0, (*it_new_peak)->X_row_1, 0, 4) * (*it_new_peak)->B),
            (Y.subMatrix(
                (*it_new_peak)->left_limit,      // start row index
                (*it_new_peak)->right_limit + 1, // end row index
                0,                               // start column index
                1))                              // end column index
        );
      }

      if (numPeaksInGroup == 1)
      {
        // calculate the extended MSE using the current peak and the ref peak and set the worse one to invalid
        calcExtendedMse(Y, *(*it_new_peak), (*validRegressionsInGroup[0][0]), df);
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
  std::vector<std::unique_ptr<DataType::Peak>> qPeaks::createPeaks(
      const std::vector<std::unique_ptr<validRegression>> &validRegressions,
      const RefMatrix &Y,
      const std::vector<double *> &X,
      const int scanNumber)
  {
    std::vector<std::unique_ptr<DataType::Peak>> peaks; // peak list
    // iterate over the validRegressions vector
    for (auto &regression : validRegressions)
    {
      // re-scale the apex position to x-axis
      double x0 = *X[(int)std::floor(regression->apex_position)];
      double dx = *X[(int)std::ceil(regression->apex_position)] - x0;
      double apex_position = x0 + dx * (regression->apex_position - std::floor(regression->apex_position));
      // create a new peak object and push it to the peaks vector; the peak object is created using the scan number, the apex position and the peak height
      peaks.push_back(std::make_unique<DataType::Peak>(
          scanNumber,
          apex_position,
          (regression->B(1, 0) > 0)
              ? std::exp(regression->B(0, 0) - regression->B(1, 0) * regression->B(1, 0) / 4 / regression->B(3, 0))
              : std::exp(regression->B(0, 0) - regression->B(1, 0) * regression->B(1, 0) / 4 / regression->B(2, 0))));
      // debugging

      // get the coefficients matrix
      Matrix B = regression->B;

      // get the index of the regression
      int index = regression->index_x0;
      // get scale
      int scale = regression->scale;
      // design matrix
      Matrix Xdesign = (*(designMatrices[scale - 2])).subMatrix(regression->X_row_0, regression->X_row_1, 0, 4);

      // calculate yfit
      Matrix yfit_matrix = (Xdesign * B).exp();
      std::vector<double> yfit;
      for (int i = 0; i < yfit_matrix.numRows(); i++)
      {
        yfit.push_back(yfit_matrix(i, 0));
      }
      // extract xfit, i.e., from X vector index to index + 2*scale+1
      std::vector<double> xfit;
      for (int i = regression->left_limit; i < regression->right_limit + 1; i++)
      {
        if (i < 0 || i >= X.size())
        {
          continue;
        }
        xfit.push_back(*X[i]);
      }

      peaks.back()->xFit = xfit;
      peaks.back()->yFit = yfit;

      // std::cout << "-----------------------------------\n";
      // std::cout << "Apex Position: " << apex_position << std::endl;
      // std::cout << "Height: " << peaks.back()->height << std::endl;
      // std::cout << "DQS: " << regression->dqs << std::endl;
    } // end for loop

    return peaks;
  } // end createPeaks
#pragma endregion createPeaks

#pragma region calcMse
  double qPeaks::calcMse(const Matrix &yhat, const Matrix &y) const
  {
    size_t n = yhat.numel();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i)
    {
      double diff = yhat.getElement(i) - y.getElement(i);
      sum += diff * diff;
    }
    return sum / (n - 4);
  } // end calcMse

  double qPeaks::calcMse(const Matrix &yhat, const RefMatrix &Y) const
  {
    size_t n = yhat.numel();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i)
    {
      double diff = yhat.getElement(i) - Y.getElement(i);
      sum += diff * diff;
    }
    return sum / (n - 4);
  } // end calcMse

  double qPeaks::calcMseEXP(const Matrix &yhat_log, const RefMatrix &Y) const
  {
    size_t n = yhat_log.numel();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i)
    {
      double diff = std::exp(yhat_log.getElement(i)) - Y.getElement(i);
      sum += diff * diff;
    }
    return sum / (n - 4);
  } // end calcMse
#pragma endregion calcMse

#pragma region calcExtendedMse
  void qPeaks::calcExtendedMse(
      const RefMatrix &Y,                                               // measured data (not log-transformed data)
      const std::vector<std::unique_ptr<validRegression>> &regressions, // regressions to compare
      const std::vector<int *> &df)                                     // degrees of freedom
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
    int left_limit = regressions.front()->left_limit;
    int right_limit = regressions.back()->right_limit;

    for (auto regression = regressions.begin(); regression != regressions.end(); ++regression)
    {
      // get the extended scale
      int extendedScale = std::max(
          (*regression)->index_x0 - left_limit, // left limit
          right_limit - (*regression)->index_x0 // right limit
      );

      // check if it is necessary to create new design matrices based on the extended scale
      if (designMatrices.size() < (extendedScale - 1)) // -1 because the design matrices are zero-based and start with scale 2
      {
#pragma omp critical
        {
          // scale is not available; create new design matrices from last available scale to the extended scale in steps of 1
          for (int new_scale = designMatrices.size() + 2; new_scale < extendedScale + 1; new_scale++)
          {
            createDesignMatrix(new_scale);
            createInverseAndPseudoInverse(*(designMatrices.back()));
          }
        }
      }

      // step 2: calculate yhat based on the coefficients matrix B and the extended design matrix X
      // extract and cut the design matrix based on the extended scale
      int row_0 = extendedScale - (*regression)->index_x0 + left_limit;      // start row index for cutting Design Matrix
      int row_1 = extendedScale - (*regression)->index_x0 + right_limit + 1; // end row index for cutting Design Matrix

      Matrix X = (*designMatrices[extendedScale - 2]).subMatrix(row_0, row_1, 0, 4);
      // calculate yhat based on the coefficients matrix B and the extended design matrix X
      Matrix yhat = X * (*regression)->B;

      // step 3: calculate the mean squared error (MSE) between the predicted and actual values
      double mse = calcMseEXP(yhat, Y.subMatrix(left_limit, right_limit + 1, 0, 1));
      // correct the df for the mse calculation
      int df_sum = 0;
      for (int i = left_limit; i < right_limit + 1; i++)
      {
        df_sum += *df[i];
      }
      if (df_sum <= 4)
      {
        continue; // not enough degrees of freedom
      }
      mse *= (yhat.numRows() - 4) / (df_sum - 4);

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
  }   // end qPeaks::calcExtendedMse

    void qPeaks::calcExtendedMse(
        const RefMatrix &Y,                 // measured data (not log-transformed data)
        validRegression &currentRegression, // current regression
        validRegression &refRegression,     // reference regression
        const std::vector<int *> &df        // degrees of freedom
    )
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
      int best_idx = -1;
      int best_extendedScale = -1;
      int best_df = 0;
      int best_left_limit = -1;
      int best_right_limit = -1;
      int best_X_row_0 = -1;
      int best_X_row_1 = -1;
      // step 1: identify left and right limit of the grouped regression windows
      int left_limit = std::min(currentRegression.left_limit, refRegression.left_limit);
      int right_limit = std::max(currentRegression.right_limit, refRegression.right_limit);
      int flag = 0;
      int stepInLoop = 0;
      for (auto regression : {&currentRegression, &refRegression})
      {
        // get the extended scale
        int extendedScale = std::max(
            regression->index_x0 - left_limit, // left limit
            right_limit - regression->index_x0 // right limit
        );

        // check if it is necessary to create new design matrices based on the extended scale
        if (designMatrices.size() < (extendedScale - 1)) // -1 because the design matrices are zero-based and start with scale 2
        {
#pragma omp critical
          {
            // scale is not available; create new design matrices from last available scale to the extended scale in steps of 1
            for (int new_scale = designMatrices.size() + 2; new_scale < extendedScale + 1; new_scale++)
            {
              createDesignMatrix(new_scale);
              createInverseAndPseudoInverse(*(designMatrices.back()));
            }
          }
        }
        // step 2: calculate yhat based on the coefficients matrix B and the extended design matrix X
        // extract and cut the design matrix based on the extended scale
        int row_0 = left_limit - regression->index_x0 + extendedScale;      // start row index for cutting Design Matrix
        int row_1 = right_limit - regression->index_x0 + extendedScale + 1; // end row index for cutting Design Matrix
        // add an adjustment if there is a valleypoint between row_0 and row_1
        int left_valley_adjust = (regression->B(2, 0) < 0)
                                     ? // no valley point
                                     0
                                     : // has a valley point
                                     std::max(
                                         0,                                                                                          // valley point is outside the window
                                         (int)(-regression->B(1, 0) / 2 / regression->B(2, 0) + regression->index_x0 - left_limit)); // valley point is inside the window
        int right_valley_adjust = (regression->B(3, 0) < 0)
                                      ? // no valley point
                                      0
                                      : // has a valley point
                                      std::max(
                                          0,                                                                                          // valley point is outside the window
                                          (int)(right_limit + regression->B(1, 0) / 2 / regression->B(3, 0) - regression->index_x0)); // valley point is inside the window
        Matrix X = (*designMatrices[extendedScale - 2]).subMatrix(row_0 + left_valley_adjust, row_1 - right_valley_adjust, 0, 4);
        // calculate yhat based on the coefficients matrix B and the extended design matrix X
        Matrix yhat = X * regression->B;

        // step 3: calculate the mean squared error (MSE) between the predicted and actual values
        double mse = calcMseEXP(yhat, Y.subMatrix(left_limit + left_valley_adjust, right_limit + 1 - right_valley_adjust, 0, 1));
        // correct the df for the mse calculation
        int df_sum = 0;
        for (int i = left_limit + left_valley_adjust; i < right_limit + 1 - right_valley_adjust; i++)
        {
          df_sum += *df[i];
        }

        if (df_sum <= 4)
        {
          regression->isValid = false;
          stepInLoop++;
          continue; // not enough degrees of freedom
        }

        mse *= (yhat.numRows() - 4) / (df_sum - 4);
        // step 4: identify the best regression based on the MSE and return the MSE and the index of the best regression
        if (mse < best_mse)
        {
          best_mse = mse;
          flag = stepInLoop;
          stepInLoop++;
        }
        else
        {
          regression->isValid = false;
          stepInLoop++;
        }
      } // end for loop
      // set worse regression to invalid
      if (flag == 0)
      { // current regression is better than ref regression
        refRegression.isValid = false;
      }
      else
      { // ref regression is better than current regression
        currentRegression.isValid = false;
      }
    }
#pragma endregion calcExtendedMse

#pragma region jacobianMatrix_PeakArea
    std::pair<Matrix, Matrix> qPeaks::jacobianMatrix_PeakArea(const Matrix &B, int scale) const
    {
      // predefine expressions
      const double b0 = B(0, 0);
      const double b1 = B(1, 0);
      const double b2 = B(2, 0);
      const double b3 = B(3, 0);
      const double SQRTB2 = std::sqrt(std::abs(-b2));
      const double SQRTB3 = std::sqrt(std::abs(-b3));
      const double EXP_B0 = std::exp(b0);
      const double B1_2_B2 = b1 / 2 / b2;
      const double EXP_B12 = std::exp(-b1 * B1_2_B2 / 2);
      const double B1_2_B3 = b1 / 2 / b3;
      const double EXP_B13 = std::exp(-b1 * B1_2_B3 / 2);

      // initialize variables
      Matrix J(1, 4);         // Jacobian matrix
      Matrix J_covered(1, 4); // Jacobian matrix for the covered peak area
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

      // calculate the Jacobian matrix terms
      const double J_1_common_L = SQRTPI_2 * EXP_B0 * EXP_B12 / SQRTB2;
      const double J_1_common_R = SQRTPI_2 * EXP_B0 * EXP_B13 / SQRTB3;
      const double J_2_common_L = B1_2_B2 * EXP_B0 / b1;
      const double J_2_common_R = B1_2_B3 * EXP_B0 / b1;

      const double J_1_L = J_1_common_L * err_L;
      const double J_1_L_covered = J_1_common_L * err_L_covered;
      const double J_1_R = J_1_common_R * err_R;
      const double J_1_R_covered = J_1_common_R * err_R_covered;

      const double J_2_L = J_2_common_L - J_1_L * B1_2_B2;
      const double J_2_L_covered = J_2_common_L - J_1_L_covered * B1_2_B2;
      const double J_2_R = -J_2_common_R - J_1_R * B1_2_B3;
      const double J_2_R_covered = -J_2_common_R - J_1_R_covered * B1_2_B3;

      J(0, 0) = J_1_R + J_1_L;
      J(0, 1) = J_2_R + J_2_L;
      J(0, 2) = -B1_2_B2 * (J_2_L + J_1_L / b1);
      J(0, 3) = -B1_2_B3 * (J_2_R + J_1_R / b1);

      J_covered(0, 0) = J_1_R_covered + J_1_L_covered - trpzd_b0;
      J_covered(0, 1) = J_2_R_covered + J_2_L_covered - trpzd_b1;
      J_covered(0, 2) = -B1_2_B2 * (J_2_L_covered + J_1_L_covered / b1) - trpzd_b2;
      J_covered(0, 3) = -B1_2_B3 * (J_2_R_covered + J_1_R_covered / b1) - trpzd_b3;

      return std::pair<Matrix, Matrix>(J, J_covered);
    } // end jacobianMatrix_PeakArea
#pragma endregion jacobianMatrix_PeakArea

#pragma region createDesignMatrix
    void qPeaks::createDesignMatrix(const int scale)
    {
      // construct matrix
      size_t m = 4;
      size_t n = 2 * scale + 1;
      Matrix X(n, m);

      // x vector
      std::vector<int> x(n);
      for (size_t i = 0; i < n; i++)
      {
        x[i] = i - scale;
      }

      // order vector
      std::vector<int> p = {0, 1, 2, 2};

      for (size_t i = 0; i < n; i++)
      {
        for (size_t j = 0; j < m; j++)
        {
          X(i, j) = std::pow(x[i], p[j]);
        }
      }

      // modification of X due to p(2) and p(3)
      for (size_t i = 0; i < n; i++)
      {
        if (x[i] < 0)
        {
          X(i, 3) = 0;
        }
        else
        {
          X(i, 2) = 0;
        }
      }

      // add the matrix to the designMatrices vector
      designMatrices.push_back(std::make_unique<Matrix>(X));
    }
#pragma endregion createDesignMatrix

#pragma region createInverseAndPseudoInverse
    void qPeaks::createInverseAndPseudoInverse(const Matrix &X)
    {
      Matrix Xt = X.T();
      Matrix XtX = Xt * X;
      inverseMatrices.push_back(std::make_unique<Matrix>(XtX.inv()));
      psuedoInverses.push_back(std::make_unique<Matrix>(*(inverseMatrices.back()) * Xt));
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

#pragma region printMatrices
    void qPeaks::printMatrices(int scale) const
    {
      std::cout << "Design Matrix\n";
      designMatrices[scale - 2]->print();
      std::cout << "Inverse Matrix\n";
      inverseMatrices[scale - 2]->print();
      std::cout << "Pseudo-Inverse Matrix\n";
      psuedoInverses[scale - 2]->print();
    }
#pragma endregion printMatrices
#pragma endregion Methods
  } // namespace q
