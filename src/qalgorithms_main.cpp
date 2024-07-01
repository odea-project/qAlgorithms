// internal
#include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_matrix.h"

// external
#include "StreamCraft_mzml.h" //@todo check if this is actually used here
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>
#include <sstream>
#include <filesystem> // printing absolute path in case read fails

int main()
{
    bool silent = false;

    q::vectorDestructions = 0;
    q::matrixDestructions = 0;

    std::streambuf *old = std::cout.rdbuf(); // save standard out config
    std::stringstream ss;

    if (silent) // redirect standard out to ss
    {
        std::cout.rdbuf(ss.rdbuf());
    }
    std::cout << "starting...\n";

    auto timeStart = std::chrono::high_resolution_clock::now();
    // print logo - @todo don't use functions which are incompatible with linux
    //   HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //   q::logo::print_qpeaks();

    // ### removed user input (make this an argument of main) and overzealous status updates

    // initialize qPeaks static variables
    q::Algorithms::qPeaks::initialize(); // @todo constexpr
    bool isCSV = false;

    std::string filename_input = "../../rawdata/example_profile.mzML"; // @todo make this a command line argument
    // "../../rawdata/example_profile.mzML"
    // "../rawdata/example_profile.mzML"
    // "../test/test_orbitrap.csv"
    // "../qAlgorithms/test/test_orbitrap.csv"

    std::filesystem::path p = filename_input;
    if (!std::filesystem::exists(p))
    {
        std::cout << "Error: The selected file does not exist.\nSupplied path: " << std::filesystem::absolute(p)
                  << "\nCurrent directory: " << std::filesystem::current_path() << "\n\nTerminated Program.\n\n";
        exit(101);
    }

    std::cout << "Executing qAlgorithms on " << std::filesystem::canonical(p) << '\n';

    q::MeasurementData::LCMSData lcmsData;

    bool fileOK = false;

    // check if the input file is a CSV file or a mzML file
    if (filename_input.find(".mzML") != std::string::npos) // @todo make sure this is the end of the filename, switch to regex
    {
        sc::MZML data(filename_input);
        fileOK = lcmsData.readStreamCraftMZML(data);
    }
    else if (filename_input.find(".csv") != std::string::npos) // @todo make sure this is the end of the filename, switch to regex
    {
        fileOK = lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',',
                                  {q::DataType::DataField::MZ,
                                   q::DataType::DataField::INTENSITY,
                                   q::DataType::DataField::SCANNUMBER,
                                   q::DataType::DataField::RETENTIONTIME}); // @todo make the cols user defined and offer these as default values
        isCSV = true;
    }
    else
    {
        std::cout << "Error: only .mzML and .csv files are supported" << std::endl;
    }

    if (!fileOK)
    { // terminate program if readCSV failed
        exit(101);
    }

    auto timeEnd = std::chrono::high_resolution_clock::now();

    size_t totalScans = lcmsData.numDatasets(); // needed to construct binning object

    std::cout << "finished reading " << totalScans << " mass spectra in " << (timeEnd - timeStart).count() << " ns\n\n";

    // @todo add timings here, include option to skip output or create .log for more elaborate diagnostics
    timeStart = std::chrono::high_resolution_clock::now();

    lcmsData.zeroFilling();

    lcmsData.cutData();

    lcmsData.filterSmallDataSets();

    lcmsData.interpolateData();

    timeEnd = std::chrono::high_resolution_clock::now();
    std::cout << "reduced data to " << lcmsData.numDatasets() << " datasets in " << (timeEnd - timeStart).count() << " ns\n\n";

    // qPEAKS
    timeStart = std::chrono::high_resolution_clock::now();
    std::cout << "finding peaks...\n";

    q::MeasurementData::varDataType dataObject = &(lcmsData.data);
    q::Algorithms::qPeaks qpeaks(dataObject);

    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> temp_peaks = qpeaks.findPeaks(dataObject);

    q::qBinning::CentroidedData testdata = qpeaks.passToBinning(temp_peaks, totalScans);

    // std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks = qpeaks.createPeakList(temp_peaks); // @todo check for better data structures

    // size_t count = 0;
    // for (size_t i = 0; i < peaks.size(); i++)
    // {
    //     count += peaks[i].size();
    // }
    timeEnd = std::chrono::high_resolution_clock::now();
    std::cout << "found " << testdata.lengthAllPoints << " peaks in " << (timeEnd - timeStart).count()
              << " ns\n\nstarting qbinning submodule...\n\n";

    std::cout << q::vectorDestructions << " , " << q::matrixDestructions << "\n";

    std::vector<q::qBinning::EIC> binnedData = q::qBinning::performQbinning(testdata, 6, false);

    // write peaks to file @todo output location should always be determined by the user!
    // std::string filename_output = filename_input;
    // // remove the file extension
    // filename_output = filename_output.substr(0, filename_output.find_last_of("."));
    // filename_output += "_peaks.csv";
    std::string filename_output = "../../peaktable.csv";
    // qpeaks.printAllPeaks(peaks, filename_output);

    std::cout << "done\n";

    std::cout.rdbuf(old); // restore previous standard out

    return 0;
}
