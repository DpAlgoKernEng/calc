/**
 * @file history_modal.cpp
 * @brief 全屏历史模态框实现
 */

#include "calc/ui/qt/widgets/history_modal.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <QApplication>

namespace calc::ui::qt {

HistoryModal::HistoryModal(QWidget* parent)
    : QWidget(parent)
    , overlay_(nullptr)
    , modal_(nullptr)
    , header_(nullptr)
    , titleLabel_(nullptr)
    , closeButton_(nullptr)
    , clearButton_(nullptr)
    , historyList_(nullptr)
    , emptyState_(nullptr)
    , opacityEffect_(nullptr)
    , fadeAnimation_(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setupUI();
    setupAnimations();
}

HistoryModal::~HistoryModal() = default;

void HistoryModal::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 半透明覆盖层
    overlay_ = new QWidget(this);
    overlay_->setObjectName("historyOverlay");

    auto* overlayLayout = new QVBoxLayout(overlay_);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->setSpacing(0);

    // 模态框卡片
    modal_ = new QWidget(overlay_);
    modal_->setObjectName("historyModal");

    auto* modalLayout = new QVBoxLayout(modal_);
    modalLayout->setContentsMargins(0, 0, 0, 0);
    modalLayout->setSpacing(0);

    // 头部
    header_ = new QWidget(modal_);
    header_->setObjectName("historyHeader");

    auto* headerLayout = new QHBoxLayout(header_);
    headerLayout->setContentsMargins(24, 24, 24, 24);
    headerLayout->setSpacing(16);

    // 标题
    titleLabel_ = new QLabel("历史记录", header_);
    titleLabel_->setObjectName("historyTitle");
    QFont titleFont = titleLabel_->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);
    headerLayout->addWidget(titleLabel_);
    headerLayout->addStretch();

    // 清空按钮
    clearButton_ = new QPushButton(header_);
    clearButton_->setObjectName("clearHistoryButton");
    clearButton_->setText("🗑️ 清空");
    clearButton_->setCursor(Qt::PointingHandCursor);
    headerLayout->addWidget(clearButton_);

    // 关闭按钮
    closeButton_ = new QPushButton(header_);
    closeButton_->setObjectName("closeHistoryButton");
    closeButton_->setText("✕");
    closeButton_->setCursor(Qt::PointingHandCursor);
    closeButton_->setFixedSize(40, 40);
    headerLayout->addWidget(closeButton_);

    modalLayout->addWidget(header_);

    // 内容区带滚动
    auto* scrollArea = new QScrollArea(modal_);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* contentWidget = new QWidget(scrollArea);
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 0, 24, 24);
    contentLayout->setSpacing(12);
    contentLayout->addStretch();

    // 空状态
    emptyState_ = new QLabel(contentWidget);
    emptyState_->setObjectName("historyEmptyState");
    emptyState_->setAlignment(Qt::AlignCenter);
    emptyState_->setText("🕐\n\n暂无历史记录");
    QFont emptyFont = emptyState_->font();
    emptyFont.setPointSize(14);
    emptyFont.setBold(true);
    emptyState_->setFont(emptyFont);
    contentLayout->addWidget(emptyState_);

    // 历史列表
    historyList_ = new QListWidget(contentWidget);
    historyList_->setObjectName("historyList");
    historyList_->setFrameShape(QFrame::NoFrame);
    historyList_->setSelectionMode(QAbstractItemView::SingleSelection);
    historyList_->setSpacing(12);
    historyList_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    historyList_->hide();  // 初始隐藏

    contentLayout->insertWidget(0, historyList_);

    scrollArea->setWidget(contentWidget);
    modalLayout->addWidget(scrollArea);

    overlayLayout->addWidget(modal_);
    mainLayout->addWidget(overlay_);

    // 连接信号
    connect(closeButton_, &QPushButton::clicked, this, &HistoryModal::onCloseButtonClicked);
    connect(clearButton_, &QPushButton::clicked, this, &HistoryModal::onClearButtonClicked);
    connect(historyList_, &QListWidget::itemClicked, this, &HistoryModal::onItemClicked);
}

