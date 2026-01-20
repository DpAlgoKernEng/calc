/**
 * @file about_dialog.h
 * @brief 关于对话框
 */

#ifndef CALC_UI_QT_DIALOGS_ABOUT_DIALOG_H
#define CALC_UI_QT_DIALOGS_ABOUT_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

namespace calc::ui::qt {

/**
 * @brief 关于对话框
 */
class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

private slots:
    void onOkClicked();
    void onLicenseClicked();
    void onQtAboutClicked();

private:
    void setupUI();

    QLabel* titleLabel_;
    QLabel* versionLabel_;
    QLabel* descriptionLabel_;
    QLabel* copyrightLabel_;
    QPushButton* licenseButton_;
    QPushButton* qtButton_;
    QPushButton* okButton_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_DIALOGS_ABOUT_DIALOG_H
