# Tests CMakeLists.txt for calc

# Enable testing
enable_testing()

# Add test subdirectories
add_subdirectory(unit)
add_subdirectory(integration)

# Add benchmark subdirectory if enabled
if(ENABLE_BENCHMARK)
    add_subdirectory(benchmark)
    message(STATUS "Benchmark targets enabled")
endif()
