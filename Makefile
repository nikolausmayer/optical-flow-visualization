##
#
# Author: Nikolaus Mayer, 2016-2019 (mayern@cs.uni-freiburg.de)
#
##

## Compiler
CXX ?= gcc

## Compiler flags; extended in 'debug'/'release' rules
CXXFLAGS += -fPIC -W -Wall -Wextra -Wpedantic -std=c++11 -pthread -isystem src/agg-2.4/include -Dcimg_use_png -Dcimg_display=0

## Linker flags
LDFLAGS = -Lsrc/agg-2.4/src -pthread -lagg -lpng

## Default name for the built executable
TARGET = visualize

## Every *.c/*.cc/*.cpp file is a source file
SOURCES = $(wildcard src/*.c src/*.cc src/*.cpp)
## Every *.h/*.hh/*.hpp file is a header file
HEADERS = $(wildcard *.h *.hh *.hpp)

## Build a *.o object file for every source file
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

## External dependencies to be downloaded and built
OTHERS = src/CImg.h src/agg-2.4/src/libagg.a


## Tell the 'make' program that e.g. 'make clean' is not supposed to 
## create a file 'clean'
##
## "Why is it called 'phony'?" -- because it's not a real target. That 
## is, the target name isn't a file that is produced by the commands 
## of that target.
.PHONY: all clean purge debug release 


## Default is release build mode
all: release
	
## When in debug mode, don't optimize, and create debug symbols
debug: CXXFLAGS += -O0 -g -D_DEBUG
debug: BUILDMODE ?= DEBUG
debug: $(TARGET)
	
## When in release mode, optimize
release: CXXFLAGS += -O2 -DNDEBUG
release: BUILDMODE ?= RELEASE
release: $(TARGET)

## Remove built object files and the main executable
clean:
	$(info ... deleting built object files and executable  ...)
	-rm src/*.o $(TARGET)

purge:
	$(info ... deleting all downloaded, built, and generated files ...)
	-rm -r src/*.o $(TARGET) agg-2.4.tar.gz CImg-v.2.4.5 v.2.4.5.tar.gz src/agg-2.4 src/CImg.h
	-rm frame*.ppm frame*.gif


src/CImg.h:
	$(info ... downloading CImg ...)
	test -f v.2.4.5.tar.gz || wget https://github.com/dtschump/CImg/archive/v.2.4.5.tar.gz
	test -d CImg-v.2.4.5 || tar xfz v.2.4.5.tar.gz
	test -f src/CImg.h || cp CImg-v.2.4.5/CImg.h src/

src/agg-2.4/src/libagg.a:
	$(info ... downloading and building AGG ...)
	test -f agg-2.4.tar.gz || wget https://github.com/nikolausmayer/AntiGrainGeometry-v2.4/raw/master/agg-2.4.tar.gz
	test -d agg-2.4 || tar xfz agg-2.4.tar.gz
	test -d src/agg-2.4 || mv agg-2.4 src/
	test -f src/agg-2.4/src/libagg.a || ( cd src/agg-2.4 && patch include/agg_scanline_u.h < ../AntiGrainGeometry-agg_scanline_u-patch.txt && make )

## The main executable depends on all object files of all source files
$(TARGET): $(OBJECTS) Makefile $(OTHERS)
	$(info ... linking $@ ...)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

## Every object file depends on its source. It may also depend on
## potentially all header files, and of course the makefile itself.
%.o: %.c Makefile $(HEADERS) $(OTHERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: %.cc Makefile $(HEADERS) $(OTHERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: %.cpp Makefile $(HEADERS) $(OTHERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@


