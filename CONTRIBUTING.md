# Contributing to Calc

Thank you for your interest in contributing to Calc! This document provides guidelines and instructions for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Testing](#testing)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)
- [Release Process](#release-process)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors.

### Our Standards

- Use welcoming and inclusive language
- Respect differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community

### Our Responsibilities

Project maintainers are responsible for clarifying standards of acceptable behavior and take appropriate action when unacceptable behavior occurs.

---

## Getting Started

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- Git
- Google Test (automatically fetched)

### Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/yourusername/calc.git
cd calc

# Configure build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

### Recommended Tools

- **IDE**: Visual Studio Code, CLion, or any C++ IDE
- **Code Formatting**: clang-format (config provided)
- **Static Analysis**: clang-tidy, cppcheck
- **Memory Checking**: Valgrind (Linux)
- **Documentation**: Doxygen

---

## Development Workflow

### 1. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/your-bug-fix
```

### 2. Make Changes

Follow the coding standards below, write tests, and update documentation.

### 3. Run Tests

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test
./build/tests/unit/tokenizer_test

# Run with coverage
./scripts/build.sh --coverage
```

### 4. Run Code Quality Checks

```bash
# Format code
./scripts/format.sh

# Run clang-tidy
./scripts/run-clang-tidy.sh

# Run cppcheck
./scripts/run-cppcheck.sh
```

### 5. Commit Changes

Follow the commit guidelines below.

### 6. Push and Create Pull Request

```bash
git push origin feature/your-feature-name
```

Then create a pull request on GitHub.

---

## Coding Standards

### C++ Style

We follow modern C++17 best practices:

#### Naming Conventions

| Type | Convention | Example |
|------|-------------|---------|
| Classes | PascalCase | `class Tokenizer` |
| Functions | camelCase | `void tokenize()` |
| Variables | camelCase | `int position;` |
| Constants | UPPER_CASE | `const int MAX_SIZE = 100;` |
| Private members | trailing_underscore | `int position_;` |
| File names | snake_case | `tokenizer.h`, `tokenizer.cpp` |

#### Best Practices

1. **Use `auto` wisely**: Use when type is obvious, explicit otherwise
   ```cpp
   auto tokens = tokenizer.tokenize();  // Good
   auto it = vec.begin();              // Good
   auto x = compute();                // Avoid if type not obvious
   ```

2. **Prefer const correctness**
   ```cpp
   const Token& getOperator() const noexcept { return op_; }
   ```

3. **Use smart pointers for ownership**
   ```cpp
   std::unique_ptr<ASTNode> node;
   std::shared_ptr<Resource> resource;  // Only when shared ownership needed
   ```

4. **Prefer references over pointers for parameters**
   ```cpp
   void process(const std::string& input);  // Preferred
   void process(const std::string* input);  // Only if null is valid
   ```

5. **Use `noexcept` where appropriate**
   ```cpp
   bool isOperator() const noexcept { return type_ == TokenType::OPERATOR; }
   ```

#### Code Organization

- One class per file pair (header + implementation)
- Headers in `include/calc/`, implementations in `src/`
- Private implementation details in `.cpp` files
- Inline only small, frequently used functions

### Documentation Style

Use Doxygen-style comments:

```cpp
/**
 * @brief Brief description of the function
 *
 * Detailed description of what the function does.
 * Can span multiple lines.
 *
 * @param param1 Description of parameter 1
 * @param param2 Description of parameter 2
 * @return Description of return value
 * @throws ExceptionType When something goes wrong
 */
int exampleFunction(int param1, const std::string& param2);
```

### Formatting

Run `./scripts/format.sh` before committing. This applies the project's clang-format configuration.

---

## Commit Guidelines

### Commit Message Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

#### Types

| Type | Description |
|------|-------------|
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation changes |
| `style` | Code style changes (formatting, etc.) |
| `refactor` | Code refactoring |
| `perf` | Performance improvements |
| `test` | Adding or updating tests |
| `chore` | Build process or auxiliary tool changes |

#### Subject

- Use imperative mood ("add" not "added", "fix" not "fixed")
- Don't capitalize first letter
- Don't end with period
- Limit to 50 characters

#### Body

- Use imperative mood
- Wrap at 72 characters
- Explain **what** and **why**, not **how**

#### Footer

- Reference issues (`Closes #123`, `Refs #456`)
- List breaking changes (`BREAKING CHANGE:`)

### Examples

```
feat(tokenizer): add support for octal numbers

Add 0o prefix support for octal number literals.
This enables程序员 mode to work with octal values.

Closes #42
```

```
fix(parser): handle nested parentheses correctly

The recursive descent parser was incorrectly
tracking parenthesis depth, causing expressions
like "((2+3)*4)" to fail parsing.

Fixes #78
```

```
docs(api): update Evaluator documentation

Added examples for using custom functions
and clarified error handling behavior.
```

---

## Testing

### Writing Tests

Tests use Google Test framework. Place tests in the `tests/` directory.

```cpp
#include <gtest/gtest.h>
#include "calc/core/tokenizer.h"

TEST(TokenizerTest, BasicNumbers) {
    calc::Tokenizer tokenizer("123 456");
    auto tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].value, "123");
    EXPECT_EQ(tokens[1].value, "456");
}

TEST(TokenizerTest, InvalidNumber) {
    calc::Tokenizer tokenizer("123abc");
    EXPECT_THROW(tokenizer.tokenize(), calc::SyntaxError);
}
```

### Test Organization

- `tests/unit/`: Unit tests for individual components
- `tests/integration/`: Integration tests for component interaction
- `tests/benchmark/`: Performance benchmarks

### Test Requirements

- All new features must have tests
- Bug fixes must include regression tests
- Maintain >90% code coverage

---

## Documentation

### When to Update Documentation

- Adding new public API → Update API documentation
- Changing behavior → Update user guide
- Architecture changes → Update design document
- New features → Update README and CHANGELOG

### Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Project overview and quick start |
| `CHANGELOG.md` | Version history |
| `docs/API.md` | API reference |
| `docs/USER_GUIDE.md` | User documentation |
| `docs/DESIGN.md` | Design decisions |
| `ARCHITECTURE.md` | System architecture |

---

## Pull Request Process

### Before Submitting

1. [ ] Code follows coding standards
2. [ ] All tests pass (`ctest`)
3. [ ] Code formatted (`./scripts/format.sh`)
4. [ ] Static analysis passes (`clang-tidy`, `cppcheck`)
5. [ ] Documentation updated
6. [ ] Commit messages follow guidelines

### PR Description Template

```markdown
## Description
Brief description of changes.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] All tests pass

## Checklist
- [ ] Code follows style guidelines
- [ ] Documentation updated
- [ ] CHANGELOG updated (if applicable)
- [ ] No merge conflicts
```

### Review Process

1. Automated checks must pass (CI/CD)
2. At least one maintainer approval required
3. Address all review comments
4. Update PR description if needed

---

## Release Process

### Version Numbering

Follow Semantic Versioning (MAJOR.MINOR.PATCH):

- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality (backwards compatible)
- **PATCH**: Bug fixes

### Creating a Release

1. Update `CHANGELOG.md` for the new version
2. Update version in `CMakeLists.txt`
3. Create release branch: `git checkout -b release/v1.0.0`
4. Update documentation as needed
5. Merge to main branch
6. Tag the release: `git tag -a v1.0.0 -m "Release v1.0.0"`
7. Push tag: `git push origin v1.0.0`
8. GitHub Actions will automatically:
   - Build for all platforms
   - Run all tests
   - Create packages
   - Generate documentation
   - Create GitHub release

---

## Questions?

If you have questions about contributing:

- Check existing issues and pull requests
- Open a new issue for bugs or feature requests
- Start a discussion for questions
- Contact maintainers directly (see MAINTAINERS.md)

---

## License

By contributing to Calc, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to Calc!
