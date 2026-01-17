/**
 * @file history_manager.cpp
 * @brief History manager implementation for calculator REPL
 */

#include "calc/ui/cli/history_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace calc {
namespace cli {

// ============================================================================
// HistoryEntry Implementation
// ============================================================================

std::string HistoryManager::formatEntry(const HistoryEntry& entry) {
    std::ostringstream oss;

    if (entry.success) {
        oss << "[" << entry.id << "] " << entry.expression << " = " << entry.result;
    } else {
        oss << "[" << entry.id << "] " << entry.expression << " (Error: " << entry.error << ")";
    }

    if (!entry.timestamp.empty()) {
        oss << " [" << entry.timestamp << "]";
    }

    return oss.str();
}

// ============================================================================
// HistoryManager Implementation
// ============================================================================

HistoryManager::HistoryManager(size_t maxSize)
    : maxSize_(maxSize)
    , nextId_(1) {
}

HistoryManager::~HistoryManager() = default;

void HistoryManager::addEntry(const HistoryEntry& entry) {
    entries_.push_back(entry);
    pruneIfNecessary();
}

size_t HistoryManager::addSuccess(const std::string& expression, double result, const std::string& mode) {
    HistoryEntry entry;
    entry.id = nextId_++;
    entry.expression = expression;
    entry.result = result;
    entry.success = true;
    entry.mode = mode;
    entry.timestamp = getCurrentTimestamp();

    entries_.push_back(entry);
    pruneIfNecessary();
    return entry.id;
}

size_t HistoryManager::addFailure(const std::string& expression, const std::string& error, const std::string& mode) {
    HistoryEntry entry;
    entry.id = nextId_++;
    entry.expression = expression;
    entry.error = error;
    entry.success = false;
    entry.mode = mode;
    entry.timestamp = getCurrentTimestamp();

    entries_.push_back(entry);
    pruneIfNecessary();
    return entry.id;
}

const std::vector<HistoryEntry>& HistoryManager::getAllEntries() const {
    return entries_;
}

std::optional<HistoryEntry> HistoryManager::getEntryById(size_t id) const {
    for (const auto& entry : entries_) {
        if (entry.id == id) {
            return entry;
        }
    }
    return std::nullopt;
}

std::optional<HistoryEntry> HistoryManager::getEntryByIndex(size_t index) const {
    // Index 0 = most recent (last in vector)
    if (index >= entries_.size()) {
        return std::nullopt;
    }

    size_t reverseIndex = entries_.size() - 1 - index;
    return entries_[reverseIndex];
}

std::optional<double> HistoryManager::getLastResult() const {
    return getResult(0);
}

std::optional<double> HistoryManager::getResult(size_t n) const {
    // n = 0 returns the most recent result
    if (n >= entries_.size()) {
        return std::nullopt;
    }

    // Search from most recent to oldest
    for (auto it = entries_.rbegin(); it != entries_.rend(); ++it) {
        if (it->success) {
            if (n == 0) {
                return it->result;
            }
            --n;
        }
    }

    return std::nullopt;
}

std::vector<HistoryEntry> HistoryManager::search(const std::string& keyword) const {
    std::vector<HistoryEntry> results;
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (const auto& entry : entries_) {
        std::string lowerExpr = entry.expression;
        std::transform(lowerExpr.begin(), lowerExpr.end(), lowerExpr.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (lowerExpr.find(lowerKeyword) != std::string::npos) {
            results.push_back(entry);
        }
    }

    return results;
}

std::vector<HistoryEntry> HistoryManager::searchByMode(const std::string& mode) const {
    std::vector<HistoryEntry> results;

    for (const auto& entry : entries_) {
        if (entry.mode == mode) {
            results.push_back(entry);
        }
    }

    return results;
}

void HistoryManager::clear() {
    entries_.clear();
    nextId_ = 1;
}

size_t HistoryManager::size() const {
    return entries_.size();
}

bool HistoryManager::isEmpty() const {
    return entries_.empty();
}

size_t HistoryManager::getMaxSize() const {
    return maxSize_;
}

void HistoryManager::setMaxSize(size_t maxSize) {
    maxSize_ = maxSize;
    pruneIfNecessary();
}

bool HistoryManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Read header
    std::string header;
    if (!std::getline(file, header)) {
        return false;
    }

    // Check for CSV header
    if (header.find("id,expression,result") != std::string::npos) {
        // Parse as CSV
        return loadFromCSV(file);
    }

    // Try other formats
    return false;
}

bool HistoryManager::saveToFile(const std::string& filepath) const {
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::path dir = path.parent_path();
    if (!dir.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        // Ignore error if directory already exists
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Write CSV header
    file << "id,expression,result,success,error,mode,timestamp\n";

    // Write entries
    for (const auto& entry : entries_) {
        file << entry.id << ","
             << escapeCsv(entry.expression) << ","
             << entry.result << ","
             << (entry.success ? "1" : "0") << ","
             << escapeCsv(entry.error) << ","
             << escapeCsv(entry.mode) << ","
             << escapeCsv(entry.timestamp) << "\n";
    }

    return file.good();
}

bool HistoryManager::load() {
    return loadFromFile(getDefaultHistoryFile());
}

bool HistoryManager::save() const {
    return saveToFile(getDefaultHistoryFile());
}

bool HistoryManager::exportToText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << "Calculation History Export\n";
    file << "==========================\n\n";

    for (const auto& entry : entries_) {
        file << formatEntry(entry) << "\n";
    }

    file << "\nTotal entries: " << entries_.size() << "\n";

    return file.good();
}

std::optional<std::string> HistoryManager::expandHistoryReference(const std::string& expr) const {
    std::string result = expr;
    size_t pos = 0;

    while ((pos = result.find('!', pos)) != std::string::npos) {
        // Check for !! (last result)
        if (pos + 1 < result.size() && result[pos + 1] == '!') {
            auto lastResult = getLastResult();
            if (!lastResult.has_value()) {
                return std::nullopt;  // No history available
            }

            // Replace !! with the numeric value
            std::ostringstream oss;
            oss << *lastResult;
            std::string valueStr = oss.str();

            result.replace(pos, 2, valueStr);
            pos += valueStr.size();
            continue;
        }

        // Check for !N pattern
        size_t endPos = pos + 1;
        while (endPos < result.size() && std::isdigit(result[endPos])) {
            ++endPos;
        }

        if (endPos > pos + 1) {
            // Extract the number after !
            std::string numStr = result.substr(pos + 1, endPos - pos - 1);
            try {
                size_t index = std::stoull(numStr);

                // Get the result at this index (0-based from most recent)
                auto value = getResult(index);
                if (!value.has_value()) {
                    return std::nullopt;  // Index out of range or no successful result
                }

                // Replace !N with the numeric value
                std::ostringstream oss;
                oss << *value;
                std::string valueStr = oss.str();

                result.replace(pos, endPos - pos, valueStr);
                pos += valueStr.size();
                continue;
            } catch (const std::exception&) {
                // Invalid number, leave as is
                ++pos;
            }
        } else {
            // Single ! without number, not a reference
            ++pos;
        }
    }

    return result;
}

std::string HistoryManager::getDefaultHistoryFile() {
    std::string homeDir;

#ifdef _WIN32
    char path[MAX_PATH];
    if (SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path) == S_OK) {
        homeDir = path;
    } else {
        homeDir = ".";
    }
    return homeDir + "\\calc\\history.csv";
#else
    const char* envHome = std::getenv("HOME");
    if (envHome) {
        homeDir = envHome;
    } else {
        homeDir = ".";
    }
    return homeDir + "/.calc_history.csv";
#endif
}

