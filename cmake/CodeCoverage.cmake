# CodeCoverage.cmake - Code coverage support for calc project
#
# Usage:
#   In main CMakeLists.txt:
#     option(ENABLE_COVERAGE "Enable code coverage" OFF)
#     if(ENABLE_COVERAGE)
#         include(cmake/CodeCoverage.cmake)
#         setup_target_for_coverage(calc_coverage calc calc_unit_tests)
#     endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Code coverage results are best obtained with CMAKE_BUILD_TYPE=Debug")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    # Add compiler flags for coverage
    add_compile_options(--coverage)
    add_link_options(--coverage)

    message(STATUS "Code coverage enabled for ${CMAKE_CXX_COMPILER_ID}")
else()
    message(WARNING "Code coverage is only supported with GCC or Clang compilers")
    return()
endif()

# Find gcov/lcov
find_program(GCOV_PATH gcov)
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(NOT GCOV_PATH)
    message(WARNING "gcov not found! Code coverage reports will not be generated.")
    return()
endif()

message(STATUS "gcov found: ${GCOV_PATH}")
if(LCOV_PATH)
    message(STATUS "lcov found: ${LCOV_PATH}")
endif()
if(GENHTML_PATH)
    message(STATUS "genhtml found: ${GENHTML_PATH}")
endif()

# Function to create coverage target for a specific test
# setup_target_for_coverage(<name> <target> <test_target> [args...])
function(setup_target_for_coverage NAME TARGET TEST_TARGET)
    if(NOT LCOV_PATH OR NOT GENHTML_PATH)
        return()
    endif()

    # Clean coverage data before running tests
    add_custom_target(${NAME}_clean
        COMMAND ${LCOV_PATH} --directory ${CMAKE_CURRENT_BINARY_DIR} --zerocounters
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.info
        COMMAND ${CMAKE_COMMAND} -E remove -rf ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_html
        COMMENT "Cleaning coverage data for ${NAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Run tests and capture coverage
    add_custom_target(${NAME}_capture
        COMMAND ${CMAKE_COMMAND} -E env GCOV_PREFIX_STRIP=1 GCOV_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_BINARY_DIR}/${TEST_TARGET} ${ARGN}
        COMMAND ${LCOV_PATH} --directory ${CMAKE_CURRENT_BINARY_DIR}
                --capture --output-file ${NAME}.info
        COMMENT "Running tests and capturing coverage data"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${NAME}_clean ${TEST_TARGET}
    )

    # Remove test coverage (Google Test, etc.)
    add_custom_target(${NAME}_filter
        COMMAND ${LCOV_PATH}
                --remove ${NAME}.info
                "/usr/*"
                "${CMAKE_BINARY_DIR}/*"
                "*/_deps/*"
                "*/build/*"
                "*/tests/*"
                "*/benchmark/*"
                --output-file ${NAME}.filtered.info
        COMMENT "Filtering coverage data for ${NAME}"
        DEPENDS ${NAME}_capture
    )

    # Generate HTML report
    add_custom_target(${NAME}_html
        COMMAND ${GENHTML_PATH}
                ${NAME}.filtered.info
                --output-directory ${NAME}_html
                --title "${PROJECT_NAME} ${PROJECT_VERSION} - ${NAME} Coverage"
                --legend
                --show-details
        COMMENT "Generating HTML coverage report"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${NAME}_filter
    )

    # Generate summary
    add_custom_target(${NAME}_summary
        COMMAND ${LCOV_PATH}
                --summary ${NAME}.filtered.info
        COMMENT "Coverage summary for ${NAME}"
        DEPENDS ${NAME}_filter
    )

    # Main coverage target (generate HTML and show summary)
    add_custom_target(${NAME}
        DEPENDS ${NAME}_html ${NAME}_summary
        COMMENT "Generating full coverage report for ${NAME}"
    )

    # Alias for convenience
    add_custom_target(${NAME}_all DEPENDS ${NAME})

    message(STATUS "Coverage target '${NAME}' created")
endfunction()

