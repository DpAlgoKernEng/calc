/**
 * @file programmer_mode_test.cpp
 * @brief Unit tests for programmer mode
 */

#include <gtest/gtest.h>
#include "calc/modes/programmer_mode.h"

namespace calc {

// ============================================================================
// Test fixture for ProgrammerMode tests
// ============================================================================

class ProgrammerModeTest : public ::testing::Test {
protected:
    void SetUp() override {
        mode = std::make_unique<ProgrammerMode>();
    mode->setPrecision(6);
    mode->setDisplayBase(10);
    }

    void TearDown() override {}

    std::unique_ptr<ProgrammerMode> mode;
};

// ============================================================================
// Mode Properties Tests
// ============================================================================

TEST_F(ProgrammerModeTest, GetName) {
    EXPECT_EQ(mode->getName(), "programmer");
}

TEST_F(ProgrammerModeTest, GetDescription) {
    EXPECT_EQ(mode->getDescription(), "Programmer mode with base conversions and bitwise operations");
}

TEST_F(ProgrammerModeTest, GetContext) {
    auto& context = mode->getContext();
    EXPECT_EQ(context.getPrecision(), 6);
}

TEST_F(ProgrammerModeTest, GetDisplayBase) {
    EXPECT_EQ(mode->getDisplayBase(), 10);
}

TEST_F(ProgrammerModeTest, GetSupportedBases) {
    auto bases = mode->getSupportedBases();
    EXPECT_EQ(bases.size(), 4);
    EXPECT_EQ(bases[0], 2);
    EXPECT_EQ(bases[1], 8);
    EXPECT_EQ(bases[2], 10);
    EXPECT_EQ(bases[3], 16);
}

TEST_F(ProgrammerModeTest, SetDisplayBase_Valid) {
    mode->setDisplayBase(2);
    EXPECT_EQ(mode->getDisplayBase(), 2);

    mode->setDisplayBase(8);
    EXPECT_EQ(mode->getDisplayBase(), 8);

    mode->setDisplayBase(16);
    EXPECT_EQ(mode->getDisplayBase(), 16);
}

TEST_F(ProgrammerModeTest, SetDisplayBase_Invalid) {
    EXPECT_THROW(mode->setDisplayBase(3), std::invalid_argument);
    EXPECT_THROW(mode->setDisplayBase(5), std::invalid_argument);
    EXPECT_THROW(mode->setDisplayBase(7), std::invalid_argument);
}

TEST_F(ProgrammerModeTest, SetPrecision_Valid) {
    mode->setPrecision(4);
    EXPECT_EQ(mode->getPrecision(), 4);
}

TEST_F(ProgrammerModeTest, SetPrecision_Negative) {
    EXPECT_THROW(mode->setPrecision(-1), std::invalid_argument);
}

TEST_F(ProgrammerModeTest, FormatResult_Decimal) {
    mode->setDisplayBase(10);
    EXPECT_EQ(mode->formatResult(42), "42");
    EXPECT_EQ(mode->formatResult(255), "255");
    EXPECT_EQ(mode->formatResult(0), "0");
    EXPECT_EQ(mode->formatResult(-42), "-42");
}

TEST_F(ProgrammerModeTest, FormatResult_Binary) {
    mode->setDisplayBase(2);
    EXPECT_EQ(mode->formatResult(10), "0b1010");
    EXPECT_EQ(mode->formatResult(255), "0b11111111");
}

TEST_F(ProgrammerModeTest, FormatResult_Octal) {
    mode->setDisplayBase(8);
    EXPECT_EQ(mode->formatResult(10), "0o12");
    EXPECT_EQ(mode->formatResult(63), "0o77");
}

TEST_F(ProgrammerModeTest, FormatResult_Hex) {
    mode->setDisplayBase(16);
    EXPECT_EQ(mode->formatResult(10), "0xA");
    EXPECT_EQ(mode->formatResult(255), "0xFF");
    EXPECT_EQ(mode->formatResult(4095), "0xFFF");
}

// ============================================================================
// Expression Evaluation Tests
// ============================================================================

TEST_F(ProgrammerModeTest, SimpleDecimal) {
    auto result = mode->evaluate("42 + 10");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 52);
}

