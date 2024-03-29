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

    // perform the running regression
    int maxScale = std::min(this->global_maxScale, (int)(n - 1) / 2);
    for (int scale = 2; scale <= maxScale; scale++)
    {
      Matrix b = Ylog.convoleCombiend(*(psuedoInverses[scale - 2]));
      validateRegressions(b, Ylog, scale);
    } // end for scale loop
  }   // end runningRegression

  void qPeaks::validateRegressions(const Matrix &B, const Matrix &Ylog, const int scale)
  {
    // declare variables
    int valley_position;
    int apex_position;
    double inverseMatrix_2_2 = (*(inverseMatrices[scale - 2]))(2, 2);
    double inverseMatrix_3_3 = (*(inverseMatrices[scale - 2]))(3, 3);
    Matrix &X = *(designMatrices[scale - 2]);
    // coefficients filter
    // iterate columwise over the coefficients matrix
    for (size_t i = 0; i < B.numCols(); i++)
    {
      // calculate the checksum for case differentiation
      int key = (B(1, i) < 0 ? 4 : 0) + (B(2, i) < 0 ? 2 : 0) + (B(3, i) < 0 ? 1 : 0);
      switch (key)
      {
      case 7: // Case 1a: apex left
        if (-B(1, i) / 2 / B(2, i) <= -scale)
        {
          continue; // invalid apex position
        }
        break;
      case 3: // Case 1b: apex right
        if (-B(1, i) / 2 / B(3, i) >= scale)
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
      double mse = calcMse(X * B.col(i), Ylog);
      double tValue = std::max(std::abs(B(2, i)) * std::sqrt(inverseMatrix_2_2 * mse), std::abs(B(3, i)) * std::sqrt(inverseMatrix_3_3 * mse));
      if (tValue < tValuesArray[scale * 2 - 4]) // df is scale*2-3 but tValuesArray is zero-based
      {
        continue; // statistical insignificance of the quadratic term
      }
    } // end for loop
  }   // end validateRegressions

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
