// qalgorithms.cpp

#include "../include/qalgorithms.h"
#include "../include/qalgorithms_utils.h"
#include "../include/qalgorithms_matrix.h"

std::mutex mtx; // Global Mutex

namespace q
{

  // Constructor
  Peakproperties::Peakproperties() {}

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
  {
  }

  // debuging & printing
  double Peakproperties::getProperty(Peakproperties::PropertiesNames varName) const
  {
    switch (varName)
    {
    case COEFF_B0:
      return coeff_b0;
    case COEFF_B1:
      return coeff_b1;
    case COEFF_B2:
      return coeff_b2;
    case COEFF_B3:
      return coeff_b3;
    case PEAKID:
      return peakID;
    case SMPLID:
      return smplID;
    case POSITION:
      return position;
    case HEIGHT:
      return height;
    case WIDTH:
      return width;
    case AREA:
      return area;
    case SIGMAPOSITION:
      return sigmaPosition;
    case SIGMAHEIGHT:
      return sigmaHeight;
    case SIGMAWIDTH:
      return sigmaWidth;
    case SIGMAAREA:
      return sigmaArea;
    case DQS:
      return dqs;
    default:
      break;
    }
    return 0.0;
  }

  void Peakproperties::print() const
  {
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
  Peakmodel::Peakmodel(int a_max)
  {
    for (int a = 2; a <= a_max; a++)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    }
    setMode(Mode::SILENT);
    n_measurementData = 0;
  }

