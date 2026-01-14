# Source code CMakeLists.txt for calc

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect source files
set(CORE_SOURCES
    # core/tokenizer.cpp
    # core/parser/shunting_yard_parser.cpp
    # core/ast/ast_node.cpp
    # core/ast/binary_op_node.cpp
    # core/ast/unary_op_node.cpp
    # core/ast/function_call_node.cpp
    # core/ast/literal_node.cpp
    # core/evaluator/evaluator.cpp
)

set(MODES_SOURCES
    # modes/standard_mode.cpp
    # modes/scientific_mode.cpp
    # modes/programmer_mode.cpp
)

set(MATH_SOURCES
    # math/math_functions.cpp
    # math/converter.cpp
)

set(UI_SOURCES
    # ui/cli/cli_app.cpp
    # ui/cli/command_parser.cpp
    # ui/cli/output_formatter.cpp
)

set(UTILS_SOURCES
    utils/error.cpp
    # utils/string_utils.cpp
)

# Create core library (for future use)
add_library(calc_core INTERFACE)
target_include_directories(calc_core INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Create utils library
add_library(calc_utils
    ${UTILS_SOURCES}
)
target_link_libraries(calc_utils PUBLIC calc_core)

# Create calc_cli executable
add_executable(calc_cli
    main.cpp
)

# Link libraries
target_link_libraries(calc_cli
    PRIVATE
        calc_utils
)

# Create alias for convenience
add_executable(calc::calc_cli ALIAS calc_cli)
