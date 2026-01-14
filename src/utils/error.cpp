/**
 * @file error.cpp
 * @brief Implementation of error handling classes
 */

#include "calc/utils/error.h"
#include <sstream>

namespace calc {

std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::INVALID_SYNTAX:
            return "Invalid Syntax";
        case ErrorCode::UNEXPECTED_TOKEN:
            return "Unexpected Token";
        case ErrorCode::DIVISION_BY_ZERO:
            return "Division by Zero";
        case ErrorCode::INVALID_FUNCTION:
            return "Invalid Function";
        case ErrorCode::DOMAIN_ERROR:
            return "Domain Error";
        case ErrorCode::NUMERIC_OVERFLOW:
            return "Overflow";
        case ErrorCode::NUMERIC_UNDERFLOW:
            return "Underflow";
        case ErrorCode::INVALID_BASE:
            return "Invalid Base";
        case ErrorCode::PARSE_ERROR:
            return "Parse Error";
        case ErrorCode::EVALUATION_ERROR:
            return "Evaluation Error";
        case ErrorCode::UNKNOWN_ERROR:
        default:
            return "Unknown Error";
    }
}

// CalculatorException implementation

CalculatorException::CalculatorException(ErrorCode code, const std::string& message, size_t position)
    : std::runtime_error(message), code_(code), position_(position) {
}

ErrorCode CalculatorException::getErrorCode() const noexcept {
    return code_;
}

size_t CalculatorException::getPosition() const noexcept {
    return position_;
}

// SyntaxError implementation

SyntaxError::SyntaxError(const std::string& message, size_t position)
    : CalculatorException(ErrorCode::INVALID_SYNTAX, message, position) {
}

// EvaluationError implementation

EvaluationError::EvaluationError(ErrorCode code, const std::string& message, size_t position)
    : CalculatorException(code, message, position) {
}

// DivisionByZeroError implementation

DivisionByZeroError::DivisionByZeroError(size_t position)
    : EvaluationError(ErrorCode::DIVISION_BY_ZERO, "Division by zero", position) {
}

// DomainError implementation

DomainError::DomainError(const std::string& message, size_t position)
    : EvaluationError(ErrorCode::DOMAIN_ERROR, message, position) {
}

} // namespace calc