TEST_F(ProgrammerModeTest, SimpleSubtraction) {
    auto result = mode->evaluate("100 - 25");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 75);
}

TEST_F(ProgrammerModeTest, SimpleMultiplication) {
    auto result = mode->evaluate("6 * 7");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 42);
}

TEST_F(ProgrammerModeTest, SimpleDivision) {
    auto result = mode->evaluate("100 / 4");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 25);
}

TEST_F(ProgrammerModeTest, EmptyExpression) {
    auto result = mode->evaluate("");
    EXPECT_FALSE(result.isSuccess());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::INVALID_SYNTAX);
}

// ============================================================================
// Binary Number Tests
// ============================================================================

TEST_F(ProgrammerModeTest, BinaryLiteral) {
    auto result = mode->evaluate("0b1010");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 10);
}

TEST_F(ProgrammerModeTest, BinaryLiteral_Uppercase) {
    auto result = mode->evaluate("0B1010");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 10);
}

TEST_F(ProgrammerModeTest, BinaryWithMath) {
    auto result = mode->evaluate("0b1010 + 0b1010");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 20);
}

TEST_F(ProgrammerModeTest, BinaryParens) {
    auto result = mode->evaluate("(0b1010)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 10);
}

// ============================================================================
// Hex Number Tests
// ============================================================================

TEST_F(ProgrammerModeTest, HexLiteral) {
    auto result = mode->evaluate("0xFF");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

TEST_F(ProgrammerModeTest, HexLiteral_Lowercase) {
    auto result = mode->evaluate("0xff");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

TEST_F(ProgrammerModeTest, HexWithMath) {
    auto result = mode->evaluate("0xFF + 0x10");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 271);
}

TEST_F(ProgrammerModeTest, HexParens) {
    auto result = mode->evaluate("(0xFF)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

// ============================================================================
// Octal Number Tests
// ============================================================================

TEST_F(ProgrammerModeTest, OctalLiteral) {
    auto result = mode->evaluate("0o77");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 63);
}

TEST_F(ProgrammerModeTest, OctalWithMath) {
    auto result = mode->evaluate("0o77 + 0o10");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 71);
}

TEST_F(ProgrammerModeTest, OctalParens) {
    auto result = mode->evaluate("(0o77)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 63);
}

// ============================================================================
// Bitwise AND Tests
// ============================================================================

TEST_F(ProgrammerModeTest, BitwiseAnd_Binary) {
    auto result = mode->evaluate("0b1010 & 0b1100");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 8);
}

TEST_F(ProgrammerModeTest, BitwiseAnd_Hex) {
    auto result = mode->evaluate("0xFF & 0x0F");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 15);
}

TEST_F(ProgrammerModeTest, BitwiseAnd_Mixed) {
    auto result = mode->evaluate("0b1010 & 0xFF");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 10);
}

// ============================================================================
// Bitwise OR Tests
// ============================================================================

TEST_F(ProgrammerModeTest, BitwiseOr_Binary) {
    auto result = mode->evaluate("0b1010 | 0b0100");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 14);
}

TEST_F(ProgrammerModeTest, BitwiseOr_Hex) {
    auto result = mode->evaluate("0xFF | 0x0F");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

TEST_F(ProgrammerModeTest, BitwiseOr_Mixed) {
    auto result = mode->evaluate("0b1010 | 0xFF");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 255);
}

// ============================================================================
// Bitwise XOR Tests
// ============================================================================

TEST_F(ProgrammerModeTest, BitwiseXor_Binary) {
    auto result = mode->evaluate("0b1010 ^ 0b1100");
    EXPECT_TRUE(result.isSuccess());
    // 1010 (10) ^ 1100 (12) = 0110 (6)
    EXPECT_EQ(result.getValue(), 6);
}

