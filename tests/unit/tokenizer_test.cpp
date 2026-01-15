/**
 * @file tokenizer_test.cpp
 * @brief Unit tests for Tokenizer class
 */

#include "calc/core/tokenizer.h"
#include <gtest/gtest.h>

using namespace calc;

// ============================================================================
// Token Tests
// ============================================================================

TEST(TokenTest, TokenTypeToString) {
    EXPECT_EQ(tokenTypeToString(TokenType::NUMBER), "NUMBER");
    EXPECT_EQ(tokenTypeToString(TokenType::OPERATOR), "OPERATOR");
    EXPECT_EQ(tokenTypeToString(TokenType::FUNCTION), "FUNCTION");
    EXPECT_EQ(tokenTypeToString(TokenType::LPAREN), "LPAREN");
    EXPECT_EQ(tokenTypeToString(TokenType::RPAREN), "RPAREN");
    EXPECT_EQ(tokenTypeToString(TokenType::COMMA), "COMMA");
    EXPECT_EQ(tokenTypeToString(TokenType::EOF_TOKEN), "EOF");
    EXPECT_EQ(tokenTypeToString(TokenType::UNKNOWN), "UNKNOWN");
}

TEST(TokenTest, DefaultConstruction) {
    Token token;
    EXPECT_EQ(token.type, TokenType::UNKNOWN);
    EXPECT_TRUE(token.value.empty());
    EXPECT_EQ(token.position, 0);
}

TEST(TokenTest, FullConstruction) {
    Token token(TokenType::NUMBER, "123", 5);
    EXPECT_EQ(token.type, TokenType::NUMBER);
    EXPECT_EQ(token.value, "123");
    EXPECT_EQ(token.position, 5);
}

TEST(TokenTest, IsOperator) {
    Token op(TokenType::OPERATOR, "+", 0);
    EXPECT_TRUE(op.isOperator());

    Token num(TokenType::NUMBER, "123", 0);
    EXPECT_FALSE(num.isOperator());
}

TEST(TokenTest, IsNumber) {
    Token num(TokenType::NUMBER, "123", 0);
    EXPECT_TRUE(num.isNumber());

    Token op(TokenType::OPERATOR, "+", 0);
    EXPECT_FALSE(op.isNumber());
}

TEST(TokenTest, IsFunction) {
    Token func(TokenType::FUNCTION, "sin", 0);
    EXPECT_TRUE(func.isFunction());

    Token num(TokenType::NUMBER, "123", 0);
    EXPECT_FALSE(num.isFunction());
}

TEST(TokenTest, OperatorPrecedence) {
    Token exp(TokenType::OPERATOR, "^", 0);
    Token mul(TokenType::OPERATOR, "*", 0);
    Token div(TokenType::OPERATOR, "/", 0);
    Token add(TokenType::OPERATOR, "+", 0);
    Token sub(TokenType::OPERATOR, "-", 0);

    EXPECT_EQ(exp.getPrecedence(), 4);
    EXPECT_EQ(mul.getPrecedence(), 3);
    EXPECT_EQ(div.getPrecedence(), 3);
    EXPECT_EQ(add.getPrecedence(), 2);
    EXPECT_EQ(sub.getPrecedence(), 2);
}

TEST(TokenTest, OperatorPrecedenceOnNonOperator) {
    Token num(TokenType::NUMBER, "123", 0);
    EXPECT_EQ(num.getPrecedence(), 0);  // Non-operators return 0
}

TEST(TokenTest, IsRightAssociative) {
    Token exp(TokenType::OPERATOR, "^", 0);
    Token add(TokenType::OPERATOR, "+", 0);

    EXPECT_TRUE(exp.isRightAssociative());
    EXPECT_FALSE(add.isRightAssociative());
}

TEST(TokenTest, IsRightAssociativeOnNonOperator) {
    Token num(TokenType::NUMBER, "123", 0);
    EXPECT_FALSE(num.isRightAssociative());
    EXPECT_EQ(num.getPrecedence(), 0);  // Non-operators return 0
}

TEST(TokenTest, TokenEquality) {
    Token token1(TokenType::NUMBER, "123", 0);
    Token token2(TokenType::NUMBER, "123", 0);
    Token token3(TokenType::NUMBER, "456", 0);

    EXPECT_EQ(token1, token2);
    EXPECT_NE(token1, token3);
}

TEST(TokenTest, TokenInequality) {
    Token token1(TokenType::NUMBER, "123", 0);
    Token token2(TokenType::OPERATOR, "+", 0);

    EXPECT_NE(token1, token2);
}

// ============================================================================
// Tokenizer Tests - Basic Numbers
// ============================================================================

