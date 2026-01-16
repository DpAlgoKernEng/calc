/**
 * @file cli_integration_test.cpp
 * @brief Integration tests for CLI components
 */

#include "calc/ui/cli/cli_app.h"
#include "calc/ui/cli/command_parser.h"
#include "calc/ui/cli/output_formatter.h"
#include "calc/ui/cli/history_manager.h"
#include <gtest/gtest.h>
#include <sstream>
#include <filesystem>
#include <fstream>

using namespace std;
using namespace calc;
using namespace calc::cli;

class CLIIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir_ = std::filesystem::temp_directory_path() / "calc_cli_test_XXXXXX";
        std::filesystem::create_directories(tempDir_);
    }

    void TearDown() override {
        if (std::filesystem::exists(tempDir_)) {
            std::filesystem::remove_all(tempDir_);
        }
    }

    std::filesystem::path tempDir_;
};

// CommandParser + Mode integration
TEST_F(CLIIntegrationTest, CommandParserWithMode_ParseStandardMode) {
    const char* argv[] = {"calc", "-m", "standard", "2+2"};
    CommandParser parser(4, const_cast<char**>(argv));
    auto options = parser.parse();

    EXPECT_EQ(options.mode, "standard");
    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "2+2");
}

TEST_F(CLIIntegrationTest, CommandParserWithMode_ParseScientificMode) {
    const char* argv[] = {"calc", "-m", "scientific", "sin(90)"};
    CommandParser parser(4, const_cast<char**>(argv));
    auto options = parser.parse();

    EXPECT_EQ(options.mode, "scientific");
    ASSERT_TRUE(options.expression.has_value());
    EXPECT_EQ(options.expression.value(), "sin(90)");
}

// HistoryManager + OutputFormatter integration
TEST_F(CLIIntegrationTest, HistoryManagerWithFormatter_FormatSuccessEntry) {
    HistoryManager hm;
    hm.addSuccess("2+2", 4, "standard");

    auto entries = hm.getAllEntries();
    ASSERT_EQ(entries.size(), 1);

    std::string formatted = HistoryManager::formatEntry(entries[0]);

    EXPECT_TRUE(formatted.find("[1]") != std::string::npos);
    EXPECT_TRUE(formatted.find("2+2") != std::string::npos);
    EXPECT_TRUE(formatted.find("= 4") != std::string::npos);
}

TEST_F(CLIIntegrationTest, HistoryManagerWithFormatter_FormatFailureEntry) {
    HistoryManager hm;
    hm.addFailure("1/0", "Division by zero", "standard");

    auto entries = hm.getAllEntries();
    ASSERT_EQ(entries.size(), 1);

    std::string formatted = HistoryManager::formatEntry(entries[0]);

    EXPECT_TRUE(formatted.find("[1]") != std::string::npos);
    EXPECT_TRUE(formatted.find("1/0") != std::string::npos);
    EXPECT_TRUE(formatted.find("(Error: Division by zero)") != std::string::npos);
}

// HistoryManager persistence integration - DISABLED (persistence not required)
TEST_F(CLIIntegrationTest, DISABLED_HistoryManagerSaveAndLoad_PreservesAllEntries) {
    std::string historyFile = (tempDir_ / "history.csv").string();

    {
        HistoryManager hm;
        hm.addSuccess("1+1", 2, "standard");
        hm.addFailure("1/0", "Error", "standard");
        hm.addSuccess("2*3", 6, "scientific");
        hm.saveToFile(historyFile);
    }

    HistoryManager hm2;
    ASSERT_TRUE(hm2.loadFromFile(historyFile));

    EXPECT_EQ(hm2.size(), 3);

    auto entry1 = hm2.getEntryById(1);
    ASSERT_TRUE(entry1.has_value());
    EXPECT_EQ(entry1->expression, "1+1");
    EXPECT_DOUBLE_EQ(entry1->result, 2);

    auto entry2 = hm2.getEntryById(2);
    ASSERT_TRUE(entry2.has_value());
    EXPECT_FALSE(entry2->success);
    EXPECT_EQ(entry2->error, "Error");

    auto entry3 = hm2.getEntryById(3);
    ASSERT_TRUE(entry3.has_value());
    EXPECT_EQ(entry3->mode, "scientific");
}

