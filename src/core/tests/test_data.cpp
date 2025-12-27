// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>

using namespace pointless::core;

TEST(DataTest, SerializeDeserialize)
{
    Data originalData;
    originalData.setRevision(42);

    Task task;
    task.uuid = "task-1";
    task.title = "Task 1";
    originalData.addTask(task);

    Tag tag;
    tag.name = "tag-1";
    originalData.addTag(tag);

    originalData.addDeletedTaskUuid("deleted-task-1");
    originalData.addDeletedTaskUuid("deleted-task-2");
    originalData.addDeletedTagName("deleted-tag-1");

    auto jsonResult = originalData.toJson();
    ASSERT_TRUE(jsonResult.has_value());
    std::string jsonStr = jsonResult.value();

    // Basic check of JSON string
    EXPECT_NE(jsonStr.find("\"revision\":42"), std::string::npos);
    EXPECT_NE(jsonStr.find("deleted-task-1"), std::string::npos);
    EXPECT_NE(jsonStr.find("deleted-tag-1"), std::string::npos);

    // Deserialize
    auto deserializedResult = Data::fromJson(jsonStr);
    ASSERT_TRUE(deserializedResult.has_value());
    Data deserializedData = deserializedResult.value();

    EXPECT_EQ(deserializedData.revision(), 42);
    EXPECT_EQ(deserializedData.taskCount(), 1);
    EXPECT_EQ(deserializedData.tagCount(), 1);

    const auto &deletedTasks = deserializedData.deletedTaskUuids();
    ASSERT_EQ(deletedTasks.size(), 2);
    EXPECT_EQ(deletedTasks[0], "deleted-task-1");
    EXPECT_EQ(deletedTasks[1], "deleted-task-2");

    const auto &deletedTags = deserializedData.deletedTagNames();
    ASSERT_EQ(deletedTags.size(), 1);
    EXPECT_EQ(deletedTags[0], "deleted-tag-1");
}

TEST(DataTest, SerializeEmptyDeletedLists)
{
    Data originalData;
    auto jsonResult = originalData.toJson();
    ASSERT_TRUE(jsonResult.has_value());
    std::string jsonStr = jsonResult.value();

    auto deserializedResult = Data::fromJson(jsonStr);
    ASSERT_TRUE(deserializedResult.has_value());
    Data deserializedData = deserializedResult.value();

    EXPECT_TRUE(deserializedData.deletedTaskUuids().empty());
    EXPECT_TRUE(deserializedData.deletedTagNames().empty());
}
