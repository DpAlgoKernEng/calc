/**
 * @file ast_test.cpp
 * @brief Unit tests for AST node classes
 */

#include "calc/core/ast.h"
#include "calc/core/token.h"
#include <gtest/gtest.h>
#include <memory>

using namespace calc;

// Simple test visitor that just checks if visit methods are called
class TestVisitor : public ASTVisitor {
public:
    int visitCount = 0;
    bool visitedLiteral = false;
    bool visitedBinary = false;
    bool visitedUnary = false;
    bool visitedFunction = false;

    void visit(LiteralNode& /*node*/) override {
        visitedLiteral = true;
        visitCount++;
    }

    void visit(BinaryOpNode& /*node*/) override {
        visitedBinary = true;
        visitCount++;
    }

    void visit(UnaryOpNode& /*node*/) override {
        visitedUnary = true;
        visitCount++;
    }

    void visit(FunctionCallNode& /*node*/) override {
        visitedFunction = true;
        visitCount++;
    }

    void reset() {
        visitCount = 0;
        visitedLiteral = false;
        visitedBinary = false;
        visitedUnary = false;
        visitedFunction = false;
    }
};

// ============================================================================
// LiteralNode Tests
// ============================================================================

TEST(ASTTest, LiteralNodeConstruction) {
    LiteralNode node(42.0);

    EXPECT_EQ(node.getValue(), 42.0);
}

TEST(ASTTest, LiteralNodeNegativeValue) {
    LiteralNode node(-3.14);

    EXPECT_DOUBLE_EQ(node.getValue(), -3.14);
}

TEST(ASTTest, LiteralNodeZero) {
    LiteralNode node(0.0);

    EXPECT_EQ(node.getValue(), 0.0);
}

TEST(ASTTest, LiteralNodeToString) {
    LiteralNode node(3.14159);

    std::string str = node.toString();
    EXPECT_FALSE(str.empty());
}

TEST(ASTTest, LiteralNodeClone) {
    LiteralNode original(123.45);
    auto cloned = original.clone();

    auto* clonedLiteral = dynamic_cast<LiteralNode*>(cloned.get());
    ASSERT_NE(clonedLiteral, nullptr);
    EXPECT_DOUBLE_EQ(clonedLiteral->getValue(), original.getValue());
}

TEST(ASTTest, LiteralNodeAcceptVisitor) {
    LiteralNode node(42.0);
    TestVisitor visitor;

    node.accept(visitor);

    EXPECT_TRUE(visitor.visitedLiteral);
    EXPECT_EQ(visitor.visitCount, 1);
}

// ============================================================================
// BinaryOpNode Tests
// ============================================================================

TEST(ASTTest, BinaryOpNodeConstruction) {
    auto left = std::make_unique<LiteralNode>(2.0);
    Token op(TokenType::OPERATOR, "+", 1);
    auto right = std::make_unique<LiteralNode>(3.0);

    BinaryOpNode node(std::move(left), op, std::move(right));

    EXPECT_EQ(node.getOperator().value, "+");
    EXPECT_NE(node.getLeft(), nullptr);
    EXPECT_NE(node.getRight(), nullptr);
}

TEST(ASTTest, BinaryOpNodeGetters) {
    auto left = std::make_unique<LiteralNode>(5.0);
    Token op(TokenType::OPERATOR, "*", 2);
    auto right = std::make_unique<LiteralNode>(7.0);

    BinaryOpNode node(std::move(left), op, std::move(right));

    EXPECT_EQ(node.getOperator().value, "*");
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getLeft())->getValue(), 5.0);
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getRight())->getValue(), 7.0);
}

TEST(ASTTest, BinaryOpNodeToString) {
    auto left = std::make_unique<LiteralNode>(1.0);
    Token op(TokenType::OPERATOR, "+", 1);
    auto right = std::make_unique<LiteralNode>(2.0);

    BinaryOpNode node(std::move(left), op, std::move(right));

    std::string str = node.toString();
    EXPECT_NE(str.find("("), std::string::npos);
    EXPECT_NE(str.find("+"), std::string::npos);
}

TEST(ASTTest, BinaryOpNodeClone) {
    auto left = std::make_unique<LiteralNode>(10.0);
    Token op(TokenType::OPERATOR, "-", 1);
    auto right = std::make_unique<LiteralNode>(5.0);

    BinaryOpNode original(std::move(left), op, std::move(right));
    auto cloned = original.clone();

    auto* clonedBinary = dynamic_cast<BinaryOpNode*>(cloned.get());
    ASSERT_NE(clonedBinary, nullptr);
    EXPECT_EQ(clonedBinary->getOperator().value, original.getOperator().value);
}

