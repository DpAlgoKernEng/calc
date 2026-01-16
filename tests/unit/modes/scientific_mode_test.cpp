/**
 * @file scientific_mode_test.cpp
 * @brief Unit tests for ScientificMode
 */

#include <gtest/gtest.h>
#include "calc/modes/scientific_mode.h"
#include "calc/core/evaluator.h"
#include "calc/modes/mode_manager.h"

using namespace calc;

class ScientificModeTest : public ::testing::Test {
protected:
    void SetUp() override {
        mode_ = std::make_unique<ScientificMode>();
        modeManager_ = std::make_unique<ModeManager>();
    }

    std::unique_ptr<ScientificMode> mode_;
    std::unique_ptr<ModeManager> modeManager_;
};

// Test mode name and description
TEST_F(ScientificModeTest, ModeName) {
    EXPECT_EQ(mode_->getName(), "scientific");
}

TEST_F(ScientificModeTest, ModeDescription) {
    EXPECT_FALSE(mode_->getDescription().empty());
}

// Test mathematical constants
TEST_F(ScientificModeTest, ConstantPI) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("PI"), 3.141592653589793);
}

TEST_F(ScientificModeTest, ConstantPi) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("pi"), 3.141592653589793);
}

TEST_F(ScientificModeTest, ConstantE) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("E"), 2.718281828459045);
}

TEST_F(ScientificModeTest, Constante) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("e"), 2.718281828459045);
}

TEST_F(ScientificModeTest, ConstantUnknown) {
    EXPECT_DOUBLE_EQ(ScientificMode::getConstant("unknown"), 0.0);
}

// Test available functions
TEST_F(ScientificModeTest, GetAvailableFunctions) {
    auto functions = mode_->getAvailableFunctions();
    EXPECT_FALSE(functions.empty());

    // Check for specific functions
    EXPECT_TRUE(std::find(functions.begin(), functions.end(), "sin") != functions.end());
    EXPECT_TRUE(std::find(functions.begin(), functions.end(), "cos") != functions.end());
    EXPECT_TRUE(std::find(functions.begin(), functions.end(), "tan") != functions.end());
    EXPECT_TRUE(std::find(functions.begin(), functions.end(), "log") != functions.end());
    EXPECT_TRUE(std::find(functions.begin(), functions.end(), "sqrt") != functions.end());
}

