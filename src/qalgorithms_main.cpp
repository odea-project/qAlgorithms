// internal
#include "../include/qalgorithms_measurement_data_lcms.h"
#include "../include/qalgorithms_qpeaks.h"
#include "../include/qalgorithms_qbin.h"

// external
// #include "../external/StreamCraft/src/StreamCraft_mzml.hpp" @todo check if this is actually used here
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <iomanip>

int main()
{
    std::cout << "starting...";
    auto timeStart = std::chrono::high_resolution_clock::now();
    // print logo - @todo don't use functions which are incompatible with linux
    //   HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //   q::logo::print_qpeaks();

    // ### removed user input (make this an argument of main) and overzealous status updates

    // initialize qPeaks static variables
    q::Algorithms::qPeaks::initialize(); // @todo constexpr
    bool isCSV = false;

    std::string filename_input = "../test/test_orbitrap.csv"; // @todo make this a command line argument

    q::MeasurementData::LCMSData lcmsData;

    // check if the input file is a CSV file or a mzML file
    if (filename_input.find(".mzML") != std::string::npos) // @todo make sure this is the end of the filename, switch to regex
    {
        sc::MZML data(filename_input);
        lcmsData.readStreamCraftMZML(data);
    }
    else if (filename_input.find(".csv") != std::string::npos) // @todo make sure this is the end of the filename, switch to regex
    {
        lcmsData.readCSV(filename_input, 1, -1, 0, -1, ',',
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

    auto timeEnd = std::chrono::high_resolution_clock::now();

    std::cout << "finished reading " << lcmsData.numDatasets() << " datasets in " << (timeEnd - timeStart).count() << " ns\n\n";

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

    std::vector<std::vector<std::unique_ptr<q::DataType::Peak>>> peaks = qpeaks.findPeaks(dataObject); // @todo check for better data structures

    size_t count = 0;
    for (size_t i = 0; i < peaks.size(); i++)
    {
        count += peaks[i].size();
    }
    timeEnd = std::chrono::high_resolution_clock::now();
    std::cout << "found " << count << " peaks in " << (timeEnd - timeStart).count() << " ns\n\n";

    // write peaks to file @todo output location should always be determined by the user!
    std::cout << "writing peaks to file...\n";
    // std::string filename_output = filename_input;
    // // remove the file extension
    // filename_output = filename_output.substr(0, filename_output.find_last_of("."));
    // filename_output += "_peaks.csv";
    std::string filename_output = "../../peaktable.csv";
    qpeaks.printAllPeaks(peaks, filename_output);

    std::cout << "done\n\n\n";

    // ### Create object to hand into qbins here ### @todo

    // plot peaks @todo make this a function argument or move it to a separate data analysis thing
    //   if (isCSV)
    //   {
    //     // ask if the user wants to plot the peaks of the first dataset
    //     std::string plot;
    //     std::cout << "Do you want to plot the peaks? (y/n): ";
    //     std::cin >> plot;
    //     // check if the answer is valid
    //     while (plot != "y" && plot != "n")
    //     {
    //       std::cout << "Error: invalid input" << std::endl;
    //       std::cout << "Do you want to plot the peaks? (y/n): ";
    //       std::cin >> plot;
    //     }
    //     if (plot == "y")
    //     {
    //       std::cout << "plot peaks........................";
    //       qpeaks.plotPeaksToPython(
    //           filename_input,  // path to the original data
    //           filename_output, // path to the peak list
    //           true,            // plot fits as well; if set to false, only original peaks and vertical lines for peak positions are plotted
    //           false);          // plot the peaks as a scatter plot; if set to true, sampleID vs. position is plotted
    //       SetConsoleTextAttribute(hConsole, 10);
    //       std::cout << "done\n";
    //       SetConsoleTextAttribute(hConsole, 15);
    //     }
    //   }
    //   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //   std::cout << "Press any key to exit" << std::endl;
    //   std::cin.get();

    return 0;
}