/**
 * @file parser_test.cpp
 * @brief Unit tests for general parser functionality
 *
 * This file contains tests that should work with any Parser implementation.
 * It serves as a framework for parser-independent tests.
 */

#include "calc/core/shunting_yard_parser.h"
#include "calc/core/recursive_descent_parser.h"
#include "calc/core/tokenizer.h"
#include <gtest/gtest.h>

using namespace calc;

namespace {

// Helper to tokenize an expression
std::vector<Token> tokenize(const std::string& expr) {
    Tokenizer tokenizer(expr);
    return tokenizer.tokenize();
}

} // anonymous namespace

// ============================================================================
// General Parser Interface Tests
// ============================================================================

// Test that both parsers produce equivalent AST structures for simple expressions
TEST(ParserTest, BothParsersHandleSimpleAddition) {
    std::string expr = "1+2";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    // Both should produce a valid BinaryOpNode
    auto* shuntingBinary = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdBinary = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingBinary, nullptr);
    ASSERT_NE(rdBinary, nullptr);
    EXPECT_EQ(shuntingBinary->getOperator().value, "+");
    EXPECT_EQ(rdBinary->getOperator().value, "+");
}

// ============================================================================
// Parser Comparison Tests
// ============================================================================

// This test suite verifies that both parsers handle the same expressions correctly
TEST(ParserTest, ParserConsistencyComplexExpression) {
    std::string expr = "2+3*4-5/2";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    // Both should parse without throwing exceptions
    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    ASSERT_NE(shuntingAst, nullptr);
    ASSERT_NE(rdAst, nullptr);

    // Both should produce the same structure (based on precedence rules)
    // Root should be subtraction (last operation in left-associative evaluation)
    auto* shuntingRoot = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdRoot = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingRoot, nullptr);
    ASSERT_NE(rdRoot, nullptr);
    EXPECT_EQ(shuntingRoot->getOperator().value, "-");
    EXPECT_EQ(rdRoot->getOperator().value, "-");
}

TEST(ParserTest, ParserConsistencyNestedParentheses) {
    std::string expr = "((1+2)*(3-4))/5";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    // Both should handle nested parentheses correctly
    auto* shuntingRoot = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdRoot = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingRoot, nullptr);
    ASSERT_NE(rdRoot, nullptr);
    EXPECT_EQ(shuntingRoot->getOperator().value, "/");
    EXPECT_EQ(rdRoot->getOperator().value, "/");
}

TEST(ParserTest, BothParsersHandleFunctions) {
    std::string expr = "sin(45) + cos(90)";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    // Both should handle function calls
    auto* shuntingRoot = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdRoot = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingRoot, nullptr);
    ASSERT_NE(rdRoot, nullptr);
    EXPECT_EQ(shuntingRoot->getOperator().value, "+");
    EXPECT_EQ(rdRoot->getOperator().value, "+");
}

TEST(ParserTest, BothParsersHandleUnaryOperators) {
    std::string expr = "-5 + -3";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    // Both should handle unary minus operators
    auto* shuntingRoot = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdRoot = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingRoot, nullptr);
    ASSERT_NE(rdRoot, nullptr);
    EXPECT_EQ(shuntingRoot->getOperator().value, "+");

    // Left side should be a unary operator
    auto* shuntingLeft = dynamic_cast<UnaryOpNode*>(shuntingRoot->getLeft());
    auto* rdLeft = dynamic_cast<UnaryOpNode*>(rdRoot->getLeft());

    ASSERT_NE(shuntingLeft, nullptr);
    ASSERT_NE(rdLeft, nullptr);
}

TEST(ParserTest, BothParsersHandleExponentiation) {
    std::string expr = "2^3^4";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    auto shuntingAst = shuntingParser.parse(tokens);
    auto rdAst = rdParser.parse(tokens);

    // Both should handle right-associative exponentiation
    auto* shuntingRoot = dynamic_cast<BinaryOpNode*>(shuntingAst.get());
    auto* rdRoot = dynamic_cast<BinaryOpNode*>(rdAst.get());

    ASSERT_NE(shuntingRoot, nullptr);
    ASSERT_NE(rdRoot, nullptr);
    EXPECT_EQ(shuntingRoot->getOperator().value, "^");
    EXPECT_EQ(rdRoot->getOperator().value, "^");

    // Right side should also be exponentiation (right-associative)
    auto* shuntingRight = dynamic_cast<BinaryOpNode*>(shuntingRoot->getRight());
    auto* rdRight = dynamic_cast<BinaryOpNode*>(rdRoot->getRight());

    ASSERT_NE(shuntingRight, nullptr);
    ASSERT_NE(rdRight, nullptr);
    EXPECT_EQ(shuntingRight->getOperator().value, "^");
    EXPECT_EQ(rdRight->getOperator().value, "^");
}

// ============================================================================
// Error Handling Consistency Tests
// ============================================================================

TEST(ParserTest, BothParsersThrowOnEmptyExpression) {
    std::string expr = "";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    EXPECT_THROW(shuntingParser.parse(tokens), SyntaxError);
    EXPECT_THROW(rdParser.parse(tokens), SyntaxError);
}

TEST(ParserTest, BothParsersThrowOnUnbalancedParentheses) {
    std::string expr = "(1+2";
    auto tokens = tokenize(expr);

    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    EXPECT_THROW(shuntingParser.parse(tokens), SyntaxError);
    EXPECT_THROW(rdParser.parse(tokens), SyntaxError);
}

// ============================================================================
// Parser Metadata Tests
// ============================================================================

TEST(ParserTest, ShuntingYardParserName) {
    ShuntingYardParser parser;
    EXPECT_EQ(parser.getName(), "ShuntingYardParser");
}

TEST(ParserTest, RecursiveDescentParserName) {
    RecursiveDescentParser parser;
    EXPECT_EQ(parser.getName(), "RecursiveDescentParser");
}

TEST(ParserTest, BothParsersEnableUnaryByDefault) {
    ShuntingYardParser shuntingParser;
    RecursiveDescentParser rdParser;

    EXPECT_TRUE(shuntingParser.isUnaryOperatorsEnabled());
    EXPECT_TRUE(rdParser.isUnaryOperatorsEnabled());
}

TEST(ParserTest, ShuntingYardParserCanDisableUnary) {
    ShuntingYardParser parser;
    parser.setUnaryOperatorsEnabled(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());
}

TEST(ParserTest, RecursiveDescentParserCanDisableUnary) {
    RecursiveDescentParser parser;
    parser.setUnaryOperatorsEnabled(false);
    EXPECT_FALSE(parser.isUnaryOperatorsEnabled());
}
