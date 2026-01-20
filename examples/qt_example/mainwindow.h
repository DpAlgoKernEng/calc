#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onButtonClicked();

private:
    void setupUI();
    void setupConnections();

    // UI Elements
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *infoLabel;
    QPushButton *testButton;
    QLabel *statusLabel;

    int clickCount = 0;
};

#endif // MAINWINDOW_H