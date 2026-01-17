/**
 * @file evaluator_benchmark.cpp
 * @brief Performance benchmarks for the evaluator
 */

#include "calc/core/tokenizer.h"
#include "calc/core/shunting_yard_parser.h"
#include "calc/core/evaluator.h"
#include "calc/modes/standard_mode.h"
#include "calc/modes/scientific_mode.h"
#include "calc/modes/programmer_mode.h"
#include "calc/benchmark/benchmark.h"

#include <string>
#include <vector>

using namespace calc;
using namespace calc::benchmark;

// Test expressions for different operations
static const std::vector<std::string> ARITHMETIC_EXPRESSIONS = {
    "1 + 2",
    "1 + 2 + 3",
    "1 + 2 + 3 + 4 + 5",
    "1 + 2 * 3 - 4 / 5",
    "(1 + 2) * (3 - 4) + (5 * 6)"
};

static const std::vector<std::string> EXPRESSION_EXPRESSIONS = {
    "2 ^ 3",
    "2 ^ 10",
    "2 ^ 20",
    "10 ^ 5",
    "(2 ^ 3) ^ 2"
};

static const std::vector<std::string> TRIG_EXPRESSIONS = {
    "sin(0)",
    "cos(0)",
    "tan(0)",
    "sin(PI/2)",
    "sin(PI/6) + cos(PI/3)"
};

static const std::vector<std::string> LOG_EXPRESSIONS = {
    "log(10)",
    "log(100)",
    "log10(100)",
    "log10(1000)",
    "exp(1) + log(E)"
};

static const std::vector<std::string> BITWISE_EXPRESSIONS = {
    "0xFF & 0x0F",
    "0xF0 | 0x0F",
    "0xFF ^ 0xAA",
    "~0x00FF",
    "(0x10 << 4) >> 2"
};

static const std::vector<std::string> NESTED_EXPRESSIONS = {
    "sin(cos(0))",
    "pow(sqrt(16), 2)",
    "log10(exp(10))",
    "abs(round(3.5))",
    "floor(ceil(3.1))"
};

static const std::vector<std::string> COMPLEX_EXPRESSIONS = {
    "sin(2 * PI * x) + cos(2 * PI * y)",
    "pow(sqrt(x^2 + y^2), 0.5)",
    "log10(exp(x) + exp(y))",
    "(1 + 2 * 3 - 4 / 5 + 6 ^ 7) * (8 - 9)",
    "sin(cos(tan(asin(acos(atan(x)))))"
};

void benchmark_arithmetic_standard_mode() {
    Benchmark b("Evaluator - Arithmetic (Standard Mode)");
    StandardMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : ARITHMETIC_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_power_operations() {
    Benchmark b("Evaluator - Power Operations");
    ScientificMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : EXPRESSION_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_trigonometric_functions() {
    Benchmark b("Evaluator - Trigonometric Functions");
    ScientificMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : TRIG_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_logarithmic_functions() {
    Benchmark b("Evaluator - Logarithmic Functions");
    ScientificMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : LOG_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_bitwise_operations() {
    Benchmark b("Evaluator - Bitwise Operations");
    ProgrammerMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : BITWISE_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_nested_functions() {
    Benchmark b("Evaluator - Nested Function Calls");
    ScientificMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : NESTED_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_complex_expressions() {
    Benchmark b("Evaluator - Complex Expressions");
    ScientificMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_mode_comparison() {
    const std::string expr = "1 + 2 * 3 - 4 / 5";

    Benchmark b("Evaluator - Mode Comparison");
    b.compare("StandardMode", [&] {
        StandardMode mode;
        (void)mode.evaluate(expr);
    }, "ScientificMode", [&] {
        ScientificMode mode;
        (void)mode.evaluate(expr);
    });
}

void benchmark_repeated_evaluation() {
    Benchmark b("Evaluator - Repeated Evaluation (Same Expression)");
    StandardMode mode;
    const std::string expr = "(1 + 2) * (3 - 4) / 5";

    b.print_result(b.run([&] {
        for (int i = 0; i < 100; ++i) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_variable_expressions() {
    Benchmark b("Evaluator - Variable-like Expressions");
    std::vector<std::string> exprs;
    for (int i = 0; i < 100; ++i) {
        exprs.push_back(std::to_string(i) + " + " + std::to_string(i+1));
    }

    StandardMode mode;
    b.print_result(b.run([&] {
        for (const auto& expr : exprs) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_full_pipeline() {
    Benchmark b("Full Pipeline - Tokenize + Parse + Evaluate");
    StandardMode mode;

    b.print_result(b.run([&] {
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_constants() {
    Benchmark b("Evaluator - Constant Evaluations");
    ScientificMode mode;

    std::vector<std::string> const_exprs = {
        "PI",
        "E",
        "PI + E",
        "PI * E",
        "PI / E"
    };

    b.print_result(b.run([&] {
        for (const auto& expr : const_exprs) {
            for (int i = 0; i < 100; ++i) {
                (void)mode.evaluate(expr);
            }
        }
    }));
}

void benchmark_rounding_functions() {
    Benchmark b("Evaluator - Rounding Functions");
    ScientificMode mode;

    std::vector<std::string> rounding_exprs = {
        "round(3.5)",
        "floor(3.9)",
        "ceil(3.1)",
        "trunc(3.7)",
        "abs(-5.5)"
    };

    b.print_result(b.run([&] {
        for (const auto& expr : rounding_exprs) {
            (void)mode.evaluate(expr);
        }
    }));
}

void benchmark_error_handling() {
    Benchmark b("Evaluator - Error Handling (Invalid Expressions)");
    StandardMode mode;

    std::vector<std::string> invalid_exprs = {
        "1 / 0",
        "sqrt(-1)",
        "log(0)",
        "sin(undefined)",
        "(1 + 2"
    };

    b.print_result(b.run([&] {
        for (const auto& expr : invalid_exprs) {
            (void)mode.evaluate(expr);
        }
    }));
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "========================================\n";
    std::cout << "Evaluator Performance Benchmarks\n";
    std::cout << "========================================\n\n";

    benchmark_arithmetic_standard_mode();
    benchmark_power_operations();
    benchmark_trigonometric_functions();
    benchmark_logarithmic_functions();
    benchmark_bitwise_operations();
    benchmark_nested_functions();
    benchmark_complex_expressions();
    benchmark_mode_comparison();
    benchmark_repeated_evaluation();
    benchmark_variable_expressions();
    benchmark_full_pipeline();
    benchmark_constants();
    benchmark_rounding_functions();
    benchmark_error_handling();

    std::cout << "========================================\n";
    std::cout << "All evaluator benchmarks completed!\n";
    std::cout << "========================================\n";

    return 0;
}
