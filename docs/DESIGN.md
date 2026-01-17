# Calc Design Document

## Table of Contents

- [Introduction](#introduction)
- [System Overview](#system-overview)
- [Architectural Decisions](#architectural-decisions)
- [Module Design](#module-design)
  - [Core Layer](#core-layer)
  - [Application Layer](#application-layer)
  - [Foundation Layer](#foundation-layer)
  - [Presentation Layer](#presentation-layer)
- [Design Patterns](#design-patterns)
- [Data Structures](#data-structures)
- [Algorithms](#algorithms)
- [Error Handling Strategy](#error-handling-strategy)
- [Extensibility](#extensibility)
- [Performance Considerations](#performance-considerations)
- [Testing Strategy](#testing-strategy)

---

## Introduction

This document describes the design decisions and technical details of Calc, a cross-platform command-line calculator built with C++17.

### Design Goals

1. **Modularity**: Clear separation between core engine and UI layer
2. **Extensibility**: Easy to add new modes, functions, and UI frontends
3. **Performance**: Efficient parsing and evaluation
4. **Cross-platform**: Works on Windows, macOS, and Linux
5. **Maintainability**: Clean code with comprehensive testing

---

## System Overview

Calc follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────┐
│         Presentation Layer              │
│  (CLI, future Qt GUI, other UIs)     │
└────────────┬────────────────────────────┘
             │
┌────────────┴────────────────────────────┐
│         Application Layer               │
│  (StandardMode, ScientificMode,       │
│   ProgrammerMode, ModeManager)         │
└────────────┬────────────────────────────┘
             │
┌────────────┴────────────────────────────┐
│           Core Layer                   │
│  (Tokenizer, Parser, Evaluator, AST)    │
└────────────┬────────────────────────────┘
             │
┌────────────┴────────────────────────────┐
│        Foundation Layer                │
│  (Error Handling, Math Functions,      │
│   Converter, Utilities)               │
└─────────────────────────────────────────┘
```

---

## Architectural Decisions

### 1. AST-Based Evaluation

**Decision**: Use Abstract Syntax Tree (AST) for intermediate representation.

**Rationale**:
- Enables multiple evaluation passes (optimization, type checking)
- Simplifies error reporting with precise position tracking
- Supports easy transformation and analysis

**Trade-offs**:
- Slightly higher memory usage than immediate evaluation
- More complex initial implementation

### 2. Visitor Pattern for Evaluation

**Decision**: Use Visitor pattern for AST evaluation.

**Rationale**:
- Separates traversal logic from node structure
- Makes it easy to add new operations (code generation, optimization)
- Follows Open/Closed Principle

**Implementation**:
```cpp
class ASTVisitor {
public:
    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
    // ... other node types
};

class EvaluatorVisitor : public ASTVisitor {
    // Evaluate each node type
};
```

### 3. Strategy Pattern for Modes

**Decision**: Each calculator mode is a strategy implementing a common interface.

**Rationale**:
- Runtime mode switching without recompilation
- Easy addition of new modes
- Encapsulates mode-specific behavior

### 4. Shunting-yard + Recursive Descent Parsers

**Decision**: Provide two parsing algorithms.

**Rationale**:
- Shunting-yard: Efficient for standard expressions
- Recursive descent: More flexible for complex grammars
- Users can choose based on their needs

### 5. Result Type with Error Information

**Decision**: Use a `EvaluationResult` type that can contain success or error.

**Rationale**:
- Avoids exceptions for control flow
- Preserves error information (code, message, position)
- Enables functional-style error handling

---

## Module Design

### Core Layer

#### Tokenizer

**Purpose**: Convert input strings into a stream of tokens.

**Algorithm**:
1. Iterate through input character by character
2. Skip whitespace
3. Identify token type (number, operator, function, etc.)
4. For numbers: handle decimal points, scientific notation, base prefixes
5. For identifiers: collect alphanumeric sequences
6. For operators: handle multi-character operators if needed

**Complexity**: O(n) where n is input length

#### Parser

**Purpose**: Build AST from token stream.

**Shunting-yard Algorithm**:
1. Use operator stack for operators and function calls
2. Use output queue for operands
3. When encountering an operator, pop higher-precedence operators to output
4. At the end, pop remaining operators
5. Build AST from the RPN notation

**Recursive Descent Algorithm**:
1. Define grammar rules as functions
2. `parseExpression()`: calls `parseTerm()` and handles +/- operators
3. `parseTerm()`: calls `parseFactor()` and handles */ operators
4. `parseFactor()`: handles numbers, parentheses, function calls, unary operators

**Operator Precedence** (highest to lowest):
1. Parentheses `( )`
2. Unary operators `- +`
3. Exponentiation `^` (right-associative)
4. Multiplication/Division/Modulus `* / %`
5. Addition/Subtraction `+ -`

#### Evaluator

**Purpose**: Compute the numeric value of an AST.

**Algorithm**:
1. Traverse AST using visitor pattern
2. For literal nodes: return the value
3. For binary operators: evaluate left and right, apply operator
4. For unary operators: evaluate operand, apply operator
5. For function calls: evaluate all arguments, call function

**Complexity**: O(n) where n is number of nodes in AST

### Application Layer

#### Mode Base Class

**Interface**:
```cpp
class Mode {
public:
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual EvaluationResult evaluate(const std::string& expression) = 0;
    virtual EvaluationContext& getContext() = 0;
};
```

**StandardMode**:
- Uses Shunting-yard parser by default
- Supports: +, -, *, /, ^, ( )
- Precision configurable

**ScientificMode**:
- Inherits from StandardMode
- Adds mathematical functions
- Adds constants PI and E
- Registers MathFunctions to EvaluationContext

**ProgrammerMode**:
- Custom parser for bitwise operations
- Different operator semantics for `^` (XOR vs power)
- Base prefix support (0b, 0o, 0x)
- Output formatting in different bases

#### ModeManager

**Purpose**: Central registry and switch for calculator modes.

**Implementation**:
```cpp
class ModeManager {
    std::unordered_map<std::string, std::unique_ptr<Mode>> modes_;
    Mode* currentMode_;
};
```

### Foundation Layer

#### Error Handling

**Exception Hierarchy**:
```
std::runtime_error
    └── CalculatorException
            ├── SyntaxError
            └── EvaluationError
                    ├── DivisionByZeroError
                    └── DomainError
```

**Error Codes**: Defined in `enum class ErrorCode`

**Position Tracking**: All errors include position in input string for better error messages

#### Math Functions

**Implementation**: Static methods wrapping `<cmath>` functions

**Registration**: `MathFunctions::registerBuiltInFunctions()` adds all functions to EvaluationContext

**Error Handling**: Domain errors (sqrt(-1), log(0), etc.) throw DomainError

#### Converter

**Purpose**: Convert between different number bases.

**Algorithms**:
- `decimalToBinary`: Repeated division by 2
- `decimalToHex`: Repeated division by 16
- `binaryToDecimal`: Positional weighting
- `hexToDecimal`: Positional weighting

### Presentation Layer

#### CLI Application

**Components**:
- `CliApp`: Main application class
- `CommandParser`: Argument parsing
- `OutputFormatter`: Result formatting with color
- `HistoryManager`: Calculation history

**REPL Mode**:
- Read: Get input from user
- Eval: Parse and evaluate expression
- Print: Display result or error
- Loop: Repeat until quit command

---

## Design Patterns

### 1. Strategy Pattern
- **Used**: Calculator modes (Standard, Scientific, Programmer)
- **Benefit**: Runtime mode switching

### 2. Factory Pattern
- **Used**: Parser creation in modes
- **Benefit**: Encapsulated object creation

### 3. Visitor Pattern
- **Used**: AST evaluation
- **Benefit**: Separate traversal from node structure

### 4. Singleton Pattern
- **Used**: ModeManager (implicitly through design)
- **Benefit**: Single point of mode management

### 5. Template Method Pattern
- **Used**: Mode base class with virtual methods
- **Benefit**: Define algorithm skeleton, let subclasses vary

---

## Data Structures

### Token

```cpp
struct Token {
    TokenType type;
    std::string value;
    size_t position;
    size_t argCount;
    NumberBase numberBase;
};
```

### AST Node Hierarchy

```
ASTNode (abstract)
├── LiteralNode
├── BinaryOpNode
│   ├── left: unique_ptr<ASTNode>
│   ├── op: Token
│   └── right: unique_ptr<ASTNode>
├── UnaryOpNode
│   ├── op: Token
│   └── operand: unique_ptr<ASTNode>
└── FunctionCallNode
    ├── name: string
    ├── position: size_t
    └── args: vector<unique_ptr<ASTNode>>
```

### EvaluationResult

```cpp
class EvaluationResult {
    std::variant<Success, Error> data_;
};
```

---

## Algorithms

### Tokenization

**Complexity**: O(n) time, O(1) extra space (excluding output)

**Edge Cases Handled**:
- Scientific notation: `1.5e-3`
- Multiple decimal points: error
- Invalid base prefixes: error
- Unknown characters: error

### Shunting-yard Parsing

**Complexity**: O(n) time, O(n) space for operator stack

**Operator Precedence Handling**:
- Higher precedence operators evaluated first
- Right-associative operators (^) handled specially

### Recursive Descent Parsing

**Complexity**: O(n) time, O(depth) stack space where depth is expression nesting

**Grammar**:
```
expression  -> term ((+|-) term)*
term        -> factor ((*|/|%) factor)*
factor      -> power (^ factor) | unary
unary       -> (+|-) unary | primary
primary     -> number | '(' expression ')' | function_call
```

### AST Evaluation

**Complexity**: O(n) time, O(depth) stack space

**Error Propagation**: Errors bubble up through the evaluation tree

---

## Error Handling Strategy

### Philosophy

1. **Use exceptions for exceptional cases**: Syntax errors, evaluation errors
2. **Return Result objects for expected failures**: Division by zero, domain errors
3. **Include position information**: Always track where error occurred
4. **Provide helpful messages**: Explain what went wrong and possibly why

### Error Types

| Error Type | When Thrown | Severity |
|------------|-------------|----------|
| SyntaxError | Invalid token sequence | High |
| DivisionByZeroError | Attempted division by zero | High |
| DomainError | Math function outside domain | High |
| EvaluationError | General evaluation problem | Medium |

### Error Reporting

```cpp
// Example error message:
Error: Division by zero at position 8
Expression: 10 / 0
             ^
```

---

## Extensibility

### Adding a New Calculator Mode

1. Create new class inheriting from `Mode`
2. Implement required virtual methods
3. Register with `ModeManager`
4. Add mode-specific tests

### Adding a New Function

```cpp
// Add to EvaluationContext
context.addFunction("myfunc", [](const std::vector<double>& args) -> double {
    // Implementation
    return result;
});
```

### Adding a New UI Layer

1. Create new UI class (e.g., `QtGui`)
2. Use existing `Mode` classes
3. Call `Mode::evaluate()` to process expressions
4. Format results as needed

---

## Performance Considerations

### Memory Management

- Use `unique_ptr` for AST node ownership
- Move semantics for large objects
- Reserve capacity for vectors when size is known

### Evaluation Speed

- Single-pass evaluation via visitor pattern
- No temporary allocations during evaluation
- Inlined operator functions for performance

### Caching Opportunities

1. **Parser result caching**: Cache parsed AST for repeated expressions
2. **Function result caching**: Memoization for pure functions
3. **Constant folding**: Evaluate constant expressions at parse time

---

## Testing Strategy

### Unit Tests

- Test each component in isolation
- Use Google Test framework
- Mock dependencies when needed

### Integration Tests

- Test interaction between components
- Test full expression evaluation pipeline
- Test mode switching

### Performance Tests

- Benchmark parsing speed
- Benchmark evaluation speed
- Memory usage profiling

### Coverage Goals

- Target: >90% code coverage
- Critical paths: 100% coverage
- Error paths: Tested explicitly

---

## Future Enhancements

### Short Term

1. Variable storage and references
2. Custom function definitions
3. Expression optimization (constant folding)
4. Symbolic computation

### Long Term

1. Graphing functions
2. Unit conversion
3. Complex number support
4. Plugin architecture
