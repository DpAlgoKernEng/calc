/**
 * @file mode_selector.cpp
 * @brief 模式选择组件实现 - 胶囊形状标签
 */

#include "calc/ui/qt/widgets/mode_selector.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

namespace calc::ui::qt {

ModeSelector::ModeSelector(QWidget* parent)
    : QWidget(parent)
    , buttonContainer_(nullptr)
    , currentMode_("standard")
{
    setupUI();
    setupConnections();
    setCurrentMode("standard");
}

ModeSelector::~ModeSelector() = default;

void ModeSelector::setupUI()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    buttonContainer_ = new QWidget(this);
    auto* containerLayout = new QHBoxLayout(buttonContainer_);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(8);

    // 创建胶囊标签 - 标准模式
    createModeTab("standard", "标准", "🧮");

    // 创建胶囊标签 - 科学模式
    createModeTab("scientific", "科学", "🧪");

    // 创建胶囊标签 - 程序员模式
    createModeTab("programmer", "程序员", "💻");

    layout->addWidget(buttonContainer_);
}

void ModeSelector::createModeTab(const QString& id, const QString& label, const QString& icon)
{
    auto* button = new QPushButton(this);
    QString buttonText = QString("%1 %2").arg(icon, label);
    button->setText(buttonText);
    button->setObjectName("modeTab");
    button->setProperty("modeId", id);
    button->setProperty("buttonType", "modeTab");

    // 胶囊形状样式
    button->setMinimumHeight(40);
    button->setMaximumHeight(40);

    QFont font = button->font();
    font.setPointSize(12);
    font.setWeight(QFont::Medium);
    button->setFont(font);

    modeButtons_[id] = button;

    auto* containerLayout = qobject_cast<QHBoxLayout*>(buttonContainer_->layout());
    containerLayout->addWidget(button);
}

void ModeSelector::setupConnections()
{
    for (auto* btn : modeButtons_.values()) {
        connect(btn, &QPushButton::clicked, this, &ModeSelector::onModeTabClicked);
    }
}

void ModeSelector::setAvailableModes(const QStringList& modes)
{
    for (auto* btn : modeButtons_.values()) {
        QString modeId = btn->property("modeId").toString();
        btn->setVisible(modes.contains(modeId));
    }
}

void ModeSelector::setCurrentMode(const QString& mode)
{
    currentMode_ = mode;

    // 更新按钮状态
    for (auto* btn : modeButtons_.values()) {
        QString modeId = btn->property("modeId").toString();
        btn->setProperty("active", modeId == mode);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}

QString ModeSelector::getCurrentMode() const
{
    return currentMode_;
}

void ModeSelector::onModeTabClicked()
{
    auto* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString mode = button->property("modeId").toString();
        if (mode != currentMode_) {
            setCurrentMode(mode);
            emit modeChanged(mode);
        }
    }
}

} // namespace calc::ui::qt
