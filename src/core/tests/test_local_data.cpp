// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"
#include "data.h"
#include "context.h"

#include <gtest/gtest.h>

#include <filesystem>

using namespace pointless::core;

TEST(LocalDataTest, LoadExistingFile)
{
    Context::setContext(Context(IDataProvider::Type::TestsLocal, "/tmp/pointless.json"));
    LocalData localData;
    std::filesystem::path testFile = std::filesystem::path(__FILE__).parent_path() / "test.json";
    auto result = localData.loadDataFromFile(testFile.string());
    ASSERT_TRUE(result.has_value());
    const Data &data = result.value();

    // Check against test.json content
    EXPECT_EQ(data.tagCount(), 3);
    EXPECT_GT(data.taskCount(), 0);
}

TEST(LocalDataTest, LoadNonExistentFile)
{
    Context::setContext(Context(IDataProvider::Type::TestsLocal, "/tmp/pointless.json"));
    LocalData localData;
    auto result = localData.loadDataFromFile("non_existent_file.json");
    ASSERT_TRUE(result.has_value());

    const Data &data = result.value();
    EXPECT_EQ(data.taskCount(), 0);
    EXPECT_EQ(data.tagCount(), 0);
}

TEST(LocalDataTest, LoadFromEnvVar)
{
    // Setup temporary directory
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "pointless_test_env";
    std::filesystem::create_directories(tempDir);

    // Copy test.json to tempDir/pointless.json
    std::filesystem::path sourceFile = std::filesystem::path(__FILE__).parent_path() / "test.json";
    std::filesystem::path destFile = tempDir / "pointless.json";
    std::filesystem::copy_file(sourceFile, destFile, std::filesystem::copy_options::overwrite_existing);

    // Set environment variable
    Context::setContext(Context(IDataProvider::Type::TestsLocal, tempDir / "pointless.json"));

    LocalData localData;
    auto result = localData.loadDataFromFile();
    ASSERT_TRUE(result.has_value());

    const Data &data = localData.data();
    EXPECT_EQ(data.tagCount(), 3);
    EXPECT_GT(data.taskCount(), 0);

    // Cleanup
    std::filesystem::remove_all(tempDir);
}

TEST(LocalDataTest, ClearServerSyncBits)
{
    Context::setContext(Context(IDataProvider::Type::TestsLocal, "/tmp/pointless.json"));
    LocalData localData;
    Data data;

    // Add a task with sync bits set
    Task task;
    task.uuid = "task1";
    task.revision = -1;
    task.needsSyncToServer = true;
    data.addTask(task);

    // Add a tag with sync bits set
    Tag tag;
    tag.name = "tag1";
    tag.revision = -1;
    tag.needsSyncToServer = true;
    data.addTag(tag);

    localData.setData(data);
    localData.data().addDeletedTagName("deletedTag1");
    localData.data().addDeletedTaskUuid("deletedTask1");

    localData.clearServerSyncBits();

    const Data &clearedData = localData.data();
    auto tasks = clearedData.getAllTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].revision, 0);
    EXPECT_FALSE(tasks[0].needsSyncToServer);

    auto tags = clearedData.allTags();
    ASSERT_EQ(tags.size(), 1);
    EXPECT_EQ(tags[0].revision, 0);
    EXPECT_FALSE(tags[0].needsSyncToServer);

    EXPECT_TRUE(localData.deletedTags().empty());
    EXPECT_TRUE(localData.deletedTasks().empty());
}

TEST(LocalDataTest, SaveData)
{
    // Setup temporary directory
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "pointless_test_save";
    std::filesystem::create_directories(tempDir);

    // Set environment variable
    Context::setContext(Context(IDataProvider::Type::TestsLocal, tempDir / "pointless.json"));

    LocalData localData;
    Data data;

    // Add a task
    Task task;
    task.uuid = "task_save_1";
    task.title = "Save Task";
    data.addTask(task);

    // Add a tag
    Tag tag;
    tag.name = "tag_save_1";
    data.addTag(tag);

    localData.setData(data);

    // Save
    auto result = localData.save();
    ASSERT_TRUE(result.has_value());

    // Verify file exists
    std::filesystem::path expectedFile = tempDir / "pointless.json";
    ASSERT_TRUE(std::filesystem::exists(expectedFile));

    // Load back to verify content
    LocalData localDataLoaded;
    auto loadResult = localDataLoaded.loadDataFromFile();
    ASSERT_TRUE(loadResult.has_value());

    const Data &loadedData = localDataLoaded.data();
    EXPECT_EQ(loadedData.taskCount(), 1);
    EXPECT_EQ(loadedData.tagCount(), 1);

    auto loadedTask = loadedData.getTask("task_save_1");
    ASSERT_TRUE(loadedTask.has_value());
    EXPECT_EQ(loadedTask->title, "Save Task");

    auto loadedTag = loadedData.getTag("tag_save_1");
    ASSERT_TRUE(loadedTag.has_value());

    // Cleanup
    std::filesystem::remove_all(tempDir);
}
