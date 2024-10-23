FROM gcc:latest

COPY . /usr/src/qalgorithms
WORKDIR /usr/src/qalgorithms

RUN g++ -Wall -Wno-unknown-pragmas -std=c++20 -fdiagnostics-color=always -g -fopenmp -O3 -mavx2 -mfma src/qalgorithms_measurement_data_tensor.cpp src/qalgorithms_measurement_data.cpp src/qalgorithms_utils.cpp src/qalgorithms_qpeaks.cpp src/qalgorithms_qbin.cpp external/StreamCraft/src/StreamCraft_mzml.cpp external/StreamCraft/src/StreamCraft_utils.cpp src/qalgorithms_main.cpp -o test/qalgorithms_main -lz 

CMD ["./test/qalgorithms_main"]