# Qt GUI 扩展设计文档

## 一、项目概述

### 1.1 设计目标

为计算器项目设计并实现跨平台 Qt 图形用户界面，在保持核心计算引擎独立性的前提下，提供直观、美观、易用的计算器应用。

### 1.2 设计原则

- **分层架构**: 保持核心引擎与 UI 层的完全分离
- **跨平台支持**: Windows、macOS、Linux 原生体验
- **可扩展性**: 便于添加新的计算模式和功能
- **用户体验**: 符合现代 GUI 设计规范
- **代码复用**: 最大化利用现有核心计算逻辑

### 1.3 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| Qt | 6.x | GUI 框架 |
| C++ | C++17 | 编程语言 |
| CMake | 3.15+ | 构建系统 |
| Qt Widgets | 6.x | 传统控件 |
| QML | (可选) | 声明式 UI |

---

## 二、阶段十任务分析

根据 `ARCHITECTURE.md` 阶段十 (Week 13-14) 的规划：

| 任务编号 | 任务描述 | 产出物 |
|----------|----------|--------|
| 10.1 | 分离 UI 层逻辑 | 核心引擎独立封装 |
| 10.2 | 设计 Qt 接口层 | 抽象 UI 接口 |
| 10.3 | 准备 Qt CMake 模块 | Qt 集成配置 |
| 10.4 | 设计信号/槽接口 | 模式事件系统 |
| 10.5 | 编写 UI 集成指南 | Qt 扩展文档 |

---

## 三、系统架构设计

### 3.1 整体架构图

```
┌─────────────────────────────────────────────────────────────────────┐
│                           Qt GUI Layer                               │
│  ┌───────────────────────────────────────────────────────────────┐   │
│  │  MainWindow (主窗口)                                          │   │
│  │  ┌─────────────────────────────────────────────────────┐    │   │
│  │  │  DisplayWidget (显示区)                              │    │   │
│  │  │  ┌─────────────┬─────────────────────────────┐      │    │   │
│  │  │  │ Expression  │       Result Display        │      │    │   │
│  │  │  │   Editor    │                             │      │    │   │
│  │  │  └─────────────┴─────────────────────────────┘      │    │   │
│  │  ├─────────────────────────────────────────────────────┤    │   │
│  │  │  ModeSelector (模式选择)                              │    │   │
│  │  ├─────────────────────────────────────────────────────┤    │   │
│  │  │  KeypadWidget (键盘区)                               │    │   │
│  │  │  ┌─────┬─────┬─────┬─────┐                          │    │   │
│  │  │  │  7  │  8  │  9  │  /  │                          │    │   │
│  │  │  ├─────┼─────┼─────┼─────┤                          │    │   │
│  │  │  │  4  │  5  │  6  │  *  │                          │    │   │
│  │  │  ├─────┼─────┼─────┼─────┤                          │    │   │
│  │  │  │  1  │  2  │  3  │  -  │                          │    │   │
│  │  │  ├─────┼─────┼─────┼─────┤                          │    │   │
│  │  │  │  0  │  .  │  =  │  +  │                          │    │   │
│  │  │  └─────┴─────┴─────┴─────┘                          │    │   │
│  │  ├─────────────────────────────────────────────────────┤    │   │
│  │  │  HistoryWidget (历史记录)                           │    │   │
│  │  └─────────────────────────────────────────────────────┘    │   │
│  └───────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   │ Qt Signals/Slots
                                   │
┌──────────────────────────────────┴─────────────────────────────────────┐
│                    Qt UI Adapter Layer                                 │
│  ┌─────────────────────────────────────────────────────────────┐     │
│  │  CalculatorController (计算控制器)                          │     │
│  │  - 桥接 Qt UI 和核心计算引擎                                 │     │
│  │  - 管理 ModeManager 实例                                    │     │
│  │  - 处理计算请求和结果                                       │     │
│  └─────────────────────────────────────────────────────────────┘     │
│  ┌─────────────────────────────────────────────────────────────┐     │
│  │  ModeWidgetFactory (模式部件工厂)                           │     │
│  │  - 创建不同模式的 UI 部件                                   │     │
│  └─────────────────────────────────────────────────────────────┘     │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   │ Core API Call
                                   │
┌──────────────────────────────────┴─────────────────────────────────────┐
│                        Application Layer (现有)                          │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐     │
│  │  StandardMode     │  │  ScientificMode  │  │  ProgrammerMode  │     │
│  └──────────────────┘  └──────────────────┘  └──────────────────┘     │
│  ┌──────────────────┐                                                   │
│  │  ModeManager     │                                                   │
│  └──────────────────┘                                                   │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   │
┌──────────────────────────────────┴─────────────────────────────────────┐
│                         Core Layer (现有)                                │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐     │
│  │    Tokenizer    │  │     Parser       │  │    Evaluator     │     │
│  └──────────────────┘  └──────────────────┘  └──────────────────┘     │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐     │
│  │  AST Nodes      │  │  Operator Def    │  │  Function Registry│    │
│  └──────────────────┘  └──────────────────┘  └──────────────────┘     │
└─────────────────────────────────────────────────────────────────────┘
```

### 3.2 目录结构

