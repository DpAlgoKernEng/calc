/**
 * @file output_formatter_test.cpp
 * @brief Unit tests for OutputFormatter
 */

#include "calc/ui/cli/output_formatter.h"
#include "calc/core/evaluator.h"
#include <gtest/gtest.h>
#include <limits>

using namespace calc;
using namespace calc::cli;

class OutputFormatterTest : public ::testing::Test {
protected:
    EvaluationResult createSuccessResult(double value) {
        return EvaluationResult(value);
    }

    EvaluationResult createErrorResult(ErrorCode code, const std::string& message, size_t pos = 0) {
        return EvaluationResult(code, message, pos);
    }
};

// Constructor tests
TEST_F(OutputFormatterTest, Constructor_DefaultValues) {
    OutputFormatter formatter;

    EXPECT_TRUE(formatter.isColorEnabled());
    EXPECT_TRUE(formatter.isExpressionShown());
}

TEST_F(OutputFormatterTest, Constructor_DisableColor) {
    OutputFormatter formatter(false, true);

    EXPECT_FALSE(formatter.isColorEnabled());
    EXPECT_TRUE(formatter.isExpressionShown());
}

TEST_F(OutputFormatterTest, Constructor_DisableExpression) {
    OutputFormatter formatter(true, false);

    EXPECT_TRUE(formatter.isColorEnabled());
    EXPECT_FALSE(formatter.isExpressionShown());
}

// Setter/Getter tests
TEST_F(OutputFormatterTest, SetUseColor_UpdatesState) {
    OutputFormatter formatter;

    formatter.setUseColor(false);
    EXPECT_FALSE(formatter.isColorEnabled());

    formatter.setUseColor(true);
    EXPECT_TRUE(formatter.isColorEnabled());
}

TEST_F(OutputFormatterTest, SetShowExpression_UpdatesState) {
    OutputFormatter formatter;

    formatter.setShowExpression(false);
    EXPECT_FALSE(formatter.isExpressionShown());

    formatter.setShowExpression(true);
    EXPECT_TRUE(formatter.isExpressionShown());
}

// FormatResult tests
TEST_F(OutputFormatterTest, FormatResult_Success_ContainsValue) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(42.0);

    std::string output = formatter.formatResult(result);

    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find("42") != std::string::npos || output.find("4.2") != std::string::npos);
    EXPECT_TRUE(output.find("Result:") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_SuccessWithExpression_ContainsBoth) {
    OutputFormatter formatter(false, true);
    auto result = createSuccessResult(6.0);

    std::string output = formatter.formatResult("2 * 3", result);

    EXPECT_TRUE(output.find("2 * 3") != std::string::npos);
    EXPECT_TRUE(output.find("6") != std::string::npos);
    EXPECT_TRUE(output.find("Expression:") != std::string::npos);
    EXPECT_TRUE(output.find("Result:") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_IntegerValue_NoDecimal) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(5.0);

    std::string output = formatter.formatResult(result);

    // Integer values should not have decimal point
    EXPECT_TRUE(output.find("5") != std::string::npos);
    EXPECT_TRUE(output.find("5.0") == std::string::npos || output.find("5 ") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_DecimalValue_HasPrecision) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(3.14159);

    std::string output = formatter.formatResult(result);

    EXPECT_TRUE(output.find("3.14159") != std::string::npos ||
                output.find("3.14159") != std::string::npos ||
                output.find("3.14159") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_NegativeValue_ContainsMinus) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(-42.0);

    std::string output = formatter.formatResult(result);

    EXPECT_TRUE(output.find("-") != std::string::npos);
    EXPECT_TRUE(output.find("42") != std::string::npos);
}

