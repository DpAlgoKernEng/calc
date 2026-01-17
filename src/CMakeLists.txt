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
set(MATH_SOURCES
    math/converter.cpp
)
set(MODES_SOURCES
    modes/standard_mode.cpp
    modes/scientific_mode.cpp
    modes/programmer_mode.cpp
    modes/mode_manager.cpp
)
set(UI_SOURCES
    ui/cli/cli_app.cpp
    ui/cli/command_parser.cpp
    ui/cli/output_formatter.cpp
    ui/cli/history_manager.cpp
)
set(UTILS_SOURCES
    utils/error.cpp
)

# Create core library
add_library(calc_core ${CORE_SOURCES})
target_include_directories(calc_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
# Link calc_utils for exception definitions used in core
target_link_libraries(calc_core PUBLIC calc_utils)

# Create math library
add_library(calc_math ${MATH_SOURCES})
target_include_directories(calc_math PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calc_math PUBLIC calc_core)

# Create modes library
add_library(calc_modes ${MODES_SOURCES})
target_include_directories(calc_modes PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calc_modes PUBLIC calc_core calc_math)

# Create UI library
add_library(calc_ui ${UI_SOURCES})
target_include_directories(calc_ui PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calc_ui PUBLIC calc_modes calc_core calc_utils)

# Create utils library
add_library(calc_utils ${UTILS_SOURCES})
target_include_directories(calc_utils PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Create calc_cli executable
add_executable(calc_cli
    main.cpp
)
target_link_libraries(calc_cli
    PRIVATE
        calc_ui
        calc_modes
        calc_core
        calc_utils
)

# Create alias for convenience
add_executable(calc::calc_cli ALIAS calc_cli)
