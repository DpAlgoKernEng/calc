/**
 * @file converter_panel.h
 * @brief 进制转换面板组件（程序员模式）
 */

#ifndef CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H
#define CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>

namespace calc::ui::qt {

/**
 * @brief 进制转换面板组件（程序员模式）
 */
class ConverterPanel : public QWidget {
    Q_OBJECT

public:
    explicit ConverterPanel(QWidget* parent = nullptr);
    ~ConverterPanel() override;

    void setValue(qint64 value);
    qint64 getValue() const;

    void setReadOnly(bool readOnly);

signals:
    void valueChanged(qint64 value);

private slots:
    void onDecimalChanged(const QString& text);
    void onHexChanged(const QString& text);
    void onBinaryChanged(const QString& text);
    void onOctalChanged(const QString& text);

private:
    void setupUI();
    void setupConnections();
    void updateAllFields(qint64 value);

    QLineEdit* decimalEdit_;
    QLineEdit* hexEdit_;
    QLineEdit* binaryEdit_;
    QLineEdit* octalEdit_;
    qint64 currentValue_;
    bool updating_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_CONVERTER_PANEL_H
