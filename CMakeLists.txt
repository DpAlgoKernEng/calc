cmake_minimum_required(VERSION 3.15)
project(calc VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include compiler options
include(cmake/CompileOptions.cmake)

# Dependencies via FetchContent
include(FetchContent)

# Google Test - use git tag 1.14.0
FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)
# For Windows: Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Installation
install(TARGETS calc_cli DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/calc)

# Print configuration summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Calc - Cross-platform Calculator")
message(STATUS "========================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "========================================")
message(STATUS "")
