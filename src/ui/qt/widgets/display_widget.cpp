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

namespace calc::ui::qt {

DisplayWidget::DisplayWidget(QWidget* parent)
    : QWidget(parent)
    , expressionEdit_(nullptr)
    , resultLabel_(nullptr)
    , statusLabel_(nullptr)
    , errorState_(false)
    , precision_(6)
{
    setupUI();
    setupConnections();
}

DisplayWidget::~DisplayWidget() = default;

void DisplayWidget::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    // 结果显示标签
    resultLabel_ = new QLabel("", this);
    resultLabel_->setObjectName("resultLabel");
    resultLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    resultLabel_->setMinimumHeight(60);

    QFont resultFont = resultLabel_->font();
    resultFont.setPointSize(28);
    resultFont.setBold(true);
    resultLabel_->setFont(resultFont);

    layout->addWidget(resultLabel_);

    // 表达式输入框
    expressionEdit_ = new QLineEdit("", this);
    expressionEdit_->setObjectName("expressionEdit");
    expressionEdit_->setMinimumHeight(50);
    expressionEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    expressionEdit_->setPlaceholderText("Enter expression...");

    QFont exprFont = expressionEdit_->font();
    exprFont.setPointSize(20);
    expressionEdit_->setFont(exprFont);

    layout->addWidget(expressionEdit_);

    // 状态标签
    statusLabel_ = new QLabel("", this);
    statusLabel_->setObjectName("statusLabel");
    statusLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statusLabel_->setMinimumHeight(20);
    statusLabel_->setWordWrap(true);

    QFont statusFont = statusLabel_->font();
    statusFont.setPointSize(10);
    statusLabel_->setFont(statusFont);

    layout->addWidget(statusLabel_);

    setStyleSheet(R"(
        QLabel#resultLabel {
            color: #2196F3;
            background-color: #f5f5f5;
            border: 2px solid #ddd;
            border-radius: 8px;
            padding: 10px;
        }

        QLineEdit#expressionEdit {
            border: 2px solid #ddd;
            border-radius: 8px;
            padding: 10px;
            background-color: white;
        }

        QLineEdit#expressionEdit:focus {
            border-color: #2196F3;
        }

        QLabel#statusLabel {
            color: #757575;
        }
    )");
}

void DisplayWidget::setupConnections()
{
    connect(expressionEdit_, &QLineEdit::textChanged,
            this, &DisplayWidget::onTextChanged);
    connect(expressionEdit_, &QLineEdit::returnPressed,
            this, &DisplayWidget::onReturnPressed);
}

void DisplayWidget::setExpression(const QString& text)
{
    expressionEdit_->setText(text);
    expressionEdit_->setCursorPosition(text.length());
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
    expressionEdit_->clear();
    resultLabel_->clear();
    statusLabel_->clear();
    statusLabel_->setStyleSheet("color: #757575;");
    setErrorState(false);
}

QString DisplayWidget::getExpression() const
{
    return expressionEdit_->text();
}

void DisplayWidget::appendExpression(const QString& text)
{
    expressionEdit_->insert(text);
}

void DisplayWidget::backspace()
{
    expressionEdit_->backspace();
}

void DisplayWidget::setErrorState(bool error)
{
    errorState_ = error;
    if (error) {
        expressionEdit_->setStyleSheet(R"(
            QLineEdit {
                border: 2px solid #F44336;
                background-color: #FFEBEE;
            }
        )");
    } else {
        expressionEdit_->setStyleSheet(R"(
            QLineEdit {
                border: 2px solid #ddd;
                background-color: white;
            }

            QLineEdit:focus {
                border-color: #2196F3;
            }
        )");
    }
}

void DisplayWidget::setBusyState(bool busy)
{
    expressionEdit_->setEnabled(!busy);
}

void DisplayWidget::setPrecision(int precision)
{
    precision_ = precision;
}

void DisplayWidget::onTextChanged(const QString& text)
{
    emit expressionChanged(text);
}

void DisplayWidget::onReturnPressed()
{
    emit calculateRequested(expressionEdit_->text());
}

} // namespace calc::ui::qt
