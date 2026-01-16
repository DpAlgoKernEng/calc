/**
 * @file output_formatter.cpp
 * @brief Output formatting implementation
 */

#include "calc/ui/cli/output_formatter.h"
#include "calc/utils/error.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace calc {
namespace cli {

// ANSI color codes
constexpr const char* COLOR_RESET = "\033[0m";
constexpr const char* COLOR_RED = "\033[31m";
constexpr const char* COLOR_GREEN = "\033[32m";
constexpr const char* COLOR_YELLOW = "\033[33m";
constexpr const char* COLOR_BLUE = "\033[34m";
constexpr const char* COLOR_MAGENTA = "\033[35m";
constexpr const char* COLOR_CYAN = "\033[36m";
constexpr const char* COLOR_WHITE = "\033[37m";

OutputFormatter::OutputFormatter(bool useColor, bool showExpression)
    : useColor_(useColor)
    , showExpression_(showExpression) {
}

std::string OutputFormatter::formatResult(const std::string& expression, const EvaluationResult& result) {
    std::ostringstream oss;

    if (showExpression_ && !expression.empty()) {
        oss << formatExpression(expression) << "\n";
    }

    oss << "Result: ";
    if (result.isSuccess()) {
        oss << colorText(formatValue(result.getValue(), 6), COLOR_GREEN);
    } else {
        oss << formatError(expression, result);
    }

    return oss.str();
}

std::string OutputFormatter::formatError(const std::string& expression, const EvaluationResult& result) {
    std::ostringstream oss;

    if (showExpression_ && !expression.empty()) {
        oss << formatExpression(expression) << "\n";
    }

    oss << "Error: " << colorText(result.getErrorMessage(), getErrorColorCode(result.getErrorCode()));

    // Show position if available
    if (result.getErrorPosition() > 0) {
        oss << " at position " << result.getErrorPosition();
    }

    return oss.str();
}

std::string OutputFormatter::formatResult(const EvaluationResult& result) {
    return formatResult("", result);
}

std::string OutputFormatter::formatExpression(const std::string& expression) {
    std::ostringstream oss;
    oss << "Expression: " << colorText(expression, COLOR_CYAN);
    return oss.str();
}

std::string OutputFormatter::formatSeparator(size_t length, char ch) {
    return std::string(length, ch);
}

void OutputFormatter::setUseColor(bool enabled) {
    useColor_ = enabled;
}

bool OutputFormatter::isColorEnabled() const {
    return useColor_;
}

void OutputFormatter::setShowExpression(bool enabled) {
    showExpression_ = enabled;
}

bool OutputFormatter::isExpressionShown() const {
    return showExpression_;
}

std::string OutputFormatter::colorText(const std::string& text, const std::string& colorCode) {
    if (!useColor_) {
        return text;
    }
    return colorCode + text + COLOR_RESET;
}

std::string OutputFormatter::getErrorColorCode(ErrorCode code) {
    switch (code) {
        case ErrorCode::INVALID_SYNTAX:
        case ErrorCode::PARSE_ERROR:
            return COLOR_YELLOW;
        case ErrorCode::DIVISION_BY_ZERO:
        case ErrorCode::DOMAIN_ERROR:
            return COLOR_RED;
        case ErrorCode::NUMERIC_OVERFLOW:
        case ErrorCode::NUMERIC_UNDERFLOW:
            return COLOR_MAGENTA;
        case ErrorCode::INVALID_FUNCTION:
        case ErrorCode::INVALID_BASE:
            return COLOR_YELLOW;
        default:
            return COLOR_RED;
    }
}

std::string OutputFormatter::formatValue(double value, int precision) {
    std::ostringstream oss;

    // Handle special cases
    if (std::isinf(value)) {
        if (value > 0) {
            return "Infinity";
        } else {
            return "-Infinity";
        }
    }

    if (std::isnan(value)) {
        return "NaN";
    }

    // Check if the value is effectively an integer
    if (std::abs(value - std::round(value)) < std::pow(10, -precision - 1)) {
        oss << std::fixed << std::setprecision(0) << value;
    } else {
        oss << std::fixed << std::setprecision(precision) << value;

        // Remove trailing zeros after decimal point
        std::string str = oss.str();
        size_t decimalPos = str.find('.');
        if (decimalPos != std::string::npos) {
            size_t lastNonZero = str.find_last_not_of('0');
            if (lastNonZero != std::string::npos) {
                if (str[lastNonZero] == '.') {
                    str = str.substr(0, lastNonZero);
                } else {
                    str = str.substr(0, lastNonZero + 1);
                }
            }
        }
        return str;
    }

    return oss.str();
}

} // namespace cli
} // namespace calc
