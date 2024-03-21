// test_qPeak_class.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"

// external
#include <iostream>
#include <chrono>

int main() {
    std::cout << "create LCMSData object....";
    q::LCMSData lcmsData;
    std::cout << "done\n";
    std::cout << "read csv....";
    lcmsData.readCSV("test/data/test_lcms_data.csv", 1, -1, 0, -1, ',', {q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME, q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY});
    // lcmsData.readCSV("test/data/test_lcms_data2.csv",0,-1,0,-1,';',{q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY, q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME});
    std::cout << "done\n";

    // Capture the start time
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "zero filling....";
    // lcmsData.zeroFilling();
    std::cout << "done\n";
    // std::cout << "cutting data....";
    // lcmsData.cutData();
    // std::cout << "done\n";
    // std::cout << "interpolating data....";
    // lcmsData.interpolateData();
    // std::cout << "done\n";
    
    // Capture the end time
    auto finish = std::chrono::high_resolution_clock::now();

    lcmsData.print();
    // Calculate and print the elapsed time
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";


  return 0;
}
