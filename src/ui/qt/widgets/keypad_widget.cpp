/**
 * @file keypad_widget.cpp
 * @brief 键盘组件实现
 */

#include "calc/ui/qt/widgets/keypad_widget.h"

#include <QGridLayout>
#include <QFont>
#include <QSize>
#include <QPropertyAnimation>
#include <QEasingCurve>

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

    // 标准模式使用5列布局
    // 列布局: [7][8][9][/][DEL]
    //        [4][5][6][*][%]
    //        [1][2][3][-][1/x]
    //        [0][.][=][+][±]

    // 第一行
    QPushButton* btn7 = createButton("7", "7");
    QPushButton* btn8 = createButton("8", "8");
    QPushButton* btn9 = createButton("9", "9");
    QPushButton* divBtn = createButton("÷", "/");
    QPushButton* delBtn = createButton("DEL", "backspace");

    layout_->addWidget(btn7, 0, 0);
    layout_->addWidget(btn8, 0, 1);
    layout_->addWidget(btn9, 0, 2);
    layout_->addWidget(divBtn, 0, 3);
    layout_->addWidget(delBtn, 0, 4);

    // 第二行
    QPushButton* btn4 = createButton("4", "4");
    QPushButton* btn5 = createButton("5", "5");
    QPushButton* btn6 = createButton("6", "6");
    QPushButton* mulBtn = createButton("×", "*");
    QPushButton* percentBtn = createButton("%", "%");

    layout_->addWidget(btn4, 1, 0);
    layout_->addWidget(btn5, 1, 1);
    layout_->addWidget(btn6, 1, 2);
    layout_->addWidget(mulBtn, 1, 3);
    layout_->addWidget(percentBtn, 1, 4);

    // 第三行
    QPushButton* btn1 = createButton("1", "1");
    QPushButton* btn2 = createButton("2", "2");
    QPushButton* btn3 = createButton("3", "3");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* invBtn = createButton("1/x", "1/x");

    layout_->addWidget(btn1, 2, 0);
    layout_->addWidget(btn2, 2, 1);
    layout_->addWidget(btn3, 2, 2);
    layout_->addWidget(subBtn, 2, 3);
    layout_->addWidget(invBtn, 2, 4);

    // 第四行
    QPushButton* btn0 = createButton("0", "0");
    QPushButton* dotBtn = createButton(".", ".");
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");
    QPushButton* addBtn = createButton("+", "+");
    QPushButton* negBtn = createButton("±", "negate");

    layout_->addWidget(btn0, 3, 0);
    layout_->addWidget(dotBtn, 3, 1);
    layout_->addWidget(eqBtn, 3, 2);
    layout_->addWidget(addBtn, 3, 3);
    layout_->addWidget(negBtn, 3, 4);

    // 第五行 - 功能键
    QPushButton* clearBtn = createButton("AC", "clear");
    QPushButton* leftParenBtn = createButton("(", "(");
    QPushButton* rightParenBtn = createButton(")", ")");
    QPushButton* sqrtBtn = createButton("√", "sqrt");
    QPushButton* squareBtn = createButton("x²", "pow2");

    layout_->addWidget(clearBtn, 4, 0);
    layout_->addWidget(leftParenBtn, 4, 1);
    layout_->addWidget(rightParenBtn, 4, 2);
    layout_->addWidget(sqrtBtn, 4, 3);
    layout_->addWidget(squareBtn, 4, 4);
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

    // 科学模式使用8列布局
    // 布局: [sin][cos][tan][log][ln][√][^][(]

    // 第一行: 三角函数和对数
    QPushButton* sinBtn = createButton("sin", "sin");
    QPushButton* cosBtn = createButton("cos", "cos");
    QPushButton* tanBtn = createButton("tan", "tan");
    QPushButton* logBtn = createButton("log", "log");
    QPushButton* lnBtn = createButton("ln", "ln");
    QPushButton* sqrtBtn = createButton("√", "sqrt");
    QPushButton* powBtn = createButton("xʸ", "^");
    QPushButton* leftParenBtn = createButton("(", "(");

    layout_->addWidget(sinBtn, 0, 0);
    layout_->addWidget(cosBtn, 0, 1);
    layout_->addWidget(tanBtn, 0, 2);
    layout_->addWidget(logBtn, 0, 3);
    layout_->addWidget(lnBtn, 0, 4);
    layout_->addWidget(sqrtBtn, 0, 5);
    layout_->addWidget(powBtn, 0, 6);
    layout_->addWidget(leftParenBtn, 0, 7);

    // 第二行: 更多函数
    QPushButton* asinBtn = createButton("sin⁻¹", "asin");
    QPushButton* acosBtn = createButton("cos⁻¹", "acos");
    QPushButton* atanBtn = createButton("tan⁻¹", "atan");
    QPushButton* absBtn = createButton("|x|", "abs");
    QPushButton* expBtn = createButton("eˣ", "exp");
    QPushButton* piBtn = createButton("π", "PI");
    QPushButton* eBtn = createButton("e", "E");
    QPushButton* rightParenBtn = createButton(")", ")");

    layout_->addWidget(asinBtn, 1, 0);
    layout_->addWidget(acosBtn, 1, 1);
    layout_->addWidget(atanBtn, 1, 2);
    layout_->addWidget(absBtn, 1, 3);
    layout_->addWidget(expBtn, 1, 4);
    layout_->addWidget(piBtn, 1, 5);
    layout_->addWidget(eBtn, 1, 6);
    layout_->addWidget(rightParenBtn, 1, 7);

    // 第三行: 数字
    QPushButton* btn7 = createButton("7", "7");
    QPushButton* btn8 = createButton("8", "8");
    QPushButton* btn9 = createButton("9", "9");
    QPushButton* delBtn = createButton("DEL", "backspace");

    layout_->addWidget(btn7, 2, 0);
    layout_->addWidget(btn8, 2, 1);
    layout_->addWidget(btn9, 2, 2);
    layout_->addWidget(delBtn, 2, 3);

    // 第四行
    QPushButton* btn4 = createButton("4", "4");
    QPushButton* btn5 = createButton("5", "5");
    QPushButton* btn6 = createButton("6", "6");
    QPushButton* divBtn = createButton("÷", "/");

    layout_->addWidget(btn4, 3, 0);
    layout_->addWidget(btn5, 3, 1);
    layout_->addWidget(btn6, 3, 2);
    layout_->addWidget(divBtn, 3, 3);

    // 第五行
    QPushButton* btn1 = createButton("1", "1");
    QPushButton* btn2 = createButton("2", "2");
    QPushButton* btn3 = createButton("3", "3");
    QPushButton* mulBtn = createButton("×", "*");

    layout_->addWidget(btn1, 4, 0);
    layout_->addWidget(btn2, 4, 1);
    layout_->addWidget(btn3, 4, 2);
    layout_->addWidget(mulBtn, 4, 3);

    // 第六行
    QPushButton* btn0 = createButton("0", "0");
    QPushButton* dotBtn = createButton(".", ".");
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* addBtn = createButton("+", "+");
    QPushButton* clearBtn = createButton("AC", "clear");

    layout_->addWidget(btn0, 5, 0);
    layout_->addWidget(dotBtn, 5, 1);
    layout_->addWidget(eqBtn, 5, 2);
    layout_->addWidget(subBtn, 5, 3);
    layout_->addWidget(addBtn, 5, 4);
    layout_->addWidget(clearBtn, 5, 5);
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

    // 程序员模式使用6列布局
    // 布局: [HEX][A][B][C][D][E][F]

    // 第一行: 十六进制字符 A-F
    QPushButton* hexLabelBtn = createButton("HEX", "hex_label");
    hexLabelBtn->setEnabled(false);
    QPushButton* btnA = createButton("A", "A");
    QPushButton* btnB = createButton("B", "B");
    QPushButton* btnC = createButton("C", "C");
    QPushButton* btnD = createButton("D", "D");
    QPushButton* btnE = createButton("E", "E");
    QPushButton* btnF = createButton("F", "F");

    layout_->addWidget(hexLabelBtn, 0, 0);
    layout_->addWidget(btnA, 0, 1);
    layout_->addWidget(btnB, 0, 2);
    layout_->addWidget(btnC, 0, 3);
    layout_->addWidget(btnD, 0, 4);
    layout_->addWidget(btnE, 0, 5);
    layout_->addWidget(btnF, 0, 6);

    // 第二行: 7, 8, 9, OR
    QPushButton* btn7 = createButton("7", "7");
    QPushButton* btn8 = createButton("8", "8");
    QPushButton* btn9 = createButton("9", "9");
    QPushButton* andBtn = createButton("AND", "&");

    layout_->addWidget(btn7, 1, 0);
    layout_->addWidget(btn8, 1, 1);
    layout_->addWidget(btn9, 1, 2);
    layout_->addWidget(andBtn, 1, 3);

    // 第三行: 4, 5, 6, OR
    QPushButton* btn4 = createButton("4", "4");
    QPushButton* btn5 = createButton("5", "5");
    QPushButton* btn6 = createButton("6", "6");
    QPushButton* orBtn = createButton("OR", "|");

    layout_->addWidget(btn4, 2, 0);
    layout_->addWidget(btn5, 2, 1);
    layout_->addWidget(btn6, 2, 2);
    layout_->addWidget(orBtn, 2, 3);

    // 第四行: 1, 2, 3, XOR
    QPushButton* btn1 = createButton("1", "1");
    QPushButton* btn2 = createButton("2", "2");
    QPushButton* btn3 = createButton("3", "3");
    QPushButton* xorBtn = createButton("XOR", "^");

    layout_->addWidget(btn1, 3, 0);
    layout_->addWidget(btn2, 3, 1);
    layout_->addWidget(btn3, 3, 2);
    layout_->addWidget(xorBtn, 3, 3);

    // 第五行: 0, NOT, LSH, RSH
    QPushButton* btn0 = createButton("0", "0");
    QPushButton* notBtn = createButton("NOT", "~");
    QPushButton* lshiftBtn = createButton("LSH", "<<");
    QPushButton* rshiftBtn = createButton("RSH", ">>");

    layout_->addWidget(btn0, 4, 0);
    layout_->addWidget(notBtn, 4, 1);
    layout_->addWidget(lshiftBtn, 4, 2);
    layout_->addWidget(rshiftBtn, 4, 3);

    // 第六行: 操作符
    QPushButton* addBtn = createButton("+", "+");
    QPushButton* subBtn = createButton("-", "-");
    QPushButton* mulBtn = createButton("×", "*");
    QPushButton* divBtn = createButton("÷", "/");

    layout_->addWidget(addBtn, 5, 0);
    layout_->addWidget(subBtn, 5, 1);
    layout_->addWidget(mulBtn, 5, 2);
    layout_->addWidget(divBtn, 5, 3);

    // 第七行: 功能键
    QPushButton* dotBtn = createButton(".", ".");
    QPushButton* leftParenBtn = createButton("(", "(");
    QPushButton* rightParenBtn = createButton(")", ")");
    QPushButton* eqBtn = createButton("=", "=");
    eqBtn->setObjectName("equalButton");
    QPushButton* clearBtn = createButton("AC", "clear");
    QPushButton* delBtn = createButton("DEL", "backspace");

    layout_->addWidget(dotBtn, 6, 0);
    layout_->addWidget(leftParenBtn, 6, 1);
    layout_->addWidget(rightParenBtn, 6, 2);
    layout_->addWidget(eqBtn, 6, 3);
    layout_->addWidget(clearBtn, 6, 4);
    layout_->addWidget(delBtn, 6, 5);
}

