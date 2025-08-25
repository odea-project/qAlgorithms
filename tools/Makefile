# build and run tests

OBJECTS = tests.o \
		  qalgorithms_input_output.o qalgorithms_metafiles.o qalgorithms_qbin.o qalgorithms_qpattern.o \
		  qalgorithms_qpeaks.o qalgorithms_read_file.o qalgorithms_utils.o

testrun: $(OBJECTS)
		 cc -o testrun $(OBJECTS)


.PHONY:  clean
clean: 
	rm testrun &(OBJECTS)