  // Constructor to initialize Peak model based on a lower and an upper limit for df
  Peakmodel::Peakmodel(int a_min, int a_max)
  {
    for (int a = a_min; a <= a_max; a++)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    }
    setMode(Mode::SILENT);
    n_measurementData = 0;
  }

  // Constructor to initialize Peak model based on a vector for a
  Peakmodel::Peakmodel(std::vector<int> A)
  {
    for (const auto &a : A)
    {
      calculateDesignMatrix(a);
      calculatePseudoInverse(a);
    }
    setMode(Mode::SILENT);
    n_measurementData = 0;
  }

  // Load t-Values
  void Peakmodel::loadTValues(const tValues &tData)
  {
    for (size_t i = 1; i < 501; i++)
    {
      tVal[i] = tData[i];
    }
  }

  // Function to calculate the Designmatrix for a given a
  void Peakmodel::calculateDesignMatrix(int a)
  {
    // construct matrix
    size_t m = 4;
    size_t n = 2 * a + 1;
    Matrix X(n, m);

    // x vector
    std::vector<int> x(n);
    for (size_t i = 0; i < n; i++)
    {
      x[i] = i - a;
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
    designMatrices.emplace(a, X);
  }

  // Function to calculate the Designmatrix for a given a
  void Peakmodel::calculatePseudoInverse(int a)
  {
    Matrix X = getDesignMatrix(a);
    Matrix Xt = X.T();
    Matrix XtX = Xt * X;
    inverseMatrices.emplace(a, XtX.inv());
    Matrix P = inverseMatrices[a] * Xt;
    pseudoInverses.emplace(a, P);
  }

  std::vector<int> Peakmodel::getScales() const
  {
    std::vector<int> scales;
    for (const auto &pair : designMatrices)
    {
      scales.push_back(pair.first);
    }
    return scales;
  }

  std::vector<int> Peakmodel::getScaleVec(
      const std::vector<int> &scales,
      const int n,
      const int N) const
  {
    std::vector<int> scaleVec(N);
    int u = 0;
    for (const int k : scales)
    {
      size_t jMax = n - 2 * k;
      for (size_t j = 0; j < jMax; j++)
      {
        scaleVec[u] = k;
        u++;
      }
    }
    return scaleVec;
  }

  // Function to get the Designmatrix for a given a
  Matrix Peakmodel::getDesignMatrix(int a) const
  {
    auto it = designMatrices.find(a);
    if (it != designMatrices.end())
    {
      return it->second;
    }
    else
    {
      throw std::runtime_error("Key not found in designMatrices map");
    }
  }

  // Function to get the Pseudo Inverse for a given a
  Matrix Peakmodel::getPseudoInverse(int a) const
  {
    auto it = pseudoInverses.find(a);
    if (it != pseudoInverses.end())
    {
      return it->second;
    }
    else
    {
      throw std::runtime_error("Key not found in pseudoInverses map");
    }
  }

  // Function to add some Measurement Data to the Peak Model
  // void Peakmodel::addSingleMeasurement(Matrix& xyData, bool zero_interpolation) {
  //   int maxKey = measurementData.empty() ? -1 : std::prev(measurementData.end())->first;
  //   int newKey = maxKey + 1;

  //   // check if sorted by column 1 and sort if unsorted
  //   xyData.sort1();
  //   if (zero_interpolation) { // default is true
  //     zeroInterpolation(xyData);
  //   }
  //   measurementData[newKey] = xyData;
  //   n_measurementData ++;
  // }

  void Peakmodel::addMultipleMeasurements(Matrix &xyData, bool zero_interpolation)
  {
    xyData.sort1(); // Sort the entire matrix by the first column
    dataID key;
    // analyze number of groups and members
    if (mode == Mode::PROGRESS)
    {
      std::cout << std::endl
                << "analyze number of datasets... ";
    }
    std::map<int, int> groupCount;
    size_t n = xyData.numRows();
    size_t m = xyData.numCols();
    for (size_t i = 0; i < n; ++i)
    {
      int groupNum = static_cast<int>(xyData(i, 2));
      groupCount[groupNum]++;
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // initialize data matrices
    if (mode == Mode::PROGRESS)
    {
      std::cout << "initialize data matrices (n=" << groupCount.size() << ")... ";
    }
    for (const auto &[groupNum, rowCount] : groupCount)
    {
      key.sampleID = groupNum;
      key.subSampleID = 0;
      measurementData.emplace(key, Matrix(rowCount, m));
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // fill data matrices
    if (mode == Mode::PROGRESS)
    {
      std::cout << "transfer data to data matrices... ";
    }
    std::map<int, size_t> currentRowCount;
    for (size_t i = 0; i < n; ++i)
    {
      int groupNum = static_cast<int>(xyData(i, 2));
      size_t rowIdx = currentRowCount[groupNum]++;
      for (size_t j = 0; j < m; j++)
      {
        dataID key;
        key.sampleID = groupNum;
        key.subSampleID = 0;
        measurementData[key](rowIdx, j) = xyData(i, j);
      }
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // gap-filling, this step will also mark large gaps with (-1, -1)
    if (mode == Mode::PROGRESS)
    {
      std::cout << "fill data gaps... ";
    }
    if (zero_interpolation)
    {
      for (auto &pair : measurementData)
      {
        gapFilling(pair.second);
        n_measurementData++;
      }
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // data splitting
    if (mode == Mode::PROGRESS)
    {
      std::cout << "split datasets into subsets... ";
    }
    if (zero_interpolation)
    {
      std::map<dataID, Matrix> tempMeasurementData;
      for (auto &pair : measurementData)
      {
        dataSplitting(pair.first, pair.second, tempMeasurementData);
      }
      measurementData = tempMeasurementData;
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "(n=" << measurementData.size() << ")"
                << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // zero-interpolation
    if (mode == Mode::PROGRESS)
    {
      std::cout << "interpolate zeros... ";
    }
    if (zero_interpolation)
    {
      for (auto &pair : measurementData)
      {
        // std::cout << pair.first.sampleID << ", " << pair.first.subSampleID << std::endl;
        zeroInterpolation(pair.second);
      }
    }
    if (mode == Mode::PROGRESS)
    {
      std::cout << "\033[1;32m done"
                << "\033[0m" << std::endl;
    }

    // for (auto& pair : measurementData) {
    //     n_measurementData ++;
    //   }
  }

  void Peakmodel::gapFilling(Matrix &xyData)
  {
    /* This function detects missing data and fills in the gaps. Specifically, when a gap is found, up to four zeros will be added to both sides of it. */
    if (xyData.numRows() < 3)
    {
      return;
    }
    // pre-define variables
    std::vector<double> filledData;
    std::vector<double> filledData_y;
    bool skipPrevGapCalculation = false;
    double lastPreviousGap = 0.0;
    // add first
    filledData.push_back(xyData(0, 0));
    filledData_y.push_back(xyData(0, 1));
    // find gaps
    for (size_t i = 1; i < xyData.numRows() - 1; ++i)
    {
      /* add current row from original data matrix to x and y vectors */
      filledData.push_back(xyData(i, 0));
      filledData_y.push_back(xyData(i, 1));
      /* define current dinstance between next and current data point */
      double gap = xyData(i + 1, 0) - xyData(i, 0);
      /* define distance between current und former data point as reference*/
      double previousGap = skipPrevGapCalculation ? lastPreviousGap : xyData(i, 0) - xyData(i - 1, 0);

      lastPreviousGap = previousGap > lastPreviousGap ? previousGap : lastPreviousGap;
      /* reset skip criterion */
      skipPrevGapCalculation = false;

      /* calculate the number of datapoints that should included into the gap */
      int fillers = static_cast<int>(gap / previousGap) - 1;

      /* define maximum number of fillers, it is 8 due to statistical conventions that a gap of more than 8 points is seen as full separation of the profiles, i.e., here a split can be performed later.*/
      int maxFillers = 8;
      int fillersToAdd = std::min(fillers, maxFillers);
      // std::cout << gap << ", " << previousGap << ", " << fillersToAdd << std::endl;
      // Add first 4 fillers
      for (int j = 0; j < 4 && j < fillersToAdd; ++j)
      {
        double fillerValueX = xyData(i, 0) + (j + 1) * (gap / (fillers + 1));
        filledData.push_back(fillerValueX);
        filledData_y.push_back(0.0);
      }

      // If there are more than 4 fillers, add the last 4 fillers
      if (fillersToAdd > 4)
      {
        /* mark a gap */
        filledData.push_back(-1.0);
        filledData_y.push_back(-1.0);
        for (int j = fillers - 4; j < fillers; ++j)
        {
          double fillerValueX = xyData(i, 0) + (j + 1) * (gap / (fillers + 1));
          filledData.push_back(fillerValueX);
          filledData_y.push_back(0.0);
        }
      }
      if (fillersToAdd > 0)
      {
        skipPrevGapCalculation = true;
      }
    }
    // add last
    filledData.push_back(xyData(xyData.numRows() - 1, 0));
    filledData_y.push_back(xyData(xyData.numRows() - 1, 1));
    // Transfer data from filledData to xyData
    xyData.reinitialize(filledData.size(), 2);
    // Matrix xyData(filledData.size(),2);
    for (size_t i = 0; i < filledData.size(); ++i)
    {
      xyData(i, 0) = filledData[i];
      xyData(i, 1) = filledData_y[i];
    }
  }

  void Peakmodel::zeroInterpolation(Matrix &xyData)
  {
    Matrix left_to_right = xyData;
    zeroInterpolation_oneDirection(left_to_right);

    // Flipping the matrix upside down
    Matrix right_to_left = xyData;
    for (size_t i = 0; i < right_to_left.numRows() / 2; i++)
    {
      for (size_t j = 0; j < right_to_left.numCols(); j++)
      {
        std::swap(right_to_left(i, j), right_to_left(right_to_left.numRows() - 1 - i, j));
      }
    }
    zeroInterpolation_oneDirection(right_to_left);
    // Flipping back
    for (size_t i = 0; i < right_to_left.numRows() / 2; i++)
    {
      for (size_t j = 0; j < right_to_left.numCols(); j++)
      {
        std::swap(right_to_left(i, j), right_to_left(right_to_left.numRows() - 1 - i, j));
      }
    }
    // Combining the results
    for (size_t i = 0; i < xyData.numRows(); i++)
    {
      xyData(i, 1) = std::max(left_to_right(i, 1), right_to_left(i, 1));
    }
  }

  void Peakmodel::zeroInterpolation_oneDirection(Matrix &xyData)
  {
    /* This function corrects zeros by interpolation considering the direct neighbors*/
    size_t n = xyData.numRows();
    // Checking for fronting zeros
    size_t i = 0;
    while (xyData(i, 1) == 0.)
    {
      i++;
      if (i == n)
      {
        return;
      }
    }
    if (i > 0)
    {
      // There are fronting zeros, which will be corrected
      for (int j = i - 1; j >= 0; --j)
      {
        xyData(j, 1) = xyData(j + 1, 1) / 2.;
      }
    }

    // Checking the rest of the data (except last data point)
    for (; i < n - 1; i++)
    {
      if (xyData(i, 1) == 0.)
      {
        xyData(i, 1) = (xyData(i - 1, 1) + xyData(i + 1, 1)) / 2.;
      }
    }

    // Checking the last data point
    if (xyData(n - 1, 1) == 0.)
    {
      xyData(n - 1, 1) = xyData(n - 2, 1) / 2.;
    }
  }

  void Peakmodel::dataSplitting(dataID key, Matrix &xyData, std::map<dataID, Matrix> &DataSet)
  {
    /* This function will splitt data along so-called large data gaps. The gaps need to be marked in the gap filling step with a -1. */
    size_t start_idx = 0;
    size_t end_idx = 0;
    int subSampleID = 0;
    for (size_t i = 0; i < xyData.numRows(); i++)
    {
      if (xyData(i, 0) == -1.)
      {
        /* gap identified*/
        end_idx = i;
        Matrix tmp_data = xyData.subMatrix(start_idx, end_idx, 0, 2);
        if (tmp_data.col(1).sumElements() > 0)
        {
          dataID newKey;
          newKey.sampleID = key.sampleID;
          newKey.subSampleID = subSampleID;
          DataSet.emplace(newKey, tmp_data);
          subSampleID++;
        }
        start_idx = i + 1;
      }
    }
  }

  Matrix Peakmodel::getData(int sampleID, int subSampleID) const
  {
    dataID key;
    key.sampleID = sampleID;
    key.subSampleID = subSampleID;
    auto it = measurementData.find(key);
    if (it != measurementData.end())
    {
      return it->second;
    }
    else
    {
      throw std::runtime_error("Key not found in measurementData map");
    }
  }

  // Main Function
  void Peakmodel::findPeaks()
  {
    /* This is the main function to perform the automated peak detection. */
    // get list of scales
    const std::vector<int> scales = getScales();
    // pre-calculate number of regressions: N = (scales.size()*n - 2*sum(scales))
    const size_t k = scales.size(); // number of scales
    const int s = 2 * sum(scales);  // 2*sum(scales)

    int n_meas = measurementData.size();
    ProgressBar progressBar(n_meas);
    int progress_i = 0;
    int updateProgress_nextValue = 0;
    bool updateProgress = true;
    for (const auto &pair : measurementData)
    {
      this->runRegression(pair, scales, k, s);
      if (mode == Mode::PROGRESS)
      {
        progress_i++;
        if (static_cast<int>(progress_i * 100 / n_meas) > updateProgress_nextValue)
        {
          progressBar.update(progress_i);
          updateProgress_nextValue++;
        }
      }
    }
    if (mode == Mode::PROGRESS)
    {
      progressBar.complete();
    }
  }

  // Run Regression
  void Peakmodel::runRegression(
      const std::pair<const dataID, const Matrix> &pair, const std::vector<int> &scales, const size_t k, const int s)
  {
    /* This is the main function to perform the automated peak detection. */
    // get key and data
    dataID key = pair.first;
    Matrix data = pair.second;
    int peakID = 0;
    // log transform
    Matrix ylog = data.col(1).log(); // 1, i.e., first column (x) will be ignored

    // preallocate conv Matrix that later will contain all regression coefficients
    size_t n = ylog.numRows();
    // check if scales fit with the data size
    if (n > 2 * scales[0])
    {
      size_t k2 = k;
      int s2 = s;
      std::vector<int> scales_tmp = scales;
      int largest_scale = scales_tmp[scales_tmp.size() - 1];
      while (n < 2 * largest_scale + 1)
      {
        scales_tmp.pop_back();
        k2 = scales_tmp.size();
        s2 = 2 * sum(scales_tmp);
        largest_scale = scales_tmp[scales_tmp.size() - 1];
      }

      // Variables and Vectors
      size_t N = k2 * n - s2;
      Matrix beta(4, N);
      bool *fltrVec = new bool[N];
      std::fill_n(fltrVec, N, true);

      std::vector<int> xIndices(N);
      std::vector<double> apex_positions(N);
      std::vector<double> apex_positions_uncertainty(N);
      std::vector<double> valley_positions(N);
      std::vector<double> peakHeight(N);
      std::vector<double> peakHeight_uncertainty(N);
      std::vector<double> peakArea(N);
      std::vector<double> peakArea_uncertainty(N);
      std::vector<int> scaleVec = getScaleVec(scales_tmp, n, N);
      std::vector<double> mse(N);
      int currentIndex = 0;

      // iterate through all scales
      for (const auto &p : pseudoInverses)
      {
        // check if data set is too small for full scan
        if (p.second.numCols() < n)
        {
          convolveP(beta, xIndices, p.second, ylog, currentIndex);
        }
      }

      // Filtering Regression Results
      // Vector of Indices that have passed the last filter
      std::vector<int> idx1;
      coefficientCriterion(N, fltrVec, scaleVec, apex_positions, valley_positions, xIndices, beta, idx1);

      std::vector<int> idx2;
      quadraticTermCriterion(N, fltrVec, scaleVec, xIndices, beta, ylog, mse, idx1, idx2);

      idx1.clear();
      parameterCriterion(N, fltrVec, peakHeight, peakHeight_uncertainty, peakArea, peakArea_uncertainty, scaleVec, beta, mse, idx2, idx1);

      idx2.clear();
      mergeRegressions(N, fltrVec, beta, scaleVec, xIndices, apex_positions, apex_positions_uncertainty, valley_positions, data.col(1), mse, idx1, idx2);

      rescalePosition(N, fltrVec, data.col(0), apex_positions, apex_positions_uncertainty, idx2);

      {
        std::lock_guard<std::mutex> lock(mtx);
        exportResults(beta, fltrVec, xIndices, N, key.sampleID, apex_positions, apex_positions_uncertainty, peakHeight, peakHeight_uncertainty, peakArea, peakArea_uncertainty, idx2, peakID);
      }
      delete[] fltrVec;
    }
  }

  // Convolution
  void Peakmodel::convolveP(
      Matrix &beta,
      std::vector<int> &xIndices,
      const Matrix &P,
      const Matrix &ylog,
      int &currentIndex)
  {
    size_t n = ylog.numRows();
    size_t k = P.numCols();
    size_t n_segments = n - k + 1;

    Matrix p1 = ylog.convolveSymmetric(P.row(0));
    Matrix p2 = ylog.convolveRotation(P.row(1));
    Matrix p34 = ylog.convolveAntisymmetric(P.row(2));
    int u = 0;
    for (size_t j = 0; j < n_segments; j++)
    {
      beta(0, j + currentIndex) = p1(0, j);
      beta(1, j + currentIndex) = p2(0, j);
      beta(2, j + currentIndex) = p34(0, j);
      beta(3, j + currentIndex) = p34(1, j);
      xIndices[j + currentIndex] = u + k / 2;
      u++;
    }
    currentIndex += u;
  }

  double Peakmodel::calcMse(const Matrix &yhat, const Matrix &y) const
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

  // Regression Coefficient Filter Criterion
  void Peakmodel::coefficientCriterion(
      const int N,
      bool *&fltrVec,
      const std::vector<int> &scaleVec,
      std::vector<double> &apex_positions,
      std::vector<double> &valley_positions,
      const std::vector<int> &xIndices,
      const Matrix &beta,
      std::vector<int> &idx)
  {
    // This function extracts peak candidates from beta matrix
    for (int col = 0; col < N; col++)
    {
      if (fltrVec[col])
      {
        coefficientCriterionCases(scaleVec[col], beta(1, col), beta(2, col), beta(3, col), xIndices[col], apex_positions[col], valley_positions[col], fltrVec[col], idx, col);
      }
    }
  }

  void Peakmodel::coefficientCriterionCases(
      const int scale,
      const double b1,
      const double b2,
      const double b3,
      const int xIndex,
      double &apex_position,
      double &valley_position,
      bool &fltrVal,
      std::vector<int> &idx,
      const size_t IDX)
  {
    /*
    This functions checks if a set of regression coefficients (b0, b1, b2, b3) are valid
    and describe an analytical peak after exp-transform.
     # valid peak case 1: b2<0 & b3<0 & apex in window
     # valid peak case 2a: b2>0 & b3<0 & b1>0 & |valley - apex| > 2 & apex in window
     # valud peak case 2b: b2<0 & b3>0 & b1<0 & |valley - apex| > 2 & apex in window
    */
    // creating unique checksum
    int key = (b1 < 0 ? 4 : 0) | (b2 < 0 ? 2 : 0) | (b3 < 0 ? 1 : 0); // @todo use enums here, logical or instead of +
    switch (key)
    {
    case 7: // Case 1a: apex left
      calcApex_position(scale, b1, b2, xIndex, apex_position, fltrVal);
      break;
    case 3: // Case 1b: apex right
      calcApex_position(scale, b1, b3, xIndex, apex_position, fltrVal);
      break;
    case 6: // Case 2a: apex left | valley right
      calcApexValley_position(scale, b1, b2, b3, xIndex, apex_position, valley_position, fltrVal);
      break;
    case 1: // Case 2b: apex right | valley left
      calcApexValley_position(scale, b1, b3, b2, xIndex, apex_position, valley_position, fltrVal);
      break;
    default:
      fltrVal = false;
      break;
    }
    if (fltrVal)
    {
      idx.push_back(IDX);
    }
  }

  void Peakmodel::calcApex_position(
      const int scale,
      const double b1,
      const double b2,
      const int xIndex,
      double &apex_position,
      bool &fltrVal)
  {
    apex_position = -b1 / 2 / b2;
    // check if apex position is in window with at least one point buffer
    if (abs(apex_position) < scale)
    {
      apex_position += xIndex;
      fltrVal = true;
    }
    else
    {
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
      double &apex_position,
      double &valley_position,
      bool &fltrVal)
  {
    apex_position = -b1 / 2 / b2;
    valley_position = -b1 / 2 / b3;
    // check if apex position is in window with at least one point buffer
    if ((abs(apex_position) < scale) & (abs(apex_position - valley_position) > 2))
    {
      apex_position += xIndex;
      valley_position += xIndex;
      fltrVal = true;
    }
    else
    {
      apex_position = 0.;
      valley_position = 0.;
      fltrVal = false;
    }
  }

  void Peakmodel::quadraticTermCriterion(
      const int N,
      bool *&fltrVec,
      const std::vector<int> &scaleVec,
      const std::vector<int> &xIndices,
      const Matrix &beta,
      const Matrix &ylog,
      std::vector<double> &mse,
      const std::vector<int> &idx1,
      std::vector<int> &idx2)
  {
    // This function extracts peak candidates from beta matrix by performing a t-Test on the quadratic terms
    for (int col : idx1)
    {
      Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
      Matrix y = ylog.subMatrix(xIndices[col] - scaleVec[col], xIndices[col] + scaleVec[col] + 1, 0, 1);
      int dof = scaleVec[col] * 2 - 3;
      double tmp_mse = calcMse(yhat, y);
      double t_meas1 = beta(2, col) / std::sqrt(inverseMatrices[scaleVec[col]](2, 2) * tmp_mse);
      double t_meas2 = beta(3, col) / std::sqrt(inverseMatrices[scaleVec[col]](3, 3) * tmp_mse);
      double t_crit = tVal[dof];
      double t_meas = 0.;
      if (std::abs(t_meas1) > std::abs(t_meas2))
      {
        t_meas = std::abs(t_meas2);
      }
      else
      {
        t_meas = std::abs(t_meas1);
      }

      if (t_meas > t_crit)
      {
        mse[col] = tmp_mse;
        idx2.push_back(col);
      }
      else
      {
        fltrVec[col] = false;
      }
    }
  }

  void Peakmodel::parameterCriterion(
      const int N,
      bool *&fltrVec,
      std::vector<double> &peakHeight,
      std::vector<double> &peakHeight_uncertainty,
      std::vector<double> &peakArea,
      std::vector<double> &peakArea_uncertainty,
      const std::vector<int> &scaleVec,
      const Matrix &beta,
      const std::vector<double> &mse,
      const std::vector<int> &idx1,
      std::vector<int> &idx2)
  {
    /* This function calculates the Peak height and Peak area including their uncertainties and checks for relevances using the 3 sigma criterion, i.e., X is relevant if X > 3s(X)*/
    int df;
    double t_crit;
    for (int col : idx1)
    {
      // Peak Height Criterion
      std::pair<double, double> h = calcPeakHeight(beta(0, col), beta(1, col), beta(2, col), beta(3, col));
      Matrix J = calcJacobianMatrix_Height(h.first, h.second);
      Matrix C = inverseMatrices[scaleVec[col]] * mse[col];
      double h_uncertainty = std::sqrt((J * C * J.T()).sumElements());
      df = scaleVec[col] * 2 - 3;
      t_crit = tVal[df];
      if (h.second / h_uncertainty < t_crit)
      {
        fltrVec[col] = false;
      }
      else
      {

        // Peak Area Criterion
        Matrix J_A = calcJacobianMatrix_Area(beta(0, col), beta(1, col), beta(2, col), beta(3, col));
        double A_uncertainty = std::sqrt((J_A * C * J_A.T()).sumElements());
        double A_notCovered = calcPeakAreaNotCovered(beta(0, col), beta(1, col), beta(2, col), beta(3, col), -scaleVec[col], scaleVec[col]);
        A_uncertainty += A_notCovered;
        // if (3*A_uncertainty > J_A(0,0) || J_A(0,0) < 0) {
        if (J_A(0, 0) / A_uncertainty < t_crit)
        {
          fltrVec[col] = false;
        }
        else
        {
          peakHeight[col] = h.second;
          peakHeight_uncertainty[col] = h_uncertainty;
          peakArea[col] = J_A(0, 0);
          peakArea_uncertainty[col] = A_uncertainty;
          idx2.push_back(col);
        }
      }
    }
  }

  // void Peakmodel::peakCoverageCriterion(
  //     const int N,
  //     bool*& fltrVec,
  //     std::vector<double>& peakArea,
  //     const std::vector<int>& scaleVec,
  //     const std::vector<int>& xIndices,
  //     const std::vector<double>& apex_positions,
  //     const std::vector<double>& valley_positions,
  //     const Matrix& beta,
  //     const std::vector<int>& idx1,
  //     std::vector<int>& idx2) {
  //       /* This function calculates the total peak area A_total and the peak area that is not covered by the data points A_ncov and therefore is uncertain due to extroplation. The filter criterion for this function is {A_ncov / A_tot < .33 && >.05}. This ensures that the peak area is mostly covered by the datapoints but the baseline is not part of the peak.*/
  //       for (int col : idx1) {
  //         std::vector<double> areaCoverage_peakArea = calcPeakArea(beta(0,col), beta(1,col), beta(2,col), beta(3,col),scaleVec[col]);
  //         if (areaCoverage_peakArea[0] > .33) {
  //           fltrVec[col] = false;
  //         } else {
  //           peakArea[col] = areaCoverage_peakArea[1];
  //           idx2.push_back(col);
  //         }
  //       }
  //     }

  std::vector<double> Peakmodel::calcPeakArea(
      const double b0,
      const double b1,
      const double b2,
      const double b3,
      const double scale) const
  {
    int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);
    // Total Area A
    double A_L;
    double A_R;
    // Not Covered Area B
    double B_L;
    double B_R;
    // Trapezoid
    double x_l = -scale;
    double x_r = scale;
    switch (key)
    {
    case 7: // Case 1a: apex left
      A_L = calcPeakArea_half(b0, b1, b2, 0.0, true, false);
      A_R = calcPeakArea_half(b0, b1, b3, 0.0, false, false);
      B_L = calcPeakArea_half(b0, b1, b2, -scale, true, false);
      B_R = calcPeakArea_half(b0, b1, b3, scale, false, false);
      break;
    case 3: // Case 1b: apex right
      A_R = calcPeakArea_half(b0, b1, b3, 0.0, false, false);
      A_L = calcPeakArea_half(b0, b1, b2, 0.0, true, false);
      B_R = calcPeakArea_half(b0, b1, b3, scale, false, false);
      B_L = calcPeakArea_half(b0, b1, b2, -scale, true, false);
      break;
    case 6: // Case 2a: apex left | valley right
      A_L = calcPeakArea_half(b0, b1, b2, 0.0, true, false);
      A_R = calcPeakArea_half(b0, b1, b3, 0.0, false, true);
      B_L = calcPeakArea_half(b0, b1, b2, -scale, true, false);
      B_R = calcPeakArea_half(b0, b1, b3, scale, false, true);
      if (std::abs(-b1 / 2 / b3) < scale)
      {
        x_r = -b1 / 2 / b3;
      }
      break;
    case 1: // Case 2b: apex right | valley left
      A_R = calcPeakArea_half(b0, b1, b3, 0.0, false, false);
      A_L = calcPeakArea_half(b0, b1, b2, 0.0, true, true);
      B_R = calcPeakArea_half(b0, b1, b3, scale, false, false);
      B_L = calcPeakArea_half(b0, b1, b2, -scale, true, true);
      if (std::abs(-b1 / 2 / b2) < scale)
      {
        x_l = -b1 / 2 / b2;
      }
      break;
    default:
      break;
    }
    const double trapezoid = (x_r - x_l) * .5 * (std::exp(b0 + b1 * x_l + b2 * x_l * x_l) + std::exp(b0 + b1 * x_r + b3 * x_r * x_r));
    std::vector<double> result(3);
    result[2] = trapezoid + B_L + B_R; // Area not Covered by data points
    result[1] = A_L + A_R;             // total peak Area
    result[0] = result[2] / result[1]; // uncoverage rate;
    return result;
  }

  double Peakmodel::calcPeakArea_half(
      const double b0,
      const double b1,
      const double b2,
      const double edge,
      const bool isleft,
      const bool hasvalley) const
  {
    const double sqrtB2 = std::sqrt(std::abs(b2));
    const double e_expr = std::exp(b0 - b1 * b1 / 4 / b2);
    double err_func_expr;
    if (hasvalley)
    {
      // check if valley is inside
      if (std::abs(-b1 / 2 / b2) < std::abs(edge))
      {
        // valley is in range
        // check if total area is calculated
        if (edge != 0)
        {
          // total area is not calculated
          return 0.;
        }
      }
      if (isleft)
      {
        err_func_expr = erfi((b1 + 2 * b2 * edge) / 2 / sqrtB2);
      }
      else
      {
        err_func_expr = -erfi((b1 + 2 * b2 * edge) / 2 / sqrtB2);
      }
    }
    else
    {
      if (isleft)
      {
        err_func_expr = 1 - std::erf((b1 + 2 * b2 * edge) / 2 / sqrtB2);
      }
      else
      {
        err_func_expr = 1 + std::erf((b1 + 2 * b2 * edge) / 2 / sqrtB2);
      }
    }
    return e_expr * err_func_expr / M_2_SQRTPI / sqrtB2;
  }

  std::pair<double, double> Peakmodel::calcPeakHeight(
      const double b0,
      const double b1,
      const double b2,
      const double b3) const
  {
    /* this function calcultes the analytical peak height from the regression coefficients
    it returns the result as a pair (x_apex, y_apex)*/
    double x_apex = -.5 * b1;
    if (b1 < 0)
    { // Apex is on the left side
      x_apex /= b2;
    }
    else
    { // Apex is on the right side
      x_apex /= b3;
    }
    double y_apex = b0 + (x_apex * b1 * .5);
    std::pair<double, double> result = std::make_pair(x_apex, std::exp(y_apex));
    return result;
  }

  Matrix Peakmodel::calcJacobianMatrix_Height(
      const double x_apex,
      const double y_apex) const
  {
    Matrix J(1, 4);
    J(0, 0) = y_apex;
    J(0, 1) = x_apex * J(0, 0);
    (x_apex < 0) ? J(0, 2) = x_apex * J(0, 1) : J(0, 3) = x_apex * J(0, 1);
    return J;
  }

  Matrix Peakmodel::calcJacobianMatrix_Position(
      const double b1,
      const double b2,
      const double b3) const
  {
    Matrix J(1, 4);
    if (b1 < 0)
    {
      J(0, 1) = -.5 / b2;
      J(0, 2) = b1 / 2 / (b2 * b2);
    }
    else
    {
      J(0, 1) = -.5 / b3;
      J(0, 3) = b1 / 2 / (b3 * b3);
    }
    return J;
  }

  Matrix Peakmodel::calcJacobianMatrix_Area(
      const double b0,
      const double b1,
      const double b2,
      const double b3) const
  {
    // calculate check sum to find the correct case
    int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);
    Matrix J_L(1, 4);
    Matrix J_R(1, 4);
    switch (key)
    {
    case 7: // Case 1a: apex left
      J_L = calcJacobianMatrix_Area_half(b0, b1, b2, true);
      J_R = calcJacobianMatrix_Area_half(b0, b1, b3, false);
      break;
    case 3: // Case 1b: apex right
      J_R = calcJacobianMatrix_Area_half(b0, b1, b3, false);
      J_L = calcJacobianMatrix_Area_half(b0, b1, b2, true);
      break;
    case 6: // Case 2a: apex left | valley right
      J_L = calcJacobianMatrix_Area_half(b0, b1, b2, true);
      J_R = calcJacobianMatrix_Area_half_Valley(b0, b1, b3, false);
      break;
    case 1: // Case 2b: apex right | valley left
      J_R = calcJacobianMatrix_Area_half(b0, b1, b3, false);
      J_L = calcJacobianMatrix_Area_half_Valley(b0, b1, b2, true);
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
      const bool isLeft) const
  {
    // precalculations:
    const double SQRTB2 = std::sqrt(-b2);
    const double EXP_B0 = std::exp(b0);
    const double B1_2_B2 = b1 / 2 / b2;
    const double EXP_B12 = std::exp(-b1 * B1_2_B2 / 2);
    double err_func_expr = 0.;
    /* the err_func_expr depends on the side */
    if (isLeft)
    {
      err_func_expr += std::erfc(b1 / 2 / SQRTB2);
    }
    else
    {
      err_func_expr += 2 - std::erfc(b1 / 2 / SQRTB2);
    }

    // first term
    double J1 = EXP_B0 * EXP_B12 * err_func_expr / M_2_SQRTPI / SQRTB2;
    // second term
    double J2 = EXP_B0 / 2 / b2 - B1_2_B2 * J1;
    // third term
    double J3 = -J1 / 2 / b2 - B1_2_B2 * J2;

    Matrix J(1, 4);
    J(0, 0) = J1;
    J(0, 1) = J2;
    isLeft ? J(0, 2) = J3 : J(0, 3) = J3;
    return J;
  }

  Matrix Peakmodel::calcJacobianMatrix_Area_half_Valley(
      const double b0,
      double b1,
      const double b2,
      const bool isLeft) const
  {
    // Precalculation:
    const double SQRTB2 = std::sqrt(b2);
    const double EXP_B0 = std::exp(b0);
    const double B1_2_B2 = b1 / 2 / b2;
    const double EXP_B12 = std::exp(-b1 * B1_2_B2 / 2);
    double err_func_expr = 0.;
    /* the err_func_expr depends on the side */
    if (isLeft)
    {
      err_func_expr += erfi(b1 / 2 / SQRTB2);
    }
    else
    {
      err_func_expr += -erfi(b1 / 2 / SQRTB2);
    }

    Matrix J(1, 4);
    J(0, 0) = EXP_B0 * EXP_B12 / M_2_SQRTPI / SQRTB2;
    J(0, 1) = EXP_B0 / 2 / b2 - B1_2_B2 * J(0, 0);
    if (isLeft)
    {
      J(0, 2) = -J(0, 0) / 2 / b2 - B1_2_B2 * J(0, 1);
    }
    else
    {
      J(0, 3) = -J(0, 0) / 2 / b2 - B1_2_B2 * J(0, 1);
    }
    return J;
  }

  double Peakmodel::calcPeakAreaNotCovered(
      const double b0,
      const double b1,
      const double b2,
      const double b3,
      const double left_edge,
      const double right_edge) const
  {
    /*This function calculates the Peak area NOT covered by regression window*/
    // calculate check sum to find the correct case
    int key = (b1 < 0 ? 4 : 0) + (b2 < 0 ? 2 : 0) + (b3 < 0 ? 1 : 0);
    double area_left;
    double area_right;
    double area_trapezoid;
    double x_left;
    double x_right;
    switch (key)
    {
    case 7: // Case 1a: apex left
      area_left = calcPeakArea_half(b0, b1, b2, left_edge, true, false);
      area_right = calcPeakArea_half(b0, b1, b3, right_edge, false, false);
      area_trapezoid = (right_edge - left_edge) * (std::exp(b0 + b1 * left_edge + b2 * left_edge * left_edge) + std::exp(b0 + b1 * right_edge + b3 * right_edge * right_edge)) * .5;
      break;
    case 3: // Case 1b: apex right
      area_left = calcPeakArea_half(b0, b1, b2, left_edge, true, false);
      area_right = calcPeakArea_half(b0, b1, b3, right_edge, false, false);
      area_trapezoid = (right_edge - left_edge) * (std::exp(b0 + b1 * left_edge + b2 * left_edge * left_edge) + std::exp(b0 + b1 * right_edge + b3 * right_edge * right_edge)) * .5;
      break;
    case 6: // Case 2a: apex left | valley right
      area_left = calcPeakArea_half(b0, b1, b2, left_edge, true, false);
      area_right = calcPeakArea_half(b0, b1, b3, right_edge, false, true);
      x_left = left_edge;
      x_right = right_edge;
      // check if valley is inside regression window:
      if (-b1 / 2 / b3 < right_edge)
      {
        x_right = -b1 / 2 / b3;
      }
      area_trapezoid = (x_right - x_left) * (std::exp(b0 + b1 * x_left + b2 * x_left * x_left) + std::exp(b0 + b1 * x_right + b3 * x_right * x_right)) * .5;
      break;
    case 1: // Case 2b: apex right | valley left
      area_left = calcPeakArea_half(b0, b1, b2, left_edge, true, true);
      area_right = calcPeakArea_half(b0, b1, b3, right_edge, false, false);
      x_left = left_edge;
      x_right = right_edge;
      // check if valley is inside regression window:
      if (-b1 / 2 / b2 > left_edge)
      {
        x_left = -b1 / 2 / b2;
      }
      area_trapezoid = (x_right - x_left) * (std::exp(b0 + b1 * x_left + b2 * x_left * x_left) + std::exp(b0 + b1 * x_right + b3 * x_right * x_right)) * .5;
      break;
    default:

      break;
    }
    return (area_left + area_right + area_trapezoid) / (2 * right_edge - 3); // where (2 * right_edge - 3 ) is df
  }

  void Peakmodel::mergeRegressions(
      const int N,
      bool *&fltrVec,
      const Matrix &beta,
      const std::vector<int> &scaleVec,
      const std::vector<int> &xIndices,
      const std::vector<double> &apex_position,
      std::vector<double> &apex_position_uncertainty,
      const std::vector<double> &valley_position,
      const Matrix &Y,
      const std::vector<double> mse,
      const std::vector<int> &idx1,
      std::vector<int> &idx2)
  {
    /*This function merges the peak candidates that are similar and therefore describe the same peak. To do so, a t-test based on the positions will be made and afterwards a comparison of the MSE in the exp-Domain.*/

    // new filter for the best peaks. initialized with all being false
    std::unordered_set<int> idx_ref;
    bool is_empty = true;

    // vector of position uncertainties
    double *var_apex = new double[N]();

    // create an mse vector for later comparison
    double *mse_exp = new double[N]();
    // group Vector for candidates that should be compared with the new one
    std::unordered_set<int> idx_candidates;
    size_t n_Candidates = 0;
    for (int col : idx1)
    {
      idx_candidates.clear();
      /*Here we scan through all peak candidates*/
      // first peak candidate will always be added
      if (is_empty)
      {
        // add first peak to ref list
        idx_ref.insert(col);

        Matrix J = calcJacobianMatrix_Position(beta(1, col), beta(2, col), beta(3, col));
        var_apex[col] = (J * inverseMatrices[scaleVec[col]] * J.T()).sumElements() * mse[col];
        apex_position_uncertainty[col] = std::sqrt(var_apex[col]);
        Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
        Matrix y = Y.subMatrix(xIndices[col] - scaleVec[col], xIndices[col] + scaleVec[col] + 1, 0, 1);

        for (size_t j = 0; j < y.numRows(); j++)
        {
          yhat(j, 0) = std::exp(yhat(j, 0));
        }
        mse_exp[col] = calcMse(yhat, y);
        is_empty = false;
      }
      else
      {
        // std::cout << idx_ref.size() << std::endl;
        // scan through BestPeaks and check for similarity
        n_Candidates = 0;
        for (int idx : idx_ref)
        {

          // check minimum distance
          if (std::abs(apex_position[col] - apex_position[idx]) < 5)
          {
            // these peaks cannot be distinguished and are therefore similar
            n_Candidates++;
            idx_candidates.insert(idx);
            // Check if mse was already calculated
            if (mse_exp[col] == 0)
            {
              Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
              Matrix y = Y.subMatrix(xIndices[col] - scaleVec[col], xIndices[col] + scaleVec[col] + 1, 0, 1);

              for (size_t j = 0; j < y.numRows(); j++)
              {
                yhat(j, 0) = std::exp(yhat(j, 0));
              }
              mse_exp[col] = calcMse(yhat, y);
            }
          }
          else
          {
            /*These peaks should be tested using t-Test for the peak distances. To do so, we need the uncertainties of the positions.*/
            // Check if variance was already calculated
            if (var_apex[col] == 0)
            {
              Matrix J = calcJacobianMatrix_Position(beta(1, col), beta(2, col), beta(3, col));
              var_apex[col] = (J * inverseMatrices[scaleVec[col]] * J.T()).sumElements() * mse[col];
              apex_position_uncertainty[col] = std::sqrt(var_apex[col]);
            }

            double se_pos = std::sqrt(var_apex[idx] + var_apex[col]);
            double t_meas = std::abs(apex_position[col] - apex_position[idx]) / se_pos;
            int df = 2 * (scaleVec[col] + scaleVec[idx]) - 6;
            double t_crit = tVal[df];
            if (t_meas < t_crit)
            {
              // these peaks cannot be distinguished and are therefore similar
              idx_candidates.insert(idx);
              n_Candidates++;
              // Check if mse was already calculated
              if (mse_exp[col] == 0)
              {
                Matrix yhat = designMatrices[scaleVec[col]] * beta.col(col);
                Matrix y = Y.subMatrix(xIndices[col] - scaleVec[col], xIndices[col] + scaleVec[col] + 1, 0, 1);

                for (size_t j = 0; j < y.numRows(); j++)
                {
                  yhat(j, 0) = std::exp(yhat(j, 0));
                }
                mse_exp[col] = calcMse(yhat, y);
              }
            }
          }
        }
        // check if there are candidates for mse_comparison:
        if (n_Candidates > 0)
        {
          // There are similar peaks: start survival of the fittest
          // calculate the average of the candidates' mse_exp
          double mse_exp_ref = 0;
          for (int i : idx_candidates)
          {
            mse_exp_ref += (mse_exp[i] / n_Candidates);
          }
          if (mse_exp[col] < mse_exp_ref)
          {
            // new peak is better than former candidates
            idx_ref.insert(col);
            // delete old ones from list
            for (int i : idx_candidates)
            {
              idx_ref.erase(i);
            }
          }
        }
        else
        {
          // No similar peaks: add new peak to the list
          idx_ref.insert(col);
        }
      }
    }
    for (int idx : idx_ref)
    {
      idx2.push_back(idx);
    }
    delete[] mse_exp;
    delete[] var_apex;
  }

  void Peakmodel::rescalePosition(
      const int N,
      bool *&fltrVec,
      const Matrix &x_data,
      std::vector<double> &apex_position,
      std::vector<double> &apex_positions_uncertainty,
      const std::vector<int> &idx1)
  {
    /*This function rescales the apex positions based in the original x_data. To do so, the */
    for (int col : idx1)
    {
      int apex_left = apex_position[col];
      int apex_right = apex_left + 1;
      double dx = x_data(apex_right, 0) - x_data(apex_left, 0);
      apex_position[col] = x_data(apex_left, 0) + (apex_position[col] - apex_left) * dx;
      apex_positions_uncertainty[col] *= dx;
    }
  }

  void Peakmodel::exportResults(
      const Matrix &beta,
      bool *&fltrVec,
      const std::vector<int> &xIndices,
      const int N,
      const int smplID,
      const std::vector<double> &apex_position,
      const std::vector<double> &apex_position_uncertainty,
      const std::vector<double> &peakHeight,
      const std::vector<double> &peakHeight_uncertainty,
      const std::vector<double> &peakArea,
      const std::vector<double> &peakArea_uncertainty,
      const std::vector<int> &idx1,
      int &peakID)
  {

    for (int col : idx1)
    {
      // create a temporary Peakproperties object:
      Peakproperties tmp_peak(
          beta(0, col),                                          // Regression Coefficients
          beta(1, col),                                          // Regression Coefficients
          beta(2, col),                                          // Regression Coefficients
          beta(3, col),                                          // Regression Coefficients
          peakID,                                                // peak ID
          smplID,                                                // smpl ID (running number of spectrum or chromatogram)
          apex_position[col],                                    // Peak Position
          peakHeight[col],                                       // Peak Height
          0.0,                                                   // Peak Width <--- This is not yet implemented
          peakArea[col],                                         // Peak Area
          apex_position_uncertainty[col],                        // sigma Position
          peakHeight_uncertainty[col],                           // sigma Height
          0.0,                                                   // sigma Width <--- This is not yet implemented
          peakArea_uncertainty[col],                             // sigma Area
          std::erfc(peakArea_uncertainty[col] / peakArea[col])); // DQS
      // add tmporary Peak to the List
      peakProperties.emplace(peakID, tmp_peak);
      peakID++;
    }
  }

  // debugging & printing
  std::vector<double> Peakmodel::getPeakProperties(const Peakproperties::PropertiesNames &varName) const
  {
    int N = peakProperties.size();
    std::vector<double> vals(N);
    size_t k = 0;
    for (const auto &peak : peakProperties)
    {
      vals[k] = peak.second.getProperty(varName);
      k++;
    }
    return vals;
  }

  const Peakproperties &Peakmodel::operator[](int ID) const
  {
    auto it = peakProperties.find(ID);
    if (it != peakProperties.end())
    {
      return it->second;
    }
    throw std::out_of_range("ID not found in peakProperties map");
  }

  void Peakmodel::printTValues()
  {
    for (const auto &kv : tVal)
    {
      std::cout << "Degree of Freedom: " << kv.first << ", t-Value: " << kv.second << std::endl;
    }
  }

  bool dataID::operator<(const dataID &other) const
  {
    return std::tie(sampleID, subSampleID) < std::tie(other.sampleID, other.subSampleID);
  }
}
