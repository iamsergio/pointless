// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"
#include "gui_controller.h"

#include "gui/qt_logger.h"
#include "core/context.h"
#include "core/utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QStandardPaths>

#include <cstdlib>

using namespace pointless;

namespace {
void printDebugInfo()
{
    P_LOG_INFO("AppDataLocation: {}", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    P_LOG_INFO("localFile: {}", core::Context::self().localFilePath());
}
}

Application::Application(int &argc, char **argv, Options options)
    : QGuiApplication(argc, argv)
{
    Q_UNUSED(options)
    QCoreApplication::setApplicationName(QStringLiteral("pointless"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Pointless"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption testSupabaseOption(QStringLiteral("test-supabase"), QStringLiteral("Use supabase test user"));
    parser.addOption(testSupabaseOption);

    parser.process(*this);

    core::Logger::initLogLevel();

    if (pointless::isIOS()) {
        // Set the data dir before creating context, as ctor reads the data dir
        core::Context::setClientDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    }

    // Context might have been set already in tests
    if (!core::Context::hasContext()) {
        core::Context::setContext(parser.isSet(testSupabaseOption) ? core::Context::defaultContextForSupabaseTesting()
                                                                   : core::Context::defaultContextForSupabaseRelease());
    }

    printDebugInfo();

    // Initialize the controllers and models before loading QML
    GuiController::instance();

    QQuickStyle::setStyle(QStringLiteral("Fusion"));

    _engine.loadFromModule("pointless", "Main");

    if (_engine.rootObjects().isEmpty()) {
        P_LOG_ERROR("No root objects loaded");
        std::exit(-1);
    }
}
