// internal
#include "qalgorithms_measurement_data_lcms.h"
#include "qalgorithms_qpeaks.h"
#include "qalgorithms_matrix.h"
#include "qalgorithms_qbin.h"

// external
#include "StreamCraft_mzml.h" //@todo check if this is actually used here
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>
#include <sstream>
#include <filesystem> // printing absolute path in case read fails
#include <fstream>

namespace q
{
    struct positionDataMassSpectra
    {
        std::vector<double> retentionTimes;
        std::vector<int> absoluteScans;
    };
}

int main()
{
    bool silent = false;

    std::streambuf *old = std::cout.rdbuf(); // save standard out config
    std::stringstream ss;

    if (silent) // redirect standard out to ss
    {
        std::cout.rdbuf(ss.rdbuf());
    }
    std::cout << "starting...\n";

    // @todo add option to take in a text file containing execution parameters and a list of target files
    std::vector<std::string> target_files_full{
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_Blank_SW_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_C1_S1_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_C2_S1_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_C3_S1_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_T1_S1_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210229_T2_S1_I_1_pos.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210311_C1_S5_I_1.mzML",
        "G:/Messdaten/AquaFlow_Rohdaten_SurfaceWater/210311_C2_S5_I_1.mzML"
        // "G:/Messdaten/LC_orbitrap_kali/22090917_Kali_Pu_Penc_2,9HCO3_0,2_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090917_Kali_Pu_Penc_2,9HCO3_0,2_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090918_Kali_Pu_Penc_2,9HCO3_0,5_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090918_Kali_Pu_Penc_2,9HCO3_0,5_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090919_Kali_Pu_Penc_2,9HCO3_1_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090919_Kali_Pu_Penc_2,9HCO3_1_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090920_Kali_Pu_Penc_2,9HCO3_2_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090920_Kali_Pu_Penc_2,9HCO3_2_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090921_H2O_3_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090935_H2O_4_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090901_H2O_1_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090902_Kali_Pu_Penc_2,9HCO3_0_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090903_Kali_Pu_Penc_2,9HCO3_0,01_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090904_Kali_Pu_Penc_2,9HCO3_0,02_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090905_Kali_Pu_Penc_2,9HCO3_0,05_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090906_Kali_Pu_Penc_2,9HCO3_0,1_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090907_Kali_Pu_Penc_2,9HCO3_0,2_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090908_Kali_Pu_Penc_2,9HCO3_0,5_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090909_Kali_Pu_Penc_2,9HCO3_1_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090910_Kali_Pu_Penc_2,9HCO3_2_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090911_H2O_2_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090912_Kali_Pu_Penc_2,9HCO3_0_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090912_Kali_Pu_Penc_2,9HCO3_0_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090913_Kali_Pu_Penc_2,9HCO3_0,01_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090913_Kali_Pu_Penc_2,9HCO3_0,01_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090914_Kali_Pu_Penc_2,9HCO3_0,02_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090914_Kali_Pu_Penc_2,9HCO3_0,02_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090915_Kali_Pu_Penc_2,9HCO3_0,05_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090915_Kali_Pu_Penc_2,9HCO3_0,05_Indigo_pos.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090916_Kali_Pu_Penc_2,9HCO3_0,1_Indigo_neg.mzML",
        // "G:/Messdaten/LC_orbitrap_kali/22090916_Kali_Pu_Penc_2,9HCO3_0,1_Indigo_pos.mzML",

        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090916_Kali_Pu_Penc_2,9HCO3_0,1_Indigo_neg.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090913_Kali_Pu_Penc_2,9HCO3_0,01_Indigo_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090912_Kali_Pu_Penc_2,9HCO3_0_Indigo_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090910_Kali_Pu_Penc_2,9HCO3_2_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090903_Kali_Pu_Penc_2,9HCO3_0,01_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090902_Kali_Pu_Penc_2,9HCO3_0_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090901_H2O_1_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090921_H2O_3_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090935_H2O_4_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090920_Kali_Pu_Penc_2,9HCO3_2_Indigo_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/LCMS_pressure_error/22090920_Kali_Pu_Penc_2,9HCO3_2_Indigo_neg.mzML"
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO02_06.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO03_08.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO11_24.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO24_50.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO29_60.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/SFC_DoE/240123_VqF_DoE-pos_RO42_86.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_1_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_2_pos.mzML", // some error during binning here?
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_C1_S1_W_MI_3_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_1_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_2_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank_SW_MI_I_3_pos.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank1.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank2.mzML",
        // "C:/Users/unisys/Documents/Studium/Messdaten/mzml/AquaFlow_MS1/210229_Blank3.mzML",
        // "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_02_21.mzML",
        // "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_03_22.mzML",
        // "C:/Users/unisys/Documents/Studium/Analytik-Praktikum/rawdata/Exchange_SFC-data/230222_SFC_Modelling_MixF_01_20.mzML",
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

        q::positionDataMassSpectra indexDict;

        q::MeasurementData::LCMSData lcmsData;

        bool fileOK = false;

        // check if the input file is a CSV file or a mzML file
        // @todo solve this by first extracting all characters from the last dot to the
        // end of the string and then implement method selection using a switch statement
        // char filetype[16] = p.extension();

        if (filename_input.find(".mzML") != std::string::npos) // @todo make sure this is the end of the filename, switch to regex
        {
            sc::MZML data(filename_input);
            fileOK = lcmsData.readStreamCraftMZML(data, indexDict.retentionTimes, indexDict.absoluteScans);
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
