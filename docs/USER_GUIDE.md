# Calc User Guide

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
  - [Linux](#linux)
  - [macOS](#macos)
  - [Windows](#windows)
  - [Building from Source](#building-from-source)
- [Quick Start](#quick-start)
- [Command Line Options](#command-line-options)
- [Modes](#modes)
  - [Standard Mode](#standard-mode)
  - [Scientific Mode](#scientific-mode)
  - [Programmer Mode](#programmer-mode)
- [Interactive REPL Mode](#interactive-repl-mode)
- [Expression Syntax](#expression-syntax)
  - [Numbers](#numbers)
  - [Operators](#operators)
  - [Functions](#functions)
  - [Parentheses](#parentheses)
- [REPL Commands](#repl-commands)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)

---

## Introduction

Calc is a powerful cross-platform command-line calculator that supports three distinct computing modes:

1. **Standard Mode**: Basic arithmetic operations
2. **Scientific Mode**: Advanced mathematical functions
3. **Programmer Mode**: Base conversions and bitwise operations

This guide covers everything you need to know to use Calc effectively.

---

## Installation

### Linux

#### Using Package Manager (Debian/Ubuntu)

```bash
sudo apt-get update
sudo apt-get install calc-cli
```

#### Using Binary Package

Download the latest `.deb` or `.tar.gz` package from the [releases page](https://github.com/yourusername/calc/releases).

For `.deb`:
```bash
sudo dpkg -i calc-cli_1.0.0_amd64.deb
```

For `.tar.gz`:
```bash
tar -xzf calc-1.0.0-linux-x86_64.tar.gz
cd calc-1.0.0-linux-x86_64
sudo cp calc_cli /usr/local/bin/
```

### macOS

#### Using Homebrew

```bash
brew install calc-cli
```

#### Using Binary Package

Download the `.dmg` file from the releases page and mount it. Then copy the binary:

```bash
sudo cp /Volumes/calc-cli/calc_cli /usr/local/bin/
```

### Windows

#### Using Installer

Download the `.msi` installer from the [releases page](https://github.com/yourusername/calc/releases) and run it.

#### Using Portable Binary

Download the `.zip` archive, extract it, and add the directory to your PATH.

### Building from Source

#### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git

#### Build Steps

```bash
# Clone the repository
git clone https://github.com/yourusername/calc.git
cd calc

# Configure build
cmake -B build -S .

# Build (Release)
cmake --build build --config Release

# Install (optional)
sudo cmake --install build

# Run tests (optional)
ctest --test-dir build --output-on-failure
```

#### Cross-Platform Builds

**For Windows using MinGW:**
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-mingw-w64-x86_64.cmake
cmake --build build --config Release
```

**For macOS Universal Binary:**
```bash
cmake -B build -S . -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --config Release
```

---

## Quick Start

### Basic Usage

```bash
# Simple expression
calc_cli "2 + 3"
# Output: 5

# Scientific notation
calc_cli "1.5e3 / 2"
# Output: 750

# Using parentheses
calc_cli "(2 + 3) * 4"
# Output: 20
```

### Interactive Mode

```bash
# Start interactive REPL
calc_cli -i

# Or with specific mode
calc_cli --mode scientific -i
```

In interactive mode, you can enter expressions one at a time and see results immediately.

---

## Command Line Options

| Option | Short | Description | Example |
|--------|-------|-------------|---------|
| `--mode <mode>` | `-m` | Set calculator mode | `--mode scientific` |
| `--precision <n>` | `-p` | Set decimal precision | `--precision 10` |
| `--interactive` | `-i` | Start interactive REPL | `-i` |
| `--help` | `-h` | Show help message | `-h` |
| `--version` | `-v` | Show version | `-v` |
| `--verbose` | | Enable verbose output | `--verbose` |
| `--no-color` | | Disable colored output | `--no-color` |
| `--base <base>` | `-b` | Set number base (programmer mode) | `--base hex` |

### Examples

```bash
# Evaluate in scientific mode
calc_cli --mode scientific "sin(PI/4) + cos(PI/4)"

# Set custom precision
calc_cli --precision 15 "1/3"

# Evaluate multiple expressions
calc_cli "2+3" "4*5" "10/2"

# Verbose mode for debugging
calc_cli --verbose "2 + sin(30)"
```

---

## Modes

### Standard Mode

Standard mode provides basic arithmetic operations.

**Supported Operators:**
- Addition: `+`
- Subtraction: `-`
- Multiplication: `*`
- Division: `/`
- Exponentiation: `^`
- Parentheses: `( )`

**Operator Precedence (highest to lowest):**
1. Parentheses `()`
2. Exponentiation `^` (right-associative)
3. Multiplication `*`, Division `/`
4. Addition `+`, Subtraction `-`

**Examples:**

```bash
# Basic arithmetic
calc_cli "2 + 3"           # 5
calc_cli "10 - 4"          # 6
calc_cli "3 * 4"           # 12
calc_cli "15 / 3"          # 5

# Exponentiation
calc_cli "2^3"             # 8
calc_cli "4^0.5"           # 2

# Complex expressions
calc_cli "2 + 3 * 4"       # 14 (multiplication first)
calc_cli "(2 + 3) * 4"     # 20 (parentheses first)
calc_cli "2^3^2"           # 512 (right-associative: 2^(3^2))

# Negative numbers
calc_cli "-5 + 3"          # -2
calc_cli "3 * -4"          # -12
```

### Scientific Mode

Scientific mode extends standard mode with mathematical functions and constants.

**Additional Functions:**

| Category | Functions |
|----------|-----------|
| Trigonometric | `sin`, `cos`, `tan`, `asin`, `acos`, `atan` |
| Hyperbolic | `sinh`, `cosh`, `tanh` |
| Logarithmic | `log` (natural log), `log10` (base 10) |
| Exponential | `exp` (e^x) |
| Roots | `sqrt` (square root), `cbrt` (cube root) |
| Rounding | `abs`, `floor`, `ceil`, `round`, `trunc` |
| Other | `fmod`, `hypot`, `max`, `min`, `remainder`, `pow` |

**Constants:**
- `PI` - π (3.14159...)
- `E` - e (2.71828...)

**Examples:**

```bash
# Trigonometric functions (angles in radians)
calc_cli --mode scientific "sin(PI/2)"     # 1
calc_cli --mode scientific "cos(0)"        # 1
calc_cli --mode scientific "tan(PI/4)"     # 1

# Logarithmic functions
calc_cli --mode scientific "log(E)"        # 1
calc_cli --mode scientific "log10(100)"   # 2
calc_cli --mode scientific "exp(2)"       # 7.389056

# Roots
calc_cli --mode scientific "sqrt(16)"      # 4
calc_cli --mode scientific "cbrt(27)"      # 3

# Rounding
calc_cli --mode scientific "floor(3.7)"    # 3
calc_cli --mode scientific "ceil(3.2)"     # 4
calc_cli --mode scientific "round(3.5)"    # 4
calc_cli --mode scientific "abs(-5)"       # 5

# Nested functions
calc_cli --mode scientific "sqrt(sin(PI/2)^2 + cos(PI/2)^2)"  # 1
calc_cli --mode scientific "max(floor(3.7), ceil(3.2))"        # 4

# Power function alternative to ^
calc_cli --mode scientific "pow(2, 10)"   # 1024
calc_cli --mode scientific "2^10"         # 1024
```

### Programmer Mode

Programmer mode handles different number bases and bitwise operations.

**Number Base Prefixes:**
- Binary: `0b` or `0B` (e.g., `0b1010`)
- Octal: `0o` or `0O` (e.g., `0o77`)
- Hexadecimal: `0x` or `0X` (e.g., `0xFF`)
- Decimal: default (e.g., `42`)

**Bitwise Operators:**
- AND: `&`
- OR: `|`
- XOR: `^`
- NOT: `~`
- Left Shift: `<<`
- Right Shift: `>>`

**Output Base:**
Use `--base` option to specify output format:
```bash
calc_cli --mode programmer --base hex
```

**Examples:**

```bash
# Different input bases
calc_cli --mode programmer "0b1010 + 0o10"      # 12 (10 + 2)
calc_cli --mode programmer "0xFF + 1"          # 256
calc_cli --mode programmer "0o77"              # 63

# Bitwise operations
calc_cli --mode programmer "0xFF & 0x0F"       # 15
calc_cli --mode programmer "0xF0 | 0x0F"       # 255
calc_cli --mode programmer "0xFF ^ 0xAA"       # 85
calc_cli --mode programmer "~0x0F"             # -16

# Bitwise shifts
calc_cli --mode programmer "0x01 << 4"         # 16
calc_cli --mode programmer "0x80 >> 4"         # 8

# Combined operations
calc_cli --mode programmer "(0xFF & 0xF0) >> 4"  # 15

# Output in different bases
calc_cli --mode programmer --base hex "0xFF"  # FF
calc_cli --mode programmer --base bin "15"    # 1111
calc_cli --mode programmer --base oct "8"     # 10
```

---

## Interactive REPL Mode

The REPL (Read-Eval-Print Loop) mode allows you to enter expressions interactively.

```bash
# Start REPL in standard mode
calc_cli -i

# Start REPL in scientific mode
calc_cli --mode scientific -i
```

**REPL Features:**
- Expression evaluation
- Result history (access with `_`)
- Mode switching
- Precision control
- History search
- Result export

---

## Expression Syntax

### Numbers

**Decimal Numbers:**
```bash
1234
12.34
.5       # 0.5
```

**Scientific Notation:**
```bash
1.5e3    # 1500
2e-3     # 0.002
```

**Negative Numbers:**
```bash
-5
-3.14
```

### Operators

**Arithmetic:**
| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `2 + 3` = 5 |
| `-` | Subtraction | `5 - 2` = 3 |
| `*` | Multiplication | `4 * 5` = 20 |
| `/` | Division | `10 / 2` = 5 |
| `^` | Exponentiation | `2^3` = 8 |

**Bitwise (Programmer mode):**
| Operator | Description | Example |
|----------|-------------|---------|
| `&` | Bitwise AND | `0xFF & 0x0F` = 15 |
| `|` | Bitwise OR | `0xF0 \| 0x0F` = 255 |
| `^` | Bitwise XOR | `0xFF ^ 0xAA` = 85 |
| `~` | Bitwise NOT | `~0x0F` = -16 |
| `<<` | Left Shift | `0x01 << 4` = 16 |
| `>>` | Right Shift | `0x80 >> 4` = 8 |

### Functions

**Function syntax:** `function_name(argument1, argument2, ...)`

```bash
# Single argument
sin(PI/2)
log(10)

# Multiple arguments
max(3, 7)
min(5, 2)

# Nested functions
sqrt(abs(-16))
pow(sin(PI/4), 2) + pow(cos(PI/4), 2)
```

### Parentheses

Use parentheses to override operator precedence:

```bash
# Without parentheses (multiplication first)
2 + 3 * 4    # = 14

# With parentheses
(2 + 3) * 4  # = 20

# Complex nesting
((2 + 3) * (4 - 1)) ^ 2  # = 225
```

---

## REPL Commands

In interactive mode, you can use special commands starting with `/`:

| Command | Description | Example |
|---------|-------------|---------|
| `/help` | Show help | `/help` |
| `/quit` | Exit REPL | `/quit` |
| `/exit` | Exit REPL | `/exit` |
| `/clear` | Clear history | `/clear` |
| `/mode <name>` | Change mode | `/mode scientific` |
| `/precision <n>` | Set precision | `/precision 10` |
| `/history [n]` | Show history | `/history` or `/history 5` |
| `/search <text>` | Search history | `/search sin` |
| `/export <file>` | Export history | `/export calc_history.txt` |

**Using Previous Result:**
Use `_` to reference the last result:

```bash
> 2 + 3
Result: 5
> _ * 2
Result: 10
> _ + 5
Result: 15
```

---

## Examples

### Everyday Calculations

```bash
# Calculate tip
calc_cli "45.00 * 0.18"           # 8.1 (18% tip)

# Convert Celsius to Fahrenheit
calc_cli "25 * 1.8 + 32"           # 77

# BMI calculation (weight in kg, height in m)
calc_cli "70 / (1.75 * 1.75)"     # 22.857143
```

### Physics Calculations

```bash
# Velocity (v = d/t)
calc_cli --mode scientific "100 / 10"    # 10 m/s

# Force (F = ma)
calc_cli --mode scientific "5 * 9.8"     # 49 N

# Kinetic energy (KE = 0.5 * m * v^2)
calc_cli --mode scientific "0.5 * 10 * 5^2"    # 125 J
```

### Financial Calculations

```bash
# Simple interest (I = P * r * t)
calc_cli --mode scientific "10000 * 0.05 * 3"    # 1500

# Future value (FV = PV * (1 + r)^t)
calc_cli --mode scientific "10000 * (1 + 0.05)^10"    # 16288.946267
```

### Programming Tasks

```bash
# Convert decimal to hex (and vice versa)
calc_cli --mode programmer "255"                    # Output in decimal
calc_cli --mode programmer --base hex "255"          # FF

# Bitmask operations
calc_cli --mode programmer "0x01 | 0x04 | 0x08"     # 13

# Extract bits
calc_cli --mode programmer "0xFF & 0x0F"            # 15
```

---

## Troubleshooting

### Common Errors

**"Division by zero"**
```
Error: Division by zero at position 8
Expression: 10 / 0
             ^
```
Solution: Check your expression for division by zero.

**"Invalid syntax"**
```
Error: Invalid syntax at position 2
Expression: 2 + * 3
           ^
```
Solution: Check for missing or incorrect operators.

**"Domain error"**
```
Error: Domain error: sqrt of negative number at position 9
Expression: sqrt(-4)
          ^
```
Solution: Mathematical functions have domain restrictions. Use `abs()` for negative values if needed.

### Precision Issues

For high-precision calculations, adjust the precision:

```bash
calc_cli --precision 15 "1/3"
# Output: 0.333333333333333

calc_cli --precision 20 "PI"
# Output: 3.14159265358979300000
```

### Color Output

To disable colored output:

```bash
calc_cli --no-color "2 + 3"
```

Or set environment variable:
```bash
export CALC_NO_COLOR=1
calc_cli "2 + 3"
```

---

## FAQ

**Q: How do I calculate the nth root?**

A: Use exponentiation with fractional exponents:
```bash
calc_cli --mode scientific "8^(1/3)"   # Cube root of 8 = 2
```

**Q: How do I convert between degrees and radians?**

A: Use the conversion formulas:
```bash
# Degrees to radians
calc_cli --mode scientific "45 * PI / 180"

# Radians to degrees
calc_cli --mode scientific "PI / 4 * 180 / PI"
```

**Q: Can I use variables?**

A: The current version doesn't support user-defined variables in expressions. However, you can use the previous result with `_` in REPL mode.

**Q: How do I calculate logarithms with different bases?**

A: Use the change of base formula: `log_b(x) = log(x) / log(b)`
```bash
calc_cli --mode scientific "log(8) / log(2)"   # log base 2 of 8 = 3
```

**Q: Can I save my calculation history?**

A: Yes, use the export command in REPL mode:
```bash
/export my_calculations.txt
```

**Q: What's the maximum number supported?**

A: Calc uses C++ `double` which typically supports values up to ~1.8 × 10^308. For integer operations in Programmer mode, the limit depends on your system (usually 64-bit signed: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807).

**Q: How do I get help for a specific mode?**

A: Use the `/help` command in REPL mode, or run:
```bash
calc_cli --help
```

---

## Additional Resources

- [API Documentation](API.md)
- [Architecture Documentation](../ARCHITECTURE.md)
- [Design Document](DESIGN.md)
- [GitHub Repository](https://github.com/yourusername/calc)
- [Issue Tracker](https://github.com/yourusername/calc/issues)

---

## License

Calc is released under the MIT License. See the [LICENSE](../LICENSE) file for details.
