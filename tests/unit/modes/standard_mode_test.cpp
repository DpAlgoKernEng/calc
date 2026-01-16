/**
 * @file standard_mode_test.cpp
 * @brief Unit tests for StandardMode
 */

#include <gtest/gtest.h>
#include "calc/modes/standard_mode.h"
#include "calc/modes/mode_manager.h"

using namespace calc;

class StandardModeTest : public ::testing::Test {
protected:
    void SetUp() override {
        mode_ = std::make_unique<StandardMode>();
    }

    std::unique_ptr<StandardMode> mode_;
};

// Test mode name and description
TEST_F(StandardModeTest, ModeName) {
    EXPECT_EQ(mode_->getName(), "standard");
}

TEST_F(StandardModeTest, ModeDescription) {
    EXPECT_EQ(mode_->getDescription(), "Standard calculator mode: +, -, *, /, ^ with parentheses");
}

// Test basic arithmetic operations
TEST_F(StandardModeTest, BasicAddition) {
    auto result = mode_->evaluate("2 + 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(StandardModeTest, BasicSubtraction) {
    auto result = mode_->evaluate("5 - 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(StandardModeTest, BasicMultiplication) {
    auto result = mode_->evaluate("4 * 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 12.0);
}

TEST_F(StandardModeTest, BasicDivision) {
    auto result = mode_->evaluate("12 / 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(StandardModeTest, BasicPower) {
    auto result = mode_->evaluate("2 ^ 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 8.0);
}

// Test operator precedence
TEST_F(StandardModeTest, OperatorPrecedenceMultiplyAdd) {
    auto result = mode_->evaluate("2 + 3 * 4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 14.0);
}

TEST_F(StandardModeTest, OperatorPrecedenceDivideSubtract) {
    auto result = mode_->evaluate("10 - 6 / 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 7.0);
}

TEST_F(StandardModeTest, OperatorPrecedencePowerMultiply) {
    auto result = mode_->evaluate("2 * 3 ^ 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 18.0);
}

TEST_F(StandardModeTest, ComplexPrecedence) {
    auto result = mode_->evaluate("2 + 3 * 4 - 5");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 9.0);
}

// Test parentheses
TEST_F(StandardModeTest, ParenthesesOverride) {
    auto result = mode_->evaluate("(2 + 3) * 4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(StandardModeTest, NestedParentheses) {
    auto result = mode_->evaluate("((2 + 3) * 4)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(StandardModeTest, ComplexParentheses) {
    auto result = mode_->evaluate("(2 + (3 * (4 - 1)))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 11.0);
}

// Test negative numbers
TEST_F(StandardModeTest, NegativeNumberLiteral) {
    auto result = mode_->evaluate("-5");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -5.0);
}

TEST_F(StandardModeTest, UnaryMinus) {
    auto result = mode_->evaluate("3 + -4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -1.0);
}

TEST_F(StandardModeTest, UnaryMinusBeforeNumber) {
    auto result = mode_->evaluate("-3 + 4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 1.0);
}

TEST_F(StandardModeTest, NegativeInParentheses) {
    auto result = mode_->evaluate("-(1 + 2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.0);
}

// Test right-associative power operator
TEST_F(StandardModeTest, PowerRightAssociative) {
    auto result = mode_->evaluate("2 ^ 3 ^ 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 512.0);  // 2^(3^2) = 2^9 = 512
}

TEST_F(StandardModeTest, ComplexPower) {
    auto result = mode_->evaluate("(2 ^ 3) ^ 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 64.0);  // (2^3)^2 = 8^2 = 64
}

// Test decimal numbers
TEST_F(StandardModeTest, DecimalAddition) {
    auto result = mode_->evaluate("2.5 + 3.5");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 6.0);
}

TEST_F(StandardModeTest, DecimalMultiplication) {
    auto result = mode_->evaluate("0.1 * 10");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

// Test scientific notation
TEST_F(StandardModeTest, ScientificNotation) {
    auto result = mode_->evaluate("1.5e2 + 2.5e2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 400.0);
}

// Test complex expressions
TEST_F(StandardModeTest, ComplexExpression1) {
    auto result = mode_->evaluate("2 + 3 * 4 - 6 / 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 11.0);
}

TEST_F(StandardModeTest, ComplexExpression2) {
    auto result = mode_->evaluate("(2 + 3) * (4 - 1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 15.0);
}

TEST_F(StandardModeTest, ComplexExpression3) {
    auto result = mode_->evaluate("2 ^ (3 + 1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 16.0);
}

// Test error handling
TEST_F(StandardModeTest, DivisionByZero) {
    auto result = mode_->evaluate("10 / 0");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(StandardModeTest, UnbalancedParenthesesOpen) {
    auto result = mode_->evaluate("(2 + 3");
    ASSERT_TRUE(result.isError());
}

TEST_F(StandardModeTest, UnbalancedParenthesesClose) {
    auto result = mode_->evaluate("2 + 3)");
    ASSERT_TRUE(result.isError());
}

TEST_F(StandardModeTest, EmptyExpression) {
    auto result = mode_->evaluate("");
    ASSERT_TRUE(result.isError());
}

TEST_F(StandardModeTest, InvalidOperator) {
    // Note: & is now a valid bitwise operator
    auto result = mode_->evaluate("2 & 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 2);  // 2 & 3 = 2
}

// Test precision setting
TEST_F(StandardModeTest, SetPrecision) {
    mode_->setPrecision(2);
    EXPECT_EQ(mode_->getPrecision(), 2);
    EXPECT_EQ(mode_->getContext().getPrecision(), 2);

    mode_->setPrecision(10);
    EXPECT_EQ(mode_->getPrecision(), 10);
    EXPECT_EQ(mode_->getContext().getPrecision(), 10);
}

// Test parser type
TEST_F(StandardModeTest, DefaultParserType) {
    EXPECT_EQ(mode_->getParserType(), "shunting-yard");
}

TEST_F(StandardModeTest, SetParserTypeToRecursiveDescent) {
    mode_->setParserType(true);
    EXPECT_EQ(mode_->getParserType(), "recursive-descent");
}

TEST_F(StandardModeTest, SetParserTypeToShuntingYard) {
    mode_->setParserType(true);
    mode_->setParserType(false);
    EXPECT_EQ(mode_->getParserType(), "shunting-yard");
}

TEST_F(StandardModeTest, BothParsersGiveSameResult) {
    mode_->setParserType(false);
    auto result1 = mode_->evaluate("2 + 3 * 4 - 6 / 2");

    mode_->setParserType(true);
    auto result2 = mode_->evaluate("2 + 3 * 4 - 6 / 2");

    ASSERT_TRUE(result1.isSuccess());
    ASSERT_TRUE(result2.isSuccess());
    EXPECT_DOUBLE_EQ(result1.getValue(), result2.getValue());
}

// Test mode manager
TEST_F(StandardModeTest, ModeManagerRegistration) {
    ModeManager manager;
    EXPECT_TRUE(manager.hasMode("standard"));
    EXPECT_EQ(manager.getModeCount(), 3);  // standard + scientific + programmer
}

TEST_F(StandardModeTest, ModeManagerGetDefault) {
    ModeManager manager;
    Mode* mode = manager.getDefaultMode();
    ASSERT_NE(mode, nullptr);
    EXPECT_EQ(mode->getName(), "standard");
}

TEST_F(StandardModeTest, ModeManagerEvaluate) {
    ModeManager manager;
    Mode* mode = manager.getMode("standard");
    ASSERT_NE(mode, nullptr);

    auto result = mode->evaluate("2 + 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

// Test context access
TEST_F(StandardModeTest, GetContext) {
    EvaluationContext& context = mode_->getContext();
    context.setPrecision(8);
    EXPECT_EQ(mode_->getContext().getPrecision(), 8);
}

TEST_F(StandardModeTest, GetConstContext) {
    const StandardMode& constMode = *mode_;
    const EvaluationContext& context = constMode.getContext();
    EXPECT_EQ(context.getPrecision(), 6);  // Default precision
}