TEST(TokenizerTest, SingleInteger) {
    Tokenizer tokenizer("42");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);  // NUMBER + EOF
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "42");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_TOKEN);
}

TEST(TokenizerTest, SingleZero) {
    Tokenizer tokenizer("0");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "0");
}

TEST(TokenizerTest, NegativeInteger) {
    Tokenizer tokenizer("-42");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 3);  // OPERATOR -, NUMBER, EOF
    EXPECT_EQ(tokens[0].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[0].value, "-");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].value, "42");
}

TEST(TokenizerTest, DecimalNumber) {
    Tokenizer tokenizer("3.14");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "3.14");
}

TEST(TokenizerTest, ScientificNotationPositive) {
    Tokenizer tokenizer("1.23e4");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "1.23e4");
}

TEST(TokenizerTest, ScientificNotationNegative) {
    Tokenizer tokenizer("1.23e-4");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "1.23e-4");
}

TEST(TokenizerTest, ScientificNotationCapitalE) {
    Tokenizer tokenizer("5.67E+3");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "5.67E+3");
}

// ============================================================================
// Tokenizer Tests - Operators
// ============================================================================

TEST(TokenizerTest, AllOperators) {
    Tokenizer tokenizer("+ - * / ^ %");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 7);  // 6 operators + EOF
    EXPECT_EQ(tokens[0].value, "+");
    EXPECT_EQ(tokens[1].value, "-");
    EXPECT_EQ(tokens[2].value, "*");
    EXPECT_EQ(tokens[3].value, "/");
    EXPECT_EQ(tokens[4].value, "^");
    EXPECT_EQ(tokens[5].value, "%");
}

// ============================================================================
// Tokenizer Tests - Parentheses
// ============================================================================

TEST(TokenizerTest, LeftParen) {
    Tokenizer tokenizer("(");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[0].value, "(");
}

TEST(TokenizerTest, RightParen) {
    Tokenizer tokenizer(")");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::RPAREN);
    EXPECT_EQ(tokens[0].value, ")");
}

TEST(TokenizerTest, ParenthesesPair) {
    Tokenizer tokenizer("()");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RPAREN);
}

// ============================================================================
// Tokenizer Tests - Functions
// ============================================================================

TEST(TokenizerTest, SingleLetterFunction) {
    Tokenizer tokenizer("sin");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::FUNCTION);
    EXPECT_EQ(tokens[0].value, "sin");
}

TEST(TokenizerTest, MultiLetterFunction) {
    Tokenizer tokenizer("sqrt");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::FUNCTION);
    EXPECT_EQ(tokens[0].value, "sqrt");
}

TEST(TokenizerTest, FunctionWithNumbers) {
    Tokenizer tokenizer("log10");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::FUNCTION);
    EXPECT_EQ(tokens[0].value, "log10");
}

TEST(TokenizerTest, MixedCaseFunction) {
    Tokenizer tokenizer("SIN");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::FUNCTION);
    EXPECT_EQ(tokens[0].value, "SIN");
}

// ============================================================================
// Tokenizer Tests - Simple Expressions
// ============================================================================

TEST(TokenizerTest, SimpleAddition) {
    Tokenizer tokenizer("1+2");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);  // 1, +, 2, EOF
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].value, "1");
    EXPECT_EQ(tokens[1].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[1].value, "+");
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[2].value, "2");
}

TEST(TokenizerTest, SimpleSubtraction) {
    Tokenizer tokenizer("5-3");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[1].value, "-");
}

TEST(TokenizerTest, SimpleMultiplication) {
    Tokenizer tokenizer("2*3");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[1].value, "*");
}

TEST(TokenizerTest, SimpleDivision) {
    Tokenizer tokenizer("6/2");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[1].value, "/");
}

// ============================================================================
// Tokenizer Tests - Complex Expressions
// ============================================================================

TEST(TokenizerTest, ExpressionWithPrecedence) {
    Tokenizer tokenizer("1+2*3");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].value, "1");
    EXPECT_EQ(tokens[1].value, "+");
    EXPECT_EQ(tokens[2].value, "2");
    EXPECT_EQ(tokens[3].value, "*");
    EXPECT_EQ(tokens[4].value, "3");
}

TEST(TokenizerTest, ExpressionWithParentheses) {
    Tokenizer tokenizer("(1+2)*3");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 8);  // Including EOF token
    EXPECT_EQ(tokens[0].value, "(");
    EXPECT_EQ(tokens[1].value, "1");
    EXPECT_EQ(tokens[2].value, "+");
    EXPECT_EQ(tokens[3].value, "2");
    EXPECT_EQ(tokens[4].value, ")");
    EXPECT_EQ(tokens[5].value, "*");
    EXPECT_EQ(tokens[6].value, "3");
    EXPECT_EQ(tokens[7].type, TokenType::EOF_TOKEN);
}

