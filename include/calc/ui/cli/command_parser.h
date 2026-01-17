/**
 * @file command_parser.h
 * @brief Command-line argument parser for the calculator
 */

#ifndef CALC_UI_CLI_COMMAND_PARSER_H
#define CALC_UI_CLI_COMMAND_PARSER_H

#include <string>
#include <vector>
#include <optional>

namespace calc {
namespace cli {

/**
 * @brief Color output mode
 */
enum class ColorMode {
    AUTO,    ///< Detect terminal capability
    ALWAYS,  ///< Always use colors
    NEVER    ///< Never use colors
};

/**
 * @brief Parsed command-line options
 */
struct CommandLineOptions {
    std::string mode = "standard";           ///< Calculator mode to use
    std::optional<std::string> expression;    ///< Expression to evaluate (if provided)
    std::optional<int> precision;             ///< Output precision (if specified)
    bool showHelp = false;                   ///< Show help message
    bool showVersion = false;                 ///< Show version information
    bool useRecursiveDescent = false;          ///< Use recursive descent parser
    bool interactive = false;                  ///< Interactive mode
    ColorMode colorMode = ColorMode::AUTO;    ///< Color output mode
    std::vector<std::string> expressions;     ///< Multiple expressions to evaluate
};

/**
 * @brief Parser for command-line arguments
 *
 * Parses command-line arguments and extracts calculator options.
 */
class CommandParser {
public:
    /**
     * @brief Construct a command parser
     * @param argc Argument count
     * @param argv Argument values
     */
    CommandParser(int argc, char* argv[]);

    /**
     * @brief Parse command-line arguments
     * @return Parsed options
     */
    CommandLineOptions parse();

    /**
     * @brief Get the program name
     * @return Program name (first argument)
     */
    std::string getProgramName() const;

/**
     * @brief Generate help message
     * @return Help message string
     */
    static std::string getHelpMessage();
    static std::string getHelpMessage(const std::vector<std::string>& availableModes);

    /**
     * @brief Get version string
     * @return Version information
     */
    static std::string getVersionString();

private:
    int argc_;
    char** argv_;

    /**
     * @brief Parse a number argument
     * @param arg The argument string
     * @return Optional parsed number
     */
    std::optional<int> parseNumber(const std::string& arg);

    /**
     * @brief Check if a string is a number
     * @param str The string to check
     * @return true if it's a valid number
     */
    bool isNumber(const std::string& str) const;
};

} // namespace cli
} // namespace calc

#endif // CALC_UI_CLI_COMMAND_PARSER_H
