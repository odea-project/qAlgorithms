// qalgorithms.cpp

#include "../include/qalgorithms.h"
#include "../include/qalgorithms_utils.h"
#include "../include/qalgorithms_matrix.h"

namespace q {

  // Constructor
  Peakproperties::Peakproperties(){}

  Peakproperties::Peakproperties(
      const double _coeff_b0,
      const double _coeff_b1,
      const double _coeff_b2,
      const double _coeff_b3,
      const double _peakID,
      const double _smplID,
      const double _position,
      const double _height,
      const double _width,
      const double _area,
      const double _sigmaPosition,
      const double _sigmaHeight,
      const double _sigmaWidth,
      const double _sigmaArea,
      const double _dqs)
  : coeff_b0(_coeff_b0),
    coeff_b1(_coeff_b1),
    coeff_b2(_coeff_b2),
    coeff_b3(_coeff_b3),
    peakID(_peakID),
    smplID(_smplID),
    position(_position),
    height(_height),
    width(_width),
    area(_area),
    sigmaPosition(_sigmaPosition),
    sigmaHeight(_sigmaHeight),
    sigmaWidth(_sigmaWidth),
    sigmaArea(_sigmaArea),
    dqs(_dqs)
    {}
  
  // debuging & printing
    double Peakproperties::getProperty(Peakproperties::PropertiesNames varName) const {
      switch (varName)
      {
      case COEFF_B0: return coeff_b0;
      case COEFF_B1: return coeff_b1;
      case COEFF_B2: return coeff_b2;
      case COEFF_B3: return coeff_b3;
      case PEAKID:   return peakID;
      case SMPLID:   return smplID;
      case POSITION: return position;
      case HEIGHT:   return height;
      case WIDTH:    return width;
      case AREA:     return area;
      case SIGMAPOSITION: return sigmaPosition;
      case SIGMAHEIGHT:   return sigmaHeight;
      case SIGMAWIDTH:    return sigmaWidth;
      case SIGMAAREA:     return sigmaArea;
      case DQS:      return dqs;
      default:
        break;
      }
      return 0.0;
    }

  void Peakproperties::print() const {
    std::cout << "Peak Properties:" << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << "Peak ID: " << peakID << std::endl;
    std::cout << "Sample ID: " << smplID << std::endl;
    std::cout << "Position: " << position << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Area: " << area << std::endl;
    std::cout << "Position Uncertainty (sigma): " << sigmaPosition << std::endl;
    std::cout << "Height Uncertainty (sigma): " << sigmaHeight << std::endl;
    std::cout << "Width Uncertainty (sigma): " << sigmaWidth << std::endl;
    std::cout << "Area Uncertainty (sigma): " << sigmaArea << std::endl;
    std::cout << "DQS: " << dqs << std::endl;
  }

  // Constructor
  Peakmodel::Peakmodel() {}

