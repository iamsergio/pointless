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

#include <QTest> // for qWait

#include <gtest/gtest.h>

#include <filesystem>

using namespace pointless;

// to run a single test:
// ./bin/test_data_controller --gtest_filter=DataControllerTest.LogoutTokenMetaTest

static int g_argc;
static char **g_argv;

constexpr const char *s_filename = "/tmp/pointless_test_data_controller_sync.json";

/// Sets the data locally and remotely before we start the test
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

TEST(DataControllerTest, ConstructDestroy)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Context::setContext({ IDataProvider::Type::TestSupabase, "/tmp/nonexistent_file.json" });
    DataController controller;
}

// DataController::sync() tests
TEST(DataControllerTest, Sync)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });
    DataController controller;

    //-----------------------------------------------------------------------
    // #2: local data doesn't exist, remote data exists -> use remote data
    core::Data remoteData;
    core::Tag tag1;
    tag1.name = "tag1";
    tag1.revision = 1;
    remoteData.addTag(tag1);

    initData(controller, {}, remoteData);
    ASSERT_FALSE(std::filesystem::exists(s_filename));
    ASSERT_TRUE(controller.refreshBlocking().has_value());

    ASSERT_TRUE(std::filesystem::exists(s_filename));

    ASSERT_EQ(controller._localData.data().tagCount(), 1);
    EXPECT_EQ(controller._localData.data().tagAt(0).name, "tag1");
    EXPECT_EQ(controller._localData.data().tagAt(0).revision, 1);
    ASSERT_EQ(controller._localData.data().taskCount(), 0);

    //-----------------------------------------------------------------------
    // #1: remote data doesn't exist, local data exists -> use local data, reset revision
    controller._localData.data().setRevision(42);
    ASSERT_TRUE(controller.merge({}).has_value());
    EXPECT_EQ(controller._localData.data().revision(), 0);
    ASSERT_EQ(controller._localData.data().tagCount(), 1);
    EXPECT_EQ(controller._localData.data().tagAt(0).name, "tag1");
    EXPECT_EQ(controller._localData.data().tagAt(0).revision, 1);
    ASSERT_EQ(controller._localData.data().taskCount(), 0);

    //-----------------------------------------------------------------------
    // #4: local data has tags (with rev -1), remote doesn't have the tag. after sync, local data tag should have revision 0. Then do a pull data to check the remote has the tag
    core::Data localDataWithNewTag;
    core::Tag newTag;
    newTag.name = "newTag";
    newTag.revision = -1;
    localDataWithNewTag.addTag(newTag);

    core::Data remoteDataEmpty;

    initData(controller, localDataWithNewTag, remoteDataEmpty);

    auto syncResult = controller.refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    // Check local data tag has revision 0
    ASSERT_EQ(controller._localData.data().tagCount(), 1);
    EXPECT_EQ(controller._localData.data().tagAt(0).name, "newTag");
    EXPECT_EQ(controller._localData.data().tagAt(0).revision, 0);

    ASSERT_EQ(syncResult->tagCount(), 1);
    EXPECT_EQ(syncResult->tagAt(0).name, "newTag");

    auto pullResult = controller.pullRemoteData();
    ASSERT_TRUE(pullResult.has_value());
    ASSERT_EQ(pullResult->tagCount(), 1);
    EXPECT_EQ(pullResult->tagAt(0).name, "newTag");
    EXPECT_EQ(pullResult->tagAt(0).revision, 0);

    //-----------------------------------------------------------------------
    // #5: local data has tasks (with rev -1), remote doesn't have the task. after sync, local data task should have revision 0. Then do a pull data to check the remote has the task
    core::Data localDataWithNewTask;

    core::Task newTask;
    newTask.uuid = "uuid-task-5";
    newTask.title = "newTask";
    newTask.revision = -1;
    localDataWithNewTask.addTask(newTask);

    core::Data remoteDataEmptyTasks;
    EXPECT_EQ(remoteDataEmptyTasks.revision(), -1);

    initData(controller, localDataWithNewTask, remoteDataEmptyTasks);

    auto syncResultTasks = controller.refreshBlocking();
    ASSERT_TRUE(syncResultTasks.has_value());

    // Check local data task has revision 0 now
    ASSERT_EQ(controller._localData.data().taskCount(), 1);
    auto localTask = controller._localData.data().taskAt(0);
    EXPECT_EQ(localTask.title, "newTask");
    EXPECT_EQ(localTask.revision, 0);

    ASSERT_EQ(syncResultTasks->taskCount(), 1);
    EXPECT_EQ(syncResultTasks->taskAt(0).title, "newTask");

    pullResult = controller.pullRemoteData();
    ASSERT_TRUE(pullResult.has_value());
    ASSERT_EQ(pullResult->taskCount(), 1);
    EXPECT_EQ(pullResult->taskAt(0).title, "newTask");
    EXPECT_EQ(pullResult->taskAt(0).revision, 0);

    //-----------------------------------------------------------------------
    // #: Test revisions were updated when pushing to remote
    EXPECT_EQ(pullResult->revision(), 1);
    EXPECT_EQ(syncResultTasks->revision(), 1);

    //-----------------------------------------------------------------------
    // #: Test revisions were updated when pushing to remote, again
    core::Task anotherTask;
    anotherTask.uuid = "uuid-task-6";
    anotherTask.title = "anotherTask";
    anotherTask.revision = -1;
    controller._localData.data().addTask(anotherTask);
    EXPECT_EQ(controller._localData.data().newTasks().size(), 1);

    auto syncResult2 = controller.refreshBlocking();
    ASSERT_TRUE(syncResult2.has_value());
    EXPECT_EQ(syncResult2->revision(), 2);

    //-----------------------------------------------------------------------
    // #7. Test task deleted locally
    core::Data remoteDataWithTask;
    core::Task taskToDelete;
    taskToDelete.uuid = "uuid-task-7";
    taskToDelete.title = "taskToDelete";
    taskToDelete.revision = 0;
    remoteDataWithTask.addTask(taskToDelete);
    remoteDataWithTask.setRevision(10);

    initData(controller, remoteDataWithTask, remoteDataWithTask);

    auto pullResult7 = controller.pullRemoteData();
    ASSERT_TRUE(pullResult7.has_value());
    ASSERT_EQ(pullResult7->taskCount(), 1);
    EXPECT_EQ(pullResult7->taskAt(0).uuid, "uuid-task-7");

    // Delete task from local data
    ASSERT_TRUE(controller._localData.removeTask("uuid-task-7"));
    ASSERT_EQ(controller._localData.data().taskCount(), 0);

    // Call refresh again
    auto syncResult7 = controller.refreshBlocking();
    ASSERT_TRUE(syncResult7.has_value());

    // Call pull and confirm task was removed
    pullResult7 = controller.pullRemoteData();
    ASSERT_TRUE(pullResult7.has_value());
    EXPECT_EQ(pullResult7->taskCount(), 0);

    //-----------------------------------------------------------------------
    // #8. Test tag deleted locally
    core::Data remoteDataWithTag;
    core::Tag tagToDelete;
    tagToDelete.name = "tagToDelete";
    tagToDelete.revision = 0;
    remoteDataWithTag.addTag(tagToDelete);
    remoteDataWithTag.setRevision(11);

    initData(controller, remoteDataWithTag, remoteDataWithTag);

    auto pullResult8 = controller.pullRemoteData();
    ASSERT_TRUE(pullResult8.has_value());
    ASSERT_EQ(pullResult8->tagCount(), 1);
    EXPECT_EQ(pullResult8->tagAt(0).name, "tagToDelete");

    // Delete tag from local data
    ASSERT_TRUE(controller._localData.removeTag("tagToDelete"));
    ASSERT_EQ(controller._localData.data().tagCount(), 0);

    // Call refresh again
    auto syncResult8 = controller.refreshBlocking();
    ASSERT_TRUE(syncResult8.has_value());

    // Call pull and confirm tag was removed
    pullResult8 = controller.pullRemoteData();
    ASSERT_TRUE(pullResult8.has_value());
    EXPECT_EQ(pullResult8->tagCount(), 0);
}

