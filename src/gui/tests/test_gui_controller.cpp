// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/data_controller.h"
#include "gui/taskmodel.h"
#include "gui/gui_controller.h"
#include "gui/pomodoro_controller.h"
#include "gui/tests/test_utils.h"
#include "gui/local_settings.h"

#include "core/context.h"
#include "core/data_provider.h"
#include "core/logger.h"

#include <QTest>

#include <gtest/gtest.h>

#include <filesystem>

using namespace pointless;

static int g_argc;
static char **g_argv;

constexpr const char *s_filename = "/tmp/pointless_test_gui_controller_sync.json";

void initData(DataController &controller, std::optional<core::Data> localData, const core::Data &remoteData)
{
    core::Logger::initLogLevel();

    ASSERT_TRUE(controller.loginWithDefaults());

    std::filesystem::remove(s_filename);
    if (localData.has_value()) {
        auto saveResult = controller._localData.setDataAndSave(*localData);
        ASSERT_TRUE(saveResult) << "Failed to save local data: " << saveResult.error();
    } else {
        controller._localData.setData({});
    }

    ASSERT_TRUE(controller.pushRemoteData(remoteData).has_value());
}

TEST(GuiControllerTest, MoveToNextMondayRemovesCurrentTag)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-task-next-monday-test";
    task.title = "Task for next monday test";
    task.tags = { core::BUILTIN_TAG_CURRENT };
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);

    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    auto *storedTask = controller->taskModel()->taskForUuid("uuid-task-next-monday-test");
    ASSERT_NE(storedTask, nullptr);
    EXPECT_TRUE(storedTask->containsTag(core::BUILTIN_TAG_CURRENT));

    guiController->moveTaskToNextMonday("uuid-task-next-monday-test");

    auto *updatedTask = controller->taskModel()->taskForUuid("uuid-task-next-monday-test");
    ASSERT_NE(updatedTask, nullptr);
    EXPECT_FALSE(updatedTask->containsTag(core::BUILTIN_TAG_CURRENT))
        << "moveTaskToNextMonday should remove the current tag";

    delete guiController;
}

TEST(GuiControllerTest, PomodoroStopsWhenTaskMarkedDone)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-pomodoro-done-test";
    task.title = "Pomodoro done test";
    task.tags = { core::BUILTIN_TAG_CURRENT };
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);
    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    guiController->pomodoroController()->play("uuid-pomodoro-done-test");
    ASSERT_TRUE(guiController->pomodoroController()->isRunning());
    ASSERT_EQ(guiController->pomodoroController()->currentTaskUuid(), "uuid-pomodoro-done-test");

    controller->taskModel()->setTaskDone("uuid-pomodoro-done-test", true);

    EXPECT_FALSE(guiController->pomodoroController()->isRunning());
    EXPECT_TRUE(guiController->pomodoroController()->currentTaskUuid().isEmpty());

    delete guiController;
}

TEST(GuiControllerTest, PomodoroStopsWhenTaskMovedToSoon)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-pomodoro-soon-test";
    task.title = "Pomodoro soon test";
    task.tags = { core::BUILTIN_TAG_CURRENT };
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);
    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    guiController->pomodoroController()->play("uuid-pomodoro-soon-test");
    ASSERT_TRUE(guiController->pomodoroController()->isRunning());

    guiController->moveTaskToSoon("uuid-pomodoro-soon-test");

    EXPECT_FALSE(guiController->pomodoroController()->isRunning());
    EXPECT_TRUE(guiController->pomodoroController()->currentTaskUuid().isEmpty());

    delete guiController;
}

TEST(GuiControllerTest, PomodoroStopsWhenTaskMovedToLater)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-pomodoro-later-test";
    task.title = "Pomodoro later test";
    task.tags = { core::BUILTIN_TAG_CURRENT };
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);
    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    guiController->pomodoroController()->play("uuid-pomodoro-later-test");
    ASSERT_TRUE(guiController->pomodoroController()->isRunning());

    guiController->moveTaskToLater("uuid-pomodoro-later-test");

    EXPECT_FALSE(guiController->pomodoroController()->isRunning());
    EXPECT_TRUE(guiController->pomodoroController()->currentTaskUuid().isEmpty());

    delete guiController;
}

