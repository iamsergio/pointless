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
#include <memory>

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

    if (parser.isSet(testSupabaseOption)) {
        pointless::core::Context context;
        context.dataProviderType = IDataProvider::Type::TestSupabase;
        pointless::core::Context::setContext(context);
    }

    QQuickStyle::setStyle(QStringLiteral("Fusion"));

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
