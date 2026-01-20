/**
 * @file display_widget.h
 * @brief 显示组件，用于显示表达式和结果
 */

#ifndef CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
#define CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H

#include <QWidget>
#include <QLabel>

namespace calc::ui::qt {

/**
 * @brief 显示组件，用于显示表达式和结果
 */
class DisplayWidget : public QWidget {
    Q_OBJECT

public:
    explicit DisplayWidget(QWidget* parent = nullptr);
    ~DisplayWidget() override;

    // 设置显示内容
    void setExpression(const QString& text);
    void setResult(const QString& text);
    void setError(const QString& error);
    void clear();

    // 获取输入
    QString getExpression() const;
    void appendExpression(const QString& text);
    void backspace();

    // 显示状态
    void setErrorState(bool error);
    void setBusyState(bool busy);
    bool hasFocus() const { return false; }  // 标签不能获取焦点

    // 格式化
    void setPrecision(int precision);

signals:
    void calculateRequested(const QString& expression);
    void clearRequested();

private:
    void setupUI();

    QLabel* expressionLabel_;  // 表达式预览标签 (只读)
    QLabel* resultLabel_;
    QLabel* statusLabel_;
    bool errorState_;
    int precision_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
