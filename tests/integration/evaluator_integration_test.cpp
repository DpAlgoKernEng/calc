/**
 * @file evaluator_integration_test.cpp
 * @brief Integration tests for the full expression evaluation pipeline
 *
 * These tests verify end-to-end functionality:
 * 1. Tokenization (string -> tokens)
 * 2. Parsing (tokens -> AST)
 * 3. Evaluation (AST -> result)
 */

#include <gtest/gtest.h>
#include "calc/core/tokenizer.h"
#include "calc/core/shunting_yard_parser.h"
#include "calc/core/recursive_descent_parser.h"
#include "calc/core/evaluator.h"
#include <cmath>

using namespace calc;

/**
 * @brief Fixture for integration testing
 *
 * Tests the complete pipeline: Tokenizer -> Parser -> Evaluator
 */
class EvaluatorIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EvaluationContext>();
        MathFunctions::registerBuiltInFunctions(*context);
        evaluator = std::make_unique<EvaluatorVisitor>();
        shuntingYardParser = std::make_unique<ShuntingYardParser>();
        recursiveDescentParser = std::make_unique<RecursiveDescentParser>();
    }

    void TearDown() override {}

    /**
     * @brief Evaluate an expression using the Shunting-Yard parser
     */
    EvaluationResult evaluateWithShuntingYard(const std::string& expr) {
        try {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            auto ast = shuntingYardParser->parse(tokens);
            return evaluator->evaluate(ast.get(), *context);
        } catch (const CalculatorException& e) {
            return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
        }
    }

    /**
     * @brief Evaluate an expression using the Recursive Descent parser
     */
    EvaluationResult evaluateWithRecursiveDescent(const std::string& expr) {
        try {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            auto ast = recursiveDescentParser->parse(tokens);
            return evaluator->evaluate(ast.get(), *context);
        } catch (const CalculatorException& e) {
            return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
        }
    }

    /**
     * @brief Evaluate with both parsers and verify they produce same result
     */
    void evaluateWithBoth(const std::string& expr, double expected,
                          double tolerance = 1e-10) {
        EvaluationResult syResult = evaluateWithShuntingYard(expr);
        EvaluationResult rdResult = evaluateWithRecursiveDescent(expr);

        EXPECT_TRUE(syResult.isSuccess()) << "Shunting-Yard: " << syResult.getErrorMessage();
        EXPECT_TRUE(rdResult.isSuccess()) << "Recursive Descent: " << rdResult.getErrorMessage();

        EXPECT_NEAR(syResult.getValue(), expected, tolerance) << expr;
        EXPECT_NEAR(rdResult.getValue(), expected, tolerance) << expr;
        EXPECT_NEAR(syResult.getValue(), rdResult.getValue(), tolerance) << expr;
    }

    std::unique_ptr<EvaluationContext> context;
    std::unique_ptr<EvaluatorVisitor> evaluator;
    std::unique_ptr<ShuntingYardParser> shuntingYardParser;
    std::unique_ptr<RecursiveDescentParser> recursiveDescentParser;
};

//=============================================================================
// Basic Arithmetic Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, SimpleAddition) {
    evaluateWithBoth("2 + 3", 5.0);
}

TEST_F(EvaluatorIntegrationTest, SimpleSubtraction) {
    evaluateWithBoth("5 - 3", 2.0);
}

TEST_F(EvaluatorIntegrationTest, SimpleMultiplication) {
    evaluateWithBoth("4 * 5", 20.0);
}

TEST_F(EvaluatorIntegrationTest, SimpleDivision) {
    evaluateWithBoth("20 / 4", 5.0);
}

TEST_F(EvaluatorIntegrationTest, MultipleOperations) {
    evaluateWithBoth("2 + 3 + 4 + 5", 14.0);
}

TEST_F(EvaluatorIntegrationTest, OperatorPrecedence_MultiplyBeforeAdd) {
    evaluateWithBoth("2 + 3 * 4", 14.0);  // 2 + (3 * 4)
}

TEST_F(EvaluatorIntegrationTest, OperatorPrecedence_DivideBeforeSubtract) {
    evaluateWithBoth("10 - 8 / 2", 6.0);  // 10 - (8 / 2)
}

TEST_F(EvaluatorIntegrationTest, OperatorPrecedence_MultiplyDivide) {
    evaluateWithBoth("10 * 5 / 2", 25.0);  // (10 * 5) / 2 (left to right)
}