// ============================================================================
// Private Methods
// ============================================================================

std::string HistoryManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");

    // Add milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

void HistoryManager::pruneIfNecessary() {
    if (maxSize_ == 0) {
        return;  // Unlimited
    }

    while (entries_.size() > maxSize_) {
        entries_.erase(entries_.begin());
    }
}

std::string HistoryManager::escapeCsv(const std::string& str) {
    if (str.find(',') != std::string::npos ||
        str.find('"') != std::string::npos ||
        str.find('\n') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : str) {
            if (c == '"') {
                escaped += "\"\"";
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    return str;
}

bool HistoryManager::loadFromCSV(std::ifstream& file) {
    // Clear current entries
    entries_.clear();
    nextId_ = 1;

    std::string line;
    size_t maxId = 0;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields;
        bool inQuotes = false;

        // Parse CSV fields properly handling quotes
        std::string current;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (inQuotes) {
                if (c == '"' && i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"';
                    ++i;  // Skip the next quote
                } else if (c == '"') {
                    inQuotes = false;
                } else {
                    current += c;
                }
            } else {
                if (c == '"') {
                    inQuotes = true;
                } else if (c == ',') {
                    fields.push_back(current);
                    current.clear();
                } else {
                    current += c;
                }
            }
        }
        fields.push_back(current);

        // Need exactly 7 fields: id,expression,result,success,error,mode,timestamp
        if (fields.size() != 7) {
            continue;
        }

        HistoryEntry entry;

        try {
            entry.id = std::stoull(fields[0]);
        } catch (...) {
            continue;
        }

        entry.expression = unescapeCsv(fields[1]);
        try {
        entry.result = 0.0;  // Initialize to avoid cppcheck warning
            entry.result = std::stod(fields[2]);
        } catch (...) {
            continue;
        }

        entry.success = (fields[3] == "1");
        entry.error = unescapeCsv(fields[4]);
        entry.mode = unescapeCsv(fields[5]);
        entry.timestamp = unescapeCsv(fields[6]);

        entries_.push_back(entry);
        if (entry.id > maxId) {
            maxId = entry.id;
        }
    }

    nextId_ = maxId + 1;
    return true;
}

std::string HistoryManager::unescapeCsv(const std::string& str) {
    if (str.empty() || str[0] != '"') {
        return str;
    }

    std::string result;
    bool inQuotes = true;

    for (size_t i = 1; i < str.size(); ++i) {
        if (inQuotes) {
            if (i + 1 < str.size() && str[i] == '"' && str[i + 1] == '"') {
                result += '"';
                ++i;
            } else if (str[i] == '"') {
                inQuotes = false;
            } else {
                result += str[i];
            }
        } else {
            result += str[i];
        }
    }

    return result;
}

} // namespace cli
} // namespace calc
