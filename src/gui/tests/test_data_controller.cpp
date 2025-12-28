// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/data_controller.h"
#include "core/context.h"
#include "core/data_provider.h"

#include <gtest/gtest.h>

#include <filesystem>

using namespace pointless;

constexpr const char *s_filename = "/tmp/pointless_test_data_controller_sync.json";

/// Sets the data locally and remotely before we start the test
void initData(DataController &controller, std::optional<core::Data> localData, const core::Data &remoteData)

{
    ASSERT_TRUE(controller.loginWithDefaults());

    std::filesystem::remove(s_filename);
    if (localData.has_value()) {
        auto saveResult = controller._localData.setDataAndSave(*localData);
        ASSERT_TRUE(saveResult) << "Failed to save local data: " << saveResult.error();
    }

    ASSERT_TRUE(controller.pushRemoteData(remoteData).has_value());
}

TEST(DataControllerTest, ConstructDestroy)
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, "/tmp/nonexistent_file.json" });
    DataController controller;
}

// DataController::sync() tests
TEST(DataControllerTest, Sync)
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });
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
    ASSERT_TRUE(controller.refresh().has_value());

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

    auto syncResult = controller.refresh();
    ASSERT_TRUE(syncResult.has_value());

    // Check local data tag has revision 0
    ASSERT_EQ(controller._localData.data().tagCount(), 1);
    EXPECT_EQ(controller._localData.data().tagAt(0).name, "newTag");
    EXPECT_EQ(controller._localData.data().tagAt(0).revision, 0);

    ASSERT_EQ(syncResult->tagCount(), 1);
    EXPECT_EQ(syncResult->tagAt(0).name, "newTag");

    //-----------------------------------------------------------------------
    // #5: local data has tasks (with rev -1), remote doesn't have the task. after sync, local data task should have revision 0. Then do a pull data to check the remote has the task
    core::Data localDataWithNewTask;
    core::Task newTask;
    newTask.uuid = "uuid-task-5";
    newTask.title = "newTask";
    newTask.revision = -1;
    localDataWithNewTask.addTask(newTask);

    core::Data remoteDataEmptyTasks;

    initData(controller, localDataWithNewTask, remoteDataEmptyTasks);

    auto syncResultTasks = controller.refresh();
    ASSERT_TRUE(syncResultTasks.has_value());

    // Check local data task has revision 0 now
    ASSERT_EQ(controller._localData.data().taskCount(), 1);
    auto localTask = controller._localData.data().taskAt(0);
    EXPECT_EQ(localTask.title, "newTask");
    EXPECT_EQ(localTask.revision, 0);

    ASSERT_EQ(syncResultTasks->taskCount(), 1);
    EXPECT_EQ(syncResultTasks->taskAt(0).title, "newTask");
}
