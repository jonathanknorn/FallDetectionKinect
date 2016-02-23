# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.

CXX = g++-4.9 -std=c++11
CC  = g++-4.9 -std=c++11

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
CXXFLAGS =  -g -O2 -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses -Wold-style-cast 
CXXFLAGS += -std=c++11
CXXFLAGS += -L/opt/vc/lib
CXXFLAGS += -lm -lmmal -lmmal_core  -lmmal_util
CXXFLAGS += -lopencv_core -lopencv_features2d -lopencv_highgui -lopencv_imgproc -lopencv_nonfree -lopencv_objdetect -lopencv_video -pthread
LDFLAGS =   -g 
#CPPFLAGS =  -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets
PROGS = main background_subtraction

all: $(PROGS)

# Targets rely on implicit rules for compiling and linking
main: floor_detector.o main.o utilities.o foreground_extractor.o logger.o
	$(CXX) $(CXXFLAGS) floor_detector.o main.o utilities.o logger.o foreground_extractor.o  -o main

background_subtraction: floor_detector.o background_subtraction.o logger.o foreground_extractor.o utilities.o
	$(CXX) $(CXXFLAGS) floor_detector.o background_subtraction.o logger.o foreground_extractor.o utilities.o -o background_subtraction

# Phony targets
.PHONY: all clean

# Standard clean
clean:
	rm -f *.o *.d $(PROGS)

# Generate dependencies in *.d files
%.d: %.cc
	@set -e; rm -f $@; \
         $(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
         sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
         rm -f $@.$$$$

# Include the *.d files
SRC = $(wildcard *.cc)
include $(SRC:.cc=.d)
