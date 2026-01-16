/**
 * @file command_parser_test.cpp
 * @brief Unit tests for CommandParser
 */

#include "calc/ui/cli/command_parser.h"
#include <gtest/gtest.h>
#include <vector>

using namespace std;
using namespace calc::cli;

class CommandParserTest : public ::testing::Test {
protected:
    CommandLineOptions parse(const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>("calc"));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);

        CommandParser parser(static_cast<int>(argv.size()) - 1, argv.data());
        return parser.parse();
    }
};

// Default options
TEST_F(CommandParserTest, NoArguments_ReturnsDefaults) {
    auto options = parse({});

    EXPECT_EQ(options.mode, "standard");
    EXPECT_FALSE(options.expression.has_value());
    EXPECT_FALSE(options.precision.has_value());
    EXPECT_FALSE(options.showHelp);
    EXPECT_FALSE(options.showVersion);
    EXPECT_FALSE(options.interactive);
    EXPECT_TRUE(options.expressions.empty());
}

// Help option
TEST_F(CommandParserTest, ShortHelp_SetsShowHelp) {
    auto options = parse({"-h"});

    EXPECT_TRUE(options.showHelp);
}

TEST_F(CommandParserTest, LongHelp_SetsShowHelp) {
    auto options = parse({"--help"});

    EXPECT_TRUE(options.showHelp);
}

// Version option
TEST_F(CommandParserTest, ShortVersion_SetsShowVersion) {
    auto options = parse({"-v"});

    EXPECT_TRUE(options.showVersion);
}

TEST_F(CommandParserTest, LongVersion_SetsShowVersion) {
    auto options = parse({"--version"});

    EXPECT_TRUE(options.showVersion);
}

// Mode option
TEST_F(CommandParserTest, ShortMode_SetsMode) {
    auto options = parse({"-m", "scientific"});

    EXPECT_EQ(options.mode, "scientific");
}

TEST_F(CommandParserTest, LongMode_SetsMode) {
    auto options = parse({"--mode", "programmer"});

    EXPECT_EQ(options.mode, "programmer");
}

TEST_F(CommandParserTest, ModeOption_DefaultIsStandard) {
    auto options = parse({});

    EXPECT_EQ(options.mode, "standard");
}

// Precision option
TEST_F(CommandParserTest, ShortPrecision_SetsPrecision) {
    auto options = parse({"-p", "10"});

    ASSERT_TRUE(options.precision.has_value());
    EXPECT_EQ(options.precision.value(), 10);
}

TEST_F(CommandParserTest, LongPrecision_SetsPrecision) {
    auto options = parse({"--precision", "4"});

    ASSERT_TRUE(options.precision.has_value());
    EXPECT_EQ(options.precision.value(), 4);
}

TEST_F(CommandParserTest, PrecisionOption_NegativeNumber_SetsShowHelp) {
    auto options = parse({"-p", "-5"});

    EXPECT_TRUE(options.showHelp);
}

TEST_F(CommandParserTest, PrecisionOption_NonNumber_SetsShowHelp) {
    auto options = parse({"-p", "abc"});

    EXPECT_TRUE(options.showHelp);
}

// Recursive parser option
TEST_F(CommandParserTest, ShortRecursive_SetsRecursive) {
    auto options = parse({"-r"});

    EXPECT_TRUE(options.useRecursiveDescent);
}

TEST_F(CommandParserTest, LongRecursive_SetsRecursive) {
    auto options = parse({"--recursive"});

    EXPECT_TRUE(options.useRecursiveDescent);
}

// Interactive option
TEST_F(CommandParserTest, ShortInteractive_SetsInteractive) {
    auto options = parse({"-i"});

    EXPECT_TRUE(options.interactive);
}

TEST_F(CommandParserTest, LongInteractive_SetsInteractive) {
    auto options = parse({"--interactive"});

    EXPECT_TRUE(options.interactive);
}

// Expression parsing
TEST_F(CommandParserTest, SingleExpression_SetsExpression) {
    auto options = parse({"2+2"});

    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "2+2");
    EXPECT_EQ(options.expressions.size(), 1);
}

TEST_F(CommandParserTest, MultipleExpressions_SavesAll) {
    auto options = parse({"2+2", "3*3", "4-1"});

    EXPECT_EQ(options.mode, "standard");
    EXPECT_EQ(options.expressions.size(), 3);
}

// Combined options
TEST_F(CommandParserTest, MultipleOptions_AllParsed) {
    auto options = parse({"-i", "-p", "8", "-m", "scientific", "sin(90)"});

    EXPECT_TRUE(options.interactive);
    ASSERT_TRUE(options.precision.has_value());
    EXPECT_EQ(options.precision.value(), 8);
    EXPECT_EQ(options.mode, "scientific");
    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "sin(90)");
}

// Unknown option
TEST_F(CommandParserTest, UnknownOption_SetsShowHelp) {
    auto options = parse({"--unknown"});

    EXPECT_TRUE(options.showHelp);
}

// Expression starting with dash is parsed as option, not expression
TEST_F(CommandParserTest, ExpressionStartingWithDash_IsParsed) {
    auto options = parse({"-2+2"});

    // -2 is treated as an option, not an expression
    EXPECT_FALSE(options.expression.has_value());
    EXPECT_TRUE(options.showHelp);  // Unknown option triggers help
}

// Expression without dash is parsed as expression
TEST_F(CommandParserTest, ExpressionWithoutDash_IsParsed) {
    auto options = parse({"2+2"});

    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "2+2");
}

// Help message
TEST_F(CommandParserTest, GetHelpMessage_ReturnsNonEmptyString) {
    std::string help = CommandParser::getHelpMessage();

    EXPECT_FALSE(help.empty());
    EXPECT_TRUE(help.find("calc") != std::string::npos);
    EXPECT_TRUE(help.find("--help") != std::string::npos);
    EXPECT_TRUE(help.find("--version") != std::string::npos);
    EXPECT_TRUE(help.find("--mode") != std::string::npos);
    EXPECT_TRUE(help.find("--precision") != std::string::npos);
    EXPECT_TRUE(help.find("--interactive") != std::string::npos);
}

// Version string
TEST_F(CommandParserTest, GetVersionString_ReturnsValidString) {
    std::string version = CommandParser::getVersionString();

    EXPECT_FALSE(version.empty());
    EXPECT_TRUE(version.find("Calc") != std::string::npos);
    EXPECT_TRUE(version.find("version") != std::string::npos);
    EXPECT_TRUE(version.find("1.0.0") != std::string::npos);
}

// GetProgramName
TEST_F(CommandParserTest, GetProgramName_ReturnsCalc) {
    std::vector<char*> argv = {const_cast<char*>("calc"), nullptr};
    CommandParser parser(1, argv.data());

    EXPECT_EQ(parser.getProgramName(), "calc");
}
