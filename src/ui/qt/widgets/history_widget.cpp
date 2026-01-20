/**
 * @file history_widget.cpp
 * @brief 历史记录组件实现
 */

#include "calc/ui/qt/widgets/history_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QHeaderView>
#include <QFont>
#include <QApplication>
#include <QClipboard>

namespace calc::ui::qt {

HistoryWidget::HistoryWidget(QWidget* parent)
    : QWidget(parent)
    , historyList_(nullptr)
    , clearButton_(nullptr)
    , copyButton_(nullptr)
{
    setupUI();
    setupConnections();
}

HistoryWidget::~HistoryWidget() = default;

void HistoryWidget::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // 标题栏
    auto* headerLayout = new QHBoxLayout();

    auto* titleLabel = new QLabel("History", this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(11);
    titleLabel->setFont(titleFont);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // 复制按钮
    copyButton_ = new QPushButton("Copy", this);
    copyButton_->setMaximumWidth(60);
    copyButton_->setEnabled(false);

    // 清除按钮
    clearButton_ = new QPushButton("Clear", this);
    clearButton_->setMaximumWidth(60);

    headerLayout->addWidget(copyButton_);
    headerLayout->addWidget(clearButton_);

    layout->addLayout(headerLayout);

    // 历史记录列表
    historyList_ = new QListWidget(this);
    historyList_->setObjectName("historyList");
    historyList_->setAlternatingRowColors(true);
    historyList_->setSelectionMode(QAbstractItemView::SingleSelection);

    QFont listFont = historyList_->font();
    listFont.setPointSize(10);
    historyList_->setFont(listFont);

    layout->addWidget(historyList_);

    setStyleSheet(R"(
        QListWidget#historyList {
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
            color: #1976D2;
        }

        QListWidget#historyList::item:hover {
            background-color: #F5F5F5;
        }

        QPushButton {
            font-size: 10px;
            padding: 4px 8px;
        }
    )");
}

void HistoryWidget::setupConnections()
{
    connect(historyList_, &QListWidget::itemDoubleClicked,
            this, &HistoryWidget::onItemDoubleClicked);
    connect(clearButton_, &QPushButton::clicked,
            this, &HistoryWidget::onClearClicked);
    connect(copyButton_, &QPushButton::clicked,
            this, &HistoryWidget::onCopyClicked);
}

void HistoryWidget::addEntry(const QVariantMap& entry)
{
    QString expression = entry["expression"].toString();
    QString result = entry["result"].toString();
    QString timestamp = entry["timestamp"].toString();

    // 创建列表项
    QString displayText = QString("%1 = %2").arg(expression).arg(result);

    auto* item = new QListWidgetItem(displayText);
    item->setData(Qt::UserRole, entry);
    item->setToolTip(QString("Expression: %1\nResult: %2\nTime: %3")
                        .arg(expression, result, timestamp));

    historyList_->insertItem(0, item);

    entries_.prepend(entry);

    copyButton_->setEnabled(true);
}

void HistoryWidget::clear()
{
    historyList_->clear();
    entries_.clear();
    copyButton_->setEnabled(false);
}

void HistoryWidget::setHistory(const QVariantList& history)
{
    clear();

    // 按时间倒序添加
    int size = static_cast<int>(history.size());
    for (int i = size - 1; i >= 0; --i) {
        addEntry(history[i].toMap());
    }
}

QVariantList HistoryWidget::getHistory() const
{
    return entries_;
}

void HistoryWidget::onItemDoubleClicked(QListWidgetItem* item)
{
    if (item) {
        QVariantMap entry = item->data(Qt::UserRole).toMap();
        emit entrySelected(entry["expression"].toString());
        copyButton_->setEnabled(true);
    }
}

void HistoryWidget::onClearClicked()
{
    emit clearRequested();
}

void HistoryWidget::onCopyClicked()
{
    auto* item = historyList_->currentItem();
    if (item) {
        QVariantMap entry = item->data(Qt::UserRole).toMap();
        QString text = QString("%1 = %2")
                          .arg(entry["expression"].toString())
                          .arg(entry["result"].toString());

        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(text);

        emit entryCopied(text);
    }
}

} // namespace calc::ui::qt
