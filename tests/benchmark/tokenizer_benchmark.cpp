/**
 * @file tokenizer_benchmark.cpp
 * @brief Performance benchmarks for the tokenizer
 */

#include "calc/core/tokenizer.h"
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

static const std::vector<std::string> LONG_EXPRESSIONS = {
    // 100 character expression
    "1+2+3+4+5+6+7+8+9+10+11+12+13+14+15+16+17+18+19+20+21+22+23+24+25+26+27+28+29+30+31+32+33+34+35+36+37+38+39+40+41+42+43+44+45+46+47+48+49+50",
    // 200 character expression
    "sin(1)+cos(2)+tan(3)+asin(4)+acos(5)+atan(6)+sinh(7)+cosh(8)+tanh(9)+log(10)+log10(11)+exp(12)+sqrt(13)+cbrt(14)+abs(15)+floor(16)+ceil(17)+round(18)+trunc(19)+pow(20,21)+hypot(22,23)+max(24,25)+min(26,27)+fmod(28,29)+remainder(30,31)",
    // 500 character expression
    "((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((1+2)*(3-4))/(5^6))+(sin(7)+cos(8))*tan(9))-asin(10)+acos(11)/atan(12))*sinh(13)+cosh(14)-tanh(15))*log(16)+log10(17)-exp(18))/sqrt(19)+cbrt(20))*abs(21)+floor(22)-ceil(23))*round(24)+trunc(25)-pow(26,27))/hypot(28,29)+max(30,31)-min(32,33))*fmod(34,35)+remainder(36,37)-sin(38)+cos(39))*tan(40)+asin(41)-acos(42))/atan(43)+sinh(44)-cosh(45))*tanh(46)+log(47)-log10(48))/exp(49)+sqrt(50)-cbrt(51))*abs(52)+floor(53)-ceil(54)"
};

static const std::vector<std::string> PROGRAMMER_EXPRESSIONS = {
    "0b1010101 + 0o12345 + 0xDEADBEEF",
    "0xFF & 0xF0 | 0x0F",
    "~0xFFFFFFFF ^ 0x0000FFFF",
    "0x12345678 << 16 >> 8",
    "(0b11001100 | 0b10101010) & ~(0b11110000)"
};

static const std::vector<std::string> SCIENTIFIC_EXPRESSIONS = {
    "1.23e-5 + 2.34e5",
    "6.626e-34 * 2.998e8",
    "1.414e0 * 1.732e0",
    "9.109e-31 / 1.673e-27",
    "6.022e23 * 1.381e-23"
};

void benchmark_simple_expressions() {
    Benchmark b("Tokenizer - Simple Expressions");
    b.run_with_index([&](size_t) {
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    });
    b.print_result(b.run([&] {
        for (const auto& expr : SIMPLE_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_medium_expressions() {
    Benchmark b("Tokenizer - Medium Expressions");
    b.print_result(b.run([&] {
        for (const auto& expr : MEDIUM_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_complex_expressions() {
    Benchmark b("Tokenizer - Complex Expressions");
    b.print_result(b.run([&] {
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_long_expressions() {
    Benchmark b("Tokenizer - Long Expressions");
    b.print_result(b.run([&] {
        for (const auto& expr : LONG_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_programmer_expressions() {
    Benchmark b("Tokenizer - Programmer Mode Expressions");
    b.print_result(b.run([&] {
        for (const auto& expr : PROGRAMMER_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_scientific_expressions() {
    Benchmark b("Tokenizer - Scientific Notation");
    b.print_result(b.run([&] {
        for (const auto& expr : SCIENTIFIC_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

void benchmark_single_long_tokenization() {
    // Benchmark tokenizing a very long string with many numbers
    std::string long_expr;
    for (int i = 0; i < 1000; ++i) {
        long_expr += std::to_string(i) + "+";
    }
    if (!long_expr.empty()) {
        long_expr.back() = '0';  // Replace last '+' with something else
    }

    Benchmark b("Tokenizer - 1000 numbers");
    b.print_result(b.run([&] {
        Tokenizer tokenizer(long_expr);
        (void)tokenizer.tokenize();
    }));
}

void benchmark_function_expressions() {
    Benchmark b("Tokenizer - Function Calls");
    b.print_result(b.run([&] {
        for (const auto& expr : COMPLEX_EXPRESSIONS) {
            Tokenizer tokenizer(expr);
            (void)tokenizer.tokenize();
        }
    }));
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "========================================\n";
    std::cout << "Tokenizer Performance Benchmarks\n";
    std::cout << "========================================\n\n";

    benchmark_simple_expressions();
    benchmark_medium_expressions();
    benchmark_complex_expressions();
    benchmark_long_expressions();
    benchmark_programmer_expressions();
    benchmark_scientific_expressions();
    benchmark_single_long_tokenization();
    benchmark_function_expressions();

    std::cout << "========================================\n";
    std::cout << "All tokenizer benchmarks completed!\n";
    std::cout << "========================================\n";

    return 0;
}
