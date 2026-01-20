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
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace calc::ui::qt {

HistoryWidget::HistoryWidget(QWidget* parent)
    : QWidget(parent)
    , historyList_(nullptr)
    , searchEdit_(nullptr)
    , modeFilterCombo_(nullptr)
    , clearButton_(nullptr)
    , copyButton_(nullptr)
    , exportButton_(nullptr)
    , countLabel_(nullptr)
    , contextMenu_(nullptr)
    , currentModeFilter_("All")
{
    setupUI();
    setupConnections();
    setupContextMenu();
}

HistoryWidget::~HistoryWidget() = default;

void HistoryWidget::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // 标题栏
    auto* headerLayout = new QHBoxLayout();

    auto* titleLabel = new QLabel("History", this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    titleLabel->setFont(titleFont);

    headerLayout->addWidget(titleLabel);

    // 数量标签
    countLabel_ = new QLabel("(0 entries)", this);
    QFont countFont = countLabel_->font();
    countFont.setPointSize(10);
    countLabel_->setFont(countFont);
    countLabel_->setStyleSheet("color: #757575;");
    headerLayout->addWidget(countLabel_);

    headerLayout->addStretch();

    // 导出按钮
    exportButton_ = new QPushButton("Export", this);
    exportButton_->setMaximumWidth(70);
    exportButton_->setEnabled(false);

    // 复制按钮
    copyButton_ = new QPushButton("Copy", this);
    copyButton_->setMaximumWidth(60);
    copyButton_->setEnabled(false);

    // 清除按钮
    clearButton_ = new QPushButton("Clear", this);
    clearButton_->setMaximumWidth(60);

    headerLayout->addWidget(exportButton_);
    headerLayout->addWidget(copyButton_);
    headerLayout->addWidget(clearButton_);

    layout->addLayout(headerLayout);

    // 搜索框
    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Search history...");
    searchEdit_->setObjectName("searchEdit");
    layout->addWidget(searchEdit_);

    // 模式过滤器
    auto* filterLayout = new QHBoxLayout();
    filterLayout->setContentsMargins(0, 0, 0, 0);

    auto* filterLabel = new QLabel("Filter:", this);
    filterLayout->addWidget(filterLabel);

    modeFilterCombo_ = new QComboBox(this);
    modeFilterCombo_->addItem("All", "All");
    modeFilterCombo_->addItem("Standard", "standard");
    modeFilterCombo_->addItem("Scientific", "scientific");
    modeFilterCombo_->addItem("Programmer", "programmer");
    filterLayout->addWidget(modeFilterCombo_);
    filterLayout->addStretch();

    layout->addLayout(filterLayout);

    // 历史记录列表
    historyList_ = new QListWidget(this);
    historyList_->setObjectName("historyList");
    historyList_->setAlternatingRowColors(true);
    historyList_->setSelectionMode(QAbstractItemView::SingleSelection);
    historyList_->setContextMenuPolicy(Qt::CustomContextMenu);

    QFont listFont = historyList_->font();
    listFont.setPointSize(10);
    historyList_->setFont(listFont);

    layout->addWidget(historyList_);

    setStyleSheet(R"(
        QLineEdit#searchEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit#searchEdit:focus {
            border-color: #2196F3;
        }
        QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 4px;
            background-color: white;
            min-width: 100px;
        }
        QLabel {
            color: #424242;
        }
        QListWidget#historyList {
            border: 1px solid #ddd;
            border-radius: 6px;
            background-color: #FAFAFA;
        }
        QListWidget#historyList::item {
            padding: 10px;
            border-bottom: 1px solid #eee;
            min-height: 40px;
        }
        QListWidget#historyList::item:selected {
            background-color: #E3F2FD;
            color: #1976D2;
            border: 1px solid #BBDEFB;
        }
        QListWidget#historyList::item:hover {
            background-color: #F5F5F5;
        }
        QPushButton {
            font-size: 10px;
            padding: 5px 10px;
            background-color: #ffffff;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #f5f5f5;
            border-color: #2196F3;
        }
        QPushButton:pressed {
            background-color: #eeeeee;
        }
        QPushButton:disabled {
            color: #BDBDBD;
            border-color: #E0E0E0;
        }
    )");
}

void HistoryWidget::setupConnections()
{
    connect(historyList_, &QListWidget::itemDoubleClicked,
            this, &HistoryWidget::onItemDoubleClicked);
    connect(historyList_, &QListWidget::customContextMenuRequested,
            this, &HistoryWidget::onItemContextMenu);
    connect(clearButton_, &QPushButton::clicked,
            this, &HistoryWidget::onClearClicked);
    connect(copyButton_, &QPushButton::clicked,
            this, &HistoryWidget::onCopyClicked);
    connect(exportButton_, &QPushButton::clicked,
            this, &HistoryWidget::onExportClicked);
    connect(searchEdit_, &QLineEdit::textChanged,
            this, &HistoryWidget::onSearchTextChanged);
    connect(modeFilterCombo_, &QComboBox::currentTextChanged,
            this, &HistoryWidget::onModeFilterChanged);
}

