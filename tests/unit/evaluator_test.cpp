/**
 * @file evaluator_test.cpp
 * @brief Unit tests for the expression evaluator
 */

#include <gtest/gtest.h>
#include "calc/core/evaluator.h"
#include "calc/core/token.h"
#include "calc/core/tokenizer.h"
#include "calc/core/recursive_descent_parser.h"
#include <cmath>

using namespace calc;

//=============================================================================
// EvaluationResult Tests
//=============================================================================

class EvaluationResultTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EvaluationResultTest, SuccessResult) {
    EvaluationResult result(42.0);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_FALSE(result.isError());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
    EXPECT_EQ(result.toString(), "42");
}

TEST_F(EvaluationResultTest, SuccessResultNegative) {
    EvaluationResult result(-3.14);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.14);
}

TEST_F(EvaluationResultTest, SuccessResultZero) {
    EvaluationResult result(0.0);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 0.0);
}

TEST_F(EvaluationResultTest, ErrorResult) {
    EvaluationResult result(ErrorCode::DIVISION_BY_ZERO,
        "Division by zero", 10);

    EXPECT_FALSE(result.isSuccess());
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
    EXPECT_EQ(result.getErrorMessage(), "Division by zero");
    EXPECT_EQ(result.getErrorPosition(), 10);
}

TEST_F(EvaluationResultTest, ErrorResultNoPosition) {
    EvaluationResult result(ErrorCode::INVALID_FUNCTION,
        "Unknown function");

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::INVALID_FUNCTION);
    EXPECT_EQ(result.getErrorMessage(), "Unknown function");
    EXPECT_EQ(result.getErrorPosition(), 0);
}

TEST_F(EvaluationResultTest, GetValueThrowsOnError) {
    EvaluationResult result(ErrorCode::EVALUATION_ERROR, "Error");

    EXPECT_THROW(result.getValue(), std::runtime_error);
}

TEST_F(EvaluationResultTest, GetErrorCodeThrowsOnSuccess) {
    EvaluationResult result(42.0);

    EXPECT_THROW(result.getErrorCode(), std::runtime_error);
}

TEST_F(EvaluationResultTest, GetErrorMessageThrowsOnSuccess) {
    EvaluationResult result(42.0);

    EXPECT_THROW(result.getErrorMessage(), std::runtime_error);
}

TEST_F(EvaluationResultTest, GetErrorPositionThrowsOnSuccess) {
    EvaluationResult result(42.0);

    EXPECT_THROW(result.getErrorPosition(), std::runtime_error);
}

//=============================================================================
// EvaluationContext Tests
//=============================================================================

class EvaluationContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EvaluationContext>();
        MathFunctions::registerBuiltInFunctions(*context);
    }

    void TearDown() override {}

    std::unique_ptr<EvaluationContext> context;
};

TEST_F(EvaluationContextTest, DefaultPrecision) {
    EXPECT_EQ(context->getPrecision(), 6);
}

TEST_F(EvaluationContextTest, SetPrecision) {
    context->setPrecision(10);
    EXPECT_EQ(context->getPrecision(), 10);
}

TEST_F(EvaluationContextTest, SetPrecisionNegativeThrows) {
    EXPECT_THROW(context->setPrecision(-1), std::invalid_argument);
}

TEST_F(EvaluationContextTest, HasBuiltInFunction) {
    EXPECT_TRUE(context->hasFunction("sin"));
    EXPECT_TRUE(context->hasFunction("cos"));
    EXPECT_TRUE(context->hasFunction("sqrt"));
    EXPECT_FALSE(context->hasFunction("unknown"));
}

TEST_F(EvaluationContextTest, CallBuiltInFunction) {
    EvaluationResult result = context->callFunction("sin", {M_PI / 2});

    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(EvaluationContextTest, CallUnknownFunction) {
    EvaluationResult result = context->callFunction("unknown", {1.0});

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::INVALID_FUNCTION);
}

