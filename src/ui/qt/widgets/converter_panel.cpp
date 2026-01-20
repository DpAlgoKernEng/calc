/**
 * @file converter_panel.cpp
 * @brief 进制转换面板组件实现
 */

#include "calc/ui/qt/widgets/converter_panel.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QFont>
#include <QIntValidator>
#include <QRegularExpressionValidator>

namespace calc::ui::qt {

ConverterPanel::ConverterPanel(QWidget* parent)
    : QWidget(parent)
    , decimalEdit_(nullptr)
    , hexEdit_(nullptr)
    , binaryEdit_(nullptr)
    , octalEdit_(nullptr)
    , currentValue_(0)
    , updating_(false)
{
    setupUI();
    setupConnections();
}

ConverterPanel::~ConverterPanel() = default;

void ConverterPanel::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(8);

    QFont labelFont;
    labelFont.setBold(true);
    labelFont.setPointSize(10);

    QFont editFont;
    editFont.setPointSize(12);
    editFont.setFamily("Monospace");

    // DEC Label
    auto* decLabel = new QLabel("DEC:", this);
    decLabel->setFont(labelFont);
    gridLayout->addWidget(decLabel, 0, 0);

    decimalEdit_ = new QLineEdit("0", this);
    decimalEdit_->setFont(editFont);
    decimalEdit_->setAlignment(Qt::AlignRight);
    decimalEdit_->setValidator(new QIntValidator(0, LLONG_MAX, this));
    gridLayout->addWidget(decimalEdit_, 0, 1);

    // HEX Label
    auto* hexLabel = new QLabel("HEX:", this);
    hexLabel->setFont(labelFont);
    gridLayout->addWidget(hexLabel, 1, 0);

    hexEdit_ = new QLineEdit("0", this);
    hexEdit_->setFont(editFont);
    hexEdit_->setAlignment(Qt::AlignRight);
    hexEdit_->setMaxLength(16);
    QRegularExpression hexRegex(QRegularExpression("^[0-9A-Fa-f]+$"));
    hexEdit_->setValidator(new QRegularExpressionValidator(hexRegex, this));
    gridLayout->addWidget(hexEdit_, 1, 1);

    // BIN Label
    auto* binLabel = new QLabel("BIN:", this);
    binLabel->setFont(labelFont);
    gridLayout->addWidget(binLabel, 2, 0);

    binaryEdit_ = new QLineEdit("0", this);
    binaryEdit_->setFont(editFont);
    binaryEdit_->setAlignment(Qt::AlignRight);
    binaryEdit_->setMaxLength(64);
    QRegularExpression binRegex(QRegularExpression("^[01]+$"));
    binaryEdit_->setValidator(new QRegularExpressionValidator(binRegex, this));
    gridLayout->addWidget(binaryEdit_, 2, 1);

    // OCT Label
    auto* octLabel = new QLabel("OCT:", this);
    octLabel->setFont(labelFont);
    gridLayout->addWidget(octLabel, 3, 0);

    octalEdit_ = new QLineEdit("0", this);
    octalEdit_->setFont(editFont);
    octalEdit_->setAlignment(Qt::AlignRight);
    octalEdit_->setMaxLength(22);
    QRegularExpression octRegex(QRegularExpression("^[0-7]+$"));
    octalEdit_->setValidator(new QRegularExpressionValidator(octRegex, this));
    gridLayout->addWidget(octalEdit_, 3, 1);

    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 1);

    layout->addLayout(gridLayout);
    layout->addStretch();

    setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }

        QLineEdit:focus {
            border-color: #2196F3;
        }
    )");
}

void ConverterPanel::setupConnections()
{
    connect(decimalEdit_, &QLineEdit::textChanged,
            this, &ConverterPanel::onDecimalChanged);
    connect(hexEdit_, &QLineEdit::textChanged,
            this, &ConverterPanel::onHexChanged);
    connect(binaryEdit_, &QLineEdit::textChanged,
            this, &ConverterPanel::onBinaryChanged);
    connect(octalEdit_, &QLineEdit::textChanged,
            this, &ConverterPanel::onOctalChanged);
}

void ConverterPanel::setValue(qint64 value)
{
    currentValue_ = value;
    updateAllFields(value);
}

qint64 ConverterPanel::getValue() const
{
    return currentValue_;
}

void ConverterPanel::setReadOnly(bool readOnly)
{
    decimalEdit_->setReadOnly(readOnly);
    hexEdit_->setReadOnly(readOnly);
    binaryEdit_->setReadOnly(readOnly);
    octalEdit_->setReadOnly(readOnly);
}

void ConverterPanel::updateAllFields(qint64 value)
{
    if (updating_) return;

    updating_ = true;

    decimalEdit_->setText(QString::number(value));
    hexEdit_->setText(QString::number(value, 16).toUpper());
    binaryEdit_->setText(QString::number(value, 2));
    octalEdit_->setText(QString::number(value, 8));

    updating_ = false;
}

void ConverterPanel::onDecimalChanged(const QString& text)
{
    if (updating_ || text.isEmpty()) return;

    bool ok;
    qint64 value = text.toLongLong(&ok);
    if (ok && value != currentValue_) {
        currentValue_ = value;
        emit valueChanged(value);
        updating_ = true;
        hexEdit_->setText(QString::number(value, 16).toUpper());
        binaryEdit_->setText(QString::number(value, 2));
        octalEdit_->setText(QString::number(value, 8));
        updating_ = false;
    }
}

void ConverterPanel::onHexChanged(const QString& text)
{
    if (updating_ || text.isEmpty()) return;

    bool ok;
    qint64 value = text.toLongLong(&ok, 16);
    if (ok && value != currentValue_) {
        currentValue_ = value;
        emit valueChanged(value);
        updating_ = true;
        decimalEdit_->setText(QString::number(value));
        binaryEdit_->setText(QString::number(value, 2));
        octalEdit_->setText(QString::number(value, 8));
        updating_ = false;
    }
}

void ConverterPanel::onBinaryChanged(const QString& text)
{
    if (updating_ || text.isEmpty()) return;

    bool ok;
    qint64 value = text.toLongLong(&ok, 2);
    if (ok && value != currentValue_) {
        currentValue_ = value;
        emit valueChanged(value);
        updating_ = true;
        decimalEdit_->setText(QString::number(value));
        hexEdit_->setText(QString::number(value, 16).toUpper());
        octalEdit_->setText(QString::number(value, 8));
        updating_ = false;
    }
}

void ConverterPanel::onOctalChanged(const QString& text)
{
    if (updating_ || text.isEmpty()) return;

    bool ok;
    qint64 value = text.toLongLong(&ok, 8);
    if (ok && value != currentValue_) {
        currentValue_ = value;
        emit valueChanged(value);
        updating_ = true;
        decimalEdit_->setText(QString::number(value));
        hexEdit_->setText(QString::number(value, 16).toUpper());
        binaryEdit_->setText(QString::number(value, 2));
        updating_ = false;
    }
}

} // namespace calc::ui::qt