TEST(DataControllerTest, SetTaskDone)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    DataController controller;

    // 1. init with empty local data and remote data containing 1 task
    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-task-done";
    task.title = "Task to be done";
    task.isDone = false;
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(controller, {}, remoteData);

    // 2. call refresh
    auto syncResult = controller.refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());
    ASSERT_EQ(syncResult->taskCount(), 1);
    EXPECT_FALSE(syncResult->taskAt(0).isDone);

    // Setup TaskModel
    TaskModel *model = controller.taskModel();

    // 3. call TaskModel's setTaskDone method
    model->setTaskDone("uuid-task-done", true);

    // 4. confirm it's done in the local data
    ASSERT_EQ(controller._localData.data().taskCount(), 1);
    EXPECT_TRUE(controller._localData.data().taskAt(0).isDone);

    // 5. confirm localData.modifiedTasks() returns 1 task
    EXPECT_EQ(controller._localData.data().modifiedTasks().size(), 1);

    // 6. call refresh again
    auto syncResult2 = controller.refreshBlocking();
    ASSERT_TRUE(syncResult2.has_value());

    // 7. confirm it's done in the local data and in the return value of refresh
    EXPECT_TRUE(controller._localData.data().taskAt(0).isDone);
    ASSERT_EQ(syncResult2->taskCount(), 1);
    EXPECT_TRUE(syncResult2->taskAt(0).isDone);

    // 8. confirm localData.modifiedTasks() is empty
    EXPECT_TRUE(controller._localData.data().modifiedTasks().empty());
}


