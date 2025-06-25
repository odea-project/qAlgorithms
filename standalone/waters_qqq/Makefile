# other/tools/Makefile

CXX      := g++
CXXFLAGS := -std=c++17 -Wall \
            -I../../include \
            -I../../external/pugixml-1.14 \
            -I../../external/StreamCraft/src
LDFLAGS  := -lz                # link against zlib

# Where to stash object files
OBJDIR   := obj

# Basename of each object (without path)
OBJS_BASENAME := \
    qalgo_waters_main.o \
    qalgorithms_input_output.o \
    StreamCraft_mzml.o \
    pugixml.o

# Prepend OBJDIR/ to each .o
OBJS     := $(patsubst %,$(OBJDIR)/%,$(OBJS_BASENAME))

TARGET   := watersQQQ.exe

.PHONY: all clean
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# Compile rules

# 1) qalgo_waters_main.cpp → obj/qalgo_waters_main.o
$(OBJDIR)/qalgo_waters_main.o: qalgo_waters_main.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 2) reader → obj/qalgorithms_input_output.o
$(OBJDIR)/qalgorithms_input_output.o: ../../src/qalgorithms_input_output.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 3) StreamCraft_mzml → obj/StreamCraft_mzml.o
$(OBJDIR)/StreamCraft_mzml.o: \
    ../../external/StreamCraft/src/StreamCraft_mzml.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4) pugixml → obj/pugixml.o
$(OBJDIR)/pugixml.o: ../../external/pugixml-1.14/src/pugixml.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure OBJDIR exists
$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
