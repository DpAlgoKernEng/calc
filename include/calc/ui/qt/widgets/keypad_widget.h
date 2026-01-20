/**
 * @file keypad_widget.h
 * @brief 键盘组件，支持多种模式布局
 */

#ifndef CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H
#define CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>
#include <QEvent>

namespace calc::ui::qt {

/**
 * @brief 键盘组件，支持多种模式布局
 */
class KeypadWidget : public QWidget {
    Q_OBJECT

public:
    explicit KeypadWidget(QWidget* parent = nullptr);
    ~KeypadWidget() override;

    // 设置键盘布局模式
    enum class LayoutMode {
        Standard,      // 标准模式：数字 + 基本运算
        Scientific,     // 科学模式：增加科学函数
        Programmer      // 程序员模式：进制转换 + 位运算
    };

    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const;

    // 按键操作
    void setButtonEnabled(const QString& key, bool enabled);
    void setButtonVisible(const QString& key, bool visible);

signals:
    void keyPressed(const QString& key);
    void calculateRequested();
    void clearRequested();
    void backspaceRequested();

private slots:
    void onButtonClicked();

private:
    void setupUI();
    void createStandardLayout();
    void createScientificLayout();
    void createProgrammerLayout();
    QPushButton* createButton(const QString& text, const QString& key);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

    QGridLayout* layout_;
    QMap<QString, QPushButton*> buttons_;
    LayoutMode currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_KEYPAD_WIDGET_H
