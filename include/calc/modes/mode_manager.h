/**
 * @file mode_manager.h
 * @brief Manager for calculator modes
 */

#ifndef CALC_MODES_MODE_MANAGER_H
#define CALC_MODES_MODE_MANAGER_H

#include "calc/modes/mode.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace calc {

/**
 * @brief Manager for registering and retrieving calculator modes
 *
 * Maintains a registry of available modes and provides methods
 * to query and access them by name.
 */
class ModeManager {
public:
    /**
     * @brief Construct a mode manager
     */
    ModeManager();

    /**
     * @brief Destructor
     */
    ~ModeManager() = default;

    // Disable copy
    ModeManager(const ModeManager&) = delete;
    ModeManager& operator=(const ModeManager&) = delete;

    /**
     * @brief Register a mode
     * @param mode Unique pointer to the mode
     * @return true if registration succeeded, false if mode with same name exists
     */
    bool registerMode(std::unique_ptr<Mode> mode);

    /**
     * @brief Get a mode by name
     * @param name The mode name
     * @return Pointer to the mode, or nullptr if not found
     */
    Mode* getMode(const std::string& name);

    /**
     * @brief Get a mode by name (const version)
     * @param name The mode name
     * @return Const pointer to the mode, or nullptr if not found
     */
    const Mode* getMode(const std::string& name) const;

    /**
     * @brief Get the default mode
     * @return Pointer to the default mode (StandardMode), or nullptr if not available
     */
    Mode* getDefaultMode();

    /**
     * @brief Get the default mode (const version)
     * @return Const pointer to the default mode, or nullptr if not available
     */
    const Mode* getDefaultMode() const;

    /**
     * @brief Get names of all registered modes
     * @return Vector of mode names
     */
    std::vector<std::string> getAvailableModes() const;

    /**
     * @brief Check if a mode is registered
     * @param name The mode name
     * @return true if mode exists
     */
    bool hasMode(const std::string& name) const;

    /**
     * @brief Get the number of registered modes
     * @return Number of modes
     */
    size_t getModeCount() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Mode>> modes_;
    std::string defaultModeName_;
};

} // namespace calc

#endif // CALC_MODES_MODE_MANAGER_H
