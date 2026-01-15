/**
 * @file recursive_descent_parser_test.cpp
 * @brief Unit tests for RecursiveDescentParser class
 */

#include "calc/core/recursive_descent_parser.h"
#include "calc/core/tokenizer.h"
#include <gtest/gtest.h>

using namespace calc;

namespace {

// Helper function to parse a string expression using RecursiveDescentParser
std::unique_ptr<ASTNode> parseExpression(const std::string& expr) {
    Tokenizer tokenizer(expr);
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;
    return parser.parse(tokens);
}

// Helper function to get string representation of parsed expression
std::string parseToString(const std::string& expr) {
    auto ast = parseExpression(expr);
    return ast->toString();
}

} // anonymous namespace

// ============================================================================
// Parser Construction Tests
// ============================================================================

TEST(RecursiveDescentParserTest, DefaultConstruction) {
    RecursiveDescentParser parser;
    EXPECT_TRUE(parser.isUnaryOperatorsEnabled());
}

TEST(RecursiveDescentParserTest, ConstructionWithoutUnary) {
    RecursiveDescentParser parser(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());
}

TEST(RecursiveDescentParserTest, SetUnaryOperatorsEnabled) {
    RecursiveDescentParser parser;
    parser.setUnaryOperatorsEnabled(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());

    parser.setUnaryOperatorsEnabled(true);
    EXPECT_TRUE(parser.isUnaryOperatorsEnabled());
}

TEST(RecursiveDescentParserTest, GetName) {
    RecursiveDescentParser parser;
    EXPECT_EQ(parser.getName(), "RecursiveDescentParser");
}

// ============================================================================
// Single Value Tests
// ============================================================================

TEST(RecursiveDescentParserTest, ParseSingleInteger) {
    auto ast = parseExpression("42");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 42.0);
}

TEST(RecursiveDescentParserTest, ParseSingleDecimal) {
    auto ast = parseExpression("3.14");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 3.14);
}

TEST(RecursiveDescentParserTest, ParseScientificNotation) {
    auto ast = parseExpression("1.23e4");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 12300.0);
}

TEST(RecursiveDescentParserTest, ParseNegativeScientificNotation) {
    auto ast = parseExpression("1.5e-3");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 0.0015);
}

// ============================================================================
// Simple Operation Tests
// ============================================================================

