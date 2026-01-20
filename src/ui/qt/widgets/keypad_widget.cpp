/**
 * @file keypad_widget.cpp
 * @brief 键盘组件实现
 */

#include "calc/ui/qt/widgets/keypad_widget.h"

#include <QGridLayout>
#include <QFont>
#include <QSize>

namespace calc::ui::qt {

KeypadWidget::KeypadWidget(QWidget* parent)
    : QWidget(parent)
    , layout_(nullptr)
    , currentMode_(LayoutMode::Standard)
{
    setupUI();
}

KeypadWidget::~KeypadWidget() = default;

void KeypadWidget::setupUI()
{
    layout_ = new QGridLayout(this);
    layout_->setSpacing(4);
    layout_->setContentsMargins(0, 0, 0, 0);

    createStandardLayout();
}

void KeypadWidget::createStandardLayout()
{
    // 清除现有布局
    while (layout_->count()) {
        QLayoutItem* item = layout_->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    buttons_.clear();

    // 数字按钮
    const QStringList numbers = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0"};
    for (int i = 0; i < numbers.size(); ++i) {
        QPushButton* btn = createButton(numbers[i], numbers[i]);
        int row = i / 3;
        int col = i % 3;
        layout_->addWidget(btn, row, col);
    }

    // 运算符按钮
    QPushButton* divBtn = createButton("/", "/");
    QPushButton* mulBtn = createButton("*", "*");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* addBtn = createButton("+", "+");

    layout_->addWidget(divBtn, 0, 3);
    layout_->addWidget(mulBtn, 1, 3);
    layout_->addWidget(subBtn, 2, 3);
    layout_->addWidget(addBtn, 3, 3);

    // 小数点和等号
    QPushButton* dotBtn = createButton(".", ".");
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");

    layout_->addWidget(dotBtn, 3, 1);
    layout_->addWidget(eqBtn, 3, 2);

    // 功能按钮
    QPushButton* clearBtn = createButton("C", "clear");
    QPushButton* ceBtn = createButton("CE", "ce");
    QPushButton* backBtn = createButton("←", "backspace");
    QPushButton* parenBtn = createButton("()", "paren");

    layout_->addWidget(clearBtn, 4, 0);
    layout_->addWidget(ceBtn, 4, 1);
    layout_->addWidget(backBtn, 4, 2);
    layout_->addWidget(parenBtn, 4, 3);

    // 添加跨列的小数点按钮
    layout_->addWidget(dotBtn, 3, 1, 1, 2);
}

void KeypadWidget::createScientificLayout()
{
    // 清除现有布局
    while (layout_->count()) {
        QLayoutItem* item = layout_->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    buttons_.clear();

    // 科学函数按钮 (第一行)
    const QStringList funcs = {"sin", "cos", "tan", "log", "ln"};
    for (int i = 0; i < funcs.size(); ++i) {
        QPushButton* btn = createButton(funcs[i], funcs[i]);
        layout_->addWidget(btn, 0, i);
    }

    // 数字和运算符
    const QStringList numbers = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0"};
    for (int i = 0; i < numbers.size(); ++i) {
        QPushButton* btn = createButton(numbers[i], numbers[i]);
        int row = (i / 3) + 1;
        int col = i % 3;
        layout_->addWidget(btn, row, col);
    }

    // 运算符
    QPushButton* divBtn = createButton("/", "/");
    QPushButton* mulBtn = createButton("*", "*");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* addBtn = createButton("+", "+");

    layout_->addWidget(divBtn, 1, 3);
    layout_->addWidget(mulBtn, 2, 3);
    layout_->addWidget(subBtn, 3, 3);
    layout_->addWidget(addBtn, 4, 3);

    // 小数点和等号
    QPushButton* dotBtn = createButton(".", ".");
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");

    layout_->addWidget(dotBtn, 4, 1);
    layout_->addWidget(eqBtn, 4, 2);

    // 额外函数
    QPushButton* powBtn = createButton("^", "^");
    QPushButton* sqrtBtn = createButton("√", "sqrt");
    QPushButton* piBtn = createButton("π", "PI");
    QPushButton* eBtn = createButton("e", "E");

    layout_->addWidget(powBtn, 0, 5);
    layout_->addWidget(sqrtBtn, 1, 5);
    layout_->addWidget(piBtn, 2, 5);
    layout_->addWidget(eBtn, 3, 5);

    // 功能按钮
    QPushButton* clearBtn = createButton("C", "clear");
    QPushButton* backBtn = createButton("←", "backspace");
    QPushButton* leftParenBtn = createButton("(", "(");
    QPushButton* rightParenBtn = createButton(")", ")");

    layout_->addWidget(clearBtn, 5, 0);
    layout_->addWidget(backBtn, 5, 1);
    layout_->addWidget(leftParenBtn, 5, 2);
    layout_->addWidget(rightParenBtn, 5, 3);
}

void KeypadWidget::createProgrammerLayout()
{
    // 清除现有布局
    while (layout_->count()) {
        QLayoutItem* item = layout_->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    buttons_.clear();

    // 十六进制字符
    const QStringList hexChars = {"A", "B", "C", "D", "E", "F"};
    for (int i = 0; i < hexChars.size(); ++i) {
        QPushButton* btn = createButton(hexChars[i], hexChars[i]);
        layout_->addWidget(btn, 0, i);
    }

    // 数字
    const QStringList numbers = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0"};
    for (int i = 0; i < numbers.size(); ++i) {
        QPushButton* btn = createButton(numbers[i], numbers[i]);
        int row = (i / 3) + 1;
        int col = i % 3;
        layout_->addWidget(btn, row, col);
    }

    // 运算符
    QPushButton* divBtn = createButton("/", "/");
    QPushButton* mulBtn = createButton("*", "*");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* addBtn = createButton("+", "+");

    layout_->addWidget(divBtn, 1, 3);
    layout_->addWidget(mulBtn, 2, 3);
    layout_->addWidget(subBtn, 3, 3);
    layout_->addWidget(addBtn, 4, 3);

    // 位运算
    QPushButton* andBtn = createButton("AND", "&");
    QPushButton* orBtn = createButton("OR", "|");
    QPushButton* xorBtn = createButton("XOR", "^");
    QPushButton* notBtn = createButton("NOT", "~");
    QPushButton* lshiftBtn = createButton("<<", "<<");
    QPushButton* rshiftBtn = createButton(">>", ">>");

    layout_->addWidget(andBtn, 0, 4);
    layout_->addWidget(orBtn, 1, 4);
    layout_->addWidget(xorBtn, 2, 4);
    layout_->addWidget(notBtn, 3, 4);
    layout_->addWidget(lshiftBtn, 4, 4);
    layout_->addWidget(rshiftBtn, 5, 4);

    // 等号和清除
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");
    QPushButton* clearBtn = createButton("CLR", "clear");
    QPushButton* delBtn = createButton("DEL", "backspace");

    layout_->addWidget(eqBtn, 5, 0);
    layout_->addWidget(clearBtn, 5, 1);
    layout_->addWidget(delBtn, 5, 2);
}

QPushButton* KeypadWidget::createButton(const QString& text, const QString& key)
{
    auto* btn = new QPushButton(text, this);
    btn->setMinimumHeight(50);

    QFont font = btn->font();
    font.setPointSize(16);
    btn->setFont(font);

    btn->setProperty("key", key);
    buttons_[key] = btn;

    connect(btn, &QPushButton::clicked, this, &KeypadWidget::onButtonClicked);

    return btn;
}

void KeypadWidget::setLayoutMode(LayoutMode mode)
{
    currentMode_ = mode;

    switch (mode) {
        case LayoutMode::Standard:
            createStandardLayout();
            break;
        case LayoutMode::Scientific:
            createScientificLayout();
            break;
        case LayoutMode::Programmer:
            createProgrammerLayout();
            break;
    }
}

KeypadWidget::LayoutMode KeypadWidget::getLayoutMode() const
{
    return currentMode_;
}

void KeypadWidget::setButtonEnabled(const QString& key, bool enabled)
{
    if (buttons_.contains(key)) {
        buttons_[key]->setEnabled(enabled);
    }
}

void KeypadWidget::setButtonVisible(const QString& key, bool visible)
{
    if (buttons_.contains(key)) {
        buttons_[key]->setVisible(visible);
    }
}

void KeypadWidget::onButtonClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString key = btn->property("key").toString();

    if (key == "clear" || key == "CE") {
        emit clearRequested();
    } else if (key == "backspace" || key == "DEL") {
        emit backspaceRequested();
    } else if (key == "=") {
        emit calculateRequested();
    } else if (key == "paren") {
        emit keyPressed("(");
    } else if (key == "PI") {
        emit keyPressed("3.141592653589793");
    } else if (key == "E") {
        emit keyPressed("2.718281828459045");
    } else if (key == "sqrt") {
        emit keyPressed("sqrt(");
    } else {
        emit keyPressed(key);
    }
}

} // namespace calc::ui::qt