TEST(ASTTest, BinaryOpNodeAcceptVisitor) {
    auto left = std::make_unique<LiteralNode>(1.0);
    Token op(TokenType::OPERATOR, "+", 1);
    auto right = std::make_unique<LiteralNode>(2.0);

    BinaryOpNode node(std::move(left), op, std::move(right));
    TestVisitor visitor;

    node.accept(visitor);

    EXPECT_TRUE(visitor.visitedBinary);
    EXPECT_EQ(visitor.visitCount, 1);
}

TEST(ASTTest, BinaryOpNodeReleaseLeft) {
    auto left = std::make_unique<LiteralNode>(100.0);
    Token op(TokenType::OPERATOR, "/", 1);
    auto right = std::make_unique<LiteralNode>(10.0);

    BinaryOpNode node(std::move(left), op, std::move(right));
    auto released = node.releaseLeft();

    EXPECT_NE(released, nullptr);
    EXPECT_EQ(node.getLeft(), nullptr);
}

TEST(ASTTest, BinaryOpNodeReleaseRight) {
    auto left = std::make_unique<LiteralNode>(8.0);
    Token op(TokenType::OPERATOR, "/", 1);
    auto right = std::make_unique<LiteralNode>(2.0);

    BinaryOpNode node(std::move(left), op, std::move(right));
    auto released = node.releaseRight();

    EXPECT_NE(released, nullptr);
    EXPECT_EQ(node.getRight(), nullptr);
}

// ============================================================================
// UnaryOpNode Tests
// ============================================================================

TEST(ASTTest, UnaryOpNodeConstruction) {
    Token op(TokenType::OPERATOR, "-", 0);
    auto operand = std::make_unique<LiteralNode>(5.0);

    UnaryOpNode node(op, std::move(operand));

    EXPECT_EQ(node.getOperator().value, "-");
    EXPECT_NE(node.getOperand(), nullptr);
}

TEST(ASTTest, UnaryOpNodeGetters) {
    Token op(TokenType::OPERATOR, "+", 0);
    auto operand = std::make_unique<LiteralNode>(3.14);

    UnaryOpNode node(op, std::move(operand));

    EXPECT_EQ(node.getOperator().value, "+");
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getOperand())->getValue(), 3.14);
}

TEST(ASTTest, UnaryOpNodeToString) {
    Token op(TokenType::OPERATOR, "-", 0);
    auto operand = std::make_unique<LiteralNode>(10.0);

    UnaryOpNode node(op, std::move(operand));

    std::string str = node.toString();
    EXPECT_NE(str.find("("), std::string::npos);
    EXPECT_NE(str.find("-"), std::string::npos);
}

TEST(ASTTest, UnaryOpNodeClone) {
    Token op(TokenType::OPERATOR, "-", 0);
    auto operand = std::make_unique<LiteralNode>(42.0);

    UnaryOpNode original(op, std::move(operand));
    auto cloned = original.clone();

    auto* clonedUnary = dynamic_cast<UnaryOpNode*>(cloned.get());
    ASSERT_NE(clonedUnary, nullptr);
    EXPECT_EQ(clonedUnary->getOperator().value, original.getOperator().value);
}

TEST(ASTTest, UnaryOpNodeAcceptVisitor) {
    Token op(TokenType::OPERATOR, "-", 0);
    auto operand = std::make_unique<LiteralNode>(7.0);

    UnaryOpNode node(op, std::move(operand));
    TestVisitor visitor;

    node.accept(visitor);

    EXPECT_TRUE(visitor.visitedUnary);
    EXPECT_EQ(visitor.visitCount, 1);
}

TEST(ASTTest, UnaryOpNodeReleaseOperand) {
    Token op(TokenType::OPERATOR, "+", 0);
    auto operand = std::make_unique<LiteralNode>(99.0);

    UnaryOpNode node(op, std::move(operand));
    auto released = node.releaseOperand();

    EXPECT_NE(released, nullptr);
    EXPECT_EQ(node.getOperand(), nullptr);
}

// ============================================================================
// FunctionCallNode Tests
// ============================================================================

TEST(ASTTest, FunctionCallNodeConstruction) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(90.0));

    FunctionCallNode node("sin", 0, std::move(args));

    EXPECT_EQ(node.getName(), "sin");
    EXPECT_EQ(node.getArgumentCount(), 1);
}