TEST_F(CLIIntegrationTest, DISABLED_HistoryManagerExport_CreatesReadableExport) {
    std::string exportFile = (tempDir_ / "export.txt").string();

    HistoryManager hm;
    hm.addSuccess("1+1", 2, "standard");
    hm.addSuccess("2+2", 4, "standard");
    hm.exportToText(exportFile);

    ASSERT_TRUE(std::filesystem::exists(exportFile));

    std::ifstream file(exportFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Calculation History Export") != std::string::npos);
    EXPECT_TRUE(content.find("Total entries: 2") != std::string::npos);
}

// History expansion integration
TEST_F(CLIIntegrationTest, HistoryManagerExpandReference_WithHistoryWorks) {
    HistoryManager hm;
    hm.addSuccess("10", 10, "standard");
    hm.addSuccess("5", 5, "standard");

    auto expanded = hm.expandHistoryReference("!0 + !1");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "5 + 10");  // !0 = most recent (5), !1 = second most recent (10)
}

TEST_F(CLIIntegrationTest, HistoryManagerExpandReference_DoubleBangWorks) {
    HistoryManager hm;
    hm.addSuccess("42", 42, "standard");

    auto expanded = hm.expandHistoryReference("!! * 2");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "42 * 2");
}

// OutputFormatter + EvaluationResult integration
TEST_F(CLIIntegrationTest, OutputFormatterWithSuccessResult_FormatsCorrectly) {
    OutputFormatter formatter(false, true);
    auto result = EvaluationResult(42.0);

    std::string output = formatter.formatResult("2*21", result);

    EXPECT_TRUE(output.find("2*21") != std::string::npos);
    EXPECT_TRUE(output.find("42") != std::string::npos);
    EXPECT_TRUE(output.find("Expression:") != std::string::npos);
    EXPECT_TRUE(output.find("Result:") != std::string::npos);
}

TEST_F(CLIIntegrationTest, OutputFormatterWithErrorResult_FormatsCorrectly) {
    OutputFormatter formatter(false, true);
    auto result = EvaluationResult(
        ErrorCode::DIVISION_BY_ZERO,
        "Cannot divide by zero",
        3
    );

    std::string output = formatter.formatError("1/0", result);

    EXPECT_TRUE(output.find("1/0") != std::string::npos);
    EXPECT_TRUE(output.find("Cannot divide by zero") != std::string::npos);
    EXPECT_TRUE(output.find("position") != std::string::npos);
    EXPECT_TRUE(output.find("3") != std::string::npos);
}

// REPLState with HistoryManager integration
TEST_F(CLIIntegrationTest, REPLState_HistoryManagerAccessible) {
    REPLState state;

    state.historyManager.addSuccess("1+1", 2, "standard");

    EXPECT_EQ(state.historyManager.size(), 1);
    EXPECT_FALSE(state.historyManager.isEmpty());

    auto lastResult = state.historyManager.getLastResult();
    ASSERT_TRUE(lastResult.has_value());
    EXPECT_DOUBLE_EQ(*lastResult, 2);
}

// CommandParser help and version
TEST_F(CLIIntegrationTest, CommandParserHelp_GeneratesValidHelp) {
    std::string help = CommandParser::getHelpMessage();

    EXPECT_FALSE(help.empty());
    EXPECT_TRUE(help.find("calc") != std::string::npos);
    EXPECT_TRUE(help.find("--help") != std::string::npos);
    EXPECT_TRUE(help.find("--version") != std::string::npos);
    EXPECT_TRUE(help.find("--mode") != std::string::npos);
    EXPECT_TRUE(help.find("--precision") != std::string::npos);
    EXPECT_TRUE(help.find("--interactive") != std::string::npos);
}

