/**
 * @file ast_node.cpp
 * @brief AST node implementations
 */

#include "calc/core/ast.h"
#include <sstream>

namespace calc {

// ============================================================================
// LiteralNode Implementation
// ============================================================================

LiteralNode::LiteralNode(double value) : value_(value) {}

std::unique_ptr<ASTNode> LiteralNode::clone() const {
    return std::make_unique<LiteralNode>(value_);
}

void LiteralNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string LiteralNode::toString() const {
    std::ostringstream oss;
    oss << value_;
    return oss.str();
}

// ============================================================================
// BinaryOpNode Implementation
// ============================================================================

BinaryOpNode::BinaryOpNode(std::unique_ptr<ASTNode> left, Token op, std::unique_ptr<ASTNode> right)
    : left_(std::move(left)), op_(op), right_(std::move(right)) {}

std::unique_ptr<ASTNode> BinaryOpNode::clone() const {
    return std::make_unique<BinaryOpNode>(left_->clone(), op_, right_->clone());
}

void BinaryOpNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string BinaryOpNode::toString() const {
    return "(" + left_->toString() + " " + op_.value + " " + right_->toString() + ")";
}

std::unique_ptr<ASTNode> BinaryOpNode::releaseLeft() {
    return std::move(left_);
}

std::unique_ptr<ASTNode> BinaryOpNode::releaseRight() {
    return std::move(right_);
}

// ============================================================================
// UnaryOpNode Implementation
// ============================================================================

UnaryOpNode::UnaryOpNode(Token op, std::unique_ptr<ASTNode> operand)
    : op_(op), operand_(std::move(operand)) {}

std::unique_ptr<ASTNode> UnaryOpNode::clone() const {
    return std::make_unique<UnaryOpNode>(op_, operand_->clone());
}

void UnaryOpNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string UnaryOpNode::toString() const {
    return "(" + op_.value + operand_->toString() + ")";
}

std::unique_ptr<ASTNode> UnaryOpNode::releaseOperand() {
    return std::move(operand_);
}

// ============================================================================
// FunctionCallNode Implementation
// ============================================================================

FunctionCallNode::FunctionCallNode(const std::string& name, size_t position,
                                   std::vector<std::unique_ptr<ASTNode>> args)
    : name_(name), position_(position), args_(std::move(args)) {}

ASTNode* FunctionCallNode::getArgument(size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("FunctionCallNode::getArgument: index out of range");
    }
    return args_[index].get();
}

std::unique_ptr<ASTNode> FunctionCallNode::clone() const {
    std::vector<std::unique_ptr<ASTNode>> clonedArgs;
    clonedArgs.reserve(args_.size());
    for (const auto& arg : args_) {
        clonedArgs.push_back(arg->clone());
    }
    return std::make_unique<FunctionCallNode>(name_, position_, std::move(clonedArgs));
}

void FunctionCallNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string FunctionCallNode::toString() const {
    std::ostringstream oss;
    oss << name_ << "(";
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << args_[i]->toString();
    }
    oss << ")";
    return oss.str();
}

std::vector<std::unique_ptr<ASTNode>> FunctionCallNode::releaseArguments() {
    return std::move(args_);
}

} // namespace calc
