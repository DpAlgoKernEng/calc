/**
 * @file function_panel.cpp
 * @brief 函数面板组件实现
 */

#include "calc/ui/qt/widgets/function_panel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QFont>

namespace calc::ui::qt {

FunctionPanel::FunctionPanel(QWidget* parent)
    : QWidget(parent)
    , tabWidget_(nullptr)
{
    setupUI();
}

FunctionPanel::~FunctionPanel() = default;

void FunctionPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // 标签页组件
    tabWidget_ = new QTabWidget(this);
    layout->addWidget(tabWidget_);

    // 创建各种函数选项卡
    createTrigFunctions();
    createLogFunctions();
    createOtherFunctions();
}

void FunctionPanel::createTrigFunctions()
{
    auto* tab = new QWidget(tabWidget_);

    auto* gridLayout = new QGridLayout(tab);
    gridLayout->setSpacing(4);

    const QStringList trigFuncs = {
        "sin", "cos", "tan",
        "asin", "acos", "atan",
        "sinh", "cosh", "tanh",
        "deg", "rad"
    };

    for (int i = 0; i < trigFuncs.size(); ++i) {
        QPushButton* btn = createFunctionButton(trigFuncs[i], trigFuncs[i]);
        int row = i / 3;
        int col = i % 3;
        gridLayout->addWidget(btn, row, col);
    }

    tabWidget_->addTab(tab, "Trig");
}

void FunctionPanel::createLogFunctions()
{
    auto* tab = new QWidget(tabWidget_);

    auto* gridLayout = new QGridLayout(tab);
    gridLayout->setSpacing(4);

    const QStringList logFuncs = {
        "log", "log10", "ln",
        "exp", "sqrt", "cbrt",
        "pow", "^", "root"
    };

    for (int i = 0; i < logFuncs.size(); ++i) {
        QPushButton* btn = createFunctionButton(logFuncs[i], logFuncs[i]);
        int row = i / 3;
        int col = i % 3;
        gridLayout->addWidget(btn, row, col);
    }

    tabWidget_->addTab(tab, "Log");
}

void FunctionPanel::createOtherFunctions()
{
    auto* tab = new QWidget(tabWidget_);

    auto* gridLayout = new QGridLayout(tab);
    gridLayout->setSpacing(4);

    const QStringList otherFuncs = {
        "abs", "floor", "ceil",
        "round", "trunc", "sign",
        "min", "max", "avg"
    };

    for (int i = 0; i < otherFuncs.size(); ++i) {
        QPushButton* btn = createFunctionButton(otherFuncs[i], otherFuncs[i]);
        int row = i / 3;
        int col = i % 3;
        gridLayout->addWidget(btn, row, col);
    }

    tabWidget_->addTab(tab, "Other");
}

QPushButton* FunctionPanel::createFunctionButton(const QString& text, const QString& function)
{
    auto* btn = new QPushButton(text, this);
    btn->setMinimumHeight(50);

    btn->setMinimumWidth(60);
    QFont font = btn->font();
    font.setPointSize(11);
    btn->setFont(font);


    // 移除内联样式，使用 QSS
    // 设置紫色渐变样式用于科学函数
    btn->setProperty("buttonType", "functionPurple");
    btn->setProperty("function", function);
    functionButtons_[function] = btn;

    connect(btn, &QPushButton::clicked, this, &FunctionPanel::onFunctionButtonClicked);

    return btn;
}

void FunctionPanel::setAvailableFunctions(const QStringList& functions)
{
    // 禁用不可用的函数按钮
    for (const QString& func : functionButtons_.keys()) {
        bool available = functions.contains(func);
        functionButtons_[func]->setEnabled(available);
    }
}

void FunctionPanel::addFunctionCategory(const QString& category, const QStringList& functions)
{
    auto* tab = new QWidget(tabWidget_);
    auto* gridLayout = new QGridLayout(tab);
    gridLayout->setSpacing(4);

    int cols = 3;
    for (int i = 0; i < functions.size(); ++i) {
        QPushButton* btn = createFunctionButton(functions[i], functions[i]);
        int row = i / cols;
        int col = i % cols;
        gridLayout->addWidget(btn, row, col);
    }

    tabWidget_->addTab(tab, category);
}

void FunctionPanel::onFunctionButtonClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        QString function = btn->property("function").toString();
        emit functionSelected(function);
    }
}

} // namespace calc::ui::qt