TEST(TokenizerTest, ComplexExpression) {
    Tokenizer tokenizer("1+2*3-4/5");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].value, "1");
    EXPECT_EQ(tokens[2].value, "2");
    EXPECT_EQ(tokens[4].value, "3");
    EXPECT_EQ(tokens[6].value, "4");
    EXPECT_EQ(tokens[8].value, "5");
}

// ============================================================================
// Tokenizer Tests - Function Calls
// ============================================================================

TEST(TokenizerTest, SingleArgFunction) {
    Tokenizer tokenizer("sin(0)");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::FUNCTION);
    EXPECT_EQ(tokens[0].value, "sin");
    EXPECT_EQ(tokens[1].type, TokenType::LPAREN);
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[3].type, TokenType::RPAREN);
}

TEST(TokenizerTest, MultiArgFunction) {
    Tokenizer tokenizer("max(1,2,3)");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 9);  // Including EOF token
    EXPECT_EQ(tokens[0].value, "max");
    EXPECT_EQ(tokens[2].value, "1");
    EXPECT_EQ(tokens[3].value, ",");
    EXPECT_EQ(tokens[4].value, "2");
    EXPECT_EQ(tokens[5].value, ",");
    EXPECT_EQ(tokens[6].value, "3");
}

TEST(TokenizerTest, NestedFunction) {
    Tokenizer tokenizer("sin(cos(45))");
    auto tokens = tokenizer.tokenize();

    // sin (, (, cos (, (, 45, ), ), EOF = 8 tokens
    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].value, "sin");
    EXPECT_EQ(tokens[2].value, "cos");
}

// ============================================================================
// Tokenizer Tests - Whitespace
// ============================================================================

TEST(TokenizerTest, SingleSpace) {
    Tokenizer tokenizer("1 + 2");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].value, "1");
    EXPECT_EQ(tokens[1].value, "+");
    EXPECT_EQ(tokens[2].value, "2");
}

TEST(TokenizerTest, MultipleSpaces) {
    Tokenizer tokenizer("1   +   2");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
}

TEST(TokenizerTest, LeadingTrailingSpaces) {
    Tokenizer tokenizer("  1+2  ");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].value, "1");
}

TEST(TokenizerTest, TabsAndNewlines) {
    Tokenizer tokenizer("1\t+\n2");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
}

// ============================================================================
// Tokenizer Tests - Position Tracking
// ============================================================================

TEST(TokenizerTest, PositionSingleNumber) {
    Tokenizer tokenizer("123");
    auto tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[0].position, 0);
}

TEST(TokenizerTest, PositionWithWhitespace) {
    Tokenizer tokenizer("  123");
    auto tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[0].position, 2);
}

TEST(TokenizerTest, PositionMultipleTokens) {
    Tokenizer tokenizer("1+2");
    auto tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[0].position, 0);  // 1
    EXPECT_EQ(tokens[1].position, 1);  // +
    EXPECT_EQ(tokens[2].position, 2);  // 2
}

// ============================================================================
// Tokenizer Tests - Error Cases
// ============================================================================

TEST(TokenizerTest, InvalidNumberMultipleDecimals) {
    Tokenizer tokenizer("1.2.3");
    EXPECT_THROW(tokenizer.tokenize(), SyntaxError);
}

TEST(TokenizerTest, InvalidNumberDecimalWithoutDigits) {
    Tokenizer tokenizer("1.");
    EXPECT_THROW(tokenizer.tokenize(), SyntaxError);
}

TEST(TokenizerTest, InvalidNumberStartingWithDot) {
    Tokenizer tokenizer(".5");
    // This is allowed (0.5)
    auto tokens = tokenizer.tokenize();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].value, ".5");
}

TEST(TokenizerTest, InvalidScientificNoDigits) {
    Tokenizer tokenizer("1e");
    EXPECT_THROW(tokenizer.tokenize(), SyntaxError);
}

TEST(TokenizerTest, InvalidCharacter) {
    Tokenizer tokenizer("1#2");
    EXPECT_THROW(tokenizer.tokenize(), SyntaxError);
}

TEST(TokenizerTest, EmptyInput) {
    Tokenizer tokenizer("");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 1);  // Only EOF
    EXPECT_EQ(tokens[0].type, TokenType::EOF_TOKEN);
}

TEST(TokenizerTest, WhitespaceOnly) {
    Tokenizer tokenizer("   ");
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 1);  // Only EOF
    EXPECT_EQ(tokens[0].type, TokenType::EOF_TOKEN);
}

// ============================================================================
// Main function
// ============================================================================

// Note: main() is provided by GTest::gtest_main, no need to define it here

