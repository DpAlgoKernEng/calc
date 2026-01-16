/**
 * @file history_manager.h
 * @brief History manager for calculator REPL
 */

#ifndef CALC_UI_CLI_HISTORY_MANAGER_H
#define CALC_UI_CLI_HISTORY_MANAGER_H

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace calc {
namespace cli {

/**
 * @brief Single history entry
 */
struct HistoryEntry {
    size_t id;                          ///< Unique identifier
    std::string expression;             ///< Original expression
    double result;                      ///< Calculation result
    bool success;                       ///< Whether calculation succeeded
    std::string error;                  ///< Error message (if failed)
    std::string mode;                   ///< Calculator mode used
    std::string timestamp;              ///< ISO 8601 timestamp
};

/**
 * @brief History manager for calculator REPL
 *
 * Provides storage, query, export, and persistence for calculation history.
 * Supports history expansion using !N syntax to reference previous results.
 */
class HistoryManager {
public:
    /**
     * @brief Construct a history manager
     * @param maxSize Maximum number of entries to keep (0 = unlimited)
     */
    explicit HistoryManager(size_t maxSize = 1000);

    /**
     * @brief Destructor
     */
    ~HistoryManager();

    // Disable copy
    HistoryManager(const HistoryManager&) = delete;
    HistoryManager& operator=(const HistoryManager&) = delete;

    // Enable move
    HistoryManager(HistoryManager&&) = default;
    HistoryManager& operator=(HistoryManager&&) = default;

    /**
     * @brief Add an entry to history
     * @param entry The history entry to add
     */
    void addEntry(const HistoryEntry& entry);

    /**
     * @brief Add a successful calculation to history
     * @param expression The expression evaluated
     * @param result The calculation result
     * @param mode The calculator mode used
     * @return The ID of the new entry
     */
    size_t addSuccess(const std::string& expression, double result, const std::string& mode);

    /**
     * @brief Add a failed calculation to history
     * @param expression The expression evaluated
     * @param error The error message
     * @param mode The calculator mode used
     * @return The ID of the new entry
     */
    size_t addFailure(const std::string& expression, const std::string& error, const std::string& mode);

    /**
     * @brief Get all history entries
     * @return Vector of all entries
     */
    const std::vector<HistoryEntry>& getAllEntries() const;

    /**
     * @brief Get an entry by ID
     * @param id The entry ID
     * @return The entry if found, nullopt otherwise
     */
    std::optional<HistoryEntry> getEntryById(size_t id) const;

    /**
     * @brief Get an entry by index (0-based, from most recent)
     * @param index The index (0 = most recent)
     * @return The entry if found, nullopt otherwise
     */
    std::optional<HistoryEntry> getEntryByIndex(size_t index) const;

    /**
     * @brief Get the most recent successful result
     * @return The most recent result if any, nullopt otherwise
     */
    std::optional<double> getLastResult() const;

    /**
     * @brief Get the result of the N-th most recent calculation
     * @param n The offset (0 = most recent)
     * @return The result if found and successful, nullopt otherwise
     */
    std::optional<double> getResult(size_t n) const;

    /**
     * @brief Search history by keyword
     * @param keyword The keyword to search for in expressions
     * @return Matching entries
     */
    std::vector<HistoryEntry> search(const std::string& keyword) const;

    /**
     * @brief Search history by mode
     * @param mode The mode to filter by
     * @return Entries with the specified mode
     */
    std::vector<HistoryEntry> searchByMode(const std::string& mode) const;

    /**
     * @brief Clear all history entries
     */
    void clear();

    /**
     * @brief Get the number of entries
     * @return The current history size
     */
    size_t size() const;

    /**
     * @brief Check if history is empty
     * @return true if empty
     */
    bool isEmpty() const;

    /**
     * @brief Get the maximum history size
     * @return Maximum number of entries
     */
    size_t getMaxSize() const;

    /**
     * @brief Set the maximum history size
     * @param maxSize Maximum number of entries (0 = unlimited)
     */
    void setMaxSize(size_t maxSize);

    /**
     * @brief Load history from file
     * @param filepath Path to the history file
     * @return true if successful
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Save history to file
     * @param filepath Path to the history file
     * @return true if successful
     */
    bool saveToFile(const std::string& filepath) const;

    /**
     * @brief Load history from default location
     * @return true if successful
     */
    bool load();

    /**
     * @brief Save history to default location
     * @return true if successful
     */
    bool save() const;

    /**
     * @brief Export history to a formatted text file
     * @param filepath Path to the export file
     * @return true if successful
     */
    bool exportToText(const std::string& filepath) const;

    /**
     * @brief Expand history references in an expression
     * @param expr The expression with potential !N references
     * @return Expanded expression with values substituted
     *
     * Replaces !N with the result of the N-th most recent calculation.
     * !! represents the last result.
     */
    std::optional<std::string> expandHistoryReference(const std::string& expr) const;

    /**
     * @brief Get the default history file path
     * @return Platform-specific default path
     */
    static std::string getDefaultHistoryFile();

    /**
     * @brief Format a history entry for display
     * @param entry The entry to format
     * @return Formatted string
     */
    static std::string formatEntry(const HistoryEntry& entry);

    /**
     * @brief Escape a string for CSV output (public for testing)
     * @param str The string to escape
     * @return Escaped string
     */
    static std::string escapeCsv(const std::string& str);

    /**
     * @brief Unescape a string from CSV format (public for testing)
     * @param str The string to unescape
     * @return Unescaped string
     */
    static std::string unescapeCsv(const std::string& str);

private:
    std::vector<HistoryEntry> entries_;
    size_t maxSize_;
    size_t nextId_;

    /**
     * @brief Generate a timestamp string
     * @return ISO 8601 formatted timestamp
     */
    std::string getCurrentTimestamp() const;

    /**
     * @brief Prune old entries if exceeding max size
     */
    void pruneIfNecessary();

    /**
     * @brief Load history from CSV file stream
     * @param file The input file stream
     * @return true if successful
     */
    bool loadFromCSV(std::ifstream& file);
};

} // namespace cli
} // namespace calc

#endif // CALC_UI_CLI_HISTORY_MANAGER_H
