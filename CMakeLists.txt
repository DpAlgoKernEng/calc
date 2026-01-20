cmake_minimum_required(VERSION 3.15)
project(calc VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure math constants (M_PI, M_E) are available on MSVC
add_compile_definitions(_USE_MATH_DEFINES)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include compiler options
include(cmake/CompileOptions.cmake)

# Code coverage option
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Benchmark option
option(ENABLE_BENCHMARK "Enable benchmark targets" OFF)

# Qt GUI option
option(BUILD_QT_GUI "Build Qt GUI application" ON)

# Warnings as errors option (for CI)
option(WERROR "Treat warnings as errors" OFF)

# Enable code coverage if requested
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    include(cmake/CodeCoverage.cmake)
endif()

# Treat warnings as errors if requested
if(WERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
    message(STATUS "Warnings treated as errors")
endif()

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
add_subdirectory(src/ui/cli)
add_subdirectory(tests)

# Qt GUI support
if(BUILD_QT_GUI)
    message(STATUS "Qt GUI build enabled")
    add_subdirectory(src/ui/qt)
endif()

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
message(STATUS "  Qt GUI: ${BUILD_QT_GUI}")
message(STATUS "========================================")
message(STATUS "")
