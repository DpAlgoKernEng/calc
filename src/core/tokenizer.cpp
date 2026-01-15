/**
 * @file tokenizer.cpp
 * @brief Tokenizer implementation
 */

#include "calc/core/tokenizer.h"
#include <stdexcept>
#include <cctype>

namespace calc {

// ============================================================================
// Token Implementation
// ============================================================================

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::NUMBER:    return "NUMBER";
        case TokenType::OPERATOR:  return "OPERATOR";
        case TokenType::FUNCTION:  return "FUNCTION";
        case TokenType::LPAREN:    return "LPAREN";
        case TokenType::RPAREN:    return "RPAREN";
        case TokenType::COMMA:     return "COMMA";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN:   return "UNKNOWN";
        default:                   return "UNKNOWN";
    }
}

Token::Token() : type(TokenType::UNKNOWN), value(""), position(0) {}

Token::Token(TokenType t, const std::string& v, size_t pos)
    : type(t), value(v), position(pos) {}

bool Token::isOperator() const noexcept {
    return type == TokenType::OPERATOR;
}

bool Token::isNumber() const noexcept {
    return type == TokenType::NUMBER;
}

bool Token::isFunction() const noexcept {
    return type == TokenType::FUNCTION;
}

int Token::getPrecedence() const {
    if (!isOperator()) {
        return 0;  // For non-operators, return 0 instead of throwing
    }

    // Operator precedence table
    // Higher number = higher precedence
    if (value == "^") {
        return 4;  // Exponentiation
    } else if (value == "*" || value == "/" || value == "%") {
        return 3;  // Multiplication, division, modulo
    } else if (value == "+" || value == "-") {
        return 2;  // Addition, subtraction
    }

    throw std::runtime_error("Unknown operator: " + value);
}

bool Token::isRightAssociative() const {
    if (!isOperator()) {
        return false;  // For non-operators, return false instead of throwing
    }

    // Only exponentiation is right-associative
    return value == "^";
}

bool Token::operator==(const Token& other) const noexcept {
    return type == other.type && value == other.value && position == other.position;
}

bool Token::operator!=(const Token& other) const noexcept {
    return !(*this == other);
}

// ============================================================================
// Tokenizer Implementation
// ============================================================================

Tokenizer::Tokenizer(const std::string& input) : input_(input), pos_(0) {}

bool Tokenizer::isAtEnd() const noexcept {
    return pos_ >= input_.length();
}

char Tokenizer::current() const noexcept {
    if (isAtEnd()) {
        return '\0';
    }
    return input_[pos_];
}

char Tokenizer::peek(size_t offset) const noexcept {
    size_t newPos = pos_ + offset;
    if (newPos >= input_.length()) {
        return '\0';
    }
    return input_[newPos];
}

char Tokenizer::advance() noexcept {
    char c = current();
    if (!isAtEnd()) {
        ++pos_;
    }
    return c;
}

bool Tokenizer::isWhitespace(char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Tokenizer::isDigit(char c) noexcept {
    return c >= '0' && c <= '9';
}

bool Tokenizer::isLetter(char c) noexcept {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Tokenizer::isOperator(char c) noexcept {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
}

void Tokenizer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(current())) {
        advance();
    }
}

Token Tokenizer::readNumber() {
    size_t startPos = pos_;

    std::string number;
    bool hasDecimal = false;

    // Integer part
    while (!isAtEnd() && isDigit(current())) {
        number += advance();
    }

    // Decimal part (only one decimal point allowed)
    if (!isAtEnd() && current() == '.') {
        if (hasDecimal) {
            throw SyntaxError("Invalid number format: multiple decimal points", startPos);
        }
        hasDecimal = true;
        number += advance();

        // Must have at least one digit after decimal point
        if (isAtEnd() || !isDigit(current())) {
            throw SyntaxError("Invalid number format: decimal point without digits", startPos);
        }

        while (!isAtEnd() && isDigit(current())) {
            number += advance();
        }
    }

    // Scientific notation
    if (!isAtEnd() && (current() == 'e' || current() == 'E')) {
        number += advance();

        // Optional sign
        if (!isAtEnd() && (current() == '+' || current() == '-')) {
            number += advance();
        }

        // Must have at least one digit after 'e' or 'E'
        if (isAtEnd() || !isDigit(current())) {
            throw SyntaxError("Invalid number format: exponent without digits", startPos);
        }

        while (!isAtEnd() && isDigit(current())) {
            number += advance();
        }
    }

    return Token(TokenType::NUMBER, number, startPos);
}

Token Tokenizer::readIdentifier() {
    size_t startPos = pos_;
    std::string identifier;

    // First character must be a letter
    if (!isLetter(current())) {
        throw SyntaxError("Function names must start with a letter", startPos);
    }

    identifier += advance();

    // Rest can be letters or digits
    while (!isAtEnd() && (isLetter(current()) || isDigit(current()))) {
        identifier += advance();
    }

    return Token(TokenType::FUNCTION, identifier, startPos);
}

Token Tokenizer::readOperator() {
    size_t startPos = pos_;
    std::string op;
    op += advance();

    // Currently we only support single-character operators
    // Future extension could support multi-character operators (e.g., "**", "<=", etc.)

    return Token(TokenType::OPERATOR, op, startPos);
}

Token Tokenizer::readLeftParen() {
    size_t startPos = pos_;
    advance();
    return Token(TokenType::LPAREN, "(", startPos);
}

Token Tokenizer::readRightParen() {
    size_t startPos = pos_;
    advance();
    return Token(TokenType::RPAREN, ")", startPos);
}

Token Tokenizer::readComma() {
    size_t startPos = pos_;
    advance();
    return Token(TokenType::COMMA, ",", startPos);
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();

        if (isAtEnd()) {
            break;
        }

        char c = current();
        size_t startPos = pos_;

        if (isDigit(c) || (c == '.' && isDigit(peek(1)))) {
            // Check if previous token was a number - if so, this might be an error
            if (!tokens.empty() && tokens.back().type == TokenType::NUMBER) {
                // If we have a number followed by something that looks like a number,
                // and it starts with '.', this is an invalid multiple decimal
                if (c == '.') {
                    throw SyntaxError("Invalid number format: multiple decimal points", startPos);
                }
            }
            tokens.push_back(readNumber());
        } else if (c == '.') {
            // Lone decimal point - check if it's a valid number like ".5"
            // But this is handled by readNumber, so fall through to error
            throw SyntaxError("Unexpected character: '.'", startPos);
        } else if (isLetter(c)) {
            tokens.push_back(readIdentifier());
        } else if (isOperator(c)) {
            tokens.push_back(readOperator());
        } else if (c == '(') {
            tokens.push_back(readLeftParen());
        } else if (c == ')') {
            tokens.push_back(readRightParen());
        } else if (c == ',') {
            tokens.push_back(readComma());
        } else {
            throw SyntaxError(std::string("Unexpected character: '") + c + "'", startPos);
        }
    }

    // Add EOF token
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", pos_));

    return tokens;
}

} // namespace calc