TEST(GuiControllerTest, PomodoroUnaffectedWhenDifferentTaskMarkedDone)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task taskA;
    taskA.uuid = "uuid-pomodoro-other-a";
    taskA.title = "Pomodoro task A";
    taskA.tags = { core::BUILTIN_TAG_CURRENT };
    taskA.revision = 0;
    remoteData.addTask(taskA);

    core::Task taskB;
    taskB.uuid = "uuid-pomodoro-other-b";
    taskB.title = "Pomodoro task B";
    taskB.tags = { core::BUILTIN_TAG_CURRENT };
    taskB.revision = 0;
    remoteData.addTask(taskB);

    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);
    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    guiController->pomodoroController()->play("uuid-pomodoro-other-a");
    ASSERT_TRUE(guiController->pomodoroController()->isRunning());

    controller->taskModel()->setTaskDone("uuid-pomodoro-other-b", true);

    EXPECT_TRUE(guiController->pomodoroController()->isRunning());
    EXPECT_EQ(guiController->pomodoroController()->currentTaskUuid(), "uuid-pomodoro-other-a");

    delete guiController;
}

TEST(GuiControllerTest, PomodoroStopsWhenTaskMovedToEvening)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-pomodoro-evening-test";
    task.title = "Pomodoro evening test";
    task.tags = { core::BUILTIN_TAG_CURRENT };
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);
    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    guiController->pomodoroController()->play("uuid-pomodoro-evening-test");
    ASSERT_TRUE(guiController->pomodoroController()->isRunning());

    guiController->moveTaskToEvening("uuid-pomodoro-evening-test");

    EXPECT_FALSE(guiController->pomodoroController()->isRunning());
    EXPECT_TRUE(guiController->pomodoroController()->currentTaskUuid().isEmpty());

    delete guiController;
}

TEST(GuiControllerTest, IsEveningFalseAt13h)
{
    EXPECT_FALSE(GuiController::isEveningForHour(13));
}

TEST(GuiControllerTest, IsEveningTrueAt18h)
{
    EXPECT_TRUE(GuiController::isEveningForHour(18));
}

TEST(GuiControllerTest, CalendarProviderConfiguredWithoutEnvVars)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();

    EXPECT_EQ(guiController->calendarProviderConfigured(), std::getenv("POINTLESS_CALDAV_URL") != nullptr);

    delete guiController;
}

TEST(GuiControllerTest, ParsePassStoreBasic)
{
    const auto result = GuiController::parsePassStoreOutput("user:foo\npass:bar");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_EQ(result["pass"].toString(), "bar");
    EXPECT_FALSE(result.contains("caldav-pass"));
}

TEST(GuiControllerTest, ParsePassStoreWithCaldavPass)
{
    const auto result = GuiController::parsePassStoreOutput("user:foo\npass:bar\ncaldav-pass:baz");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_EQ(result["pass"].toString(), "bar");
    EXPECT_EQ(result["caldav-pass"].toString(), "baz");
    EXPECT_FALSE(result.contains("caldav-url"));
    EXPECT_FALSE(result.contains("caldav-user"));
}

TEST(GuiControllerTest, ParsePassStoreCaldavBeforePass)
{
    const auto result = GuiController::parsePassStoreOutput("user:foo\ncaldav-pass:baz\npass:bar");
    EXPECT_EQ(result["pass"].toString(), "bar");
    EXPECT_EQ(result["caldav-pass"].toString(), "baz");
    EXPECT_NE(result["pass"].toString(), result["caldav-pass"].toString());
}

TEST(GuiControllerTest, ParsePassStorePartialOutput)
{
    const auto result = GuiController::parsePassStoreOutput("user:foo");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_FALSE(result.contains("pass"));
    EXPECT_FALSE(result.contains("caldav-pass"));
}

TEST(GuiControllerTest, ParsePassStoreWithCaldavUrlAndUser)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\ncaldav-pass:baz\ncaldav-url:https://dav.example.com\ncaldav-user:davuser");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_EQ(result["pass"].toString(), "bar");
    EXPECT_EQ(result["caldav-pass"].toString(), "baz");
    EXPECT_EQ(result["caldav-url"].toString(), "https://dav.example.com");
    EXPECT_EQ(result["caldav-user"].toString(), "davuser");
}

TEST(GuiControllerTest, ParsePassStoreCaldavUrlOnly)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\ncaldav-url:https://dav.example.com");
    EXPECT_EQ(result["caldav-url"].toString(), "https://dav.example.com");
    EXPECT_FALSE(result.contains("caldav-pass"));
    EXPECT_FALSE(result.contains("caldav-user"));
}

TEST(GuiControllerTest, ParsePassStoreEmptyOutput)
{
    const auto result = GuiController::parsePassStoreOutput("");
    EXPECT_TRUE(result.isEmpty());
}

