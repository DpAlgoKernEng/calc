/**
 * @file mode_manager.cpp
 * @brief Manager for calculator modes implementation
 */

#include "calc/modes/mode_manager.h"
#include "calc/modes/standard_mode.h"
#include "calc/modes/scientific_mode.h"
#include "calc/modes/programmer_mode.h"
#include <algorithm>

namespace calc {

ModeManager::ModeManager()
    : defaultModeName_("standard") {
    // Register StandardMode as default
    auto standardMode = std::make_unique<StandardMode>();
    registerMode(std::move(standardMode));

    // Register ScientificMode
    auto scientificMode = std::make_unique<ScientificMode>();
    registerMode(std::move(scientificMode));

    // Register ProgrammerMode
    auto programmerMode = std::make_unique<ProgrammerMode>();
    registerMode(std::move(programmerMode));
}

bool ModeManager::registerMode(std::unique_ptr<Mode> mode) {
    if (!mode) {
        return false;
    }

    const std::string& name = mode->getName();
    if (modes_.find(name) != modes_.end()) {
        return false;  // Mode with this name already exists
    }

    modes_[name] = std::move(mode);
    return true;
}

Mode* ModeManager::getMode(const std::string& name) {
    auto it = modes_.find(name);
    if (it != modes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Mode* ModeManager::getMode(const std::string& name) const {
    auto it = modes_.find(name);
    if (it != modes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

Mode* ModeManager::getDefaultMode() {
    return getMode(defaultModeName_);
}

const Mode* ModeManager::getDefaultMode() const {
    return getMode(defaultModeName_);
}

std::vector<std::string> ModeManager::getAvailableModes() const {
    std::vector<std::string> names;
    names.reserve(modes_.size());

    for (const auto& pair : modes_) {
        names.push_back(pair.first);
    }

    // Sort alphabetically for consistent output
    std::sort(names.begin(), names.end());
    return names;
}

bool ModeManager::hasMode(const std::string& name) const {
    return modes_.find(name) != modes_.end();
}

size_t ModeManager::getModeCount() const {
    return modes_.size();
}

} // namespace calc
