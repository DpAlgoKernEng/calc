/**
 * @file standard_mode.h
 * @brief Standard calculator mode implementation
 */

#ifndef CALC_MODES_STANDARD_MODE_H
#define CALC_MODES_STANDARD_MODE_H

#include "calc/modes/mode.h"
#include "calc/core/parser.h"
#include "calc/core/evaluator.h"
#include <memory>

namespace calc {

/**
 * @brief Standard calculator mode
 *
 * Supports basic arithmetic operations: +, -, *, /, ^
 * Supports parentheses for grouping
 * Supports negative numbers (unary minus)
 * Operator precedence: ^ (right-assoc) > *,/ > +,-
 */
class StandardMode : public Mode {
public:
    /**
     * @brief Construct a standard mode
     * @param precision Decimal precision for output (default: 6)
     */
    explicit StandardMode(int precision = 6);

    /**
     * @brief Destructor
     */
    ~StandardMode() override = default;

    // Mode implementation
    std::string getName() const override;
    std::string getDescription() const override;
    EvaluationResult evaluate(const std::string& expression) override;
    EvaluationContext& getContext() override;
    const EvaluationContext& getContext() const override;

    /**
     * @brief Set the output precision
     * @param precision The new precision (must be >= 0)
     */
    void setPrecision(int precision);

    /**
     * @brief Get the current output precision
     * @return Current precision
     */
    int getPrecision() const;

    /**
     * @brief Get the parser type being used
     * @return Parser type name ("shunting-yard" or "recursive-descent")
     */
    std::string getParserType() const;

    /**
     * @brief Set the parser type to use
     * @param useRecursiveDescent If true, use recursive descent parser; otherwise shunting-yard
     */
    void setParserType(bool useRecursiveDescent);

private:
    EvaluationContext context_;
    EvaluatorVisitor evaluator_;
    bool useRecursiveDescentParser_;

    /**
     * @brief Create the appropriate parser based on current setting
     * @return Unique pointer to the parser
     */
    std::unique_ptr<Parser> createParser() const;
};

} // namespace calc

#endif // CALC_MODES_STANDARD_MODE_H