TEST_F(EvaluatorIntegrationTest, OperatorPrecedence_AddSubtract) {
    evaluateWithBoth("10 - 5 + 3", 8.0);  // (10 - 5) + 3 (left to right)
}

TEST_F(EvaluatorIntegrationTest, Parentheses_OverridePrecedence) {
    evaluateWithBoth("(2 + 3) * 4", 20.0);
}

TEST_F(EvaluatorIntegrationTest, Parentheses_Nested) {
    evaluateWithBoth("((2 + 3) * (4 - 1)) / 5", 3.0);
}

TEST_F(EvaluatorIntegrationTest, NegativeNumbers) {
    evaluateWithBoth("-5 + 3", -2.0);
    evaluateWithBoth("5 + -3", 2.0);
    evaluateWithBoth("-5 - -3", -2.0);
}

TEST_F(EvaluatorIntegrationTest, DecimalNumbers) {
    evaluateWithBoth("3.14 + 2.86", 6.0);
    evaluateWithBoth("0.5 * 0.5", 0.25);
}

TEST_F(EvaluatorIntegrationTest, ScientificNotation) {
    evaluateWithBoth("1e2 + 2e1", 120.0);
    evaluateWithBoth("1.5e2 + 0.5e2", 200.0);
    evaluateWithBoth("2.5e-1 * 2", 0.5);
}

TEST_F(EvaluatorIntegrationTest, ZeroOperations) {
    evaluateWithBoth("0 + 0", 0.0);
    evaluateWithBoth("0 * 5", 0.0);
    evaluateWithBoth("5 * 0", 0.0);
}

TEST_F(EvaluatorIntegrationTest, IdentityOperations) {
    evaluateWithBoth("5 * 1", 5.0);
    evaluateWithBoth("5 / 1", 5.0);
    evaluateWithBoth("0 + 5", 5.0);
}

//=============================================================================
// Power Operation Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, PowerOperation) {
    evaluateWithBoth("2 ^ 3", 8.0);
    evaluateWithBoth("3 ^ 2", 9.0);
    evaluateWithBoth("5 ^ 0", 1.0);
}

