/**
 * @file mode.h
 * @brief Base class for calculator modes
 */

#ifndef CALC_MODES_MODE_H
#define CALC_MODES_MODE_H

#include "calc/core/evaluator.h"
#include <string>

namespace calc {

/**
 * @brief Abstract base class for calculator modes
 *
 * Each mode (Standard, Scientific, Programmer) implements this interface
 * to provide mode-specific evaluation capabilities.
 */
class Mode {
public:
    virtual ~Mode() = default;

    /**
     * @brief Get the name of this mode
     * @return Mode name (e.g., "standard", "scientific", "programmer")
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get a description of this mode
     * @return Mode description
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Evaluate an expression in this mode
     * @param expression The expression string to evaluate
     * @return Evaluation result containing value or error
     */
    virtual EvaluationResult evaluate(const std::string& expression) = 0;

    /**
     * @brief Get the evaluation context used by this mode
     * @return Reference to the evaluation context
     */
    virtual EvaluationContext& getContext() = 0;

    /**
     * @brief Get the evaluation context used by this mode (const version)
     * @return Const reference to the evaluation context
     */
    virtual const EvaluationContext& getContext() const = 0;
};

} // namespace calc

#endif // CALC_MODES_MODE_H
