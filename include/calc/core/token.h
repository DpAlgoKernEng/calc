/**
 * @file token.h
 * @brief Token type and Token struct for lexical analysis
 */

#ifndef CALC_CORE_TOKEN_H
#define CALC_CORE_TOKEN_H

#include <string>
#include <cstddef>

namespace calc {

/**
 * @brief Enumeration of token types for lexical analysis
 */
enum class TokenType {
    NUMBER,          ///< Numeric literal (integer or floating-point)
    OPERATOR,        ///< Arithmetic operator (+, -, *, /, ^, etc.)
    FUNCTION,        ///< Function name (sin, cos, sqrt, etc.)
    LPAREN,          ///< Left parenthesis '('
    RPAREN,          ///< Right parenthesis ')'
    COMMA,           ///< Comma separator for function arguments
    EOF_TOKEN,       ///< End of input marker
    UNKNOWN          ///< Unrecognized token
};

/**
 * @brief Number base enumeration for programmer mode
 */
enum class NumberBase {
    DECIMAL,    ///< Default: 42, 3.14
    BINARY,     ///< 0b1010, 0B1010
    OCTAL,      ///< 0o77, 0o17
    HEXADECIMAL ///< 0xFF, 0XFF
};

/**
 * @brief Convert token type to string representation
 * @param type The token type to convert
 * @return Human-readable string representation
 */
std::string tokenTypeToString(TokenType type);

/**
 * @brief Represents a single lexical token from the input
 */
struct Token {
    TokenType type;      ///< The type of this token
    std::string value;   ///< The string value of this token
    size_t position;     ///< Starting position in original input string
    size_t argCount;     ///< Number of arguments for function tokens (default 0)
    NumberBase numberBase; ///< Number base for number tokens (default: DECIMAL)

    /**
     * @brief Construct a default token
     */
    Token();

    /**
     * @brief Construct a token with all fields
     * @param t The token type
     * @param v The token value string
     * @param pos The starting position
     */
    Token(TokenType t, const std::string& v, size_t pos);

    /**
     * @brief Construct a token with number base
     * @param t The token type
     * @param v The token value string
     * @param pos The starting position
     * @param base The number base (for number tokens)
     */
    Token(TokenType t, const std::string& v, size_t pos, NumberBase base);

    /**
     * @brief Check if this token is an operator
     * @return true if type is OPERATOR
     */
    bool isOperator() const noexcept;

    /**
     * @brief Check if this token is a number
     * @return true if type is NUMBER
     */
    bool isNumber() const noexcept;

    /**
     * @brief Check if this token is a function
     * @return true if type is FUNCTION
     */
    bool isFunction() const noexcept;

    /**
     * @brief Get the precedence of this operator token
     * @return Precedence level (higher = higher precedence)
     * @throws std::runtime_error if token is not an operator
     */
    int getPrecedence() const;

    /**
     * @brief Check if this operator is right-associative
     * @return true if right-associative (e.g., exponentiation ^)
     * @throws std::runtime_error if token is not an operator
     */
    bool isRightAssociative() const;

    /**
     * @brief Equality comparison
     */
    bool operator==(const Token& other) const noexcept;

    /**
     * @brief Inequality comparison
     */
    bool operator!=(const Token& other) const noexcept;
};

} // namespace calc

// ============================================================================
// Helper functions for base detection (inline functions)
// ============================================================================

namespace calc {

/**
 * @brief Check if character sequence is a binary prefix (0b or 0B)
 * @param str The string to check
 * @param pos Position in string
 * @return true if binary prefix found
 */
inline bool isBinaryPrefix(const std::string& str, size_t pos) {
    return (pos + 1 < str.length() && str[pos] == '0' &&
            (str[pos + 1] == 'b' || str[pos + 1] == 'B'));
}

/**
 * @brief Check if character sequence is a hex prefix (0x or 0X)
 * @param str The string to check
 * @param pos Position in string
 * @return true if hex prefix found
 */
inline bool isHexPrefix(const std::string& str, size_t pos) {
    return (pos + 1 < str.length() && str[pos] == '0' &&
            (str[pos + 1] == 'x' || str[pos + 1] == 'X'));
}

/**
 * @brief Check if character sequence is an octal prefix (0o or 0O)
 * @param str The string to check
 * @param pos Position in string
 * @return true if octal prefix found
 */
inline bool isOctalPrefix(const std::string& str, size_t pos) {
    return (pos + 1 < str.length() && str[pos] == '0' &&
            (str[pos + 1] == 'o' || str[pos + 1] == 'O'));
}

} // namespace calc

#endif // CALC_CORE_TOKEN_H
