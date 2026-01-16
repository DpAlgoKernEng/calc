# Integration tests CMakeLists.txt for calc

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect integration test sources
set(INTEGRATION_TEST_SOURCES
    evaluator_integration_test.cpp
    standard_mode_integration_test.cpp
    scientific_mode_integration_test.cpp
    programmer_mode_integration_test.cpp
)

# Create integration test executable
add_executable(calc_integration_tests ${INTEGRATION_TEST_SOURCES})

# Link Google Test
target_link_libraries(calc_integration_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        calc_utils
        calc_core
        calc_modes
)

# Create alias for tests executable
add_executable(calc::integration_tests ALIAS calc_integration_tests)

# Register tests with CTest
add_test(NAME integration_tests COMMAND calc_integration_tests)

# Set test properties
set_tests_properties(integration_tests PROPERTIES
    LABELS "integration"
    TIMEOUT 30
)
