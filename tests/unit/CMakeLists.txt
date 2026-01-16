# Unit tests CMakeLists.txt for calc

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect unit test sources
set(UNIT_TEST_SOURCES
    error_test.cpp
    tokenizer_test.cpp
    ast_test.cpp
    parser_test.cpp
    shunting_yard_parser_test.cpp
    recursive_descent_parser_test.cpp
    evaluator_test.cpp
    math/converter_test.cpp
    modes/standard_mode_test.cpp
    modes/scientific_mode_test.cpp
    modes/programmer_mode_test.cpp
)

# Create unit test executable
add_executable(calc_unit_tests ${UNIT_TEST_SOURCES})

# Link Google Test
target_link_libraries(calc_unit_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        calc_utils
        calc_core
        calc_modes
        calc_math
)

# Create alias for tests executable
add_executable(calc::unit_tests ALIAS calc_unit_tests)

# Register tests with CTest
add_test(NAME unit_tests COMMAND calc_unit_tests)

# Set test properties
set_tests_properties(unit_tests PROPERTIES
    LABELS "unit"
    TIMEOUT 30
)
