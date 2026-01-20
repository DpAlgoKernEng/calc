/**
 * @file calculator_controller.h
 * @brief 计算器控制器，处理 Qt UI 与核心引擎的交互
 */

#ifndef CALC_UI_QT_CORE_CALCULATOR_CONTROLLER_H
#define CALC_UI_QT_CORE_CALCULATOR_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include "calc/ui/qt/calc_engine_adapter.h"
#include <memory>

namespace calc::ui::qt {

/**
 * @brief 计算器控制器，处理 Qt UI 与核心引擎的交互
 *
 * 使用 Qt 信号/槽机制实现异步通信和状态通知。
 */
class CalculatorController : public QObject {
    Q_OBJECT

public:
    explicit CalculatorController(QObject* parent = nullptr);
    ~CalculatorController() override;

    // 核心功能
    Q_INVOKABLE void evaluateExpression(const QString& expression);
    Q_INVOKABLE void switchMode(const QString& modeName);
    Q_INVOKABLE void addToHistory(const QString& expression, const QString& result);

    // 配置
    Q_INVOKABLE void setPrecision(int precision);
    Q_INVOKABLE void setAngleUnit(int unit); // 0=度, 1=弧度

    // 查询
    Q_INVOKABLE QStringList getAvailableModes() const;
    Q_INVOKABLE QString getCurrentMode() const;
    Q_INVOKABLE QStringList getAvailableFunctions() const;

    // 历史记录
    Q_INVOKABLE QVariantList getHistory() const;
    Q_INVOKABLE void clearHistory();

signals:
    /**
     * @brief 计算完成信号
     * @param success 是否成功
     * @param result 结果字符串
     * @param errorMsg 错误消息（失败时）
     */
    void evaluationFinished(bool success, const QString& result, const QString& errorMsg);

    /**
     * @brief 模式切换信号
     * @param modeName 新模式名称
     */
    void modeChanged(const QString& modeName);

    /**
     * @brief 历史记录更新信号
     */
    void historyUpdated();

    /**
     * @brief 错误信号
     * @param errorMsg 错误消息
     */
    void errorOccurred(const QString& errorMsg);

private:
    std::unique_ptr<CalcEngineAdapter> adapter_;
    QList<QVariantMap> history_;
    int maxHistorySize_;
    int angleUnit_;
    QString currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_CORE_CALCULATOR_CONTROLLER_H