TEST_F(EvaluatorIntegrationTest, PowerWithNegativeExponent) {
    evaluateWithBoth("2 ^ -2", 0.25, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, PowerWithFractionalExponent) {
    evaluateWithBoth("4 ^ 0.5", 2.0);
    evaluateWithBoth("27 ^ (1/3)", 3.0, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, PowerPrecedence_HigherThanMultiply) {
    evaluateWithBoth("2 * 3 ^ 2", 18.0);  // 2 * (3 ^ 2)
}

TEST_F(EvaluatorIntegrationTest, PowerRightAssociative) {
    evaluateWithBoth("2 ^ 3 ^ 2", 512.0);  // 2 ^ (3 ^ 2)
}

//=============================================================================
// Modulo Operation Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, ModuloOperation) {
    evaluateWithBoth("17 % 5", 2.0);
    evaluateWithBoth("10 % 3", 1.0);
    evaluateWithBoth("20 % 4", 0.0);
}

TEST_F(EvaluatorIntegrationTest, ModuloWithPrecedence) {
    evaluateWithBoth("10 + 7 % 5", 12.0);  // 10 + (7 % 5)
}

TEST_F(EvaluatorIntegrationTest, ModuloDivisionByZero) {
    EvaluationResult syResult = evaluateWithShuntingYard("10 % 0");
    EXPECT_TRUE(syResult.isError());
    EXPECT_EQ(syResult.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);

    EvaluationResult rdResult = evaluateWithRecursiveDescent("10 % 0");
    EXPECT_TRUE(rdResult.isError());
    EXPECT_EQ(rdResult.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

//=============================================================================
// Function Call Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, BasicFunctionCall) {
    evaluateWithBoth("sin(0)", 0.0, 1e-10);
    evaluateWithBoth("cos(0)", 1.0, 1e-10);
    evaluateWithBoth("abs(-5)", 5.0);
}

TEST_F(EvaluatorIntegrationTest, FunctionWithArithmetic) {
    evaluateWithBoth("sin(PI/2)", 1.0, 1e-7);
    evaluateWithBoth("2 * sin(PI/6)", 1.0, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, NestedFunctionCalls) {
    evaluateWithBoth("sqrt(abs(-16))", 4.0);
    evaluateWithBoth("abs(sin(PI/2))", 1.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, FunctionInExpression) {
    evaluateWithBoth("sqrt(16) + 4", 8.0);
    evaluateWithBoth("2 * abs(-3) + 1", 7.0);
}

TEST_F(EvaluatorIntegrationTest, FunctionWithMultipleArguments) {
    evaluateWithBoth("hypot(3, 4)", 5.0, 1e-10);
    evaluateWithBoth("max(5, 3)", 5.0);
    evaluateWithBoth("min(5, 3)", 3.0);
}

TEST_F(EvaluatorIntegrationTest, LogarithmicFunctions) {
    evaluateWithBoth("log(E)", 1.0, 1e-7);
    evaluateWithBoth("log10(10)", 1.0, 1e-10);
    evaluateWithBoth("exp(1)", M_E, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, ExponentialFunctions) {
    evaluateWithBoth("sqrt(16)", 4.0);
    evaluateWithBoth("cbrt(27)", 3.0);
    evaluateWithBoth("cbrt(-8)", -2.0);
}

TEST_F(EvaluatorIntegrationTest, RoundingFunctions) {
    evaluateWithBoth("floor(3.7)", 3.0);
    evaluateWithBoth("ceil(3.2)", 4.0);
    evaluateWithBoth("round(3.5)", 4.0);
    evaluateWithBoth("trunc(3.7)", 3.0);
}

TEST_F(EvaluatorIntegrationTest, HyperbolicFunctions) {
    evaluateWithBoth("sinh(0)", 0.0, 1e-10);
    evaluateWithBoth("cosh(0)", 1.0, 1e-10);
    evaluateWithBoth("tanh(0)", 0.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, InverseTrigonometricFunctions) {
    evaluateWithBoth("asin(0)", 0.0, 1e-10);
    evaluateWithBoth("acos(1)", 0.0, 1e-10);
    evaluateWithBoth("atan(0)", 0.0, 1e-10);
    evaluateWithBoth("asin(sin(PI/4))", M_PI/4, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, Constants) {
    evaluateWithBoth("PI", M_PI, 1e-7);
    evaluateWithBoth("E", M_E, 1e-7);
}

//=============================================================================
// Complex Expression Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, ComplexExpression1) {
    evaluateWithBoth("2 + 3 * (4 - 1) / 3", 5.0);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression2) {
    evaluateWithBoth("(2 + 3) * (4 - 1) / 5", 3.0);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression3) {
    evaluateWithBoth("sqrt(16) + pow(2, 3) * sin(PI/2)", 12.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression4) {
    evaluateWithBoth("abs(-10) + max(5, 3) - min(8, 4)", 11.0);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression5) {
    evaluateWithBoth("log(exp(10))", 10.0, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression6) {
    evaluateWithBoth("2 ^ (1 + 2) * 3 - 1", 23.0);
}

TEST_F(EvaluatorIntegrationTest, ComplexExpression7) {
    evaluateWithBoth("((1 + 2) * 3) / 2", 4.5);
}

TEST_F(EvaluatorIntegrationTest, VeryComplexExpression) {
    // A challenging expression with multiple operations and functions
    evaluateWithBoth("sqrt(abs(-25)) + pow(2, 3) - max(10, 5) * sin(PI/2) + min(3, 7)",
                   6.0, 1e-10);
}

//=============================================================================
// Error Handling Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, DivisionByZero) {
    EvaluationResult syResult = evaluateWithShuntingYard("10 / 0");
    EXPECT_TRUE(syResult.isError());
    EXPECT_EQ(syResult.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);

    EvaluationResult rdResult = evaluateWithRecursiveDescent("10 / 0");
    EXPECT_TRUE(rdResult.isError());
    EXPECT_EQ(rdResult.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(EvaluatorIntegrationTest, DivisionByZeroInComplexExpression) {
    EvaluationResult result = evaluateWithShuntingYard("10 / (5 - 5)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DIVISION_BY_ZERO);
}

TEST_F(EvaluatorIntegrationTest, UnknownFunction) {
    EvaluationResult syResult = evaluateWithShuntingYard("unknown(5)");
    EXPECT_TRUE(syResult.isError());
    EXPECT_EQ(syResult.getErrorCode(), ErrorCode::INVALID_FUNCTION);

    EvaluationResult rdResult = evaluateWithRecursiveDescent("unknown(5)");
    EXPECT_TRUE(rdResult.isError());
    EXPECT_EQ(rdResult.getErrorCode(), ErrorCode::INVALID_FUNCTION);
}

TEST_F(EvaluatorIntegrationTest, SqrtNegative) {
    EvaluationResult result = evaluateWithShuntingYard("sqrt(-1)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(EvaluatorIntegrationTest, LogNegative) {
    EvaluationResult result = evaluateWithShuntingYard("log(-1)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(EvaluatorIntegrationTest, LogZero) {
    EvaluationResult result = evaluateWithShuntingYard("log(0)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(EvaluatorIntegrationTest, AsinOutOfRange) {
    EvaluationResult result = evaluateWithShuntingYard("asin(2)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(EvaluatorIntegrationTest, AcosOutOfRange) {
    EvaluationResult result = evaluateWithShuntingYard("acos(2)");
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getErrorCode(), ErrorCode::DOMAIN_ERROR);
}

TEST_F(EvaluatorIntegrationTest, EmptyExpression) {
    EvaluationResult result = evaluateWithShuntingYard("");
    EXPECT_TRUE(result.isError());
}

TEST_F(EvaluatorIntegrationTest, InvalidSyntax) {
    EvaluationResult result = evaluateWithShuntingYard("2 + * 3");
    EXPECT_TRUE(result.isError());
}

//=============================================================================
// Edge Cases Integration Tests
//=============================================================================

TEST_F(EvaluatorIntegrationTest, VeryLargeNumbers) {
    evaluateWithBoth("1000000 * 1000000", 1e12);
    evaluateWithBoth("10 ^ 10", 1e10);
}

TEST_F(EvaluatorIntegrationTest, VerySmallNumbers) {
    evaluateWithBoth("0.0001 * 0.0001", 1e-8, 1e-12);
}

TEST_F(EvaluatorIntegrationTest, DecimalPrecision) {
    evaluateWithBoth("1 / 3", 1.0 / 3.0, 1e-10);
    evaluateWithBoth("1 / 7", 1.0 / 7.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, UnaryPlusOnPositive) {
    evaluateWithBoth("+5", 5.0);
}

TEST_F(EvaluatorIntegrationTest, UnaryPlusInExpression) {
    evaluateWithBoth("+5 + +3", 8.0);
}

TEST_F(EvaluatorIntegrationTest, MultipleUnaryOperators) {
    evaluateWithBoth("5 - -3", 8.0);
    evaluateWithBoth("-(-5)", 5.0);
}

TEST_F(EvaluatorIntegrationTest, LongChainOfOperations) {
    evaluateWithBoth("1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10", 55.0);
}

TEST_F(EvaluatorIntegrationTest, LongChainOfMultiplications) {
    evaluateWithBoth("2 * 3 * 4 * 5", 120.0);
}

TEST_F(EvaluatorIntegrationTest, AlternatingOperations) {
    evaluateWithBoth("10 - 5 + 3 - 2 + 1", 7.0);
}

TEST_F(EvaluatorIntegrationTest, NestedParenthesesDeep) {
    evaluateWithBoth("((((1 + 2))))", 3.0);
}

TEST_F(EvaluatorIntegrationTest, MultipleFunctionsInExpression) {
    evaluateWithBoth("sin(0) + cos(0) + tan(0)", 1.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, FunctionWithComplexArgument) {
    evaluateWithBoth("sin((PI / 2) + 0)", 1.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, FmodFunction) {
    evaluateWithBoth("fmod(10, 3)", 1.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, RemainderFunction) {
    evaluateWithBoth("remainder(10, 3)", 1.0, 1e-10);
}

TEST_F(EvaluatorIntegrationTest, PowWithFunctionResult) {
    evaluateWithBoth("pow(2, abs(3))", 8.0);
}

TEST_F(EvaluatorIntegrationTest, TrigonometricValues) {
    evaluateWithBoth("sin(PI/2)", 1.0, 1e-7);
    evaluateWithBoth("cos(PI)", -1.0, 1e-7);
    evaluateWithBoth("tan(PI/4)", 1.0, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, Tan90DegreesApproachesInfinity) {
    // tan(PI/2) approaches infinity
    EvaluationResult result = evaluateWithShuntingYard("tan(PI/2)");
    // The result might be very large but not necessarily infinity due to precision
    EXPECT_TRUE(result.isSuccess());
}

//=============================================================================
// Comparison Between Parsers
//=============================================================================

TEST_F(EvaluatorIntegrationTest, BothParsersProduceSameResults_Simple) {
    const std::vector<std::string> expressions = {
        "2 + 3",
        "5 - 3",
        "4 * 5",
        "20 / 4",
        "2 ^ 3",
        "17 % 5"
    };

    for (const auto& expr : expressions) {
        EvaluationResult sy = evaluateWithShuntingYard(expr);
        EvaluationResult rd = evaluateWithRecursiveDescent(expr);

        EXPECT_TRUE(sy.isSuccess()) << "Shunting-Yard failed on: " << expr;
        EXPECT_TRUE(rd.isSuccess()) << "Recursive Descent failed on: " << expr;

        if (sy.isSuccess() && rd.isSuccess()) {
            EXPECT_NEAR(sy.getValue(), rd.getValue(), 1e-10)
                << "Results differ for: " << expr
                << " (SY: " << sy.getValue() << ", RD: " << rd.getValue() << ")";
        }
    }
}

TEST_F(EvaluatorIntegrationTest, BothParsersProduceSameResults_Complex) {
    const std::vector<std::string> expressions = {
        "(2 + 3) * 4",
        "2 + 3 * 4",
        "2 ^ 3 ^ 2",
        "sin(PI/2) + cos(0)",
        "sqrt(16) + 4",
        "abs(-5) * 2 - 1",
        "max(5, 3) + min(8, 4)"
    };

    for (const auto& expr : expressions) {
        EvaluationResult sy = evaluateWithShuntingYard(expr);
        EvaluationResult rd = evaluateWithRecursiveDescent(expr);

        EXPECT_TRUE(sy.isSuccess()) << "Shunting-Yard failed on: " << expr;
        EXPECT_TRUE(rd.isSuccess()) << "Recursive Descent failed on: " << expr;

        if (sy.isSuccess() && rd.isSuccess()) {
            EXPECT_NEAR(sy.getValue(), rd.getValue(), 1e-9)
                << "Results differ for: " << expr
                << " (SY: " << sy.getValue() << ", RD: " << rd.getValue() << ")";
        }
    }
}

//=============================================================================
// Real-world Calculations
//=============================================================================

TEST_F(EvaluatorIntegrationTest, QuadraticFormulaPart1) {
    // Calculate (-b + sqrt(b^2 - 4ac)) / (2a) for a=1, b=-5, c=6
    // Should give 3
    evaluateWithBoth("(-(-5) + sqrt((-5)^2 - 4*1*6)) / (2*1)", 3.0, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, QuadraticFormulaPart2) {
    // Calculate (-b - sqrt(b^2 - 4ac)) / (2a) for a=1, b=-5, c=6
    // Should give 2
    evaluateWithBoth("(-(-5) - sqrt((-5)^2 - 4*1*6)) / (2*1)", 2.0, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, DistanceBetweenPoints) {
    // Distance between (3, 4) and (0, 0)
    // sqrt((3-0)^2 + (4-0)^2) = 5
    evaluateWithBoth("sqrt((3-0)^2 + (4-0)^2)", 5.0, 1e-9);
}

TEST_F(EvaluatorIntegrationTest, AreaOfCircle) {
    // Area of circle with radius 5: PI * r^2
    evaluateWithBoth("PI * 5 ^ 2", M_PI * 25, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, VolumeOfSphere) {
    // Volume of sphere with radius 3: (4/3) * PI * r^3
    evaluateWithBoth("(4/3) * PI * 3 ^ 3", (4.0/3.0) * M_PI * 27, 1e-8);
}

TEST_F(EvaluatorIntegrationTest, CompoundInterest) {
    // Compound interest: P * (1 + r/n)^(n*t)
    // P=1000, r=0.05, n=12, t=1
    evaluateWithBoth("1000 * (1 + 0.05/12) ^ (12*1)", 1000 * std::pow(1 + 0.05/12, 12), 1e-7);
}

TEST_F(EvaluatorIntegrationTest, DegreesToRadians) {
    // Convert 180 degrees to radians: 180 * PI / 180 = PI
    evaluateWithBoth("180 * PI / 180", M_PI, 1e-7);
}

TEST_F(EvaluatorIntegrationTest, PythagoreanTheorem) {
    // c = sqrt(a^2 + b^2) for a=3, b=4 => c=5
    evaluateWithBoth("sqrt(3^2 + 4^2)", 5.0, 1e-9);
}
