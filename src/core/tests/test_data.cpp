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
