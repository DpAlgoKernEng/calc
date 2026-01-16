/**
 * @file programmer_mode.h
 * @brief Programmer calculator mode implementation
 */

#ifndef CALC_MODES_PROGRAMMER_MODE_H
#define CALC_MODES_PROGRAMMER_MODE_H

#include "calc/modes/mode.h"
#include "calc/core/parser.h"
#include "calc/core/evaluator.h"
#include "calc/core/token.h"
#include "calc/math/converter.h"
#include <memory>

namespace calc {

/**
 * @brief Programmer calculator mode
 *
 * Supports base conversions (binary, octal, hexadecimal, decimal)
 * and bitwise operations: &, |, ^, ~, <<, >>
 * Operator precedence: <<, >> > & > ^ > |, +, -
 */
class ProgrammerMode : public Mode {
public:
    /**
     * @brief Construct a programmer mode
     * @param precision Decimal precision for output (default: 6)
     */
    explicit ProgrammerMode(int precision = 6);

    /**
     * @brief Destructor
     */
    ~ProgrammerMode() override = default;

    // Mode implementation
    std::string getName() const override;
    std::string getDescription() const override;
    EvaluationResult evaluate(const std::string& expression) override;
    EvaluationContext& getContext() override;
    const EvaluationContext& getContext() const override;

    /**
     * @brief Set the display base for results
     * @param base The base (2, 8, 10, or 16)
     * @throws std::invalid_argument if base is not valid
     */
    void setDisplayBase(int base);

    /**
     * @brief Get the current display base
     * @return The display base (2, 8, 10, or 16)
     */
    int getDisplayBase() const;

    /**
     * @brief Format a numeric result based on display base
     * @param value The value to format
     * @return Formatted string with appropriate prefix
     */
    std::string formatResult(long long value) const;

    /**
     * @brief Get list of supported display bases
     * @return Vector of supported base numbers
     */
    std::vector<int> getSupportedBases() const;

    /**
     * @brief Set the output precision
     * @param precision The new precision (must be >= 0)
     */
    void setPrecision(int precision);

    /**
     * @brief Get current output precision
     * @return Current precision
     */
    int getPrecision() const;

private:
    EvaluationContext context_;
    EvaluatorVisitor evaluator_;
    int displayBase_;
    int precision_;

    /**
     * @brief Create appropriate parser
     * @return Unique pointer to parser
     */
    std::unique_ptr<Parser> createParser() const;

    /**
     * @brief Check if a base value is valid
     * @param base The base to check
     * @return true if base is 2, 8, 10, or 16
     */
    static bool isValidBase(int base);
};

} // namespace calc

#endif // CALC_MODES_PROGRAMMER_MODE_H