```
calc/
├── src/
│   ├── ui/
│   │   ├── qt/                           # Qt GUI 实现
│   │   │   ├── main.cpp                  # Qt 应用入口
│   │   │   ├── CMakeLists.txt            # Qt 构建配置
│   │   │   ├── core/                     # Qt 核心层
│   │   │   │   ├── calculator_controller.h
│   │   │   │   ├── calculator_controller.cpp
│   │   │   │   ├── mode_widget_factory.h
│   │   │   │   └── mode_widget_factory.cpp
│   │   │   ├── widgets/                  # 自定义控件
│   │   │   │   ├── main_window.h
│   │   │   │   ├── main_window.cpp
│   │   │   │   ├── display_widget.h
│   │   │   │   ├── display_widget.cpp
│   │   │   │   ├── keypad_widget.h
│   │   │   │   ├── keypad_widget.cpp
│   │   │   │   ├── mode_selector.h
│   │   │   │   ├── mode_selector.cpp
│   │   │   │   ├── history_widget.h
│   │   │   │   ├── history_widget.cpp
│   │   │   │   ├── function_panel.h
│   │   │   │   ├── function_panel.cpp
│   │   │   │   ├── converter_panel.h
│   │   │   │   └── converter_panel.cpp
│   │   │   ├── models/                   # 数据模型
│   │   │   │   ├── history_model.h
│   │   │   │   ├── history_model.cpp
│   │   │   │   ├── mode_model.h
│   │   │   │   └── mode_model.cpp
│   │   │   ├── dialogs/                  # 对话框
│   │   │   │   ├── about_dialog.h
│   │   │   │   ├── about_dialog.cpp
│   │   │   │   ├── settings_dialog.h
│   │   │   │   └── settings_dialog.cpp
│   │   │   └── resources/                # 资源文件
│   │   │       ├── icons.qrc
│   │   │       └── styles.qss
│   │   └── cli/                          # CLI 实现 (现有)
│   └── ...
├── include/calc/
│   └── ui/
│       └── qt/                           # Qt 公共头文件
│           └── calc_engine_adapter.h    # 核心引擎适配器接口
├── tests/
│   └── qt/                               # Qt 单元测试
│       ├── CMakeLists.txt
│       ├── controller_test.cpp
│       ├── widgets_test.cpp
│       └── integration_test.cpp
├── examples/
│   └── qt_example/                       # Qt 示例 (现有)
│       ├── main.cpp
│       ├── mainwindow.h
│       └── mainwindow.cpp
└── cmake/
    └── QtSetup.cmake                     # Qt 集成配置
```

---

## 四、模块详细设计

### 4.1 核心适配层

#### 4.1.1 CalcEngineAdapter

**文件**: `include/calc/ui/qt/calc_engine_adapter.h`

**功能**: 提供与核心计算引擎的适配接口，供 Qt UI 调用。

```cpp
#ifndef CALC_UI_QT_CALC_ENGINE_ADAPTER_H
#define CALC_UI_QT_CALC_ENGINE_ADAPTER_H

#include <calc/modes/mode_manager.h>
#include <calc/core/evaluator.h>
#include <string>
#include <memory>

namespace calc::ui::qt {

/**
 * @brief Qt GUI 适配器，封装核心计算引擎
 *
 * 提供简化的接口供 Qt 组件调用，隐藏核心实现的复杂性。
 * 支持信号/槽机制的异步计算结果返回。
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
```

#### 4.1.2 CalculatorController

**文件**: `src/ui/qt/core/calculator_controller.h`, `.cpp`

**功能**: Qt UI 的业务逻辑控制器，管理计算状态和事件。

```cpp
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
    void evaluateExpression(const QString& expression);
    void switchMode(const QString& modeName);
    void addToHistory(const QString& expression, const QString& result);

    // 配置
    void setPrecision(int precision);
    void setAngleUnit(int unit); // 0=度, 1=弧度

    // 查询
    QStringList getAvailableModes() const;
    QString getCurrentMode() const;
    QStringList getAvailableFunctions() const;

    // 历史记录
    QVariantList getHistory() const;
    void clearHistory();

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
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_CORE_CALCULATOR_CONTROLLER_H
```

### 4.2 UI 组件层

#### 4.2.1 MainWindow

**文件**: `src/ui/qt/widgets/main_window.h`, `.cpp`

**功能**: 主窗口，整合所有 UI 组件。

```cpp
#ifndef CALC_UI_QT_WIDGETS_MAIN_WINDOW_H
#define CALC_UI_QT_WIDGETS_MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <memory>

namespace calc::ui::qt {

class DisplayWidget;
class KeypadWidget;
class ModeSelector;
class HistoryWidget;
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

private slots:
    void onModeChanged(const QString& modeName);
    void onEvaluationFinished(bool success, const QString& result, const QString& errorMsg);
    void onHistoryItemClicked(const QString& expression);
    void onFunctionSelected(const QString& function);
    void onSettingsRequested();
    void onAboutRequested();
    void onExitRequested();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    void updateUIForMode(const QString& modeName);

    // UI Components
    QWidget* centralWidget_;
    DisplayWidget* displayWidget_;
    KeypadWidget* keypadWidget_;
    ModeSelector* modeSelector_;
    HistoryWidget* historyWidget_;
    FunctionPanel* functionPanel_;
    ConverterPanel* converterPanel_;
    QStackedWidget* modeStack_;

    // Controller
    CalculatorController* controller_;

    // State
    QString currentMode_;
    bool isShiftPressed_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_MAIN_WINDOW_H
```

