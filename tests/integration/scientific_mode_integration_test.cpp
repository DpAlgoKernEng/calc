/**
 * @file scientific_mode_integration_test.cpp
 * @brief Integration tests for ScientificMode
 */

#include <gtest/gtest.h>
#include "calc/modes/scientific_mode.h"
#include "calc/modes/mode_manager.h"
#include "calc/core/evaluator.h"

using namespace calc;

class ScientificModeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        modeManager_ = std::make_unique<ModeManager>();
    }

    std::unique_ptr<ModeManager> modeManager_;
};

// Test mode registration
TEST_F(ScientificModeIntegrationTest, ModeManagerRegistration) {
    EXPECT_TRUE(modeManager_->hasMode("standard"));
    EXPECT_TRUE(modeManager_->hasMode("scientific"));
    EXPECT_EQ(modeManager_->getModeCount(), 2);
}

// Test mode name and description
TEST_F(ScientificModeIntegrationTest, ModeName) {
    Mode* mode = modeManager_->getMode("scientific");
    ASSERT_NE(mode, nullptr);
    EXPECT_EQ(mode->getName(), "scientific");
}

TEST_F(ScientificModeIntegrationTest, ModeDescription) {
    Mode* mode = modeManager_->getMode("scientific");
    ASSERT_NE(mode, nullptr);
    EXPECT_FALSE(mode->getDescription().empty());
}

// Test mathematical constants
TEST_F(ScientificModeIntegrationTest, ConstantsViaStaticMethod) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("PI"), 3.141592653589793);
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("pi"), 3.141592653589793);
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("E"), 2.718281828459045);
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("e"), 2.718281828459045);
}

TEST_F(ScientificModeIntegrationTest, ConstantsViaEvaluation) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("PI + E");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.859874, 1e-5);  // PI + E
}

// Test available functions list
TEST_F(ScientificModeIntegrationTest, GetAvailableFunctions) {
    auto mode = modeManager_->getMode("scientific");
    ASSERT_NE(mode, nullptr);
    auto functions = dynamic_cast<ScientificMode*>(mode)->getAvailableFunctions();
    EXPECT_FALSE(functions.empty());
    EXPECT_GT(functions.size(), 10);
}

// Trigonometric functions tests
TEST_F(ScientificModeIntegrationTest, TrigonometricBasicSin) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sin(PI/6)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.5, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, TrigonometricBasicCos) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("cos(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, TrigonometricBasicTan) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("tan(PI/4)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, InverseTrigonometricAsin) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("asin(0.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.5235987756, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, InverseTrigonometricAcos) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("acos(0.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.047197551, 1e-8);  // PI/3
}

TEST_F(ScientificModeIntegrationTest, InverseTrigonometricAtan) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("atan(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.785398163, 1e-8);  // PI/4
}

// Hyperbolic functions tests
TEST_F(ScientificModeIntegrationTest, HyperbolicSinh) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sinh(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.175201194, 1e-8);  // (e - 1/e)/2
}

TEST_F(ScientificModeIntegrationTest, HyperbolicCosh) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("cosh(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.5430806, 1e-6);  // (e + 1/e)/2
}

TEST_F(ScientificModeIntegrationTest, HyperbolicTanh) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("tanh(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.761594156, 1e-10);
}

// Logarithmic functions tests
TEST_F(ScientificModeIntegrationTest, LogarithmicLog) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("log(E)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, LogarithmicLog10) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("log10(10)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 1.0);
}

// Exponential functions tests
TEST_F(ScientificModeIntegrationTest, ExponentialExp) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("exp(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.718281828, 1e-8);  // e
}

TEST_F(ScientificModeIntegrationTest, ExponentialExp2) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("exp(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

// Root functions tests
TEST_F(ScientificModeIntegrationTest, RootSqrt) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sqrt(4)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(ScientificModeIntegrationTest, RootSqrt2) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sqrt(2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.414213562, 1e-8);  // sqrt(2)
}

TEST_F(ScientificModeIntegrationTest, RootCbrt) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("cbrt(8)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.0, 1e-10);
}

// Power function tests
TEST_F(ScientificModeIntegrationTest, PowerPow) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("pow(2, 3)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 8.0);
}

TEST_F(ScientificModeIntegrationTest, PowerPow2) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("pow(2, 10)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1024.0, 1e-8);
}

