/**
 * @file display_widget.h
 * @brief 显示组件，用于显示表达式和结果
 */

#ifndef CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
#define CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

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

    // 格式化
    void setPrecision(int precision);

signals:
    void expressionChanged(const QString& expression);
    void calculateRequested(const QString& expression);
    void clearRequested();

private slots:
    void onTextChanged(const QString& text);
    void onReturnPressed();

private:
    void setupUI();
    void setupConnections();

    QLineEdit* expressionEdit_;
    QLabel* resultLabel_;
    QLabel* statusLabel_;
    bool errorState_;
    int precision_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_DISPLAY_WIDGET_H
