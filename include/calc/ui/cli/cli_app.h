/**
 * @file cli_app.h
 * @brief Main CLI application class
 */

#ifndef CALC_UI_CLI_CLI_APP_H
#define CALC_UI_CLI_CLI_APP_H

#include "calc/modes/mode_manager.h"
#include "calc/ui/cli/command_parser.h"
#include "calc/ui/cli/output_formatter.h"
#include <string>
#include <vector>
#include <sstream>

namespace calc {
namespace cli {

/**
 * @brief REPL state for interactive mode
 */
struct REPLState {
    size_t expressionCount = 0;
    std::string lastExpression;
    double lastResult = 0.0;
    bool hasLastResult = false;
};

/**
 * @brief Main CLI application class
 *
 * Handles command-line argument processing, expression evaluation,
 * and interactive REPL mode.
 */
class CliApp {
public:
    /**
     * @brief Construct a CLI application
     * @param argc Argument count
     * @param argv Argument values
     */
    CliApp(int argc, char* argv[]);

    /**
     * @brief Destructor
     */
    ~CliApp() = default;

    /**
     * @brief Run the CLI application
     * @return Exit code (0 for success, non-zero for error)
     */
    int run();

private:
    int argc_;
    char** argv_;
    ModeManager modeManager_;
    OutputFormatter formatter_;
    Mode* currentMode_;

    /**
     * @brief Process command-line arguments
     * @param options Parsed command-line options
     * @return Exit code (0 to continue, non-zero to exit)
     */
    int processOptions(const CommandLineOptions& options);

    /**
     * @brief Evaluate a single expression
     * @param expression The expression to evaluate
     * @param options Command-line options
     * @return Exit code
     */
    int evaluateExpression(const std::string& expression, const CommandLineOptions& options);

    /**
     * @brief Run interactive REPL mode
     * @param options Command-line options
     * @return Exit code
     */
    int runInteractiveMode(const CommandLineOptions& options);

    /**
     * @brief Evaluate an expression in REPL mode
     * @param state REPL state
     * @param line Input line
     * @param options Command-line options
     * @return true to continue, false to exit
     */
    bool evaluateREPLExpression(REPLState& state, const std::string& line, const CommandLineOptions& options);

    /**
     * @brief Print the welcome banner
     */
    void printBanner();

    /**
     * @brief Print the REPL prompt
     * @param state REPL state
     */
    void printPrompt(const REPLState& state);

    /**
     * @brief Print help for REPL mode
     */
    void printREPLHelp();

    /**
     * @brief Process a REPL command
     * @param command The command to process
     * @param state REPL state
     * @param options Command-line options
     * @return true to continue, false to exit
     */
    bool processREPLCommand(const std::string& command, REPLState& state, const CommandLineOptions& options);

    /**
     * @brief Handle quit/exit command
     * @return false to exit REPL
     */
    bool handleQuitCommand();

    /**
     * @brief Handle help command
     */
    void handleHelpCommand();

    /**
     * @brief Handle clear command
     * @param state REPL state
     */
    void handleClearCommand(REPLState& state);

    /**
     * @brief Handle mode command
     * @param args Command arguments
     * @param options Command-line options
     */
    void handleModeCommand(const std::string& args, const CommandLineOptions& options);

    /**
     * @brief Handle precision command
     * @param args Command arguments
     * @param options Command-line options
     */
    void handlePrecisionCommand(const std::string& args, const CommandLineOptions& options);

    /**
     * @brief Handle history command
     * @param state REPL state
     */
    void handleHistoryCommand(const REPLState& state);

    /**
     * @brief Trim whitespace from a string
     * @param str The string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Check if a line is a REPL command
     * @param line The input line
     * @return true if it's a command
     */
    static bool isREPLCommand(const std::string& line);
};

} // namespace cli
} // namespace calc

#endif // CALC_UI_CLI_CLI_APP_H
