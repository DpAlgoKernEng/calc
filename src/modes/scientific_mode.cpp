/**
 * @file scientific_mode.cpp
 * @brief Scientific calculator mode implementation
 */

#include "calc/modes/scientific_mode.h"
#include <unordered_map>
#include <cmath>

namespace calc {

// Mathematical constants
namespace {
    constexpr double PI = 3.14159265358979323846;
    constexpr double E = 2.71828182845904523536;
}

ScientificMode::ScientificMode(int precision)
    : StandardMode(precision) {
    // Register scientific mode specific functions
    registerScientificFunctions();
}

std::string ScientificMode::getName() const {
    return "scientific";
}

std::string ScientificMode::getDescription() const {
    return "Scientific mode: all arithmetic operations + math functions (sin/cos/tan, log/exp/sqrt, etc.)";
}

std::vector<std::string> ScientificMode::getAvailableFunctions() const {
    return {
        // Trigonometric functions
        "sin", "cos", "tan",
        "asin", "acos", "atan",
        "sinh", "cosh", "tanh",

        // Logarithmic and exponential
        "log", "log10", "exp", "ln",
        "sqrt", "cbrt", "pow",

        // Rounding and absolute
        "abs", "floor", "ceil", "round", "trunc",

        // Other
        "hypot", "fmod", "remainder", "max", "min"
    };
}

double ScientificMode::getConstant(const std::string& name) {
    auto constants = getConstants();
    auto it = constants.find(name);
    if (it != constants.end()) {
        return it->second;
    }
    return 0.0;
}

std::unordered_map<std::string, double> ScientificMode::getConstants() {
    return {
        {"PI", PI},
        {"pi", PI},
        {"E", E},
        {"e", E}
    };
}

void ScientificMode::registerScientificFunctions() {
    // Most mathematical functions are already registered in StandardMode
    // via MathFunctions::registerBuiltInFunctions().
    // We just need to ensure the context has all functions available.
    // The StandardMode constructor already calls MathFunctions::registerBuiltInFunctions(),
    // so all the functions should already be available.

    // Additional functions specific to scientific mode can be added here if needed
    // For now, we rely on the standard MathFunctions which includes:
    // - Trigonometric: sin, cos, tan, asin, acos, atan, sinh, cosh, tanh
    // - Logarithmic: log, log10, exp, sqrt, cbrt, pow
    // - Rounding: abs, floor, ceil, round, trunc
    // - Other: fmod, remainder, max, min, hypot
}

} // namespace calc
