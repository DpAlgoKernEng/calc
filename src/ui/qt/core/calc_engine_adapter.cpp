/**
 * @file calc_engine_adapter.cpp
 * @brief 核心引擎适配器实现
 */

#include "calc/ui/qt/calc_engine_adapter.h"
#include "calc/modes/standard_mode.h"
#include "calc/modes/scientific_mode.h"
#include "calc/modes/programmer_mode.h"

namespace calc::ui::qt {

CalcEngineAdapter::CalcEngineAdapter()
    : modeManager_(std::make_unique<ModeManager>())
    , currentMode_("standard")
    , precision_(6)
{
    // 注册默认模式
    modeManager_->registerMode(std::make_unique<StandardMode>());
    modeManager_->registerMode(std::make_unique<ScientificMode>());
    modeManager_->registerMode(std::make_unique<ProgrammerMode>());
}

CalcEngineAdapter::~CalcEngineAdapter() = default;

EvaluationResult CalcEngineAdapter::evaluate(const std::string& expression,
                                             const std::string& modeName)
{
    Mode* mode = modeManager_->getMode(modeName.empty() ? currentMode_ : modeName);
    if (!mode) {
        return EvaluationResult(ErrorCode::UNKNOWN_ERROR, "Mode not found");
    }

    EvaluationResult result = mode->evaluate(expression);

    // 如果成功且指定了模式，更新当前模式
    if (result.isSuccess() && !modeName.empty()) {
        currentMode_ = modeName;
    }

    return result;
}

std::vector<std::string> CalcEngineAdapter::getAvailableModes() const
{
    return modeManager_->getAvailableModes();
}

bool CalcEngineAdapter::switchMode(const std::string& modeName)
{
    if (!modeManager_->hasMode(modeName)) {
        return false;
    }
    currentMode_ = modeName;
    return true;
}

std::string CalcEngineAdapter::getCurrentMode() const
{
    return currentMode_;
}

std::string CalcEngineAdapter::getModeDescription(const std::string& modeName) const
{
    const Mode* mode = modeManager_->getMode(modeName);
    return mode ? mode->getDescription() : "";
}

void CalcEngineAdapter::setPrecision(int precision)
{
    precision_ = precision;
    // 更新所有模式的上下文精度
    std::vector<std::string> modes = modeManager_->getAvailableModes();
    for (const auto& name : modes) {
        Mode* mode = modeManager_->getMode(name);
        if (mode) {
            mode->getContext().setPrecision(precision);
        }
    }
}

int CalcEngineAdapter::getPrecision() const
{
    return precision_;
}

std::vector<std::string> CalcEngineAdapter::getAvailableFunctions(const std::string& modeName) const
{
    const Mode* mode = modeManager_->getMode(modeName);
    if (!mode) {
        return {};
    }

    // 返回当前模式上下文中注册的函数
    std::vector<std::string> functions;
    const EvaluationContext& ctx = mode->getContext();

    // 科学模式支持更多函数
    if (modeName == "scientific") {
        functions = {
            "sin", "cos", "tan", "asin", "acos", "atan",
            "sinh", "cosh", "tanh", "log", "log10", "exp",
            "sqrt", "cbrt", "pow", "abs", "floor", "ceil",
            "round", "trunc"
        };
    } else if (modeName == "standard") {
        functions = {"abs", "sqrt", "pow"};
    } else if (modeName == "programmer") {
        functions = {"abs"};
    }

    return functions;
}

} // namespace calc::ui::qt
