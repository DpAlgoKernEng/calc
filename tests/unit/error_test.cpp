/**
 * @file error_test.cpp
 * @brief Unit tests for error handling
 */

#include <gtest/gtest.h>
#include "calc/utils/error.h"

using namespace calc;

// Test ErrorCode to string conversion
TEST(ErrorTest, ErrorCodeToString) {
    EXPECT_EQ(errorCodeToString(ErrorCode::INVALID_SYNTAX), "Invalid Syntax");
    EXPECT_EQ(errorCodeToString(ErrorCode::UNEXPECTED_TOKEN), "Unexpected Token");
    EXPECT_EQ(errorCodeToString(ErrorCode::DIVISION_BY_ZERO), "Division by Zero");
    EXPECT_EQ(errorCodeToString(ErrorCode::INVALID_FUNCTION), "Invalid Function");
    EXPECT_EQ(errorCodeToString(ErrorCode::DOMAIN_ERROR), "Domain Error");
    EXPECT_EQ(errorCodeToString(ErrorCode::NUMERIC_OVERFLOW), "Overflow");
    EXPECT_EQ(errorCodeToString(ErrorCode::NUMERIC_UNDERFLOW), "Underflow");
    EXPECT_EQ(errorCodeToString(ErrorCode::INVALID_BASE), "Invalid Base");
    EXPECT_EQ(errorCodeToString(ErrorCode::PARSE_ERROR), "Parse Error");
    EXPECT_EQ(errorCodeToString(ErrorCode::EVALUATION_ERROR), "Evaluation Error");
    EXPECT_EQ(errorCodeToString(ErrorCode::UNKNOWN_ERROR), "Unknown Error");
}

// Test CalculatorException construction and accessors
TEST(ErrorTest, CalculatorExceptionConstruction) {
    CalculatorException ex(ErrorCode::DIVISION_BY_ZERO, "Test message", 42);

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
    EXPECT_EQ(ex.getPosition(), 42);
    EXPECT_STREQ(ex.what(), "Test message");
}

// Test CalculatorException default position
TEST(ErrorTest, CalculatorExceptionDefaultPosition) {
    CalculatorException ex(ErrorCode::INVALID_SYNTAX, "Test message");

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::INVALID_SYNTAX);
    EXPECT_EQ(ex.getPosition(), 0);
    EXPECT_STREQ(ex.what(), "Test message");
}

// Test SyntaxError inherits from CalculatorException
TEST(ErrorTest, SyntaxErrorInheritance) {
    SyntaxError ex("Syntax error at position 10", 10);

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::INVALID_SYNTAX);
    EXPECT_EQ(ex.getPosition(), 10);
    EXPECT_STREQ(ex.what(), "Syntax error at position 10");

    // Should be catchable as CalculatorException
    CalculatorException& base = ex;
    EXPECT_EQ(base.getErrorCode(), ErrorCode::INVALID_SYNTAX);
}

// Test EvaluationError
TEST(ErrorTest, EvaluationErrorConstruction) {
    EvaluationError ex(ErrorCode::DOMAIN_ERROR, "Value out of domain", 5);

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::DOMAIN_ERROR);
    EXPECT_EQ(ex.getPosition(), 5);
    EXPECT_STREQ(ex.what(), "Value out of domain");
}

// Test DivisionByZeroError
TEST(ErrorTest, DivisionByZeroErrorConstruction) {
    DivisionByZeroError ex(15);

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
    EXPECT_EQ(ex.getPosition(), 15);
    EXPECT_STREQ(ex.what(), "Division by zero");
}

// Test DivisionByZeroError default position
TEST(ErrorTest, DivisionByZeroErrorDefaultPosition) {
    DivisionByZeroError ex;

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
    EXPECT_EQ(ex.getPosition(), 0);
    EXPECT_STREQ(ex.what(), "Division by zero");
}

// Test DomainError
TEST(ErrorTest, DomainErrorConstruction) {
    DomainError ex("Cannot compute square root of negative number", 20);

    EXPECT_EQ(ex.getErrorCode(), ErrorCode::DOMAIN_ERROR);
    EXPECT_EQ(ex.getPosition(), 20);
    EXPECT_STREQ(ex.what(), "Cannot compute square root of negative number");
}

// Test exceptions are throwable and catchable
TEST(ErrorTest, ExceptionThrowAndCatch) {
    EXPECT_THROW({
        throw CalculatorException(ErrorCode::INVALID_SYNTAX, "Test error");
    }, CalculatorException);

    EXPECT_THROW({
        throw SyntaxError("Syntax error");
    }, SyntaxError);

    EXPECT_THROW({
        throw DivisionByZeroError();
    }, DivisionByZeroError);

    EXPECT_THROW({
        throw DomainError("Domain error");
    }, DomainError);
}

// Test catching derived exception as base type
TEST(ErrorTest, CatchDerivedAsBase) {
    try {
        throw DivisionByZeroError(10);
    } catch (const CalculatorException& ex) {
        EXPECT_EQ(ex.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
        EXPECT_EQ(ex.getPosition(), 10);
    }

    try {
        throw DomainError("Test domain error", 5);
    } catch (const EvaluationError& ex) {
        EXPECT_EQ(ex.getErrorCode(), ErrorCode::DOMAIN_ERROR);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
