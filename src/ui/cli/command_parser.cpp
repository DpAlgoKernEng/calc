/**
 * @file command_parser.cpp
 * @brief Command-line argument parser implementation
 */

#include "calc/ui/cli/command_parser.h"
#include <iostream>
#include <sstream>

namespace calc {
namespace cli {

CommandParser::CommandParser(int argc, char* argv[])
    : argc_(argc)
    , argv_(argv) {
}

std::string CommandParser::getProgramName() const {
    if (argc_ > 0 && argv_[0] != nullptr) {
        return argv_[0];
    }
    return "calc";
}

std::string CommandParser::getHelpMessage() {
    std::ostringstream oss;
    oss << "Calc - Cross-platform Command-line Calculator\n"
        << "\n"
        << "Usage: calc [OPTIONS] [EXPRESSION]\n"
        << "\n"
        << "Options:\n"
        << "  -h, --help              Show this help message and exit\n"
        << "  -v, --version           Show version information and exit\n"
        << "  -m, --mode <mode>      Set calculator mode (default: standard)\n"
        << "                          Available modes: standard\n"
        << "  -p, --precision <num>   Set output precision (default: 6)\n"
        << "  -r, --recursive         Use recursive descent parser\n"
        << "  -i, --interactive      Run in interactive (REPL) mode\n"
        << "\n"
        << "Standard Mode Operations:\n"
        << "  +  -  *  /  ^          Basic arithmetic operations\n"
        << "  (  )                   Parentheses for grouping\n"
        << "                          Operator precedence: ^ > *,/ > +,-\n"
        << "\n"
        << "Examples:\n"
        << "  calc \"2 + 3 * 4\"\n"
        << "  calc -p 2 \"sqrt(16) + 2\"\n"
        << "  calc -i\n"
        << "  calc -m standard \"(2 + 3) * 4\"\n"
        << "\n"
        << "For more information, visit: https://github.com/yourusername/calc";
    return oss.str();
}

std::string CommandParser::getVersionString() {
    std::ostringstream oss;
    oss << "Calc version 1.0.0\n"
        << "Built with C++17\n"
        << "Cross-platform: Windows, macOS, Linux";
    return oss.str();
}

CommandLineOptions CommandParser::parse() {
    CommandLineOptions options;

    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];

        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        }
        else if (arg == "-v" || arg == "--version") {
            options.showVersion = true;
        }
        else if (arg == "-m" || arg == "--mode") {
            if (i + 1 < argc_) {
                options.mode = argv_[++i];
            } else {
                std::cerr << "Error: --mode requires an argument\n";
                options.showHelp = true;
            }
        }
        else if (arg == "-p" || arg == "--precision") {
            if (i + 1 < argc_) {
                auto precision = parseNumber(argv_[++i]);
                if (precision.has_value()) {
                    options.precision = precision.value();
                } else {
                    std::cerr << "Error: Invalid precision value\n";
                    options.showHelp = true;
                }
            } else {
                std::cerr << "Error: --precision requires an argument\n";
                options.showHelp = true;
            }
        }
        else if (arg == "-r" || arg == "--recursive") {
            options.useRecursiveDescent = true;
        }
        else if (arg == "-i" || arg == "--interactive") {
            options.interactive = true;
        }
        else if (arg.empty() || arg[0] != '-') {
            // Not an option, treat as expression
            if (!options.expression.has_value()) {
                options.expression = arg;
            }
            options.expressions.push_back(arg);
        }
        else {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            options.showHelp = true;
        }
    }

    return options;
}

std::optional<int> CommandParser::parseNumber(const std::string& arg) {
    try {
        size_t pos = 0;
        int value = std::stoi(arg, &pos);
        if (pos == arg.length() && value >= 0) {
            return value;
        }
    } catch (const std::exception&) {
        // Invalid number
    }
    return std::nullopt;
}

bool CommandParser::isNumber(const std::string& str) const {
    if (str.empty()) {
        return false;
    }

    size_t pos = 0;
    try {
        std::stod(str, &pos);
        return pos == str.length();
    } catch (...) {
        return false;
    }
}

} // namespace cli
} // namespace calc