TEST(RecursiveDescentParserTest, ParseSimpleAddition) {
    auto ast = parseExpression("1+2");
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

TEST(RecursiveDescentParserTest, ParseSimpleSubtraction) {
    auto ast = parseExpression("5-3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, ParseSimpleMultiplication) {
    auto ast = parseExpression("2*3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "*");
}

TEST(RecursiveDescentParserTest, ParseSimpleDivision) {
    auto ast = parseExpression("6/2");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "/");
}

TEST(RecursiveDescentParserTest, ParseModulo) {
    auto ast = parseExpression("7%3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "%");
}

TEST(RecursiveDescentParserTest, ParseExponentiation) {
    auto ast = parseExpression("2^3");
    auto* binary = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->getOperator().value, "^");
}

// ============================================================================
// Operator Precedence Tests
// ============================================================================

TEST(RecursiveDescentParserTest, PrecedenceMultiplicationBeforeAddition) {
    auto ast = parseExpression("1+2*3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    // Right side should be the multiplication
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "*");
}

TEST(RecursiveDescentParserTest, PrecedenceDivisionBeforeAddition) {
    auto ast = parseExpression("1+4/2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "/");
}

TEST(RecursiveDescentParserTest, PrecedenceModuloBeforeAddition) {
    auto ast = parseExpression("1+5%2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "%");
}

TEST(RecursiveDescentParserTest, PrecedenceExponentiationBeforeMultiplication) {
    auto ast = parseExpression("2*3^2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "*");

    // Right side should be the exponentiation
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "^");
}

TEST(RecursiveDescentParserTest, PrecedenceComplex) {
    auto ast = parseExpression("1+2*3-4/5");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, SamePrecedenceLeftAssociative) {
    auto ast = parseExpression("8/4/2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "/");

    // Left side should be the first division
    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "/");
}

TEST(RecursiveDescentParserTest, LeftAssociativeMultiplication) {
    auto ast = parseExpression("2*3*4");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "*");

    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "*");
}

TEST(RecursiveDescentParserTest, LeftAssociativeAddition) {
    auto ast = parseExpression("1+2+3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "+");
}

// ============================================================================
// Parentheses Tests
// ============================================================================

TEST(RecursiveDescentParserTest, ParenthesesOverridePrecedence) {
    auto ast = parseExpression("(1+2)*3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "*");

    // Left side should be the addition
    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, NestedParentheses) {
    auto ast = parseExpression("((1+2)*3)+4");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, ParenthesesAroundSingleValue) {
    auto ast = parseExpression("(42)");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 42.0);
}

TEST(RecursiveDescentParserTest, ParenthesesWithDivision) {
    auto ast = parseExpression("(1+2)/(3+4)");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "/");

    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(left->getOperator().value, "+");
    EXPECT_EQ(right->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, DeeplyNestedParentheses) {
    auto ast = parseExpression("(((1)))");
    auto* literal = dynamic_cast<LiteralNode*>(ast.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_DOUBLE_EQ(literal->getValue(), 1.0);
}

// ============================================================================
// Unary Operator Tests
// ============================================================================

TEST(RecursiveDescentParserTest, UnaryMinusAtStart) {
    auto ast = parseExpression("-5");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");

    auto* operand = dynamic_cast<LiteralNode*>(unary->getOperand());
    ASSERT_NE(operand, nullptr);
    EXPECT_DOUBLE_EQ(operand->getValue(), 5.0);
}

TEST(RecursiveDescentParserTest, UnaryPlusAtStart) {
    auto ast = parseExpression("+5");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, UnaryMinusInExpression) {
    auto ast = parseExpression("-5+3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* left = dynamic_cast<UnaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, UnaryAfterOperator) {
    auto ast = parseExpression("5+-3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* right = dynamic_cast<UnaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, UnaryAfterLeftParen) {
    auto ast = parseExpression("(-5)");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, DoubleNegative) {
    auto ast = parseExpression("--5");
    auto* outer = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->getOperator().value, "-");

    auto* inner = dynamic_cast<UnaryOpNode*>(outer->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, TripleNegative) {
    auto ast = parseExpression("---5");
    auto* outer = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->getOperator().value, "-");

    auto* middle = dynamic_cast<UnaryOpNode*>(outer->getOperand());
    ASSERT_NE(middle, nullptr);
    EXPECT_EQ(middle->getOperator().value, "-");

    auto* inner = dynamic_cast<UnaryOpNode*>(middle->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, UnaryWithParenthesizedExpression) {
    auto ast = parseExpression("-(1+2)");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");

    auto* inner = dynamic_cast<BinaryOpNode*>(unary->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, UnaryDisabled) {
    RecursiveDescentParser parser(false);
    Tokenizer tokenizer("-5");
    auto tokens = tokenizer.tokenize();

    // Without unary support, this should fail
    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

// ============================================================================
// Exponentiation Associativity Tests
// ============================================================================

TEST(RecursiveDescentParserTest, ExponentiationRightAssociative) {
    auto ast = parseExpression("2^3^4");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "^");

    // Right side should be another exponentiation (right-associative)
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "^");
}

TEST(RecursiveDescentParserTest, ExponentiationWithUnary) {
    auto ast = parseExpression("-2^3");
    auto* unary = dynamic_cast<UnaryOpNode*>(ast.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->getOperator().value, "-");

    auto* inner = dynamic_cast<BinaryOpNode*>(unary->getOperand());
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getOperator().value, "^");
}

TEST(RecursiveDescentParserTest, ComplexExponentiation) {
    auto ast = parseExpression("2^(3+1)");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "^");

    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, ExponentiationWithParentheses) {
    auto ast = parseExpression("(2+3)^(4-1)");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "^");

    auto* left = dynamic_cast<BinaryOpNode*>(root->getLeft());
    auto* right = dynamic_cast<BinaryOpNode*>(root->getRight());
    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(left->getOperator().value, "+");
    EXPECT_EQ(right->getOperator().value, "-");
}

// ============================================================================
// Function Call Tests
// ============================================================================

TEST(RecursiveDescentParserTest, SingleArgFunction) {
    auto ast = parseExpression("sin(0)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
    EXPECT_EQ(func->getArgumentCount(), 1);
}

TEST(RecursiveDescentParserTest, FunctionWithExpression) {
    auto ast = parseExpression("sin(90+180)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
    EXPECT_EQ(func->getArgumentCount(), 1);

    auto* arg = dynamic_cast<BinaryOpNode*>(func->getArgument(0));
    ASSERT_NE(arg, nullptr);
    EXPECT_EQ(arg->getOperator().value, "+");
}

TEST(RecursiveDescentParserTest, FunctionWithParentheses) {
    auto ast = parseExpression("sin((90+180))");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "sin");
}

TEST(RecursiveDescentParserTest, MultiArgFunction) {
    auto ast = parseExpression("max(1,2,3)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "max");
    EXPECT_EQ(func->getArgumentCount(), 3);
}

TEST(RecursiveDescentParserTest, FunctionWithComplexArgs) {
    auto ast = parseExpression("max(1+2,3*4,5/2)");
    auto* func = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->getName(), "max");
    EXPECT_EQ(func->getArgumentCount(), 3);
}

TEST(RecursiveDescentParserTest, NestedFunction) {
    auto ast = parseExpression("sin(cos(0))");
    auto* outer = dynamic_cast<FunctionCallNode*>(ast.get());

    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->getName(), "sin");
    EXPECT_EQ(outer->getArgumentCount(), 1);

    auto* inner = dynamic_cast<FunctionCallNode*>(outer->getArgument(0));
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->getName(), "cos");
}

// ============================================================================
// Complex Expression Tests
// ============================================================================

TEST(RecursiveDescentParserTest, MixedUnaryBinary) {
    auto ast = parseExpression("-sin(45)+2^3");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "+");

    auto* left = dynamic_cast<UnaryOpNode*>(root->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, ComplexNested) {
    auto ast = parseExpression("((1+2)*(3-4))/5");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getOperator().value, "/");
}

TEST(RecursiveDescentParserTest, VeryComplexExpression) {
    auto ast = parseExpression("1+2*3-4/5^2");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    // Should be: 1 + (2*3) - (4/(5^2))
    // Root is the last subtraction
    EXPECT_EQ(root->getOperator().value, "-");
}

TEST(RecursiveDescentParserTest, ExpressionWithAllOperators) {
    auto ast = parseExpression("1+2-3*4/5%6^7");
    auto* root = dynamic_cast<BinaryOpNode*>(ast.get());

    ASSERT_NE(root, nullptr);
    // Should respect precedence: ^ first, then % / *, then + -
}

// ============================================================================
// Error Cases
// ============================================================================

TEST(RecursiveDescentParserTest, EmptyExpression) {
    Tokenizer tokenizer("");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(RecursiveDescentParserTest, UnbalancedParenthesesMissingClose) {
    Tokenizer tokenizer("(1+2");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(RecursiveDescentParserTest, UnbalancedParenthesesMissingOpen) {
    Tokenizer tokenizer("1+2)");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(RecursiveDescentParserTest, OperatorWithoutOperands) {
    Tokenizer tokenizer("+");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(RecursiveDescentParserTest, TrailingOperator) {
    Tokenizer tokenizer("1+");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

TEST(RecursiveDescentParserTest, MisplacedComma) {
    Tokenizer tokenizer(",");
    auto tokens = tokenizer.tokenize();
    RecursiveDescentParser parser;

    EXPECT_THROW(parser.parse(tokens), SyntaxError);
}

// ============================================================================
// String Representation Tests
// ============================================================================

TEST(RecursiveDescentParserTest, ToStringSimpleAddition) {
    std::string str = parseToString("1+2");
    EXPECT_NE(str.find("+"), std::string::npos);
}

TEST(RecursiveDescentParserTest, ToStringComplex) {
    std::string str = parseToString("(1+2)*3");
    EXPECT_NE(str.find("+"), std::string::npos);
    EXPECT_NE(str.find("*"), std::string::npos);
}

TEST(RecursiveDescentParserTest, ToStringUnary) {
    std::string str = parseToString("-5");
    EXPECT_NE(str.find("-"), std::string::npos);
}

TEST(RecursiveDescentParserTest, ToStringFunction) {
    std::string str = parseToString("sin(0)");
    EXPECT_NE(str.find("sin"), std::string::npos);
}

// ============================================================================
// Main function
// ============================================================================

// Note: main() is provided by GTest::gtest_main, no need to define it here
