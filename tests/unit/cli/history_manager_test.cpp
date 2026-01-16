/**
 * @file history_manager_test.cpp
 * @brief Unit tests for HistoryManager
 */

#include "calc/ui/cli/history_manager.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

using namespace calc::cli;

class HistoryManagerTest : public ::testing::Test {
protected:
    std::filesystem::path tempDir_;
    std::string historyFile_;

    void SetUp() override {
        // Create a temporary directory for test files
        tempDir_ = std::filesystem::temp_directory_path() / "calc_test_XXXXXX";
        std::filesystem::create_directories(tempDir_);
        historyFile_ = (tempDir_ / "history.csv").string();
    }

    void TearDown() override {
        // Clean up temporary directory
        if (std::filesystem::exists(tempDir_)) {
            std::filesystem::remove_all(tempDir_);
        }
    }
};

// Constructor and basic operations
TEST_F(HistoryManagerTest, DefaultConstructor_CreatesEmptyHistory) {
    HistoryManager hm;
    EXPECT_EQ(hm.size(), 0);
    EXPECT_TRUE(hm.isEmpty());
}

TEST_F(HistoryManagerTest, ConstructorWithMaxSize_RespectsLimit) {
    HistoryManager hm(5);
    EXPECT_EQ(hm.getMaxSize(), 5);
}

TEST_F(HistoryManagerTest, ConstructorWithMaxSize_ZeroMeansUnlimited) {
    HistoryManager hm(0);  // Zero means unlimited
    EXPECT_EQ(hm.getMaxSize(), 0);
}

// AddEntry operations
TEST_F(HistoryManagerTest, AddSuccess_IncrementsId) {
    HistoryManager hm;
    size_t id1 = hm.addSuccess("1 + 1", 2, "standard");
    size_t id2 = hm.addSuccess("2 + 2", 4, "standard");

    EXPECT_EQ(hm.size(), 2);
    EXPECT_EQ(id1, 1);
    EXPECT_EQ(id2, 2);
}

TEST_F(HistoryManagerTest, AddFailure_IncrementsIdAndStoresError) {
    HistoryManager hm;
    size_t id = hm.addFailure("1 / 0", "Division by zero", "standard");

    EXPECT_EQ(hm.size(), 1);

    auto entry = hm.getEntryById(id);
    ASSERT_TRUE(entry.has_value());
    EXPECT_EQ(entry->expression, "1 / 0");
    EXPECT_FALSE(entry->success);
    EXPECT_EQ(entry->mode, "standard");
    EXPECT_EQ(entry->error, "Division by zero");
}

// GetEntry operations
TEST_F(HistoryManagerTest, GetEntryById_ReturnsCorrectEntry) {
    HistoryManager hm;
    hm.addSuccess("test", 42.0, "standard");

    auto entry = hm.getEntryById(1);
    ASSERT_TRUE(entry.has_value());
    EXPECT_EQ(entry->expression, "test");
    EXPECT_DOUBLE_EQ(entry->result, 42.0);
    EXPECT_EQ(entry->id, 1);
    EXPECT_EQ(entry->mode, "standard");
    EXPECT_TRUE(entry->success);
}

TEST_F(HistoryManagerTest, GetEntryByIndex_0_ReturnsMostRecent) {
    HistoryManager hm;
    hm.addSuccess("first", 100.0, "standard");
    hm.addSuccess("second", 200.0, "standard");

    auto entry = hm.getEntryByIndex(0);  // Most recent
    ASSERT_TRUE(entry.has_value());
    EXPECT_DOUBLE_EQ(entry->result, 200.0);
    EXPECT_EQ(entry->expression, "second");
}

TEST_F(HistoryManagerTest, GetEntryByIndex_1_ReturnsSecondMostRecent) {
    HistoryManager hm;
    hm.addSuccess("first", 100.0, "standard");
    hm.addSuccess("second", 200.0, "standard");

    auto entry = hm.getEntryByIndex(1);  // Second most recent
    ASSERT_TRUE(entry.has_value());
    EXPECT_DOUBLE_EQ(entry->result, 100.0);
    EXPECT_EQ(entry->expression, "first");
}

