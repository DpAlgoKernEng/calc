/**
 * @file main_window.h
 * @brief 计算器主窗口
 */

#ifndef CALC_UI_QT_WIDGETS_MAIN_WINDOW_H
#define CALC_UI_QT_WIDGETS_MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <memory>

namespace calc::ui::qt {

class DisplayWidget;
class KeypadWidget;
class ModeSelector;
class HistoryWidget;
class HistoryButton;
class HistoryModal;
class FunctionPanel;
class ConverterPanel;
class CalculatorController;

/**
 * @brief 计算器主窗口
 *
 * 整合显示区、键盘区、历史记录等组件。
 * 支持不同模式的 UI 切换。
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void onModeChanged(const QString& modeName);
    void onEvaluationFinished(bool success, const QString& result, const QString& errorMsg);
    void onHistoryItemClicked(const QString& expression);
    void onFunctionSelected(const QString& function);
    void onSettingsRequested();
    void onAboutRequested();
    void onExitRequested();
    void onHistoryButtonClicked();
    void onHistoryModalClosed();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    void updateUIForMode(const QString& modeName);

    // UI Components
    QWidget* centralWidget_;
    QWidget* glassCard_;
    QWidget* headerBar_;
    DisplayWidget* displayWidget_;
    KeypadWidget* keypadWidget_;
    ModeSelector* modeSelector_;
    HistoryWidget* historyWidget_;
    HistoryButton* historyButton_;
    HistoryModal* historyModal_;
    FunctionPanel* functionPanel_;
    ConverterPanel* converterPanel_;
    QStackedWidget* modeStack_;

    // Controller
    CalculatorController* controller_;

    // State
    QString currentMode_;
    bool isShiftPressed_;
    bool historyModalVisible_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_MAIN_WINDOW_H
