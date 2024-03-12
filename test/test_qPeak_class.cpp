// test_qPeak_class.cpp

// internal
#include "../include/qalgorithms_measurement_data_lcms.h"

// external
#include <iostream>

int main() {
    q::LCMSData lcmsData;
    lcmsData.readCSV("test/data/test_lcms_data.csv", 1, -1, 0, -1, ',', {q::DataType::DataField::SCANNUMBER, q::DataType::DataField::RETENTIONTIME, q::DataType::DataField::MZ, q::DataType::DataField::INTENSITY});

    lcmsData.print();

  return 0;
}