TEST_F(ProgrammerModeTest, BitwiseXor_Hex) {
    auto result = mode->evaluate("0xFF ^ 0x0F");
    EXPECT_TRUE(result.isSuccess());
    // FF (255) ^ 0F (15) = F0 (240)
    EXPECT_EQ(result.getValue(), 240);
}

TEST_F(ProgrammerModeTest, BitwiseXor_Mixed) {
    auto result = mode->evaluate("0b1010 ^ 0xFF");
    EXPECT_TRUE(result.isSuccess());
    // 1010 (10) ^ 11111111 (255) = 11110101 (245)
    EXPECT_EQ(result.getValue(), 245);
}

TEST_F(ProgrammerModeTest, BitwiseXor_Self) {
    auto result = mode->evaluate("10 ^ 10");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 0);
}

TEST_F(ProgrammerModeTest, BitwiseXor_WithShifts) {
    auto result = mode->evaluate("(0b1 << 2) ^ 0b1010");
    EXPECT_TRUE(result.isSuccess());
    // (1 << 2) = 0100 (4), 0100 ^ 1010 = 1110 (14)
    EXPECT_EQ(result.getValue(), 14);
}

// ============================================================================
// Bitwise NOT Tests
// ============================================================================

TEST_F(ProgrammerModeTest, BitwiseNot_Binary) {
    auto result = mode->evaluate("~0b1010");
    // ~1010 = 0101 = -11
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), -11);
}

TEST_F(ProgrammerModeTest, BitwiseNot_Hex) {
    auto result = mode->evaluate("~0x0F");
    // ~0x0F = ~15 = -16
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), -16);
}

TEST_F(ProgrammerModeTest, BitwiseNot_Decimal) {
    auto result = mode->evaluate("~10");
    // ~10 = -11
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), -11);
}

TEST_F(ProgrammerModeTest, BitwiseNot_Zero) {
    auto result = mode->evaluate("~0");
    // ~0 = -1
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), -1);
}

TEST_F(ProgrammerModeTest, BitwiseNot_Negative) {
    auto result = mode->evaluate("~(-5)");
    // ~(-5) = ~4294967291 = 4 (in two's complement)
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 4);
}

// ============================================================================
// Left Shift Tests
// ============================================================================

TEST_F(ProgrammerModeTest, LeftShift_Binary) {
    auto result = mode->evaluate("0b1 << 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 8);
}

TEST_F(ProgrammerModeTest, LeftShift_Decimal) {
    auto result = mode->evaluate("5 << 2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 20);
}

TEST_F(ProgrammerModeTest, LeftShift_Hex) {
    auto result = mode->evaluate("0x1 << 4");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 16);
}

// ============================================================================
// Right Shift Tests
// ============================================================================

TEST_F(ProgrammerModeTest, RightShift_Binary) {
    auto result = mode->evaluate("0b1000 >> 2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 2);
}

TEST_F(ProgrammerModeTest, RightShift_Decimal) {
    auto result = mode->evaluate("64 >> 2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 16);
}

TEST_F(ProgrammerModeTest, RightShift_Hex) {
    auto result = mode->evaluate("0x10 >> 2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 4);
}

// ============================================================================
// Complex Expression Tests
// ============================================================================

TEST_F(ProgrammerModeTest, MixedOperations) {
    auto result = mode->evaluate("(0b1010 & 0b1100) | 0b0011");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 11);
}

TEST_F(ProgrammerModeTest, ShiftAndBitwise) {
    auto result = mode->evaluate("(0b1 << 2) & 0b0011");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 0);
}

TEST_F(ProgrammerModeTest, ComplexHexExpression) {
    auto result = mode->evaluate("(0xFF & 0x0F) | 0xAA");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 175);
}

TEST_F(ProgrammerModeTest, ArithmeticWithBitwise) {
    auto result = mode->evaluate("10 + 5 & 0b11");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 11);
}

// ============================================================================
// Parentheses Tests
// ============================================================================

TEST_F(ProgrammerModeTest, ParenthesizedBinary) {
    auto result = mode->evaluate("(0b1010 | 0b0100)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_EQ(result.getValue(), 14);
}

} // namespace calc
