/**
 * @file cli_app.cpp
 * @brief CLI application implementation
 */

#include "calc/ui/cli/cli_app.h"
#include "calc/modes/standard_mode.h"
#include <iostream>
#include <algorithm>

namespace calc {
namespace cli {

CliApp::CliApp(int argc, char* argv[])
    : argc_(argc)
    , argv_(argv)
    , formatter_(false, true)  // Disable colors by default, show expression
    , currentMode_(nullptr) {
}

int CliApp::run() {
    CommandParser parser(argc_, argv_);

    // Parse command-line options
    CommandLineOptions options = parser.parse();

    // Handle --help
    if (options.showHelp) {
        std::cout << CommandParser::getHelpMessage() << std::endl;
        return 0;
    }

    // Handle --version
    if (options.showVersion) {
        std::cout << CommandParser::getVersionString() << std::endl;
        return 0;
    }

    // Process options and get mode
    int result = processOptions(options);
    if (result != 0) {
        return result;
    }

    // Run in interactive mode or evaluate expression
    if (options.interactive) {
        return runInteractiveMode(options);
    } else if (options.expression.has_value()) {
        return evaluateExpression(options.expression.value(), options);
    }

    // No expression given, print help
    std::cout << CommandParser::getHelpMessage() << std::endl;
    return 0;
}

int CliApp::processOptions(const CommandLineOptions& options) {
    // Get the mode
    currentMode_ = modeManager_.getMode(options.mode);
    if (currentMode_ == nullptr) {
        std::cerr << "Error: Unknown mode '" << options.mode << "'" << std::endl;
        std::cerr << "Available modes: ";
        auto modes = modeManager_.getAvailableModes();
        for (size_t i = 0; i < modes.size(); ++i) {
            if (i > 0) std::cerr << ", ";
            std::cerr << modes[i];
        }
        std::cerr << std::endl;
        return 1;
    }

    // Set precision if specified
    if (options.precision.has_value()) {
        currentMode_->getContext().setPrecision(options.precision.value());
    }

    // Set parser type
    if (options.mode == "standard") {
        auto* stdMode = dynamic_cast<calc::StandardMode*>(currentMode_);
        if (stdMode) {
            stdMode->setParserType(options.useRecursiveDescent);
        }
    }

    return 0;
}

int CliApp::evaluateExpression(const std::string& expression, const CommandLineOptions& options) {
    EvaluationResult result = currentMode_->evaluate(expression);

    if (result.isSuccess()) {
        std::cout << formatter_.formatResult(expression, result) << std::endl;
        return 0;
    } else {
        std::cerr << formatter_.formatError(expression, result) << std::endl;
        return 1;
    }
}

int CliApp::runInteractiveMode(const CommandLineOptions& options) {
    printBanner();

    REPLState state;

    std::string line;

    while (true) {
        printPrompt(state);
        if (!std::getline(std::cin, line)) {
            // EOF reached
            std::cout << std::endl;
            break;
        }

        line = trim(line);

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Handle REPL commands
        if (isREPLCommand(line)) {
            if (!processREPLCommand(line, state, options)) {
                return 0;  // Exit requested
            }
            continue;
        }

        // Evaluate expression
        if (!evaluateREPLExpression(state, line, options)) {
            break;  // Exit on error
        }
    }

    return 0;
}

bool CliApp::evaluateREPLExpression(REPLState& state, const std::string& line, const CommandLineOptions& options) {
    state.expressionCount++;
    state.lastExpression = line;

    // Expand history references (!N, !!)
    auto expanded = state.historyManager.expandHistoryReference(line);
    std::string expressionToEval = expanded.has_value() ? *expanded : line;

    EvaluationResult result = currentMode_->evaluate(expressionToEval);

    if (result.isSuccess()) {
        state.lastResult = result.getValue();
        state.hasLastResult = true;

        // Add to history
        state.historyManager.addSuccess(line, result.getValue(), currentMode_->getName());

        std::cout << "  [" << state.historyManager.size() << "] ";
        std::cout << formatter_.formatExpression(line) << std::endl;
        std::cout << "  = " << formatter_.formatResult(result) << std::endl;
        std::cout << std::endl;
    } else {
        // Add failure to history
        state.historyManager.addFailure(line, result.getErrorMessage(), currentMode_->getName());

        std::cerr << "  Error: " << result.getErrorMessage();
        if (result.getErrorPosition() > 0) {
            std::cerr << " at position " << result.getErrorPosition();
        }
        std::cerr << std::endl;
        std::cerr << std::endl;
    }

    return true;
}

void CliApp::printBanner() {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║        Calc - Cross-platform Calculator       ║" << std::endl;
    std::cout << "║                 Version 1.0.0                ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Type expressions to evaluate, or type 'help' for available commands." << std::endl;
    std::cout << "Press Ctrl+C or type 'quit' to exit." << std::endl;
    std::cout << std::endl;
}

void CliApp::printPrompt(const REPLState& state) {
    std::cout << "calc";
    if (currentMode_) {
        std::cout << "[" << currentMode_->getName() << "]";
    }
    std::cout << "> ";
    std::cout.flush();
}

void CliApp::printREPLHelp() {
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help           - Show this help message" << std::endl;
    std::cout << "  quit, exit     - Exit the calculator" << std::endl;
    std::cout << "  clear          - Clear the screen and history" << std::endl;
    std::cout << "  mode <name>    - Switch calculator mode" << std::endl;
    std::cout << "  precision <n>   - Set output precision" << std::endl;
    std::cout << "  history [N]    - Show calculation history (N entries or all)" << std::endl;
    std::cout << "  search <kw>    - Search history by keyword" << std::endl;
    std::cout << "  export <file>  - Export history to file" << std::endl;
    std::cout << std::endl;
    std::cout << "History references:" << std::endl;
    std::cout << "  !!             - Use last result" << std::endl;
    std::cout << "  !N             - Use N-th most recent result (0 = most recent)" << std::endl;
    std::cout << std::endl;
    std::cout << "Available modes: ";
    auto modes = modeManager_.getAvailableModes();
    for (size_t i = 0; i < modes.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << modes[i];
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

bool CliApp::processREPLCommand(const std::string& command, REPLState& state, const CommandLineOptions& options) {
    std::istringstream iss(command);
    std::string cmd;
    std::getline(iss, cmd, ' ');
    cmd = trim(cmd);

    std::string args;
    std::getline(iss, args);
    args = trim(args);

    if (cmd == "quit" || cmd == "exit") {
        return handleQuitCommand();
    } else if (cmd == "help" || cmd == "?") {
        handleHelpCommand();
    } else if (cmd == "clear") {
        handleClearCommand(state);
    } else if (cmd == "mode") {
        handleModeCommand(args, options);
    } else if (cmd == "precision" || cmd == "prec") {
        handlePrecisionCommand(args, options);
    } else if (cmd == "history" || cmd == "hist") {
        handleHistoryCommand(state, args);
    } else if (cmd == "search") {
        handleSearchCommand(state, args);
    } else if (cmd == "export") {
        handleExportCommand(state, args);
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
    }

    return true;
}

bool CliApp::handleQuitCommand() {
    std::cout << "Goodbye!" << std::endl;
    return false;
}

void CliApp::handleHelpCommand() {
    printREPLHelp();
}

void CliApp::handleClearCommand(REPLState& state) {
    // Clear screen (platform-specific)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    state.expressionCount = 0;
    state.hasLastResult = false;
    std::cout << "History cleared." << std::endl;
    std::cout << std::endl;
}

void CliApp::handleModeCommand(const std::string& args, const CommandLineOptions& options) {
    if (args.empty()) {
        std::cout << "Current mode: " << currentMode_->getName() << std::endl;
        std::cout << "Description: " << currentMode_->getDescription() << std::endl;
        return;
    }

    Mode* newMode = modeManager_.getMode(args);
    if (newMode == nullptr) {
        std::cout << "Unknown mode: " << args << std::endl;
        std::cout << "Available modes: ";
        auto modes = modeManager_.getAvailableModes();
        for (size_t i = 0; i < modes.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << modes[i];
        }
        std::cout << std::endl;
        return;
    }

    currentMode_ = newMode;
    std::cout << "Switched to " << currentMode_->getName() << " mode." << std::endl;
    std::cout << currentMode_->getDescription() << std::endl;

    // Apply current options to the new mode
    if (options.precision.has_value()) {
        currentMode_->getContext().setPrecision(options.precision.value());
    }
}

void CliApp::handlePrecisionCommand(const std::string& args, const CommandLineOptions& options) {
    if (args.empty()) {
        std::cout << "Current precision: " << currentMode_->getContext().getPrecision() << std::endl;
        return;
    }

    try {
        int precision = std::stoi(args);
        if (precision < 0) {
            std::cout << "Error: Precision must be non-negative." << std::endl;
            return;
        }
        currentMode_->getContext().setPrecision(precision);
        std::cout << "Precision set to " << precision << "." << std::endl;
    } catch (const std::exception&) {
        std::cout << "Error: Invalid precision value: " << args << std::endl;
    }
}

void CliApp::handleHistoryCommand(const REPLState& state, const std::string& args) {
    const auto& entries = state.historyManager.getAllEntries();

    if (entries.empty()) {
        std::cout << "No calculations yet." << std::endl;
        return;
    }

    size_t count = entries.size();
    if (!args.empty()) {
        try {
            size_t n = std::stoull(args);
            count = std::min(n, entries.size());
        } catch (const std::exception&) {
            std::cout << "Invalid count: " << args << std::endl;
            return;
        }
    }

    std::cout << std::endl;
    std::cout << "Calculation History (" << count << " of " << entries.size() << "):" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    // Show the most recent entries
    size_t startIndex = entries.size() - count;
    for (size_t i = startIndex; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        std::cout << HistoryManager::formatEntry(entry) << std::endl;
    }

    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Total: " << entries.size() << " entries" << std::endl;
    std::cout << std::endl;
}

void CliApp::handleSearchCommand(const REPLState& state, const std::string& keyword) {
    if (keyword.empty()) {
        std::cout << "Usage: search <keyword>" << std::endl;
        return;
    }

    auto results = state.historyManager.search(keyword);

    if (results.empty()) {
        std::cout << "No entries found matching '" << keyword << "'" << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "Found " << results.size() << " entries matching '" << keyword << "':" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& entry : results) {
        std::cout << HistoryManager::formatEntry(entry) << std::endl;
    }

    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::endl;
}

void CliApp::handleExportCommand(const REPLState& state, const std::string& filepath) {
    if (filepath.empty()) {
        std::cout << "Usage: export <filepath>" << std::endl;
        return;
    }

    if (state.historyManager.exportToText(filepath)) {
        std::cout << "History exported to: " << filepath << std::endl;
        std::cout << "Total entries: " << state.historyManager.size() << std::endl;
    } else {
        std::cout << "Failed to export history to: " << filepath << std::endl;
    }
    std::cout << std::endl;
}

std::string CliApp::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool CliApp::isREPLCommand(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty()) {
        return false;
    }

    // Check if it starts with a known command
    if (trimmed[0] == ':') {
        return true;
    }

    std::istringstream iss(trimmed);
    std::string cmd;
    std::getline(iss, cmd, ' ');

    return cmd == "quit" || cmd == "exit" || cmd == "help" || cmd == "?" ||
           cmd == "clear" || cmd == "mode" || cmd == "precision" ||
           cmd == "prec" || cmd == "history" || cmd == "hist" ||
           cmd == "search" || cmd == "export";
}

} // namespace cli
} // namespace calc
