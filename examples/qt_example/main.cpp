#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    // Create Qt application
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Qt Calculator Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Calc Project");

    // Try to use Fusion style for consistent cross-platform appearance
    app.setStyle(QStyleFactory::create("Fusion"));

    // Create and show main window
    MainWindow window;
    window.show();

    // Display startup message
    qInfo() << "Qt Calculator Example starting...";
    qInfo() << "Qt Version:" << qVersion();

    // Execute application
    return app.exec();
}