# Function to create a combined coverage target for multiple test executables
# setup_combined_coverage(<name> <test_target1> <test_target2> ...)
function(setup_combined_coverage NAME)
    if(NOT LCOV_PATH OR NOT GENHTML_PATH)
        return()
    endif()

    # Collect test targets
    set(TEST_TARGETS ${ARGN})

    # Clean coverage data
    add_custom_target(${NAME}_clean
        COMMAND ${LCOV_PATH} --directory ${CMAKE_CURRENT_BINARY_DIR} --zerocounters
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.info
        COMMAND ${CMAKE_COMMAND} -E remove -rf ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_html
        COMMENT "Cleaning combined coverage data"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Run all tests
    add_custom_target(${NAME}_run_tests
        DEPENDS ${NAME}_clean
    )

    foreach(TEST_TARGET ${TEST_TARGETS})
        add_custom_command(TARGET ${NAME}_run_tests POST_BUILD
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_TARGET}
            COMMENT "Running ${TEST_TARGET}"
        )
    endforeach()

    # Capture coverage
    add_custom_target(${NAME}_capture
        COMMAND ${LCOV_PATH} --directory ${CMAKE_CURRENT_BINARY_DIR}
                --capture --output-file ${NAME}.info
        COMMENT "Capturing combined coverage data"
        DEPENDS ${NAME}_run_tests
    )

    # Filter coverage data
    add_custom_target(${NAME}_filter
        COMMAND ${LCOV_PATH}
                --remove ${NAME}.info
                "/usr/*"
                "${CMAKE_BINARY_DIR}/*"
                "*/_deps/*"
                "*/build/*"
                "*/tests/*"
                "*/benchmark/*"
                --output-file ${NAME}.filtered.info
        COMMENT "Filtering combined coverage data"
        DEPENDS ${NAME}_capture
    )

    # Generate HTML report
    add_custom_target(${NAME}_html
        COMMAND ${GENHTML_PATH}
                ${NAME}.filtered.info
                --output-directory ${NAME}_html
                --title "${PROJECT_NAME} ${PROJECT_VERSION} - Combined Coverage"
                --legend
                --show-details
        COMMENT "Generating combined HTML coverage report"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${NAME}_filter
    )

    # Generate summary
    add_custom_target(${NAME}_summary
        COMMAND ${LCOV_PATH}
                --summary ${NAME}.filtered.info
        COMMENT "Combined coverage summary"
        DEPENDS ${NAME}_filter
    )

    # Main coverage target
    add_custom_target(${NAME}
        DEPENDS ${NAME}_html ${NAME}_summary
        COMMENT "Generating combined coverage report"
    )

    message(STATUS "Combined coverage target '${NAME}' created for: ${TEST_TARGETS}")
endfunction()

# Function to check coverage threshold
# check_coverage_threshold(<info_file> <line_threshold> <function_threshold>)
function(check_coverage_threshold INFO_FILE LINE_THRESHOLD FUNCTION_THRESHOLD)
    if(NOT LCOV_PATH)
        return()
    endif()

    # Extract line coverage percentage
    execute_process(
        COMMAND ${LCOV_PATH} --summary ${INFO_FILE}
        OUTPUT_VARIABLE COV_SUMMARY
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Parse summary (format: "lines......: X.X% (Y of Z)")
    string(REGEX MATCH "lines\\.+: ([0-9.]+)%" LINE_MATCH "${COV_SUMMARY}")
    set(LINE_COV ${CMAKE_MATCH_1})

    # Parse functions (format: "functions..: X.X% (Y of Z)")
    string(REGEX MATCH "functions\\.+: ([0-9.]+)%" FUNC_MATCH "${COV_SUMMARY}")
    set(FUNC_COV ${CMAKE_MATCH_1})

    # Check thresholds
    set(RESULT TRUE)

    if(DEFINED LINE_COV)
        if(NOT "${LINE_COV}" STREQUAL "")
            if(LINE_COV LESS LINE_THRESHOLD)
                message(WARNING "Line coverage ${LINE_COV}% is below threshold ${LINE_THRESHOLD}%")
                set(RESULT FALSE)
            else()
                message(STATUS "Line coverage ${LINE_COV}% meets threshold ${LINE_THRESHOLD}%")
            endif()
        endif()
    endif()

    if(DEFINED FUNC_COV)
        if(NOT "${FUNC_COV}" STREQUAL "")
            if(FUNC_COV LESS FUNCTION_THRESHOLD)
                message(WARNING "Function coverage ${FUNC_COV}% is below threshold ${FUNCTION_THRESHOLD}%")
                set(RESULT FALSE)
            else()
                message(STATUS "Function coverage ${FUNC_COV}% meets threshold ${FUNCTION_THRESHOLD}%")
            endif()
        endif()
    endif()

    set(COVERAGE_OK ${RESULT} PARENT_SCOPE)
endfunction()
