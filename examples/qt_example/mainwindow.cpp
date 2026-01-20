#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();

    // Set window properties
    setWindowTitle("Qt Calculator Example");
    resize(400, 300);
}

void MainWindow::setupUI()
{
    // Create central widget and layout
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    // Create UI elements
    titleLabel = new QLabel("Qt Calculator Integration Test", centralWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    infoLabel = new QLabel(
        "This example demonstrates Qt integration with the calculator project.\n"
        "Click the button to test basic Qt functionality.",
        centralWidget
    );
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);

    testButton = new QPushButton("Test Qt Button", centralWidget);
    testButton->setMinimumHeight(40);

    statusLabel = new QLabel("Button clicked 0 times", centralWidget);
    statusLabel->setAlignment(Qt::AlignCenter);

    // Add widgets to layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(testButton);
    mainLayout->addWidget(statusLabel);
    mainLayout->addStretch();

    // Set central widget
    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections()
{
    connect(testButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
}

void MainWindow::onButtonClicked()
{
    clickCount++;
    statusLabel->setText(QString("Button clicked %1 times").arg(clickCount));

    // Show a message box on every 5th click
    if (clickCount % 5 == 0) {
        QMessageBox::information(
            this,
            "Qt Test",
            QString("Great! Qt is working properly.\nYou've clicked %1 times.").arg(clickCount)
        );
    }

    // Change button text occasionally
    if (clickCount == 1) {
        testButton->setText("Qt is Working!");
    } else if (clickCount == 3) {
        testButton->setText("Keep Clicking!");
    }
}