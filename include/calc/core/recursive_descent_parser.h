/**
 * @file recursive_descent_parser.h
 * @brief Recursive descent parser implementation
 */

#ifndef CALC_CORE_RECURSIVE_DESCENT_PARSER_H
#define CALC_CORE_RECURSIVE_DESCENT_PARSER_H

#include "calc/core/parser.h"
#include "calc/core/token.h"
#include "calc/core/ast.h"
#include "calc/utils/error.h"
#include <vector>
#include <memory>

namespace calc {

/**
 * @brief Parser implementation using recursive descent parsing
 *
 * Recursive descent parsing is a top-down parsing technique where each
 * non-terminal in the grammar has a corresponding function that parses
 * that portion of the input. This parser follows the grammar:
 *
 * expression   ::= term (( '+' | '-' ) term)*
 * term         ::= factor (( '*' | '/' | '%' ) factor)*
 * factor       ::= power ('^' factor)?
 * power        ::= unary | postfix '^' power
 * unary        ::= ('+' | '-') unary | postfix
 * postfix      ::= primary ( '(' arguments? ')' )?
 * primary      ::= NUMBER | '(' expression ')' | FUNCTION
 * arguments    ::= expression (',' expression)*
 *
 * This implementation handles:
 * - Operator precedence (power > mul/div/mod > add/sub)
 * - Operator associativity (^ is right-associative, others are left)
 * - Parentheses for grouping
 * - Function calls with multiple arguments
 * - Unary operators (+ and -)
 *
 * Advantages over shunting-yard:
 * - More intuitive and easier to understand
 * - Better error messages (knows exactly what was expected)
 * - Easier to extend with new grammar rules
 */
class RecursiveDescentParser : public Parser {
public:
    /**
     * @brief Construct a recursive descent parser
     * @param enableUnaryOperators Enable support for unary + and - operators
     */
    explicit RecursiveDescentParser(bool enableUnaryOperators = true);

    /**
     * @brief Parse tokens into an AST
     * @param tokens The token stream to parse
     * @return Root node of the constructed AST
     * @throws SyntaxError on parsing errors
     */
    std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) override;

    /**
     * @brief Get parser name
     */
    std::string getName() const override { return "RecursiveDescentParser"; }

    /**
     * @brief Enable or disable unary operator detection
     * @param enable Whether to enable unary operators
     */
    void setUnaryOperatorsEnabled(bool enable) noexcept {
        enableUnaryOperators_ = enable;
    }

    /**
     * @brief Check if unary operators are enabled
     */
    bool isUnaryOperatorsEnabled() const noexcept {
        return enableUnaryOperators_;
    }

private:
    // =========================================================================
    // Token stream management
    // =========================================================================

    /**
     * @brief Get current token
     * @return Current token, or EOF if at end
     */
    const Token& peek() const noexcept;

    /**
     * @brief Get token at offset from current position
     * @param offset Offset (0 = current, 1 = next, etc.)
     * @return Token at offset position
     */
    const Token& peek(size_t offset) const noexcept;

    /**
     * @brief Consume current token and advance
     * @return The consumed token
     */
    const Token& advance() noexcept;

    /**
     * @brief Check if we're at end of input
     * @return true if current token is EOF
     */
    bool isAtEnd() const noexcept;

    /**
     * @brief Check if current token matches expected type
     * @param type Expected token type
     * @return true if current token type matches
     */
    bool match(TokenType type) const noexcept;

    /**
     * @brief Consume current token if it matches expected type
     * @param type Expected token type
     * @return true if token was consumed
     */
    bool consume(TokenType type) noexcept;

    /**
     * @brief Consume current token, throw if it doesn't match expected type
     * @param type Expected token type
     * @param errorMessage Custom error message
     * @return The consumed token
     * @throws SyntaxError if token type doesn't match
     */
    const Token& expect(TokenType type, const std::string& errorMessage);

    // =========================================================================
    // Grammar parsing methods (in order of precedence)
    // =========================================================================

    /**
     * @brief Parse an expression
     * expression ::= term (( '+' | '-' ) term)*
     * This is the entry point for parsing expressions
     */
    std::unique_ptr<ASTNode> parseExpression();

    /**
     * @brief Parse a term
     * term ::= factor (( '*' | '/' | '%' ) factor)*
     */
    std::unique_ptr<ASTNode> parseTerm();

    /**
     * @brief Parse a factor with exponentiation (right-associative)
     * factor ::= power ('^' factor)?
     */
    std::unique_ptr<ASTNode> parseFactor();

    /**
     * @brief Parse a power
     * power ::= unary | postfix '^' power
     */
    std::unique_ptr<ASTNode> parsePower();

    /**
     * @brief Parse the right side of a power operator
     * Helper method that allows unary operators after '^'
     */
    std::unique_ptr<ASTNode> parsePowerRightSide();

    /**
     * @brief Parse a unary operation
     * unary ::= ('+' | '-') unary | postfix
     */
    std::unique_ptr<ASTNode> parseUnary();

    /**
     * @brief Parse a postfix expression (function calls)
     * postfix ::= primary ( '(' arguments? ')' )?
     */
    std::unique_ptr<ASTNode> parsePostfix();

    /**
     * @brief Parse a primary expression
     * primary ::= NUMBER | '(' expression ')' | FUNCTION
     */
    std::unique_ptr<ASTNode> parsePrimary();

    /**
     * @brief Parse function arguments
     * arguments ::= expression (',' expression)*
     * @return Vector of argument AST nodes
     */
    std::vector<std::unique_ptr<ASTNode>> parseArguments();

    /**
     * @brief Check if a token is a binary operator
     */
    static bool isBinaryOperator(const Token& token) noexcept;

    /**
     * @brief Get the precedence level of a binary operator
     * @param op The operator token
     * @return Precedence level (higher = higher precedence)
     */
    static int getPrecedence(const Token& op);

    /**
     * @brief Check if an operator is right-associative
     * @param op The operator token
     * @return true if right-associative
     */
    static bool isRightAssociative(const Token& op) noexcept;

    // =========================================================================
    // Member variables
    // =========================================================================

    std::vector<Token> tokens_;      ///< Token stream being parsed
    size_t current_;                ///< Current position in token stream
    bool enableUnaryOperators_;     ///< Enable unary + and - detection
};

} // namespace calc

#endif // CALC_CORE_RECURSIVE_DESCENT_PARSER_H
