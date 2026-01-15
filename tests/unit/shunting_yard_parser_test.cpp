/**
 * @file shunting_yard_parser_test.cpp
 * @brief Unit tests for ShuntingYardParser class
 */

#include "calc/core/shunting_yard_parser.h"
#include "calc/core/tokenizer.h"
#include <gtest/gtest.h>

using namespace calc;

namespace {

// Helper function to parse a string expression using ShuntingYardParser
std::unique_ptr<ASTNode> parse(const std::string& expr) {
    Tokenizer tokenizer(expr);
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;
    return parser.parse(tokens);
}

// Helper function to get string representation of parsed expression
std::string toString(const std::string& expr) {
    auto ast = parse(expr);
    return ast->toString();
}

} // anonymous namespace

// ============================================================================
// ShuntingYardParser Construction Tests
// ============================================================================

TEST(ShuntingYardParserTest, DefaultConstruction) {
    ShuntingYardParser parser;
    EXPECT_TRUE(parser.isUnaryOperatorsEnabled());
}

TEST(ShuntingYardParserTest, ConstructionWithoutUnary) {
    ShuntingYardParser parser(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());
}

TEST(ShuntingYardParserTest, SetUnaryOperatorsEnabled) {
    ShuntingYardParser parser;
    parser.setUnaryOperatorsEnabled(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());

    parser.setUnaryOperatorsEnabled(true);
    EXPECT_TRUE(parser.isUnaryOperatorsEnabled());
}

TEST(ShuntingYardParserTest, GetName) {
    ShuntingYardParser parser;
    EXPECT_EQ(parser.getName(), "ShuntingYardParser");
}

// ============================================================================
// Single Value Tests
// ============================================================================

TEST(ShuntingYardParserTest, ParseSingleInteger) {
    auto ast = parse("42");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 42.0);
}

TEST(ShuntingYardParserTest, ParseSingleDecimal) {
    auto ast = parse("3.14");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 3.14);
}

TEST(ShuntingYardParserTest, ParseScientificNotation) {
    auto ast = parse("1.23e4");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 12300.0);
}

// ============================================================================
// Simple Operation Tests
// ============================================================================

TEST(ShuntingYardParserTest, ParseSimpleAddition) {
    auto ast = parse("1+2");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "+");

    auto* left = dynamic_cast<LiteralNode*>(binary->getLeft());
    auto* right = dynamic_cast<LiteralNode*>(binary->getRight());
    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);
    EXPECT_DOUBLE_EQ(left->getValue(), 1.0);
    EXPECT_DOUBLE_EQ(right->getValue(), 2.0);
}