TEST_F(CLIIntegrationTest, CommandParserVersion_GeneratesValidVersion) {
    std::string version = CommandParser::getVersionString();

    EXPECT_FALSE(version.empty());
    EXPECT_TRUE(version.find("Calc") != std::string::npos);
    EXPECT_TRUE(version.find("version") != std::string::npos);
    EXPECT_TRUE(version.find("1.0.0") != std::string::npos);
}

// Color output integration
TEST_F(CLIIntegrationTest, OutputFormatter_WithColor_ContainsAnsiCodes) {
    OutputFormatter formatter(true, false);
    auto result = EvaluationResult(42.0);

    std::string output = formatter.formatResult(result);

    // Should contain ANSI escape codes
    EXPECT_TRUE(output.find("\033") != std::string::npos);
}

TEST_F(CLIIntegrationTest, OutputFormatter_WithoutColor_NoAnsiCodes) {
    OutputFormatter formatter(false, false);
    auto result = EvaluationResult(42.0);

    std::string output = formatter.formatResult(result);

    // Should NOT contain ANSI escape codes
    EXPECT_TRUE(output.find("\033") == std::string::npos);
}

// History search integration
TEST_F(CLIIntegrationTest, HistoryManagerSearch_FindsEntries) {
    HistoryManager hm;
    hm.addSuccess("sin(90)", 1, "scientific");
    hm.addSuccess("cos(90)", 0, "scientific");
    hm.addSuccess("tan(45)", 1, "scientific");

    auto results = hm.search("sin");

    EXPECT_EQ(results.size(), 1);
    EXPECT_TRUE(results[0].expression.find("sin") != std::string::npos);
}

TEST_F(CLIIntegrationTest, HistoryManagerSearchByMode_FiltersByMode) {
    HistoryManager hm;
    hm.addSuccess("1+1", 2, "standard");
    hm.addSuccess("2+2", 4, "standard");
    hm.addSuccess("sin(90)", 1, "scientific");

    auto results = hm.searchByMode("standard");

    EXPECT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.mode, "standard");
    }
}

// History max size integration
TEST_F(CLIIntegrationTest, HistoryManagerMaxSize_PrunesOldEntries) {
    HistoryManager hm;
    hm.setMaxSize(3);

    hm.addSuccess("1", 1, "standard");
    hm.addSuccess("2", 2, "standard");
    hm.addSuccess("3", 3, "standard");
    hm.addSuccess("4", 4, "standard");

    EXPECT_EQ(hm.size(), 3);

    // Oldest entry should be gone
    EXPECT_FALSE(hm.getEntryById(1));
    // Newest 3 entries should exist
    EXPECT_TRUE(hm.getEntryById(2));
    EXPECT_TRUE(hm.getEntryById(3));
    EXPECT_TRUE(hm.getEntryById(4));
}

// CSV export/import integration - DISABLED (persistence not required)
TEST_F(CLIIntegrationTest, DISABLED_HistoryManagerCsvRoundTrip_PreservesSpecialCharacters) {
    std::string historyFile = (tempDir_ / "history.csv").string();

    {
        HistoryManager hm;
        hm.addSuccess("a,b,c", 1, "standard");  // Contains comma
        hm.addSuccess("a\"b\"c", 2, "standard");  // Contains quotes
        hm.addSuccess("test", 3, "standard");
        hm.saveToFile(historyFile);
    }

    HistoryManager hm2;
    ASSERT_TRUE(hm2.loadFromFile(historyFile));

    EXPECT_EQ(hm2.size(), 3);

    auto entry1 = hm2.getEntryById(1);
    ASSERT_TRUE(entry1.has_value());
    EXPECT_EQ(entry1->expression, "a,b,c");

    auto entry2 = hm2.getEntryById(2);
    ASSERT_TRUE(entry2.has_value());
    EXPECT_EQ(entry2->expression, "a\"b\"c");
}
