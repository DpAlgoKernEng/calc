# Integration tests CMakeLists.txt for calc

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect integration test sources (empty for now)
set(INTEGRATION_TEST_SOURCES
    # standard_mode_test.cpp
    # scientific_mode_test.cpp
    # programmer_mode_test.cpp
)

# No integration tests yet in Phase 1
# Uncomment when tests are implemented:
# add_executable(calc_integration_tests ${INTEGRATION_TEST_SOURCES})
# target_link_libraries(calc_integration_tests
#     PRIVATE
#         GTest::gtest
#         GTest::gtest_main
#         calc_cli
# )
# add_test(NAME integration_tests COMMAND calc_integration_tests)
