/**
 * @file history_button.h
 * @brief 悬浮历史按钮 + 徽章
 */

#ifndef CALC_UI_QT_WIDGETS_HISTORY_BUTTON_H
#define CALC_UI_QT_WIDGETS_HISTORY_BUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

namespace calc::ui::qt {

/**
 * @brief 悬浮圆形按钮，带历史记录计数徽章
 * 匹配 React 示例中的悬浮历史按钮设计
 */
class HistoryButton : public QWidget {
    Q_OBJECT

public:
    explicit HistoryButton(QWidget* parent = nullptr);
    ~HistoryButton() override;

    void setCount(int count);
    int getCount() const { return count_; }

signals:
    void clicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void setupUI();

    QPushButton* button_;
    QLabel* badge_;
    QHBoxLayout* layout_;
    int count_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_HISTORY_BUTTON_H
