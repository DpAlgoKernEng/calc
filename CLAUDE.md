# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A cross-platform command-line calculator built with CMake and C++17, supporting three computation modes:

1. **Standard Mode**: Basic arithmetic operations (+, -, *, /)
2. **Scientific Mode**: Complex nested expressions and mathematical functions (sin, cos, tan, log, sqrt, pow, etc.)
3. **Programmer Mode**: Base conversions (decimal, binary, hexadecimal) with bitwise and shift operations

The architecture is designed for future extension to Qt-based GUI applications.

## Build Commands

```bash
# Configure build
cmake -B build -S .

# Build project (Debug)
cmake --build build --config Debug

# Build project (Release)
cmake --build build --config Release

# Clean build
rm -rf build

# Run tests
ctest --test-dir build --output-on-failure

# Run calculator
./build/calc_cli
```

## Architecture

### Core Components

```
calc/
├── src/
│   ├── core/           # Core expression parsing and evaluation
│   │   ├── parser/     # Expression parsers (Recursive descent, shunting-yard)
│   │   ├── evaluator/  # Evaluation engine with operator precedence
│   │   └── tokenizer/  # Tokenization of input strings
│   ├── modes/          # Mode-specific implementations
│   │   ├── standard/   # Standard arithmetic mode
│   │   ├── scientific/# Scientific mode with math functions
│   │   └── programmer/ # Programmer mode (base conversion, bitwise)
│   ├── math/           # Math utilities and functions library
│   ├── ui/             # Command-line interface
│   └── main.cpp
├── include/calc/       # Public headers
├── tests/              # Unit and integration tests
└── cmake/              # CMake modules and toolchains
```

### Design Patterns

- **Strategy Pattern**: Mode selection (Standard/Scientific/Programmer)
- **Factory Pattern**: Parser and Calculator creation
- **Visitor Pattern**: Expression tree evaluation
- **Abstract Syntax Tree (AST)**: Intermediate representation of expressions

### Key Classes

- `CalcEngine`: Main calculator engine facade
- `Token`: Lexical unit representing numbers, operators, functions
- `Tokenizer`: Converts input strings into token streams
- `Parser`: Builds AST from tokens (mode-specific)
- `ASTNode`: Base class for expression tree nodes
- `Evaluator`: Traverses and evaluates AST
- `ModeRegistry`: Central registry of available modes
- `Converter`: Base conversion utilities (dec/bin/hex)

### Cross-Platform Support

- Use `std::filesystem` for file operations
- Platform-specific code isolated in `src/platform/`
- CMake toolchain files for Windows/MSVC, macOS/Clang, Linux/GCC

### Qt Integration Path

The core engine is UI-agnostic. For future Qt integration:

1. Keep all logic in `src/core/` and `src/modes/`
2. Create `src/ui/qt/` as new layer
3. Implement `QWidget/QMainWindow` wrappers around existing modes
4. Use signals/slots to connect UI events to `CalcEngine`

## Development Notes

- C++17 features used: `std::optional`, `std::variant`, structured bindings, `if constexpr`
- Exceptions used for error handling in parsing/evaluation
- RAII throughout for resource management
- constexpr for compile-time math operations where applicable
