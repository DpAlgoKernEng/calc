/**
 * @file parser_benchmark.cpp
 * @brief Performance benchmarks for the parsers
 */

#include "calc/core/tokenizer.h"
#include "calc/core/shunting_yard_parser.h"
#include "calc/core/recursive_descent_parser.h"
#include "calc/core/ast.h"
#include "calc/benchmark/benchmark.h"

#include <string>
#include <vector>

using namespace calc;
using namespace calc::benchmark;

// Test expressions of varying complexity
static const std::vector<std::string> SIMPLE_EXPRESSIONS = {
    "1 + 2",
    "3.14 * 2",
    "x / y",
    "10 - 5",
    "100 + 200"
};

static const std::vector<std::string> MEDIUM_EXPRESSIONS = {
    "1 + 2 * 3 - 4 / 5",
    "(1 + 2) * (3 - 4)",
    "sin(x) + cos(y)",
    "pow(2, 10) + sqrt(100)",
    "log(100) / log(10)"
};

static const std::vector<std::string> COMPLEX_EXPRESSIONS = {
    "sin(cos(tan(x)))",
    "pow(sqrt(pow(x, 2) + pow(y, 2)), 0.5)",
    "log10(exp(x) + exp(y)) + log(x * y)",
    "(1 + 2 * 3 - 4 / 5 + 6 ^ 7) * (8 - 9)",
    "sin(2 * PI * x) + cos(2 * PI * y) + tan(2 * PI * z)"
};

static const std::vector<std::string> NESTED_EXPRESSIONS = {
    "(((((1)))))*(((((2)))))",
    "sin(cos(tan(asin(acos(atan(x)))))",
    "pow(pow(pow(pow(x, 2), 3), 4), 5)",
    "log(log(log(log(log(x)))))",
    "((((((1 + 2) * 3) - 4) / 5) ^ 6) + 7)"
};

static const std::vector<std::string> FUNCTION_CHAIN_EXPRESSIONS = {
    "sin(cos(tan(x)))",
    "abs(round(floor(ceil(sqrt(100))))",
    "asin(acos(atan(sinh(cosh(tanh(x)))))",
    "log10(log(exp(pow(x, 2))))",
    "max(min(x, 100), abs(x))"
};

void benchmark_shunting_yard_simple() {
    Benchmark b("ShuntingYardParser - Simple Expressions");
    b.print_result(b.run([&] {
        ShuntingYardParser parser;
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_recursive_descent_simple() {
    Benchmark b("RecursiveDescentParser - Simple Expressions");
    b.print_result(b.run([&] {
        RecursiveDescentParser parser;
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_shunting_yard_medium() {
    Benchmark b("ShuntingYardParser - Medium Expressions");
    b.print_result(b.run([&] {
        ShuntingYardParser parser;
        for (const auto& expr : MEDIUM_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_recursive_descent_medium() {
    Benchmark b("RecursiveDescentParser - Medium Expressions");
    b.print_result(b.run([&] {
        RecursiveDescentParser parser;
        for (const auto& expr : MEDIUM_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_shunting_yard_complex() {
    Benchmark b("ShuntingYardParser - Complex Expressions");
    b.print_result(b.run([&] {
        ShuntingYardParser parser;
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_recursive_descent_complex() {
    Benchmark b("RecursiveDescentParser - Complex Expressions");
    b.print_result(b.run([&] {
        RecursiveDescentParser parser;
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_shunting_yard_nested() {
    Benchmark b("ShuntingYardParser - Nested Expressions");
    b.print_result(b.run([&] {
        ShuntingYardParser parser;
        for (const auto& expr : NESTED_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_recursive_descent_nested() {
    Benchmark b("RecursiveDescentParser - Nested Expressions");
    b.print_result(b.run([&] {
        RecursiveDescentParser parser;
        for (const auto& expr : NESTED_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }));
}

void benchmark_parser_comparison_simple() {
    Benchmark b("Parser Comparison - Simple Expressions");
    b.compare("ShuntingYard", [&] {
        ShuntingYardParser parser;
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }, "RecursiveDescent", [&] {
        RecursiveDescentParser parser;
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    });
}

void benchmark_parser_comparison_complex() {
    Benchmark b("Parser Comparison - Complex Expressions");
    b.compare("ShuntingYard", [&] {
        ShuntingYardParser parser;
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }, "RecursiveDescent", [&] {
        RecursiveDescentParser parser;
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    });
}

void benchmark_parser_comparison_nested() {
    Benchmark b("Parser Comparison - Nested Expressions");
    b.compare("ShuntingYard", [&] {
        ShuntingYardParser parser;
        for (const auto& expr : NESTED_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }, "RecursiveDescent", [&] {
        RecursiveDescentParser parser;
        for (const auto& expr : NESTED_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    });
}

void benchmark_function_calls() {
    Benchmark b("Parser Comparison - Function Chains");
    b.compare("ShuntingYard", [&] {
        ShuntingYardParser parser;
        for (const auto& expr : FUNCTION_CHAIN_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    }, "RecursiveDescent", [&] {
        RecursiveDescentParser parser;
        for (const auto& expr : FUNCTION_CHAIN_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            auto tokens = tokenizer.tokenize();
            (void)parser.parse(tokens);
        }
    });
}

void benchmark_single_complex_expression() {
    std::string complex_expr;
    for (int i = 0; i < 100; ++i) {
        complex_expr += "(sin(" + std::to_string(i) + ")+cos(" + std::to_string(i+1) + "))*";
    }
    complex_expr.back() = ')';  // Replace last '*'

    Benchmark b("Parser - Single 100-term Complex Expression");
    b.compare("ShuntingYard", [&] {
        ShuntingYardParser parser;
        Tokenizer tokenizer(complex_expr);
        auto tokens = tokenizer.tokenize();
        (void)parser.parse(tokens);
    }, "RecursiveDescent", [&] {
        RecursiveDescentParser parser;
        Tokenizer tokenizer(complex_expr);
        auto tokens = tokenizer.tokenize();
        (void)parser.parse(tokens);
    });
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "========================================\n";
    std::cout << "Parser Performance Benchmarks\n";
    std::cout << "========================================\n\n";

    benchmark_shunting_yard_simple();
    benchmark_recursive_descent_simple();
    benchmark_shunting_yard_medium();
    benchmark_recursive_descent_medium();
    benchmark_shunting_yard_complex();
    benchmark_recursive_descent_complex();
    benchmark_shunting_yard_nested();
    benchmark_recursive_descent_nested();

    std::cout << "\n========================================\n";
    std::cout << "Parser Comparisons\n";
    std::cout << "========================================\n\n";

    benchmark_parser_comparison_simple();
    benchmark_parser_comparison_complex();
    benchmark_parser_comparison_nested();
    benchmark_function_calls();
    benchmark_single_complex_expression();

    std::cout << "========================================\n";
    std::cout << "All parser benchmarks completed!\n";
    std::cout << "========================================\n";

    return 0;
}
