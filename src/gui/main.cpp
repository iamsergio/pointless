// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../core/logger.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDirIterator>
#include <QDebug>
#include <QQuickStyle>

void printQrcContents(const QString &path = ":/", int depth = 0)
{
    QDirIterator it(path, QDirIterator::Subdirectories);
    QString indent = QString("  ").repeated(depth);

    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo = it.fileInfo();

        if (fileInfo.isDir()) {
            qDebug() << indent << "Dir:" << filePath;
        } else {
            qDebug() << indent << "File:" << filePath;
        }
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");
    QQmlApplicationEngine engine;

    engine.loadFromModule("pointless", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    if (qEnvironmentVariableIsSet("VERBOSE")) {
        P_LOG_INFO("VERBOSE environment variable is set, using debug log level");
        Logger::getLogger()->set_log_level(quill::LogLevel::Debug);
    }

    // printQrcContents();

    return app.exec();
}
