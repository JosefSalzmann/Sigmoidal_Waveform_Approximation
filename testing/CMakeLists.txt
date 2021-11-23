cmake_minimum_required(VERSION 2.6)
project(runTests)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "../src/tests/circuit_file_parser_tests.cpp" "../src/tests/unit_tests.cpp")
add_executable(runTests ${SOURCES})
 
# Link runTests with what we want to test and the GTest and pthread library
# add_executable(runTests "../src/unit_tests.cpp" "../src/unit_test2.cpp")
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)


