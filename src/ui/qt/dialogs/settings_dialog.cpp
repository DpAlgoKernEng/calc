/**
 * @file settings_dialog.cpp
 * @brief 设置对话框实现
 */

#include "calc/ui/qt/dialogs/settings_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>

namespace calc::ui::qt {

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , precisionSpinBox_(nullptr)
    , angleUnitCombo_(nullptr)
    , showHistoryCheck_(nullptr)
    , historyMaxSizeSpinBox_(nullptr)
{
    setupUI();
    setupConnections();

    // 设置默认值
    setPrecision(DEFAULT_PRECISION);
    setAngleUnit(DEFAULT_ANGLE_UNIT);
    setShowHistory(DEFAULT_SHOW_HISTORY);
    setHistoryMaxSize(DEFAULT_HISTORY_MAX_SIZE);

    setWindowTitle("Settings");
    setMinimumWidth(400);
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    // 计算设置组
    auto* calcGroup = new QGroupBox("Calculation", this);
    auto* calcLayout = new QFormLayout(calcGroup);

    precisionSpinBox_ = new QSpinBox(this);
    precisionSpinBox_->setRange(0, 15);
    precisionSpinBox_->setValue(DEFAULT_PRECISION);
    calcLayout->addRow("Precision (decimal places):", precisionSpinBox_);

    angleUnitCombo_ = new QComboBox(this);
    angleUnitCombo_->addItem("Degrees", 0);
    angleUnitCombo_->addItem("Radians", 1);
    angleUnitCombo_->setCurrentIndex(DEFAULT_ANGLE_UNIT);
    calcLayout->addRow("Angle unit:", angleUnitCombo_);

    layout->addWidget(calcGroup);

    // 历史记录设置组
    auto* historyGroup = new QGroupBox("History", this);
    auto* historyLayout = new QFormLayout(historyGroup);

    showHistoryCheck_ = new QCheckBox("Show history panel", this);
    showHistoryCheck_->setChecked(DEFAULT_SHOW_HISTORY);
    historyLayout->addRow(showHistoryCheck_);

    historyMaxSizeSpinBox_ = new QSpinBox(this);
    historyMaxSizeSpinBox_->setRange(10, 1000);
    historyMaxSizeSpinBox_->setValue(DEFAULT_HISTORY_MAX_SIZE);
    historyMaxSizeSpinBox_->setSuffix(" entries");
    historyLayout->addRow("Maximum history size:", historyMaxSizeSpinBox_);

    layout->addWidget(historyGroup);

    layout->addStretch();

    // 恢复默认按钮
    auto* restoreButton = new QPushButton("Restore Defaults", this);
    connect(restoreButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaults);
    layout->addWidget(restoreButton);

    // 对话框按钮
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
    );
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::onRejected);
    layout->addWidget(buttons);
}

void SettingsDialog::setupConnections()
{
    // 设置变化时通知
    connect(precisionSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged),
            [this]() { emit settingsChanged(); });
    connect(angleUnitCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this]() { emit settingsChanged(); });
    connect(showHistoryCheck_, &QCheckBox::stateChanged,
            [this]() { emit settingsChanged(); });
    connect(historyMaxSizeSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged),
            [this]() { emit settingsChanged(); });
}

void SettingsDialog::setPrecision(int precision)
{
    precisionSpinBox_->setValue(precision);
}

int SettingsDialog::getPrecision() const
{
    return precisionSpinBox_->value();
}

void SettingsDialog::setAngleUnit(int unit)
{
    angleUnitCombo_->setCurrentIndex(unit);
}

int SettingsDialog::getAngleUnit() const
{
    return angleUnitCombo_->currentData().toInt();
}

void SettingsDialog::setShowHistory(bool show)
{
    showHistoryCheck_->setChecked(show);
}

bool SettingsDialog::getShowHistory() const
{
    return showHistoryCheck_->isChecked();
}

void SettingsDialog::setHistoryMaxSize(int size)
{
    historyMaxSizeSpinBox_->setValue(size);
}

int SettingsDialog::getHistoryMaxSize() const
{
    return historyMaxSizeSpinBox_->value();
}

void SettingsDialog::onRestoreDefaults()
{
    setPrecision(DEFAULT_PRECISION);
    setAngleUnit(DEFAULT_ANGLE_UNIT);
    setShowHistory(DEFAULT_SHOW_HISTORY);
    setHistoryMaxSize(DEFAULT_HISTORY_MAX_SIZE);
    emit settingsChanged();
}

void SettingsDialog::onAccepted()
{
    accept();
}

void SettingsDialog::onRejected()
{
    reject();
}

} // namespace calc::ui::qt