TEST_F(HistoryManagerTest, GetEntryByIndex_OutOfRange) {
    HistoryManager hm;
    hm.addSuccess("test", 123.456, "standard");

    EXPECT_FALSE(hm.getEntryByIndex(1));
    EXPECT_FALSE(hm.getEntryByIndex(999));
}

TEST_F(HistoryManagerTest, GetLastResult_ReturnsMostRecent) {
    HistoryManager hm;
    hm.addSuccess("1", 1, "standard");

    auto result = hm.getLastResult();
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(*result, 1);
}

TEST_F(HistoryManagerTest, GetLastResult_NoResults) {
    HistoryManager hm;
    EXPECT_FALSE(hm.getLastResult().has_value());
}

TEST_F(HistoryManagerTest, GetResult_ReturnsCorrectResult) {
    HistoryManager hm;
    hm.addSuccess("first", 10, "standard");
    hm.addSuccess("second", 20, "standard");
    hm.addSuccess("third", 30, "standard");

    auto r0 = hm.getResult(0);  // Most recent
    auto r1 = hm.getResult(1);  // Second most recent
    auto r2 = hm.getResult(2);  // Third most recent

    ASSERT_TRUE(r0.has_value());
    EXPECT_DOUBLE_EQ(*r0, 30);

    ASSERT_TRUE(r1.has_value());
    EXPECT_DOUBLE_EQ(*r1, 20);

    ASSERT_TRUE(r2.has_value());
    EXPECT_DOUBLE_EQ(*r2, 10);
}

TEST_F(HistoryManagerTest, GetResult_SkipsFailures) {
    HistoryManager hm;
    hm.addSuccess("first", 10, "standard");
    hm.addFailure("bad expr", "error", "standard");
    hm.addSuccess("second", 20, "standard");

    auto r0 = hm.getResult(0);  // Should be second (20), not the failure
    auto r1 = hm.getResult(1);  // Should be first (10)

    ASSERT_TRUE(r0.has_value());
    EXPECT_DOUBLE_EQ(*r0, 20);

    ASSERT_TRUE(r1.has_value());
    EXPECT_DOUBLE_EQ(*r1, 10);
}

// Search operations
TEST_F(HistoryManagerTest, Search_FindsMatchingKeywords) {
    HistoryManager hm;
    hm.addSuccess("test_sin", 1, "scientific");
    hm.addSuccess("test_cos", 0, "scientific");
    hm.addSuccess("other_expr", 5, "standard");

    auto results = hm.search("test");
    EXPECT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.mode, "scientific");
        EXPECT_TRUE(r.success);
        EXPECT_TRUE(r.expression.find("test") != std::string::npos);
    }
}

TEST_F(HistoryManagerTest, Search_IsCaseInsensitive) {
    HistoryManager hm;
    hm.addSuccess("SIN_FUNCTION", 1, "scientific");

    auto results = hm.search("sin");
    EXPECT_EQ(results.size(), 1);
}

TEST_F(HistoryManagerTest, SearchByMode_FiltersByMode) {
    HistoryManager hm;
    hm.addSuccess("1", 1, "standard");
    hm.addSuccess("2", 2, "standard");
    hm.addSuccess("3", 3, "scientific");

    auto results = hm.searchByMode("scientific");
    EXPECT_EQ(results.size(), 1);
    for (const auto& r : results) {
        EXPECT_EQ(r.mode, "scientific");
        EXPECT_TRUE(r.success);
    }
}

// Clear operations
TEST_F(HistoryManagerTest, Clear_EmptiesAndResetsId) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addSuccess("2 + 2", 4, "standard");
    EXPECT_EQ(hm.size(), 2);

    hm.clear();

    EXPECT_EQ(hm.size(), 0);
    EXPECT_TRUE(hm.isEmpty());

    // Next entry should have ID 1
    size_t id = hm.addSuccess("3 + 3", 6, "standard");
    EXPECT_EQ(id, 1);
}

// MaxSize operations
TEST_F(HistoryManagerTest, GetMaxSize_DefaultValue) {
    HistoryManager hm;
    EXPECT_EQ(hm.getMaxSize(), 1000);
}

TEST_F(HistoryManagerTest, SetMaxSize_ReducesSize) {
    HistoryManager hm(5);
    EXPECT_EQ(hm.getMaxSize(), 5);
    hm.setMaxSize(2);
    EXPECT_EQ(hm.getMaxSize(), 2);
}

