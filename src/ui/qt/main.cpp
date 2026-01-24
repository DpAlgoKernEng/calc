/**
 * @file main.cpp
 * @brief Qt GUI 应用入口
 */

#include "calc/ui/qt/widgets/main_window.h"
#include <QApplication>
#include <QStyleFactory>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

int main(int argc, char *argv[])
{
    // 创建 Qt 应用
    QApplication app(argc, argv);
    qDebug() << "QApplication created";
    qDebug() << "Library paths:" << QCoreApplication::libraryPaths();
    qDebug() << "Application dir:" << QCoreApplication::applicationDirPath();
    qDebug() << "Platform plugin paths:" << QCoreApplication::libraryPaths();

    // 设置应用元数据
    app.setApplicationName("Calc");
    app.setApplicationDisplayName("Calc");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Calc Project");
    app.setOrganizationDomain("calc.example.com");

    // 命令行解析
    QCommandLineParser parser;
    parser.setApplicationDescription("Cross-platform calculator");
    parser.addHelpOption();
    parser.addVersionOption();

    // 添加自定义选项
    QCommandLineOption modeOption(QStringList() << "m" << "mode",
                                 "Start in specified mode (standard|scientific|programmer)",
                                 "mode", "standard");
    parser.addOption(modeOption);

    QCommandLineOption styleOption(QStringList() << "s" << "style",
                                  "Use specified Qt style (Fusion|Windows|macOS)",
                                  "style");
    parser.addOption(styleOption);

    parser.process(app);

    // 设置样式
    QString style = parser.value(styleOption);
    if (!style.isEmpty()) {
        if (style == "Fusion" || style == "Windows" || style == "macOS") {
            app.setStyle(QStyleFactory::create(style));
        }
    } else {
        // 默认使用 Fusion 风格以获得一致的外观
        app.setStyle(QStyleFactory::create("Fusion"));
    }

    // 设置全局样式表 - 暂时禁用以调试崩溃
    app.setStyleSheet("");

    // 创建主窗口
    calc::ui::qt::MainWindow window;

    // 设置初始模式
    QString initialMode = parser.value(modeOption);
    if (initialMode == "scientific") {
        window.setProperty("initialMode", "scientific");
    } else if (initialMode == "programmer") {
        window.setProperty("initialMode", "programmer");
    }

    window.show();

    qInfo() << "Calc GUI started";
    qInfo() << "Qt Version:" << qVersion();
    qInfo() << "Mode:" << (initialMode.isEmpty() ? "standard" : initialMode);

    return app.exec();
}