#### 4.2.2 DisplayWidget

**文件**: `src/ui/qt/widgets/display_widget.h`, `.cpp`

**功能**: 显示表达式输入和计算结果。

```cpp
#ifndef CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
#define CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

namespace calc::ui::qt {

/**
 * @brief 显示组件，用于显示表达式和结果
 */
class DisplayWidget : public QWidget {
    Q_OBJECT

public:
    explicit DisplayWidget(QWidget* parent = nullptr);
    ~DisplayWidget() override;

    // 设置显示内容
    void setExpression(const QString& text);
    void setResult(const QString& text);
    void setError(const QString& error);
    void clear();

    // 获取输入
    QString getExpression() const;
    void appendExpression(const QString& text);
    void backspace();

    // 显示状态
    void setErrorState(bool error);
    void setBusyState(bool busy);

    // 格式化
    void setPrecision(int precision);

signals:
    void expressionChanged(const QString& expression);
    void calculateRequested(const QString& expression);
    void clearRequested();

private slots:
    void onTextChanged(const QString& text);
    void onReturnPressed();

private:
    void setupUI();
    void setupConnections();

    QLineEdit* expressionEdit_;
    QLabel* resultLabel_;
    QLabel* statusLabel_;
    bool errorState_;
    int precision_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
```

#### 4.2.3 KeypadWidget

**文件**: `src/ui/qt/widgets/keypad_widget.h`, `.cpp`

**功能**: 计算器键盘，根据不同模式动态布局。

```cpp
#ifndef CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H
#define CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>

namespace calc::ui::qt {

/**
 * @brief 键盘组件，支持多种模式布局
 */
class KeypadWidget : public QWidget {
    Q_OBJECT

public:
    explicit KeypadWidget(QWidget* parent = nullptr);
    ~KeypadWidget() override;

    // 设置键盘布局模式
    enum class LayoutMode {
        Standard,      // 标准模式：数字 + 基本运算
        Scientific,     // 科学模式：增加科学函数
        Programmer      // 程序员模式：进制转换 + 位运算
    };

    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const;

    // 按键操作
    void setButtonEnabled(const QString& key, bool enabled);
    void setButtonVisible(const QString& key, bool visible);

signals:
    void keyPressed(const QString& key);
    void calculateRequested();
    void clearRequested();
    void backspaceRequested();

private slots:
    void onButtonClicked();

private:
    void setupUI();
    void createStandardLayout();
    void createScientificLayout();
    void createProgrammerLayout();
    QPushButton* createButton(const QString& text, const QString& key);

    QGridLayout* layout_;
    QMap<QString, QPushButton*> buttons_;
    LayoutMode currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H
```

#### 4.2.4 ModeSelector

**文件**: `src/ui/qt/widgets/mode_selector.h`, `.cpp`

**功能**: 模式选择器组件。

```cpp
#ifndef CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
#define CALC_UI_QT_WIDGETS_MODE_SELECTOR_H

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>

namespace calc::ui::qt {

/**
 * @brief 模式选择组件
 */
class ModeSelector : public QWidget {
    Q_OBJECT

public:
    explicit ModeSelector(QWidget* parent = nullptr);
    ~ModeSelector() override;

    void setAvailableModes(const QStringList& modes);
    void setCurrentMode(const QString& mode);
    QString getCurrentMode() const;

signals:
    void modeChanged(const QString& mode);

private slots:
    void onModeButtonClicked();

private:
    void setupUI();
    void setupConnections();

    QButtonGroup* modeGroup_;
    QPushButton* addButton_;
    QString currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
```

#### 4.2.5 HistoryWidget

**文件**: `src/ui/qt/widgets/history_widget.h`, `.cpp`

**功能**: 计算历史记录显示和管理。

```cpp
#ifndef CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H
#define CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace calc::ui::qt {

/**
 * @brief 历史记录组件
 */
class HistoryWidget : public QWidget {
    Q_OBJECT

public:
    struct HistoryEntry {
        QString expression;
        QString result;
        QString timestamp;
        QString mode;
    };

    explicit HistoryWidget(QWidget* parent = nullptr);
    ~HistoryWidget() override;

    void addEntry(const HistoryEntry& entry);
    void clear();
    void setHistory(const QList<HistoryEntry>& history);

    QList<HistoryEntry> getHistory() const;

signals:
    void entrySelected(const QString& expression);
    void entryCopied(const QString& text);
    void clearRequested();

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);
    void onClearClicked();
    void onCopyClicked();

private:
    void setupUI();
    void setupConnections();

    QListWidget* historyList_;
    QPushButton* clearButton_;
    QPushButton* copyButton_;
    QList<HistoryEntry> entries_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H
```

#### 4.2.6 FunctionPanel

**文件**: `src/ui/qt/widgets/function_panel.h`, `.cpp`

**功能**: 科学计算模式下的函数面板。

```cpp
#ifndef CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H
#define CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>

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

    QTabWidget* tabWidget_;
    QMap<QString, QPushButton*> functionButtons_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_FUNCTION_PANEL_H
```

#### 4.2.7 ConverterPanel

**文件**: `src/ui/qt/widgets/converter_panel.h`, `.cpp`

**功能**: 程序员模式下的进制转换面板。

