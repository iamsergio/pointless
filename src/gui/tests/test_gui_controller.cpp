// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/data_controller.h"
#include "gui/taskmodel.h"
#include "gui/gui_controller.h"
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

int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;

    QCoreApplication::setOrganizationName("com.pointless.tests");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
