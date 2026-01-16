/**
 * @file output_formatter.h
 * @brief Output formatting for calculator results
 */

#ifndef CALC_UI_CLI_OUTPUT_FORMATTER_H
#define CALC_UI_CLI_OUTPUT_FORMATTER_H

#include "calc/core/evaluator.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace calc {
namespace cli {

/**
 * @brief Formatter for calculator output
 *
 * Formats evaluation results, errors, and other information
 * for display to the user.
 */
class OutputFormatter {
public:
    /**
     * @brief Construct an output formatter
     * @param useColor Enable colored output if supported (default: true)
     * @param showExpression Show the original expression in output (default: true)
     */
    explicit OutputFormatter(bool useColor = true, bool showExpression = true);

    /**
     * @brief Destructor
     */
    ~OutputFormatter() = default;

    /**
     * @brief Format a successful result
     * @param expression The original expression
     * @param result The evaluation result (must be successful)
     * @return Formatted output string
     */
    std::string formatResult(const std::string& expression, const EvaluationResult& result);

    /**
     * @brief Format an error result
     * @param expression The original expression
     * @param result The evaluation result (must be an error)
     * @return Formatted error string
     */
    std::string formatError(const std::string& expression, const EvaluationResult& result);

    /**
     * @brief Format a result without expression
     * @param result The evaluation result
     * @return Formatted output string
     */
    std::string formatResult(const EvaluationResult& result);

    /**
     * @brief Format an expression for display
     * @param expression The expression string
     * @return Formatted expression
     */
    std::string formatExpression(const std::string& expression);

    /**
     * @brief Format a separator line
     * @param length Length of the separator (default: 40)
     * @param ch Character to use (default: '-')
     * @return Formatted separator
     */
    std::string formatSeparator(size_t length = 40, char ch = '-');

    /**
     * @brief Enable or disable colored output
     * @param enabled Whether to enable colors
     */
    void setUseColor(bool enabled);

    /**
     * @brief Check if colored output is enabled
     * @return true if colors are enabled
     */
    bool isColorEnabled() const;

    /**
     * @brief Enable or disable showing expression in output
     * @param enabled Whether to show expression
     */
    void setShowExpression(bool enabled);

    /**
     * @brief Check if expression is shown in output
     * @return true if expression is shown
     */
    bool isExpressionShown() const;

private:
    bool useColor_;
    bool showExpression_;

    /**
     * @brief Add ANSI color codes to text
     * @param text The text to color
     * @param colorCode The ANSI color code
     * @return Colored text
     */
    std::string colorText(const std::string& text, const std::string& colorCode);

    /**
     * @brief Get ANSI color code for error type
     * @param code The error code
     * @return ANSI color code
     */
    std::string getErrorColorCode(ErrorCode code);

    /**
     * @brief Format a double value
     * @param value The value to format
     * @param precision The precision to use
     * @return Formatted string
     */
    std::string formatValue(double value, int precision = 6);
};

} // namespace cli
} // namespace calc

#endif // CALC_UI_CLI_OUTPUT_FORMATTER_H
