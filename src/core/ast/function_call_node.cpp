/**
 * @file function_call_node.cpp
 * @brief FunctionCallNode implementation
 */

#include "calc/core/ast.h"
#include <sstream>

namespace calc {

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
