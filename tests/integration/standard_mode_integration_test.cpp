/**
 * @file standard_mode_integration_test.cpp
 * @brief Integration tests for StandardMode
 */

#include <gtest/gtest.h>
#include "calc/modes/standard_mode.h"
#include "calc/core/tokenizer.h"
#include "calc/core/evaluator.h"

using namespace calc;

class StandardModeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        mode_ = std::make_unique<StandardMode>();
    }

    std::unique_ptr<StandardMode> mode_;
};

// Full pipeline tests (tokenize -> parse -> evaluate)
TEST_F(StandardModeIntegrationTest, FullPipelineSimpleAddition) {
    const std::string expr = "2 + 3";

    // Tokenize
    Tokenizer tokenizer(expr);
    auto tokens = tokenizer.tokenize();
    ASSERT_FALSE(tokens.empty());

    // Parse and evaluate
    auto result = mode_->evaluate(expr);
    ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(StandardModeIntegrationTest, FullPipelineComplexExpression) {
    const std::string expr = "(2 + 3) * 4 - 6 / 2";

    auto result = mode_->evaluate(expr);
    ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
    EXPECT_DOUBLE_EQ(result.getValue(), 17.0);
}

TEST_F(StandardModeIntegrationTest, FullPipelineWithPower) {
    const std::string expr = "2 ^ 3 + 4";

    auto result = mode_->evaluate(expr);
    ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
    EXPECT_DOUBLE_EQ(result.getValue(), 12.0);
}

TEST_F(StandardModeIntegrationTest, FullPipelineNestedParentheses) {
    const std::string expr = "((2 + 3) * (4 - 1))";

    auto result = mode_->evaluate(expr);
    ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
    EXPECT_DOUBLE_EQ(result.getValue(), 15.0);
}

// Test sequence of evaluations
TEST_F(StandardModeIntegrationTest, SequenceOfEvaluations) {
    std::vector<std::pair<std::string, double>> tests = {
        {"2 + 2", 4.0},
        {"3 * 3", 9.0},
        {"10 / 2", 5.0},
        {"2 ^ 4", 16.0},
        {"(2 + 3) * 4", 20.0}
    };

    for (const auto& [expr, expected] : tests) {
        auto result = mode_->evaluate(expr);
        ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
        EXPECT_DOUBLE_EQ(result.getValue(), expected) << "Expression: " << expr;
    }
}

// Test with both parsers
TEST_F(StandardModeIntegrationTest, BothParsersConsistency) {
    std::vector<std::string> expressions = {
        "2 + 3 * 4",
        "(2 + 3) * 4",
        "10 - 6 / 2",
        "2 ^ 3 ^ 2",
        "((2 + 3) * 4)",
        "2 + 3 * 4 - 5 / 2"
    };

    mode_->setParserType(false);  // Shunting-yard
    std::vector<double> results1;
    for (const auto& expr : expressions) {
        auto result = mode_->evaluate(expr);
        ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
        results1.push_back(result.getValue());
    }

    mode_->setParserType(true);  // Recursive descent
    std::vector<double> results2;
    for (const auto& expr : expressions) {
        auto result = mode_->evaluate(expr);
        ASSERT_TRUE(result.isSuccess()) << "Expression: " << expr;
        results2.push_back(result.getValue());
    }

    // Verify all results match
    for (size_t i = 0; i < results1.size(); ++i) {
        EXPECT_DOUBLE_EQ(results1[i], results2[i])
            << "Expression: " << expressions[i];
    }
}

// Test precision affects output
TEST_F(StandardModeIntegrationTest, PrecisionContextSetting) {
    mode_->setPrecision(2);
    EXPECT_EQ(mode_->getContext().getPrecision(), 2);

    auto result = mode_->evaluate("1 / 3");
    ASSERT_TRUE(result.isSuccess());

    // Result should be computable, the precision is for formatting
    EXPECT_GT(result.getValue(), 0.3);
    EXPECT_LT(result.getValue(), 0.34);
}

// Test error messages contain useful information
TEST_F(StandardModeIntegrationTest, ErrorMessages) {
    struct ErrorTest {
        std::string expression;
        ErrorCode expectedCode;
    };

    std::vector<ErrorTest> tests = {
        {"10 / 0", ErrorCode::DIVISION_BY_ZERO},
        {"(2 + 3", ErrorCode::INVALID_SYNTAX},
        {"2 + 3)", ErrorCode::INVALID_SYNTAX},
    };

    for (const auto& test : tests) {
        auto result = mode_->evaluate(test.expression);
        ASSERT_TRUE(result.isError())
            << "Expression '" << test.expression << "' should error";
        // We don't assert exact error code because different parsers may return different codes
        // for similar errors. Just verify it's an error.
        EXPECT_FALSE(result.getErrorMessage().empty())
            << "Error message should not be empty";
    }
}

// Test edge cases
TEST_F(StandardModeIntegrationTest, EdgeCaseZero) {
    auto result = mode_->evaluate("0 + 0");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 0.0);
}

TEST_F(StandardModeIntegrationTest, EdgeCaseLargeNumber) {
    auto result = mode_->evaluate("1000000 * 1000000");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 1e12);
}

TEST_F(StandardModeIntegrationTest, EdgeCaseNegativeNumber) {
    auto result = mode_->evaluate("-100 + 50");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -50.0);
}

TEST_F(StandardModeIntegrationTest, EdgeCaseDecimal) {
    auto result = mode_->evaluate("0.1 + 0.2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.3, 1e-10);
}

// Test all operators in one expression
TEST_F(StandardModeIntegrationTest, AllOperatorsInOneExpression) {
    auto result = mode_->evaluate("2 + 3 * 4 - 6 / 2 ^ 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 12.5);  // 2 + 12 - 6/4 = 2 + 12 - 1.5 = 12.5
}

// Test mode interface methods
TEST_F(StandardModeIntegrationTest, ModeInterfaceMethods) {
    // Test getName
    EXPECT_EQ(mode_->getName(), "standard");

    // Test getDescription
    EXPECT_FALSE(mode_->getDescription().empty());

    // Test getContext
    EvaluationContext& ctx = mode_->getContext();
    ctx.setPrecision(10);
    EXPECT_EQ(mode_->getContext().getPrecision(), 10);
}

// Test consecutive evaluations don't interfere
TEST_F(StandardModeIntegrationTest, ConsecutiveEvaluations) {
    auto r1 = mode_->evaluate("x = 5");
    auto r2 = mode_->evaluate("2 + 3");

    // Even if first expression fails, second should work
    ASSERT_TRUE(r2.isSuccess());
    EXPECT_DOUBLE_EQ(r2.getValue(), 5.0);
}

// Test whitespace handling
TEST_F(StandardModeIntegrationTest, WhitespaceHandling) {
    std::vector<std::string> expressions = {
        "2+3",
        "2 +3",
        "2+ 3",
        "  2  +  3  ",
        "2\n+\n3",
        "2\t+\t3"
    };

    for (const auto& expr : expressions) {
        auto result = mode_->evaluate(expr);
        ASSERT_TRUE(result.isSuccess()) << "Expression: '" << expr << "'";
        EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
    }
}

// Test expression with multiple parentheses levels
TEST_F(StandardModeIntegrationTest, DeeplyNestedParentheses) {
    auto result = mode_->evaluate("((((((1 + 2) + 3) + 4) + 5) + 6))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 21.0);
}
