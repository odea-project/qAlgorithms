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

  void qPeaks::findPeaks(const varDataType &dataVec)
  {
    std::visit([this](auto &&arg)
               {
    // iterate over the map of varDataType datatype objects
    // use parallel for loop to iterate over the dataVec
#pragma omp parallel for
                for (auto &pair : *arg)
                 {
                  // de-reference the unique pointer of the object
                  auto &dataObj = *(pair.get());
                  auto &data = dataObj.dataPoints;
                  int n = data.size();
                  // store x and y values in RefMatrix objects
                  RefMatrix X(n, 1);
                  RefMatrix Y(n, 1);
                  for (size_t i = 0; i < n; i++)
                  {
                    X.assignRef(i, 0, data[i]->x());
                    Y.assignRef(i, 0, data[i]->y());
                  }
                  runningRegression(X, Y);
                } },
               dataVec);
  } // end findPeaks

  void qPeaks::runningRegression(const RefMatrix &X, const RefMatrix &Y)
  { 
    // perform log-transform on Y
    size_t n = Y.getRows();
    Matrix Ylog = Y.log();

    int maxScale = std::min(this->global_maxScale, (int)(n - 1) / 2);
    std::vector<std::unique_ptr<validRegression>> validRegressions; // index in B, scale, apex_position and MSE
    validRegressions.reserve(calculateNumberOfRegressions(n));
    for (int scale = 2; scale <= maxScale; scale++)
    {
      // coeffiencts matrix B of the running regression
      Matrix b = Ylog.convoleCombiend(*(psuedoInverses[scale - 2]));
      validateRegressions(b, Ylog, scale, validRegressions);
    } // end for scale loop
    if (validRegressions.empty())
    {
      return; // no valid peaks at all
    }
    int lowestScale = validRegressions.front()->scale;
    int highestScale = validRegressions.back()->scale;
    if (lowestScale == highestScale)
    {
      return; // only one scale @todo: return the results
    }
    // apply survival of the fittest considering different scales
    std::vector<std::vector<std::unique_ptr<validRegression>>::iterator> best_peak_iterators; // iterators to the best peaks
    std::vector<std::vector<std::unique_ptr<validRegression>>::iterator> candidate_peak_iterators; // iterators to the candidate peaks
    auto it = validRegressions.begin();
    best_peak_iterators.push_back(it);
    // iterate over the validRegressions vector
    for (auto peak = std::next(it); peak != validRegressions.end(); ++peak)
    {
      double left_limit = (*peak)->index - (*peak)->scale;
      double right_limit = (*peak)->index + (*peak)->scale;
      // iterate over the best peaks
      for (auto best_peak = best_peak_iterators.begin(); best_peak != best_peak_iterators.end(); ++best_peak)
      {
        if ((**best_peak)->apex_position > left_limit && (**best_peak)->apex_position < right_limit)
        {
          candidate_peak_iterators.push_back(*best_peak);
        }
      }
      // compare mse of best and mean mse of candidates
      if ((*peak)->mse == 0)
      { // calculate the mse of the current peak
        
        (*peak)->mse = calcMse(((*designMatrices[(*peak)->scale-2]) * (*peak)->B).exp(), (Ylog.subMatrix((*peak)->index, (*peak)->index + 2 * (*peak)->scale + 1, 0, 1)).exp());
      }
      candidate_peak_iterators.clear();
    }

    // auto beginRange = validRegressions.end();
    // auto endRange = validRegressions.end();
    // // iterate over the validRegressions vector
    // for (auto it = validRegressions.begin(); it != validRegressions.end();)
    // {
    //   if ((*it)->scale > lowestScale)
    //   {
    //     // iterate forward from the beginning of the vector to first elemement that has equal scale to current one.
    //     auto it_again = validRegressions.begin();
    //     int currentScale = (*it)->scale;
    //     while ((*it_again)->scale != currentScale)
    //     {
    //       // @todo: add some code to set the beginRange and endRange iterators
    //       ++it_again;
    //     }
    //     ++it;
    //   }
    //   else
    //   {
    //     ++it;
    //   }
    // }
    
    

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
    std::vector<std::pair<int, double>> valid_regressions_tmp; // temporary vector to store valid regressions <index, apex_position>

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
        if (apex_position <= -scale)
        {
          continue; // invalid apex position
        }
        break;
      case 3:                                   // Case 1b: apex right
        apex_position = -B(1, i) / 2 / B(3, i); // is positive
        if (apex_position >= scale)
        {
          continue; // invalid apex position
        }
        break;
      case 6:                                     // Case 2a: apex left | valley right
        apex_position = -B(1, i) / 2 / B(2, i);   // is negative
        valley_position = -B(1, i) / 2 / B(3, i); // is positive
        if (apex_position <= -scale || valley_position - apex_position <= 2)
        {
          continue; // invalid apex and valley positions
        }
        break;
      case 1:                                     // Case 2b: apex right | valley left
        apex_position = -B(1, i) / 2 / B(3, i);   // is positive
        valley_position = -B(1, i) / 2 / B(2, i); // is negative
        if (apex_position >= scale || apex_position - valley_position <= 2)
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
      std::pair<Matrix, double> Jacobian_area = jacobianMatrix_PeakArea(B.col(i), scale);
      double area_uncertainty = std::sqrt((Jacobian_area.first * C * Jacobian_area.first.T()).sumElements()) + (Jacobian_area.first(0, 0) - Jacobian_area.second); // uncertainty of the peak area based on the Jacobian matrix and the non-covered peak area
      if (Jacobian_area.first(0, 0) / area_uncertainty < tValuesArray[scale * 2 - 4])
      {
        continue; // statistical insignificance of the peak area
      }

      // at this point, the peak is validated
      valid_regressions_tmp.push_back(std::make_pair(i, apex_position+i +scale));
    } // end for loop

    // early return if no or only one valid peak
    if (valid_regressions_tmp.size() < 2)
    {
      if (valid_regressions_tmp.empty())
      {
        return; // no valid peaks
      }
      validRegressions.push_back(std::make_unique<validRegression>(valid_regressions_tmp[0].first, scale, valid_regressions_tmp[0].second, 0, B.col(valid_regressions_tmp[0].first)));
      return; // not enough peaks to form a group
    }

    // group the valid peaks
    std::vector<std::vector<int>> groups;
    // initialize iterators for valid_regressions_tmp
    auto it = valid_regressions_tmp.begin();
    auto it_next = std::next(it);
    // iterate over the temporary vector of valid regressions
    while (it_next != valid_regressions_tmp.end())
    {
      if (std::abs(it->second - it_next->second) >= 2 * scale + 1)
      { // create a new group
        if (groups.empty())
        {
          groups.push_back(std::vector<int>{it->first});
          groups.push_back(std::vector<int>{it_next->first});
        }
        else
        {
          groups.push_back(std::vector<int>{it_next->first});
        }
      }
      else
      { // add to the current group
        if (groups.empty())
        {
          groups.push_back(std::vector<int>{it->first});
          groups.back().push_back(it_next->first);
        }
        else
        {
          groups.back().push_back(it_next->first);
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
        validRegressions.push_back(std::make_unique<validRegression>(group[0], scale, B(1,group[0]) > 0 ? -B(1,group[0]) / 2 / B(3,group[0]) + scale + group[0] : -B(1,group[0]) / 2 / B(2,group[0]) + scale + group[0], 0, B.col(group[0])));
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
        validRegressions.push_back(std::make_unique<validRegression>(best_idx, scale, B(1,best_idx) > 0 ? -B(1,best_idx) / 2 / B(3,best_idx) + scale + best_idx : -B(1,best_idx) / 2 / B(2,best_idx) + scale + best_idx, best_mse, B.col(best_idx)));
      } // end if; single item or group with multiple members
    }   // end for loop (group in vector of groups)
  }     // end validateRegressions

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
                             ? std::erf(-B(1, 0) / 2 / SQRTB2) + 1 // ordinary peak
                             : erfi(B(1, 0) / 2 / SQRTB2);         // peak with valley point

    const double err_R = (B(3, 0) < 0)
                             ? std::erfc(-B(1, 0) / 2 / SQRTB3) // ordinary peak
                             : -erfi(B(1, 0) / 2 / SQRTB3);     // peak with valley point

    const double err_L_covered = (-B1_2_B2 < -scale)
                                     ? 0                                             // erfi(B(1, 0) / 2 / SQRTB2)
                                     : -erfi(B(1, 0) / 2 / SQRTB2 + scale * SQRTB2); // + erfi(B(1, 0) / 2 / SQRTB2)

    const double err_R_covered = (-B1_2_B3 > scale)
                                     ? 0                                           //- erfi(B(1, 0) / 2 / SQRTB3)
                                     : erf(B(1, 0) / 2 / SQRTB3 + scale * SQRTB3); //- erfi(B(1, 0) / 2 / SQRTB3);

    // calculate the Jacobian matrix terms
    Matrix J(1, 4);
    J(0, 0) = SQRTPI_2 * ((EXP_B0 * EXP_B12) / SQRTB2 * err_L + (EXP_B0 * EXP_B13) / SQRTB3 * err_R);
    J(0, 1) = (EXP_B0 / 2) * (1 / B(2, 0) + 1 / B(3, 0)) - J(0, 0) * (B1_2_B2 + B1_2_B3);
    J(0, 2) = -J(0, 0) / 2 / B(2, 0) - B1_2_B2 * J(0, 1);
    J(0, 3) = -J(0, 0) / 2 / B(3, 0) - B1_2_B3 * J(0, 1);

    // calculate the the peak area covered by data points
    const double peak_area_covered = SQRTPI_2 * ((EXP_B0 * EXP_B12) / SQRTB2 * (err_L + err_L_covered) + (EXP_B0 * EXP_B13) / SQRTB3 * (err_R + err_R_covered));

    return std::pair<Matrix, double>(J, peak_area_covered);
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