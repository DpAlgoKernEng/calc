# Calc API Reference

## Overview

Calc is a cross-platform command-line calculator built with C++17. This document provides an API reference for developers who wish to integrate the calculator engine into their applications.

## Table of Contents

- [Core Layer](#core-layer)
  - [Token System](#token-system)
  - [Tokenizer](#tokenizer)
  - [AST (Abstract Syntax Tree)]#ast)
  - [Parser Interface](#parser-interface)
  - [Evaluator](#evaluator)
- [Application Layer](#application-layer)
  - [Mode Base Class](#mode-base-class)
  - [Standard Mode](#standard-mode)
  - [Scientific Mode](#scientific-mode)
  - [Programmer Mode](#programmer-mode)
  - [Mode Manager](#mode-manager)
- [Foundation Layer](#foundation-layer)
  - [Error Handling](#error-handling)
  - [Math Functions](#math-functions)
  - [Converter](#converter)
- [UI Layer](#ui-layer)
  - [CLI Application](#cli-application)
  - [Command Parser](#command-parser)
  - [Output Formatter](#output-formatter)
  - [History Manager](#history-manager)

---

## Core Layer

### Token System

The token system provides lexical analysis capabilities for mathematical expressions.

#### TokenType

```cpp
enum class TokenType {
    NUMBER,          ///< Numeric literal (integer or floating-point)
    OPERATOR,        ///< Arithmetic operator (+, -, *, /, ^, etc.)
    FUNCTION,        ///< Function name (sin, cos, sqrt, etc.)
    LPAREN,          ///< Left parenthesis '('
    RPAREN,          ///< Right parenthesis ')'
    COMMA,           ///< Comma separator for function arguments
    EOF_TOKEN,       ///< End of input marker
    UNKNOWN          ///< Unrecognized token
};
```

#### Token

```cpp
struct Token {
    TokenType type;      ///< The type of this token
    std::string value;   ///< The string value of this token
    size_t position;     ///< Starting position in original input string
    size_t argCount;     ///< Number of arguments for function tokens (default 0)
    NumberBase numberBase; ///< Number base for number tokens (default: DECIMAL)
};
```

**Methods:**
- `bool isOperator() const noexcept` - Check if this token is an operator
- `bool isNumber() const noexcept` - Check if this token is a number
- `bool isFunction() const noexcept` - Check if this token is a function
- `int getPrecedence() const` - Get operator precedence level
- `bool isRightAssociative() const` - Check if operator is right-associative

### Tokenizer

Converts input strings into streams of lexical tokens.

```cpp
class Tokenizer {
public:
    explicit Tokenizer(const std::string& input);
    std::vector<Token> tokenize();
};
```

**Usage Example:**
```cpp
calc::Tokenizer tokenizer("2 + 3 * sin(45)");
auto tokens = tokenizer.tokenize();
```

### AST (Abstract Syntax Tree)

Abstract syntax tree nodes for expression representation.

#### ASTNode

Base class for all AST nodes.

```cpp
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::unique_ptr<ASTNode> clone() const = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::string toString() const = 0;
};
```

#### LiteralNode

Represents a numeric literal.

```cpp
class LiteralNode : public ASTNode {
public:
    explicit LiteralNode(double value);
    double getValue() const noexcept;
};
```

#### BinaryOpNode

Represents a binary operation (e.g., a + b, x * y).

```cpp
class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(std::unique_ptr<ASTNode> left, Token op, std::unique_ptr<ASTNode> right);
    ASTNode* getLeft() const noexcept;
    const Token& getOperator() const noexcept;
    ASTNode* getRight() const noexcept;
};
```

#### UnaryOpNode

Represents a unary operation (e.g., -x, +x).

```cpp
class UnaryOpNode : public ASTNode {
public:
    UnaryOpNode(Token op, std::unique_ptr<ASTNode> operand);
    const Token& getOperator() const noexcept;
    ASTNode* getOperand() const noexcept;
};
```

#### FunctionCallNode

Represents a function call (e.g., sin(x), max(a, b, c)).

```cpp
class FunctionCallNode : public ASTNode {
public:
    FunctionCallNode(const std::string& name, size_t position,
                     std::vector<std::unique_ptr<ASTNode>> args);
    const std::string& getName() const noexcept;
    size_t getArgumentCount() const noexcept;
    ASTNode* getArgument(size_t index) const;
};
```

### Parser Interface

Abstract base class for parsers.

```cpp
class Parser {
public:
    virtual ~Parser() = default;
    virtual std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) = 0;
    virtual std::string getName() const = 0;
};
```

### Evaluator

Expression evaluation with context and result handling.

#### EvaluationResult

Result of an evaluation operation.

```cpp
class EvaluationResult {
public:
    explicit EvaluationResult(double value);
    EvaluationResult(ErrorCode code, const std::string& message, size_t position = 0);

    bool isSuccess() const noexcept;
    bool isError() const noexcept;
    double getValue() const;
    ErrorCode getErrorCode() const;
    const std::string& getErrorMessage() const;
};
```

#### EvaluationContext

Context for evaluation operations.

```cpp
class EvaluationContext {
public:
    explicit EvaluationContext(int precision = 6);

    int getPrecision() const noexcept;
    void setPrecision(int precision);

    bool hasFunction(const std::string& name) const;
    void addFunction(const std::string& name,
                    std::function<double(const std::vector<double>&)> callback);
    EvaluationResult callFunction(const std::string& name, const std::vector<double>& args);

    OperatorSemantics getOperatorSemantics(const std::string& op) const;
    void setOperatorSemantics(const std::string& op, OperatorSemantics semantics);
};
```

#### EvaluatorVisitor

Visitor-based evaluator implementation.

```cpp
class EvaluatorVisitor : public ASTVisitor, public Evaluator {
public:
    EvaluatorVisitor();
    EvaluationResult evaluate(const ASTNode* node, EvaluationContext& context) override;
    EvaluationResult getResult() const;
    void reset();
};
```

---

## Application Layer

### Mode Base Class

Abstract base class for calculator modes.

```cpp
class Mode {
public:
    virtual ~Mode() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual EvaluationResult evaluate(const std::string& expression) = 0;
    virtual EvaluationContext& getContext() = 0;
    virtual const EvaluationContext& getContext() const = 0;
};
```

### Standard Mode

Standard calculator mode for basic arithmetic.

```cpp
class StandardMode : public Mode {
public:
    explicit StandardMode(int precision = 6);

    std::string getName() const override;
    std::string getDescription() const override;
    EvaluationResult evaluate(const std::string& expression) override;

    void setPrecision(int precision);
    int getPrecision() const;
    std::string getParserType() const;
    void setParserType(bool useRecursiveDescent);
};
```

**Supported operators:** `+`, `-`, `*`, `/`, `^` (power)

**Operator precedence:** `^` (right-assoc) > `*,/` > `+,-`

### Scientific Mode

Scientific calculator mode with mathematical functions.

```cpp
class ScientificMode : public StandardMode {
public:
    explicit ScientificMode(int precision = 6);

    std::string getName() const override;
    std::string getDescription() const override;
};
```

**Supported functions:**
- Trigonometric: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- Hyperbolic: `sinh`, `cosh`, `tanh`
- Logarithmic: `log`, `log10`, `exp`
- Root functions: `sqrt`, `cbrt`
- Rounding: `abs`, `floor`, `ceil`, `round`, `trunc`
- Other: `fmod`, `hypot`, `max`, `min`, `remainder`

**Constants:** `PI`, `E`

### Programmer Mode

Programmer calculator mode for base conversions and bitwise operations.

```cpp
class ProgrammerMode : public Mode {
public:
    explicit ProgrammerMode(int precision = 6);

    std::string getName() const override;
    std::string getDescription() const override;
    EvaluationResult evaluate(const std::string& expression) override;

    void setPrecision(int precision);
    int getPrecision() const;
    NumberBase getNumberBase() const;
    void setNumberBase(NumberBase base);
};
```

**Supported number bases:**
- Binary: `0b1010` or `0B1010`
- Octal: `0o17` or `0O17`
- Hexadecimal: `0xFF` or `0XFF`
- Decimal: default

**Bitwise operators:** `&`, `|`, `^`, `~`, `<<`, `>>`

### Mode Manager

Central registry and manager for calculator modes.

```cpp
class ModeManager {
public:
    ModeManager();
    ~ModeManager();

    Mode* getMode(const std::string& name) const;
    std::vector<std::string> getAvailableModes() const;
    Mode* getCurrentMode() const;
    void setCurrentMode(const std::string& name);
    void addMode(const std::string& name, std::unique_ptr<Mode> mode);
};
```

---

## Foundation Layer

### Error Handling

#### ErrorCode

Enumeration of error codes.

```cpp
enum class ErrorCode {
    INVALID_SYNTAX,      ///< Invalid expression syntax
    UNEXPECTED_TOKEN,    ///< Unexpected token in expression
    DIVISION_BY_ZERO,   ///< Division by zero attempted
    INVALID_FUNCTION,   ///< Unknown or invalid function name
    DOMAIN_ERROR,       ///< Mathematical domain error (e.g., sqrt(-1))
    NUMERIC_OVERFLOW,   ///< Numeric overflow
    NUMERIC_UNDERFLOW,  ///< Numeric underflow
    INVALID_BASE,      ///< Invalid numeric base for conversion
    PARSE_ERROR,        ///< General parsing error
    EVALUATION_ERROR,   ///< General evaluation error
    UNKNOWN_ERROR       ///< Unknown error type
};
```

#### CalculatorException

Base exception class for calculator errors.

```cpp
class CalculatorException : public std::runtime_error {
public:
    explicit CalculatorException(ErrorCode code, const std::string& message, size_t position = 0);
    ErrorCode getErrorCode() const noexcept;
    size_t getPosition() const noexcept;
};
```

**Specialized exceptions:**
- `SyntaxError` - Syntax errors during tokenization or parsing
- `EvaluationError` - General evaluation errors
- `DivisionByZeroError` - Division by zero
- `DomainError` - Domain errors (e.g., sqrt of negative number)

### Math Functions

Built-in mathematical functions.

```cpp
class MathFunctions {
public:
    static void registerBuiltInFunctions(EvaluationContext& context);

    // Trigonometric
    static double sin(double x);
    static double cos(double x);
    static double tan(double x);
    static double asin(double x);
    static double acos(double x);
    static double atan(double x);

    // Hyperbolic
    static double sinh(double x);
    static double cosh(double x);
    static double tanh(double x);

    // Logarithmic and exponential
    static double log(double x);
    static double log10(double x);
    static double exp(double x);
    static double sqrt(double x);
    static double cbrt(double x);
    static double pow(double x, double y);

    // Rounding and absolute
    static double abs(double x);
    static double floor(double x);
    static double ceil(double x);
    static double round(double x);
    static double trunc(double x);

    // Other
    static double fmod(double x, double y);
    static double remainder(double x, double y);
    static double max(double a, double b);
    static double min(double a, double b);
    static double hypot(double x, double y);
};
```

### Converter

Base conversion utilities.

```cpp
class Converter {
public:
    static std::string decimalToBinary(long long value);
    static std::string decimalToHex(long long value);
    static std::string decimalToOctal(long long value);
    static long long binaryToDecimal(const std::string& binary);
    static long long hexToDecimal(const std::string& hex);
    static long long octalToDecimal(const std::string& octal);
    static std::string numberToBase(long long value, int base, bool isSigned = true);
};
```

---

## UI Layer

### CLI Application

Main CLI application class.

```cpp
class CliApp {
public:
    CliApp(int argc, char* argv[]);
    int run();

    static std::string trim(const std::string& str);
    static bool isREPLCommand(const std::string& line);
};
```

### Command Parser

Command-line argument parser.

```cpp
class CommandParser {
public:
    CommandParser(int argc, char* argv[]);
    CommandLineOptions parse();

    static void printHelp();
    static void printVersion();
};
```

**CommandLineOptions:**
```cpp
struct CommandLineOptions {
    std::string mode;
    std::vector<std::string> expressions;
    int precision = 6;
    bool showHelp = false;
    bool showVersion = false;
    bool interactive = false;
    bool verbose = false;
    bool noColor = false;
    NumberBase numberBase = NumberBase::DECIMAL;
};
```

### Output Formatter

Output formatting with ANSI color support.

```cpp
class OutputFormatter {
public:
    explicit OutputFormatter(bool useColor = true, int precision = 6);

    void setUseColor(bool useColor);
    void setPrecision(int precision);

    std::string formatResult(const EvaluationResult& result) const;
    std::string formatError(const EvaluationResult& result) const;
    std::string formatPrompt(const REPLState& state) const;
    std::string formatSuccess(const std::string& message) const;
    std::string formatWarning(const std::string& message) const;
    std::string formatError(const std::string& message) const;
    std::string formatInfo(const std::string& message) const;
    std::string formatMode(Mode* mode) const;
};
```

### History Manager

Calculation history management.

```cpp
class HistoryManager {
public:
    HistoryManager(size_t maxEntries = 100);
    ~HistoryManager();

    void addEntry(const HistoryEntry& entry);
    std::vector<HistoryEntry> getAllEntries() const;
    std::vector<HistoryEntry> searchEntries(const std::string& keyword) const;
    std::vector<HistoryEntry> getLastNEntries(size_t n) const;
    std::optional<HistoryEntry> getEntry(size_t index) const;
    void clear();
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    size_t size() const;
};
```

---

## Quick Start Examples

### Basic Evaluation

```cpp
#include "calc/modes/standard_mode.h"

calc::StandardMode mode;
auto result = mode.evaluate("2 + 3 * 4");
if (result.isSuccess()) {
    std::cout << "Result: " << result.getValue() << std::endl;
}
```

### Scientific Functions

```cpp
#include "calc/modes/scientific_mode.h"

calc::ScientificMode mode;
auto result = mode.evaluate("sin(PI/2) * 2");
if (result.isSuccess()) {
    std::cout << "Result: " << result.getValue() << std::endl;
}
```

### Programmer Mode

```cpp
#include "calc/modes/programmer_mode.h"

calc::ProgrammerMode mode;
mode.setNumberBase(calc::NumberBase::HEXADECIMAL);
auto result = mode.evaluate("0xFF & 0x0F");
if (result.isSuccess()) {
    std::cout << "Result: " << result.getValue() << std::endl;
}
```

### Custom Function

```cpp
#include "calc/modes/standard_mode.h"

calc::StandardMode mode;
auto& context = mode.getContext();

// Add a custom function
context.addFunction("square", [](const std::vector<double>& args) -> double {
    if (args.size() != 1) throw calc::EvaluationError(...);
    return args[0] * args[0];
});

auto result = mode.evaluate("square(5)");
```

---

## See Also

- [Architecture Documentation](../ARCHITECTURE.md)
- [User Guide](USER_GUIDE.md)
- [Design Document](DESIGN.md)
- [Full API Docs](./api/html/index.html) - Generated by Doxygen