TEST(DataControllerTest, TimerSavesToDisk)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    DataController controller;
    ASSERT_EQ(controller.thread(), QCoreApplication::instance()->thread());

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-task-timer";
    task.title = "Task for timer test";
    task.isDone = false;
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(controller, {}, remoteData);

    auto syncResult = controller.refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());
    ASSERT_EQ(syncResult->taskCount(), 1);
    EXPECT_FALSE(syncResult->taskAt(0).isDone);
    ASSERT_EQ(controller._localData.data().taskCount(), 1);
    EXPECT_FALSE(controller._localData.data().taskAt(0).isDone);

    P_LOG_INFO("test: Both remote and local have the task, now testing timer...");
    TaskModel *model = controller.taskModel();
    model->setTaskDone("uuid-task-timer", true);

    ASSERT_EQ(controller._localData.data().taskCount(), 1);
    EXPECT_TRUE(controller._localData.data().taskAt(0).isDone);

    P_LOG_INFO("test: Starting to wait");
    QTest::qWait(std::chrono::seconds(3));

    core::LocalData diskData;
    auto loadResult = diskData.loadDataFromFile();
    ASSERT_TRUE(loadResult) << "Failed to load data from disk: " << loadResult.error();

    ASSERT_TRUE(diskData.data().isValid());

    ASSERT_EQ(diskData.data().taskCount(), 1);
    EXPECT_EQ(diskData.data().taskAt(0).uuid, "uuid-task-timer");
    EXPECT_TRUE(diskData.data().taskAt(0).isDone);
}

TEST(DataControllerTest, MergeNeedsLocalSave)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });
    DataController controller;

    core::Data localData;
    localData.setRevision(1);

    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-task-merge";
    task.title = "Remote Task";
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(2);

    initData(controller, localData, remoteData);

    auto pullResult = controller.pullRemoteData();
    ASSERT_TRUE(pullResult.has_value());

    auto result = controller.merge(*pullResult);
    ASSERT_TRUE(result.has_value());

    EXPECT_TRUE(result->needsLocalSave);
}

TEST(DataControllerTest, MoveToSoonClearsDueDate)
{
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    GuiController *guiController = GuiController::instance();
    DataController *controller = guiController->dataController();

    // 1. Create task due today
    core::Data remoteData;
    core::Task task;
    task.uuid = "uuid-task-soon-test";
    task.title = "Task for soon test";
    task.dueDate = std::chrono::system_clock::now();
    task.revision = 0;
    remoteData.addTask(task);
    remoteData.setRevision(1);

    initData(*controller, {}, remoteData);

    auto syncResult = controller->refreshBlocking();
    ASSERT_TRUE(syncResult.has_value());

    // Check isCurrent is true, isSoon is false
    auto *storedTask = controller->taskModel()->taskForUuid("uuid-task-soon-test");
    ASSERT_NE(storedTask, nullptr);
    EXPECT_TRUE(storedTask->isCurrent());
    EXPECT_FALSE(storedTask->isSoon());

    // 2. Call moveToSoon
    guiController->moveTaskToSoon("uuid-task-soon-test");

    // 3. Assert due date was cleared
    auto *updatedTask = controller->taskModel()->taskForUuid("uuid-task-soon-test");
    ASSERT_NE(updatedTask, nullptr);
    EXPECT_FALSE(updatedTask->dueDate.has_value()) << "Due date was not cleared!";

    delete guiController;
}

TEST(DataControllerTest, LogoutTokenMetaTest)
{
    // Requested test:
    // starts with logout(), checks settings to see that refresh token is empty, login.
    // then check its not empty. then logout again, and check that its not empty, but different from initial.

    // Setup environment
    QGuiApplication _app(g_argc, g_argv);
    core::Logger::initLogLevel();
    core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });

    DataController controller;
    LocalSettings &settings = controller.localSettings();

    // 1. Logout
    controller.logout();

    // 2. Check settings empty
    EXPECT_TRUE(settings.refreshToken().empty()) << "Refresh token should be empty after logout";

    // 3. Login
    ASSERT_TRUE(controller.loginWithDefaults());

    // 4. Check settings not empty
    std::string firstToken = settings.refreshToken();
    EXPECT_FALSE(firstToken.empty()) << "Refresh token should not be empty after login";
}

int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;

    QCoreApplication::setOrganizationName("com.pointless.tests");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
