/**
 * @file programmer_mode_integration_test.cpp
 * @brief Integration tests for ProgrammerMode
 */

#include <gtest/gtest.h>
#include "calc/modes/programmer_mode.h"
#include "calc/modes/mode_manager.h"

using namespace calc;

class ProgrammerModeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        modeManager_ = std::make_unique<ModeManager>();
    }

    std::unique_ptr<ModeManager> modeManager_;
};

// ============================================================================
// Mode Registration Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, ModeManagerRegistration) {
    EXPECT_TRUE(modeManager_->hasMode("standard"));
    EXPECT_TRUE(modeManager_->hasMode("scientific"));
    EXPECT_TRUE(modeManager_->hasMode("programmer"));
    EXPECT_EQ(modeManager_->getModeCount(), 3);
}

TEST_F(ProgrammerModeIntegrationTest, ProgrammerModeAvailable) {
    Mode* mode = modeManager_->getMode("programmer");
    ASSERT_NE(mode, nullptr);
    EXPECT_EQ(mode->getName(), "programmer");
}

// ============================================================================
// Base Conversion Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, BinaryLiteralEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("0b1010");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 10);
}

TEST_F(ProgrammerModeIntegrationTest, HexLiteralEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("0xFF");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

TEST_F(ProgrammerModeIntegrationTest, OctalLiteralEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("0o77");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 63);
}

// ============================================================================
// Bitwise Operations Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, BitwiseAndEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("10 & 6");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 2);
}

TEST_F(ProgrammerModeIntegrationTest, BitwiseOrEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("10 | 6");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 14);
}

TEST_F(ProgrammerModeIntegrationTest, BitwiseXorEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("10 ^ 6");
    ASSERT_TRUE(result.isSuccess());
    // 1010 (10) ^ 0110 (6) = 1100 (12)
    EXPECT_EQ(result.getValue(), 12);
}

TEST_F(ProgrammerModeIntegrationTest, LeftShiftEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("10 << 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 40);
}

TEST_F(ProgrammerModeIntegrationTest, RightShiftEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("100 >> 2");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 25);
}

// ============================================================================
// Complex Expression Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, CombinedBitwiseOperations) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("(10 & 6) | 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 3);  // (10 & 6) = 2, then 2 | 3 = 3
}

TEST_F(ProgrammerModeIntegrationTest, MixedBaseOperations) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("0b1010 + 0xFF");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 265);
}

TEST_F(ProgrammerModeIntegrationTest, ShiftAndBitwise) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("(10 << 2) & 3");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 0);
}

// ============================================================================
// Display Base Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, DisplayBaseSwitching) {
    auto mode = modeManager_->getMode("programmer");
    auto* programmerMode = dynamic_cast<ProgrammerMode*>(mode);
    ASSERT_NE(programmerMode, nullptr);

    // Test in decimal
    programmerMode->setDisplayBase(10);
    EXPECT_EQ(programmerMode->getDisplayBase(), 10);

    auto resultDec = programmerMode->evaluate("10 + 5");
    ASSERT_TRUE(resultDec.isSuccess());
    EXPECT_EQ(programmerMode->formatResult(static_cast<long long>(resultDec.getValue())), "15");

    // Test in binary
    programmerMode->setDisplayBase(2);
    EXPECT_EQ(programmerMode->getDisplayBase(), 2);

    auto resultBin = programmerMode->evaluate("10 + 5");
    ASSERT_TRUE(resultBin.isSuccess());
    EXPECT_EQ(programmerMode->formatResult(static_cast<long long>(resultBin.getValue())), "0b1111");

    // Test in hexadecimal
    programmerMode->setDisplayBase(16);
    EXPECT_EQ(programmerMode->getDisplayBase(), 16);

    auto resultHex = programmerMode->evaluate("10 + 5");
    ASSERT_TRUE(resultHex.isSuccess());
    EXPECT_EQ(programmerMode->formatResult(static_cast<long long>(resultHex.getValue())), "0xF");

    // Test in octal
    programmerMode->setDisplayBase(8);
    EXPECT_EQ(programmerMode->getDisplayBase(), 8);

    auto resultOct = programmerMode->evaluate("10 + 5");
    ASSERT_TRUE(resultOct.isSuccess());
    EXPECT_EQ(programmerMode->formatResult(static_cast<long long>(resultOct.getValue())), "0o17");
}

TEST_F(ProgrammerModeIntegrationTest, SupportedBases) {
    auto mode = modeManager_->getMode("programmer");
    auto* programmerMode = dynamic_cast<ProgrammerMode*>(mode);
    ASSERT_NE(programmerMode, nullptr);

    auto bases = programmerMode->getSupportedBases();
    EXPECT_EQ(bases.size(), 4);
    EXPECT_EQ(bases[0], 2);
    EXPECT_EQ(bases[1], 8);
    EXPECT_EQ(bases[2], 10);
    EXPECT_EQ(bases[3], 16);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(ProgrammerModeIntegrationTest, InvalidDisplayBase) {
    auto mode = modeManager_->getMode("programmer");
    auto* programmerMode = dynamic_cast<ProgrammerMode*>(mode);
    ASSERT_NE(programmerMode, nullptr);

    EXPECT_THROW(programmerMode->setDisplayBase(3), std::invalid_argument);
    EXPECT_THROW(programmerMode->setDisplayBase(5), std::invalid_argument);
    EXPECT_THROW(programmerMode->setDisplayBase(7), std::invalid_argument);
    EXPECT_THROW(programmerMode->setDisplayBase(17), std::invalid_argument);
}

TEST_F(ProgrammerModeIntegrationTest, EmptyExpression) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("");
    ASSERT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::INVALID_SYNTAX);
}

TEST_F(ProgrammerModeIntegrationTest, ComplexBitwiseExpression) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("(0xFF & 0x0F) | 0xAA");
    ASSERT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 175);
}

TEST_F(ProgrammerModeIntegrationTest, BitwiseNotEvaluation) {
    auto mode = modeManager_->getMode("programmer");
    auto result = mode->evaluate("~5");
    ASSERT_TRUE(result.isSuccess());
    // ~5 = -6 (bitwise NOT)
    EXPECT_EQ(result.getValue(), -6);
}

TEST_F(ProgrammerModeIntegrationTest, XorVsPower_Semantics) {
    // Verify that '^' is bitwise XOR in Programmer mode
    auto progMode = modeManager_->getMode("programmer");
    auto progResult = progMode->evaluate("2 ^ 3");
    ASSERT_TRUE(progResult.isSuccess());
    // In Programmer mode: 2 ^ 3 = 1 (bitwise XOR)
    EXPECT_EQ(progResult.getValue(), 1);

    // Verify that '^' is power operation in Standard mode
    auto stdMode = modeManager_->getMode("standard");
    auto stdResult = stdMode->evaluate("2 ^ 3");
    ASSERT_TRUE(stdResult.isSuccess());
    // In Standard mode: 2 ^ 3 = 8 (power)
    EXPECT_NEAR(stdResult.getValue(), 8.0, 1e-10);
}