void HistoryModal::setupAnimations()
{
    opacityEffect_ = new QGraphicsOpacityEffect(this);
    overlay_->setGraphicsEffect(opacityEffect_);

    fadeAnimation_ = new QPropertyAnimation(opacityEffect_, "opacity");
    fadeAnimation_->setDuration(200);
    fadeAnimation_->setEasingCurve(QEasingCurve::OutCubic);
    connect(fadeAnimation_, &QPropertyAnimation::finished, this, &HistoryModal::onAnimationFinished);
}

void HistoryModal::setHistory(const QVariantList& history)
{
    history_ = history;

    historyList_->clear();

    if (history.isEmpty()) {
        historyList_->hide();
        emptyState_->show();
    } else {
        emptyState_->hide();
        historyList_->show();

        for (const auto& entry : history) {
            createHistoryItem(entry.toMap());
        }
    }
}

void HistoryModal::createHistoryItem(const QVariantMap& entry)
{
    QString expression = entry["expression"].toString();
    QString result = entry["result"].toString();
    QString timestamp = entry["timestamp"].toString();
    QString mode = entry["mode"].toString();

    auto* item = new QListWidgetItem();
    item->setData(Qt::UserRole, entry);

    // 创建按钮作为项目
    auto* btn = new QPushButton();
    btn->setObjectName("historyItem");
    btn->setProperty("buttonType", "historyItem");
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setCursor(Qt::PointingHandCursor);

    auto* btnLayout = new QVBoxLayout(btn);
    btnLayout->setContentsMargins(20, 16, 20, 16);
    btnLayout->setSpacing(8);

    // 表达式 (小字体)
    auto* exprLabel = new QLabel(expression, btn);
    QFont exprFont = exprLabel->font();
    exprFont.setPointSize(12);
    exprFont.setFamily("SF Mono, Monaco, Consolas, monospace");
    exprLabel->setFont(exprFont);
    exprLabel->setStyleSheet("color: #6B7280; background: transparent; border: none;");
    btnLayout->addWidget(exprLabel);

    // 结果 (大字体)
    auto* resultLabel = new QLabel(QString("= %1").arg(result), btn);
    QFont resultFont = resultLabel->font();
    resultFont.setPointSize(24);
    resultFont.setBold(true);
    resultLabel->setFont(resultFont);
    resultLabel->setAlignment(Qt::AlignRight);
    resultLabel->setStyleSheet("color: #1F2937; background: transparent; border: none;");
    btnLayout->addWidget(resultLabel);

    // 时间戳
    auto* timeLabel = new QLabel(timestamp, btn);
    QFont timeFont = timeLabel->font();
    timeFont.setPointSize(10);
    timeLabel->setFont(timeFont);
    timeLabel->setStyleSheet("color: #9CA3AF; background: transparent; border: none;");
    timeLabel->setAlignment(Qt::AlignLeft);
    btnLayout->addWidget(timeLabel);

    btn->setMinimumHeight(120);

    item->setSizeHint(btn->sizeHint());
    historyList_->addItem(item);
    historyList_->setItemWidget(item, btn);

    // 连接按钮点击信号
    connect(btn, &QPushButton::clicked, [this, entry]() {
        emit itemSelected(entry["expression"].toString());
        onCloseButtonClicked();
    });
}

void HistoryModal::updateEmptyState()
{
    emptyState_->setVisible(historyList_->count() == 0);
}

void HistoryModal::clear()
{
    history_.clear();
    historyList_->clear();
    updateEmptyState();
}

void HistoryModal::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QWidget::paintEvent(event);
}

void HistoryModal::mousePressEvent(QMouseEvent* event)
{
    // 点击模态框外部时关闭
    if (!modal_->rect().contains(modal_->mapFromParent(event->pos()))) {
        onCloseButtonClicked();
    }
}

void HistoryModal::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        onCloseButtonClicked();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void HistoryModal::onItemClicked(QListWidgetItem* item)
{
    if (item) {
        QVariantMap entry = item->data(Qt::UserRole).toMap();
        emit itemSelected(entry["expression"].toString());
        onCloseButtonClicked();
    }
}

void HistoryModal::onCloseButtonClicked()
{
    fadeAnimation_->setStartValue(1.0);
    fadeAnimation_->setEndValue(0.0);
    fadeAnimation_->start();
}

void HistoryModal::onClearButtonClicked()
{
    emit clearRequested();
}

void HistoryModal::onAnimationFinished()
{
    if (fadeAnimation_->endValue().toReal() == 0.0) {
        hide();
        emit closed();
    }
}

} // namespace calc::ui::qt
