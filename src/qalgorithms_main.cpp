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
#include <fstream>

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

    // @todo add option to take in a text file containing execution parameters and a list of target files
    std::vector<std::string> target_files_full{
        "G:/_Studium/Analytik-Praktikum/qbinning/rawdata/210229_C1_S1_W_MI_1_pos.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_1_pos.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_2_pos.mzML", // some error during binning here?
        "nonsense",

        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_3_pos.mzML",
        "nonsense", // @todo add bypass if one of the files is bad
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_1_pos.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_2_pos.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_3_pos.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank1.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank2.mzML",
        "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank3.mzML",
        "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_02_21.mzML",
        "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_03_22.mzML",
        "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_01_20.mzML",
    };

    for (std::string currentFile : target_files_full)
    {
        auto timeStart = std::chrono::high_resolution_clock::now();

        // ### removed user input (make this an argument of main) and overzealous status updates

        // initialize qPeaks static variables
        q::Algorithms::qPeaks::initialize(); // @todo constexpr
        bool isCSV = false;

        std::string filename_input = currentFile; // "../../rawdata/example_profile.mzML"; // @todo make this a command line argument
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
        // @todo solve this by first extracting all characters from the last dot to the
        // end of the string and then implement method selection using a switch statement
        // char filetype[16] = p.extension();

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

        std::cout << "zero filling ... ";
        lcmsData.zeroFilling();

        std::cout << "cutting data ... ";
        lcmsData.cutData(); // @todo integrate any and all filtering during this step

        std::cout << "filtering ... ";
        lcmsData.filterSmallDataSets();

        std::cout << "interpolating ... \n";
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

        timeEnd = std::chrono::high_resolution_clock::now();
        std::cout << "found " << testdata.lengthAllPoints << " peaks in " << (timeEnd - timeStart).count()
                  << " ns\n\nstarting qbinning submodule...\n\n";

        // write peaks to file @todo output location should always be determined by the user!
        // std::string filename_output = filename_input;
        // // remove the file extension
        // filename_output = filename_output.substr(0, filename_output.find_last_of("."));
        // filename_output += "_peaks.csv";
        // std::string filename_output = "../../peaktable_full2_5.csv";
        // qpeaks.printAllPeaks(peaks, filename_output);

        // std::cout << q::vectorDestructions << " , " << q::matrixDestructions << "\n";

        size_t found = filename_input.find_last_of(".");
        std::string summary_output_location = filename_input.substr(0, found);

        // print faulty dataset
        // std::string binsFull = summary_output_location + "_centroids.csv";
        // std::fstream file_out_all;
        // std::stringstream output_all;
        // file_out_all.open(binsFull, std::ios::out);
        // assert(file_out_all.is_open());
        // output_all << "ID,mz,scan,intensity,mzError,DQSC,DQSB_base,DQSB_scaled\n";
        // for (size_t i = 0; i < testdata.allDatapoints.size(); i++)
        // {

        //     std::vector<q::qBinning::qCentroid> binnedPoints = testdata.allDatapoints[i];

        //     for (size_t j = 0; j < binnedPoints.size(); j++)
        //     {
        //         char buffer[128];
        //         // scan position, mz, centroid error, scan number, intensity, DQS centroid
        //         sprintf(buffer, "%u,%0.15f,%0.15f,%d,%0.15f,%0.15f\n",
        //                 i, binnedPoints[j].mz, binnedPoints[j].mzError,
        //                 binnedPoints[j].scanNo, binnedPoints[j].intensity, binnedPoints[j].DQScentroid);
        //         output_all << buffer;
        //     }
        // }
        // file_out_all << output_all.str();
        // file_out_all.close();

        std::vector<q::qBinning::EIC> binnedData = q::qBinning::performQbinning(testdata, summary_output_location, 6, false);

        std::cout << "done\n";

        std::cout.rdbuf(old); // restore previous standard out
    }
    return 0;
}
