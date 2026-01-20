/**
 * @file calc_engine_adapter.h
 * @brief Qt GUI 适配器，封装核心计算引擎
 */

#ifndef CALC_UI_QT_CALC_ENGINE_ADAPTER_H
#define CALC_UI_QT_CALC_ENGINE_ADAPTER_H

#include "calc/modes/mode_manager.h"
#include "calc/core/evaluator.h"
#include <string>
#include <memory>
#include <vector>

namespace calc::ui::qt {

/**
 * @brief Qt GUI 适配器，封装核心计算引擎
 *
 * 提供简化的接口供 Qt 组件调用，隐藏核心实现的复杂性。
 * 支持异步计算结果返回。
 */
class CalcEngineAdapter {
public:
    CalcEngineAdapter();
    ~CalcEngineAdapter();

    // 禁用复制
    CalcEngineAdapter(const CalcEngineAdapter&) = delete;
    CalcEngineAdapter& operator=(const CalcEngineAdapter&) = delete;

    /**
     * @brief 评估表达式
     * @param expression 表达式字符串
     * @param modeName 计算模式名称
     * @return 计算结果
     */
    EvaluationResult evaluate(const std::string& expression,
                             const std::string& modeName = "standard");

    /**
     * @brief 获取所有可用模式
     * @return 模式名称列表
     */
    std::vector<std::string> getAvailableModes() const;

    /**
     * @brief 切换当前模式
     * @param modeName 目标模式名称
     * @return 是否成功
     */
    bool switchMode(const std::string& modeName);

    /**
     * @brief 获取当前模式
     * @return 当前模式名称
     */
    std::string getCurrentMode() const;

    /**
     * @brief 获取模式描述
     * @param modeName 模式名称
     * @return 模式描述
     */
    std::string getModeDescription(const std::string& modeName) const;

    /**
     * @brief 设置计算精度
     * @param precision 小数点后位数
     */
    void setPrecision(int precision);

    /**
     * @brief 获取计算精度
     * @return 当前精度设置
     */
    int getPrecision() const;

    /**
     * @brief 获取指定模式可用的函数列表
     * @param modeName 模式名称
     * @return 函数名称列表
     */
    std::vector<std::string> getAvailableFunctions(const std::string& modeName) const;

private:
    std::unique_ptr<ModeManager> modeManager_;
    std::string currentMode_;
    int precision_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_CALC_ENGINE_ADAPTER_H
