/**
 * @file history_widget.h
 * @brief 历史记录组件
 */

#ifndef CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H
#define CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVariantList>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

namespace calc::ui::qt {

/**
 * @brief 历史记录组件
 */
class HistoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget* parent = nullptr);
    ~HistoryWidget() override;

    void addEntry(const QVariantMap& entry);
    void clear();
    void setHistory(const QVariantList& history);

    QVariantList getHistory() const;

signals:
    void entrySelected(const QString& expression);
    void entryCopied(const QString& text);
    void clearRequested();

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onClearClicked();
    void onCopyClicked();

private:
    void setupUI();
    void setupConnections();

    QListWidget* historyList_;
    QPushButton* clearButton_;
    QPushButton* copyButton_;
    QVariantList entries_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H