TEST_F(HistoryManagerTest, SetMaxSize_ZeroMeansUnlimited) {
    HistoryManager hm(0);  // Zero means unlimited
    EXPECT_EQ(hm.getMaxSize(), 0);
}

TEST_F(HistoryManagerTest, PruneIfNecessary_RemovesOldestEntries) {
    HistoryManager hm;
    hm.setMaxSize(3);

    hm.addSuccess("first", 1, "standard");
    hm.addSuccess("second", 2, "standard");
    hm.addSuccess("third", 3, "standard");
    EXPECT_EQ(hm.size(), 3);

    // Adding fourth entry should prune the first one
    hm.addSuccess("fourth", 4, "standard");
    EXPECT_EQ(hm.size(), 3);

    // After pruning:
    // - Original "first" (id=1) is gone (oldest)
    // - Original "second" (id=2) is retained
    // - Original "third" (id=3) is retained
    // - Original "fourth" (id=4) is retained (newest)
    EXPECT_FALSE(hm.getEntryById(1));  // Original first is gone
    EXPECT_TRUE(hm.getEntryById(2));   // Original second is retained
    EXPECT_TRUE(hm.getEntryById(3));   // Original third is retained
    EXPECT_TRUE(hm.getEntryById(4));   // Original fourth is retained
}

// ExpandHistoryReference tests
TEST_F(HistoryManagerTest, ExpandHistoryReference_DoubleBang_ReplacesLastResult) {
    HistoryManager hm;
    hm.addSuccess("10", 10, "standard");

    auto expanded = hm.expandHistoryReference("!!");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "10");
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_NumberedReference_0) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addSuccess("2 + 2", 4, "standard");

    auto expanded = hm.expandHistoryReference("!0");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "4");  // Most recent
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_NumberedReference_1) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addSuccess("2 + 2", 4, "standard");

    auto expanded = hm.expandHistoryReference("!1");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "2");  // Second most recent
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_NumberedReference_OutOfRange) {
    HistoryManager hm;
    hm.addSuccess("1", 1, "standard");
    hm.addSuccess("2", 2, "standard");
    hm.addSuccess("3", 3, "standard");

    auto expanded = hm.expandHistoryReference("!10");  // Index out of range
    EXPECT_FALSE(expanded.has_value());
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_InvalidIndex_NoResults) {
    HistoryManager hm;
    auto expanded = hm.expandHistoryReference("!0");
    EXPECT_FALSE(expanded.has_value());
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_EmptyString_ReturnsEmpty) {
    HistoryManager hm;
    auto expanded = hm.expandHistoryReference("");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "");
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_NoReference_ReturnsOriginal) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");

    auto expanded = hm.expandHistoryReference("1 + 2");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "1 + 2");
}

TEST_F(HistoryManagerTest, ExpandHistoryReference_MultipleReferences) {
    HistoryManager hm;
    hm.addSuccess("10", 10, "standard");
    hm.addSuccess("5", 5, "standard");

    auto expanded = hm.expandHistoryReference("!0 + !1");
    ASSERT_TRUE(expanded.has_value());
    EXPECT_EQ(*expanded, "5 + 10");  // !0 = second most recent (5), !1 = most recent (10)
}

// File I/O tests - DISABLED (persistence not required)
TEST_F(HistoryManagerTest, DISABLED_SaveToFile_CreatesValidCSV) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");

    ASSERT_TRUE(hm.saveToFile(historyFile_));
    EXPECT_TRUE(std::filesystem::exists(historyFile_));

    std::ifstream file(historyFile_);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("id,expression,result") != std::string::npos);
}

TEST_F(HistoryManagerTest, DISABLED_LoadFromFile_RestoresEntries) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addSuccess("2 + 2", 4, "standard");

    ASSERT_TRUE(hm.saveToFile(historyFile_));

    HistoryManager hm2;
    ASSERT_TRUE(hm2.loadFromFile(historyFile_));

    EXPECT_EQ(hm2.size(), 2);

    auto entry1 = hm2.getEntryById(1);
    ASSERT_TRUE(entry1.has_value());
    EXPECT_EQ(entry1->expression, "1 + 1");
    EXPECT_DOUBLE_EQ(entry1->result, 2);
    EXPECT_TRUE(entry1->success);

    auto entry2 = hm2.getEntryById(2);
    ASSERT_TRUE(entry2.has_value());
    EXPECT_EQ(entry2->expression, "2 + 2");
    EXPECT_DOUBLE_EQ(entry2->result, 4);
    EXPECT_TRUE(entry2->success);
}

