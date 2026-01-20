/**
 * @file display_widget.cpp
 * @brief 显示组件实现
 */

#include "calc/ui/qt/widgets/display_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QPalette>
#include <QEvent>
#include <QStyle>

namespace calc::ui::qt {

DisplayWidget::DisplayWidget(QWidget* parent)
    : QWidget(parent)
    , expressionLabel_(nullptr)
    , resultLabel_(nullptr)
    , statusLabel_(nullptr)
    , errorState_(false)
    , precision_(6)
{
    setupUI();
}

DisplayWidget::~DisplayWidget() = default;

void DisplayWidget::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // 表达式预览 (小字体，顶部) - 改为只读标签
    expressionLabel_ = new QLabel("", this);
    expressionLabel_->setObjectName("expressionLabel");
    expressionLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    expressionLabel_->setMinimumHeight(24);
    expressionLabel_->setWordWrap(true);

    QFont exprFont = expressionLabel_->font();
    exprFont.setPointSize(14);
    exprFont.setFamily("SF Mono, Monaco, Consolas, monospace");
    expressionLabel_->setFont(exprFont);

    layout->addWidget(expressionLabel_);

    // 主结果 (大字体，底部，右对齐)
    resultLabel_ = new QLabel("0", this);
    resultLabel_->setObjectName("resultLabel");
    resultLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    resultLabel_->setMinimumHeight(80);

    QFont resultFont = resultLabel_->font();
    resultFont.setPointSize(48);
    resultFont.setBold(true);
    resultFont.setWeight(QFont::Black);
    resultLabel_->setFont(resultFont);

    layout->addWidget(resultLabel_);

    // 状态标签 (隐藏，玻璃拟态设计不需要)
    statusLabel_ = new QLabel("", this);
    statusLabel_->setObjectName("statusLabel");
    statusLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statusLabel_->setMinimumHeight(20);
    statusLabel_->hide();  // 隐藏以获得更干净的玻璃拟态设计

    layout->addWidget(statusLabel_);

    // 移除内联样式，使用 QSS
    // Styles are now in default.qss
}

void DisplayWidget::setExpression(const QString& text)
{
    // 表达式显示在预览标签中（小字体，顶部）
    expressionLabel_->setText(text);
}

void DisplayWidget::setResult(const QString& text)
{
    resultLabel_->setText(text);
    statusLabel_->clear();
}

void DisplayWidget::setError(const QString& error)
{
    statusLabel_->setText(error);
    statusLabel_->setStyleSheet("color: #F44336;");
}

void DisplayWidget::clear()
{
    expressionLabel_->clear();
    resultLabel_->setText("0");
    statusLabel_->clear();
    statusLabel_->setStyleSheet("color: #757575;");
    setErrorState(false);
}

QString DisplayWidget::getExpression() const
{
    // 返回当前输入，即 resultLabel 的内容
    return resultLabel_->text();
}

void DisplayWidget::appendExpression(const QString& text)
{
    // 添加到结果显示
    QString current = resultLabel_->text();
    if (current == "0" && !text.contains('.')) {
        resultLabel_->setText(text);
    } else {
        resultLabel_->setText(current + text);
    }
}

void DisplayWidget::backspace()
{
    // 删除最后一个字符
    QString current = resultLabel_->text();
    if (current.length() > 1) {
        resultLabel_->setText(current.left(current.length() - 1));
    } else {
        resultLabel_->setText("0");
    }
}

void DisplayWidget::setErrorState(bool error)
{
    errorState_ = error;
    // 使用 QSS 样式而不是内联样式
    // 样式在 default.qss 中定义
    if (error) {
        resultLabel_->setProperty("error", true);
        resultLabel_->style()->unpolish(resultLabel_);
        resultLabel_->style()->polish(resultLabel_);
    } else {
        resultLabel_->setProperty("error", false);
        resultLabel_->style()->unpolish(resultLabel_);
        resultLabel_->style()->polish(resultLabel_);
    }
}

void DisplayWidget::setBusyState(bool busy)
{
    setEnabled(!busy);
}

void DisplayWidget::setPrecision(int precision)
{
    precision_ = precision;
}

} // namespace calc::ui::qt
