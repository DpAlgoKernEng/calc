/**
 * @file unary_op_node.cpp
 * @brief UnaryOpNode implementation
 */

#include "calc/core/ast.h"

namespace calc {

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

} // namespace calc
