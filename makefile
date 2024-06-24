# C:\'Program Files (x86)'\GnuWin32\bin\make.exe

all: qalgorithms_main

qalgorithms_main: qalgorithms_main.cpp qalgorithms_measurement_data_lcms.o qalgorithms_qpeaks.o qalgorithms_qbin.o
	g++ -o qalgorithms_main qalgorithms_measurement_data_lcms.o qalgorithms_qpeaks.o qalgorithms_qbin.o

qalgorithms_measurement_data_lcms.o: qalgorithms_measurement_data_lcms.cpp qalgorithms_measurement_data_lcms.h qalgorithms_measurement_data.o

qalgorithms_qpeaks.o

qalgorithms_qbin.o

qalgorithms_measurement_data.o