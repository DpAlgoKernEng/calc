/**
 * @file function_panel.h
 * @brief 函数面板组件（科学计算模式）
 */

#ifndef CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H
#define CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QMap>

namespace calc::ui::qt {

/**
 * @brief 函数面板组件（科学计算模式）
 */
class FunctionPanel : public QWidget {
    Q_OBJECT

public:
    explicit FunctionPanel(QWidget* parent = nullptr);
    ~FunctionPanel() override;

    void setAvailableFunctions(const QStringList& functions);
    void addFunctionCategory(const QString& category, const QStringList& functions);

signals:
    void functionSelected(const QString& function);

private slots:
    void onFunctionButtonClicked();

private:
    void setupUI();
    void createTrigFunctions();
    void createLogFunctions();
    void createOtherFunctions();
    QPushButton* createFunctionButton(const QString& text, const QString& function);

    QTabWidget* tabWidget_;
    QMap<QString, QPushButton*> functionButtons_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H