```cpp
#ifndef CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H
#define CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace calc::ui::qt {

/**
 * @brief 进制转换面板组件（程序员模式）
 */
class ConverterPanel : public QWidget {
    Q_OBJECT

public:
    explicit ConverterPanel(QWidget* parent = nullptr);
    ~ConverterPanel() override;

    void setValue(qint64 value);
    qint64 getValue() const;

signals:
    void valueChanged(qint64 value);

private slots:
    void onDecimalChanged();
    void onHexChanged();
    void onBinaryChanged();
    void onOctalChanged();

private:
    void setupUI();
    void setupConnections();
    void updateAllFields(qint64 value);

    QLineEdit* decimalEdit_;
    QLineEdit* hexEdit_;
    QLineEdit* binaryEdit_;
    QLineEdit* octalEdit_;
    qint64 currentValue_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H
```

### 4.3 数据模型层

#### 4.3.1 HistoryModel

**文件**: `src/ui/qt/models/history_model.h`, `.cpp`

**功能**: 历史记录的 Qt Model 实现。

```cpp
#ifndef CALC_UI_QT_MODELS_HISTORY_MODEL_H
#define CALC_UI_QT_MODELS_HISTORY_MODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QVector>

namespace calc::ui::qt {

/**
 * @brief 历史记录数据模型
 */
struct HistoryItem {
    QString expression;
    QString result;
    QDateTime timestamp;
    QString mode;
    bool success;
};

class HistoryModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        ExpressionRole = Qt::UserRole + 1,
        ResultRole,
        TimestampRole,
        ModeRole,
        SuccessRole
    };

    explicit HistoryModel(QObject* parent = nullptr);
    ~HistoryModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model operations
    void addItem(const HistoryItem& item);
    void addItems(const QVector<HistoryItem>& items);
    void clear();
    void removeItem(int index);
    HistoryItem getItem(int index) const;

    // Persistence
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

    // Filtering
    void setModeFilter(const QString& mode);
    QString getModeFilter() const;

private:
    QVector<HistoryItem> items_;
    QString modeFilter_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_HISTORY_MODEL_H
```

#### 4.3.2 ModeModel

**文件**: `src/ui/qt/models/mode_model.h`, `.cpp`

**功能**: 计算模式的数据模型。

```cpp
#ifndef CALC_UI_QT_MODELS_MODE_MODEL_H
#define CALC_UI_QT_MODELS_MODE_MODEL_H

#include <QAbstractListModel>
#include <QVector>

namespace calc::ui::qt {

/**
 * @brief 计算模式数据模型
 */
struct ModeInfo {
    QString name;
    QString displayName;
    QString description;
    QString icon;
};

class ModeModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DisplayNameRole,
        DescriptionRole,
        IconRole
    };

    explicit ModeModel(QObject* parent = nullptr);
    ~ModeModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model operations
    void setModes(const QVector<ModeInfo>& modes);
    void addMode(const ModeInfo& mode);
    void clear();

    int findIndex(const QString& name) const;
    ModeInfo getMode(const QString& name) const;
    ModeInfo getModeAt(int index) const;

private:
    QVector<ModeInfo> modes_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_MODE_MODEL_H
```

### 4.4 对话框组件

#### 4.4.1 SettingsDialog

**文件**: `src/ui/qt/dialogs/settings_dialog.h`, `.cpp`

**功能**: 设置对话框。

```cpp
#ifndef CALC_UI_QT_DIALOGS_SETTINGS_DIALOG_H
#define CALC_UI_QT_DIALOGS_SETTINGS_DIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

namespace calc::ui::qt {

/**
 * @brief 设置对话框
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

    // Settings accessors
    void setPrecision(int precision);
    int getPrecision() const;

    void setAngleUnit(int unit); // 0=度, 1=弧度
    int getAngleUnit() const;

    void setShowHistory(bool show);
    bool getShowHistory() const;

    void setHistoryMaxSize(int size);
    int getHistoryMaxSize() const;

signals:
    void settingsChanged();

private slots:
    void onAccepted();
    void onRejected();
    void onRestoreDefaults();

private:
    void setupUI();
    void setupConnections();

    QSpinBox* precisionSpinBox_;
    QComboBox* angleUnitCombo_;
    QCheckBox* showHistoryCheck_;
    QSpinBox* historyMaxSizeSpinBox_;

    // Default values
    static constexpr int DEFAULT_PRECISION = 6;
    static constexpr int DEFAULT_ANGLE_UNIT = 0;
    static constexpr bool DEFAULT_SHOW_HISTORY = true;
    static constexpr int DEFAULT_HISTORY_MAX_SIZE = 100;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_MODE_MODEL_H
```

#### 4.4.2 AboutDialog

**文件**: `src/ui/qt/dialogs/about_dialog.h`, `.cpp`

**功能**: 关于对话框。

```cpp
#ifndef CALC_UI_QT_DIALOGS_ABOUT_DIALOG_H
#define CALC_UI_QT_DIALOGS_ABOUT_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

namespace calc::ui::qt {

/**
 * @brief 关于对话框
 */
class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

private slots:
    void onOkClicked();
    void onLicenseClicked();
    void onQtAboutClicked();

private:
    void setupUI();

    QLabel* titleLabel_;
    QLabel* versionLabel_;
    QLabel* descriptionLabel_;
    QLabel* copyrightLabel_;
    QPushButton* licenseButton_;
    QPushButton* qtButton_;
    QPushButton* okButton_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_MODE_MODEL_H
```

