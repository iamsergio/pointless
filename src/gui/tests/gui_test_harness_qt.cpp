// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui_test_harness.h"
#include "gui/application.h"
#include "gui/Clock.h"
#include "gui/gui_controller.h"
#include "gui/data_controller.h"
#include "gui/tests/test_utils.h"
#include "core/logger.h"

#include <Spix/QtQmlBot.h>
#include "../../../3rdparty/spix/libs/Scenes/QtQuick/src/Utils/DebugDump.h"

#include <QDebug>
#include <QGuiApplication>
#include <QMetaObject>
#include <QQuickWindow>

using namespace pointless;
using namespace pointless::tests;

struct GuiTestHarness::Private
{
    Private(int &argc, char **argv)
        : app(argc, argv, "com.pointless.tests", initPlatform())
    {
    }

    pointless::Application app;
    spix::QtQmlBot bot;
};

void GuiTestHarness::setTestNow(int year, int month, int day, int hour, int minute)
{
    Gui::Clock::setTestNow(QDateTime(QDate(year, month, day), QTime(hour, minute)));
}

GuiTestHarness::GuiTestHarness(int &argc, char **argv)
    : d(std::make_unique<Private>(argc, argv))
{
}

GuiTestHarness::~GuiTestHarness() = default;

int GuiTestHarness::run(spix::TestServer &test)
{
    d->bot.runTestServer(test);
    return d->app.exec();
}

void GuiTestHarness::finish()
{
    if (qApp->platformName() == QStringLiteral("offscreen"))
        qApp->quit();
    else
        P_LOG_INFO("Test finished! Not quitting since using non-offscreen for visual debugging");
}

core::LocalData &GuiTestHarness::localData()
{
    return GuiController::instance()->dataController()->localData();
}

void GuiTestHarness::runOnGuiThread(const std::function<void()> &fn)
{
    QMetaObject::invokeMethod(qApp, [&fn]() { fn(); }, Qt::BlockingQueuedConnection);
}

void GuiTestHarness::dumpTree()
{
    runOnGuiThread([] {
        const auto windows = QGuiApplication::topLevelWindows();
        for (auto window : windows) {
            if (auto quickWindow = qobject_cast<QQuickWindow *>(window)) {
                qDebug() << "\n=== Dumping window: " << quickWindow->objectName() << " ===";
                spix::utils::DumpQQuickItemTree(quickWindow->contentItem());
            }
        }
    });
}
