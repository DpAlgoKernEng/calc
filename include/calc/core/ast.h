/**
 * @file ast.h
 * @brief AST node hierarchy and visitor pattern
 */

#ifndef CALC_CORE_AST_H
#define CALC_CORE_AST_H

#include "calc/core/token.h"
#include "calc/utils/error.h"
#include <memory>
#include <vector>
#include <string>

namespace calc {

// Forward declaration
class ASTVisitor;

/**
 * @brief Base class for all AST nodes
 *
 * Implements the Visitor pattern for tree traversal and evaluation.
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;

    /**
     * @brief Create a deep copy of this node
     * @return Unique pointer to the cloned node
     */
    virtual std::unique_ptr<ASTNode> clone() const = 0;

    /**
     * @brief Accept a visitor for traversal
     * @param visitor The visitor to accept
     */
    virtual void accept(ASTVisitor& visitor) = 0;

    /**
     * @brief Get the string representation of this node (for debugging)
     */
    virtual std::string toString() const = 0;
};

/**
 * @brief Represents a numeric literal
 */
class LiteralNode : public ASTNode {
public:
    /**
     * @brief Construct a literal node
     * @param value The numeric value
     */
    explicit LiteralNode(double value);

    /**
     * @brief Get the literal value
     */
    double getValue() const noexcept { return value_; }

    // ASTNode implementation
    std::unique_ptr<ASTNode> clone() const override;
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    double value_;
};

/**
 * @brief Represents a binary operation (e.g., a + b, x * y)
 */
class BinaryOpNode : public ASTNode {
public:
    /**
     * @brief Construct a binary operation node
     * @param left The left operand
     * @param op The operator token
     * @param right The right operand
     */
    BinaryOpNode(std::unique_ptr<ASTNode> left, Token op, std::unique_ptr<ASTNode> right);

    /**
     * @brief Get the left operand
     */
    ASTNode* getLeft() const noexcept { return left_.get(); }

    /**
     * @brief Get the operator
     */
    const Token& getOperator() const noexcept { return op_; }

    /**
     * @brief Get the right operand
     */
    ASTNode* getRight() const noexcept { return right_.get(); }

    /**
     * @brief Release left operand (for transfer of ownership)
     */
    std::unique_ptr<ASTNode> releaseLeft();

    /**
     * @brief Release right operand (for transfer of ownership)
     */
    std::unique_ptr<ASTNode> releaseRight();

    // ASTNode implementation
    std::unique_ptr<ASTNode> clone() const override;
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::unique_ptr<ASTNode> left_;
    Token op_;
    std::unique_ptr<ASTNode> right_;
};

/**
 * @brief Represents a unary operation (e.g., -x, +x)
 */
class UnaryOpNode : public ASTNode {
public:
    /**
     * @brief Construct a unary operation node
     * @param op The operator token
     * @param operand The operand
     */
    UnaryOpNode(Token op, std::unique_ptr<ASTNode> operand);

    /**
     * @brief Get the operator
     */
    const Token& getOperator() const noexcept { return op_; }

    /**
     * @brief Get the operand
     */
    ASTNode* getOperand() const noexcept { return operand_.get(); }

    /**
     * @brief Release operand (for transfer of ownership)
     */
    std::unique_ptr<ASTNode> releaseOperand();

    // ASTNode implementation
    std::unique_ptr<ASTNode> clone() const override;
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    Token op_;
    std::unique_ptr<ASTNode> operand_;
};

/**
 * @brief Represents a function call (e.g., sin(x), max(a, b, c))
 */
class FunctionCallNode : public ASTNode {
public:
    /**
     * @brief Construct a function call node
     * @param name The function name
     * @param position The position of function name in input
     * @param args Vector of argument nodes
     */
    FunctionCallNode(const std::string& name, size_t position,
                     std::vector<std::unique_ptr<ASTNode>> args);

    /**
     * @brief Get the function name
     */
    const std::string& getName() const noexcept { return name_; }

    /**
     * @brief Get the position of function name in input
     */
    size_t getPosition() const noexcept { return position_; }

    /**
     * @brief Get the number of arguments
     */
    size_t getArgumentCount() const noexcept { return args_.size(); }

    /**
     * @brief Get an argument by index
     * @param index The argument index
     * @return Pointer to the argument node
     */
    ASTNode* getArgument(size_t index) const;

    /**
     * @brief Get all arguments
     */
    const std::vector<std::unique_ptr<ASTNode>>& getArguments() const noexcept { return args_; }

    /**
     * @brief Release all arguments (for transfer of ownership)
     */
    std::vector<std::unique_ptr<ASTNode>> releaseArguments();

    // ASTNode implementation
    std::unique_ptr<ASTNode> clone() const override;
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string name_;
    size_t position_;
    std::vector<std::unique_ptr<ASTNode>> args_;
};

/**
 * @brief Abstract visitor interface for AST traversal
 *
 * This is defined now for Phase 2, though full implementation
 * will come in Phase 3 (Evaluator).
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(UnaryOpNode& node) = 0;
    virtual void visit(FunctionCallNode& node) = 0;
};

} // namespace calc

#endif // CALC_CORE_AST_H
