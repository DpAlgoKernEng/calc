/**
 * @file programmer_mode.cpp
 * @brief Programmer calculator mode implementation
 */

#include "calc/modes/programmer_mode.h"
#include "calc/core/shunting_yard_parser.h"
#include "calc/core/tokenizer.h"

namespace calc {

ProgrammerMode::ProgrammerMode(int precision)
    : context_(precision)
    , evaluator_()
    , displayBase_(10)
    , precision_(precision)
{
    // Configure operator semantics for Programmer mode
    // '^' is bitwise XOR in Programmer mode
    context_.setOperatorSemantics("^", OperatorSemantics::BITWISE_XOR);

    // Note: We use Shunting Yard parser by default
}

//=============================================================================
// Mode Implementation
//=============================================================================

std::string ProgrammerMode::getName() const {
    return "programmer";
}

std::string ProgrammerMode::getDescription() const {
    return "Programmer mode with base conversions and bitwise operations";
}

EvaluationResult ProgrammerMode::evaluate(const std::string& expression) {
    if (expression.empty()) {
        return EvaluationResult(ErrorCode::INVALID_SYNTAX, "Empty expression", 0);
    }

    // Create parser
    auto parser = createParser();

    // Create tokenizer
    Tokenizer tokenizer(expression);

    // Tokenize the input
    std::vector<Token> tokens = tokenizer.tokenize();

    // Parse tokens into AST
    std::unique_ptr<ASTNode> ast = parser->parse(tokens);

    // Evaluate the AST
    EvaluationResult result = evaluator_.evaluate(ast.get(), context_);

    return result;
}

EvaluationContext& ProgrammerMode::getContext() {
    return context_;
}

const EvaluationContext& ProgrammerMode::getContext() const {
    return context_;
}

//=============================================================================
// Display Base Management
//=============================================================================

void ProgrammerMode::setDisplayBase(int base) {
    if (!isValidBase(base)) {
        throw std::invalid_argument("Invalid display base: " + std::to_string(base) + ". Must be 2, 8, 10, or 16.");
    }
    displayBase_ = base;
}

int ProgrammerMode::getDisplayBase() const {
    return displayBase_;
}

std::vector<int> ProgrammerMode::getSupportedBases() const {
    return {2, 8, 10, 16};
}

std::string ProgrammerMode::formatResult(long long value) const {
    // Map base integer to NumberBase enum
    NumberBase base;
    switch (displayBase_) {
        case 2:
            base = NumberBase::BINARY;
            break;
        case 8:
            base = NumberBase::OCTAL;
            break;
        case 16:
            base = NumberBase::HEXADECIMAL;
            break;
        case 10:
        default:
            base = NumberBase::DECIMAL;
            break;
    }
    return Converter::format(value, base);
}

//=============================================================================
// Precision Management
//=============================================================================

void ProgrammerMode::setPrecision(int precision) {
    if (precision < 0) {
        throw std::invalid_argument("Precision must be non-negative");
    }
    precision_ = precision;
    context_.setPrecision(precision);
}

int ProgrammerMode::getPrecision() const {
    return precision_;
}

//=============================================================================
// Private Methods
//=============================================================================

std::unique_ptr<Parser> ProgrammerMode::createParser() const {
    // Use Shunting Yard parser for programmer mode
    return std::make_unique<ShuntingYardParser>();
}

bool ProgrammerMode::isValidBase(int base) {
    return base == 2 || base == 8 || base == 10 || base == 16;
}

} // namespace calc
