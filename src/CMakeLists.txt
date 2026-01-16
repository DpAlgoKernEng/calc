# Source code CMakeLists.txt for calc

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect source files
set(CORE_SOURCES
    core/tokenizer.cpp
    core/parser/shunting_yard_parser.cpp
    core/parser/recursive_descent_parser.cpp
    core/ast/literal_node.cpp
    core/ast/binary_op_node.cpp
    core/ast/unary_op_node.cpp
    core/ast/function_call_node.cpp
    core/evaluator/evaluator.cpp
    core/evaluator/evaluator_visitor.cpp
)

set(MODES_SOURCES
    modes/standard_mode.cpp
    modes/mode_manager.cpp
    # modes/scientific_mode.cpp
    # modes/programmer_mode.cpp
)

set(UI_SOURCES
    ui/cli/cli_app.cpp
    ui/cli/command_parser.cpp
    ui/cli/output_formatter.cpp
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

# Create core parsing library
add_library(calc_core_parser
    ${CORE_SOURCES}
)
target_link_libraries(calc_core_parser PUBLIC calc_core)

# Create modes library
add_library(calc_modes
    ${MODES_SOURCES}
)
target_link_libraries(calc_modes PUBLIC calc_core_parser calc_utils)

# Create UI library
add_library(calc_ui
    ${UI_SOURCES}
)
target_link_libraries(calc_ui PUBLIC calc_modes calc_core_parser calc_utils)

# Create calc_cli executable
add_executable(calc_cli
    main.cpp
)

# Link libraries
target_link_libraries(calc_cli
    PRIVATE
        calc_ui
)

# Create alias for convenience
add_executable(calc::calc_cli ALIAS calc_cli)