---

## 五、CMake 构建配置

### 5.1 主 CMakeLists.txt 集成

在项目根目录 `CMakeLists.txt` 中添加 Qt GUI 选项：

```cmake
# Qt GUI 选项 (新增)
option(BUILD_QT_GUI "Build Qt GUI application" OFF)

if(BUILD_QT_GUI)
    # 查找 Qt6
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    # 自动 MOC/UIC/RCC
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    # 添加 Qt GUI 子目录
    add_subdirectory(src/ui/qt)

    # 安装 Qt GUI
    install(TARGETS calc_qt_gui DESTINATION bin)
endif()
```

### 5.2 Qt GUI CMakeLists.txt

**文件**: `src/ui/qt/CMakeLists.txt`

```cmake
# Qt GUI CMake Configuration

cmake_minimum_required(VERSION 3.16)

# 项目配置
project(calc_qt_gui VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt 配置
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 自动化
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 头文件目录
target_include_directories(calc_qt_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 收集源文件
set(CORE_SOURCES
    core/calculator_controller.cpp
)

set(WIDGET_SOURCES
    widgets/main_window.cpp
    widgets/display_widget.cpp
    widgets/keypad_widget.cpp
    widgets/mode_selector.cpp
    widgets/history_widget.cpp
    widgets/function_panel.cpp
    widgets/converter_panel.cpp
)

set(MODEL_SOURCES
    models/history_model.cpp
    models/mode_model.cpp
)

set(DIALOG_SOURCES
    dialogs/settings_dialog.cpp
    dialogs/about_dialog.cpp
)

# 创建 Qt GUI 可执行文件
add_executable(calc_qt_gui
    main.cpp
    ${CORE_SOURCES}
    ${WIDGET_SOURCES}
    ${MODEL_SOURCES}
    ${DIALOG_SOURCES}
    resources/icons.qrc
    resources/styles.qss
)

# 链接库
target_link_libraries(calc_qt_gui PRIVATE
    Qt6::Core
    Qt6::Widgets
    calc_ui       # CLI UI 库（共享历史管理等）
    calc_modes    # 计算模式
    calc_core     # 核心引擎
    calc_utils    # 工具类
)

# 设置输出目录
set_target_properties(calc_qt_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.calc.qtgui"
    MACOSX_BUNDLE_BUNDLE_NAME "Calc"
)

# macOS 特定设置
if(APPLE)
    set_target_properties(calc_qt_gui PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "calc.icns"
    )
endif()

# Windows 特定设置
if(WIN32)
    set_target_properties(calc_qt_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
```

### 5.3 资源文件

**文件**: `src/ui/qt/resources/icons.qrc`

```xml
<RCC>
    <qresource prefix="/icons">
        <file>calc_256x256.png</file>
        <file>mode_standard.png</file>
        <file>mode_scientific.png</file>
        <file>mode_programmer.png</file>
    </qresource>
</RCC>
```

**文件**: `src/ui/qt/resources/styles.qss`

```qss
/* Qt GUI 样式表 */
QMainWindow {
    background-color: #f5f5f5;
}

/* 显示区样式 */
QLineEdit#expressionEdit {
    font-size: 24px;
    padding: 10px;
    border: 2px solid #ddd;
    border-radius: 8px;
    background-color: white;
}

QLabel#resultLabel {
    font-size: 32px;
    font-weight: bold;
    color: #2196F3;
    padding: 10px;
}

/* 按钮样式 */
QPushButton {
    font-size: 18px;
    padding: 15px;
    border-radius: 8px;
    background-color: #e0e0e0;
    border: none;
}

QPushButton:hover {
    background-color: #d0d0d0;
}

QPushButton:pressed {
    background-color: #c0c0c0;
}

QPushButton#equalButton {
    background-color: #2196F3;
    color: white;
    font-weight: bold;
}

QPushButton#equalButton:hover {
    background-color: #1976D2;
}

/* 历史记录样式 */
QListWidget#historyList {
    font-size: 14px;
    border: 1px solid #ddd;
    border-radius: 4px;
    background-color: white;
}

QListWidget#historyList::item {
    padding: 8px;
    border-bottom: 1px solid #eee;
}

QListWidget#historyList::item:selected {
    background-color: #E3F2FD;
}

/* 模式选择器样式 */
QRadioButton {
    font-size: 14px;
    padding: 5px;
}

QRadioButton::indicator {
    width: 18px;
    height: 18px;
}
```

---

## 六、信号/槽设计

### 6.1 信号流向图

```
┌─────────────────────────────────────────────────────────────────┐
│                       信号流向设计                                │
└─────────────────────────────────────────────────────────────────┘

用户操作
    │
    ├──> KeypadWidget::keyPressed(key)
    │       │
    │       └──> DisplayWidget::appendExpression(key)
    │               │
    │               └──> CalculatorController::evaluateExpression(expr)
    │                       │
    │                       └──> CalcEngineAdapter::evaluate()
    │                               │
    │                               └──> (核心计算)
    │                                       │
    │                                       └──> CalculatorController::evaluationFinished()
    │                                               │
    │                                               ├──> DisplayWidget::setResult()
    │                                               └──> HistoryWidget::addEntry()
    │
    ├──> ModeSelector::modeChanged(mode)
    │       │
    │       └──> CalculatorController::switchMode(mode)
    │               │
    │               ├──> KeypadWidget::setLayoutMode()
    │               └──> MainWindow::updateUIForMode()
    │
    ├──> HistoryWidget::entrySelected(expression)
    │       │
    │       └──> DisplayWidget::setExpression()
    │               │
    │               └──> CalculatorController::evaluateExpression()
    │
    └──> FunctionPanel::functionSelected(func)
            │
            └──> DisplayWidget::appendExpression(func + "(")
```

