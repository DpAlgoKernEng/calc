/**
 * @file main_window.cpp
 * @brief 计算器主窗口实现
 */

#include "calc/ui/qt/widgets/main_window.h"
#include "calc/ui/qt/widgets/display_widget.h"
#include "calc/ui/qt/widgets/keypad_widget.h"
#include "calc/ui/qt/widgets/mode_selector.h"
#include "calc/ui/qt/widgets/history_widget.h"
#include "calc/ui/qt/widgets/function_panel.h"
#include "calc/ui/qt/widgets/converter_panel.h"
#include "calc/ui/qt/core/calculator_controller.h"
#include "calc/ui/qt/dialogs/settings_dialog.h"
#include "calc/ui/qt/dialogs/about_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QKeyEvent>

namespace calc::ui::qt {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , controller_(new CalculatorController(this))
    , currentMode_("standard")
    , isShiftPressed_(false)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();

    setWindowTitle("Calc");
    resize(800, 600);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // 创建中心组件
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    // 主布局
    auto* mainLayout = new QVBoxLayout(centralWidget_);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // 模式选择器
    modeSelector_ = new ModeSelector(centralWidget_);
    mainLayout->addWidget(modeSelector_);

    // 分割器：左侧键盘，右侧历史记录
    auto* splitter = new QSplitter(Qt::Horizontal, centralWidget_);

    // 左侧：显示区 + 键盘
    auto* leftWidget = new QWidget(centralWidget_);
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);

    // 显示区
    displayWidget_ = new DisplayWidget(leftWidget);
    leftLayout->addWidget(displayWidget_, 1);

    // 模式堆叠窗口
    modeStack_ = new QStackedWidget(leftWidget);
    leftLayout->addWidget(modeStack_, 2);

    // 键盘组件
    keypadWidget_ = new KeypadWidget(modeStack_);
    functionPanel_ = new FunctionPanel(modeStack_);
    converterPanel_ = new ConverterPanel(modeStack_);

    modeStack_->addWidget(keypadWidget_);       // Index 0: Standard
    modeStack_->addWidget(functionPanel_);        // Index 1: Scientific
    modeStack_->addWidget(converterPanel_);       // Index 2: Programmer

    splitter->addWidget(leftWidget);

    // 右侧：历史记录
    historyWidget_ = new HistoryWidget(centralWidget_);
    historyWidget_->setMinimumWidth(250);
    splitter->addWidget(historyWidget_);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
}

