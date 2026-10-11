// SPDX-FileCopyrightText: 2026 Sergio Martins
// SPDX-License-Identifier: MIT

// The Slint implementation of the GUI test harness: the Slint UI over the Qt C++ code, driven by Spix::Slint.

#include "gui_test_harness.h"
#include "app.h"
#include "bind.h"
#include "gui/application.h"
#include "gui/Clock.h"
#include "gui/gui_controller.h"
#include "gui/data_controller.h"
#include "gui/tests/test_utils.h"
#include "core/logger.h"

#include <Spix/SlintBot.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QMetaObject>

using namespace pointless;
using namespace pointless::tests;

struct GuiTestHarness::Private
{
    Private(int &argc, char **argv)
        : app(argc, argv, "com.pointless.tests", initPlatform())
        , window(ui::App::create())
    {
        bindPointless(bridge, *window, *GuiController::instance());
    }

    pointless::Application app;
    slint::ComponentHandle<ui::App> window;
    kdab::qtslintbridge::Bridge bridge;
    spix::SlintBot bot;
};

const char *GuiTestHarness::name()
{
    return "slint";
}

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
    d->bot.addWindow("mainWindow", d->window);
    d->bot.runTestServer(test);
    d->window->run();
    return 0;
}

void GuiTestHarness::finish()
{
    if (qApp->platformName() == QStringLiteral("offscreen"))
        slint::quit_event_loop();
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
    P_LOG_INFO("dumpTree() isn't implemented for Slint");
}
