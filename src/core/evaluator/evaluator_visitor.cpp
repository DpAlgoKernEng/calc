/**
 * @file evaluator_visitor.cpp
 * @brief Implementation of visitor-based expression evaluator
 */

#include "calc/core/evaluator.h"
#include <cmath>

namespace calc {

//=============================================================================
// EvaluatorVisitor Implementation
//=============================================================================

EvaluatorVisitor::EvaluatorVisitor()
    : result_(0.0), context_(nullptr)
{
}

EvaluationResult EvaluatorVisitor::evaluate(
    const ASTNode* node,
    EvaluationContext& context)
{
    if (node == nullptr) {
        return EvaluationResult(ErrorCode::EVALUATION_ERROR,
            "Cannot evaluate null node");
    }

    // Save and set current context
    EvaluationContext* prevContext = context_;
    context_ = &context;

    // Reset result state
    result_ = EvaluationResult(0.0);

    // Accept visitor to trigger evaluation
    const_cast<ASTNode*>(node)->accept(*this);

    // Restore previous context
    context_ = prevContext;

    return result_;
}

void EvaluatorVisitor::visit(LiteralNode& node) {
    result_ = EvaluationResult(node.getValue());
}

void EvaluatorVisitor::visit(BinaryOpNode& node) {
    // Evaluate left operand
    EvaluationResult leftResult = evaluate(node.getLeft(), *context_);
    if (leftResult.isError()) {
        result_ = leftResult;
        return;
    }

    // Evaluate right operand
    EvaluationResult rightResult = evaluate(node.getRight(), *context_);
    if (rightResult.isError()) {
        result_ = rightResult;
        return;
    }

    // Perform binary operation
    result_ = evaluateBinaryOp(leftResult.getValue(), node.getOperator(),
                               rightResult.getValue());
}

void EvaluatorVisitor::visit(UnaryOpNode& node) {
    // Evaluate operand
    EvaluationResult operandResult = evaluate(node.getOperand(), *context_);
    if (operandResult.isError()) {
        result_ = operandResult;
        return;
    }

    // Perform unary operation
    result_ = evaluateUnaryOp(node.getOperator(), operandResult.getValue());
}

void EvaluatorVisitor::visit(FunctionCallNode& node) {
    // Evaluate all arguments
    std::vector<double> args;
    args.reserve(node.getArgumentCount());

    for (size_t i = 0; i < node.getArgumentCount(); ++i) {
        EvaluationResult argResult = evaluate(node.getArgument(i), *context_);
        if (argResult.isError()) {
            result_ = argResult;
            return;
        }
        args.push_back(argResult.getValue());
    }

    // Call function through context
    result_ = context_->callFunction(node.getName(), args);

    // If the function failed, add position information
    if (result_.isError() && result_.getErrorPosition() == 0) {
        // Create a new result with the function's position
        result_ = EvaluationResult(
            result_.getErrorCode(),
            result_.getErrorMessage(),
            node.getPosition());
    }
}

EvaluationResult EvaluatorVisitor::getResult() const {
    return result_;
}

void EvaluatorVisitor::reset() {
    result_ = EvaluationResult(0.0);
    context_ = nullptr;
}

EvaluationResult EvaluatorVisitor::evaluateBinaryOp(
    double left,
    const Token& op,
    double right)
{
    const std::string& opStr = op.value;
    size_t position = op.position;

    try {
        // Check for division by zero
        if ((opStr == "/" || opStr == "%") && approxEqual(right, 0.0)) {
            return EvaluationResult(ErrorCode::DIVISION_BY_ZERO,
                "Division by zero", position);
        }

        // Perform operation
        double result = 0.0;

        if (opStr == "+") {
            result = left + right;
        } else if (opStr == "-") {
            result = left - right;
        } else if (opStr == "*") {
            result = left * right;
        } else if (opStr == "/") {
            result = left / right;
        } else if (opStr == "%") {
            result = std::fmod(left, right);
        } else if (opStr == "^") {
            // Check operator semantics from context
            OperatorSemantics semantics = context_->getOperatorSemantics("^");
            if (semantics == OperatorSemantics::BITWISE_XOR) {
                // Programmer mode: Bitwise XOR
                result = static_cast<double>(
                    static_cast<long long>(left) ^ static_cast<long long>(right));
            } else {
                // Standard/Scientific mode: Power operation (exponentiation)
                result = std::pow(left, right);
            }
        } else if (opStr == "&") {
            // Bitwise AND
            result = static_cast<double>(static_cast<long long>(left) & static_cast<long long>(right));
        } else if (opStr == "|") {
            // Bitwise OR
            result = static_cast<double>(static_cast<long long>(left) | static_cast<long long>(right));
        } else if (opStr == "<<") {
            // Left shift
            result = static_cast<double>(static_cast<long long>(left) << static_cast<long long>(right));
        } else if (opStr == ">>") {
            // Right shift
            result = static_cast<double>(static_cast<long long>(left) >> static_cast<long long>(right));
        } else {
            return EvaluationResult(ErrorCode::EVALUATION_ERROR,
                "Unknown binary operator: " + opStr, position);
        }

        // Check for overflow/underflow
        if (std::isinf(result) && !std::isinf(left) && !std::isinf(right)) {
            return EvaluationResult(ErrorCode::NUMERIC_OVERFLOW,
                "Numeric overflow", position);
        }

        if (result == 0.0 && !approxEqual(left, 0.0) && !approxEqual(right, 0.0)) {
            // Possible underflow - not necessarily an error
        }

        // Check for NaN
        if (std::isnan(result)) {
            return EvaluationResult(ErrorCode::DOMAIN_ERROR,
                "Result is NaN (Not a Number) - possible domain error", position);
        }

        return EvaluationResult(result);

    } catch (const std::domain_error& e) {
        return EvaluationResult(ErrorCode::DOMAIN_ERROR,
            e.what(), position);
    } catch (const std::overflow_error& e) {
        return EvaluationResult(ErrorCode::NUMERIC_OVERFLOW,
            e.what(), position);
    } catch (const std::underflow_error& e) {
        return EvaluationResult(ErrorCode::NUMERIC_UNDERFLOW,
            e.what(), position);
    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::EVALUATION_ERROR,
            e.what(), position);
    }
}

