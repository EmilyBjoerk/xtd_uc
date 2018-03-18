TARGET = xtd_uc_test
LIBS = 
CXX = avr-g++
AVR_FLAGS = -DF_CPU=8000000 -mmcu=atmega328p
CFLAGS = --std=c++14 -g -Wall -Wextra -pedantic -pipe -Iinclude/ -fno-PIC -fno-PIE -fno-threadsafe-statics -flto -fuse-linker-plugin -fwhole-program -fpack-struct -fshort-enums -ffunction-sections $(AVR_FLAGS)
LDFLAGS = -Wall -Wl,-u,vfscanf -lscanf_flt -lm -fno-PIC -fno-pic -fno-PIE -fno-pie $(AVR_FLAGS)
.PHONY: default all clean

default: $(TARGET)
all: default test

SRCS = $(wildcard src/*.cpp) $(wildcard test/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
HEADERS = $(wildcard include/*.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET)

defines:
	$(CXX) $(CFLAGS) -dM -E - < /dev/null
