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
                // get the largest number of data points in a single data set within the dataVec
                size_t maxDataPoints = 0;
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
                 // set the maxScale to floor of (maxDataPoints-1)/2
                 int maxScale = (int) (maxDataPoints-1)/2;
                  // iterate over the range of the maxScale to create Matrices for each scale
                  for (int scale = 2; scale <= maxScale; scale++)
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
                  // initialize the network structure of peaks
                  // std::unordered_map<int, std::unique_ptr<Peak>> peaklist;

                  /* THIS IS TOO SLOW TO ADD ALL REGRESSIONS
                  // add Peak objects to the peaklist
                  int currentScale = 2;
                  int currentScaleIndexLimit = currentScale * (n - currentScale - 1) - n + 2;
                  for (int i = 0; i < numRegressions; i++)
                  {
                    if (i == currentScaleIndexLimit)
                    {
                      currentScale++;
                      currentScaleIndexLimit += n - currentScale * 2;
                    }
                    peaklist.emplace(i, std::make_unique<Peak>(i, currentScale));
                  }
                  */
                  runningRegression(X, Y);
                } },
               dataVec);
  } // end findPeaks

  void qPeaks::runningRegression(const RefMatrix &X, const RefMatrix &Y)
  {
    // perform log-transform on Y
    size_t n = Y.getRows();
    Matrix Ylog = Y.log();

    int numRegressions = calculateNumberOfRegressions(n);
    // reserve space for regression coefficients matrix, which is a matrix of size 4 x numRegressions
    Matrix B(4, numRegressions);
    // add the organization structure for filtering and iterating over the regressions
    std::vector<int> regression_indices;
    regression_indices.reserve(numRegressions);
    for (int i = 0; i < numRegressions; i++)
    {
      regression_indices.push_back(i);
    }

    // initialize the regression support vectors
    std::vector<int> xIndices;
    xIndices.reserve(n);

    // perform the running regression
    int maxScale = (int) (n-1)/2;
    for (int scale = 2; scale <= maxScale; scale++)
    {
      convolveP(B, xIndices, *(psuedoInverses[scale-2]), Ylog);
    } // end for scale loop
    

  } // end runningRegression

  // Convolution
  void qPeaks::convolveP(
    Matrix& beta, 
    std::vector<int>& xIndices, 
    const Matrix& P, 
    const Matrix& ylog) {
      int currentIndex = 0;
      size_t n = ylog.numRows();
      size_t k = P.numCols();
      size_t n_segments = n - k + 1;

      Matrix p1 = ylog.convolveSymmetric(P.row(0));
      Matrix p2 = ylog.convolveRotation(P.row(1));
      Matrix p34 = ylog.convolveAntisymmetric(P.row(2));
      int u = 0;
      for (size_t j = 0; j < n_segments; j++)
      {
        beta(0,j + currentIndex) = p1(0, j);
        beta(1,j + currentIndex) = p2(0, j); 
        beta(2,j + currentIndex) = p34(0,j); 
        beta(3,j + currentIndex) = p34(1,j); 
        xIndices[j + currentIndex] = u + k/2;
        u++;
      } 
      currentIndex += u;
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
    int maxScale = (int) (n - 1) / 2;
    int sumScales = (int) (maxScale * (maxScale + 1) / 2) - 1;
    return n * (maxScale-1) - sumScales*2;
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