// Test trigonometric functions
TEST_F(ScientificModeTest, TrigonometricSin) {
    auto result = mode_->evaluate("sin(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(ScientificModeTest, TrigonometricSinPiOver2) {
    auto result = mode_->evaluate("sin(PI/2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, TrigonometricCos) {
    auto result = mode_->evaluate("cos(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, TrigonometricTan) {
    auto result = mode_->evaluate("tan(PI/4)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, TrigonometricAsin) {
    auto result = mode_->evaluate("asin(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(ScientificModeTest, TrigonometricAcos) {
    auto result = mode_->evaluate("acos(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.570796327, 1e-8);  // PI/2
}

TEST_F(ScientificModeTest, TrigonometricAtan) {
    auto result = mode_->evaluate("atan(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.785398163, 1e-8);  // PI/4
}

// Test logarithmic and exponential functions
TEST_F(ScientificModeTest, LogarithmicLog) {
    auto result = mode_->evaluate("log(E)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, LogarithmicLog10) {
    auto result = mode_->evaluate("log10(100)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.0, 1e-10);
}

TEST_F(ScientificModeTest, ExponentialExp) {
    auto result = mode_->evaluate("exp(1)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.718281828, 1e-8);  // e
}

TEST_F(ScientificModeTest, ExponentialExp2) {
    auto result = mode_->evaluate("exp(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, SquareRootSqrt) {
    auto result = mode_->evaluate("sqrt(16)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeTest, SquareRootSqrt2) {
    auto result = mode_->evaluate("sqrt(25)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(ScientificModeTest, CubeRootCbrt) {
    auto result = mode_->evaluate("cbrt(27)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);
}

TEST_F(ScientificModeTest, PowerPow) {
    auto result = mode_->evaluate("pow(2, 3)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 8.0);
}

TEST_F(ScientificModeTest, PowerPowZero) {
    auto result = mode_->evaluate("pow(5, 0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

// Test rounding and absolute functions
TEST_F(ScientificModeTest, AbsPositive) {
    auto result = mode_->evaluate("abs(5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(ScientificModeTest, AbsNegative) {
    auto result = mode_->evaluate("abs(-5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(ScientificModeTest, FloorPositive) {
    auto result = mode_->evaluate("floor(3.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(ScientificModeTest, FloorNegative) {
    auto result = mode_->evaluate("floor(-3.7)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -4.0);
}

TEST_F(ScientificModeTest, CeilPositive) {
    auto result = mode_->evaluate("ceil(3.2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeTest, CeilNegative) {
    auto result = mode_->evaluate("ceil(-3.2)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.0);
}

TEST_F(ScientificModeTest, RoundPositive) {
    auto result = mode_->evaluate("round(3.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(ScientificModeTest, RoundNegative) {
    auto result = mode_->evaluate("round(-3.5)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -4.0);
}

TEST_F(ScientificModeTest, TruncPositive) {
    auto result = mode_->evaluate("trunc(3.9)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(ScientificModeTest, TruncNegative) {
    auto result = mode_->evaluate("trunc(-3.9)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.0);
}

// Test hyperbolic functions
TEST_F(ScientificModeTest, HyperbolicSinh) {
    auto result = mode_->evaluate("sinh(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(ScientificModeTest, HyperbolicCosh) {
    auto result = mode_->evaluate("cosh(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(ScientificModeTest, HyperbolicTanh) {
    auto result = mode_->evaluate("tanh(0)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

// Test combined expressions
TEST_F(ScientificModeTest, CombinedTrigonometricArithmetic) {
    auto result = mode_->evaluate("2 * sin(PI/4) + 1");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.414213562, 1e-8);  // 2*sqrt(2)/2 + 1 = sqrt(2) + 1
}

TEST_F(ScientificModeTest, CombinedLogarithmicArithmetic) {
    auto result = mode_->evaluate("log(100) + log(10)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 6.907755279, 1e-8);  // ln(100) + ln(10) = ln(1000)
}

TEST_F(ScientificModeTest, CombinedWithConstants) {
    auto result = mode_->evaluate("sin(PI/2) + sqrt(E)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.648721271, 1e-8);  // 1 + sqrt(e)
}

// Test mode manager integration
TEST_F(ScientificModeTest, ModeManagerContainsScientific) {
    EXPECT_TRUE(modeManager_->hasMode("scientific"));
    EXPECT_TRUE(modeManager_->hasMode("standard"));
}

TEST_F(ScientificModeTest, ModeManagerGetScientific) {
    auto mode = modeManager_->getMode("scientific");
    ASSERT_NE(mode, nullptr);
    EXPECT_EQ(mode->getName(), "scientific");
}

// Test error handling
TEST_F(ScientificModeTest, DomainErrorNegativeSqrt) {
    auto result = mode_->evaluate("sqrt(-1)");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(ScientificModeTest, DomainErrorLogOfZero) {
    auto result = mode_->evaluate("log(0)");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

// Test that all standard functions still work in scientific mode
TEST_F(ScientificModeTest, StandardArithmeticStillWorks) {
    auto result = mode_->evaluate("2 + 3 * 4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 14.0);
}

TEST_F(ScientificModeTest, StandardPowerStillWorks) {
    auto result = mode_->evaluate("2 ^ 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 8.0);
}

TEST_F(ScientificModeTest, StandardParenthesesStillWork) {
    auto result = mode_->evaluate("(2 + 3) * 4");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

// Test nested function calls
TEST_F(ScientificModeTest, NestedTrigonometricFunctions) {
    auto result = mode_->evaluate("sin(cos(0))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.841470985, 1e-8);  // sin(1)
}

TEST_F(ScientificModeTest, NestedLogarithmicFunctions) {
    auto result = mode_->evaluate("log(sqrt(16))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.386294361, 1e-8);  // ln(4)
}

TEST_F(ScientificModeTest, DoubleNestedFunctions) {
    auto result = mode_->evaluate("sin(cos(log(4)))");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.182429620, 1e-8);  // sin(cos(ln(4)))
}

TEST_F(ScientificModeTest, NestedWithArithmetic) {
    auto result = mode_->evaluate("2 + 3 * sin(PI/6)");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.5, 1e-10);
}

// Test precision setting
TEST_F(ScientificModeTest, SetPrecision) {
    mode_->setPrecision(10);
    EXPECT_EQ(mode_->getPrecision(), 10);
    EXPECT_EQ(mode_->getContext().getPrecision(), 10);
}

// Test that scientific mode extends standard mode properly
TEST_F(ScientificModeTest, InheritsStandardMode) {
    // Verify that scientific mode has all standard mode capabilities
    auto result = mode_->evaluate("(2 + 3) * 4 - 1");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 19.0, 1e-10);  // (2+3)*4 - 1 = 20 - 1 = 19
}