### 6.2 信号定义汇总

| 信号 | 发送者 | 接收者 | 参数 | 描述 |
|------|--------|--------|------|------|
| keyPressed | KeypadWidget | DisplayWidget | QString key | 按键按下 |
| calculateRequested | DisplayWidget/KeypadWidget | CalculatorController | QString expr | 请求计算 |
| modeChanged | ModeSelector/CalculatorController | MainWindow/KeypadWidget | QString mode | 模式切换 |
| evaluationFinished | CalculatorController | DisplayWidget/HistoryWidget | bool, QString, QString | 计算完成 |
| historyUpdated | CalculatorController | HistoryWidget | - | 历史更新 |
| entrySelected | HistoryWidget | DisplayWidget | QString expr | 选择历史条目 |
| functionSelected | FunctionPanel | DisplayWidget | QString func | 选择函数 |
| valueChanged | ConverterPanel | CalculatorController | qint64 value | 值改变 |

---

## 七、实施计划

### 7.1 任务分解

| 阶段 | 任务 | 优先级 | 预计工时 |
|------|------|--------|----------|
| **准备阶段** | | | |
| 10.1.1 | 创建 src/ui/qt 目录结构 | P0 | 0.5h |
| 10.1.2 | 修改主 CMakeLists.txt 支持 Qt | P0 | 1h |
| 10.1.3 | 编写 src/ui/qt/CMakeLists.txt | P0 | 1h |
| 10.1.4 | 搭建 Qt 构建环境测试 | P0 | 1h |
| **核心适配层** | | | |
| 10.2.1 | 实现 CalcEngineAdapter | P0 | 3h |
| 10.2.2 | 实现 CalculatorController | P0 | 4h |
| 10.2.3 | 编写核心适配层测试 | P1 | 2h |
| **基础组件** | | | |
| 10.3.1 | 实现 MainWindow | P0 | 4h |
| 10.3.2 | 实现 DisplayWidget | P0 | 3h |
| 10.3.3 | 实现 KeypadWidget (标准布局) | P0 | 3h |
| 10.3.4 | 实现 ModeSelector | P0 | 2h |
| **模式扩展** | | | |
| 10.4.1 | 实现 KeypadWidget 科学布局 | P1 | 2h |
| 10.4.2 | 实现 KeypadWidget 程序员布局 | P1 | 2h |
| 10.4.3 | 实现 FunctionPanel | P1 | 3h |
| 10.4.4 | 实现 ConverterPanel | P1 | 3h |
| **历史记录** | | | |
| 10.5.1 | 实现 HistoryModel | P1 | 2h |
| 10.5.2 | 实现 HistoryWidget | P1 | 3h |
| **对话框** | | | |
| 10.6.1 | 实现 SettingsDialog | P2 | 2h |
| 10.6.2 | 实现 AboutDialog | P2 | 1h |
| **集成与测试** | | | |
| 10.7.1 | 集成所有组件 | P0 | 2h |
| 10.7.2 | 信号/槽连接测试 | P0 | 2h |
| 10.7.3 | 跨平台测试 (Win/Mac/Linux) | P1 | 4h |
| **资源与样式** | | | |
| 10.8.1 | 创建图标资源 | P2 | 2h |
| 10.8.2 | 编写样式表 (QSS) | P2 | 2h |
| 10.8.3 | 主题切换功能 | P3 | 2h |
| **文档** | | | |
| 10.9.1 | API 文档更新 | P1 | 2h |
| 10.9.2 | 用户指南编写 | P1 | 3h |
| 10.9.3 | 部署文档编写 | P2 | 2h |

### 7.2 实施里程碑

| 里程碑 | 描述 | 完成标准 |
|--------|------|----------|
| M1 | 环境搭建完成 | Qt GUI 可编译运行 |
| M2 | 核心适配层完成 | 可通过适配器调用核心引擎 |
| M3 | 标准模式 UI 完成 | 可进行基本四则运算 |
| M4 | 科学模式 UI 完成 | 可使用科学函数 |
| M5 | 程序员模式 UI 完成 | 可进行进制转换 |
| M6 | 历史记录完成 | 可查看和重用历史 |
| M7 | 跨平台测试通过 | Windows/macOS/Linux 均可用 |
| M8 | 文档完成 | API 和用户指南完整 |

### 7.3 依赖关系

```
环境搭建 (10.1)
    │
    ├──> 核心适配层 (10.2)
    │       │
    │       └──> 基础组件 (10.3)
    │               │
    │               ├──> 模式扩展 (10.4)
    │               │       │
    │               │       └──> 集成测试 (10.7.1)
    │               │
    │               └──> 历史记录 (10.5)
    │                       │
    │                       └──> 集成测试 (10.7.1)
    │
    └──> 对话框 (10.6)
            │
            └──> 资源样式 (10.8)
                    │
                    └──> 集成测试 (10.7.2)
                            │
                            └──> 跨平台测试 (10.7.3)
                                    │
                                    └──> 文档 (10.9)
```

