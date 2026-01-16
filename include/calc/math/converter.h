/**
 * @file converter.h
 * @brief Base conversion utilities for programmer mode
 */

#ifndef CALC_MATH_CONVERTER_H
#define CALC_MATH_CONVERTER_H

#include <string>
#include <algorithm>
#include <stdexcept>
#include <cctype>

namespace calc {

// Forward declaration of NumberBase (defined in token.h)
enum class NumberBase;

/**
 * @brief Base conversion utilities for programmer mode
 *
 * Provides conversion between decimal, binary, octal, and hexadecimal representations.
 * Supports both direct conversions and validation of number strings.
 */
class Converter {
public:
    // ========================================================================
    // Decimal to other bases
    // ========================================================================

    /**
     * @brief Convert decimal value to binary string
     * @param value The decimal value to convert
     * @return Binary string representation (e.g., "1010" for 10)
     */
    static std::string decimalToBinary(long long value);

    /**
     * @brief Convert decimal value to hexadecimal string
     * @param value The decimal value to convert
     * @return Hexadecimal string (uppercase, e.g., "FF" for 255)
     */
    static std::string decimalToHex(long long value);

    /**
     * @brief Convert decimal value to octal string
     * @param value The decimal value to convert
     * @return Octal string (e.g., "77" for 63)
     */
    static std::string decimalToOctal(long long value);

    // ========================================================================
    // Other bases to decimal
    // ========================================================================

    /**
     * @brief Convert binary string to decimal value
     * @param binary Binary string (e.g., "1010")
     * @return Decimal value
     * @throws std::invalid_argument if string is not a valid binary number
     */
    static long long binaryToDecimal(const std::string& binary);

    /**
     * @brief Convert hexadecimal string to decimal value
     * @param hex Hexadecimal string (case-insensitive, e.g., "FF" or "ff")
     * @return Decimal value
     * @throws std::invalid_argument if string is not a valid hexadecimal number
     */
    static long long hexToDecimal(const std::string& hex);

    /**
     * @brief Convert octal string to decimal value
     * @param octal Octal string (e.g., "77")
     * @return Decimal value
     * @throws std::invalid_argument if string is not a valid octal number
     */
    static long long octalToDecimal(const std::string& octal);

    // ========================================================================
    // Generic conversion
    // ========================================================================

    /**
     * @brief Convert decimal value to arbitrary base string
     * @param value The decimal value to convert
     * @param base The target base (2-36)
     * @return String representation in specified base
     * @throws std::invalid_argument if base is not in range [2, 36]
     */
    static std::string convertToBase(long long value, int base);

    /**
     * @brief Convert string in arbitrary base to decimal value
     * @param value The string to convert
     * @param base The source base (2-36)
     * @return Decimal value
     * @throws std::invalid_argument if base is not in range [2, 36] or string is invalid
     */
    static long long fromBase(const std::string& value, int base);

    // ========================================================================
    // Validation functions
    // ========================================================================

    /**
     * @brief Check if string is a valid binary number
     * @param value The string to validate
     * @return true if valid, false otherwise
     */
    static bool isValidBinary(const std::string& value);

    /**
     * @brief Check if string is a valid hexadecimal number
     * @param value The string to validate
     * @return true if valid, false otherwise
     */
    static bool isValidHex(const std::string& value);

    /**
     * @brief Check if string is a valid octal number
     * @param value The string to validate
     * @return true if valid, false otherwise
     */
    static bool isValidOctal(const std::string& value);

    // ========================================================================
    // Formatting functions
    // ========================================================================

    /**
     * @brief Format binary string with prefix
     * @param binary The binary string
     * @return Formatted string with "0b" prefix
     */
    static std::string formatBinary(const std::string& binary);

    /**
     * @brief Format hexadecimal string with prefix
     * @param hex The hexadecimal string
     * @return Formatted string with "0x" prefix and uppercase letters
     */
    static std::string formatHex(const std::string& hex);

    /**
     * @brief Format octal string with prefix
     * @param octal The octal string
     * @return Formatted string with "0o" prefix
     */
    static std::string formatOctal(const std::string& octal);

    /**
     * @brief Format value to specified base with appropriate prefix
     * @param value The decimal value
     * @param base The target base
     * @return Formatted string with prefix
     */
    static std::string format(long long value, NumberBase base);

private:
    /**
     * @brief Convert digit character to its numeric value
     * @param c Character to convert (0-9, A-Z, a-z)
     * @return Numeric value (0-35)
     * @throws std::invalid_argument if character is not a valid digit
     */
    static int charToDigit(char c);

    /**
     * @brief Convert numeric value to digit character
     * @param value Value to convert (0-35)
     * @return Character representation (0-9, A-Z)
     * @throws std::invalid_argument if value is out of range
     */
    static char digitToChar(int value);

    /**
     * @brief Check if character is a valid digit in given base
     * @param c Character to check
     * @param base The base to check against
     * @return true if valid, false otherwise
     */
    static bool isValidDigitForBase(char c, int base);
};

} // namespace calc

#endif // CALC_MATH_CONVERTER_H
