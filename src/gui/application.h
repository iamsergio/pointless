// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace pointless {

class Application : public QGuiApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);

private:
    QQmlApplicationEngine _engine;
};

}
