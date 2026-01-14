/**
 * @file error.h
 * @brief Error handling and exception types for the calc calculator
 */

#ifndef CALC_UTILS_ERROR_H
#define CALC_UTILS_ERROR_H

#include <string>
#include <stdexcept>

namespace calc {

/**
 * @brief Enumeration of error codes for calculator operations
 */
enum class ErrorCode {
    INVALID_SYNTAX,      ///< Invalid expression syntax
    UNEXPECTED_TOKEN,    ///< Unexpected token in expression
    DIVISION_BY_ZERO,   ///< Division by zero attempted
    INVALID_FUNCTION,   ///< Unknown or invalid function name
    DOMAIN_ERROR,       ///< Mathematical domain error (e.g., sqrt(-1))
    NUMERIC_OVERFLOW,   ///< Numeric overflow
    NUMERIC_UNDERFLOW,  ///< Numeric underflow
    INVALID_BASE, ///< Invalid numeric base for conversion
    PARSE_ERROR,        ///< General parsing error
    EVALUATION_ERROR,   ///< General evaluation error
    UNKNOWN_ERROR       ///< Unknown error type
};

/**
 * @brief Convert error code to human-readable string
 * @param code The error code to convert
 * @return String representation of the error code
 */
std::string errorCodeToString(ErrorCode code);

/**
 * @brief Custom exception class for calculator errors
 */
class CalculatorException : public std::runtime_error {
public:
    /**
     * @brief Construct a calculator exception
     * @param code The error code
     * @param message The error message
     * @param position Optional position in input string where error occurred
     */
    explicit CalculatorException(ErrorCode code, const std::string& message, size_t position = 0);

    /**
     * @brief Get the error code
     * @return The error code associated with this exception
     */
    ErrorCode getErrorCode() const noexcept;

    /**
     * @brief Get the position where the error occurred
     * @return The position index (0 if not applicable)
     */
    size_t getPosition() const noexcept;

private:
    ErrorCode code_;
    size_t position_;
};

/**
 * @brief Exception thrown for syntax errors during tokenization or parsing
 */
class SyntaxError : public CalculatorException {
public:
    explicit SyntaxError(const std::string& message, size_t position = 0);
};

/**
 * @brief Exception thrown for evaluation errors
 */
class EvaluationError : public CalculatorException {
public:
    explicit EvaluationError(ErrorCode code, const std::string& message, size_t position = 0);
};

/**
 * @brief Exception thrown for division by zero
 */
class DivisionByZeroError : public EvaluationError {
public:
    explicit DivisionByZeroError(size_t position = 0);
};

/**
 * @brief Exception thrown for domain errors (e.g., sqrt of negative number)
 */
class DomainError : public EvaluationError {
public:
    explicit DomainError(const std::string& message, size_t position = 0);
};

} // namespace calc

#endif // CALC_UTILS_ERROR_H
