/**
 * @file converter_test.cpp
 * @brief Unit tests for base conversion utilities
 */

#include "calc/math/converter.h"
#include "calc/core/token.h"
#include <gtest/gtest.h>
#include <stdexcept>

namespace calc {

// ============================================================================
// Test fixture for Converter tests
// ============================================================================

class ConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// Decimal to Binary Tests
// ============================================================================

TEST_F(ConverterTest, DecimalToBinary_Zero) {
    EXPECT_EQ(Converter::decimalToBinary(0), "0");
}

TEST_F(ConverterTest, DecimalToBinary_Positive) {
    EXPECT_EQ(Converter::decimalToBinary(1), "1");
    EXPECT_EQ(Converter::decimalToBinary(2), "10");
    EXPECT_EQ(Converter::decimalToBinary(10), "1010");
    EXPECT_EQ(Converter::decimalToBinary(42), "101010");
    EXPECT_EQ(Converter::decimalToBinary(255), "11111111");
}

TEST_F(ConverterTest, DecimalToBinary_Negative) {
    EXPECT_EQ(Converter::decimalToBinary(-1), "-1");
    EXPECT_EQ(Converter::decimalToBinary(-10), "-1010");
    EXPECT_EQ(Converter::decimalToBinary(-42), "-101010");
}

TEST_F(ConverterTest, DecimalToBinary_PowersOfTwo) {
    EXPECT_EQ(Converter::decimalToBinary(1), "1");
    EXPECT_EQ(Converter::decimalToBinary(2), "10");
    EXPECT_EQ(Converter::decimalToBinary(4), "100");
    EXPECT_EQ(Converter::decimalToBinary(8), "1000");
    EXPECT_EQ(Converter::decimalToBinary(16), "10000");
    EXPECT_EQ(Converter::decimalToBinary(32), "100000");
    EXPECT_EQ(Converter::decimalToBinary(64), "1000000");
    EXPECT_EQ(Converter::decimalToBinary(128), "10000000");
    EXPECT_EQ(Converter::decimalToBinary(256), "100000000");
}

// ============================================================================
// Decimal to Hex Tests
// ============================================================================

TEST_F(ConverterTest, DecimalToHex_Zero) {
    EXPECT_EQ(Converter::decimalToHex(0), "0");
}

TEST_F(ConverterTest, DecimalToHex_Positive) {
    EXPECT_EQ(Converter::decimalToHex(10), "A");
    EXPECT_EQ(Converter::decimalToHex(15), "F");
    EXPECT_EQ(Converter::decimalToHex(16), "10");
    EXPECT_EQ(Converter::decimalToHex(255), "FF");
    EXPECT_EQ(Converter::decimalToHex(256), "100");
    EXPECT_EQ(Converter::decimalToHex(4095), "FFF");
    EXPECT_EQ(Converter::decimalToHex(65535), "FFFF");
}

TEST_F(ConverterTest, DecimalToHex_Negative) {
    EXPECT_EQ(Converter::decimalToHex(-1), "-1");
    EXPECT_EQ(Converter::decimalToHex(-255), "-FF");
}

TEST_F(ConverterTest, DecimalToHex_Uppercase) {
    // Hex should always be uppercase
    EXPECT_EQ(Converter::decimalToHex(26), "1A");
    EXPECT_EQ(Converter::decimalToHex(175), "AF");
    EXPECT_EQ(Converter::decimalToHex(255), "FF");
}

// ============================================================================
// Decimal to Octal Tests
// ============================================================================

TEST_F(ConverterTest, DecimalToOctal_Zero) {
    EXPECT_EQ(Converter::decimalToOctal(0), "0");
}

TEST_F(ConverterTest, DecimalToOctal_Positive) {
    EXPECT_EQ(Converter::decimalToOctal(8), "10");
    EXPECT_EQ(Converter::decimalToOctal(63), "77");
    EXPECT_EQ(Converter::decimalToOctal(64), "100");
    EXPECT_EQ(Converter::decimalToOctal(511), "777");
}

TEST_F(ConverterTest, DecimalToOctal_Negative) {
    EXPECT_EQ(Converter::decimalToOctal(-8), "-10");
    EXPECT_EQ(Converter::decimalToOctal(-63), "-77");
}

// ============================================================================
// Binary to Decimal Tests
// ============================================================================

TEST_F(ConverterTest, BinaryToDecimal_Zero) {
    EXPECT_EQ(Converter::binaryToDecimal("0"), 0);
}

TEST_F(ConverterTest, BinaryToDecimal_Positive) {
    EXPECT_EQ(Converter::binaryToDecimal("1"), 1);
    EXPECT_EQ(Converter::binaryToDecimal("10"), 2);
    EXPECT_EQ(Converter::binaryToDecimal("1010"), 10);
    EXPECT_EQ(Converter::binaryToDecimal("11111111"), 255);
}

TEST_F(ConverterTest, BinaryToDecimal_Negative) {
    EXPECT_EQ(Converter::binaryToDecimal("-1010"), -10);
}

TEST_F(ConverterTest, BinaryToDecimal_Invalid_Throws) {
    EXPECT_THROW(Converter::binaryToDecimal("102"), std::invalid_argument);
    EXPECT_THROW(Converter::binaryToDecimal("2"), std::invalid_argument);
    EXPECT_THROW(Converter::binaryToDecimal(""), std::invalid_argument);
}

// ============================================================================
// Hex to Decimal Tests
// ============================================================================

TEST_F(ConverterTest, HexToDecimal_Zero) {
    EXPECT_EQ(Converter::hexToDecimal("0"), 0);
}

TEST_F(ConverterTest, HexToDecimal_Uppercase) {
    EXPECT_EQ(Converter::hexToDecimal("A"), 10);
    EXPECT_EQ(Converter::hexToDecimal("F"), 15);
    EXPECT_EQ(Converter::hexToDecimal("FF"), 255);
    EXPECT_EQ(Converter::hexToDecimal("1A"), 26);
}

TEST_F(ConverterTest, HexToDecimal_Lowercase) {
    EXPECT_EQ(Converter::hexToDecimal("a"), 10);
    EXPECT_EQ(Converter::hexToDecimal("f"), 15);
    EXPECT_EQ(Converter::hexToDecimal("ff"), 255);
    EXPECT_EQ(Converter::hexToDecimal("1a"), 26);
}

TEST_F(ConverterTest, HexToDecimal_MixedCase) {
    EXPECT_EQ(Converter::hexToDecimal("1aBc"), 6844);
}

TEST_F(ConverterTest, HexToDecimal_Negative) {
    EXPECT_EQ(Converter::hexToDecimal("-FF"), -255);
}

TEST_F(ConverterTest, HexToDecimal_Invalid_Throws) {
    EXPECT_THROW(Converter::hexToDecimal("G"), std::invalid_argument);
    EXPECT_THROW(Converter::hexToDecimal("X"), std::invalid_argument);
    EXPECT_THROW(Converter::hexToDecimal(""), std::invalid_argument);
}

// ============================================================================
// Octal to Decimal Tests
// ============================================================================

TEST_F(ConverterTest, OctalToDecimal_Zero) {
    EXPECT_EQ(Converter::octalToDecimal("0"), 0);
}

TEST_F(ConverterTest, OctalToDecimal_Positive) {
    EXPECT_EQ(Converter::octalToDecimal("10"), 8);
    EXPECT_EQ(Converter::octalToDecimal("77"), 63);
    EXPECT_EQ(Converter::octalToDecimal("100"), 64);
}

TEST_F(ConverterTest, OctalToDecimal_Negative) {
    EXPECT_EQ(Converter::octalToDecimal("-77"), -63);
}

TEST_F(ConverterTest, OctalToDecimal_Invalid_Throws) {
    EXPECT_THROW(Converter::octalToDecimal("78"), std::invalid_argument);
    EXPECT_THROW(Converter::octalToDecimal("9"), std::invalid_argument);
    EXPECT_THROW(Converter::octalToDecimal(""), std::invalid_argument);
}

// ============================================================================
// Generic Conversion Tests
// ============================================================================

TEST_F(ConverterTest, ConvertToBase_Base2) {
    EXPECT_EQ(Converter::convertToBase(10, 2), "1010");
    EXPECT_EQ(Converter::convertToBase(255, 2), "11111111");
}

TEST_F(ConverterTest, ConvertToBase_Base8) {
    EXPECT_EQ(Converter::convertToBase(10, 8), "12");
    EXPECT_EQ(Converter::convertToBase(63, 8), "77");
}

TEST_F(ConverterTest, ConvertToBase_Base16) {
    EXPECT_EQ(Converter::convertToBase(10, 16), "A");
    EXPECT_EQ(Converter::convertToBase(255, 16), "FF");
}

TEST_F(ConverterTest, ConvertToBase_Base36) {
    EXPECT_EQ(Converter::convertToBase(35, 36), "Z");
    EXPECT_EQ(Converter::convertToBase(1234, 36), "YA");
}

TEST_F(ConverterTest, FromBase_Base2) {
    EXPECT_EQ(Converter::fromBase("1010", 2), 10);
}

TEST_F(ConverterTest, FromBase_Base8) {
    EXPECT_EQ(Converter::fromBase("12", 8), 10);
}

TEST_F(ConverterTest, FromBase_Base16) {
    EXPECT_EQ(Converter::fromBase("A", 16), 10);
}

TEST_F(ConverterTest, ConvertFromBase_RoundTrip) {
    // Test round-trip conversion
    for (int i = 0; i <= 255; ++i) {
        std::string binary = Converter::convertToBase(i, 2);
        EXPECT_EQ(Converter::fromBase(binary, 2), i);

        std::string hex = Converter::convertToBase(i, 16);
        EXPECT_EQ(Converter::fromBase(hex, 16), i);

        std::string octal = Converter::convertToBase(i, 8);
        EXPECT_EQ(Converter::fromBase(octal, 8), i);
    }
}

TEST_F(ConverterTest, ConvertToBase_InvalidBase_Throws) {
    EXPECT_THROW(Converter::convertToBase(10, 1), std::invalid_argument);
    EXPECT_THROW(Converter::convertToBase(10, 37), std::invalid_argument);
    EXPECT_THROW(Converter::convertToBase(10, 0), std::invalid_argument);
}

TEST_F(ConverterTest, FromBase_InvalidBase_Throws) {
    EXPECT_THROW(Converter::fromBase("10", 1), std::invalid_argument);
    EXPECT_THROW(Converter::fromBase("10", 37), std::invalid_argument);
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST_F(ConverterTest, IsValidBinary_Valid) {
    EXPECT_TRUE(Converter::isValidBinary("0"));
    EXPECT_TRUE(Converter::isValidBinary("1"));
    EXPECT_TRUE(Converter::isValidBinary("1010"));
    EXPECT_TRUE(Converter::isValidBinary("11111111"));
    EXPECT_TRUE(Converter::isValidBinary("-1010"));
}

TEST_F(ConverterTest, IsValidBinary_Invalid) {
    EXPECT_FALSE(Converter::isValidBinary(""));
    EXPECT_FALSE(Converter::isValidBinary("2"));
    EXPECT_FALSE(Converter::isValidBinary("102"));
    EXPECT_FALSE(Converter::isValidBinary("abc"));
    EXPECT_FALSE(Converter::isValidBinary("-"));
}

TEST_F(ConverterTest, IsValidHex_Valid) {
    EXPECT_TRUE(Converter::isValidHex("0"));
    EXPECT_TRUE(Converter::isValidHex("A"));
    EXPECT_TRUE(Converter::isValidHex("a"));
    EXPECT_TRUE(Converter::isValidHex("FF"));
    EXPECT_TRUE(Converter::isValidHex("ff"));
    EXPECT_TRUE(Converter::isValidHex("1A2b"));
    EXPECT_TRUE(Converter::isValidHex("-FF"));
}

TEST_F(ConverterTest, IsValidHex_Invalid) {
    EXPECT_FALSE(Converter::isValidHex(""));
    EXPECT_FALSE(Converter::isValidHex("G"));
    EXPECT_FALSE(Converter::isValidHex("X"));
    EXPECT_FALSE(Converter::isValidHex("xyz"));
    EXPECT_FALSE(Converter::isValidHex("-"));
}

TEST_F(ConverterTest, IsValidOctal_Valid) {
    EXPECT_TRUE(Converter::isValidOctal("0"));
    EXPECT_TRUE(Converter::isValidOctal("7"));
    EXPECT_TRUE(Converter::isValidOctal("77"));
    EXPECT_TRUE(Converter::isValidOctal("100"));
    EXPECT_TRUE(Converter::isValidOctal("-77"));
}

TEST_F(ConverterTest, IsValidOctal_Invalid) {
    EXPECT_FALSE(Converter::isValidOctal(""));
    EXPECT_FALSE(Converter::isValidOctal("8"));
    EXPECT_FALSE(Converter::isValidOctal("9"));
    EXPECT_FALSE(Converter::isValidOctal("78"));
    EXPECT_FALSE(Converter::isValidOctal("-"));
}

// ============================================================================
// Formatting Tests
// ============================================================================

TEST_F(ConverterTest, FormatBinary) {
    EXPECT_EQ(Converter::formatBinary("1010"), "0b1010");
    EXPECT_EQ(Converter::formatBinary("0"), "0b0");
}

TEST_F(ConverterTest, FormatHex) {
    EXPECT_EQ(Converter::formatHex("FF"), "0xFF");
    EXPECT_EQ(Converter::formatHex("ff"), "0xFF");  // Should uppercase
    EXPECT_EQ(Converter::formatHex("1a"), "0x1A");
}

TEST_F(ConverterTest, FormatOctal) {
    EXPECT_EQ(Converter::formatOctal("77"), "0o77");
    EXPECT_EQ(Converter::formatOctal("0"), "0o0");
}

TEST_F(ConverterTest, Format_NumberBase) {
    EXPECT_EQ(Converter::format(10, NumberBase::BINARY), "0b1010");
    EXPECT_EQ(Converter::format(10, NumberBase::OCTAL), "0o12");
    EXPECT_EQ(Converter::format(10, NumberBase::HEXADECIMAL), "0xA");
    EXPECT_EQ(Converter::format(10, NumberBase::DECIMAL), "10");
}

TEST_F(ConverterTest, Format_Negative_NumberBase) {
    EXPECT_EQ(Converter::format(-10, NumberBase::BINARY), "-0b1010");
    EXPECT_EQ(Converter::format(-10, NumberBase::OCTAL), "-0o12");
    EXPECT_EQ(Converter::format(-10, NumberBase::HEXADECIMAL), "-0xA");
    EXPECT_EQ(Converter::format(-10, NumberBase::DECIMAL), "-10");
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ConverterTest, LargeNumberConversion) {
    long long largeValue = 0x7FFFFFFFFFFFFFFFLL;  // Max int64
    std::string hex = Converter::decimalToHex(largeValue);
    EXPECT_EQ(hex, "7FFFFFFFFFFFFFFF");
    EXPECT_EQ(Converter::hexToDecimal(hex), largeValue);
}

TEST_F(ConverterTest, MinValueConversion) {
    long long minValue = -9223372036854775807LL;  // Near min int64
    std::string binary = Converter::decimalToBinary(minValue);
    EXPECT_EQ(Converter::binaryToDecimal(binary), minValue);
}

TEST_F(ConverterTest, RoundTripMultipleValues) {
    // Test round-trip for various values
    std::vector<long long> testValues = {
        0, 1, -1, 42, -42, 255, -255, 1024, -1024,
        65535, -65535, 0xFFFFFFFFLL, -0xFFFFFFFFLL
    };

    for (long long value : testValues) {
        std::string binary = Converter::decimalToBinary(value);
        EXPECT_EQ(Converter::binaryToDecimal(binary), value)
            << "Failed round-trip for " << value << " (binary: " << binary << ")";

        std::string hex = Converter::decimalToHex(value);
        EXPECT_EQ(Converter::hexToDecimal(hex), value)
            << "Failed round-trip for " << value << " (hex: " << hex << ")";

        std::string octal = Converter::decimalToOctal(value);
        EXPECT_EQ(Converter::octalToDecimal(octal), value)
            << "Failed round-trip for " << value << " (octal: " << octal << ")";
    }
}

} // namespace calc