---

## 八、UI 原型设计

### 8.1 标准模式 UI

```
┌─────────────────────────────────────────────────────────────┐
│  Calc                                           [Settings][?]│
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Expression:                                               │
│  ┌─────────────────────────────────────────────────────┐  │
│  │ 123 + 456                                         │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
│  Result:                                                   │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                 579                                │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  [Standard]  [Scientific]  [Programmer]                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────┬─────┬─────┬─────┐                               │
│  │  7  │  8  │  9  │  /  │                               │
│  ├─────┼─────┼─────┼─────┤                               │
│  │  4  │  5  │  6  │  *  │                               │
│  ├─────┼─────┼─────┼─────┤                               │
│  │  1  │  2  │  3  │  -  │                               │
│  ├─────┼─────┼─────┼─────┤                               │
│  │  0  │  .  │  =  │  +  │                               │
│  └─────┴─────┴─────┴─────┘                               │
│                                                             │
│  [C]  [CE]  [←]  [()]                                     │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  History                                        [Clear] [x] │
│  ┌─────────────────────────────────────────────────────┐  │
│  │ 123 + 456 = 579                   [12:34:56]        │  │
│  │ 10 * 20 = 200                     [12:30:00]        │  │
│  └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 科学计算模式 UI

```
┌─────────────────────────────────────────────────────────────┐
│  Calc                                           [Settings][?]│
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Expression:                                               │
│  ┌─────────────────────────────────────────────────────┐  │
│  │ sin(45°) + cos(30°)                              │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
│  Result:                                                   │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                1.366025                             │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  [Standard]  [Scientific]  [Programmer]                    │
├─────────────────────────────────────────────────────────────┤
│  ┌─────┬─────┬─────┬─────┬─────┬─────┐                   │
│  │ sin │ cos │ tan │ log │ ln  │ x² │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  7  │  8  │  9  │  /  │ √   │ x! │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  4  │  5  │  6  │  *  │ (   │ )  │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  1  │  2  │  3  │  -  │ π   │ e  │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  0  │  .  │  =  │  +  │ ^   │    │                   │
│  └─────┴─────┴─────┴─────┴─────┴─────┘                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.3 程序员模式 UI

```
┌─────────────────────────────────────────────────────────────┐
│  Calc                                           [Settings][?]│
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  DEC:  ┌─────────────────────────────────────────────────┐ │
│        │                                              255 │ │
│        └─────────────────────────────────────────────────┘ │
│  HEX:  ┌─────────────────────────────────────────────────┐ │
│        │                                              FF  │ │
│        └─────────────────────────────────────────────────┘ │
│  BIN:  ┌─────────────────────────────────────────────────┐ │
│        │                           11111111              │ │
│        └─────────────────────────────────────────────────┘ │
│  OCT:  ┌─────────────────────────────────────────────────┐ │
│        │                                              377 │ │
│        └─────────────────────────────────────────────────┘ │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  [Standard]  [Scientific]  [Programmer]                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────┬─────┬─────┬─────┬─────┬─────┐                   │
│  │ HEX │  A  │  B  │  C  │  D  │  E  │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  7  │  8  │  9  │  /  │ AND │ OR  │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  4  │  5  │  6  │  *  │ XOR │ NOT │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  1  │  2  │  3  │  -  │  << │ >> │                   │
│  ├─────┼─────┼─────┼─────┼─────┼─────┤                   │
│  │  0  │  F  │  =  │  +  │ CLR │ DEL │                   │
│  └─────┴─────┴─────┴─────┴─────┴─────┘                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 九、文件创建清单

### 9.1 核心适配层文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `include/calc/ui/qt/calc_engine_adapter.h` | 头文件 | 核心引擎适配器接口 |
| `src/ui/qt/core/calculator_controller.h` | 头文件 | 计算器控制器 |
| `src/ui/qt/core/calculator_controller.cpp` | 源文件 | 控制器实现 |
| `src/ui/qt/core/mode_widget_factory.h` | 头文件 | 模式部件工厂 |
| `src/ui/qt/core/mode_widget_factory.cpp` | 源文件 | 工厂实现 |

### 9.2 UI 组件文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `src/ui/qt/widgets/main_window.h` | 头文件 | 主窗口 |
| `src/ui/qt/widgets/main_window.cpp` | 源文件 | 主窗口实现 |
| `src/ui/qt/widgets/display_widget.h` | 头文件 | 显示组件 |
| `src/ui/qt/widgets/display_widget.cpp` | 源文件 | 显示组件实现 |
| `src/ui/qt/widgets/keypad_widget.h` | 头文件 | 键盘组件 |
| `src/ui/qt/widgets/keypad_widget.cpp` | 源文件 | 键盘组件实现 |
| `src/ui/qt/widgets/mode_selector.h` | 头文件 | 模式选择器 |
| `src/ui/qt/widgets/mode_selector.cpp` | 源文件 | 模式选择器实现 |
| `src/ui/qt/widgets/history_widget.h` | 头文件 | 历史记录组件 |
| `src/ui/qt/widgets/history_widget.cpp` | 源文件 | 历史记录实现 |
| `src/ui/qt/widgets/function_panel.h` | 头文件 | 函数面板 |
| `src/ui/qt/widgets/function_panel.cpp` | 源文件 | 函数面板实现 |
| `src/ui/qt/widgets/converter_panel.h` | 头文件 | 进制转换面板 |
| `src/ui/qt/widgets/converter_panel.cpp` | 源文件 | 进制转换实现 |

### 9.3 数据模型文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `src/ui/qt/models/history_model.h` | 头文件 | 历史记录模型 |
| `src/ui/qt/models/history_model.cpp` | 源文件 | 历史记录模型实现 |
| `src/ui/qt/models/mode_model.h` | 头文件 | 模式模型 |
| `src/ui/qt/models/mode_model.cpp` | 源文件 | 模式模型实现 |

### 9.4 对话框文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `src/ui/qt/dialogs/settings_dialog.h` | 头文件 | 设置对话框 |
| `src/ui/qt/dialogs/settings_dialog.cpp` | 源文件 | 设置对话框实现 |
| `src/ui/qt/dialogs/about_dialog.h` | 头文件 | 关于对话框 |
| `src/ui/qt/dialogs/about_dialog.cpp` | 源文件 | 关于对话框实现 |

### 9.5 应用入口与配置文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `src/ui/qt/main.cpp` | 源文件 | Qt 应用入口 |
| `src/ui/qt/CMakeLists.txt` | 配置文件 | Qt GUI 构建配置 |
| `src/ui/qt/resources/icons.qrc` | 资源文件 | 图标资源 |
| `src/ui/qt/resources/styles.qss` | 样式文件 | UI 样式表 |

### 9.6 测试文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `tests/qt/CMakeLists.txt` | 配置文件 | Qt 测试配置 |
| `tests/qt/controller_test.cpp` | 源文件 | 控制器测试 |
| `tests/qt/widgets_test.cpp` | 源文件 | 组件测试 |
| `tests/qt/integration_test.cpp` | 源文件 | 集成测试 |

### 9.7 文档文件

| 文件路径 | 类型 | 描述 |
|----------|------|------|
| `docs/GUI_QT_DESIGN.md` | 文档 | 本设计文档 |
| `docs/GUI_QT_API.md` | 文档 | Qt GUI API 文档 |
| `docs/GUI_QT_USER_GUIDE.md` | 文档 | Qt GUI 用户指南 |
| `docs/GUI_QT_DEPLOYMENT.md` | 文档 | 部署文档 |

---

## 十、跨平台部署

### 10.1 Windows 部署

```cmake
# Windows 部署配置
if(WIN32)
    # 使用 windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET calc_qt_gui POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --release
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                $<TARGET_FILE:calc_qt_gui>
            COMMENT "Deploying Qt dependencies"
        )
    endif()
