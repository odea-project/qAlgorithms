// qalgorithms_qpeaks.cpp
//
// internal
#include "../include/qalgorithms_qpeaks.h"

// external

namespace q
{
  // Constructor
  qPeaks::qPeaks() {}

  qPeaks::qPeaks(const varDataType &dataVec)
  {
    std::visit([this](auto &&arg)
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
                  this->global_maxScale = 10;
                  // iterate over the range of the maxScale to create Matrices for each scale
                  for (int scale = 2; scale <= this->global_maxScale; scale++)
                  {
                    createDesignMatrix(scale);
                    createInverseAndPseudoInverse(*(designMatrices.back()));
                  } },
               dataVec);
  }

  // Destructor
  qPeaks::~qPeaks() {}

  std::vector<std::vector<std::unique_ptr<DataType::Peak>>> qPeaks::findPeaks(const varDataType &dataVec)
  {
    std::vector<std::vector<std::unique_ptr<DataType::Peak>>> all_peaks;
    std::visit([&all_peaks, this](auto &&arg)
               {
                all_peaks.resize(arg->size());
    // iterate over the map of varDataType datatype objects
    // use parallel for loop to iterate over the dataVec
#pragma omp parallel for
                // for (auto &pair : *arg)
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
                  std::vector<double> X(n);
                  for (size_t i = 0; i < n; i++)
                  {
                    // X.assignRef(i, 0, data[i]->x());
                    X[i] = data[i]->x();
                    Y.assignRef(i, 0, data[i]->y());
                  }
                  std::vector<std::unique_ptr<validRegression>> validRegressions; // index in B, scale, apex_position and MSE
                  runningRegression(Y, validRegressions);
                  all_peaks[i] = createPeaks(validRegressions, Y, X, dataObj.getScanNumber());
                } // end parallel for loop
                ; },
               dataVec); // end visit
    return all_peaks;
  } // end findPeaks

  void qPeaks::runningRegression(const RefMatrix &Y, std::vector<std::unique_ptr<validRegression>> &validRegressions)
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
      validateRegressions(b, Ylog, scale, validRegressions);
    } // end for scale loop
    mergeRegressionsOverScales(validRegressions, Ylog);
  } // end runningRegression

  void qPeaks::validateRegressions(const Matrix &B, const Matrix &Ylog, const int scale, std::vector<std::unique_ptr<validRegression>> &validRegressions)
  {
    // declase constants
    const double index_offset = (scale - 2) * Ylog.numRows() - scale * (scale - 1) + 2;
    // declare variables
    double valley_position;
    double apex_position;
    double inverseMatrix_2_2 = (*(inverseMatrices[scale - 2]))(2, 2);
    double inverseMatrix_3_3 = (*(inverseMatrices[scale - 2]))(3, 3);
    Matrix &X = *(designMatrices[scale - 2]);
    Matrix Jacobian_height(1, 4);
    std::vector<std::tuple<int, double, double>> valid_regressions_tmp; // temporary vector to store valid regressions <index, apex_position>

    // iterate columwise over the coefficients matrix
    for (size_t i = 0; i < B.numCols(); i++)
    {
      // coefficients filter
      // calculate the checksum for case differentiation
      int key = (B(1, i) < 0 ? 4 : 0) + (B(2, i) < 0 ? 2 : 0) + (B(3, i) < 0 ? 1 : 0);
      switch (key)
      {
      case 7:                                   // Case 1a: apex left
        apex_position = -B(1, i) / 2 / B(2, i); // is negative
        if (apex_position <= -scale+1) // scale +1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex position
        }
        break;
      case 3:                                   // Case 1b: apex right
        apex_position = -B(1, i) / 2 / B(3, i); // is positive
        if (apex_position >= scale-1) // scale -1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex position
        }
        break;
      case 6:                                     // Case 2a: apex left | valley right
        apex_position = -B(1, i) / 2 / B(2, i);   // is negative
        valley_position = -B(1, i) / 2 / B(3, i); // is positive
        if (apex_position <= -scale+1 || valley_position - apex_position <= 2) // scale +1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex and valley positions
        }
        break;
      case 1:                                     // Case 2b: apex right | valley left
        apex_position = -B(1, i) / 2 / B(3, i);   // is positive
        valley_position = -B(1, i) / 2 / B(2, i); // is negative
        if (apex_position >= scale-1 || apex_position - valley_position <= 2) // scale -1: prevent appex position to be at the edge of the data
        {
          continue; // invalid apex and valley positions
        }
        break;
      default:
        continue; // invalid case
      }           // end switch

      // quadratic term filter
      double mse = calcMse(X * B.col(i), (Ylog.subMatrix(i, i + 2 * scale + 1, 0, 1)));
      double tValue = std::max(std::abs(B(2, i)) / std::sqrt(inverseMatrix_2_2 * mse), std::abs(B(3, i)) / std::sqrt(inverseMatrix_3_3 * mse));
      if (tValue < tValuesArray[scale * 2 - 4]) // df is scale*2-3 but tValuesArray is zero-based
      {
        continue; // statistical insignificance of the quadratic term
      }

      // height filter
      double height = std::exp(B(0, i) + (apex_position * B(1, i) * .5));
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
      if (height / h_uncertainty < tValuesArray[scale * 2 - 4])
      {
        continue; // statistical insignificance of the height
      }

      // peak area filter
      std::pair<Matrix, double> Jacobian_area = jacobianMatrix_PeakArea(B.col(i), scale); // first: Jacobian matrix, second: non-covered peak area divided by degrees of freedom
      double area_uncertainty = std::sqrt((Jacobian_area.first * C * Jacobian_area.first.T()).sumElements() + Jacobian_area.second * Jacobian_area.second); // uncertainty of the peak area based on the Jacobian matrix and the non-covered peak area
      if (Jacobian_area.first(0, 0) / area_uncertainty < tValuesArray[scale * 2 - 4])
      {
        // continue; // statistical insignificance of the peak area
      }

      // at this point, the peak is validated
      valid_regressions_tmp.push_back(std::make_tuple(i, apex_position + i + scale,Jacobian_area.first(0, 0) / area_uncertainty));

    } // end for loop

    // early return if no or only one valid peak
    if (valid_regressions_tmp.size() < 2)
    {
      if (valid_regressions_tmp.empty())
      {
        return; // no valid peaks
      }
      validRegressions.push_back(std::make_unique<validRegression>(std::get<0>(valid_regressions_tmp[0]), scale, std::get<1>(valid_regressions_tmp[0]), 0, B.col(std::get<0>(valid_regressions_tmp[0]))));
      return; // not enough peaks to form a group
    }

    // group the valid peaks
    std::vector<std::vector<int>> groups; // vector of peak indices
    // initialize iterators for valid_regressions_tmp
    auto it = valid_regressions_tmp.begin();
    auto it_next = std::next(it);
    // iterate over the temporary vector of valid regressions
    while (it_next != valid_regressions_tmp.end())
    {
      if (std::abs(std::get<1>(*it) - std::get<1>(*it_next)) > 2 * scale + 1) // difference between two peaks is larger than the window size (2*scale+1)
      { // create a new group
        if (groups.empty())
        {
          groups.push_back(std::vector<int>{std::get<0>(*it)});
          groups.push_back(std::vector<int>{std::get<0>(*it_next)});
        }
        else
        {
          groups.push_back(std::vector<int>{std::get<0>(*it_next)});
        }
      }
      else
      { // add to the current group
        if (groups.empty())
        {
          groups.push_back(std::vector<int>{std::get<0>(*it)});
          groups.back().push_back(std::get<0>(*it_next));
        }
        else
        {
          groups.back().push_back(std::get<0>(*it_next));
        }
      }
      ++it_next;
      ++it;
    } // end for loop

    // at this point, the groups within a scale are formed; now survival of the fittest peak in each group is performed
    for (auto &group : groups)
    {
      if (group.size() == 1)
      { // already isolated peak => push to valid regressions
        validRegressions.push_back(std::make_unique<validRegression>(group[0], scale, B(1, group[0]) > 0 ? -B(1, group[0]) / 2 / B(3, group[0]) + scale + group[0] : -B(1, group[0]) / 2 / B(2, group[0]) + scale + group[0], 0, B.col(group[0])));
      }
      else
      { // survival of the fittest based on mse between original data and reconstructed (exp transform of regression)
        double best_mse = std::numeric_limits<double>::infinity();
        int best_idx;
        for (int index : group)
        {
          double mse = calcMse((X * B.col(index)).exp(), (Ylog.subMatrix(index, index + 2 * scale + 1, 0, 1)).exp());
          if (mse < best_mse)
          {
            best_mse = mse;
            best_idx = index;
          }
        } // end for loop (indices in group)
        validRegressions.push_back(std::make_unique<validRegression>(best_idx, scale, B(1, best_idx) > 0 ? -B(1, best_idx) / 2 / B(3, best_idx) + scale + best_idx : -B(1, best_idx) / 2 / B(2, best_idx) + scale + best_idx, best_mse, B.col(best_idx)));
      } // end if; single item or group with multiple members
    }   // end for loop (group in vector of groups)
  }     // end validateRegressions

  void qPeaks::mergeRegressionsOverScales(std::vector<std::unique_ptr<validRegression>> &validRegressions, Matrix &Ylog)
  {
    if (validRegressions.empty())
    {
      return; // no valid peaks at all
    }

    int lowestScale = validRegressions.front()->scale;
    int highestScale = validRegressions.back()->scale;
    if (lowestScale == highestScale)
    {
      return; // only one scale; validRegressions is already fine.
    }

    // apply survival of the fittest considering different scales
    // iterate over the validRegressions vector
    for (auto it = validRegressions.begin(); it != validRegressions.end(); ++it)
    {
      double left_limit = (*it)->index;
      double right_limit = (*it)->index + 2 * (*it)->scale + 1;
      double mean_mse = 0;
      int grpSize = 0;
      // iterate over the validRegressions vector till the current regression's scale
      for (auto it2 = validRegressions.begin(); (*it2)->scale < (*it)->scale; ++it2)
      {
        if (!(*it2)->isValid)
          continue; // skip the invalid peaks
        if ((*it2)->apex_position > left_limit && (*it2)->index < right_limit)
        {
          if ((*it2)->mse == 0)
          { // calculate the mse of the current peak
            (*it2)->mse = calcMse(((*designMatrices[(*it2)->scale - 2]) * (*it2)->B).exp(), (Ylog.subMatrix((*it2)->index, (*it2)->index + 2 * (*it2)->scale + 1, 0, 1)).exp());
          }
          grpSize++;
          mean_mse += (*it2)->mse;
        }
      } // end for loop, inner loop, it2

      if (grpSize > 0)
      {
        mean_mse /= grpSize;
      }
      else
      {
        continue; // no peaks in the group
      }

      if ((*it)->mse == 0.0)
      { // calculate the mse of the current peak
        (*it)->mse = calcMse(((*designMatrices[(*it)->scale - 2]) * (*it)->B).exp(), (Ylog.subMatrix((*it)->index, (*it)->index + 2 * (*it)->scale + 1, 0, 1)).exp());
      }

      if ((*it)->mse < mean_mse || grpSize == 1)
      {
        // Set isValid to false for the candidates
        for (auto it2 = validRegressions.begin(); (*it2)->scale < (*it)->scale; ++it2)
        {
          if (!(*it2)->isValid)
            continue; // skip the invalid peaks
          if ((*it2)->apex_position > left_limit && (*it2)->index < right_limit)
          {
            (*it2)->isValid = false;
          }
        }
      }
      else
      {
        (*it)->isValid = false;
      }
    } // end for loop, outer loop, it

    // Remove the peaks with isValid == false from the validRegressions
    validRegressions.erase(std::remove_if(validRegressions.begin(), validRegressions.end(),
                                          [](const auto &peak)
                                          { return !peak->isValid; }),
                           validRegressions.end());
  }

  std::vector<std::unique_ptr<DataType::Peak>> qPeaks::createPeaks(const std::vector<std::unique_ptr<validRegression>> &validRegressions, const RefMatrix &Y, const std::vector<double> &X, const int scanNumber)
  {
    std::vector<std::unique_ptr<DataType::Peak>> peaks; // peak list
    // iterate over the validRegressions vector
    for (auto &regression : validRegressions)
    {
      // re-scale the apex position to x-axis
      double x0 = X[(int) std::floor(regression->apex_position)];
      double dx = X[(int) std::ceil(regression->apex_position)] - x0;
      double apex_position = x0 + dx * (regression->apex_position - std::floor(regression->apex_position));
      // create a new peak object and push it to the peaks vector; the peak object is created using the scan number, the apex position and the peak height
      peaks.push_back(std::make_unique<DataType::Peak>(scanNumber, apex_position, 
      (regression->B(1, 0) > 0) 
      ? std::exp(regression->B(0, 0) - regression->B(1, 0) * regression->B(1,0) / 4 / regression->B(3, 0)) 
      : std::exp(regression->B(0, 0) - regression->B(1, 0) * regression->B(1,0) / 4 / regression->B(2, 0))));

      // debugging
      // get the coefficients matrix
      Matrix B = regression->B;
      
      // get the index of the regression
      int index = regression->index;
      // get scale
      int scale = regression->scale;

      // design matrix
      Matrix Xdesign = *(designMatrices[scale - 2]);
      // calculate yfit
      Matrix yfit_matrix = (Xdesign * B).exp();
      std::vector<double> yfit;
      for (int i = 0; i < yfit_matrix.numRows(); i++)
      {
        yfit.push_back(yfit_matrix(i, 0));
      }
      // extract xfit, i.e., from X vector index to index + 2*scale+1
      std::vector<double> xfit;
      for (int i = index; i < index + 2 * scale + 1; i++)
      {
        xfit.push_back(X[i]);
      }
      peaks.back()->xFit = xfit;
      peaks.back()->yFit = yfit;
    } // end for loop

    return peaks;
  }

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
  }

  std::pair<Matrix, double> qPeaks::jacobianMatrix_PeakArea(const Matrix &B, int scale) const
  {
    // predefine expressions
    const double SQRTB2 = std::sqrt(std::abs(-B(2, 0)));
    const double SQRTB3 = std::sqrt(std::abs(-B(3, 0)));
    const double EXP_B0 = std::exp(B(0, 0));
    const double B1_2_B2 = B(1, 0) / 2 / B(2, 0);
    const double EXP_B12 = std::exp(-B(1, 0) * B1_2_B2 / 2);
    const double B1_2_B3 = B(1, 0) / 2 / B(3, 0);
    const double EXP_B13 = std::exp(-B(1, 0) * B1_2_B3 / 2);
    // here we have to check if there is a valley point or not
    const double err_L = (B(2, 0) < 0)
                             ? std::erf(B(1, 0) / 2 / SQRTB2) + 1 // ordinary peak
                             : erfi(B(1, 0) / 2 / SQRTB2);         // peak with valley point; 

    const double err_R = (B(3, 0) < 0)
                             ? std::erf(B(1, 0) / 2 / SQRTB3) + 1 // ordinary peak
                             : -erfi(B(1, 0) / 2 / SQRTB3);     // peak with valley point ;

    const double err_L_covered = (B(2,0) < 0)
      ? // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
        std::erf(B(1, 0) / 2 / SQRTB2) - std::erf(B(1, 0) / 2 / SQRTB2 - scale * SQRTB2) 
      : // valley point, i.e., check position
        (-B1_2_B2 < -scale) 
        ? // valley point is outside the window, use scale as limit
          erfi(B(1, 0) / 2 / SQRTB2) - erfi(B(1, 0) / 2 / SQRTB2 - scale * SQRTB2)
        : // valley point is inside the window, use valley point as limit
          erfi(B(1, 0) / 2 / SQRTB2);
    
    const double err_R_covered = (B(3,0) < 0)
      ? // ordinary peak half, take always scale as integration limit; we use erf instead of erfi due to the sqrt of absoulte value
        std::erf(B(1, 0) / 2 / SQRTB3) - std::erf(B(1, 0) / 2 / SQRTB3 + scale * SQRTB3) 
      : // valley point, i.e., check position
        (-B1_2_B3 > scale) 
        ? // valley point is outside the window, use scale as limit
          -erfi(B(1, 0) / 2 / SQRTB3) + erfi(B(1, 0) / 2 / SQRTB3 + scale * SQRTB3)
        : // valley point is inside the window, use valley point as limit
          -erfi(B(1, 0) / 2 / SQRTB3);
    // calculate the Jacobian matrix terms
    Matrix J(1, 4);
    J(0, 0) = SQRTPI_2 * ((EXP_B0 * EXP_B12) / SQRTB2 * err_L + (EXP_B0 * EXP_B13) / SQRTB3 * err_R);
    J(0, 1) = (EXP_B0 / 2) * (1 / B(2, 0) + 1 / B(3, 0)) - J(0, 0) * (B1_2_B2 + B1_2_B3);
    J(0, 2) = -J(0, 0) / 2 / B(2, 0) - B1_2_B2 * J(0, 1);
    J(0, 3) = -J(0, 0) / 2 / B(3, 0) - B1_2_B3 * J(0, 1);

    // calculate the trapezoid under the peak covered by data points
    const double x_left = (B(2, 0) < 0)
      ? // ordinary peak half, take always scale as integration limit
        -scale
      : // valley point, i.e., check position
        (-B1_2_B2 < -scale)
        ? // valley point is outside the window, use scale as limit
          -scale
        : // valley point is inside the window, use valley point as limit
          -B1_2_B2;
    
    const double x_right = (B(3, 0) < 0)
      ? // ordinary peak half, take always scale as integration limit
        scale
      : // valley point, i.e., check position
        (-B1_2_B3 > scale)
        ? // valley point is outside the window, use scale as limit
          scale
        : // valley point is inside the window, use valley point as limit
          -B1_2_B3;

    const double y_left = std::exp(B(0, 0) + B(1, 0) * x_left + B(2, 0) * x_left * x_left);
    const double y_right = std::exp(B(0, 0) + B(1, 0) * x_right + B(3, 0) * x_right * x_right);
    const double ymin = std::min(y_left, y_right); // @todo: this changes trapzoid to rectangle
    const double peak_area_covered_trapezoid = (y_right + y_left) * (x_right - x_left) / 2;

    // calculate the the peak area covered by data points
    const double peak_area_covered = SQRTPI_2 * ((EXP_B0 * EXP_B12) / SQRTB2 * err_L_covered + (EXP_B0 * EXP_B13) / SQRTB3 * err_R_covered) - peak_area_covered_trapezoid;
    
    return std::pair<Matrix, double>(J, (J(0,0) - peak_area_covered) / (int) (x_right - x_left-3));
  }

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

  void qPeaks::info() const
  {
    std::cout << "qPeaks object\n";
    std::cout << "designMatrices size: " << designMatrices.size() << "\n";
    std::cout << "inverseMatrices size: " << inverseMatrices.size() << "\n";
    std::cout << "psuedoInverses size: " << psuedoInverses.size() << "\n";
  }

  void qPeaks::printMatrices(int scale) const
  {
    std::cout << "Design Matrix\n";
    designMatrices[scale - 2]->print();
    std::cout << "Inverse Matrix\n";
    inverseMatrices[scale - 2]->print();
    std::cout << "Pseudo-Inverse Matrix\n";
    psuedoInverses[scale - 2]->print();
  }
} // namespace q
