# Changelog

All notable changes to Calc will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Interactive REPL mode with history and search
- Color-coded output for better readability
- Multiple precision levels support
- History export/import functionality
- Documentation generation with Doxygen

### Changed
- Improved error messages with position indicators
- Enhanced CLI with better argument parsing

### Fixed
- Fixed parsing of negative numbers in expressions
- Fixed edge case in programmer mode for large hex values

---

## [1.0.0] - 2025-01-17

### Added

#### Standard Mode
- Basic arithmetic operations: +, -, *, /
- Parentheses for expression grouping
- Exponentiation operator (^) with right-associativity
- Unary minus for negative numbers

#### Scientific Mode
- Trigonometric functions: sin, cos, tan, asin, acos, atan
- Hyperbolic functions: sinh, cosh, tanh
- Logarithmic functions: log, log10, exp
- Root functions: sqrt, cbrt
- Rounding functions: abs, floor, ceil, round, trunc
- Mathematical constants: PI, E
- Power function: pow

#### Programmer Mode
- Number base support: decimal, binary (0b), octal (0o), hexadecimal (0x)
- Bitwise operators: &, |, ^, ~, <<, >>
- Base conversion utilities

#### Core Engine
- Tokenizer with multi-format number support
- Two parsing algorithms:
  - Shunting-yard algorithm for efficient parsing
  - Recursive descent parser for flexibility
- AST-based evaluation with visitor pattern
- Extensible function registry

#### CLI Interface
- Command-line argument parsing
- Mode selection (--mode)
- Precision control (--precision)
- Interactive REPL mode (-i)
- Colored output support
- History management
- Expression validation

#### Build System
- CMake-based build system
- Cross-platform support (Windows, macOS, Linux)
- Google Test integration
- Code coverage support
- Static analysis (clang-tidy, cppcheck)
- Packaging scripts

#### Documentation
- Architecture documentation
- API reference
- User guide
- Phase reports

---

## Version Guidelines

### Version Numbering: MAJOR.MINOR.PATCH

- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality in a backwards compatible manner
- **PATCH**: Backwards compatible bug fixes

### Types of Changes

#### Added
- New features
- New functionality

#### Changed
- Changes in existing functionality
- Behavior changes

#### Deprecated
- Soon-to-be removed features

#### Removed
- Removed features (should be deprecated first)

#### Fixed
- Bug fixes

#### Security
- Security vulnerability fixes