TEST(ShuntingYardParserTest, ParseSimpleSubtraction) {
    auto ast = parse("5-3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, ParseSimpleMultiplication) {
    auto ast = parse("2*3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "*");
}

TEST(ShuntingYardParserTest, ParseSimpleDivision) {
    auto ast = parse("6/2");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "/");
}

TEST(ShuntingYardParserTest, ParseModulo) {
    auto ast = parse("7%3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "%");
}

TEST(ShuntingYardParserTest, ParseExponentiation) {
    auto ast = parse("2^3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "^");
}

// ============================================================================
// Operator Precedence Tests
// ============================================================================

TEST(ShuntingYardParserTest, PrecedenceMultiplicationBeforeAddition) {
    auto ast = parse("1+2*3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    // Right side should be multiplication
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "*");
}

TEST(ShuntingYardParserTest, PrecedenceExponentiationBeforeMultiplication) {
    auto ast = parse("2*3^2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "*");

    // Right side should be exponentiation
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "^");
}

TEST(ShuntingYardParserTest, PrecedenceComplex) {
    auto ast = parse("1+2*3-4/5");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, SamePrecedenceLeftAssociative) {
    auto ast = parse("8/4/2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "/");

    // Left side should be first division
    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "/");
}

// ============================================================================
// Parentheses Tests
// ============================================================================

TEST(ShuntingYardParserTest, ParenthesesOverridePrecedence) {
    auto ast = parse("(1+2)*3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "*");

    // Left side should be addition
    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "+");
}

TEST(ShuntingYardParserTest, NestedParentheses) {
    auto ast = parse("((1+2)*3)+4");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");
}

TEST(ShuntingYardParserTest, ParenthesesAroundSingleValue) {
    auto ast = parse("(42)");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 42.0);
}

// ============================================================================
// Unary Operator Tests
// ============================================================================

TEST(ShuntingYardParserTest, UnaryMinusAtStart) {
    auto ast = parse("-5");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");

    auto* operand = dynamic_cast<LiteralNode*>(unary->getOperand());
    ASSERT_NE(operand, nullptr);
    EXPECT_DOUBLE_EQ(operand->getValue(), 5.0);
}

TEST(ShuntingYardParserTest, UnaryPlusAtStart) {
    auto ast = parse("+5");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "+");
}

TEST(ShuntingYardParserTest, UnaryMinusInExpression) {
    auto ast = parse("-5+3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* left = dynamic_cast<UnaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, UnaryAfterOperator) {
    auto ast = parse("5+-3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* right = dynamic_cast<UnaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, UnaryAfterLeftParen) {
    auto ast = parse("(-5)");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, DoubleNegative) {
    auto ast = parse("--5");
    auto* outer = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->getOperator().value, "-");

    auto* inner = dynamic_cast<UnaryOpNode*>(outer->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, UnaryDisabled) {
    ShuntingYardParser parser(false);
    Tokenizer tokenizer("-5");
    auto tokens = tokenizer.tokenize();

    // Without unary support, -5 should throw an exception
    // because - would be treated as a binary operator with no left operand
    EXPECT_THROW(parser.parse(tokens), CalculatorException);
}

// ============================================================================
// Exponentiation Associativity Tests
// ============================================================================

TEST(ShuntingYardParserTest, ExponentiationRightAssociative) {
    auto ast = parse("2^3^4");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "^");

    // Right side should be another exponentiation (right-associative)
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "^");
}

TEST(ShuntingYardParserTest, ExponentiationWithUnary) {
    auto ast = parse("-2^3");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");

    auto* inner = dynamic_cast<BinaryOpNode*>(unary->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "^");
}

// ============================================================================
// Function Call Tests
// ============================================================================

TEST(ShuntingYardParserTest, SingleArgFunction) {
    auto ast = parse("sin(0)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
    EXPECT_EQ(func->getArgumentCount(), 1);
}

TEST(ShuntingYardParserTest, FunctionWithExpression) {
    auto ast = parse("sin(90+180)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
    EXPECT_EQ(func->getArgumentCount(), 1);

    auto* arg = dynamic_cast<BinaryOpNode*>(func->getArgument(0));
    ASSERT_NE(arg, nullptr);
    EXPECT_EQ(arg->getOperator().value, "+");
}

TEST(ShuntingYardParserTest, FunctionWithParentheses) {
    auto ast = parse("sin((90+180))");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
}

// ============================================================================
// Complex Expression Tests
// ============================================================================

TEST(ShuntingYardParserTest, MixedUnaryBinary) {
    auto ast = parse("-sin(45)+2^3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* left = dynamic_cast<UnaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "-");
}

TEST(ShuntingYardParserTest, ComplexNested) {
    auto ast = parse("((1+2)*(3-4))/5");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "/");
}

// ============================================================================
// Error Cases
// ============================================================================

TEST(ShuntingYardParserTest, EmptyExpression) {
    Tokenizer tokenizer("");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(ShuntingYardParserTest, UnbalancedParenthesesMissingClose) {
    Tokenizer tokenizer("(1+2");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(ShuntingYardParserTest, UnbalancedParenthesesMissingOpen) {
    Tokenizer tokenizer("1+2)");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(ShuntingYardParserTest, OperatorWithoutOperands) {
    Tokenizer tokenizer("+");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), CalculatorException);
}

TEST(ShuntingYardParserTest, TwoOperatorsInARow) {
    Tokenizer tokenizer("1++2");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(ShuntingYardParserTest, TrailingOperator) {
    Tokenizer tokenizer("1+");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), CalculatorException);
}

TEST(ShuntingYardParserTest, MisplacedComma) {
    Tokenizer tokenizer(",");
    auto tokens = tokenizer.tokenize();
    ShuntingYardParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

// ============================================================================
// String Representation Tests
// ============================================================================

TEST(ShuntingYardParserTest, ToStringSimpleAddition) {
    std::string str = toString("1+2");
    EXPECT_NE(str.find("+"), std::string::npos);
}

TEST(ShuntingYardParserTest, ToStringComplex) {
    std::string str = toString("(1+2)*3");
    EXPECT_NE(str.find("+"), std::string::npos);
    EXPECT_NE(str.find("*"), std::string::npos);
}

TEST(ShuntingYardParserTest, ToStringUnary) {
    std::string str = toString("-5");
    EXPECT_NE(str.find("-"), std::string::npos);
}

TEST(ShuntingYardParserTest, ToStringFunction) {
    std::string str = toString("sin(0)");
    EXPECT_NE(str.find("sin"), std::string::npos);
}
