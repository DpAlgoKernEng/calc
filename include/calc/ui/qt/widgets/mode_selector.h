/**
 * @file mode_selector.h
 * @brief 模式选择组件 - 胶囊形状标签
 */

#ifndef CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
#define CALC_UI_QT_WIDGETS_MODE_SELECTOR_H

#include <QWidget>
#include <QPushButton>
#include <QMap>

namespace calc::ui::qt {

/**
 * @brief 模式选择组件 - 胶囊形状标签
 * 匹配 React 示例中的 pill tabs 设计
 */
class ModeSelector : public QWidget {
    Q_OBJECT

public:
    explicit ModeSelector(QWidget* parent = nullptr);
    ~ModeSelector() override;

    void setAvailableModes(const QStringList& modes);
    void setCurrentMode(const QString& mode);
    QString getCurrentMode() const;

signals:
    void modeChanged(const QString& mode);

private slots:
    void onModeTabClicked();

private:
    void setupUI();
    void setupConnections();
    void createModeTab(const QString& id, const QString& label, const QString& icon);

    QWidget* buttonContainer_;
    QMap<QString, QPushButton*> modeButtons_;
    QString currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