EvaluationResult EvaluatorVisitor::evaluateUnaryOp(
    const Token& op,
    double operand)
{
    const std::string& opStr = op.value;
    size_t position = op.position;

    try {
        double result = 0.0;

        if (opStr == "+") {
            result = operand;
        } else if (opStr == "-") {
            result = -operand;
        } else if (opStr == "~" || opStr == "u~") {
            // Bitwise NOT (for integer-like values)
            // Tokenizer marks ~ as u~ for unary operator
            result = ~static_cast<long long>(operand);
        } else {
            return EvaluationResult(ErrorCode::EVALUATION_ERROR,
                "Unknown unary operator: " + opStr, position);
        }

        // Check for overflow
        if (std::isinf(result) && !std::isinf(operand)) {
            return EvaluationResult(ErrorCode::NUMERIC_OVERFLOW,
                "Numeric overflow", position);
        }

        return EvaluationResult(result);

    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::EVALUATION_ERROR,
            e.what(), position);
    }
}

bool EvaluatorVisitor::approxEqual(double a, double b, double epsilon) {
    // Handle NaN
    if (std::isnan(a) || std::isnan(b)) {
        return false;
    }

    // Handle infinity
    if (std::isinf(a) && std::isinf(b)) {
        return a == b; // Both same sign infinity
    }

    // Relative and absolute comparison
    double diff = std::abs(a - b);
    if (diff < epsilon) {
        return true;
    }

    // Relative comparison for larger numbers
    double maxVal = std::max(std::abs(a), std::abs(b));
    if (diff < epsilon * maxVal) {
        return true;
    }

    return false;
}

} // namespace calc