void MainWindow::setupMenuBar()
{
    auto* menuBar = this->menuBar();

    // 文件菜单
    auto* fileMenu = menuBar->addMenu("&File");

    auto* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExitRequested);
    fileMenu->addAction(exitAction);

    // 编辑菜单
    auto* editMenu = menuBar->addMenu("&Edit");

    auto* clearHistoryAction = new QAction("Clear &History", this);
    clearHistoryAction->setShortcut(QKeySequence("Ctrl+Shift+H"));
    connect(clearHistoryAction, &QAction::triggered, controller_, &CalculatorController::clearHistory);
    editMenu->addAction(clearHistoryAction);

    // 视图菜单
    auto* viewMenu = menuBar->addMenu("&View");

    auto* standardModeAction = new QAction("&Standard Mode", this);
    standardModeAction->setShortcut(QKeySequence("Ctrl+1"));
    connect(standardModeAction, &QAction::triggered, [this]() { onModeChanged("standard"); });
    viewMenu->addAction(standardModeAction);

    auto* scientificModeAction = new QAction("&Scientific Mode", this);
    scientificModeAction->setShortcut(QKeySequence("Ctrl+2"));
    connect(scientificModeAction, &QAction::triggered, [this]() { onModeChanged("scientific"); });
    viewMenu->addAction(scientificModeAction);

    auto* programmerModeAction = new QAction("&Programmer Mode", this);
    programmerModeAction->setShortcut(QKeySequence("Ctrl+3"));
    connect(programmerModeAction, &QAction::triggered, [this]() { onModeChanged("programmer"); });
    viewMenu->addAction(programmerModeAction);

    viewMenu->addSeparator();

    auto* settingsAction = new QAction("&Settings...", this);
    settingsAction->setShortcut(QKeySequence("Ctrl+,"));
    settingsAction->setStatusTip("Open settings dialog");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsRequested);
    viewMenu->addAction(settingsAction);

    // 帮助菜单
    auto* helpMenu = menuBar->addMenu("&Help");

    auto* aboutAction = new QAction("&About", this);
    aboutAction->setShortcut(QKeySequence("F1"));
    aboutAction->setStatusTip("About this application");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutRequested);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStatusBar()
{
    auto* statusBar = this->statusBar();
    statusBar->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // 控制器信号
    connect(controller_, &CalculatorController::evaluationFinished,
            this, &MainWindow::onEvaluationFinished);
    connect(controller_, &CalculatorController::modeChanged,
            this, &MainWindow::onModeChanged);
    connect(controller_, &CalculatorController::errorOccurred,
            statusBar(), [this](const QString& msg) {
                statusBar()->showMessage(msg, 3000);
            });

    // 显示组件信号
    connect(displayWidget_, &DisplayWidget::calculateRequested,
            controller_, &CalculatorController::evaluateExpression);

    // 键盘组件信号
    connect(keypadWidget_, &KeypadWidget::keyPressed,
            displayWidget_, &DisplayWidget::appendExpression);
    connect(keypadWidget_, &KeypadWidget::calculateRequested,
            this, [this]() {
                controller_->evaluateExpression(displayWidget_->getExpression());
            });
    connect(keypadWidget_, &KeypadWidget::clearRequested,
            displayWidget_, &DisplayWidget::clear);
    connect(keypadWidget_, &KeypadWidget::backspaceRequested,
            displayWidget_, &DisplayWidget::backspace);

    // 模式选择器信号 - 直接触发到 controller，避免循环
    connect(modeSelector_, &ModeSelector::modeChanged,
            controller_, &CalculatorController::switchMode);

    // 历史记录信号
    connect(historyWidget_, &HistoryWidget::entrySelected,
            this, &MainWindow::onHistoryItemClicked);
    connect(controller_, &CalculatorController::historyUpdated,
            historyWidget_, [this]() {
                historyWidget_->setHistory(controller_->getHistory());
            });

    // 函数面板信号
    connect(functionPanel_, &FunctionPanel::functionSelected,
            this, &MainWindow::onFunctionSelected);

    // 键盘快捷键
    auto* enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, [this]() {
        controller_->evaluateExpression(displayWidget_->getExpression());
    });

    auto* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escapeShortcut, &QShortcut::activated, displayWidget_, &DisplayWidget::clear);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = true;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onModeChanged(const QString& modeName)
{
    currentMode_ = modeName;

    // 更新模式选择器
    modeSelector_->setCurrentMode(modeName);

    // 更新 UI 布局
    updateUIForMode(modeName);

    // 更新状态栏
    QString desc = controller_->getCurrentMode();
    statusBar()->showMessage(QString("Mode: %1").arg(desc));
}

void MainWindow::onEvaluationFinished(bool success, const QString& result, const QString& errorMsg)
{
    if (success) {
        displayWidget_->setResult(result);
        displayWidget_->setErrorState(false);
        statusBar()->showMessage("Calculation completed");
    } else {
        displayWidget_->setError(errorMsg);
        displayWidget_->setErrorState(true);
        statusBar()->showMessage(QString("Error: %1").arg(errorMsg));
    }
}

void MainWindow::onHistoryItemClicked(const QString& expression)
{
    displayWidget_->setExpression(expression);
}

void MainWindow::onFunctionSelected(const QString& function)
{
    displayWidget_->appendExpression(function + "(");
}

void MainWindow::onSettingsRequested()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAboutRequested()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onExitRequested()
{
    close();
}

void MainWindow::updateUIForMode(const QString& modeName)
{
    if (modeName == "standard") {
        modeStack_->setCurrentIndex(0);
        keypadWidget_->setLayoutMode(KeypadWidget::LayoutMode::Standard);
        functionPanel_->hide();
        converterPanel_->hide();
    } else if (modeName == "scientific") {
        modeStack_->setCurrentIndex(1);
        keypadWidget_->setLayoutMode(KeypadWidget::LayoutMode::Scientific);
        functionPanel_->show();
        converterPanel_->hide();
    } else if (modeName == "programmer") {
        modeStack_->setCurrentIndex(2);
        keypadWidget_->setLayoutMode(KeypadWidget::LayoutMode::Programmer);
        functionPanel_->hide();
        converterPanel_->show();
    }
}

} // namespace calc::ui::qt
