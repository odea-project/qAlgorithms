# C:\'Program Files (x86)'\GnuWin32\bin\make.exe
# g++ -I .\qAlgorithms\include\:.\qAlgorithms\external\pugixml-1.14\src\:.\qAlgorithms\external\StreamCraft\src -MM .\qAlgorithms\src\qalgorithms_measurement_data_lcms.cpp

VPATH = src:include:external/pugixml-1.14/src:external/StreamCraft/src

CC = g++
FLAGS = -g -Wall
OBJECTS = qalgorithms_main.o qalgorithms_measurement_data_lcms.o qalgorithms_qpeaks.o qalgorithms_qbin.o

all: qalgorithms_main

qalgorithms_main: ${OBJECTS}
	${CC} -o qalgorithms_main ${OBJECTS}

qalgorithms_measurement_data_lcms.o: qalgorithms_measurement_data_lcms.cpp qalgorithms_measurement_data_lcms.h qalgorithms_measurement_data.h \
qalgorithms_datatype_mass_spectrum.h qalgorithms_datatype.h 

qalgorithms_qpeaks.o:

qalgorithms_qbin.o:

qalgorithms_measurement_data.o: