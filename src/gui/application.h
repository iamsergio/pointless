// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifdef POINTLESS_DEVELOPER_MODE
#include <Spix/QtQmlBot.h>
#include <Spix/AnyRpcServer.h>
#endif

class Application : public QGuiApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

private:
    QQmlApplicationEngine _engine;
#ifdef POINTLESS_DEVELOPER_MODE
    spix::AnyRpcServer _server;
    spix::QtQmlBot *_bot = nullptr;
#endif
};
