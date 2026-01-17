# Scripts Directory

This directory contains utility scripts for building, testing, and maintaining the calc project.

## Available Scripts

### Code Formatting

- **`format.sh`** - Format all C++ source files using clang-format
  ```bash
  ./scripts/format.sh
  ```

- **`check-format.sh`** - Check code formatting without modifying files (fails if not formatted)
  ```bash
  ./scripts/check-format.sh
  ```

### Static Analysis

- **`run-clang-tidy.sh`** - Run clang-tidy static analysis
  ```bash
  ./scripts/run-clang-tidy.sh
  ./scripts/run-clang-tidy.sh --fix           # Apply automatic fixes
  ./scripts/run-clang-tidy.sh --warnings-as-errors
  ./scripts/run-clang-tidy.sh --filter="src/core"
  ```

- **`run-cppcheck.sh`** - Run cppcheck static analysis
  ```bash
  ./scripts/run-cppcheck.sh
  ./scripts/run-cppcheck.sh --enable-all      # Enable all checks
  ./scripts/run-cppcheck.sh --warning-as-error
  ./scripts/run-cppcheck.sh --output=report.xml
  ```

### Code Coverage

- **`coverage.sh`** - Generate code coverage reports using lcov
  ```bash
  ./scripts/coverage.sh                      # Generate full report
  ./scripts/coverage.sh --target=unit         # Only unit tests
  ./scripts/coverage.sh --threshold=90        # Set minimum coverage threshold
  ./scripts/coverage.sh --open               # Open HTML report in browser
  ./scripts/coverage.sh --clean              # Clean build before coverage
  ```

### Memory Leak Detection

- **`valgrind.sh`** - Run tests with Valgrind memory leak detection
  ```bash
  ./scripts/valgrind.sh                    # Basic memory check
  ./scripts/valgrind.sh --leak-check=full   # Full leak check
  ./scripts/valgrind.sh --show-reachable    # Show reachable leaks
  ./scripts/valgrind.sh --verbose          # Verbose output
  ```

### Configuration Files

- **`valgrind.supp`** - Valgrind suppressions for false positives

## Quick Reference

| Task | Command |
|------|---------|
| Format code | `./scripts/format.sh` |
| Check formatting | `./scripts/check-format.sh` |
| Static analysis (clang-tidy) | `./scripts/run-clang-tidy.sh` |
| Static analysis (cppcheck) | `./scripts/run-cppcheck.sh` |
| Generate coverage report | `./scripts/coverage.sh` |
| Check memory leaks | `./scripts/valgrind.sh` |

## Environment Variables

All scripts support the following environment variables:

- `BUILD_DIR` - Build directory (default: `build`)

Example:
```bash
BUILD_DIR=build-debug ./scripts/coverage.sh
```

## CI/CD Integration

These scripts are integrated into the GitHub Actions workflows:

- `.github/workflows/ci.yml` - Main CI pipeline
- `.github/workflows/code-quality.yml` - Code quality checks
- `.github/workflows/coverage.yml` - Coverage reports
- `.github/workflows/benchmark.yml` - Performance benchmarks
