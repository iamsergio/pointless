// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"
#include "core/logger.h"
#include "core/test_supabase_provider.h"
#include "core/data_provider.h"
#include "core/context.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>

#include <cstdlib>

using namespace pointless;

Application::Application(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("pointless"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Pointless"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption testSupabaseOption(QStringLiteral("test-supabase"), QStringLiteral("Use supabase test user"));
    parser.addOption(testSupabaseOption);

    parser.process(*this);

    if (qEnvironmentVariableIsSet("VERBOSE")) {
        P_LOG_INFO("VERBOSE environment variable is set, using debug log level");
        Logger::getLogger()->set_log_level(quill::LogLevel::Debug);
    }

    // Context might have been set already in tests
    if (!core::Context::hasContext()) {
        core::Context::setContext(parser.isSet(testSupabaseOption) ? core::Context::defaultContextForSupabaseTesting()
                                                                   : core::Context::defaultContextForSupabaseRelease());
    }

    QQuickStyle::setStyle(QStringLiteral("Fusion"));

    _engine.loadFromModule("pointless", "Main");

    if (_engine.rootObjects().isEmpty()) {
        P_LOG_ERROR("No root objects loaded");
        std::exit(-1);
    }
}
