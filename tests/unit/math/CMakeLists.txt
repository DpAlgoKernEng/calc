# Math unit tests CMakeLists.txt for calc
# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect math test sources
set(MATH_TEST_SOURCES
    converter_test.cpp
)

# Create math test executable
add_executable(calc_math_tests ${MATH_TEST_SOURCES})

# Link Google Test
target_link_libraries(calc_math_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        calc_core
        calc_math
        calc_utils
)

# Register tests with CTest
add_test(NAME math_tests COMMAND calc_math_tests)

# Set test properties
set_tests_properties(math_tests PROPERTIES
    LABELS "math"
    TIMEOUT 30
)
