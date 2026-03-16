// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"
#include "gui_controller.h"
#include "data_controller.h"

#include "gui/qt_logger.h"
#include "core/context.h"
#include "core/utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QSettings>

#include <chrono>
#include <cstdlib>

using namespace pointless;

namespace {
void printDebugInfo()
{
    P_LOG_INFO("AppDataLocation: {}", QSettings().fileName().toStdString());
    P_LOG_INFO("localFile: {}", core::Context::self().localFilePath());
}
}

Application::Application(int &argc, char **argv, const QString &orgName, Options options)
    : QGuiApplication(argc, argv)
{
    Q_UNUSED(options)
    QCoreApplication::setApplicationName(QStringLiteral("pointless"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    QCoreApplication::setOrganizationName(orgName);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Pointless"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption testSupabaseOption(QStringLiteral("test-supabase"), QStringLiteral("Use supabase test user"));
    parser.addOption(testSupabaseOption);

    QCommandLineOption noRestoreAuthOption(QStringLiteral("no-restore-auth"), QStringLiteral("Do not restore authentication on startup"));
    parser.addOption(noRestoreAuthOption);

    QCommandLineOption debugOption(QStringLiteral("debug"), QStringLiteral("Enable debug features"));
    parser.addOption(debugOption);

#ifdef POINTLESS_DEVELOPER_MODE
    QCommandLineOption loginOption(QStringLiteral("login"), QStringLiteral("Auto-login with default credentials"));
    parser.addOption(loginOption);
#endif

    QCommandLineOption benchmarkOption(QStringLiteral("benchmark"), QStringLiteral("Run benchmark after login"));
    parser.addOption(benchmarkOption);

    parser.process(*this);

    GuiController::setDebugMode(parser.isSet(debugOption));

    core::Logger::initLogLevel();

    if (pointless::isIOS()) {
        // Set the data dir before creating context, as ctor reads the data dir
        core::Context::setClientDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    }

    // Context might have been set already in tests
    if (!core::Context::hasContext()) {
        auto startupOptions = static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth);
        if (parser.isSet(noRestoreAuthOption)) {
            startupOptions = static_cast<unsigned int>(core::Context::StartupOption::None);
        }

        core::Context::setContext(parser.isSet(testSupabaseOption) ? core::Context::defaultContextForSupabaseTesting(startupOptions)
                                                                   : core::Context::defaultContextForSupabaseRelease(startupOptions));
    }

    printDebugInfo();

    // Initialize the controllers and models before loading QML
    GuiController::instance();

    if (parser.isSet(benchmarkOption)) {
        auto *gc = GuiController::instance();
        connect(gc->dataController(), &DataController::refreshFinished, gc, [gc](bool success, const QString &) {
            if (!success || !gc->isAuthenticated())
                return;

            auto switchView = [gc](GuiController::ViewType gcView, TaskFilterModel::ViewType filterView, const char *viewName) {
                gc->setCurrentViewType(gcView);
                gc->taskFilterModel()->setViewType(filterView);
                P_LOG_INFO("Benchmark [{}]: filtered={} tasks, total={}", viewName, gc->taskFilterModel()->rowCount(), gc->taskModel()->rowCount());
            };

            using GV = GuiController::ViewType;
            using FV = TaskFilterModel::ViewType;

            using namespace std::chrono_literals;
            constexpr auto benchmarkInterval = 500ms;

            auto *t1 = new QTimer(gc);
            t1->setSingleShot(true);
            auto *t2 = new QTimer(gc);
            t2->setSingleShot(true);
            auto *t3 = new QTimer(gc);
            t3->setSingleShot(true);
            auto *t4 = new QTimer(gc);
            t4->setSingleShot(true);

            connect(t1, &QTimer::timeout, gc, [switchView, t2] { switchView(GV::Soon, FV::Soon, "Soon"); t2->start(benchmarkInterval); });
            connect(t2, &QTimer::timeout, gc, [switchView, t3] { switchView(GV::Later, FV::Later, "Later"); t3->start(benchmarkInterval); });
            connect(t3, &QTimer::timeout, gc, [switchView, t4] { switchView(GV::Goals, FV::Goals, "Goals"); t4->start(benchmarkInterval); });
            connect(t4, &QTimer::timeout, gc, [switchView] { switchView(GV::Week, FV::Week, "Week"); QCoreApplication::exit(0); });

            t1->start(benchmarkInterval);
        });
    }

    QQuickStyle::setStyle(QStringLiteral("Fusion"));

    _engine.loadFromModule("pointless", "Main");

    if (_engine.rootObjects().isEmpty()) {
        P_LOG_ERROR("No root objects loaded");
        std::exit(-1);
    }
}

Application::~Application()
{
    P_LOG_INFO("Application exiting");
    delete GuiController::instance();
}
