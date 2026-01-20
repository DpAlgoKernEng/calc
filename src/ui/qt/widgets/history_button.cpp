/**
 * @file history_button.cpp
 * @brief 悬浮历史按钮 + 徽章实现
 */

#include "calc/ui/qt/widgets/history_button.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCursor>

namespace calc::ui::qt {

HistoryButton::HistoryButton(QWidget* parent)
    : QWidget(parent)
    , button_(nullptr)
    , badge_(nullptr)
    , layout_(nullptr)
    , count_(0)
{
    setupUI();
    setCount(0);
}

HistoryButton::~HistoryButton() = default;

void HistoryButton::setupUI()
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(64, 64);

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    button_ = new QPushButton(this);
    button_->setObjectName("historyButton");
    button_->setFixedSize(48, 48);
    button_->setCursor(Qt::PointingHandCursor);
    button_->setText("🕐");
    button_->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            font-size: 20px;
            padding: 0;
        }
    )");

    layout_->addWidget(button_);

    // Badge (绝对定位在右上角)
    badge_ = new QLabel(this);
    badge_->setObjectName("historyBadge");
    badge_->move(40, 4);  // Position at top-right
    badge_->hide();
}

void HistoryButton::setCount(int count)
{
    count_ = count;

    if (count > 0) {
        QString displayText = count > 9 ? "9+" : QString::number(count);
        badge_->setText(displayText);
        badge_->adjustSize();
        badge_->show();
    } else {
        badge_->hide();
    }
}

void HistoryButton::enterEvent(QEnterEvent* event)
{
    button_->setStyleSheet(R"(
        QPushButton {
            background: rgba(243, 244, 246, 0.8);
            border: none;
            border-radius: 9999px;
            font-size: 20px;
            padding: 0;
        }
    )");
    QWidget::enterEvent(event);
}

void HistoryButton::leaveEvent(QEvent* event)
{
    button_->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            font-size: 20px;
            padding: 0;
        }
    )");
    QWidget::leaveEvent(event);
}

void HistoryButton::mousePressEvent(QMouseEvent* event)
{
    button_->setStyleSheet(R"(
        QPushButton {
            background: rgba(229, 231, 235, 0.9);
            border: none;
            border-radius: 9999px;
            font-size: 20px;
            padding: 0;
        }
    )");
    QWidget::mousePressEvent(event);
}

void HistoryButton::mouseReleaseEvent(QMouseEvent* event)
{
    button_->setStyleSheet(R"(
        QPushButton {
            background: rgba(243, 244, 246, 0.8);
            border: none;
            border-radius: 9999px;
            font-size: 20px;
            padding: 0;
        }
    )");
    emit clicked();
    QWidget::mouseReleaseEvent(event);
}

} // namespace calc::ui::qt
