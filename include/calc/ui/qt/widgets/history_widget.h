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
#include <QLineEdit>
#include <QComboBox>
#include <QMenu>

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
    void entryDeleted(int index);
    void exportRequested();

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onClearClicked();
    void onCopyClicked();
    void onSearchTextChanged(const QString& text);
    void onModeFilterChanged(const QString& mode);
    void onItemContextMenu(const QPoint& pos);
    void onExportClicked();
    void onDeleteItem();

private:
    void setupUI();
    void setupConnections();
    void setupContextMenu();
    void applyFilters();
    void updateEntryCount();
    bool matchesFilter(const QVariantMap& entry) const;

    QListWidget* historyList_;
    QLineEdit* searchEdit_;
    QComboBox* modeFilterCombo_;
    QPushButton* clearButton_;
    QPushButton* copyButton_;
    QPushButton* exportButton_;
    QLabel* countLabel_;
    QMenu* contextMenu_;

    QVariantList entries_;
    QString currentSearch_;
    QString currentModeFilter_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_HISTORY_WIDGET_H