TEST_F(EvaluationContextTest, CallFunctionWrongArguments) {
    EvaluationResult result = context->callFunction("sin", {1.0, 2.0});

    EXPECT_TRUE(result.isError());
}

TEST_F(EvaluationContextTest, AddCustomFunction) {
    bool called = false;
    context->addFunction("test", [&](const std::vector<double>& args) -> double {
        called = true;
        EXPECT_EQ(args.size(), 1);
        return args[0] * 2;
    });

    EXPECT_TRUE(context->hasFunction("test"));

    EvaluationResult result = context->callFunction("test", {21.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
    EXPECT_TRUE(called);
}

//=============================================================================
// EvaluatorVisitor Tests
//=============================================================================

class EvaluatorVisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EvaluationContext>();
        MathFunctions::registerBuiltInFunctions(*context);
        evaluator = std::make_unique<EvaluatorVisitor>();
    }

    void TearDown() override {}

    std::unique_ptr<EvaluationContext> context;
    std::unique_ptr<EvaluatorVisitor> evaluator;
};

TEST_F(EvaluatorVisitorTest, EvaluateLiteral) {
    LiteralNode node(42.0);

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateLiteralNegative) {
    LiteralNode node(-3.14);

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -3.14);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinaryAddition) {
    Token plus(TokenType::OPERATOR, "+", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(2.0), plus,
                      std::make_unique<LiteralNode>(3.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinarySubtraction) {
    Token minus(TokenType::OPERATOR, "-", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(5.0), minus,
                      std::make_unique<LiteralNode>(3.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinaryMultiplication) {
    Token mult(TokenType::OPERATOR, "*", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(4.0), mult,
                      std::make_unique<LiteralNode>(5.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinaryDivision) {
    Token div(TokenType::OPERATOR, "/", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(20.0), div,
                      std::make_unique<LiteralNode>(4.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinaryPower) {
    Token pow(TokenType::OPERATOR, "^", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(2.0), pow,
                      std::make_unique<LiteralNode>(10.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 1024.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateBinaryModulo) {
    Token mod(TokenType::OPERATOR, "%", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(17.0), mod,
                      std::make_unique<LiteralNode>(5.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnaryPlus) {
    Token plus(TokenType::OPERATOR, "+", 0);
    UnaryOpNode node(plus, std::make_unique<LiteralNode>(42.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnaryMinus) {
    Token minus(TokenType::OPERATOR, "-", 0);
    UnaryOpNode node(minus, std::make_unique<LiteralNode>(42.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -42.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnaryBitwiseNot) {
    Token tilde(TokenType::OPERATOR, "~", 0);
    UnaryOpNode node(tilde, std::make_unique<LiteralNode>(5.0));  // 5 = 101, ~5 = -6

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -6.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateFunctionCall) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(M_PI / 2));
    FunctionCallNode node("sin", 0, std::move(args));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(EvaluatorVisitorTest, EvaluateFunctionCallMultipleArgs) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(3.0));
    args.push_back(std::make_unique<LiteralNode>(4.0));
    FunctionCallNode node("hypot", 0, std::move(args));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(EvaluatorVisitorTest, EvaluateComplexExpression) {
    // (2 + 3) * 4 = 20
    Token plus(TokenType::OPERATOR, "+", 0);
    Token mult(TokenType::OPERATOR, "*", 0);

    auto left = std::make_unique<BinaryOpNode>(
        std::make_unique<LiteralNode>(2.0), plus,
        std::make_unique<LiteralNode>(3.0));
    BinaryOpNode node(std::move(left), mult, std::make_unique<LiteralNode>(4.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateNestedFunctionCalls) {
    // sqrt(abs(-16)) = 4
    std::vector<std::unique_ptr<ASTNode>> innerArgs;
    innerArgs.push_back(std::make_unique<LiteralNode>(-16.0));
    auto innerCall = std::make_unique<FunctionCallNode>("abs", 0, std::move(innerArgs));

    std::vector<std::unique_ptr<ASTNode>> outerArgs;
    outerArgs.push_back(std::move(innerCall));
    FunctionCallNode node("sqrt", 0, std::move(outerArgs));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(EvaluatorVisitorTest, EvaluateDivisionByZero) {
    Token div(TokenType::OPERATOR, "/", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(10.0), div,
                      std::make_unique<LiteralNode>(0.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(EvaluatorVisitorTest, EvaluateModuloByZero) {
    Token mod(TokenType::OPERATOR, "%", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(10.0), mod,
                      std::make_unique<LiteralNode>(0.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnknownBinaryOperator) {
    Token unknown(TokenType::OPERATOR, "@", 0);
    BinaryOpNode node(std::make_unique<LiteralNode>(2.0), unknown,
                      std::make_unique<LiteralNode>(3.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::EVALUATION_ERROR);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnknownUnaryOperator) {
    Token unknown(TokenType::OPERATOR, "@", 0);
    UnaryOpNode node(unknown, std::make_unique<LiteralNode>(42.0));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::EVALUATION_ERROR);
}

TEST_F(EvaluatorVisitorTest, EvaluateUnknownFunction) {
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(1.0));
    FunctionCallNode node("unknown", 0, std::move(args));

    EvaluationResult result = evaluator->evaluate(&node, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::INVALID_FUNCTION);
}

TEST_F(EvaluatorVisitorTest, EvaluateNullNode) {
    EvaluationResult result = evaluator->evaluate(nullptr, *context);

    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::EVALUATION_ERROR);
}

TEST_F(EvaluatorVisitorTest, GetResultBeforeEvaluation) {
    // Create a fresh evaluator
    EvaluatorVisitor fresh;

    // The result should be the default value (0.0)
    EXPECT_NO_THROW(fresh.getResult());
    EXPECT_TRUE(fresh.getResult().isSuccess());
    EXPECT_DOUBLE_EQ(fresh.getResult().getValue(), 0.0);
}

TEST_F(EvaluatorVisitorTest, ResetEvaluator) {
    LiteralNode node(42.0);
    evaluator->evaluate(&node, *context);
    EXPECT_DOUBLE_EQ(evaluator->getResult().getValue(), 42.0);

    evaluator->reset();
    EXPECT_DOUBLE_EQ(evaluator->getResult().getValue(), 0.0);
}

//=============================================================================
// MathFunctions Tests
//=============================================================================

class MathFunctionsTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EvaluationContext>();
        MathFunctions::registerBuiltInFunctions(*context);
    }

    void TearDown() override {}

    std::unique_ptr<EvaluationContext> context;
};

TEST_F(MathFunctionsTest, TrigonometricSin) {
    EvaluationResult result = context->callFunction("sin", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);

    result = context->callFunction("sin", {M_PI / 2});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(MathFunctionsTest, TrigonometricCos) {
    EvaluationResult result = context->callFunction("cos", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);

    result = context->callFunction("cos", {M_PI});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -1.0, 1e-10);
}

TEST_F(MathFunctionsTest, TrigonometricTan) {
    EvaluationResult result = context->callFunction("tan", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);

    result = context->callFunction("tan", {M_PI / 4});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(MathFunctionsTest, TrigonometricAsin) {
    EvaluationResult result = context->callFunction("asin", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);

    result = context->callFunction("asin", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_PI / 2, 1e-10);
}

TEST_F(MathFunctionsTest, TrigonometricAcos) {
    EvaluationResult result = context->callFunction("acos", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);

    result = context->callFunction("acos", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_PI / 2, 1e-10);
}

TEST_F(MathFunctionsTest, TrigonometricAtan) {
    EvaluationResult result = context->callFunction("atan", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);

    result = context->callFunction("atan", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_PI / 4, 1e-10);
}

TEST_F(MathFunctionsTest, HyperbolicSinh) {
    EvaluationResult result = context->callFunction("sinh", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(MathFunctionsTest, HyperbolicCosh) {
    EvaluationResult result = context->callFunction("cosh", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(MathFunctionsTest, HyperbolicTanh) {
    EvaluationResult result = context->callFunction("tanh", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(MathFunctionsTest, LogarithmicLog) {
    EvaluationResult result = context->callFunction("log", {M_E});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);

    result = context->callFunction("log", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(MathFunctionsTest, LogarithmicLogNegative) {
    EvaluationResult result = context->callFunction("log", {-1.0});
    EXPECT_TRUE(result.isError());
}

TEST_F(MathFunctionsTest, LogarithmicLog10) {
    EvaluationResult result = context->callFunction("log10", {10.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);

    result = context->callFunction("log10", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.0, 1e-10);
}

TEST_F(MathFunctionsTest, ExponentialExp) {
    EvaluationResult result = context->callFunction("exp", {0.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);

    result = context->callFunction("exp", {1.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_E, 1e-10);
}

TEST_F(MathFunctionsTest, ExponentialSqrt) {
    EvaluationResult result = context->callFunction("sqrt", {4.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.0, 1e-10);

    result = context->callFunction("sqrt", {16.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 4.0, 1e-10);
}

TEST_F(MathFunctionsTest, ExponentialSqrtNegative) {
    EvaluationResult result = context->callFunction("sqrt", {-1.0});
    EXPECT_TRUE(result.isError());
}

TEST_F(MathFunctionsTest, ExponentialCbrt) {
    EvaluationResult result = context->callFunction("cbrt", {8.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 2.0, 1e-10);

    result = context->callFunction("cbrt", {-8.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -2.0, 1e-10);
}

TEST_F(MathFunctionsTest, ExponentialPow) {
    EvaluationResult result = context->callFunction("pow", {2.0, 3.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 8.0, 1e-10);

    result = context->callFunction("pow", {5.0, 2.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 25.0, 1e-10);
}

TEST_F(MathFunctionsTest, RoundingAbs) {
    EvaluationResult result = context->callFunction("abs", {5.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);

    result = context->callFunction("abs", {-5.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(MathFunctionsTest, RoundingFloor) {
    EvaluationResult result = context->callFunction("floor", {3.7});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);

    result = context->callFunction("floor", {-3.7});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -4.0, 1e-10);
}

TEST_F(MathFunctionsTest, RoundingCeil) {
    EvaluationResult result = context->callFunction("ceil", {3.2});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 4.0, 1e-10);

    result = context->callFunction("ceil", {-3.2});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -3.0, 1e-10);
}

TEST_F(MathFunctionsTest, RoundingRound) {
    EvaluationResult result = context->callFunction("round", {3.4});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);

    result = context->callFunction("round", {3.6});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 4.0, 1e-10);
}

TEST_F(MathFunctionsTest, RoundingTrunc) {
    EvaluationResult result = context->callFunction("trunc", {3.7});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);

    result = context->callFunction("trunc", {-3.7});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -3.0, 1e-10);
}

TEST_F(MathFunctionsTest, OtherFmod) {
    EvaluationResult result = context->callFunction("fmod", {10.0, 3.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(MathFunctionsTest, OtherMax) {
    EvaluationResult result = context->callFunction("max", {3.0, 5.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);

    result = context->callFunction("max", {5.0, 3.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(MathFunctionsTest, OtherMin) {
    EvaluationResult result = context->callFunction("min", {3.0, 5.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);

    result = context->callFunction("min", {5.0, 3.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 3.0, 1e-10);
}

TEST_F(MathFunctionsTest, OtherHypot) {
    EvaluationResult result = context->callFunction("hypot", {3.0, 4.0});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(MathFunctionsTest, ConstantsPI) {
    EvaluationResult result = context->callFunction("PI", {});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_PI, 1e-10);
}

TEST_F(MathFunctionsTest, ConstantsE) {
    EvaluationResult result = context->callFunction("E", {});
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), M_E, 1e-10);
}

//=============================================================================
// End-to-End Evaluation Tests
//=============================================================================

class EndToEndEvaluationTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EvaluationContext>();
        MathFunctions::registerBuiltInFunctions(*context);
        evaluator = std::make_unique<EvaluatorVisitor>();
        parser = std::make_unique<RecursiveDescentParser>();
    }

    std::unique_ptr<EvaluationContext> context;
    std::unique_ptr<EvaluatorVisitor> evaluator;
    std::unique_ptr<RecursiveDescentParser> parser;

    EvaluationResult evaluateExpression(const std::string& expr) {
        try {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            auto ast = parser->parse(tokens);
            return evaluator->evaluate(ast.get(), *context);
        } catch (const CalculatorException& e) {
            return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
        }
    }
};

TEST_F(EndToEndEvaluationTest, SimpleAddition) {
    EvaluationResult result = evaluateExpression("2 + 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EndToEndEvaluationTest, SimpleSubtraction) {
    EvaluationResult result = evaluateExpression("5 - 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(EndToEndEvaluationTest, SimpleMultiplication) {
    EvaluationResult result = evaluateExpression("4 * 5");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(EndToEndEvaluationTest, SimpleDivision) {
    EvaluationResult result = evaluateExpression("20 / 4");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EndToEndEvaluationTest, OperatorPrecedence) {
    EvaluationResult result = evaluateExpression("2 + 3 * 4");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 14.0);  // 2 + (3 * 4)
}

TEST_F(EndToEndEvaluationTest, Parentheses) {
    EvaluationResult result = evaluateExpression("(2 + 3) * 4");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 20.0);
}

TEST_F(EndToEndEvaluationTest, PowerPrecedence) {
    EvaluationResult result = evaluateExpression("2 ^ 3 ^ 2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 512.0);  // 2 ^ (3 ^ 2)
}

TEST_F(EndToEndEvaluationTest, NegativeNumber) {
    EvaluationResult result = evaluateExpression("-5 + 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), -2.0);
}

TEST_F(EndToEndEvaluationTest, FunctionCall) {
    EvaluationResult result = evaluateExpression("sin(PI / 2)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(EndToEndEvaluationTest, NestedFunctionCall) {
    EvaluationResult result = evaluateExpression("sqrt(abs(-16))");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(EndToEndEvaluationTest, ComplexExpression) {
    EvaluationResult result = evaluateExpression("2 + 3 * (4 - 1) / 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EndToEndEvaluationTest, DivisionByZeroInExpression) {
    EvaluationResult result = evaluateExpression("10 / 0");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(EndToEndEvaluationTest, ScientificNotation) {
    EvaluationResult result = evaluateExpression("1.5e2 + 0.5e2");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 200.0);
}

TEST_F(EndToEndEvaluationTest, ModuloOperation) {
    EvaluationResult result = evaluateExpression("17 % 5");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 2.0);
}

TEST_F(EndToEndEvaluationTest, MultipleFunctionCalls) {
    EvaluationResult result = evaluateExpression("sin(0) + cos(0)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(EndToEndEvaluationTest, LogarithmicFunction) {
    EvaluationResult result = evaluateExpression("log(E)");
    EXPECT_TRUE(result.isSuccess());
    // Allow more tolerance since E is converted from string
    EXPECT_NEAR(result.getValue(), 1.0, 1e-6);
}

TEST_F(EndToEndEvaluationTest, SquareRootFunction) {
    EvaluationResult result = evaluateExpression("sqrt(16)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(EndToEndEvaluationTest, AbsoluteValueFunction) {
    EvaluationResult result = evaluateExpression("abs(-42)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
}

TEST_F(EndToEndEvaluationTest, RoundingFunctions) {
    EvaluationResult result = evaluateExpression("floor(3.7)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);

    result = evaluateExpression("ceil(3.2)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);

    result = evaluateExpression("round(3.5)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 4.0);
}

TEST_F(EndToEndEvaluationTest, VeryComplexExpression) {
    EvaluationResult result = evaluateExpression("sqrt(16) + pow(2, 3) * sin(PI/2) - abs(-2)");
    EXPECT_TRUE(result.isSuccess());
    // sqrt(16) = 4, pow(2, 3) = 8, sin(PI/2) ≈ 1, abs(-2) = 2
    // Result: 4 + 8 * 1 - 2 = 10
    EXPECT_NEAR(result.getValue(), 10.0, 1e-9);
}

TEST_F(EndToEndEvaluationTest, UnaryPlusInExpression) {
    EvaluationResult result = evaluateExpression("+5");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EndToEndEvaluationTest, NegativeDecimal) {
    EvaluationResult result = evaluateExpression("-3.14");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), -3.14, 1e-10);
}

TEST_F(EndToEndEvaluationTest, ZeroResult) {
    EvaluationResult result = evaluateExpression("5 - 5");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 0.0);
}

TEST_F(EndToEndEvaluationTest, LargeNumbers) {
    EvaluationResult result = evaluateExpression("1000000 * 1000000");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 1e12);
}

TEST_F(EndToEndEvaluationTest, DecimalPrecision) {
    EvaluationResult result = evaluateExpression("1 / 3");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 0.33333333333333, 1e-10);
}

TEST_F(EndToEndEvaluationTest, MaxFunction) {
    EvaluationResult result = evaluateExpression("max(5, 3)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST_F(EndToEndEvaluationTest, MinFunction) {
    EvaluationResult result = evaluateExpression("min(5, 3)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(EndToEndEvaluationTest, HypotFunction) {
    EvaluationResult result = evaluateExpression("hypot(3, 4)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 5.0, 1e-10);
}

TEST_F(EndToEndEvaluationTest, CbrtFunction) {
    EvaluationResult result = evaluateExpression("cbrt(27)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 3.0);
}

TEST_F(EndToEndEvaluationTest, RemainderFunction) {
    EvaluationResult result = evaluateExpression("remainder(10, 3)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 1.0, 1e-10);
}

TEST_F(EndToEndEvaluationTest, Tan45Degrees) {
    EvaluationResult result = evaluateExpression("tan(PI/4)");
    EXPECT_TRUE(result.isSuccess());
    // Allow more tolerance for floating point errors from PI string conversion
    EXPECT_NEAR(result.getValue(), 1.0, 1e-6);
}

TEST_F(EndToEndEvaluationTest, Sin60Degrees) {
    EvaluationResult result = evaluateExpression("sin(PI/3)");
    EXPECT_TRUE(result.isSuccess());
    // Allow more tolerance for floating point errors from PI string conversion
    EXPECT_NEAR(result.getValue(), 0.86602540378444, 1e-6);
}

TEST_F(EndToEndEvaluationTest, Cos60Degrees) {
    EvaluationResult result = evaluateExpression("cos(PI/3)");
    EXPECT_TRUE(result.isSuccess());
    // Allow more tolerance for floating point errors from PI string conversion
    EXPECT_NEAR(result.getValue(), 0.5, 1e-6);
}

TEST_F(EndToEndEvaluationTest, AsinOfSin) {
    EvaluationResult result = evaluateExpression("asin(sin(PI/4))");
    EXPECT_TRUE(result.isSuccess());
    // Allow more tolerance for floating point errors from PI string conversion
    EXPECT_NEAR(result.getValue(), M_PI / 4, 1e-7);
}

TEST_F(EndToEndEvaluationTest, AexpOfLog) {
    EvaluationResult result = evaluateExpression("exp(log(10))");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_NEAR(result.getValue(), 10.0, 1e-9);
}

TEST_F(EndToEndEvaluationTest, PowOfPow) {
    EvaluationResult result = evaluateExpression("pow(pow(2, 2), 2)");
    EXPECT_TRUE(result.isSuccess());
    EXPECT_DOUBLE_EQ(result.getValue(), 16.0);
}