// Rounding and absolute functions tests
TEST_F(ScientificModeIntegrationTest, AbsPositive) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("abs(-5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(ScientificModeIntegrationTest, AbsNegative) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("abs(-3.14)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.14, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, FloorPositive) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("floor(3.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(ScientificModeIntegrationTest, FloorNegative) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("floor(-2.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.0);
}

TEST_F(ScientificModeIntegrationTest, CeilPositive) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("ceil(3.2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeIntegrationTest, CeilNegative) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("ceil(-2.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -2.0);
}

TEST_F(ScientificModeIntegrationTest, RoundPositive) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("round(3.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeIntegrationTest, RoundNegative) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("round(3.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeIntegrationTest, TruncPositive) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("trunc(3.9)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(ScientificModeIntegrationTest, TruncNegative) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("trunc(-2.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -2.0);
}

// Other functions tests
TEST_F(ScientificModeIntegrationTest, OtherMax) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("max(5, 10)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 10.0);
}

TEST_F(ScientificModeIntegrationTest, OtherMin) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("min(5, 10)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(ScientificModeIntegrationTest, OtherHypot) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("hypot(3, 4)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, OtherFmod) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("fmod(10, 3)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

// Combined expressions tests
TEST_F(ScientificModeIntegrationTest, CombinedConstantsAndTrigonometric) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sin(PI) + cos(PI)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -1.0, 1e-10);  // 0 + (-1) = -1
}

TEST_F(ScientificModeIntegrationTest, CombinedLogarithmicAndArithmetic) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("log(E) + 1");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.0, 1e-10);  // ln(e) + 1 = 1 + 1 = 2
}

TEST_F(ScientificModeIntegrationTest, CombinedExponentialAndArithmetic) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("exp(2) + 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 10.389056, 1e-5);  // e^2 + 3
}

TEST_F(ScientificModeIntegrationTest, CombinedPowerAndLogarithmic) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("pow(2, log(8))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 4.2264358, 1e-5);  // 2^(ln(8))
}

// Test error handling
TEST_F(ScientificModeIntegrationTest, DomainError) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sqrt(-1)");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(ScientificModeIntegrationTest, DomainErrorZero) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("log(0)");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

// Test nested function calls
TEST_F(ScientificModeIntegrationTest, NestedTrigonometricFunctions) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sin(cos(0))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.8414709848, 1e-10);
}

TEST_F(ScientificModeIntegrationTest, NestedLogarithmicFunctions) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("log(sqrt(16))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.386294361, 1e-8);  // ln(4)
}

TEST_F(ScientificModeIntegrationTest, NestedCombined) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("sin(PI) + log(exp(1))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);  // 0 + ln(e) = 0 + 1 = 1
}

// Test that scientific mode includes all standard functions
TEST_F(ScientificModeIntegrationTest, IncludesStandardArithmetic) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("2 + 3 * 4 - 6 / 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 11.0);
}

TEST_F(ScientificModeIntegrationTest, IncludesStandardPower) {
    auto mode = modeManager_->getMode("scientific");
    auto result = mode->evaluate("2 ^ 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 8.0);
}

// Test that both modes produce consistent results for common expressions
TEST_F(ScientificModeIntegrationTest, ModeConsistencyBasicMath) {
    auto mode = modeManager_->getMode("scientific");
    auto resultSci = mode->evaluate("2 + 3");

    auto stdMode = modeManager_->getMode("standard");
    auto resultStd = stdMode->evaluate("2 + 3");

    ASSERT_TRUE(resultSci.isSuccess());
    ASSERT_TRUE(resultStd.isSuccess());
    EXPECT_NEAR(resultSci.getValue(), resultStd.getValue(), 1e-10);
}

TEST_F(ScientificModeIntegrationTest, ModeConsistencyComplexMath) {
    auto mode = modeManager_->getMode("scientific");
    auto resultSci = mode->evaluate("(2 + 3) * 4 - 6 / 2");

    auto stdMode = modeManager_->getMode("standard");
    auto resultStd = stdMode->evaluate("(2 + 3) * 4 - 6 / 2");

    ASSERT_TRUE(resultSci.isSuccess());
    ASSERT_TRUE(resultStd.isSuccess());
    EXPECT_NEAR(resultSci.getValue(), resultStd.getValue(), 1e-10);
}
