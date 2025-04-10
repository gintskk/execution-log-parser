cmake_minimum_required(VERSION 3.10)
project(SimpleGoogleTest)

# Enable testing
enable_testing()

# Add GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Your test file
add_executable(test_execution_trace_parcing
 parser.cpp 
 tests/test_execution_trace_parcing.cpp)
add_executable(test_sequential_consistency
 parser.cpp 
 graph_checker.cpp
 tests/test_sequential_consistency.cpp)

# Link GoogleTest
target_link_libraries(test_execution_trace_parcing ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_sequential_consistency ${GTEST_LIBRARIES} pthread)

# Register the test with CTest
add_test(NAME test_execution_trace_parcing COMMAND test_execution_trace_parcing)
add_test(NAME test_sequential_consistency COMMAND test_sequential_consistency)
