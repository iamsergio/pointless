// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>
#include <algorithm>

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

TEST(DataTest, FindDuplicateCalendarTaskUuids_NoDuplicates)
{
    Data data;

    Task t1;
    t1.uuid = "uuid-1";
    t1.title = "Meeting A";
    t1.uuidInDeviceCalendar = "cal-event-1";
    t1.deviceCalendarName = "Work";
    data.addTask(t1);

    Task t2;
    t2.uuid = "uuid-2";
    t2.title = "Meeting B";
    t2.uuidInDeviceCalendar = "cal-event-2";
    t2.deviceCalendarName = "Work";
    data.addTask(t2);

    auto dupes = data.findDuplicateCalendarTaskUuids();
    EXPECT_TRUE(dupes.empty());
}

TEST(DataTest, FindDuplicateCalendarTaskUuids_ReturnsOlderTask)
{
    Data data;
    auto older = std::chrono::system_clock::time_point(std::chrono::milliseconds(1000));
    auto newer = std::chrono::system_clock::time_point(std::chrono::milliseconds(2000));
    auto eventDate = std::chrono::system_clock::time_point(std::chrono::hours(24 * 100));

    Task t1;
    t1.uuid = "uuid-1";
    t1.title = "Meeting A";
    t1.uuidInDeviceCalendar = "ios-event-1";
    t1.deviceCalendarName = "Work";
    t1.dueDate = eventDate;
    t1.modificationTimestamp = older;
    data.addTask(t1);

    Task t2;
    t2.uuid = "uuid-2";
    t2.title = "Meeting A";
    t2.uuidInDeviceCalendar = "linux-event-1";
    t2.deviceCalendarName = "Work";
    t2.dueDate = eventDate;
    t2.modificationTimestamp = newer;
    data.addTask(t2);

    auto dupes = data.findDuplicateCalendarTaskUuids();
    ASSERT_EQ(dupes.size(), 1);
    EXPECT_EQ(dupes[0], "uuid-1");
}

TEST(DataTest, FindDuplicateCalendarTaskUuids_MissingModificationTimestamp)
{
    Data data;
    auto someTime = std::chrono::system_clock::time_point(std::chrono::milliseconds(5000));
    auto eventDate = std::chrono::system_clock::time_point(std::chrono::hours(24 * 100));

    Task t1;
    t1.uuid = "uuid-1";
    t1.title = "Meeting A";
    t1.uuidInDeviceCalendar = "ios-event-1";
    t1.deviceCalendarName = "Work";
    t1.dueDate = eventDate;
    data.addTask(t1);

    Task t2;
    t2.uuid = "uuid-2";
    t2.title = "Meeting A";
    t2.uuidInDeviceCalendar = "linux-event-1";
    t2.deviceCalendarName = "Work";
    t2.dueDate = eventDate;
    t2.modificationTimestamp = someTime;
    data.addTask(t2);

    auto dupes = data.findDuplicateCalendarTaskUuids();
    ASSERT_EQ(dupes.size(), 1);
    EXPECT_EQ(dupes[0], "uuid-1");
}

TEST(DataTest, FindDuplicateCalendarTaskUuids_ThreeDuplicates)
{
    Data data;
    auto t1Time = std::chrono::system_clock::time_point(std::chrono::milliseconds(1000));
    auto t2Time = std::chrono::system_clock::time_point(std::chrono::milliseconds(3000));
    auto t3Time = std::chrono::system_clock::time_point(std::chrono::milliseconds(2000));
    auto eventDate = std::chrono::system_clock::time_point(std::chrono::hours(24 * 100));

    Task t1;
    t1.uuid = "uuid-1";
    t1.title = "Meeting A";
    t1.uuidInDeviceCalendar = "event-1";
    t1.deviceCalendarName = "Work";
    t1.dueDate = eventDate;
    t1.modificationTimestamp = t1Time;
    data.addTask(t1);

    Task t2;
    t2.uuid = "uuid-2";
    t2.title = "Meeting A";
    t2.uuidInDeviceCalendar = "event-2";
    t2.deviceCalendarName = "Work";
    t2.dueDate = eventDate;
    t2.modificationTimestamp = t2Time;
    data.addTask(t2);

    Task t3;
    t3.uuid = "uuid-3";
    t3.title = "Meeting A";
    t3.uuidInDeviceCalendar = "event-3";
    t3.deviceCalendarName = "Work";
    t3.dueDate = eventDate;
    t3.modificationTimestamp = t3Time;
    data.addTask(t3);

    auto dupes = data.findDuplicateCalendarTaskUuids();
    ASSERT_EQ(dupes.size(), 2);
    EXPECT_NE(std::ranges::find(dupes, "uuid-1"), dupes.end());
    EXPECT_NE(std::ranges::find(dupes, "uuid-3"), dupes.end());
    EXPECT_EQ(std::ranges::find(dupes, "uuid-2"), dupes.end());
}

