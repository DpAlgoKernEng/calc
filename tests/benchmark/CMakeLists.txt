# Benchmark CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

message(STATUS "Configuring benchmark targets")

# Find benchmark executable if Google Benchmark is available (optional)
find_package(benchmark QUIET)

# Create benchmark executables using our internal framework
add_executable(tokenizer_benchmark
    tokenizer_benchmark.cpp
)

target_link_libraries(tokenizer_benchmark
    PRIVATE
    calc_core
    calc_utils
)

add_executable(parser_benchmark
    parser_benchmark.cpp
)

target_link_libraries(parser_benchmark
    PRIVATE
    calc_core
    calc_utils
    calc_math
)

add_executable(evaluator_benchmark
    evaluator_benchmark.cpp
)

target_link_libraries(evaluator_benchmark
    PRIVATE
    calc_core
    calc_modes
)

# Only build if benchmarks are enabled
set_target_properties(tokenizer_benchmark parser_benchmark evaluator_benchmark PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)

# Custom target to build all benchmarks
add_custom_target(benchmarks
    DEPENDS tokenizer_benchmark parser_benchmark evaluator_benchmark
)

# Custom target to run all benchmarks
add_custom_target(run_benchmarks
    COMMAND ${CMAKE_COMMAND} -E env
        ${CMAKE_CURRENT_BINARY_DIR}/tokenizer_benchmark
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/parser_benchmark
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/evaluator_benchmark
    DEPENDS benchmarks
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running all performance benchmarks"
)

message(STATUS "  - tokenizer_benchmark")
message(STATUS "  - parser_benchmark")
message(STATUS "  - evaluator_benchmark")