void HistoryWidget::setupContextMenu()
{
    contextMenu_ = new QMenu(this);

    auto* copyAction = contextMenu_->addAction("Copy");
    connect(copyAction, &QAction::triggered, this, &HistoryWidget::onCopyClicked);

    auto* deleteAction = contextMenu_->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, &HistoryWidget::onDeleteItem);

    contextMenu_->addSeparator();

    auto* selectAction = contextMenu_->addAction("Use Expression");
    connect(selectAction, &QAction::triggered, [this]() {
        auto* item = historyList_->currentItem();
        if (item) {
            QVariantMap entry = item->data(Qt::UserRole).toMap();
            emit entrySelected(entry["expression"].toString());
        }
    });
}

void HistoryWidget::addEntry(const QVariantMap& entry)
{
    QString expression = entry["expression"].toString();
    QString result = entry["result"].toString();
    QString timestamp = entry["timestamp"].toString();
    QString mode = entry["mode"].toString();

    // 创建列表项
    QString displayText = QString("%1 = %2").arg(expression).arg(result);

    auto* item = new QListWidgetItem(displayText);
    item->setData(Qt::UserRole, entry);
    item->setData(Qt::UserRole + 1, entries_.size()); // 原始索引
    item->setToolTip(QString("Expression: %1\nResult: %2\nMode: %3\nTime: %4")
                        .arg(expression, result, mode, timestamp));

    historyList_->insertItem(0, item);

    entries_.prepend(entry);

    applyFilters();
    updateEntryCount();

    copyButton_->setEnabled(true);
    exportButton_->setEnabled(true);
}

void HistoryWidget::clear()
{
    historyList_->clear();
    entries_.clear();
    copyButton_->setEnabled(false);
    exportButton_->setEnabled(false);
    updateEntryCount();
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

void HistoryWidget::onSearchTextChanged(const QString& text)
{
    currentSearch_ = text;
    applyFilters();
}

void HistoryWidget::onModeFilterChanged(const QString& /*mode*/)
{
    currentModeFilter_ = modeFilterCombo_->currentData().toString();
    applyFilters();
}

void HistoryWidget::onItemContextMenu(const QPoint& pos)
{
    auto* item = historyList_->itemAt(pos);
    if (item) {
        historyList_->setCurrentItem(item);
        contextMenu_->exec(historyList_->mapToGlobal(pos));
    }
}

void HistoryWidget::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export History",
        "",
        "JSON Files (*.json);;Text Files (*.txt)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    if (fileName.endsWith(".json")) {
        // JSON导出
        QJsonArray jsonArray;
        for (const auto& entry : entries_) {
            QVariantMap map = entry.toMap();
            QJsonObject obj;
            obj["expression"] = map["expression"].toString();
            obj["result"] = map["result"].toString();
            obj["timestamp"] = map["timestamp"].toString();
            obj["mode"] = map["mode"].toString();
            jsonArray.append(obj);
        }

        QJsonDocument doc(jsonArray);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
            QMessageBox::information(this, "Export", "History exported successfully!");
        } else {
            QMessageBox::warning(this, "Export Error", "Failed to write file.");
        }
    } else {
        // 文本导出
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const auto& entry : entries_) {
                QVariantMap map = entry.toMap();
                out << map["expression"].toString()
                    << " = "
                    << map["result"].toString()
                    << "\n";
            }
            file.close();
            QMessageBox::information(this, "Export", "History exported successfully!");
        } else {
            QMessageBox::warning(this, "Export Error", "Failed to write file.");
        }
    }
}

void HistoryWidget::onDeleteItem()
{
    auto* item = historyList_->currentItem();
    if (item) {
        int originalIndex = item->data(Qt::UserRole + 1).toInt();
        if (originalIndex >= 0 && originalIndex < entries_.size()) {
            entries_.removeAt(originalIndex);
            delete item;
            applyFilters();
            updateEntryCount();
            emit entryDeleted(originalIndex);
        }
    }
}

void HistoryWidget::applyFilters()
{
    historyList_->clear();

    for (int i = 0; i < entries_.size(); ++i) {
        QVariantMap entry = entries_[i].toMap();
        if (matchesFilter(entry)) {
            QString expression = entry["expression"].toString();
            QString result = entry["result"].toString();
            QString displayText = QString("%1 = %2").arg(expression).arg(result);

            auto* item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, entries_[i]);
            item->setData(Qt::UserRole + 1, i);
            item->setToolTip(QString("Expression: %1\nResult: %2\nMode: %3\nTime: %4")
                                .arg(expression, result, entry["mode"].toString(), entry["timestamp"].toString()));

            historyList_->addItem(item);
        }
    }
}

void HistoryWidget::updateEntryCount()
{
    int visibleCount = historyList_->count();
    int totalCount = static_cast<int>(entries_.size());
    countLabel_->setText(QString("(%1 visible / %2 total)").arg(visibleCount).arg(totalCount));
}

bool HistoryWidget::matchesFilter(const QVariantMap& entry) const
{
    // 模式过滤
    if (currentModeFilter_ != "All") {
        if (entry["mode"].toString() != currentModeFilter_) {
            return false;
        }
    }

    // 搜索过滤
    if (!currentSearch_.isEmpty()) {
        QString search = currentSearch_.toLower();
        QString expression = entry["expression"].toString().toLower();
        QString result = entry["result"].toString().toLower();

        if (!expression.contains(search) && !result.contains(search)) {
            return false;
        }
    }

    return true;
}

} // namespace calc::ui::qt
