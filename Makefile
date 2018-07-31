# There are a few different purposes for this Makefile
# 1. Test what can be tested on PC with unit testing
# 2. Make sure that the code compiles on the intended targets

# Stuff common to all platforms
CXXFLAGS=-std=c++14 -g -Wall -Wextra -pedantic -Werror -Iinclude/
CXXFLAGS_AVR=-Os -fno-threadsafe-statics -fuse-linker-plugin -fpack-struct -fshort-enums -ffunction-sections
CXXFLAGS_AVR_FLTO=-flto -fwhole-program
CXX_AVR=avr-g++ $(CXXFLAGS) $(CXXFLAGS_AVR) -DF_CPU=1000000UL
CXX_HOST=clang++ $(CXXFLAGS) -O0 -g3 -pipe
CXX_AVR_ATTINY=$(CXX_AVR) -mmcu=attiny85
CXX_AVR_ATMEGA=$(CXX_AVR) -mmcu=atmega328p
LDFLAGS=-Wall
LDFLAGS_AVR=$(LDFLAGS) -Wl,-u,vfscanf -lscanf_flt -lm
HEADERS=$(wildcard include/xtd_uc/*.hpp)

# Host unit tests
GOOGLE_TEST_LIB=gtest
GOOGLE_TEST_INCLUDE=/usr/local/include
HOST_TEST=unit_tests
HOST_TEST_CXX=$(CXX_HOST) -I$(GOOGLE_TEST_INCLUDE) -DF_CPU=1000000 -DENABLE_TEST -ftrapv -fsanitize=undefined -fprofile-instr-generate -fcoverage-mapping
HOST_TEST_LDFLAGS=$(LDFLAGS) -lm -l$(GOOGLE_TEST_LIB)
HOST_TEST_OBJ_DIR=build/host
HOST_TEST_SOURCES=$(wildcard test/*.cpp)
HOST_TEST_OBJECTS=$(addprefix $(HOST_TEST_OBJ_DIR)/, $(HOST_TEST_SOURCES:.cpp=.o))

$(HOST_TEST): $(HOST_TEST_OBJECTS)
	$(HOST_TEST_CXX) $(HOST_TEST_LDFLAGS) $(HOST_TEST_OBJECTS) -o $@

$(HOST_TEST_OBJ_DIR)/%.o: %.cpp $(HEADERS) | build_dir
	$(HOST_TEST_CXX) -c $< -o $@

# ATTiny85
ATTINY=attiny.bin
ATTINY_SOURCES=$(wildcard src/*.cpp) $(wildcard src/attiny/*.cpp)
ATTINY_OBJ_DIR=build/attiny
ATTINY_OBJECTS=$(addprefix $(ATTINY_OBJ_DIR)/, $(ATTINY_SOURCES:.cpp=.o))
ATTINY_ASSEMBLY=$(ATTINY_OBJECTS:.o=.s)

$(ATTINY): $(ATTINY_OBJECTS)
	$(CXX_AVR_ATTINY) $(CXXFLAGS_AVR_FLTO) $(LDFLAGS_AVR) $(ATTINY_OBJECTS) -o $@

$(ATTINY_OBJ_DIR)/%.o: %.cpp $(HEADERS) | build_dir
	$(CXX_AVR_ATTINY) $(CXXFLAGS_AVR_FLTO) -c $< -o $@

$(ATTINY_OBJ_DIR)/%.s: %.cpp
	$(CXX_AVR_ATTINY) -S -fverbose-asm -c $< -o $@

asm-attiny: $(ATTINY_ASSEMBLY)

# ATMega328p
ATMEGA=atmega.bin
ATMEGA_SOURCES=$(wildcard src/*.cpp) $(wildcard src/atmega/*.cpp)
ATMEGA_OBJ_DIR=build/atmega
ATMEGA_OBJECTS=$(addprefix $(ATMEGA_OBJ_DIR)/, $(ATMEGA_SOURCES:.cpp=.o))
ATMEGA_ASSEMBLY=$(ATMEGA_OBJECTS:.o=.s)

$(ATMEGA): $(ATMEGA_OBJECTS)
	$(CXX_AVR_ATMEGA) $(CXXFLAGS_AVR_FLTO) $(LDFLAGS_AVR) $(ATMEGA_OBJECTS) -o $@

$(ATMEGA_OBJ_DIR)/%.o: %.cpp $(HEADERS) | build_dir
	$(CXX_AVR_ATMEGA) $(CXXFLAGS_AVR_FLTO) -c $< -o $@

$(ATMEGA_OBJ_DIR)/%.s: %.cpp
	$(CXX_AVR_ATMEGA) -S -fverbose-asm -c $< -o $@

asm-atmega: $(ATMEGA_ASSEMBLY)

###
.PHONY: default all test clean build_dir
default: all
all: test

test: $(HOST_TEST) $(ATTINY) $(ATMEGA)
	./$(HOST_TEST)

build_dir:
	@mkdir -p $(HOST_TEST_OBJ_DIR)/{test,src}
	@mkdir -p $(ATTINY_OBJ_DIR)/src/attiny
	@mkdir -p $(ATMEGA_OBJ_DIR)/src/atmega

clean:
	-rm -rf build/
	-rm -f $(HOST_TEST)
	-rm -f $(ATTINY)
	-rm -f $(ATMEGA)

show-defines:
	$(CXX) $(CFLAGS) -dM -E - < /dev/null

show-flags:
	$(CXX) $(CFLAGS) -Q -v -E - < /dev/null
