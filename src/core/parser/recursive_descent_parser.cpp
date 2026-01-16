/**
 * @file recursive_descent_parser.cpp
 * @brief Recursive descent parser implementation
 */

#include "calc/core/recursive_descent_parser.h"
#include <sstream>

namespace calc {

// ============================================================================
// Construction / Entry Point
// ============================================================================

RecursiveDescentParser::RecursiveDescentParser(bool enableUnaryOperators)
    : current_(0), enableUnaryOperators_(enableUnaryOperators) {}

std::unique_ptr<ASTNode> RecursiveDescentParser::parse(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    current_ = 0;

    if (isAtEnd()) {
        throw SyntaxError("Empty expression", 0);
    }

    auto result = parseExpression();

    // Ensure we've consumed all input
    if (!isAtEnd() && peek().type != TokenType::EOF_TOKEN) {
        throw SyntaxError("Unexpected token at end of expression: " + peek().value,
                        peek().position);
    }

    return result;
}

// ============================================================================
// Token Stream Management
// ============================================================================

const Token& RecursiveDescentParser::peek() const noexcept {
    if (current_ >= tokens_.size()) {
        // Return EOF token if at end
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0);
        return eofToken;
    }
    return tokens_[current_];
}

const Token& RecursiveDescentParser::peek(size_t offset) const noexcept {
    size_t index = current_ + offset;
    if (index >= tokens_.size()) {
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0);
        return eofToken;
    }
    return tokens_[index];
}

const Token& RecursiveDescentParser::advance() noexcept {
    if (!isAtEnd()) {
        ++current_;
    }
    return peek();
}

bool RecursiveDescentParser::isAtEnd() const noexcept {
    return current_ >= tokens_.size() || peek().type == TokenType::EOF_TOKEN;
}

bool RecursiveDescentParser::match(TokenType type) const noexcept {
    return peek().type == type;
}

