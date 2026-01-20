/**
 * @file settings_dialog.h
 * @brief 设置对话框
 */

#ifndef CALC_UI_QT_DIALOGS_SETTINGS_DIALOG_H
#define CALC_UI_QT_DIALOGS_SETTINGS_DIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

namespace calc::ui::qt {

/**
 * @brief 设置对话框
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

    // Settings accessors
    void setPrecision(int precision);
    int getPrecision() const;

    void setAngleUnit(int unit); // 0=度, 1=弧度
    int getAngleUnit() const;

    void setShowHistory(bool show);
    bool getShowHistory() const;

    void setHistoryMaxSize(int size);
    int getHistoryMaxSize() const;

signals:
    void settingsChanged();

private slots:
    void onAccepted();
    void onRejected();
    void onRestoreDefaults();

private:
    void setupUI();
    void setupConnections();

    QSpinBox* precisionSpinBox_;
    QComboBox* angleUnitCombo_;
    QCheckBox* showHistoryCheck_;
    QSpinBox* historyMaxSizeSpinBox_;

    // Default values
    static constexpr int DEFAULT_PRECISION = 6;
    static constexpr int DEFAULT_ANGLE_UNIT = 0;
    static constexpr bool DEFAULT_SHOW_HISTORY = true;
    static constexpr int DEFAULT_HISTORY_MAX_SIZE = 100;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_DIALOGS_SETTINGS_DIALOG_H
