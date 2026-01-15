/**
 * @file binary_op_node.cpp
 * @brief BinaryOpNode implementation
 */

#include "calc/core/ast.h"

namespace calc {

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

} // namespace calc