bool RecursiveDescentParser::consume(TokenType type) noexcept {
    if (match(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& RecursiveDescentParser::expect(TokenType type, const std::string& errorMessage) {
    if (!match(type)) {
        std::ostringstream oss;
        oss << errorMessage << ", found: " << peek().value << " (";
        oss << tokenTypeToString(peek().type) << ")";
        throw SyntaxError(oss.str(), peek().position);
    }
    const Token& token = peek();
    advance();
    return token;
}

// ============================================================================
// Grammar Parsing Methods
// ============================================================================

// expression ::= term (( '+' | '-' ) term)*
std::unique_ptr<ASTNode> RecursiveDescentParser::parseExpression() {
    auto left = parseTerm();

    while (match(TokenType::OPERATOR) &&
           (peek().value == "+" || peek().value == "-")) {
        Token op = peek();
        advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }

    return left;
}

// term ::= factor (( '*' | '/' | '%' ) factor)*
std::unique_ptr<ASTNode> RecursiveDescentParser::parseTerm() {
    auto left = parseFactor();

    while (match(TokenType::OPERATOR) &&
           (peek().value == "*" || peek().value == "/" || peek().value == "%")) {
        Token op = peek();
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }

    return left;
}

// factor ::= unary
// Note: This calls parseUnary which handles unary operators before power
std::unique_ptr<ASTNode> RecursiveDescentParser::parseFactor() {
    return parseUnary();
}

// unary ::= ('+' | '-') unary | power
// This handles unary operators with higher precedence than power
// For -2^3, this returns UnaryOpNode(-, PowerNode(2, 3))
std::unique_ptr<ASTNode> RecursiveDescentParser::parseUnary() {
    if (!enableUnaryOperators_) {
        return parsePower();
    }

    // Check for unary + or -
    if (match(TokenType::OPERATOR) && (peek().value == "+" || peek().value == "-")) {
        Token op = peek();
        advance();

        // Parse the operand (which will handle power, postfix, etc.)
        // This ensures unary operators bind tighter than exponentiation
        auto operand = parseUnary();  // Recurse for nested unary like --5
        return std::make_unique<UnaryOpNode>(op, std::move(operand));
    }

    return parsePower();
}

// power ::= postfix '^' power
// This handles right-associative exponentiation: 2^3^4 = 2^(3^4)
std::unique_ptr<ASTNode> RecursiveDescentParser::parsePower() {
    auto left = parsePostfix();

    if (match(TokenType::OPERATOR) && peek().value == "^") {
        Token op = peek();
        advance();
        // For the right side of '^', we need to allow unary operators
        // But we can't directly call parsePower() recursively because that would
        // go through parseFactor() -> parseUnary() -> parsePower() and miss unary support
        // Solution: Parse the right side starting from the lowest precedence that
        // allows unary operators, which is parseUnary()
        auto right = parsePowerRightSide();
        return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }

    return left;
}

// Helper method to parse the right side of '^' operator
// This allows unary operators and maintains right-associativity
// For 2^3^4, this returns PowerNode(3, 4), and for 2^-2, this returns UnaryOpNode(-, 2)
std::unique_ptr<ASTNode> RecursiveDescentParser::parsePowerRightSide() {
    // Check for unary + or - (same as in parseUnary)
    if (enableUnaryOperators_ &&
        match(TokenType::OPERATOR) && (peek().value == "+" || peek().value == "-")) {
        Token op = peek();
        advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryOpNode>(op, std::move(operand));
    }

    // Parse postfix (which handles numbers, parentheses, functions)
    auto node = parsePostfix();

    // Check for another '^' to maintain right-associativity
    if (match(TokenType::OPERATOR) && peek().value == "^") {
        Token op = peek();
        advance();
        auto right = parsePowerRightSide();
        return std::make_unique<BinaryOpNode>(std::move(node), op, std::move(right));
    }

    return node;
}

// postfix ::= primary ( '(' arguments? ')' )?
std::unique_ptr<ASTNode> RecursiveDescentParser::parsePostfix() {
    auto node = parsePrimary();

    // Check for function call: identifier '(' args? ')'
    if (match(TokenType::LPAREN)) {
        // If the previous node was not a function token, this might be parenthesized grouping
        // But in our grammar, functions are FUNCTION tokens, so this is a function call
        // Check if the primary was a function call node
        auto* funcNode = dynamic_cast<FunctionCallNode*>(node.get());

        if (funcNode) {
            advance();  // Consume '('
            auto args = parseArguments();
            expect(TokenType::RPAREN, "Expected ')' after function arguments");

            // Create new function call node with parsed arguments
            node = std::make_unique<FunctionCallNode>(
                funcNode->getName(),
                funcNode->getPosition(),
                std::move(args)
            );
        } else {
            // This is parenthesized expression, not a function call
            // But parsePrimary already handled '(' expression ')' case
            // So we shouldn't get here with a non-function node
            // This could be a case like (42) where the parentheses are redundant
            advance();  // Consume '('
            auto args = parseArguments();
            expect(TokenType::RPAREN, "Expected ')' after expression");

            // If we have a single argument and no comma, it was just grouped parentheses
            // Return the argument directly (grouping doesn't need a special node)
            if (args.size() == 1) {
                node = std::move(args[0]);
            }
        }
    }

    return node;
}

// primary ::= NUMBER | '(' expression ')' | FUNCTION
std::unique_ptr<ASTNode> RecursiveDescentParser::parsePrimary() {
    // Number literal
    if (match(TokenType::NUMBER)) {
        const Token& token = peek();
        advance();
        try {
            double value = std::stod(token.value);
            return std::make_unique<LiteralNode>(value);
        } catch (const std::exception&) {
            throw SyntaxError("Invalid number: " + token.value, token.position);
        }
    }

    // Parenthesized expression
    if (match(TokenType::LPAREN)) {
        advance();  // Consume '('
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    // Function name (will be followed by '(' in parsePostfix)
    if (match(TokenType::FUNCTION)) {
        const Token& token = peek();
        advance();
        // Create a placeholder function call node with no arguments
        // parsePostfix will fill in the arguments
        return std::make_unique<FunctionCallNode>(
            token.value, token.position,
            std::vector<std::unique_ptr<ASTNode>>{}
        );
    }

    // Unexpected token
    std::ostringstream oss;
    oss << "Expected number, '(', or function, found: " << peek().value;
    throw SyntaxError(oss.str(), peek().position);
}

// arguments ::= expression (',' expression)*
std::vector<std::unique_ptr<ASTNode>> RecursiveDescentParser::parseArguments() {
    std::vector<std::unique_ptr<ASTNode>> args;

    // Check if we have any arguments (empty argument list is valid)
    if (match(TokenType::RPAREN)) {
        return args;  // Empty argument list
    }

    // Parse first argument
    args.push_back(parseExpression());

    // Parse additional arguments
    while (consume(TokenType::COMMA)) {
        args.push_back(parseExpression());
    }

    return args;
}

// ============================================================================
// Operator Utilities
// ============================================================================

bool RecursiveDescentParser::isBinaryOperator(const Token& token) noexcept {
    if (token.type != TokenType::OPERATOR) {
        return false;
    }
    return token.value == "+" || token.value == "-" ||
           token.value == "*" || token.value == "/" ||
           token.value == "%" || token.value == "^";
}

int RecursiveDescentParser::getPrecedence(const Token& op) {
    if (!isBinaryOperator(op)) {
        return 0;
    }

    if (op.value == "^") {
        return 4;  // Exponentiation (highest)
    } else if (op.value == "*" || op.value == "/" || op.value == "%") {
        return 3;  // Multiplication, division, modulo
    } else if (op.value == "+" || op.value == "-") {
        return 2;  // Addition, subtraction
    }

    return 0;
}

bool RecursiveDescentParser::isRightAssociative(const Token& op) noexcept {
    // Only exponentiation is right-associative
    return op.type == TokenType::OPERATOR && op.value == "^";
}

} // namespace calc
