# CLI Application CMake Configuration

cmake_minimum_required(VERSION 3.16)

# Project configuration
project(calc_cli VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect source files
set(CLI_SOURCES
    cli_app.cpp
    command_parser.cpp
    history_manager.cpp
    output_formatter.cpp
)

# Collect CLI headers
set(CLI_HEADERS
    include/calc/ui/cli/cli_app.h
    include/calc/ui/cli/command_parser.h
    include/calc/ui/cli/history_manager.h
    include/calc/ui/cli/output_formatter.h
)

# Create CLI library for reuse in tests
add_library(calc_cli_lib ${CLI_SOURCES})
target_include_directories(calc_cli_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/calc/ui/cli
)
target_link_libraries(calc_cli_lib PUBLIC
    calc_core
    calc_modes
    calc_utils
)

# Create CLI executable
add_executable(calc_cli main.cpp)
target_include_directories(calc_cli PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calc_cli PRIVATE calc_cli_lib)

# Set output directory
set_target_properties(calc_cli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Print configuration info
message(STATUS "")
message(STATUS "========================================")
message(STATUS " CLI Configuration")
message(STATUS "========================================")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
message(STATUS "")
