/**
 * @file evaluator.h
 * @brief Expression evaluator interface and result types
 */

#ifndef CALC_CORE_EVALUATOR_H
#define CALC_CORE_EVALUATOR_H

#include "calc/core/ast.h"
#include "calc/utils/error.h"
#include <cmath>
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

namespace calc {

/**
 * @brief Result of an evaluation operation
 *
 * Contains either a successful numeric result or error information.
 */
class EvaluationResult {
public:
    /**
     * @brief Construct a successful result
     * @param value The computed value
     */
    explicit EvaluationResult(double value);

    /**
     * @brief Construct an error result
     * @param code The error code
     * @param message The error message
     * @param position Optional position in input where error occurred
     */
    EvaluationResult(ErrorCode code, const std::string& message, size_t position = 0);

    /**
     * @brief Check if evaluation was successful
     * @return true if successful, false if error
     */
    bool isSuccess() const noexcept;

    /**
     * @brief Check if evaluation resulted in an error
     * @return true if error, false if successful
     */
    bool isError() const noexcept;

    /**
     * @brief Get the computed value
     * @return The numeric result
     * @throws std::runtime_error if result is an error
     */
    double getValue() const;

    /**
     * @brief Get the error code
     * @return The error code
     * @throws std::runtime_error if result is successful
     */
    ErrorCode getErrorCode() const;

    /**
     * @brief Get the error message
     * @return The error message
     * @throws std::runtime_error if result is successful
     */
    const std::string& getErrorMessage() const;

    /**
     * @brief Get the error position
     * @return The position index
     * @throws std::runtime_error if result is successful
     */
    size_t getErrorPosition() const;

    /**
     * @brief Convert result to string representation
     * @return String representation of the result
     */
    std::string toString() const;

private:
    struct Success {
        double value;
    };

    struct Error {
        ErrorCode code;
        std::string message;
        size_t position;
    };

    std::variant<Success, Error> data_;
};

/**
 * @brief Context for evaluation operations
 *
 * Provides configuration and state for evaluation, such as
 * precision settings, function registries, and variable storage.
 */
class EvaluationContext {
public:
    /**
     * @brief Construct an evaluation context
     * @param precision Decimal precision for output formatting
     */
    explicit EvaluationContext(int precision = 6);

    /**
     * @brief Get the decimal precision
     * @return The precision setting
     */
    int getPrecision() const noexcept;

    /**
     * @brief Set the decimal precision
     * @param precision The new precision (must be >= 0)
     */
    void setPrecision(int precision);

    /**
     * @brief Check if a function is available
     * @param name The function name
     * @return true if the function exists
     */
    bool hasFunction(const std::string& name) const;

    /**
     * @brief Add a custom function to the context
     * @param name The function name
     * @param callback The function callback
     */
    void addFunction(const std::string& name, std::function<double(const std::vector<double>&)> callback);

    /**
     * @brief Call a function by name
     * @param name The function name
     * @param args The function arguments
     * @return Evaluation result
     */
    EvaluationResult callFunction(const std::string& name, const std::vector<double>& args);

private:
    int precision_;
    std::unordered_map<std::string, std::function<double(const std::vector<double>&)>> functions_;
};

/**
 * @brief Abstract evaluator interface
 *
 * Evaluates AST nodes and returns numeric results or errors.
 * Concrete implementations may use different evaluation strategies.
 */
class Evaluator {
public:
    virtual ~Evaluator() = default;

    /**
     * @brief Evaluate an AST node
     * @param node The node to evaluate
     * @param context The evaluation context
     * @return The evaluation result
     */
    virtual EvaluationResult evaluate(const ASTNode* node, EvaluationContext& context) = 0;

    /**
     * @brief Evaluate an AST node with default context
     * @param node The node to evaluate
     * @return The evaluation result
     */
    EvaluationResult evaluate(const ASTNode* node);
};

/**
 * @brief Visitor-based evaluator implementation
 *
 * Traverses the AST using the Visitor pattern and evaluates expressions.
 * Handles arithmetic operators, function calls, and error conditions.
 */
class EvaluatorVisitor : public ASTVisitor, public Evaluator {
public:
    /**
     * @brief Construct an evaluator visitor
     */
    EvaluatorVisitor();

    /**
     * @brief Evaluate an AST node
     * @param node The node to evaluate
     * @param context The evaluation context
     * @return The evaluation result
     */
    EvaluationResult evaluate(const ASTNode* node, EvaluationContext& context) override;

    // ASTVisitor implementation
    void visit(LiteralNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(UnaryOpNode& node) override;
    void visit(FunctionCallNode& node) override;

    /**
     * @brief Get the last evaluation result
     * @return The last result
     * @throws std::runtime_error if no evaluation has been performed
     */
    EvaluationResult getResult() const;

    /**
     * @brief Reset the evaluator state
     */
    void reset();

private:
    EvaluationResult result_;
    EvaluationContext* context_;

    /**
     * @brief Perform binary operation
     * @param left Left operand value
     * @param op The operator
     * @param right Right operand value
     * @return Result of the operation
     */
    EvaluationResult evaluateBinaryOp(double left, const Token& op, double right);

    /**
     * @brief Perform unary operation
     * @param op The operator
     * @param operand The operand value
     * @return Result of the operation
     */
    EvaluationResult evaluateUnaryOp(const Token& op, double operand);

    /**
     * @brief Check if two doubles are approximately equal
     * @param a First value
     * @param b Second value
     * @param epsilon Tolerance
     * @return true if approximately equal
     */
    static bool approxEqual(double a, double b, double epsilon = 1e-10);
};

/**
 * @brief Built-in mathematical functions for evaluation
 *
 * Provides standard mathematical functions that are available
 * in all evaluation contexts by default.
 */
class MathFunctions {
public:
    /**
     * @brief Register all built-in math functions to a context
     * @param context The context to register functions to
     */
    static void registerBuiltInFunctions(EvaluationContext& context);

    // Trigonometric functions
    static double sin(double x);
    static double cos(double x);
    static double tan(double x);
    static double asin(double x);
    static double acos(double x);
    static double atan(double x);

    // Hyperbolic functions
    static double sinh(double x);
    static double cosh(double x);
    static double tanh(double x);

    // Logarithmic and exponential functions
    static double log(double x);
    static double log10(double x);
    static double exp(double x);
    static double sqrt(double x);
    static double cbrt(double x);
    static double pow(double x, double y);

    // Rounding and absolute functions
    static double abs(double x);
    static double floor(double x);
    static double ceil(double x);
    static double round(double x);
    static double trunc(double x);

    // Other functions
    static double fmod(double x, double y);
    static double remainder(double x, double y);
    static double max(double a, double b);
    static double min(double a, double b);
    static double hypot(double x, double y);
};

} // namespace calc

#endif // CALC_CORE_EVALUATOR_H
