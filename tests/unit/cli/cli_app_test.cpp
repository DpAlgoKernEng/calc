/**
 * @file cli_app_test.cpp
 * @brief Unit tests for CliApp
 */

#include "calc/ui/cli/cli_app.h"
#include "calc/modes/mode_manager.h"
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

using namespace std;
using namespace calc::cli;

class CliAppTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default setup for tests
    }

    std::vector<std::string> parseInput(const std::string& input) {
        std::istringstream iss(input);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        return lines;
    }
};

// Trim tests (static method)
TEST_F(CliAppTest, Trim_RemovesLeadingWhitespace) {
    std::string result = CliApp::trim("   hello");
    EXPECT_EQ(result, "hello");
}

TEST_F(CliAppTest, Trim_RemovesTrailingWhitespace) {
    std::string result = CliApp::trim("hello   ");
    EXPECT_EQ(result, "hello");
}

TEST_F(CliAppTest, Trim_RemovesBothWhitespace) {
    std::string result = CliApp::trim("   hello   ");
    EXPECT_EQ(result, "hello");
}

TEST_F(CliAppTest, Trim_RemovesTabsAndNewlines) {
    std::string result = CliApp::trim("\t\n  hello  \t\n");
    EXPECT_EQ(result, "hello");
}

TEST_F(CliAppTest, Trim_EmptyString_ReturnsEmpty) {
    std::string result = CliApp::trim("");
    EXPECT_EQ(result, "");
}

TEST_F(CliAppTest, Trim_OnlyWhitespace_ReturnsEmpty) {
    std::string result = CliApp::trim("   \t\n  ");
    EXPECT_EQ(result, "");
}

TEST_F(CliAppTest, Trim_NoWhitespace_ReturnsOriginal) {
    std::string result = CliApp::trim("hello");
    EXPECT_EQ(result, "hello");
}

// IsREPLCommand tests (static method)
TEST_F(CliAppTest, IsREPLCommand_Quit_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("quit"));
    EXPECT_TRUE(CliApp::isREPLCommand("exit"));
}

TEST_F(CliAppTest, IsREPLCommand_Help_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("help"));
    EXPECT_TRUE(CliApp::isREPLCommand("?"));
}

TEST_F(CliAppTest, IsREPLCommand_Clear_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("clear"));
}

TEST_F(CliAppTest, IsREPLCommand_Mode_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("mode"));
}

TEST_F(CliAppTest, IsREPLCommand_Precision_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("precision"));
    EXPECT_TRUE(CliApp::isREPLCommand("prec"));
}

TEST_F(CliAppTest, IsREPLCommand_History_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("history"));
    EXPECT_TRUE(CliApp::isREPLCommand("hist"));
}

TEST_F(CliAppTest, IsREPLCommand_Search_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("search"));
}

TEST_F(CliAppTest, IsREPLCommand_Export_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("export"));
}

TEST_F(CliAppTest, IsREPLCommand_ColonPrefix_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand(":"));
    EXPECT_TRUE(CliApp::isREPLCommand(":command"));
}

TEST_F(CliAppTest, IsREPLCommand_Expression_ReturnsFalse) {
    EXPECT_FALSE(CliApp::isREPLCommand("2+2"));
    EXPECT_FALSE(CliApp::isREPLCommand("sin(90)"));
    EXPECT_FALSE(CliApp::isREPLCommand("1 + 2 * 3"));
}

TEST_F(CliAppTest, IsREPLCommand_UnknownCommand_ReturnsFalse) {
    EXPECT_FALSE(CliApp::isREPLCommand("unknown"));
    EXPECT_FALSE(CliApp::isREPLCommand("foo"));
}

TEST_F(CliAppTest, IsREPLCommand_EmptyString_ReturnsFalse) {
    EXPECT_FALSE(CliApp::isREPLCommand(""));
}

TEST_F(CliAppTest, IsREPLCommand_WhitespaceOnly_ReturnsFalse) {
    EXPECT_FALSE(CliApp::isREPLCommand("   "));
}

TEST_F(CliAppTest, IsREPLCommand_CommandWithArgs_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("mode scientific"));
    EXPECT_TRUE(CliApp::isREPLCommand("precision 10"));
    EXPECT_TRUE(CliApp::isREPLCommand("history 5"));
    EXPECT_TRUE(CliApp::isREPLCommand("search sin"));
}

TEST_F(CliAppTest, IsREPLCommand_CommandWithExtraWhitespace_ReturnsTrue) {
    EXPECT_TRUE(CliApp::isREPLCommand("   quit   "));
}

// REPLState tests
TEST_F(CliAppTest, REPLState_DefaultValues) {
    REPLState state;

    EXPECT_EQ(state.expressionCount, 0);
    EXPECT_TRUE(state.lastExpression.empty());
    EXPECT_DOUBLE_EQ(state.lastResult, 0.0);
    EXPECT_FALSE(state.hasLastResult);
}

TEST_F(CliAppTest, REPLState_HasHistoryManager) {
    REPLState state;

    EXPECT_EQ(state.historyManager.size(), 0);
    EXPECT_TRUE(state.historyManager.isEmpty());
}

TEST_F(CliAppTest, REPLState_HistoryManagerCanAddEntries) {
    REPLState state;

    state.historyManager.addSuccess("1+1", 2, "standard");

    EXPECT_EQ(state.historyManager.size(), 1);
    EXPECT_FALSE(state.historyManager.isEmpty());
}

// CommandLineOptions tests
TEST_F(CliAppTest, CommandLineOptions_Defaults) {
    CommandLineOptions options;

    EXPECT_EQ(options.mode, "standard");
    EXPECT_FALSE(options.expression.has_value());
    EXPECT_FALSE(options.precision.has_value());
    EXPECT_FALSE(options.showHelp);
    EXPECT_FALSE(options.showVersion);
    EXPECT_FALSE(options.useRecursiveDescent);
    EXPECT_FALSE(options.interactive);
    EXPECT_TRUE(options.expressions.empty());
}

TEST_F(CliAppTest, CommandLineOptions_SetMode) {
    CommandLineOptions options;
    options.mode = "scientific";

    EXPECT_EQ(options.mode, "scientific");
}

TEST_F(CliAppTest, CommandLineOptions_SetExpression) {
    CommandLineOptions options;
    options.expression = "2+2";

    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "2+2");
}

TEST_F(CliAppTest, CommandLineOptions_SetPrecision) {
    CommandLineOptions options;
    options.precision = 10;

    ASSERT_TRUE(options.precision.has_value());
    EXPECT_EQ(options.precision.value(), 10);
}

TEST_F(CliAppTest, CommandLineOptions_SetShowHelp) {
    CommandLineOptions options;
    options.showHelp = true;

    EXPECT_TRUE(options.showHelp);
}

TEST_F(CliAppTest, CommandLineOptions_SetInteractive) {
    CommandLineOptions options;
    options.interactive = true;

    EXPECT_TRUE(options.interactive);
}
