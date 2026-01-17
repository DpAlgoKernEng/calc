# Calc

A powerful cross-platform command-line calculator built with C++17 and CMake.

![Build Status](https://img.shields.io/badge/build-passing-success)
![License](https://img.shields.io/badge/license-MIT-blue)
![C++](https://img.shields.io/badge/C++-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.15+-blue)

## Features

- **Three Calculator Modes**
  - **Standard Mode**: Basic arithmetic operations (+, -, *, /, ^)
  - **Scientific Mode**: Advanced mathematical functions (sin, cos, log, sqrt, etc.)
  - **Programmer Mode**: Base conversions and bitwise operations

- **Cross-Platform**: Runs on Windows, macOS, and Linux

- **Interactive REPL**: Read-Eval-Print Loop with history and search

- **Extensible**: Easy to add new modes, functions, and UI frontends

- **Modern C++17**: Uses `std::variant`, `std::optional`, and other C++17 features

## Quick Start

### Installation

#### From Package Manager (Linux)

```bash
sudo apt-get install calc-cli
```

#### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/calc.git
cd calc

# Configure and build
cmake -B build -S .
cmake --build build --config Release

# Install (optional)
sudo cmake --install build
```

See [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for detailed installation instructions.

### Basic Usage

```bash
# Simple expression
calc_cli "2 + 3"
# Output: 5

# Scientific mode
calc_cli --mode scientific "sin(PI/2) + cos(PI/2)"
# Output: 1

# Interactive mode
calc_cli -i

# Programmer mode
calc_cli --mode programmer "0xFF & 0x0F"
# Output: 15
```

## Command Line Options

| Option | Description |
| ------- | ----------- |
| `-m, --mode <mode>` | Set calculator mode (standard, scientific, programmer) |
| `-p, --precision <n>` | Set decimal precision (default: 6) |
| `-i, --interactive` | Start interactive REPL |
| `-h, --help` | Show help message |
| `-v, --version` | Show version |
| `--verbose` | Enable verbose output |
| `--no-color` | Disable colored output |
| `-b, --base <base>` | Set number base (programmer mode) |

## Modes

### Standard Mode

Supports basic arithmetic operations:

```bash
calc_cli --mode standard "2 + 3 * 4"
# Output: 14

calc_cli --mode standard "(2 + 3) * 4"
# Output: 20

calc_cli --mode standard "2^3"
# Output: 8
```

### Scientific Mode

Includes mathematical functions and constants:

```bash
calc_cli --mode scientific "sin(PI/2)"
# Output: 1

calc_cli --mode scientific "sqrt(16) + log10(100)"
# Output: 6

calc_cli --mode scientific "exp(2) * cos(0)"
# Output: 7.389056
```

### Programmer Mode

Handles different number bases and bitwise operations:

```bash
calc_cli --mode programmer "0b1010 + 0o10"
# Output: 12

calc_cli --mode programmer "0xFF & 0x0F"
# Output: 15

calc_cli --mode programmer "0x01 << 4"
# Output: 16
```

## Interactive REPL Mode

Start the interactive REPL:

```bash
calc_cli -i
```

In REPL mode:

- Enter expressions to evaluate
- Use `_` to reference the last result
- Type `/help` for available commands
- Type `/quit` to exit

REPL Commands:
- `/help` - Show help
- `/quit` - Exit REPL
- `/clear` - Clear history
- `/mode <name>` - Change mode
- `/precision <n>` - Set precision
- `/history [n]` - Show history
- `/search <text>` - Search history
- `/export <file>` - Export history

## Development

### Building from Source

```bash
# Configure build
cmake -B build -S .

# Build (Debug)
cmake --build build --config Debug

# Build (Release)
cmake --build build --config Release

# Run tests
ctest --test-dir build --output-on-failure
```

### Using Build Scripts

```bash
# Build with custom options
./scripts/build.sh --type Release --test

# Build with code coverage
./scripts/build.sh --coverage

# Package for distribution
./scripts/package.sh

# Install to system
sudo ./scripts/install.sh
```

### Code Quality

```bash
# Format code
./scripts/format.sh

# Run static analysis
./scripts/run-clang-tidy.sh
./scripts/run-cppcheck.sh

# Check for memory leaks
./scripts/valgrind.sh
```

### Generating Documentation

```bash
# Generate API documentation
./scripts/docs/docs.sh generate

# Serve documentation locally
./scripts/docs/docs.sh serve
```

## Project Structure

```text
calc/
├── CMakeLists.txt              # Main CMake configuration
├── include/calc/               # Public headers
│   ├── core/                   # Core components
│   ├── modes/                  # Calculator modes
│   ├── math/                   # Math utilities
│   ├── ui/cli/                 # CLI components
│   └── utils/                  # Utilities
├── src/                        # Source code
├── tests/                      # Tests
│   ├── unit/                   # Unit tests
│   ├── integration/            # Integration tests
│   └── benchmark/             # Performance benchmarks
├── docs/                       # Documentation
│   ├── API.md                  # API reference
│   ├── USER_GUIDE.md           # User guide
│   └── DESIGN.md               # Design document
├── scripts/                    # Build and utility scripts
└── cmake/                      # CMake modules
```

## Documentation

- [API Reference](docs/API.md) - Detailed API documentation
- [User Guide](docs/USER_GUIDE.md) - Complete user documentation
- [Design Document](docs/DESIGN.md) - Design decisions and architecture
- [Architecture](ARCHITECTURE.md) - System architecture overview
- [Contributing](CONTRIBUTING.md) - Contribution guidelines

## Contributing

Contributions are welcome! Please read the [Contributing Guidelines](CONTRIBUTING.md) before submitting pull requests.

## License

Calc is released under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with [CMake](https://cmake.org/)
- Tested with [Google Test](https://google.github.io/googletest/)
- Documented with [Doxygen](https://www.doxygen.nl/)
