/**
 * @file literal_node.cpp
 * @brief LiteralNode implementation
 */

#include "calc/core/ast.h"
#include <sstream>

namespace calc {

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

} // namespace calc
