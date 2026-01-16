/**
 * @file scientific_mode.h
 * @brief Scientific calculator mode implementation
 */

#ifndef CALC_MODES_SCIENTIFIC_MODE_H
#define CALC_MODES_SCIENTIFIC_MODE_H

#include "calc/modes/standard_mode.h"

namespace calc {

/**
 * @brief Scientific calculator mode
 *
 * Extends StandardMode with comprehensive mathematical function support.
 * Supports all standard arithmetic operations plus:
 * - Trigonometric functions: sin, cos, tan, asin, acos, atan
 * - Hyperbolic functions: sinh, cosh, tanh
 * - Logarithmic and exponential: log, log10, exp, sqrt, cbrt, pow
 * - Rounding and absolute: abs, floor, ceil, round, trunc
 * - Constants: PI, E
 */
class ScientificMode : public StandardMode {
public:
    /**
     * @brief Construct a scientific mode
     * @param precision Decimal precision for output (default: 6)
     */
    explicit ScientificMode(int precision = 6);

    /**
     * @brief Destructor
     */
    ~ScientificMode() override = default;

    // Mode implementation
    std::string getName() const override;
    std::string getDescription() const override;

    /**
     * @brief Get available mathematical functions in this mode
     * @return Vector of function names
     */
    std::vector<std::string> getAvailableFunctions() const;

    /**
     * @brief Get the value of a mathematical constant
     * @param name The constant name (e.g., "PI", "E")
     * @return The constant value, or 0 if not found
     */
    static double getConstant(const std::string& name);

    /**
     * @brief Get all available constants
     * @return Map of constant names to their values
     */
    static std::unordered_map<std::string, double> getConstants();

private:
    /**
     * @brief Register scientific mode specific functions
     * Called during construction to extend base functionality
     */
    void registerScientificFunctions();
};

} // namespace calc

#endif // CALC_MODES_SCIENTIFIC_MODE_H
