/**
 * @file history_modal.h
 * @brief 全屏历史模态框
 */

#ifndef CALC_UI_QT_WIDGETS_HISTORY_MODAL_H
#define CALC_UI_QT_WIDGETS_HISTORY_MODAL_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVariantList>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

namespace calc::ui::qt {

/**
 * @brief 全屏历史模态框
 * 匹配 React 示例中的覆盖层设计
 */
class HistoryModal : public QWidget {
    Q_OBJECT

public:
    explicit HistoryModal(QWidget* parent = nullptr);
    ~HistoryModal() override;

    void setHistory(const QVariantList& history);
    QVariantList getHistory() const;
    void clear();

signals:
    void closed();
    void itemSelected(const QString& expression);
    void clearRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onCloseButtonClicked();
    void onClearButtonClicked();
    void onAnimationFinished();

private:
    void setupUI();
    void setupAnimations();
    void createHistoryItem(const QVariantMap& entry);
    void updateEmptyState();

    QWidget* overlay_;
    QWidget* modal_;
    QWidget* header_;
    QLabel* titleLabel_;
    QPushButton* closeButton_;
    QPushButton* clearButton_;
    QListWidget* historyList_;
    QLabel* emptyState_;

    QVariantList history_;
    QGraphicsOpacityEffect* opacityEffect_;
    QPropertyAnimation* fadeAnimation_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_HISTORY_MODAL_H
