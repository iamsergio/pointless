// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDirIterator>
#include <QDebug>

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
    QQmlApplicationEngine engine;

    engine.loadFromModule("pointless", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    // printQrcContents();

    return app.exec();
}
