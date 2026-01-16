/**
 * @file converter.cpp
 * @brief Base conversion utilities implementation
 */

#include "calc/math/converter.h"
#include "calc/core/token.h"
#include <algorithm>
#include <stdexcept>
#include <cctype>

namespace calc {

// ============================================================================
// Decimal to other bases
// ============================================================================

std::string Converter::decimalToBinary(long long value) {
    return convertToBase(value, 2);
}

std::string Converter::decimalToHex(long long value) {
    std::string hex = convertToBase(value, 16);
    // Convert to uppercase
    std::transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
    return hex;
}

std::string Converter::decimalToOctal(long long value) {
    return convertToBase(value, 8);
}

// ============================================================================
// Other bases to decimal
// ============================================================================

long long Converter::binaryToDecimal(const std::string& binary) {
    return fromBase(binary, 2);
}

long long Converter::hexToDecimal(const std::string& hex) {
    return fromBase(hex, 16);
}

long long Converter::octalToDecimal(const std::string& octal) {
    return fromBase(octal, 8);
}

// ============================================================================
// Generic conversion
// ============================================================================

std::string Converter::convertToBase(long long value, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("Base must be between 2 and 36");
    }

    if (value == 0) {
        return "0";
    }

    // Handle negative numbers
    bool isNegative = value < 0;
    if (isNegative) {
        value = -value;
    }

    std::string result;
    while (value > 0) {
        int remainder = static_cast<int>(value % base);
        result.insert(result.begin(), digitToChar(remainder));
        value /= base;
    }

    if (isNegative) {
        result.insert(result.begin(), '-');
    }

    return result;
}

long long Converter::fromBase(const std::string& value, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("Base must be between 2 and 36");
    }

    if (value.empty()) {
        throw std::invalid_argument("Cannot convert empty string");
    }

    // Check if string is valid for the base
    if (!isValidDigitForBase(value[0], base) && value[0] != '-') {
        throw std::invalid_argument("Invalid digit for base " + std::to_string(base) + ": '" + std::string(1, value[0]) + "'");
    }

    size_t start = 0;
    bool isNegative = false;

    if (value[0] == '-') {
        isNegative = true;
        start = 1;
    }

    long long result = 0;
    for (size_t i = start; i < value.length(); ++i) {
        char c = value[i];
        if (!isValidDigitForBase(c, base)) {
            throw std::invalid_argument("Invalid digit for base " + std::to_string(base) + ": '" + std::string(1, c) + "'");
        }
        result = result * base + charToDigit(c);
    }

    return isNegative ? -result : result;
}

// ============================================================================
// Validation functions
// ============================================================================

bool Converter::isValidBinary(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    size_t start = 0;
    if (value[0] == '-') {
        start = 1;
    }

    if (start == value.length()) {
        return false;
    }

    for (size_t i = start; i < value.length(); ++i) {
        if (value[i] != '0' && value[i] != '1') {
            return false;
        }
    }
    return true;
}

bool Converter::isValidHex(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    size_t start = 0;
    if (value[0] == '-') {
        start = 1;
    }

    if (start == value.length()) {
        return false;
    }

    for (size_t i = start; i < value.length(); ++i) {
        if (!std::isxdigit(static_cast<unsigned char>(value[i]))) {
            return false;
        }
    }
    return true;
}

bool Converter::isValidOctal(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    size_t start = 0;
    if (value[0] == '-') {
        start = 1;
    }

    if (start == value.length()) {
        return false;
    }

    for (size_t i = start; i < value.length(); ++i) {
        if (value[i] < '0' || value[i] > '7') {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Formatting functions
// ============================================================================

std::string Converter::formatBinary(const std::string& binary) {
    return "0b" + binary;
}

std::string Converter::formatHex(const std::string& hex) {
    std::string formatted = hex;
    std::transform(formatted.begin(), formatted.end(), formatted.begin(), ::toupper);
    return "0x" + formatted;
}

std::string Converter::formatOctal(const std::string& octal) {
    return "0o" + octal;
}

std::string Converter::format(long long value, NumberBase base) {
    switch (base) {
        case NumberBase::BINARY:
            if (value < 0) {
                return "-" + formatBinary(decimalToBinary(-value));
            }
            return formatBinary(decimalToBinary(value));
        case NumberBase::OCTAL:
            if (value < 0) {
                return "-" + formatOctal(decimalToOctal(-value));
            }
            return formatOctal(decimalToOctal(value));
        case NumberBase::HEXADECIMAL:
            if (value < 0) {
                return "-" + formatHex(decimalToHex(-value));
            }
            return formatHex(decimalToHex(value));
        case NumberBase::DECIMAL:
        default:
            return std::to_string(value);
    }
}

// ============================================================================
// Private helper functions
// ============================================================================

int Converter::charToDigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        return 10 + (c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        return 10 + (c - 'a');
    }
    throw std::invalid_argument("Invalid digit character: '" + std::string(1, c) + "'");
}

char Converter::digitToChar(int value) {
    if (value >= 0 && value <= 9) {
        return static_cast<char>('0' + value);
    } else if (value >= 10 && value <= 35) {
        return static_cast<char>('A' + value - 10);
    }
    throw std::invalid_argument("Invalid digit value: " + std::to_string(value));
}

bool Converter::isValidDigitForBase(char c, int base) {
    if (base <= 10) {
        return c >= '0' && c < ('0' + base);
    }
    if (c >= '0' && c <= '9') {
        return true;
    }
    char upperC = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
    if (upperC >= 'A' && upperC < ('A' + base - 10)) {
        return true;
    }
    return false;
}

} // namespace calc