  // Constructor to initialize Peak model based on an upper limit for df
  Peakmodel::Peakmodel(int a_max) {
    for (int a = 2; a <= a_max; a++)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    } 
  }

  // Constructor to initialize Peak model based on a lower and an upper limit for df
  Peakmodel::Peakmodel(int a_min, int a_max) {
    for (int a = a_min; a <= a_max; a++)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    } 
  }

  // Constructor to initialize Peak model based on a vector for a
  Peakmodel::Peakmodel(std::vector<int> A) {
    for (const auto &a : A)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    }
  }

  // Load t-Values
  void Peakmodel::loadTValues(const tValues& tData) {
    for (size_t i = 1; i < 501; i++) {
      tVal[i] = tData[i];
    }
  }

  // Function to calculate the Designmatrix for a given a
  void Peakmodel::calculateDesignMatrix(int a) { 
    // construct matrix
    size_t m = 4;
    size_t n = 2*a + 1;
    Matrix X(n,m);

    // x vector
    std::vector<int> x(n);
    for (size_t i = 0; i < n; i++)
    {x[i] = i - a;}
    
    // order vector
    std::vector<int> p = {0, 1, 2, 2};
    
    for (size_t i = 0; i < n; i++)
    {
      for (size_t j = 0; j < m; j++)
      {
        X(i, j) = std::pow(x[i],p[j]);    
      }    
    }

    // modification of X due to p(2) and p(3)
    for (size_t i = 0; i < n; i++)
    {
      if (x[i] < 0)
        {X(i,3) = 0;} 
      else 
        {X(i,2) = 0;}
    }    
    designMatrices.emplace(a, X);
  }

  // Function to calculate the Designmatrix for a given a
  void Peakmodel::calculatePseudoInverse(int a) {
    Matrix X = getDesignMatrix(a);
    Matrix Xt = X.T();
    Matrix XtX = Xt*X;
    inverseMatrices.emplace(a, XtX.inv());
    Matrix P = inverseMatrices[a] * Xt;
    pseudoInverses.emplace(a, P);
  }

  std::vector<int> Peakmodel::getScales() const {
    std::vector<int> scales;
    for (const auto& pair : designMatrices) {
      scales.push_back(pair.first);
    }
    return scales;
  }

  std::vector<int> Peakmodel::getScaleVec(
    const std::vector<int>& scales, 
    const int n, 
    const int N) const {
      std::vector<int> scaleVec(N);
      int u = 0;
      for (const int k : scales) {
        size_t jMax = n - 2*k;
        for (size_t j = 0; j < jMax; j++) {
          scaleVec[u] = k;
          u++;
        }
      }
      return scaleVec;
  }

  // Function to get the Designmatrix for a given a
  Matrix Peakmodel::getDesignMatrix(int a) const {
    auto it = designMatrices.find(a);
    if (it != designMatrices.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Key not found in designMatrices map");
    }
  }

  // Function to get the Pseudo Inverse for a given a
  Matrix Peakmodel::getPseudoInverse(int a) const {
    auto it = pseudoInverses.find(a);
    if (it != pseudoInverses.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Key not found in pseudoInverses map");
    }
  }

  // Function to add some Measurement Data to the Peak Model
  void Peakmodel::addSingleMeasurement(Matrix& xyData, bool zero_interpolation) {
    int maxKey = measurementData.empty() ? -1 : std::prev(measurementData.end())->first;
    int newKey = maxKey + 1;

    // check if sorted by column 1 and sort if unsorted
    xyData.sort1();
    if (zero_interpolation) { // default is true
      zeroInterpolation(xyData);
    }
    measurementData[newKey] = xyData;
  }

  void Peakmodel::addMultipleMeasurements(Matrix& xyData, bool zero_interpolation) {
    xyData.sort1(); // Sort the entire matrix by the first column

    // analyze number of groups and members
    std::map<int, int> groupCount;
    size_t n = xyData.numRows();
    size_t m = xyData.numCols();
    for (size_t i = 0; i < n; ++i) {
        int groupNum = static_cast<int>(xyData(i, 2));
        groupCount[groupNum]++;
    }

    // initialize data matrices
    for (const auto& [groupNum, rowCount] : groupCount) {
        measurementData.emplace(groupNum, Matrix(rowCount,m));
    }

    // fill data matrices
    std::map<int, size_t> currentRowCount; 
    for (size_t i = 0; i < n; ++i) {
        int groupNum = static_cast<int>(xyData(i, 2));
        size_t rowIdx = currentRowCount[groupNum]++;
        for (size_t j = 0; j < m; j++) {
          measurementData[groupNum](rowIdx,j) = xyData(i,j);
        }
    }

    // zero-interpolation
    if (zero_interpolation) {
      for (auto& pair : measurementData) {
        zeroInterpolation(pair.second);
      }
    }
  }

  void Peakmodel::zeroInterpolation(Matrix& xyData) {
    Matrix left_to_right = xyData;
    zeroInterpolation_oneDirection(left_to_right);

    // Flipping the matrix upside down
    Matrix right_to_left = xyData;
    for (size_t i = 0; i < right_to_left.numRows() / 2; i++) {
        for (size_t j = 0; j < right_to_left.numCols(); j++) {
            std::swap(right_to_left(i,j), right_to_left(right_to_left.numRows()-1-i, j));
        }
    }
    zeroInterpolation_oneDirection(right_to_left);
    // Flipping back
    for (size_t i = 0; i < right_to_left.numRows() / 2; i++) {
        for (size_t j = 0; j < right_to_left.numCols(); j++) {
            std::swap(right_to_left(i,j), right_to_left(right_to_left.numRows()-1-i, j));
        }
    }
    // Combining the results
    for (size_t i = 0; i < xyData.numRows(); i++) {
        xyData(i, 1) = std::max(left_to_right(i, 1), right_to_left(i, 1));
    }

  }

  void Peakmodel::zeroInterpolation_oneDirection(Matrix& xyData) {
    /* This function corrects zeros by interpolation considering the direct neighbors*/
    size_t n = xyData.numRows();
    // Checking for fronting zeros
    size_t i = 0;
    while (xyData(i, 1) == 0.)
    {
      i++;
      if (i ==n) {
        return;
      }
    }
    if (i > 0) {
      // There are fronting zeros, which will be corrected
      for (int j = i-1; j >= 0; --j) {
        xyData(j, 1) = xyData(j+1,1) / 2.;
      }
    }

    // Checking the rest of the data (except last data point)
    for (; i < n-1; i++) {
      if (xyData(i,1) == 0.) {
        xyData(i,1) = (xyData(i-1,1) + xyData(i+1,1)) / 2.;
      }
    }
    
    // Checking the last data point
    if (xyData(n-1,1) == 0.) {
      xyData(n-1,1) = xyData(n-2,1) / 2.;
    }
  }

  Matrix Peakmodel::getData(int idx) const {
    auto it = measurementData.find(idx);
        if (it != measurementData.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Key not found in measurementData map");
        }
  }

  // Run Regression
  void Peakmodel::runRegression() {
    // get list of scales
    std::vector<int> scales = getScales();
    // pre-calculate number of regressions: N = (scales.size()*n - 2*sum(scales)) 
    size_t k = scales.size(); // number of scales
    int s = 2*sum(scales); // 2*sum(scales)
    // initialize peakID
    int peakID = 0;

    // iterate through the whole dataset
    for (const auto& pair: measurementData)
    {
      // get key and data
      int key = pair.first;
      Matrix data = pair.second;

      // log transform
      Matrix ylog = data.col(1).log(); // 1, i.e., first column (x) will be ignored

      // preallocate conv Matrix that later will contain all regression coefficients
      size_t n = ylog.numRows();
      size_t N = k*n - s;
      Matrix beta(4,N);
      bool* fltrVec = new bool[N];
      std::fill_n(fltrVec, N, true);

      

      std::vector<int> xIndices(N);
      std::vector<double> apex_positions(N);
      std::vector<double> apex_positions_uncertainty(N);
      std::vector<double> valley_positions(N);
      std::vector<double> peakHeight(N);
      std::vector<double> peakHeight_uncertainty(N);
      std::vector<double> peakArea(N);
      std::vector<double> peakArea_uncertainty(N);
      std::vector<int> scaleVec = getScaleVec(scales, n, N);
      std::vector<double> mse(N);
      int currentIndex = 0;
      // iterate through all scales
      for (const auto& p : pseudoInverses) {
        convolveP(beta, xIndices, p.second, ylog, currentIndex);
      }
      std::cout << "("<< key << "): " << sum(fltrVec,N) << " --> ";

      // Filtering Regression Results
      // Vector of Indices that have passed the last filter
      std::vector<int> idx1;
      coefficientCriterion(N, fltrVec, scaleVec, apex_positions, valley_positions, xIndices, beta, idx1);
      std::cout << idx1.size() << " --> ";

      std::vector<int> idx2;
      quadraticTermCriterion(N, fltrVec, scaleVec, xIndices, beta,  ylog, mse, idx1, idx2);
      std::cout << idx2.size() << " --> ";

      idx1.clear();
      parameterCriterion(N, fltrVec,peakHeight,peakHeight_uncertainty, peakArea, peakArea_uncertainty,scaleVec, beta, mse, idx2, idx1);
      std::cout << idx1.size() << " --> ";
      
      idx2.clear();
      mergeRegressions(N, fltrVec, beta, scaleVec, xIndices, apex_positions, apex_positions_uncertainty, valley_positions, data.col(1), mse, idx1, idx2);
      std::cout << idx2.size() << std::endl;

      rescalePosition(N,fltrVec,data.col(0),apex_positions,apex_positions_uncertainty,idx2);

      exportResults(beta, fltrVec, xIndices, N, key,apex_positions,apex_positions_uncertainty,peakHeight,peakHeight_uncertainty,peakArea,peakArea_uncertainty,idx2,peakID);
      delete[] fltrVec;
    }
  }

  // Convolution
  void Peakmodel::convolveP(
    Matrix& beta, 
    std::vector<int>& xIndices, 
    const Matrix& P, 
    const Matrix& ylog, 
    int& currentIndex) {
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

  double Peakmodel::calcMse(const Matrix& yhat, const Matrix& y) const{
    size_t n = yhat.numel();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
            double diff = yhat.getElement(i) - y.getElement(i);
            sum += diff * diff;
        }
    return sum / (n - 4);
  }

  // Regression Coefficient Filter Criterion
  void Peakmodel::coefficientCriterion(
    const int N,
    bool*& fltrVec, 
    const std::vector<int>& scaleVec, 
    std::vector<double>& apex_positions, 
    std::vector<double>& valley_positions, 
    const std::vector<int>& xIndices, 
    const Matrix& beta,
    std::vector<int>& idx) {
      // This function extracts peak candidates from beta matrix
      for (int col = 0; col < N; col++) {
        if (fltrVec[col]) {
          coefficientCriterionCases(scaleVec[col], beta(1,col),beta(2,col),beta(3,col), xIndices[col],apex_positions[col],valley_positions[col],fltrVec[col],idx, col);
        }
      }   
  }

  void Peakmodel::coefficientCriterionCases(
    const int scale, 
    const double b1, 
    const double b2, 
    const double b3, 
    const int xIndex, 
    double& apex_position, 
    double& valley_position, 
    bool& fltrVal,
    std::vector<int>& idx,
    const size_t IDX) {
    /*
    This functions checks if a set of regression coefficients (b0, b1, b2, b3) are valid
    and describe an analytical peak after exp-transform.
     # valid peak case 1: b2<0 & b3<0 & apex in window
     # valid peak case 2a: b2>0 & b3<0 & b1>0 & |valley - apex| > 2 & apex in window
     # valud peak case 2b: b2<0 & b3>0 & b1<0 & |valley - apex| > 2 & apex in window
    */
      // creating unique checksum
      int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);
      switch (key) {
          case 7:  // Case 1a: apex left
              calcApex_position(scale, b1, b2, xIndex, apex_position, fltrVal);
              break;
          case 3:  // Case 1b: apex right
              calcApex_position(scale, b1, b3, xIndex, apex_position, fltrVal);
              break;
          case 6:  // Case 2a: apex left | valley right
              calcApexValley_position(scale, b1, b2, b3, xIndex, apex_position, valley_position, fltrVal);
              break;
          case 1:  // Case 2b: apex right | valley left
              calcApexValley_position(scale, b1, b3, b2, xIndex, apex_position, valley_position, fltrVal);
              break;
          default:
              fltrVal = false;
              break;
      }
      if (fltrVal) {idx.push_back(IDX);}
  }

  void Peakmodel::calcApex_position(
    const int scale, 
    const double b1, 
    const double b2, 
    const int xIndex, 
    double& apex_position, 
    bool& fltrVal) {
      apex_position = - b1 / 2 / b2;
      // check if apex position is in window with at least one point buffer
      if (abs(apex_position) < scale) {
        apex_position += xIndex;
        fltrVal = true;
      } else {
        apex_position = 0.;
        fltrVal = false;
      }
  }

  void Peakmodel::calcApexValley_position(
    const int scale, 
    const double b1, 
    const double b2, 
    const double b3, 
    const int xIndex, 
    double& apex_position, 
    double& valley_position, 
    bool& fltrVal) {
      apex_position = - b1 / 2 / b2;
      valley_position = -b1 / 2 / b3;
      // check if apex position is in window with at least one point buffer
      if ((abs(apex_position) < scale) & (abs(apex_position - valley_position) > 2)) {
        apex_position += xIndex;
        valley_position += xIndex;
        fltrVal = true;
      } else {
        apex_position = 0.;
        valley_position = 0.;
        fltrVal = false;
      }
  }

  void Peakmodel::quadraticTermCriterion(
    const int N,
    bool*& fltrVec, 
    const std::vector<int>& scaleVec,
    const std::vector<int>& xIndices, 
    const Matrix& beta, 
    const Matrix& ylog, 
    std::vector<double>& mse,
    const std::vector<int>& idx1,
    std::vector<int>& idx2) {
      // This function extracts peak candidates from beta matrix by performing a t-Test on the quadratic terms
      for (int col : idx1) {
          Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
          Matrix y = ylog.subMatrix(xIndices[col]-scaleVec[col],xIndices[col]+scaleVec[col]+1,0,1);
          int dof = scaleVec[col]*2-3;
          double tmp_mse = calcMse(yhat, y);
          double t_meas1 = beta(2,col) / std::sqrt(inverseMatrices[scaleVec[col]](2,2) * tmp_mse);
          double t_meas2 = beta(3,col) / std::sqrt(inverseMatrices[scaleVec[col]](3,3) * tmp_mse);
          double t_crit = tVal[dof];
          double t_meas = 0.;
          if (std::abs(t_meas1) > std::abs(t_meas2)) {
            t_meas = std::abs(t_meas2);
          } else {
            t_meas = std::abs(t_meas1);
          }

          if (t_meas > t_crit) {
            mse[col] = tmp_mse;
            idx2.push_back(col);
          } else {
            fltrVec[col] = false;
          }
      }  
  }

  void Peakmodel::parameterCriterion(
    const int N,
    bool*& fltrVec,
    std::vector<double>& peakHeight,
    std::vector<double>& peakHeight_uncertainty,
    std::vector<double>& peakArea,
    std::vector<double>& peakArea_uncertainty,
    const std::vector<int>& scaleVec,
    const Matrix& beta,
    const std::vector<double>& mse,
    const std::vector<int>& idx1,
    std::vector<int>& idx2) {
      /* This function calculates the Peak height and Peak area including their uncertainties and checks for relevances using the 3 sigma criterion, i.e., X is relevant if X > 3s(X)*/
      for (int col : idx1) {
        // Peak Height Criterion
        std::pair<double,double> h = calcPeakHeight(beta(0,col), beta(1,col), beta(2,col), beta(3,col));
        Matrix J = calcJacobianMatrix_Height(h.first, h.second);
        Matrix C = inverseMatrices[scaleVec[col]] * mse[col];
        double h_uncertainty = std::sqrt( (J * C * J.T()).sumElements());
        if (3*h_uncertainty > h.second) {
          fltrVec[col] = false;
        } else {

        // Peak Area Criterion
        Matrix J_A = calcJacobianMatrix_Area(beta(0,col), beta(1,col), beta(2,col), beta(3,col));
        double A_uncertainty = std::sqrt((J_A * C * J_A.T()).sumElements());
        double A_notCovered = calcPeakAreaNotCovered(beta(0,col), beta(1,col), beta(2,col), beta(3,col),-scaleVec[col],scaleVec[col]) ;
        A_uncertainty += A_notCovered;
        if (3*A_uncertainty > J_A(0,0) || J_A(0,0) < 0) {
          fltrVec[col] = false;
        } else {
          peakHeight[col] = h.second;
          peakHeight_uncertainty[col] = h_uncertainty;
          peakArea[col] = J_A(0,0);
          peakArea_uncertainty[col] = A_uncertainty;
          idx2.push_back(col);
        }
        }
      }
    }

  std::pair<double,double> Peakmodel::calcPeakHeight(
      const double b0,
      const double b1,
      const double b2,
      const double b3
      ) const {
        /* this function calcultes the analytical peak height from the regression coefficients
        it returns the result as a pair (x_apex, y_apex)*/
        double x_apex = -.5 * b1;
        if (b1 < 0) { // Apex is on the left side
          x_apex /= b2;
        } else { // Apex is on the right side
          x_apex /= b3;
        }
        double y_apex = b0 + (x_apex * b1 * .5);
        std::pair<double,double> result = std::make_pair(x_apex, std::exp(y_apex));
        return result;
      }
  
  Matrix Peakmodel::calcJacobianMatrix_Height(
      const double x_apex,
      const double y_apex
      ) const {
        Matrix J(1,4);
        J(0,0) = y_apex;
        J(0,1) = x_apex * J(0,0);
        (x_apex < 0) ? J(0,2) = x_apex * J(0,1) : J(0,3) = x_apex * J(0,1); 
        return J;
      }
  
  Matrix Peakmodel::calcJacobianMatrix_Position(
        const double b1,
        const double b2,
        const double b3
    ) const {
      Matrix J(1,4);
      if (b1 < 0) {
              J(0,1) = -.5 / b2;
              J(0,2) = b1 / 2 / (b2 * b2);
            } else {
              J(0,1) = -.5 / b3;
              J(0,3) = b1 / 2 / (b3 * b3);
            }
      return J;
    }

  Matrix Peakmodel::calcJacobianMatrix_Area(
        const double b0,
        const double b1,
        const double b2,
        const double b3
      ) const {
        // calculate check sum to find the correct case
        int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);
        Matrix J_L(1,4);
        Matrix J_R(1,4);
        switch (key) {
          case 7:  // Case 1a: apex left
              J_L = calcJacobianMatrix_Area_half(b0,b1,b2,true);
              J_R = calcJacobianMatrix_Area_half(b0,b1,b3,false);
              break;
          case 3:  // Case 1b: apex right
              J_R = calcJacobianMatrix_Area_half(b0,b1,b3,false);
              J_L = calcJacobianMatrix_Area_half(b0,b1,b2,true);
              break;
          case 6:  // Case 2a: apex left | valley right
              J_L = calcJacobianMatrix_Area_half(b0,b1,b2,true);
              J_R = calcJacobianMatrix_Area_half_Valley(b0,b1,b3,false);
              break;
          case 1:  // Case 2b: apex right | valley left
              J_R = calcJacobianMatrix_Area_half(b0,b1,b3,false);
              J_L = calcJacobianMatrix_Area_half_Valley(b0,b1,b2,true);
              break;
          default:

              break;
      }
      return J_R + J_L;
      }
  
  Matrix Peakmodel::calcJacobianMatrix_Area_half(
      const double b0,
      double b1,
      const double b2,
      const bool isLeft
    ) const {
      // side check
      if (!isLeft) {
        b1 = -b1;
      }
      // precalculations:
      double sqrtPi = std::sqrt(M_PI);
      double sqrtNeg4B2 = 2*std::sqrt(-b2);
      double expB0 = std::exp(b0);
      double expTerm = std::exp(-b1/4/b2);
      double erfcTerm = std::erfc(b1 / sqrtNeg4B2);

      // first term 
      double J1 = sqrtPi * expB0 * expTerm * erfcTerm / sqrtNeg4B2;
      // second term
      double J2 = - (J1 * b1 - expB0) / 2 / b2;
      // third term
      double J3 = (-expB0 * expTerm * expTerm * b1 + J1 * b1 * b1 - J1 * 4 * b2) / 64 / b2 / b2;

      Matrix J(1,4);
      J(0,0) = J1;
      J(0,1) = J2;
      isLeft ? J(0,2) = J3 : J(0,3) = J3;
      return J;
    }

  Matrix Peakmodel::calcJacobianMatrix_Area_half_Valley(
      const double b0,
      double b1,
      const double b2,
      const bool isLeft
    ) const {
      // Precalculation:
      if (!isLeft) {
        b1 = -b1;
        }
      double sqrtB2 = std::sqrt(b2);
      double b1_2_b2 = b1 / 2 / b2;
      double BaseTerm = std::exp(b0 - b1*b1_2_b2 / 2) / M_2_SQRTPI * erfi(b1_2_b2); 


      Matrix J(1,4);
      J(0,0) = - BaseTerm / sqrtB2;
      J(0,1) = BaseTerm * b1_2_b2 / sqrtB2;
      if (isLeft) {
        J(0,2) = J(0,1) / b1 - J(0,1) * b1_2_b2 ;
      } else {
        J(0,3) = J(0,1) / b1 - J(0,1) * b1_2_b2 ;
      }
      return J;
    }

    double Peakmodel::calcPeakAreaNotCovered(
      const double b0,
      const double b1,
      const double b2,
      const double b3,
      const double left_edge,
      const double right_edge) const {
        /*This function calculates the Peak area NOT covered by regression window*/
        // constants:
        double y_left = 0;
        double y_right = 0;
        double A_left = 0;
        double A_right = 0;
        // left side:
        double mu_L = -b1 / 2 / b2;
        double h_L = exp(b0-b1/2*mu_L);
        double sqrtB2 = sqrt(std::abs(b2));
        double const_L = h_L / sqrtB2 / M_2_SQRTPI;
        double range_L = mu_L - left_edge;
        if (b2 < 0) {
          // ordinary peak with integration from -inf to left_edge
          A_left += const_L * erfc(sqrtB2 * range_L);
          y_left += exp(b0 + b1 * left_edge + b2 * left_edge*left_edge);
        } else {
          // check if valley point is inside regression window
          if (left_edge < mu_L) {
            // valley point is inside, i.e., integration window from vp to vp
            y_left += exp(b0 + b1 * mu_L + b2 * mu_L*mu_L);
          } else {
            // valley point is outside, i.e., integration window from vp to left_edge
            A_left += const_L * erfi(- sqrtB2 * range_L);
            y_left += exp(b0 + b1 * left_edge + b2 * left_edge*left_edge);
          }
        }

        // right side:
        double mu_R = -b1 / 2 / b3;
        double h_R = exp(b0-b1/2*mu_R);
        double sqrtB3 = sqrt(std::abs(b3));
        double const_R = h_R / sqrtB3 / M_2_SQRTPI;
        double range_R = right_edge - mu_R;
        if (b3 < 0) {
          // ordinary peak with integration from right_edge to inf
          A_right += const_R * erfc(sqrtB3 * range_R);
          y_right += exp(b0 + b1 * right_edge + b3 * right_edge*right_edge);
        } else {
          // check if valley point is inside regression window
          if (right_edge > mu_R) {
            // valley point is inside, i.e., integration window from vp to vp
            y_right += exp(b0 + b1 * mu_R + b3 * mu_R*mu_R);
          } else {
            // valley point is outside, i.e., integration window from right_edge to vp
            A_right += const_R * erfi(- sqrtB3 * range_R);
            y_right += exp(b0 + b1 * right_edge + b3 * right_edge*right_edge);
          }
        }

      return A_left - A_right + (right_edge - left_edge) * (y_left + y_right) / 2;
      }
  
  void Peakmodel::mergeRegressions(
      const int N,
      bool*& fltrVec,
      const Matrix& beta,
      const std::vector<int>& scaleVec,
      const std::vector<int>& xIndices, 
      const std::vector<double>& apex_position,
      std::vector<double>& apex_position_uncertainty,
      const std::vector<double>& valley_position,
      const Matrix& Y,
      const std::vector<double> mse,
      const std::vector<int>& idx1,
      std::vector<int>& idx2
    ) {
      /*This function merges the peak candidates that are similar and therefore describe the same peak. To do so, a t-test based on the positions will be made and afterwards a comparison of the MSE in the exp-Domain.*/

      // new filter for the best peaks. initialized with all being false
      std::unordered_set<int> idx_ref;
      bool is_empty = true;

      // vector of position uncertainties
      double* var_apex = new double[N]();
      
      // create an mse vector for later comparison
      double* mse_exp = new double[N](); 
      // group Vector for candidates that should be compared with the new one
      std::unordered_set<int> idx_candidates;
      size_t n_Candidates = 0;
      for (int col : idx1) {
        idx_candidates.clear();
          /*Here we scan through all peak candidates*/
          // first peak candidate will always be added
          if (is_empty) {
            // add first peak to ref list
            idx_ref.insert(col);

            Matrix J = calcJacobianMatrix_Position(beta(1,col),beta(2,col),beta(3,col));
            var_apex[col] = (J * inverseMatrices[scaleVec[col]] * J.T()).sumElements() * mse[col];
            apex_position_uncertainty[col] = std::sqrt(var_apex[col]);
            Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
            Matrix y = Y.subMatrix(xIndices[col]-scaleVec[col],xIndices[col]+scaleVec[col]+1,0,1);
            
            for (size_t j = 0; j < y.numRows(); j++) {
              yhat(j,0) = std::exp(yhat(j,0));
            }
            mse_exp[col] = calcMse(yhat, y);
            is_empty = false;
          } else {
          
          // scan through BestPeaks and check for similarity
          n_Candidates = 0;
          for (int idx : idx_ref) {

              // check minimum distance
              if (std::abs(apex_position[col] - apex_position[idx]) < 5) {
                // these peaks cannot be distinguished and are therefore similar
                n_Candidates ++;
                idx_candidates.insert(col);
              } else {
                /*These peaks should be tested using t-Test for the peak distances. To do so, we need the uncertainties of the positions.*/

                // Check if variance was already calculated
                if (var_apex[col] == 0) {
                Matrix J = calcJacobianMatrix_Position(beta(1,col),beta(2,col),beta(3,col));
                var_apex[col] = (J * inverseMatrices[scaleVec[col]] * J.T()).sumElements() * mse[col];
                apex_position_uncertainty[col] = std::sqrt(var_apex[col]);}

                double se_pos = std::sqrt(var_apex[idx] + var_apex[col]);
                double t_meas = std::abs(apex_position[col] - apex_position[idx]) / se_pos;
                int df = 2 * (scaleVec[col] + scaleVec[idx]) - 6;
                double t_crit = tVal[df];
                if (t_meas < t_crit) {
                  // these peaks cannot be distinguished and are therefore similar
                  idx_candidates.insert(col);
                  n_Candidates++;
                  // Check if mse was already calculated
                  if (mse_exp[col] == 0) {
                  Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
                  Matrix y = Y.subMatrix(xIndices[col]-scaleVec[col],xIndices[col]+scaleVec[col]+1,0,1);
      
                  for (size_t j = 0; j < y.numRows(); j++) {
                    yhat(j,0) = std::exp(yhat(j,0));
                  }
                  mse_exp[col] = calcMse(yhat, y);
                  }
                }
              }
          }
          // check if there are candidates for mse_comparison:
          if (n_Candidates > 0) {
            // There are similar peaks: start survival of the fittest
            // calculate the average of the candidates' mse_exp
            double mse_exp_ref = 0;
            for (int i : idx_candidates) {
                mse_exp_ref += (mse_exp[i] / n_Candidates);
            }
            if (mse_exp[col] < mse_exp_ref) {
              // new peak is better than former candidates
              idx_ref.insert(col);
              // delete old ones from list
              for (int i : idx_candidates) {
                idx_ref.erase(i);
              }
            }
          } else {
            // No similar peaks: add new peak to the list
            idx_ref.insert(col);
          }
          }
      }
      for (int idx : idx_ref) {
        idx2.push_back(idx);
      }
      delete[] mse_exp;
      delete[] var_apex;
    }

  void Peakmodel::rescalePosition(
      const int N,
      bool*& fltrVec,
      const Matrix& x_data,
      std::vector<double>& apex_position,
      std::vector<double>& apex_positions_uncertainty,
      const std::vector<int>& idx1
    ) {
      /*This function rescales the apex positions based in the original x_data. To do so, the */
      for (int col : idx1) {
        int apex_left = apex_position[col];
        int apex_right = apex_left + 1;
        double dx = x_data(apex_right,0) - x_data(apex_left,0);
        apex_position[col] = x_data(apex_left,0) + (apex_position[col] - apex_left) * dx;
        apex_positions_uncertainty[col] *= dx;
      }
    }

  void Peakmodel::exportResults(
    const Matrix& beta, 
      bool*& fltrVec, 
      const std::vector<int>& xIndices,
      const int N,
      const int smplID,
      const std::vector<double>& apex_position,
      const std::vector<double>& apex_position_uncertainty,
      const std::vector<double>& peakHeight,
      const std::vector<double>& peakHeight_uncertainty,
      const std::vector<double>& peakArea,
      const std::vector<double>& peakArea_uncertainty,
      const std::vector<int>& idx1,
      int& peakID) {
    
    /* THIS WAS FOR CSV EXPORT */
    // // Open CSV-file
    // std::ofstream file("results.csv", std::ios::app); 
    // if (!file.is_open()) {
    //     std::cerr << "Error while opening CSV-File." << std::endl;
    //     return;
    // }

    // for (int col : idx1) {
    //     file << key << ","
    //          << beta(0, col) << "," 
    //          << beta(1, col) << "," 
    //          << beta(2, col) << ","
    //          << beta(3, col) << "," 
    //          << xIndices[col] << ","
    //          << apex_Position[col] << ","
    //          << peakHeight[col] << std::endl;
    // }
    for (int col : idx1) {
      // create a temporary Peakproperties object:
      Peakproperties tmp_peak(
        beta(0,col), // Regression Coefficients
        beta(1,col), // Regression Coefficients
        beta(2,col), // Regression Coefficients
        beta(3,col), // Regression Coefficients
        peakID, // peak ID
        smplID, // smpl ID (running number of spectrum or chromatogram)
        apex_position[col], // Peak Position
        peakHeight[col], // Peak Height
        0.0, // Peak Width <--- This is not yet implemented
        peakArea[col], // Peak Area
        apex_position_uncertainty[col], // sigma Position 
        peakHeight_uncertainty[col], // sigma Height
        0.0, // sigma Width <--- This is not yet implemented
        peakArea_uncertainty[col], // sigma Area
        std::erfc(peakArea_uncertainty[col] / peakArea[col])); // DQS
      // add tmporary Peak to the List
      peakProperties.emplace(peakID, tmp_peak);
      peakID++;
    }
  }

  // debugging & printing
  std::vector<double> Peakmodel::getPeakProperties(const Peakproperties::PropertiesNames& varName) const {
    int N = peakProperties.size();
    std::vector<double> vals(N);
    size_t k = 0;
    for (const auto& peak : peakProperties) {
        vals[k] = peak.second.getProperty(varName);
        k++;
      }
  return vals;
  }

  const Peakproperties& Peakmodel::operator[](int ID) const {
    auto it = peakProperties.find(ID);
    if (it != peakProperties.end()) {
        return it->second;
    }
    throw std::out_of_range("ID not found in peakProperties map");
  }

  void Peakmodel::printTValues() {
    for (const auto& kv : tVal) {
            std::cout << "Degree of Freedom: " << kv.first << ", t-Value: " << kv.second << std::endl;
        }
  }
}

