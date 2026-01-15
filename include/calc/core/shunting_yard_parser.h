/**
 * @file shunting_yard_parser.h
 * @brief Shunting-yard algorithm parser implementation
 */

#ifndef CALC_CORE_SHUNTING_YARD_PARSER_H
#define CALC_CORE_SHUNTING_YARD_PARSER_H

#include "calc/core/parser.h"
#include "calc/core/token.h"
#include "calc/core/ast.h"
#include "calc/utils/error.h"
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>

namespace calc {

/**
 * @brief Parser implementation using Dijkstra's shunting-yard algorithm
 *
 * The shunting-yard algorithm converts infix notation expressions
 * to postfix notation (Reverse Polish Notation), which is then
 * used to build an Abstract Syntax Tree.
 *
 * Algorithm steps:
 * 1. Convert infix tokens to postfix (RPN) using operator stacks
 * 2. Build AST from postfix representation using operand stacks
 *
 * This implementation handles:
 * - Operator precedence and associativity
 * - Parentheses for grouping
 * - Function calls with multiple arguments
 * - Unary operators (prefix + and -)
 */
class ShuntingYardParser : public Parser {
public:
    /**
     * @brief Construct a shunting-yard parser
     * @param enableUnaryOperators Enable support for unary + and - operators
     */
    explicit ShuntingYardParser(bool enableUnaryOperators = true);

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
    std::string getName() const override { return "ShuntingYardParser"; }

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
    /**
     * @brief Check if a token at position should be treated as unary
     * @param tokens The token stream
     * @param index The index to check
     * @return true if token at index is a unary operator
     */
    bool isUnaryOperator(const std::vector<Token>& tokens, size_t index) const;

    /**
     * @brief Get operator precedence level
     * @param op The operator token
     * @return Precedence level (higher = higher precedence)
     * @throws SyntaxError if token is not a recognized operator
     */
    int getPrecedence(const Token& op) const;

    /**
     * @brief Check if operator is right-associative
     * @param op The operator token
     * @return true if right-associative
     */
    bool isRightAssociative(const Token& op) const;

    /**
     * @brief Convert infix tokens to postfix (RPN) notation
     * @param tokens Input token stream
     * @return Vector of tokens in postfix order
     * @throws SyntaxError on conversion errors
     */
    std::vector<Token> toPostfix(const std::vector<Token>& tokens);

    /**
     * @brief Build AST from postfix (RPN) tokens
     * @param postfixTokens Tokens in postfix order
     * @return Root node of the constructed AST
     * @throws SyntaxError on tree construction errors
     */
    std::unique_ptr<ASTNode> buildAST(const std::vector<Token>& postfixTokens);

    /**
     * @brief Pop and apply operator from stack to operands
     * @param op The operator to apply
     * @param operands Stack of operand nodes
     * @throws SyntaxError if insufficient operands or invalid operation
     */
    void applyOperator(const Token& op,
                      std::stack<std::unique_ptr<ASTNode>>& operands);

    /**
     * @brief Build a function call node
     * @param name The function name
     * @param position The position in original input
     * @param operandCount Number of operands to use as arguments
     * @param operands Stack to pop operands from
     * @return Function call node
     * @throws SyntaxError if insufficient operands
     */
    std::unique_ptr<FunctionCallNode> buildFunctionCall(
        const std::string& name,
        size_t position,
        size_t operandCount,
        std::stack<std::unique_ptr<ASTNode>>& operands);

    /**
     * @brief Validate that parenthesis count matches
     * @param tokens The token stream
     * @throws SyntaxError if parentheses are unbalanced
     */
    void validateParentheses(const std::vector<Token>& tokens) const;

    /**
     * @brief Validate operator sequences (no consecutive binary operators)
     * @param tokens The token stream
     * @throws SyntaxError if operators are invalidly placed
     * @note This check applies in both unary-enabled and unary-disabled modes
     *       to catch cases like "1++2" which are invalid even with unary operators enabled
     */
    void validateOperators(const std::vector<Token>& tokens) const;

    /**
     * @brief Create a unary operation node from a token
     * @param op The operator token (must be + or -)
     * @param operand The operand expression
     * @return Unary operation node
     */
    std::unique_ptr<UnaryOpNode> createUnaryOp(Token op,
                                               std::unique_ptr<ASTNode> operand);

    bool enableUnaryOperators_;  ///< Enable unary + and - detection
};

} // namespace calc

#endif // CALC_CORE_SHUNTING_YARD_PARSER_H
