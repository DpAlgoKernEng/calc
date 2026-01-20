/**
 * @file mode_selector.cpp
 * @brief 模式选择组件实现
 */

#include "calc/ui/qt/widgets/mode_selector.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFrame>
#include <QFont>

namespace calc::ui::qt {

ModeSelector::ModeSelector(QWidget* parent)
    : QWidget(parent)
    , modeGroup_(new QButtonGroup(this))
    , currentMode_("standard")
{
    setupUI();
    setupConnections();

    // 设置默认模式
    setCurrentMode("standard");
}

ModeSelector::~ModeSelector() = default;

void ModeSelector::setupUI()
{
    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(16);
    layout->setContentsMargins(8, 8, 8, 8);

    // 创建单选按钮
    createModeButton("Standard", "standard", "Basic arithmetic");
    createModeButton("Scientific", "scientific", "Advanced functions");
    createModeButton("Programmer", "programmer", "Base conversion & bitwise");

    // 添加分隔符
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addStretch();

    // 帮助按钮
    auto* helpButton = new QPushButton("?", this);
    helpButton->setMaximumSize(24, 24);
    helpButton->setToolTip("Help");
    layout->addWidget(helpButton);
}

void ModeSelector::createModeButton(const QString& displayName, const QString& name, const QString& tooltip)
{
    auto* radioBtn = new QRadioButton(displayName, this);
    radioBtn->setProperty("modeName", name);
    radioBtn->setToolTip(tooltip);

    QFont font = radioBtn->font();
    font.setPointSize(12);
    radioBtn->setFont(font);

    modeGroup_->addButton(radioBtn);
    modeButtons_[name] = radioBtn;

    auto* layout = qobject_cast<QHBoxLayout*>(this->layout());
    layout->insertWidget(layout->count() - 2, radioBtn);
}

void ModeSelector::setupConnections()
{
    connect(modeGroup_, &QButtonGroup::idClicked, this, [this](int id) {
        auto* btn = qobject_cast<QRadioButton*>(modeGroup_->button(id));
        if (btn) {
            QString mode = btn->property("modeName").toString();
            emit modeChanged(mode);
        }
    });
}

void ModeSelector::setAvailableModes(const QStringList& modes)
{
    // 隐藏所有按钮
    for (auto* btn : modeButtons_.values()) {
        btn->hide();
    }

    // 显示指定模式的按钮
    for (const QString& mode : modes) {
        if (modeButtons_.contains(mode)) {
            modeButtons_[mode]->show();
        }
    }
}

void ModeSelector::setCurrentMode(const QString& mode)
{
    currentMode_ = mode;

    if (modeButtons_.contains(mode)) {
        modeButtons_[mode]->setChecked(true);
    }
}

QString ModeSelector::getCurrentMode() const
{
    return currentMode_;
}

void ModeSelector::onModeButtonClicked()
{
    auto* btn = qobject_cast<QRadioButton*>(sender());
    if (btn) {
        QString mode = btn->property("modeName").toString();
        currentMode_ = mode;
        emit modeChanged(mode);
    }
}

} // namespace calc::ui::qt