// FormatError tests
TEST_F(OutputFormatterTest, FormatError_ContainsMessage) {
    OutputFormatter formatter(false, false);
    auto result = createErrorResult(ErrorCode::DIVISION_BY_ZERO, "Cannot divide by zero");

    std::string output = formatter.formatError("1 / 0", result);

    EXPECT_TRUE(output.find("Error:") != std::string::npos);
    EXPECT_TRUE(output.find("Cannot divide by zero") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatError_WithPosition_ContainsPosition) {
    OutputFormatter formatter(false, false);
    auto result = createErrorResult(ErrorCode::INVALID_SYNTAX, "Invalid syntax", 5);

    std::string output = formatter.formatError("1 + + 2", result);

    EXPECT_TRUE(output.find("position") != std::string::npos);
    EXPECT_TRUE(output.find("5") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatError_NoPosition_NoPositionText) {
    OutputFormatter formatter(false, false);
    auto result = createErrorResult(ErrorCode::INVALID_SYNTAX, "Invalid syntax", 0);

    std::string output = formatter.formatError("bad", result);

    EXPECT_FALSE(output.find("position 0") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatError_WithExpression_ContainsExpression) {
    OutputFormatter formatter(false, true);
    auto result = createErrorResult(ErrorCode::INVALID_FUNCTION, "Unknown function", 3);

    std::string output = formatter.formatError("foo(5)", result);

    EXPECT_TRUE(output.find("foo(5)") != std::string::npos);
    EXPECT_TRUE(output.find("Error:") != std::string::npos);
    EXPECT_TRUE(output.find("Unknown function") != std::string::npos);
}

// FormatExpression tests
TEST_F(OutputFormatterTest, FormatExpression_ContainsExpression) {
    OutputFormatter formatter(false);
    std::string output = formatter.formatExpression("2 + 3 * 4");

    EXPECT_TRUE(output.find("2 + 3 * 4") != std::string::npos);
    EXPECT_TRUE(output.find("Expression:") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatExpression_Empty_ReturnsEmptyPrefix) {
    OutputFormatter formatter(false);
    std::string output = formatter.formatExpression("");

    EXPECT_TRUE(output.find("Expression:") != std::string::npos);
}

// FormatSeparator tests
TEST_F(OutputFormatterTest, FormatSeparator_DefaultLength40_Returns40Dashes) {
    OutputFormatter formatter(false);
    std::string sep = formatter.formatSeparator();

    EXPECT_EQ(sep.size(), 40);
    EXPECT_EQ(sep, std::string(40, '-'));
}

TEST_F(OutputFormatterTest, FormatSeparator_CustomLength_ReturnsCustomDashes) {
    OutputFormatter formatter(false);
    std::string sep = formatter.formatSeparator(20);

    EXPECT_EQ(sep.size(), 20);
    EXPECT_EQ(sep, std::string(20, '-'));
}

TEST_F(OutputFormatterTest, FormatSeparator_CustomCharacter_ReturnsCustomChar) {
    OutputFormatter formatter(false);
    std::string sep = formatter.formatSeparator(10, '*');

    EXPECT_EQ(sep.size(), 10);
    EXPECT_EQ(sep, std::string(10, '*'));
}

// Color tests (with colors enabled)
TEST_F(OutputFormatterTest, FormatResult_WithColor_ContainsAnsiCodes) {
    OutputFormatter formatter(true, false);
    auto result = createSuccessResult(42.0);

    std::string output = formatter.formatResult(result);

    // Should contain ANSI color codes when enabled
    EXPECT_TRUE(output.find("\033") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_WithoutColor_NoAnsiCodes) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(42.0);

    std::string output = formatter.formatResult(result);

    // Should NOT contain ANSI color codes when disabled
    EXPECT_TRUE(output.find("\033") == std::string::npos);
}

// Special value tests
TEST_F(OutputFormatterTest, FormatResult_Infinity_HandlesPositiveInfinity) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(std::numeric_limits<double>::infinity());

    std::string output = formatter.formatResult(result);

    EXPECT_TRUE(output.find("Infinity") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_NegativeInfinity_HandlesNegativeInfinity) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(-std::numeric_limits<double>::infinity());

    std::string output = formatter.formatResult(result);

    EXPECT_TRUE(output.find("-Infinity") != std::string::npos);
}

TEST_F(OutputFormatterTest, FormatResult_NaN_HandlesNaN) {
    OutputFormatter formatter(false, false);
    auto result = createSuccessResult(std::numeric_limits<double>::quiet_NaN());

    std::string output = formatter.formatResult(result);

    EXPECT_TRUE(output.find("NaN") != std::string::npos);
}