TEST(ASTTest, FunctionCallNodeGetters) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(1.0));
    args.push_back(std::make_unique<LiteralNode>(2.0));
    args.push_back(std::make_unique<LiteralNode>(3.0));

    FunctionCallNode node("max", 0, std::move(args));

    EXPECT_EQ(node.getName(), "max");
    EXPECT_EQ(node.getArgumentCount(), 3);
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getArgument(0))->getValue(), 1.0);
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getArgument(1))->getValue(), 2.0);
    EXPECT_EQ(dynamic_cast<LiteralNode*>(node.getArgument(2))->getValue(), 3.0);
}

TEST(ASTTest, FunctionCallNodeGetPosition) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(45.0));

    FunctionCallNode node("cos", 5, std::move(args));

    EXPECT_EQ(node.getPosition(), 5);
}

TEST(ASTTest, FunctionCallNodeToString) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(0.0));

    FunctionCallNode node("sin", 0, std::move(args));

    std::string str = node.toString();
    EXPECT_NE(str.find("sin"), std::string::npos);
    EXPECT_NE(str.find("("), std::string::npos);
}

TEST(ASTTest, FunctionCallNodeToStringMultipleArgs) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(1.0));
    args.push_back(std::make_unique<LiteralNode>(2.0));

    FunctionCallNode node("max", 0, std::move(args));

    std::string str = node.toString();
    EXPECT_NE(str.find("max"), std::string::npos);
    EXPECT_NE(str.find(","), std::string::npos);
}

TEST(ASTTest, FunctionCallNodeClone) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(180.0));

    FunctionCallNode original("cos", 0, std::move(args));
    auto cloned = original.clone();

    auto* clonedFunction = dynamic_cast<FunctionCallNode*>(cloned.get());
    ASSERT_NE(clonedFunction, nullptr);
    EXPECT_EQ(clonedFunction->getName(), original.getName());
    EXPECT_EQ(clonedFunction->getArgumentCount(), original.getArgumentCount());
}

TEST(ASTTest, FunctionCallNodeAcceptVisitor) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(0.0));

    FunctionCallNode node("sin", 0, std::move(args));
    TestVisitor visitor;

    node.accept(visitor);

    EXPECT_TRUE(visitor.visitedFunction);
    EXPECT_EQ(visitor.visitCount, 1);
}

TEST(ASTTest, FunctionCallNodeGetArgumentOutOfBounds) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(1.0));

    FunctionCallNode node("test", 0, std::move(args));

    EXPECT_THROW(node.getArgument(1), std::out_of_range);
}

TEST(ASTTest, FunctionCallNodeReleaseArguments) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(1.0));
    args.push_back(std::make_unique<LiteralNode>(2.0));

    FunctionCallNode node("test", 0, std::move(args));

    auto released = node.releaseArguments();

    EXPECT_EQ(released.size(), 2);
    EXPECT_EQ(node.getArgumentCount(), 0);
}

// ============================================================================
// Complex AST Tests
// ============================================================================

TEST(ASTTest, NestedBinaryOps) {
    // Build tree: (1 + 2) * 3
    auto leftLeft = std::make_unique<LiteralNode>(1.0);
    auto leftRight = std::make_unique<LiteralNode>(2.0);
    Token plusOp(TokenType::OPERATOR, "+", 1);
    auto left = std::make_unique<BinaryOpNode>(std::move(leftLeft), plusOp, std::move(leftRight));

    auto right = std::make_unique<LiteralNode>(3.0);
    Token mulOp(TokenType::OPERATOR, "*", 5);
    BinaryOpNode root(std::move(left), mulOp, std::move(right));

    std::string str = root.toString();
    EXPECT_NE(str.find("+"), std::string::npos);
    EXPECT_NE(str.find("*"), std::string::npos);
}

TEST(ASTTest, UnaryInBinary) {
    // Build tree: -5 + 3
    auto leftOperand = std::make_unique<LiteralNode>(5.0);
    Token unaryOp(TokenType::OPERATOR, "-", 0);
    auto left = std::make_unique<UnaryOpNode>(unaryOp, std::move(leftOperand));

    auto right = std::make_unique<LiteralNode>(3.0);
    Token binOp(TokenType::OPERATOR, "+", 2);
    BinaryOpNode root(std::move(left), binOp, std::move(right));

    std::string str = root.toString();
    EXPECT_NE(str.find("-"), std::string::npos);
    EXPECT_NE(str.find("+"), std::string::npos);
}

// ============================================================================
// Main function
// ============================================================================

// Note: main() is provided by GTest::gtest_main, no need to define it here

