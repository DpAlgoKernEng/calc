/**
 * @file calculator_controller.cpp
 * @brief 计算器控制器实现
 */

#include "calc/ui/qt/core/calculator_controller.h"
#include "calc/modes/standard_mode.h"
#include "calc/modes/scientific_mode.h"
#include "calc/modes/programmer_mode.h"
#include <QDateTime>

namespace calc::ui::qt {

CalculatorController::CalculatorController(QObject* parent)
    : QObject(parent)
    , adapter_(std::make_unique<CalcEngineAdapter>())
    , maxHistorySize_(100)
    , angleUnit_(0)
    , currentMode_("standard")
{
}

CalculatorController::~CalculatorController() = default;

void CalculatorController::evaluateExpression(const QString& expression)
{
    if (expression.isEmpty()) {
        emit errorOccurred("Expression is empty");
        return;
    }

    std::string stdExpr = expression.toStdString();
    EvaluationResult result = adapter_->evaluate(stdExpr, getCurrentMode().toStdString());

    if (result.isSuccess()) {
        QString resultStr = QString::fromStdString(result.toString());
        emit evaluationFinished(true, resultStr, QString());
        addToHistory(expression, resultStr);
    } else {
        QString errorMsg = QString::fromStdString(result.getErrorMessage());
        emit evaluationFinished(false, QString(), errorMsg);
        emit errorOccurred(errorMsg);
    }
}

void CalculatorController::switchMode(const QString& modeName)
{
    // 避免重复切换相同模式导致无限递归
    if (currentMode_ == modeName) {
        return;
    }

    if (adapter_->switchMode(modeName.toStdString())) {
        currentMode_ = modeName;
        emit modeChanged(modeName);
    } else {
        emit errorOccurred(QString("Failed to switch to mode: %1").arg(modeName));
    }
}

void CalculatorController::addToHistory(const QString& expression, const QString& result)
{
    QVariantMap entry;
    entry["expression"] = expression;
    entry["result"] = result;
    entry["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    entry["mode"] = getCurrentMode();

    history_.prepend(entry);

    // 限制历史记录大小
    while (history_.size() > maxHistorySize_) {
        history_.removeLast();
    }

    emit historyUpdated();
}

void CalculatorController::setPrecision(int precision)
{
    if (precision >= 0 && precision <= 15) {
        adapter_->setPrecision(precision);
    }
}

void CalculatorController::setAngleUnit(int unit)
{
    if (unit == 0 || unit == 1) {
        angleUnit_ = unit;
    }
}

QStringList CalculatorController::getAvailableModes() const
{
    std::vector<std::string> modes = adapter_->getAvailableModes();
    QStringList result;
    for (const auto& mode : modes) {
        result.append(QString::fromStdString(mode));
    }
    return result;
}

QString CalculatorController::getCurrentMode() const
{
    return QString::fromStdString(adapter_->getCurrentMode());
}

QStringList CalculatorController::getAvailableFunctions() const
{
    std::vector<std::string> funcs = adapter_->getAvailableFunctions(getCurrentMode().toStdString());
    QStringList result;
    for (const auto& func : funcs) {
        result.append(QString::fromStdString(func));
    }
    return result;
}

QVariantList CalculatorController::getHistory() const
{
    QVariantList result;
    for (const auto& entry : history_) {
        result.append(entry);
    }
    return result;
}

void CalculatorController::clearHistory()
{
    history_.clear();
    emit historyUpdated();
}

} // namespace calc::ui::qt
