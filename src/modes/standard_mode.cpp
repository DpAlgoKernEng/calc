/**
 * @file standard_mode.cpp
 * @brief Standard calculator mode implementation
 */

#include "calc/modes/standard_mode.h"
#include "calc/core/tokenizer.h"
#include "calc/core/shunting_yard_parser.h"
#include "calc/core/recursive_descent_parser.h"

namespace calc {

StandardMode::StandardMode(int precision)
    : context_(precision)
    , useRecursiveDescentParser_(false) {
    // Register basic math functions (sqrt, abs, etc.)
    // Standard mode uses minimal functions, but we can include some basic ones
    MathFunctions::registerBuiltInFunctions(context_);
}

std::string StandardMode::getName() const {
    return "standard";
}

std::string StandardMode::getDescription() const {
    return "Standard calculator mode: +, -, *, /, ^ with parentheses";
}

EvaluationResult StandardMode::evaluate(const std::string& expression) {
    // Step 1: Tokenize the expression
    Tokenizer tokenizer(expression);
    std::vector<Token> tokens;
    try {
        tokens = tokenizer.tokenize();
    } catch (const CalculatorException& e) {
        return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::PARSE_ERROR, e.what(), 0);
    }

    // Check for empty token list
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EOF_TOKEN)) {
        return EvaluationResult(ErrorCode::PARSE_ERROR, "Empty expression", 0);
    }

    // Step 2: Parse tokens into AST
    auto parser = createParser();
    std::unique_ptr<ASTNode> ast;
    try {
        ast = parser->parse(tokens);
        if (!ast) {
            return EvaluationResult(ErrorCode::PARSE_ERROR, "Failed to parse expression", 0);
        }
    } catch (const CalculatorException& e) {
        return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::PARSE_ERROR, e.what(), 0);
    }

    // Step 3: Evaluate the AST
    try {
        return evaluator_.evaluate(ast.get(), context_);
    } catch (const CalculatorException& e) {
        return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::EVALUATION_ERROR, e.what(), 0);
    }
}

EvaluationContext& StandardMode::getContext() {
    return context_;
}

const EvaluationContext& StandardMode::getContext() const {
    return context_;
}

void StandardMode::setPrecision(int precision) {
    context_.setPrecision(precision);
}

int StandardMode::getPrecision() const {
    return context_.getPrecision();
}

std::string StandardMode::getParserType() const {
    return useRecursiveDescentParser_ ? "recursive-descent" : "shunting-yard";
}

void StandardMode::setParserType(bool useRecursiveDescent) {
    useRecursiveDescentParser_ = useRecursiveDescent;
}

std::unique_ptr<Parser> StandardMode::createParser() const {
    if (useRecursiveDescentParser_) {
        return std::make_unique<RecursiveDescentParser>();
    }
    return std::make_unique<ShuntingYardParser>();
}

} // namespace calc