QPushButton* KeypadWidget::createButton(const QString& text, const QString& key)
{
    auto* btn = new QPushButton(text, this);
    btn->setMinimumHeight(64);
    btn->setMinimumWidth(64);

    QFont font = btn->font();
    font.setPointSize(18);
    font.setWeight(QFont::DemiBold);
    btn->setFont(font);

    // 移除内联样式，使用 QSS
    // 设置按钮类型属性用于样式
    if (key == "=") {
        btn->setObjectName("equalButton");
        btn->setProperty("buttonType", "calcButton");
    } else if (key == "backspace" || key == "DEL") {
        btn->setProperty("buttonType", "functionGray");
    } else if (key == "clear" || key == "CE") {
        btn->setProperty("buttonType", "functionGray");
    } else if (QStringList({"+", "-", "*", "/", "×", "÷"}).contains(key)) {
        btn->setProperty("buttonType", "operator");
    } else if (QStringList({"%", "1/x", "negate"}).contains(key)) {
        btn->setProperty("buttonType", "functionGray");
    } else {
        btn->setProperty("buttonType", "calcButton");
    }

    // 安装事件过滤器用于悬停动画
    btn->installEventFilter(this);

    btn->setProperty("key", key);
    buttons_[key] = btn;

    connect(btn, &QPushButton::clicked, this, &KeypadWidget::onButtonClicked);

    return btn;
}

