# Simple Qt Example for Calculator Project
# This example tests Qt integration with the existing calculator architecture

cmake_minimum_required(VERSION 3.16)
project(qt_calc_example VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to match main project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6 package
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic moc, uic, and rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add executable
add_executable(qt_calc_example
    main.cpp
    mainwindow.cpp
)

# Link with Qt libraries
target_link_libraries(qt_calc_example PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Include current directory for headers
target_include_directories(qt_calc_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Copy Qt runtime dependencies on Windows (optional)
# This is commented out for simplicity - uncomment if needed for Windows deployment
#if(WIN32)
#    add_custom_command(TARGET qt_calc_example POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            "$<TARGET_FILE:Qt6::Core>"
#            "$<TARGET_FILE_DIR:qt_calc_example>"
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            "$<TARGET_FILE:Qt6::Widgets>"
#            "$<TARGET_FILE_DIR:qt_calc_example>"
#    )
#endif()

# Print configuration summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Qt Calculator Example Configuration")
message(STATUS "========================================")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Qt Version: ${Qt6_VERSION}")
message(STATUS "  Qt Libraries: Core, Widgets")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
message(STATUS "")