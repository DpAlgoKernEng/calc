/**
 * @file main_window.cpp
 * @brief 计算器主窗口实现
 */

#include "calc/ui/qt/widgets/main_window.h"
#include "calc/ui/qt/widgets/display_widget.h"
#include "calc/ui/qt/widgets/keypad_widget.h"
#include "calc/ui/qt/widgets/mode_selector.h"
#include "calc/ui/qt/widgets/history_widget.h"
#include "calc/ui/qt/widgets/history_button.h"
#include "calc/ui/qt/widgets/history_modal.h"
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
    , historyModalVisible_(false)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();

    setWindowTitle("Calc Pro");

    // 设置固定尺寸以匹配 React 卡片设计
    resize(700, 900);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // 创建中心组件
    centralWidget_ = new QWidget(this);
    centralWidget_->setObjectName("centralWidget");
    setCentralWidget(centralWidget_);

    // 主布局 - 居中
    auto* mainLayout = new QVBoxLayout(centralWidget_);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(0);
    mainLayout->addStretch();  // 推内容到中间

    // 玻璃卡片容器 (主计算器卡片)
    glassCard_ = new QWidget(centralWidget_);
    glassCard_->setObjectName("glassCard");
    glassCard_->setMaximumWidth(600);

    auto* cardLayout = new QVBoxLayout(glassCard_);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // 头部栏：模式标签 + 历史按钮
    headerBar_ = new QWidget(glassCard_);
    headerBar_->setObjectName("headerBar");

    auto* headerLayout = new QHBoxLayout(headerBar_);
    headerLayout->setContentsMargins(24, 24, 16, 24);
    headerLayout->setSpacing(8);

    // 模式选择器 (胶囊标签)
    modeSelector_ = new ModeSelector(headerBar_);
    headerLayout->addWidget(modeSelector_);
    headerLayout->addStretch();

    // 悬浮历史按钮
    historyButton_ = new HistoryButton(headerBar_);
    headerLayout->addWidget(historyButton_);

    cardLayout->addWidget(headerBar_);

    // 显示区域
    QWidget* displayContainer = new QWidget(glassCard_);
    displayContainer->setObjectName("displayArea");

    auto* displayLayout = new QVBoxLayout(displayContainer);
    displayLayout->setContentsMargins(32, 24, 32, 24);

    displayWidget_ = new DisplayWidget(displayContainer);
    displayLayout->addWidget(displayWidget_);

    cardLayout->addWidget(displayContainer);

    // 按钮容器
    QWidget* buttonContainer = new QWidget(glassCard_);
    buttonContainer->setObjectName("buttonContainer");

    auto* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(32, 0, 32, 32);
    buttonLayout->setSpacing(0);

    // 模式堆叠窗口用于不同按钮布局
    modeStack_ = new QStackedWidget(buttonContainer);
    modeStack_->setObjectName("modeStack");
    buttonLayout->addWidget(modeStack_);

    // 键盘组件
    keypadWidget_ = new KeypadWidget(modeStack_);
    functionPanel_ = new FunctionPanel(modeStack_);
    converterPanel_ = new ConverterPanel(modeStack_);

    modeStack_->addWidget(keypadWidget_);       // Index 0: Standard
    modeStack_->addWidget(functionPanel_);        // Index 1: Scientific
    modeStack_->addWidget(converterPanel_);       // Index 2: Programmer

    cardLayout->addWidget(buttonContainer);

    // 添加卡片到主布局
    mainLayout->addWidget(glassCard_);
    mainLayout->addStretch();

    // 历史模态框 (初始隐藏)
    historyModal_ = new HistoryModal(this);
    historyModal_->setObjectName("historyModal");
    historyModal_->hide();
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

    // 历史按钮信号 (新增)
    connect(historyButton_, &HistoryButton::clicked,
            this, &MainWindow::onHistoryButtonClicked);

    // 历史模态框信号 (新增)
    connect(historyModal_, &HistoryModal::closed,
            this, &MainWindow::onHistoryModalClosed);
    connect(historyModal_, &HistoryModal::itemSelected,
            this, &MainWindow::onHistoryItemClicked);
    connect(historyModal_, &HistoryModal::clearRequested,
            controller_, &CalculatorController::clearHistory);
    connect(controller_, &CalculatorController::historyUpdated,
            this, [this]() {
                historyModal_->setHistory(controller_->getHistory());
                historyButton_->setCount(static_cast<int>(controller_->getHistory().size()));
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
    connect(escapeShortcut, &QShortcut::activated, [this]() {
        if (historyModalVisible_) {
            onHistoryButtonClicked();  // 关闭模态框
        } else {
            displayWidget_->clear();
        }
    });
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = false;
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // 如果按键在显示框中，让显示框处理
    if (displayWidget_->hasFocus()) {
        return;
    }

    // 处理数字和运算符键
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        QString num = QString::number(event->key() - Qt::Key_0);
        displayWidget_->appendExpression(num);
    } else if (event->key() == Qt::Key_A || event->key() == Qt::Key_B ||
               event->key() == Qt::Key_C || event->key() == Qt::Key_D ||
               event->key() == Qt::Key_E || event->key() == Qt::Key_F) {
        // 在程序员模式下支持十六进制输入
        if (currentMode_ == "programmer") {
            displayWidget_->appendExpression(QChar(event->key()).toUpper());
        }
    } else if (event->key() == Qt::Key_Plus) {
        displayWidget_->appendExpression("+");
    } else if (event->key() == Qt::Key_Minus) {
        displayWidget_->appendExpression("-");
    } else if (event->key() == Qt::Key_Asterisk) {
        displayWidget_->appendExpression("*");
    } else if (event->key() == Qt::Key_Slash) {
        displayWidget_->appendExpression("/");
    } else if (event->key() == Qt::Key_Period) {
        displayWidget_->appendExpression(".");
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        controller_->evaluateExpression(displayWidget_->getExpression());
    } else if (event->key() == Qt::Key_Equal) {
        controller_->evaluateExpression(displayWidget_->getExpression());
    } else if (event->key() == Qt::Key_Backspace) {
        displayWidget_->backspace();
    } else if (event->key() == Qt::Key_Escape) {
        displayWidget_->clear();
    } else if (event->key() == Qt::Key_Delete) {
        displayWidget_->clear();
    } else if (event->key() == Qt::Key_ParenLeft) {
        displayWidget_->appendExpression("(");
    } else if (event->key() == Qt::Key_ParenRight) {
        displayWidget_->appendExpression(")");
    } else if (event->key() == Qt::Key_Shift) {
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

    // 更新状态栏，显示更详细的信息
    QString modeDesc;
    if (modeName == "standard") {
        modeDesc = "Standard - Basic arithmetic operations";
    } else if (modeName == "scientific") {
        modeDesc = "Scientific - Advanced mathematical functions";
    } else if (modeName == "programmer") {
        modeDesc = "Programmer - Base conversion & bitwise operations";
    }
    statusBar()->showMessage(QString("Switched to %1").arg(modeDesc), 3000);

    // 更新键盘布局
    keypadWidget_->update();

    // 确保焦点在输入框
    displayWidget_->setFocus();
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

void MainWindow::onHistoryButtonClicked()
{
    historyModalVisible_ = !historyModalVisible_;

    if (historyModalVisible_) {
        // 显示模态框
        historyModal_->setHistory(controller_->getHistory());
        historyModal_->setGeometry(0, 0, width(), height());
        historyModal_->show();
        historyModal_->raise();
        historyModal_->activateWindow();
    } else {
        historyModal_->hide();
    }
}

void MainWindow::onHistoryModalClosed()
{
    historyModalVisible_ = false;
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
