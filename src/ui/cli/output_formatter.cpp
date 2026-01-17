/**
 * @file output_formatter.cpp
 * @brief Output formatting implementation
 */

#include "calc/ui/cli/output_formatter.h"
#include "calc/utils/error.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <cctype>

// Platform-specific includes for isatty
#if defined(__unix__) || defined(__unix) || defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#include <stdio.h>
#endif

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
constexpr const char* COLOR_BRIGHT_RED = "\033[91m";
constexpr const char* COLOR_BRIGHT_GREEN = "\033[92m";
constexpr const char* COLOR_BRIGHT_YELLOW = "\033[93m";
constexpr const char* COLOR_BRIGHT_BLUE = "\033[94m";
constexpr const char* COLOR_BRIGHT_MAGENTA = "\033[95m";
constexpr const char* COLOR_BRIGHT_CYAN = "\033[96m";

OutputFormatter::OutputFormatter(ColorMode colorMode, bool showExpression, bool enableSyntaxHighlight)
    : colorMode_(colorMode)
    , useColor_(false)
    , showExpression_(showExpression)
    , syntaxHighlight_(enableSyntaxHighlight) {
    updateColorState();
}

bool OutputFormatter::isTerminalColorSupported() {
    // Check NO_COLOR environment variable (standard convention)
    if (std::getenv("NO_COLOR") != nullptr) {
        return false;
    }

    // Check if running in a terminal
    const char* term = std::getenv("TERM");
    if (term == nullptr) {
        return false;
    }

    // Check for dumb terminal
    std::string termStr(term);
    if (termStr == "dumb" || termStr == "none") {
        return false;
    }

    // Check if output is to a terminal (isatty)
    #if defined(__unix__) || defined(__unix) || defined(__linux__) || defined(__APPLE__)
    return isatty(STDOUT_FILENO) != 0;
    #elif defined(_WIN32)
    return _isatty(_fileno(stdout)) != 0;
    #else
    return true;  // Assume supported for other platforms
    #endif
}

void OutputFormatter::updateColorState() {
    switch (colorMode_) {
        case ColorMode::ALWAYS:
            useColor_ = true;
            break;
        case ColorMode::NEVER:
            useColor_ = false;
            break;
        case ColorMode::AUTO:
            useColor_ = isTerminalColorSupported();
            break;
    }
}

std::string OutputFormatter::applySyntaxHighlight(const std::string& expression) {
    if (!syntaxHighlight_ || !useColor_) {
        return expression;
    }

    std::string result;
    size_t pos = 0;

    while (pos < expression.length()) {
        char c = expression[pos];

        // Skip whitespace
        if (std::isspace(c)) {
            result += c;
            ++pos;
            continue;
        }

        // Check for function names (alphanumeric followed by '(')
        if (std::isalpha(c)) {
            size_t start = pos;
            while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) {
                ++pos;
            }
            std::string token = expression.substr(start, pos - start);

            // Check if it's followed by '(' (function call)
            if (pos < expression.length() && expression[pos] == '(') {
                result += colorText(token, COLOR_BRIGHT_MAGENTA);
            } else if (token == "PI" || token == "E") {
                // Constants
                result += colorText(token, COLOR_BRIGHT_YELLOW);
            } else {
                result += token;
            }
        }
        // Check for numbers (including hex/binary/octal prefixes)
        else if (std::isdigit(c) || (c == '.' && pos + 1 < expression.length() && std::isdigit(expression[pos + 1]))) {
            size_t start = pos;
            // Check for base prefixes
            if (c == '0' && pos + 1 < expression.length()) {
                char next = expression[pos + 1];
                if (next == 'x' || next == 'X' || next == 'b' || next == 'B' || std::isdigit(next)) {
                    ++pos;  // Skip the '0'
                    if (next == 'x' || next == 'X') {
                        ++pos;  // Skip the 'x'
                        while (pos < expression.length() && (std::isdigit(expression[pos]) ||
                               (expression[pos] >= 'a' && expression[pos] <= 'f') ||
                               (expression[pos] >= 'A' && expression[pos] <= 'F'))) {
                            ++pos;
                        }
                    } else if (next == 'b' || next == 'B') {
                        ++pos;  // Skip the 'b'
                        while (pos < expression.length() && (expression[pos] == '0' || expression[pos] == '1')) {
                            ++pos;
                        }
                    } else {
                        // Octal
                        while (pos < expression.length() && expression[pos] >= '0' && expression[pos] <= '7') {
                            ++pos;
                        }
                    }
                }
            } else {
                // Regular number
                while (pos < expression.length() && (std::isdigit(expression[pos]) || expression[pos] == '.')) {
                    ++pos;
                }
                // Check for exponent
                if (pos < expression.length() && (expression[pos] == 'e' || expression[pos] == 'E')) {
                    ++pos;
                    if (pos < expression.length() && (expression[pos] == '+' || expression[pos] == '-')) {
                        ++pos;
                    }
                    while (pos < expression.length() && std::isdigit(expression[pos])) {
                        ++pos;
                    }
                }
            }
            result += colorText(expression.substr(start, pos - start), COLOR_BRIGHT_CYAN);
        }
        // Check for operators
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
                 c == '&' || c == '|' || c == '~' || c == '%') {
            result += colorText(std::string(1, c), COLOR_BRIGHT_RED);
            ++pos;
        }
        // Check for shift operators
        else if ((c == '<' || c == '>') && pos + 1 < expression.length() && expression[pos + 1] == c) {
            result += colorText(std::string(2, c), COLOR_BRIGHT_RED);
            pos += 2;
        }
        // Check for brackets/parentheses
        else if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
            result += colorText(std::string(1, c), COLOR_BRIGHT_YELLOW);
            ++pos;
        }
        // Check for comma
        else if (c == ',') {
            result += colorText(std::string(1, c), COLOR_WHITE);
            ++pos;
        }
        else {
            result += c;
            ++pos;
        }
    }

    return result;
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
    oss << "Expression: " << applySyntaxHighlight(expression);
    return oss.str();
}

std::string OutputFormatter::formatSeparator(size_t length, char character) {
    return std::string(length, character);
}

void OutputFormatter::setUseColor(bool enabled) {
    useColor_ = enabled;
}

void OutputFormatter::setColorMode(ColorMode mode) {
    colorMode_ = mode;
    updateColorState();
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

void OutputFormatter::setSyntaxHighlight(bool enabled) {
    syntaxHighlight_ = enabled;
}

bool OutputFormatter::isSyntaxHighlightEnabled() const {
    return syntaxHighlight_;
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
