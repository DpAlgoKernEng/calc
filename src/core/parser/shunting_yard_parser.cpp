/**
 * @file shunting_yard_parser.cpp
 * @brief Shunting-yard algorithm parser implementation
 */

#include "calc/core/shunting_yard_parser.h"
#include "calc/math/converter.h"

namespace calc {

// ============================================================================
// ShuntingYardParser Implementation
// ============================================================================

ShuntingYardParser::ShuntingYardParser(bool enableUnaryOperators)
    : enableUnaryOperators_(enableUnaryOperators) {}

std::unique_ptr<ASTNode> ShuntingYardParser::parse(const std::vector<Token>& tokens) {
    // Validate parentheses first
    validateParentheses(tokens);

    // Validate operators (no consecutive binary operators)
    validateOperators(tokens);

    // Convert infix to postfix (RPN)
    std::vector<Token> postfixTokens = toPostfix(tokens);

    // Build AST from postfix
    return buildAST(postfixTokens);
}

bool ShuntingYardParser::isUnaryOperator(const std::vector<Token>& tokens, size_t index) const {
    const Token& token = tokens[index];
    if (token.type != TokenType::OPERATOR) {
        return false;
    }

    // +, -, ~ can be unary
    if (token.value != "+" && token.value != "-" && token.value != "~") {
        return false;
    }

    // When unary operators are disabled, they can never be unary
    if (!enableUnaryOperators_) {
        return false;
    }

    // An operator is unary if:
    // 1. It's the first token
    // 2. The previous token is an operator
    // 3. The previous token is a left parenthesis
    // 4. The previous token is a comma (function argument separator)
    if (index == 0) {
        return true;
    }

    const Token& prev = tokens[index - 1];
    return prev.type == TokenType::OPERATOR ||
           prev.type == TokenType::LPAREN ||
           prev.type == TokenType::COMMA;
}

int ShuntingYardParser::getPrecedence(const Token& op) const {
    if (op.type != TokenType::OPERATOR) {
        throw CalculatorException(ErrorCode::PARSE_ERROR, "Cannot get precedence of non-operator token", op.position);
    }

    // Unary operators have the same precedence as exponentiation
    // This allows exponentiation to bind tighter than unary minus
    // e.g., -2^3 = -(2^3), not (-2)^3
    if (op.value == "u+" || op.value == "u-") {
        return 3;  // Same precedence as exponentiation
    }

    // Binary operators (lower number = higher precedence)
    if (op.value == "^") {
        return 3;  // Exponentiation (right-associative)
    } else if (op.value == "<<" || op.value == ">>") {
        return 3;  // Shift operators (same precedence as power)
    } else if (op.value == "&") {
        return 2;  // Bitwise AND (same precedence as mult/div)
    } else if (op.value == "*" || op.value == "/" || op.value == "%") {
        return 2;  // Multiplication, division, modulo
    } else if (op.value == "|") {
        return 1;  // Bitwise OR (same precedence as add/sub)
    } else if (op.value == "+" || op.value == "-") {
        return 1;  // Addition, subtraction
    }

    throw CalculatorException(ErrorCode::PARSE_ERROR, "Unknown operator: " + op.value, op.position);
}

bool ShuntingYardParser::isRightAssociative(const Token& op) const {
    if (op.type != TokenType::OPERATOR) {
        return false;
    }

    // Only exponentiation is right-associative
    // Unary operators are prefix operators, not right-associative
    return op.value == "^";
}

void ShuntingYardParser::validateParentheses(const std::vector<Token>& tokens) const {
    int depth = 0;
    for (const auto& token : tokens) {
        if (token.type == TokenType::LPAREN) {
            ++depth;
        } else if (token.type == TokenType::RPAREN) {
            --depth;
            if (depth < 0) {
                throw SyntaxError("Unbalanced parentheses: too many closing ')'", token.position);
            }
        }
    }

    if (depth > 0) {
        throw SyntaxError("Unbalanced parentheses: missing closing ')'", tokens.back().position);
    }
}

void ShuntingYardParser::validateOperators(const std::vector<Token>& tokens) const {
    // Check for consecutive operators where NEITHER can be unary
    // Valid: 5+-3 (binary +, unary -), --5 (both unary), +-5 (both unary)
    // Invalid: 1**2, 1*/2, 1+*2 (neither * can be unary), 1++2 (ambiguous)
    for (size_t i = 1; i < tokens.size(); ++i) {
        const Token& current = tokens[i];
        const Token& prev = tokens[i - 1];

        // Only check when we have two consecutive operators
        if (current.type != TokenType::OPERATOR || prev.type != TokenType::OPERATOR) {
            continue;
        }

        // Check if prev is at index 0 (both operators are at start)
        if (i == 1) {
            // Both are at start, so prev must be unary
            // Current is also in unary position
            // This is valid for +-5, --5, ++5
            continue;
        }

        // Check if current can be unary
        bool currentCanBeUnary = (current.value == "+" || current.value == "-");

        // Check if prev can be unary
        bool prevCanBeUnary = (prev.value == "+" || prev.value == "-");

        // If neither operator can be unary, it's an error
        if (!currentCanBeUnary && !prevCanBeUnary) {
            throw SyntaxError("Consecutive operators are not allowed", current.position);
        }

        // If prev is at a position where it CANNOT be unary (after NUMBER),
        // and current is also + or -, then current as unary is ambiguous
        // Example: 1++2 - prev is binary, current would be unary (+2)
        // This is rejected for clarity, even though 1+(+2) is mathematically valid
        // But 5+-3 is accepted because unary - is clearer
        // Also, +5 + +3 is accepted because the middle + is not binary
        // Check prevPrev only if available (i >= 2)
        if (i >= 2) {
            const Token& prevPrev = tokens[i - 2];
            // If prevPrev is a number, prev was in binary position (after operand)
            // We want to reject cases like 1++2 where prev is binary + followed by unary +
            // But accept +5 + +3 where the middle + is binary and current is unary
            // The difference: in 1++2, prevPrev (1) is an operand
            // In +5 + +3, prevPrev (5) is also an operand, BUT the middle + is between operands
            // Actually wait, we need to check if there's an operand on BOTH sides of prev
            if (prevPrev.type == TokenType::NUMBER && current.value == "+") {
                // Check if there's a NUMBER (or similar operand) after prevPrev to see if prev is binary
                // For 1++2: prevPrev=1 (NUM), prev=+, current=+ - prev is NOT binary
                // For +5 + +3: prevPrev=5 (NUM), prev=+, current=+ - prev IS binary
                // We want to reject 1++2 but accept +5 + +3
                // The key is: is prev actually in a binary position?
                // prev is binary if it has an operand before it
                // For 1++2: prev has NUM before (1) and + after - middle is binary!
                // For +5 + +3: prev has NUM before (5) and + after - middle is binary!
                // Hmm, both seem to have same structure...

                // Actually, the difference is WHERE the consecutive operators appear:
                // 1++2: the ++ appears immediately after a number
                // +5 + +3: the ++ appears after a binary + operator
                // Wait no, let me reconsider...

                // For +5 + +3 at index 3 (second +):
                //   current = +, prev = +, prevPrev = 5
                //   The prevPrev (5) is a NUMBER
                //   The prev (+) is after 5, so it's binary
                //   The current (+) is after prev (+), so it's unary
                //   This is +5 + (+3) = valid

                // For 1++2 at index 2 (second +):
                //   current = +, prev = +, prevPrev = 1
                //   The prevPrev (1) is a NUMBER
                //   The prev (+) is after 1, so it's binary
                //   The current (+) is after prev (+), so it's unary
                //   This is 1 + (+2) = but ambiguous because the + after 1 looks like it should be binary

                // The ambiguity in 1++2 is that "1++" visually looks like an increment operator
                // We want to reject this specific case but allow +5 + +3
                // The key: is prevPrev a NUMBER AND is prev the first token OR is prevPrev also an operator?
                // For 1++2: prevPrev = NUM, prevPrevPrev doesn't exist (index -1)
                // For +5 + +3: prevPrev = NUM, prevPrevPrev = OPERATOR (the first +)

                // Check if prevPrev itself is preceded by a NUMBER (or nothing)
                if (i >= 3) {
                    const Token& prevPrevPrev = tokens[i - 3];
                    // If prevPrevPrev is also a NUMBER or doesn't exist, it's like 1++2
                    // If prevPrevPrev is an OPERATOR, it's like +5 + +3
                    if (prevPrevPrev.type == TokenType::NUMBER || i == 2) {
                        throw SyntaxError("Consecutive operators are not allowed: ambiguous unary '+'", current.position);
                    }
                } else if (i == 2) {
                    // At index 2 with no prevPrevPrev, this is 1++2 style
                    throw SyntaxError("Consecutive operators are not allowed: ambiguous unary '+'", current.position);
                }
            }
        }
    }
}

std::vector<Token> ShuntingYardParser::toPostfix(const std::vector<Token>& tokens) {
    std::vector<Token> output;
    // Use vector instead of stack to allow index-based access for function arg count tracking
    std::vector<Token> operatorStack;
    const Token* top = nullptr;  // Declare outside switch for use in OPERATOR and EOF cases

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens[i];

        switch (token.type) {
            case TokenType::NUMBER:
                output.push_back(token);
                break;

            case TokenType::FUNCTION: {
                // Push function onto stack with argCount initialized to 0
                Token funcToken(token);
                funcToken.argCount = 0;
                operatorStack.push_back(funcToken);
                break;
            }

            case TokenType::COMMA:
                // Pop operators until we hit a left parenthesis
                while (!operatorStack.empty() && operatorStack.back().type != TokenType::LPAREN) {
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                if (operatorStack.empty()) {
                    throw SyntaxError("Misplaced comma in function arguments", token.position);
                }
                // Increment argument count for the function on top of stack
                // Find the function by searching backwards
                for (int j = static_cast<int>(operatorStack.size()) - 1; j >= 0; --j) {
                    if (operatorStack[static_cast<size_t>(j)].type == TokenType::FUNCTION) {
                        operatorStack[static_cast<size_t>(j)].argCount++;
                        break;
                    }
                }
                break;

            case TokenType::OPERATOR:
                // Check if this should be treated as a unary operator
                if (isUnaryOperator(tokens, i)) {
                    Token unaryOp(token);
                    unaryOp.value = "u" + token.value;  // Mark as unary (e.g., "u+", "u-")
                    operatorStack.push_back(unaryOp);
                } else {
                    // Binary operator
                    // First, pop any FUNCTION tokens to output (they behave like operands)
                    // This handles cases like "PI/2" where PI is a zero-arg function
                    while (!operatorStack.empty() &&
                           operatorStack.back().type == TokenType::FUNCTION) {
                        output.push_back(operatorStack.back());
                        operatorStack.pop_back();
                    }
                    // Then pop operators with higher or equal precedence
                    while (!operatorStack.empty() &&
                           operatorStack.back().type == TokenType::OPERATOR) {
                        top = &operatorStack.back();

                        if ((!isRightAssociative(token) && getPrecedence(token) <= getPrecedence(*top)) ||
                            (isRightAssociative(token) && getPrecedence(token) < getPrecedence(*top))) {
                            output.push_back(*top);
                            operatorStack.pop_back();
                        } else {
                            break;
                        }
                    }
                    operatorStack.push_back(token);
                }
                break;

            case TokenType::LPAREN:
                operatorStack.push_back(token);
                break;

            case TokenType::RPAREN:
                // Pop operators until we hit a left parenthesis
                while (!operatorStack.empty() && operatorStack.back().type != TokenType::LPAREN) {
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }

                if (operatorStack.empty()) {
                    throw SyntaxError("Unbalanced parentheses: missing opening '('", token.position);
                }

                // Pop left parenthesis
                operatorStack.pop_back();

                // If top of stack is a function, pop it to output with argCount
                if (!operatorStack.empty() && operatorStack.back().type == TokenType::FUNCTION) {
                    // argCount represents the number of commas, so actual args = argCount + 1
                    operatorStack.back().argCount++;
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                break;

            case TokenType::EOF_TOKEN:
                // End of input: pop remaining operators
                while (!operatorStack.empty()) {
                    top = &operatorStack.back();
                    if (top->type == TokenType::LPAREN) {
                        throw SyntaxError("Unbalanced parentheses: missing closing ')'", top->position);
                    }
                    output.push_back(*top);
                    operatorStack.pop_back();
                }
                break;

            case TokenType::UNKNOWN:
                throw SyntaxError("Unknown token in input", token.position);
        }
    }

    return output;
}

std::unique_ptr<ASTNode> ShuntingYardParser::buildAST(const std::vector<Token>& postfixTokens) {
    std::stack<std::unique_ptr<ASTNode>> operandStack;

    for (const auto& token : postfixTokens) {
        switch (token.type) {
            case TokenType::NUMBER: {
                double value;
                // Handle different number bases
                if (token.numberBase == NumberBase::BINARY) {
                    value = static_cast<double>(Converter::binaryToDecimal(token.value));
                } else if (token.numberBase == NumberBase::OCTAL) {
                    value = static_cast<double>(Converter::octalToDecimal(token.value));
                } else if (token.numberBase == NumberBase::HEXADECIMAL) {
                    value = static_cast<double>(Converter::hexToDecimal(token.value));
                } else {
                    // Default: decimal
                    value = std::stod(token.value);
                }
                operandStack.push(std::make_unique<LiteralNode>(value));
                break;
            }

            case TokenType::OPERATOR: {
                // Check if it's a unary operator
                if (token.value == "u+" || token.value == "u-" || token.value == "u~") {
                    // Unary operator
                    if (operandStack.empty()) {
                        throw CalculatorException(ErrorCode::UNEXPECTED_TOKEN, "Missing operand for unary operator", token.position);
                    }
                    auto operand = std::move(const_cast<std::unique_ptr<ASTNode>&>(operandStack.top()));
                    operandStack.pop();
                    Token unaryOp(token);
                    unaryOp.value = token.value.substr(1);  // Remove 'u' prefix
                    operandStack.push(createUnaryOp(unaryOp, std::move(operand)));
                } else {
                    // Binary operator
                    applyOperator(token, operandStack);
                }
                break;
            }

            case TokenType::FUNCTION: {
                // Pop operands for function arguments
                // Use the argCount stored in the token
                // Use the tracked argument count (stored during toPostfix)
                size_t argCount = token.argCount;

                // For zero-argument functions (like PI, E), the operand stack can be empty
                if (argCount > 0 && operandStack.empty()) {
                    throw SyntaxError("Missing arguments for function: " + token.value, token.position);
                }

                auto args = buildFunctionCall(token.value, token.position, argCount, operandStack);
                operandStack.push(std::move(args));
                break;
            }

            case TokenType::EOF_TOKEN:
                // Skip EOF token
                break;

            default:
                throw CalculatorException(ErrorCode::UNEXPECTED_TOKEN,
                    "Unexpected token in postfix expression: " + token.value, token.position);
        }
    }

    if (operandStack.empty()) {
        throw SyntaxError("Empty expression", 0);
    }

    if (operandStack.size() > 1) {
        throw CalculatorException(ErrorCode::UNEXPECTED_TOKEN, "Too many operands in expression", 0);
    }

    return std::move(operandStack.top());
}

void ShuntingYardParser::applyOperator(const Token& op,
                                        std::stack<std::unique_ptr<ASTNode>>& operands) {
    if (operands.size() < 2) {
        throw CalculatorException(ErrorCode::UNEXPECTED_TOKEN,
            "Not enough operands for operator: " + op.value, op.position);
    }

    auto right = std::move(const_cast<std::unique_ptr<ASTNode>&>(operands.top()));
    operands.pop();

    auto left = std::move(const_cast<std::unique_ptr<ASTNode>&>(operands.top()));
    operands.pop();

    operands.push(std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right)));
}

std::unique_ptr<FunctionCallNode> ShuntingYardParser::buildFunctionCall(
    const std::string& name,
    size_t position,
    size_t operandCount,
    std::stack<std::unique_ptr<ASTNode>>& operands) {

    if (operands.size() < operandCount) {
        throw CalculatorException(ErrorCode::UNEXPECTED_TOKEN,
            "Not enough arguments for function: " + name, position);
    }

    std::vector<std::unique_ptr<ASTNode>> args;
    args.reserve(operandCount);

    // Pop arguments in reverse order (they were pushed in LIFO order)
    for (size_t i = 0; i < operandCount; ++i) {
        args.insert(args.begin(), std::move(const_cast<std::unique_ptr<ASTNode>&>(operands.top())));
        operands.pop();
    }

    return std::make_unique<FunctionCallNode>(name, position, std::move(args));
}

std::unique_ptr<UnaryOpNode> ShuntingYardParser::createUnaryOp(Token op,
                                                                 std::unique_ptr<ASTNode> operand) {
    return std::make_unique<UnaryOpNode>(op, std::move(operand));
}

} // namespace calc
