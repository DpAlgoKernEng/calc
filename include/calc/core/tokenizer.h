/**
 * @file tokenizer.h
 * @brief Tokenizer class for lexical analysis
 */

#ifndef CALC_CORE_TOKENIZER_H
#define CALC_CORE_TOKENIZER_H

#include "calc/core/token.h"
#include "calc/utils/error.h"
#include <vector>
#include <string>

namespace calc {

/**
 * @brief Converts input strings into streams of lexical tokens
 *
 * The Tokenizer processes character-by-character, recognizing:
 * - Numeric literals (including decimals and scientific notation)
 * - Operators (+, -, *, /, ^, %)
 * - Function names (alphanumeric starting with letter)
 * - Parentheses and commas
 * - Whitespace (ignored)
 */
class Tokenizer {
public:
    /**
     * @brief Construct a tokenizer for the given input
     * @param input The input string to tokenize
     */
    explicit Tokenizer(const std::string& input);

    /**
     * @brief Tokenize the entire input string
     * @return Vector of tokens in order of appearance
     * @throws SyntaxError if invalid syntax is encountered
     */
    std::vector<Token> tokenize();

private:
    /**
     * @brief Check if current position is at end of input
     * @return true if pos_ >= input_.length()
     */
    bool isAtEnd() const noexcept;

    /**
     * @brief Get current character
     * @return The character at current position, or '\0' if at end
     */
    char current() const noexcept;

    /**
     * @brief Get character at offset from current position
     * @param offset The offset (0 = current, 1 = next, etc.)
     * @return The character at offset, or '\0' if out of bounds
     */
    char peek(size_t offset = 1) const noexcept;

    /**
     * @brief Advance to next character and return previous
     * @return The character before advancing
     */
    char advance() noexcept;

    /**
     * @brief Skip over whitespace characters
     */
    void skipWhitespace();

    /**
     * @brief Check if character is whitespace
     */
    static bool isWhitespace(char c) noexcept;

    /**
     * @brief Check if character is a digit (0-9)
     */
    static bool isDigit(char c) noexcept;

    /**
     * @brief Check if character is a letter (a-z, A-Z)
     */
    static bool isLetter(char c) noexcept;

    /**
     * @brief Check if character is an operator
     */
    static bool isOperator(char c) noexcept;

    /**
     * @brief Tokenize a numeric literal with base prefix (0b, 0x, 0o)
     * Handles binary, octal, hexadecimal, and decimal numbers
     * @throws SyntaxError on invalid number format
     */
    Token readNumberWithPrefix();

    /**
     * @brief Tokenize a numeric literal (legacy decimal-only)
     * Handles integers, decimals, and scientific notation
     * @throws SyntaxError on invalid number format
     */
    Token readNumber();

    /**
     * @brief Tokenize a function name or identifier
     * @throws SyntaxError on invalid identifier format
     */
    Token readIdentifier();

    /**
     * @brief Tokenize a single-character operator
     */
    Token readOperator();

    /**
     * @brief Tokenize a left parenthesis
     */
    Token readLeftParen();

    /**
     * @brief Tokenize a right parenthesis
     */
    Token readRightParen();

    /**
     * @brief Tokenize a comma
     */
    Token readComma();

    std::string input_;   ///< Input string being tokenized
    size_t pos_;          ///< Current position in input_
};

} // namespace calc

#endif // CALC_CORE_TOKENIZER_H
