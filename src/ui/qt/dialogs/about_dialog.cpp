/**
 * @file about_dialog.cpp
 * @brief 关于对话框实现
 */

#include "calc/ui/qt/dialogs/about_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMessageBox>
#include <QPixmap>

namespace calc::ui::qt {

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
    , titleLabel_(nullptr)
    , versionLabel_(nullptr)
    , descriptionLabel_(nullptr)
    , copyrightLabel_(nullptr)
    , licenseButton_(nullptr)
    , qtButton_(nullptr)
    , okButton_(nullptr)
{
    setupUI();

    setWindowTitle("About Calc");
    setMinimumWidth(450);
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);

    // 标题
    titleLabel_ = new QLabel("Calc", this);
    QFont titleFont = titleLabel_->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);
    titleLabel_->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel_);

    // 版本
    versionLabel_ = new QLabel("Version 1.0.0", this);
    QFont versionFont = versionLabel_->font();
    versionFont.setPointSize(12);
    versionLabel_->setFont(versionFont);
    versionLabel_->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel_);

    // 描述
    descriptionLabel_ = new QLabel(
        "A cross-platform calculator with three modes:\n"
        "• Standard - Basic arithmetic operations\n"
        "• Scientific - Advanced mathematical functions\n"
        "• Programmer - Base conversion and bitwise operations",
        this
    );
    descriptionLabel_->setAlignment(Qt::AlignCenter);
    descriptionLabel_->setWordWrap(true);
    layout->addWidget(descriptionLabel_);

    // 版权信息
    copyrightLabel_ = new QLabel(
        "Copyright (C) 2026\n"
        "Licensed under MIT License",
        this
    );
    copyrightLabel_->setAlignment(Qt::AlignCenter);
    copyrightLabel_->setStyleSheet("color: #757575;");
    layout->addWidget(copyrightLabel_);

    layout->addStretch();

    // 按钮区域
    auto* buttonLayout = new QHBoxLayout();

    licenseButton_ = new QPushButton("View License", this);
    connect(licenseButton_, &QPushButton::clicked, this, &AboutDialog::onLicenseClicked);
    buttonLayout->addWidget(licenseButton_);

    buttonLayout->addStretch();

    qtButton_ = new QPushButton("About Qt", this);
    connect(qtButton_, &QPushButton::clicked, this, &AboutDialog::onQtAboutClicked);
    buttonLayout->addWidget(qtButton_);

    layout->addLayout(buttonLayout);

    // 对话框按钮
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &AboutDialog::onOkClicked);
    layout->addWidget(buttons);
}

void AboutDialog::onOkClicked()
{
    accept();
}

void AboutDialog::onLicenseClicked()
{
    QString licenseText =
        "MIT License\n\n"
        "Copyright (c) 2026\n\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
        "of this software and associated documentation files (the \"Software\"), to deal\n"
        "in the Software without restriction, including without limitation the rights\n"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
        "copies of the Software, and to permit persons to whom the Software is\n"
        "furnished to do so, subject to the following conditions:\n\n"
        "The above copyright notice and this permission notice shall be included in all\n"
        "copies or substantial portions of the Software.\n\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        "SOFTWARE.";

    QMessageBox::information(this, "License", licenseText);
}

void AboutDialog::onQtAboutClicked()
{
    QMessageBox::aboutQt(this, "About Qt");
}

} // namespace calc::ui::qt
