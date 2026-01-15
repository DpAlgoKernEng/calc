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

#endif // CALC_CORE_TOKEN_H
