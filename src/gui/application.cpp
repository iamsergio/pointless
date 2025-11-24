// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"
#include "../core/logger.h"

#include <QQuickStyle>
#include <QDebug>

Application::Application(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
#ifdef POINTLESS_DEVELOPER_MODE
    _bot = new spix::QtQmlBot();
    _bot->runTestServer(_server);
#endif

    QQuickStyle::setStyle("Fusion");

    _engine.loadFromModule("pointless", "Main");

    if (_engine.rootObjects().isEmpty()) {
        P_LOG_ERROR("No root objects loaded");
        std::exit(-1);
    }

    if (qEnvironmentVariableIsSet("VERBOSE")) {
        P_LOG_INFO("VERBOSE environment variable is set, using debug log level");
        Logger::getLogger()->set_log_level(quill::LogLevel::Debug);
    }
}