bool KeypadWidget::eventFilter(QObject* watched, QEvent* event)
{
    auto* button = qobject_cast<QPushButton*>(watched);
    if (!button) return QWidget::eventFilter(watched, event);

    if (event->type() == QEvent::Enter) {
        // 悬停时放大
        QRect geo = button->geometry();
        QPropertyAnimation* anim = new QPropertyAnimation(button, "geometry");
        anim->setDuration(200);
        anim->setStartValue(geo);
        anim->setEndValue(geo.adjusted(-3, -3, 3, 3));  // 放大1.05倍
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QPropertyAnimation::DeleteWhenStopped);
        return true;
    }
    else if (event->type() == QEvent::Leave) {
        // 离开时恢复
        QRect geo = button->geometry();
        QPropertyAnimation* anim = new QPropertyAnimation(button, "geometry");
        anim->setDuration(200);
        anim->setStartValue(geo);
        anim->setEndValue(geo.adjusted(3, 3, -3, -3));
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QPropertyAnimation::DeleteWhenStopped);
        return true;
    }

    return QWidget::eventFilter(watched, event);
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
    } else if (key == "hex_label") {
        // 标签按钮，不处理
        return;
    } else if (key == "PI") {
        emit keyPressed("3.141592653589793");
    } else if (key == "E") {
        emit keyPressed("2.718281828459045");
    } else if (key == "sqrt") {
        emit keyPressed("sqrt(");
    } else if (key == "pow2") {
        emit keyPressed("^2");
    } else if (key == "1/x") {
        emit keyPressed("1/(");
    } else if (key == "negate") {
        emit keyPressed("(-");
    } else if (key == "asin") {
        emit keyPressed("asin(");
    } else if (key == "acos") {
        emit keyPressed("acos(");
    } else if (key == "atan") {
        emit keyPressed("atan(");
    } else if (key == "abs") {
        emit keyPressed("abs(");
    } else if (key == "exp") {
        emit keyPressed("exp(");
    } else if (key == "sin" || key == "cos" || key == "tan" || key == "log" || key == "ln") {
        emit keyPressed(key + "(");
    } else {
        emit keyPressed(key);
    }
}

} // namespace calc::ui::qt
