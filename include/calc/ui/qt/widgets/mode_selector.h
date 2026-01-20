/**
 * @file mode_selector.h
 * @brief 模式选择组件
 */

#ifndef CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
#define CALC_UI_QT_WIDGETS_MODE_SELECTOR_H

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QFrame>
#include <QFont>

namespace calc::ui::qt {

/**
 * @brief 模式选择组件
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
    void onModeButtonClicked();

private:
    void setupUI();
    void setupConnections();
    void createModeButton(const QString& displayName, const QString& name, const QString& tooltip);

    QButtonGroup* modeGroup_;
    QMap<QString, QRadioButton*> modeButtons_;
    QString currentMode_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_WIDGETS_MODE_SELECTOR_H