TEST(GuiControllerTest, ParsePassStoreNumberedFields)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\n"
        "caldav-url-2:https://server2.example.com/dav\n"
        "caldav-user-2:user2\n"
        "caldav-pass-2:pass2\n"
        "caldav-url-3:https://server3.example.com/dav\n"
        "caldav-user-3:user3\n"
        "caldav-pass-3:pass3");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_EQ(result["caldav-url-2"].toString(), "https://server2.example.com/dav");
    EXPECT_EQ(result["caldav-user-2"].toString(), "user2");
    EXPECT_EQ(result["caldav-pass-2"].toString(), "pass2");
    EXPECT_EQ(result["caldav-url-3"].toString(), "https://server3.example.com/dav");
    EXPECT_EQ(result["caldav-user-3"].toString(), "user3");
    EXPECT_EQ(result["caldav-pass-3"].toString(), "pass3");
    EXPECT_FALSE(result.contains("caldav-url"));
}

TEST(GuiControllerTest, ParsePassStoreMixedUnnumberedAndNumbered)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\n"
        "caldav-url:https://server1.example.com/dav\n"
        "caldav-user:user1\n"
        "caldav-pass:pass1\n"
        "caldav-url-2:https://server2.example.com/dav\n"
        "caldav-user-2:user2\n"
        "caldav-pass-2:pass2");
    EXPECT_EQ(result["caldav-url"].toString(), "https://server1.example.com/dav");
    EXPECT_EQ(result["caldav-user"].toString(), "user1");
    EXPECT_EQ(result["caldav-pass"].toString(), "pass1");
    EXPECT_EQ(result["caldav-url-2"].toString(), "https://server2.example.com/dav");
    EXPECT_EQ(result["caldav-user-2"].toString(), "user2");
    EXPECT_EQ(result["caldav-pass-2"].toString(), "pass2");
}

TEST(GuiControllerTest, ParsePassStoreNumberedGaps)
{
    const auto result = GuiController::parsePassStoreOutput(
        "caldav-url-2:https://server2.example.com/dav\n"
        "caldav-url-5:https://server5.example.com/dav");
    EXPECT_EQ(result["caldav-url-2"].toString(), "https://server2.example.com/dav");
    EXPECT_EQ(result["caldav-url-5"].toString(), "https://server5.example.com/dav");
    EXPECT_FALSE(result.contains("caldav-url-3"));
    EXPECT_FALSE(result.contains("caldav-url-4"));
}

TEST(GuiControllerTest, ParsePassStoreWithIcalUrl)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\nical-url:https://example.com/calendar.ics");
    EXPECT_EQ(result["user"].toString(), "foo");
    EXPECT_EQ(result["ical-url"].toString(), "https://example.com/calendar.ics");
    EXPECT_FALSE(result.contains("ical-name"));
}

TEST(GuiControllerTest, ParsePassStoreWithIcalUrlAndName)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\nical-url:https://example.com/calendar.ics\nical-name:Work Events");
    EXPECT_EQ(result["ical-url"].toString(), "https://example.com/calendar.ics");
    EXPECT_EQ(result["ical-name"].toString(), "Work Events");
}

TEST(GuiControllerTest, ParsePassStoreNumberedIcalFields)
{
    const auto result = GuiController::parsePassStoreOutput(
        "ical-url-2:https://example.com/holidays.ics\n"
        "ical-name-2:Holidays\n"
        "ical-url-3:https://other.com/birthdays.ics\n"
        "ical-name-3:Birthdays");
    EXPECT_EQ(result["ical-url-2"].toString(), "https://example.com/holidays.ics");
    EXPECT_EQ(result["ical-name-2"].toString(), "Holidays");
    EXPECT_EQ(result["ical-url-3"].toString(), "https://other.com/birthdays.ics");
    EXPECT_EQ(result["ical-name-3"].toString(), "Birthdays");
    EXPECT_FALSE(result.contains("ical-url"));
}

TEST(GuiControllerTest, ParsePassStoreMixedCaldavAndIcal)
{
    const auto result = GuiController::parsePassStoreOutput(
        "user:foo\npass:bar\n"
        "caldav-url:https://caldav.example.com\n"
        "caldav-user:user1\n"
        "caldav-pass:pass1\n"
        "ical-url:https://example.com/calendar.ics\n"
        "ical-name:Work Events\n"
        "ical-url-2:https://other.com/holidays.ics\n"
        "ical-name-2:Holidays");
    EXPECT_EQ(result["caldav-url"].toString(), "https://caldav.example.com");
    EXPECT_EQ(result["caldav-user"].toString(), "user1");
    EXPECT_EQ(result["caldav-pass"].toString(), "pass1");
    EXPECT_EQ(result["ical-url"].toString(), "https://example.com/calendar.ics");
    EXPECT_EQ(result["ical-name"].toString(), "Work Events");
    EXPECT_EQ(result["ical-url-2"].toString(), "https://other.com/holidays.ics");
    EXPECT_EQ(result["ical-name-2"].toString(), "Holidays");
}

int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;

    initPlatform();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
