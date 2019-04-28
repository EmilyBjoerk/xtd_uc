cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")

set(CXXSTANDARD "-std=c++14")
set(COPT        "-O0 -ggdb3 -pthread")
set(CWARN       "-Werror -Wall -Wextra -pedantic -Wno-attributes")
set(CTUNING     "-ftrapv -fsanitize=undefined -fsanitize=unsigned-integer-overflow -fsanitize=implicit-conversion -fprofile-instr-generate -fcoverage-mapping")
set(CFAKE_AVR   "-D__AVR__=1 -D__AVR_MEGA__ -DF_CPU=16000000UL -DENABLE_TEST -DHAS_STL")
set(CDEFS       "-DUART_BAUD=9600 -DUART_DATA_BITS=8 -DUART_PARITY_BITS=0 -DUART_STOP_BITS=1")

project(unit-tests C CXX)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXSTANDARD} ${CFAKE_AVR} ${COPT} ${CWARN} ${CTUNING} ${CDEFS}")

get_filename_component(BASE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/" ABSOLUTE)
set(INC_PATH     "${BASE_PATH}/include")
set(TEST_PATH     "${BASE_PATH}/test")
set(SRC_PATH     "${BASE_PATH}/src")
file(GLOB TEST_FILES "${TEST_PATH}/*.cpp")

set(SRC_FILES ${SRC_PATH}/delay.cpp)

enable_testing()
find_package(GTest REQUIRED)
include(FindGMock)

include_directories(${INC_PATH} ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${TEST_FILES} ${SRC_FILES})
target_link_libraries(unit-tests ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} -lpthread)
add_test(unit-tests unit-tests)
