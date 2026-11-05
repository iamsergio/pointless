// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"
#include "gui_controller.h"
#include "data_controller.h"
#include "pointless_init.h"
#include "pointless_settings.h"

#include "gui/qt_logger.h"
#include "core/calendar_provider.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QStyleHints>

#include <chrono>
#include <cstdlib>

#if defined(Q_OS_LINUX) && defined(POINTLESS_DEVELOPER_MODE)
#include <sys/prctl.h>
#endif

using namespace pointless;

Application::Application(int &argc, char **argv, const QString &orgName, Options options)
    : PointlessAppBase(argc, argv)
{
    Q_UNUSED(options)

#if defined(Q_OS_LINUX) && defined(POINTLESS_DEVELOPER_MODE)
    // Opts this process into same-uid ptrace (e.g. from qt-commander's injector) without
    // lowering /proc/sys/kernel/yama/ptrace_scope system-wide, which would expose every
    // other process (browsers, ssh-agent, ...) to the same relaxation.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
#endif

    QCoreApplication::setApplicationName(QString::fromLatin1(settingsApplicationName));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    QCoreApplication::setOrganizationName(orgName);
    setSettingsIdentity(orgName, QString::fromLatin1(settingsApplicationName));

    styleHints()->setWheelScrollLines(styleHints()->wheelScrollLines() * 2);

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

    QCommandLineOption benchmarkStartupOption(QStringLiteral("benchmark-startup"), QStringLiteral("Exit after first frame is rendered"));
    parser.addOption(benchmarkStartupOption);

#ifndef Q_OS_DARWIN
    QCommandLineOption dumpICalOption(QStringLiteral("dump-ical"), QStringLiteral("Write the iCal data of each calendar fetch to the temp directory"));
    parser.addOption(dumpICalOption);
#endif

    parser.process(*this);

#ifndef Q_OS_DARWIN
    if (parser.isSet(dumpICalOption))
        core::setICalDumpDirectory(QDir(QDir::tempPath()).filePath(QStringLiteral("pointless-ical")).toStdString());
#endif

    InitOptions initOptions;
    initOptions.debug = parser.isSet(debugOption);
    initOptions.restoreAuth = !parser.isSet(noRestoreAuthOption);
    initOptions.testSupabase = parser.isSet(testSupabaseOption);
    initialize(initOptions);

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

            connect(t1, &QTimer::timeout, gc, [switchView, t2, benchmarkInterval] { switchView(GV::Soon, FV::Soon, "Soon"); t2->start(benchmarkInterval); });
            connect(t2, &QTimer::timeout, gc, [switchView, t3, benchmarkInterval] { switchView(GV::Later, FV::Later, "Later"); t3->start(benchmarkInterval); });
            connect(t3, &QTimer::timeout, gc, [switchView, t4, benchmarkInterval] { switchView(GV::Goals, FV::Goals, "Goals"); t4->start(benchmarkInterval); });
            connect(t4, &QTimer::timeout, gc, [switchView] { switchView(GV::Week, FV::Week, "Week"); QCoreApplication::exit(0); });

            t1->start(benchmarkInterval);
        });
    }

#ifdef SLINT_OVER_CORE
    // The UI is Slint, created by the bridge's main() (src/slint/bridge)
    return;
#endif

    _engine.loadFromModule("pointless", "Main");

    if (_engine.rootObjects().isEmpty()) {
        P_LOG_ERROR("No root objects loaded");
        std::exit(-1);
    }

    if (parser.isSet(benchmarkStartupOption)) {
        auto *window = qobject_cast<QQuickWindow *>(_engine.rootObjects().first());
        if (window) {
            connect(window, &QQuickWindow::frameSwapped, this, [] {
                P_LOG_INFO("Benchmark startup: first frame rendered, exiting");
                quit();
            });
        }
    }
}

Application::~Application()
{
    shutdown();
}