TEST(DataTest, FindDuplicateCalendarTaskUuids_RecurringEventsNotDeduplicated)
{
    Data data;
    auto monday = std::chrono::system_clock::time_point(std::chrono::hours(24 * 100));
    auto tuesday = std::chrono::system_clock::time_point(std::chrono::hours(24 * 101));

    Task t1;
    t1.uuid = "uuid-1";
    t1.title = "Standup";
    t1.uuidInDeviceCalendar = "event-1";
    t1.deviceCalendarName = "Work";
    t1.dueDate = monday;
    data.addTask(t1);

    Task t2;
    t2.uuid = "uuid-2";
    t2.title = "Standup";
    t2.uuidInDeviceCalendar = "event-2";
    t2.deviceCalendarName = "Work";
    t2.dueDate = tuesday;
    data.addTask(t2);

    auto dupes = data.findDuplicateCalendarTaskUuids();
    EXPECT_TRUE(dupes.empty());
}

TEST(DataTest, RenameTag_Success)
{
    Data data;
    Tag tag;
    tag.name = "old-tag";
    data.addTag(tag);

    EXPECT_TRUE(data.renameTag("old-tag", "new-tag"));
    EXPECT_FALSE(data.containsTag("old-tag"));
    EXPECT_TRUE(data.containsTag("new-tag"));
}

TEST(DataTest, RenameTag_UpdatesTaskTags)
{
    Data data;
    Tag tag;
    tag.name = "old-tag";
    data.addTag(tag);

    Task t1;
    t1.uuid = "task-1";
    t1.title = "Task 1";
    t1.tags = {"old-tag", "other-tag"};
    data.addTask(t1);

    Task t2;
    t2.uuid = "task-2";
    t2.title = "Task 2";
    t2.tags = {"other-tag"};
    data.addTask(t2);

    EXPECT_TRUE(data.renameTag("old-tag", "new-tag"));

    auto task1 = data.getTask("task-1");
    ASSERT_TRUE(task1.has_value());
    EXPECT_NE(std::ranges::find(task1->tags, "new-tag"), task1->tags.end());
    EXPECT_EQ(std::ranges::find(task1->tags, "old-tag"), task1->tags.end());
    EXPECT_NE(std::ranges::find(task1->tags, "other-tag"), task1->tags.end());

    auto task2 = data.getTask("task-2");
    ASSERT_TRUE(task2.has_value());
    EXPECT_EQ(std::ranges::find(task2->tags, "new-tag"), task2->tags.end());
}

TEST(DataTest, RenameTag_OldNameNotFound)
{
    Data data;
    Tag tag;
    tag.name = "existing";
    data.addTag(tag);

    EXPECT_FALSE(data.renameTag("nonexistent", "new-name"));
    EXPECT_TRUE(data.containsTag("existing"));
}

TEST(DataTest, RenameTag_NewNameAlreadyExists)
{
    Data data;
    Tag t1;
    t1.name = "tag-a";
    data.addTag(t1);
    Tag t2;
    t2.name = "tag-b";
    data.addTag(t2);

    EXPECT_FALSE(data.renameTag("tag-a", "tag-b"));
    EXPECT_TRUE(data.containsTag("tag-a"));
    EXPECT_TRUE(data.containsTag("tag-b"));
}

TEST(DataTest, RenameTag_SameName)
{
    Data data;
    Tag tag;
    tag.name = "same";
    data.addTag(tag);

    EXPECT_FALSE(data.renameTag("same", "same"));
}

TEST(DataTest, RenameTag_EmptyNewName)
{
    Data data;
    Tag tag;
    tag.name = "tag";
    data.addTag(tag);

    EXPECT_FALSE(data.renameTag("tag", ""));
    EXPECT_TRUE(data.containsTag("tag"));
}

TEST(DataTest, RenameTag_MarksNeedsSyncToServer)
{
    Data data;
    Tag tag;
    tag.name = "old-tag";
    tag.needsSyncToServer = false;
    data.addTag(tag);

    Task task;
    task.uuid = "task-1";
    task.title = "Task 1";
    task.tags = {"old-tag"};
    task.needsSyncToServer = false;
    data.addTask(task);

    Task unrelated;
    unrelated.uuid = "task-2";
    unrelated.title = "Task 2";
    unrelated.tags = {"other"};
    unrelated.needsSyncToServer = false;
    data.addTask(unrelated);

    EXPECT_TRUE(data.renameTag("old-tag", "new-tag"));

    auto renamedTag = data.getTag("new-tag");
    ASSERT_TRUE(renamedTag.has_value());
    EXPECT_TRUE(renamedTag->needsSyncToServer);

    auto affectedTask = data.getTask("task-1");
    ASSERT_TRUE(affectedTask.has_value());
    EXPECT_TRUE(affectedTask->needsSyncToServer);

    auto unrelatedTask = data.getTask("task-2");
    ASSERT_TRUE(unrelatedTask.has_value());
    EXPECT_FALSE(unrelatedTask->needsSyncToServer);
}

TEST(DataTest, RenameTag_TracksDeletedOldName)
{
    Data data;
    Tag tag;
    tag.name = "old-tag";
    data.addTag(tag);

    EXPECT_TRUE(data.renameTag("old-tag", "new-tag"));

    const auto &deleted = data.deletedTagNames();
    ASSERT_EQ(deleted.size(), 1);
    EXPECT_EQ(deleted[0], "old-tag");
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