endif()
```

### 10.2 macOS 部署

```cmake
# macOS 部署配置
if(APPLE)
    # 创建 .app bundle
    set_target_properties(calc_qt_gui PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist
    )

    # 使用 macdeployqt
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)

    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_target(deploy_macos
            COMMAND ${MACDEPLOYQT_EXECUTABLE}
                $<TARGET_BUNDLE_DIR:calc_qt_gui>
                -always-overwrite
            DEPENDS calc_qt_gui
            COMMENT "Deploying macOS app bundle"
        )
    endif()
endif()
```

### 10.3 Linux 部署

```cmake
# Linux 部署配置
if(UNIX AND NOT APPLE)
    # 安装桌面文件
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/calc.desktop
        DESTINATION share/applications
    )

    # 安装图标
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/calc.png
        DESTINATION share/icons/hicolor/256x256/apps
    )

    # 安装 AppStream 元数据
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/calc.appdata.xml
        DESTINATION share/metainfo
    )
endif()
```

---

## 十一、未来扩展规划

### 11.1 短期扩展 (1-3个月)

1. **QML 界面支持**
   - 使用 Qt Quick 实现更现代化的 UI
   - 支持动画和手势操作

2. **主题系统**
   - 深色模式/浅色模式切换
   - 自定义主题支持

3. **插件架构**
   - 支持动态加载计算函数插件
   - 用户自定义函数

### 11.2 中期扩展 (3-6个月)

1. **图形绘制**
   - 函数图像绘制
   - 数据可视化

2. **云同步**
   - 历史记录云端同步
   - 跨设备数据共享

3. **多语言支持**
   - i18n 国际化框架
   - 多语言界面

### 11.3 长期规划 (6-12个月)

1. **移动端支持**
   - Qt for Android/iOS 移植
   - 触控优化

2. **WebAssembly**
   - Qt for WebAssembly
   - 浏览器内运行

3. **AI 辅助**
   - 自然语言表达式识别
   - 智能计算建议

---

## 十二、总结

本设计文档为计算器项目的 Qt GUI 扩展提供了完整的架构设计和实施计划。主要特点包括：

1. **清晰的分层架构**: 保持核心引擎与 UI 层的解耦
2. **模块化设计**: 便于维护和扩展
3. **跨平台支持**: 统一的 CMake 配置支持三大平台
4. **信号/槽机制**: 充分利用 Qt 的特性实现组件通信
5. **详细的实施计划**: 包含任务分解、里程碑和依赖关系

通过本设计，开发者可以按照文档逐步实现 Qt GUI，在保持核心计算逻辑不变的前提下，为用户提供直观、美观、易用的图形界面。