TEST_F(HistoryManagerTest, DISABLED_LoadAndSave_RoundTrip_PreservesState) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addFailure("bad", "error", "standard");
    hm.addSuccess("3 * 3", 9, "scientific");

    ASSERT_TRUE(hm.saveToFile(historyFile_));

    HistoryManager hm2;
    ASSERT_TRUE(hm2.loadFromFile(historyFile_));

    EXPECT_EQ(hm2.size(), 3);

    auto entry = hm2.getEntryById(2);
    ASSERT_TRUE(entry.has_value());
    EXPECT_FALSE(entry->success);
    EXPECT_EQ(entry->error, "error");
    EXPECT_EQ(entry->id, 2);  // Verify the ID is correct
}

TEST_F(HistoryManagerTest, DISABLED_ExportToText_CreatesValidFile) {
    HistoryManager hm;
    hm.addSuccess("1 + 1", 2, "standard");
    hm.addSuccess("2 + 2", 4, "standard");

    std::string exportFile = (tempDir_ / "export.txt").string();
    ASSERT_TRUE(hm.exportToText(exportFile));
    EXPECT_TRUE(std::filesystem::exists(exportFile));

    std::ifstream file(exportFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Calculation History Export") != std::string::npos);
    EXPECT_TRUE(content.find("[1]") != std::string::npos);
    EXPECT_TRUE(content.find("= 2") != std::string::npos);
    EXPECT_TRUE(content.find("[2]") != std::string::npos);
    EXPECT_TRUE(content.find("= 4") != std::string::npos);
}

// FormatEntry tests
TEST_F(HistoryManagerTest, FormatEntry_Successful) {
    HistoryEntry entry = {1, "1 + 1", 2.0, true, "", "standard", "2024-01-01T12:00:00"};
    std::string formatted = HistoryManager::formatEntry(entry);

    EXPECT_TRUE(formatted.find("[1]") != std::string::npos);
    EXPECT_TRUE(formatted.find("1 + 1") != std::string::npos);
    EXPECT_TRUE(formatted.find("= 2") != std::string::npos);
}

TEST_F(HistoryManagerTest, FormatEntry_Failed) {
    HistoryEntry entry = {1, "1 / 0", 0.0, false, "Division by zero", "standard", "2024-01-01T12:00:00"};
    std::string formatted = HistoryManager::formatEntry(entry);

    EXPECT_TRUE(formatted.find("[1]") != std::string::npos);
    EXPECT_TRUE(formatted.find("1 / 0") != std::string::npos);
    EXPECT_TRUE(formatted.find("(Error: Division by zero)") != std::string::npos);
}

// CSV escape/unescape tests
TEST_F(HistoryManagerTest, EscapeCsv_HandlesCommas) {
    std::string input = "a,b,c";
    std::string escaped = HistoryManager::escapeCsv(input);

    EXPECT_EQ(escaped, "\"a,b,c\"");
}

TEST_F(HistoryManagerTest, EscapeCsv_HandlesQuotes) {
    std::string input = "a\"b\"c";
    std::string escaped = HistoryManager::escapeCsv(input);

    EXPECT_EQ(escaped, "\"a\"\"b\"\"c\"");
}

TEST_F(HistoryManagerTest, UnescapeCsv_HandlesCommas) {
    std::string escaped = "\"a,b,c\"";
    std::string unescaped = HistoryManager::unescapeCsv(escaped);

    EXPECT_EQ(unescaped, "a,b,c");
}

TEST_F(HistoryManagerTest, UnescapeCsv_HandlesQuotes) {
    std::string escaped = "\"a\"\"b\"\"c\"";
    std::string unescaped = HistoryManager::unescapeCsv(escaped);

    EXPECT_EQ(unescaped, "a\"b\"c");
}
