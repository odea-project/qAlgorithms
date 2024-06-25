VPATH=./src:./include/:./external/StreamCraft/src/:./external/pugixml-1.14/src/
OBJECTS=qalgorithms_utils.o qalgorithms_matrix.o qalgorithms_qbin.o qalgorithms_main.o \
		qalgorithms_qpeaks.o qalgorithms_datatype_peak.o qalgorithms_measurement_data.o \
		qalgorithms_datatype_mass_spectrum.o qalgorithms_measurement_data_lcms.o \
		StreamCraft_mzml.o StreamCraft_utils.o pugixml.o
INCLUDES = -I ./include -I ./external/StreamCraft/src/ -I ./external/pugixml-1.14/src/
CPPFLAGS=-Wall ${INCLUDES}
CC=g++

all: qbin

qbin: ${OBJECTS}
	g++ ${CPPFLAGS} -o output ${OBJECTS}

qalgorithms_utils.o: qalgorithms_utils.h qalgorithms_matrix.h
qalgorithms_matrix.o: qalgorithms_matrix.h
qalgorithms_qbin.o: qalgorithms_qbin.h
qalgorithms_main.o: qalgorithms_measurement_data_lcms.h qalgorithms_qpeaks.h StreamCraft_mzml.h
qalgorithms_qpeaks.o: qalgorithms_measurement_data.h StreamCraft_mzml.h
qalgorithms_datatype_peak.o: qalgorithms_datatype_peak.h
qalgorithms_measurement_data.o: qalgorithms_datatype_mass_spectrum.h
qalgorithms_datatype_mass_spectrum.o: qalgorithms_datatype.h
qalgorithms_measurement_data_lcms.o: qalgorithms_measurement_data_lcms.h

StreamCraft_mzml.o: pugixml.h StreamCraft_utils.h
StreamCraft_utils.o: 

pugixml.o: pugiconfig.h


test: qalgorithms_qpeaks.o
	echo "works"

clean: 
	rm -f ${OBJECTS}
	rm -f output

.PHONY: clean all

