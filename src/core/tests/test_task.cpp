
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task.h"
#include "Clock.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>
#include <chrono>

using namespace pointless::core;

TEST(TaskTest, SerializeJson)
{
    Task original_task;
    original_task.revision = 123;
    original_task.needsSyncToServer = true;
    original_task.uuid = "test-uuid-12345";
    original_task.parentUuid = "parent-uuid-67890";
    original_task.title = "Test Task Title";
    original_task.isDone = false;
    original_task.isImportant = true;
    original_task.hideOnWeekends = false;
    original_task.creationTimestamp = pointless::core::Clock::now();
    original_task.modificationTimestamp = original_task.creationTimestamp + std::chrono::hours(1);
    original_task.dueDate = original_task.creationTimestamp + std::chrono::hours(24 * 7);
    original_task.uuidInDeviceCalendar = "calendar-uuid-abc";
    original_task.deviceCalendarUuid = "device-calendar-xyz";
    original_task.deviceCalendarName = "Work Calendar";

    original_task.tags = { "work", "urgent" };

    auto json_result = glz::write_json(original_task);
    ASSERT_TRUE(json_result.has_value());

    std::string json_str = json_result.value();
    EXPECT_FALSE(json_str.empty());

    EXPECT_TRUE(json_str.find("\"uuid\":\"test-uuid-12345\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"title\":\"Test Task Title\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"revision\":123") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"creationTimestamp\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"modificationTimestamp\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"dueDate\"") != std::string::npos);
}

TEST(TaskTest, DeserializeJsonReadsTags)
{
    std::string json = R"({
        "revision": 123,
        "uuid": "test-uuid-12345",
        "parentUuid": "parent-uuid-67890",
        "title": "Test Task Title",
        "isDone": false,
        "isImportant": true,
        "hideOnWeekends": false,
        "creationTimestamp": 1700000000,
        "modificationTimestamp": 1700003600,
        "dueDate": 1700604800,
        "uuidInDeviceCalendar": "calendar-uuid-abc",
        "deviceCalendarUuid": "device-calendar-xyz",
        "deviceCalendarName": "Work Calendar",
        "tags": ["work", "urgent"]
    })";

    Task task;
    auto result = glz::read_json(task, json);
    ASSERT_TRUE(result == glz::error_code::none);
    ASSERT_EQ(task.tags.size(), 2);
    EXPECT_EQ(task.tags[0], "work");
    EXPECT_EQ(task.tags[1], "urgent");
}

TEST(TaskTest, IsDueIn)
{
    pointless::core::Task task;
    // No due date
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));

    // Due in 2 days
    task.dueDate = pointless::core::Clock::now() + std::chrono::hours(48);
    EXPECT_TRUE(task.isDueIn(std::chrono::days(3)));
    EXPECT_TRUE(task.isDueIn(std::chrono::days(2)));
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));

    // Due in the past
    task.dueDate = pointless::core::Clock::now() - std::chrono::hours(24);
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));
}

TEST(TaskTest, IsDueThisWeek)
{
    pointless::core::Task task;

    auto monday = std::chrono::sys_days(std::chrono::January / 12 / 2026);
    auto saturday = std::chrono::sys_days(std::chrono::January / 17 / 2026);
    auto sunday = std::chrono::sys_days(std::chrono::January / 18 / 2026);
    auto nextMonday = std::chrono::sys_days(std::chrono::January / 19 / 2026);

    Clock::setTestNow(monday);

    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = monday + std::chrono::hours(72);
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = sunday;
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = nextMonday;
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = monday - std::chrono::hours(24);
    EXPECT_FALSE(task.isDueThisWeek());

    Clock::setTestNow(saturday);
    task.dueDate = std::nullopt;
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = saturday + std::chrono::hours(12);
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = saturday + std::chrono::days(1);
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = saturday + std::chrono::days(2);
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = sunday;
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = nextMonday;
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = saturday - std::chrono::hours(48);
    EXPECT_TRUE(task.isDueThisWeek());

    Clock::setTestNow(sunday);
    task.dueDate = std::nullopt;
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = sunday + std::chrono::hours(12);
    EXPECT_TRUE(task.isDueThisWeek());

    task.dueDate = nextMonday;
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = nextMonday + std::chrono::days(1);
    EXPECT_FALSE(task.isDueThisWeek());

    task.dueDate = monday;
    EXPECT_TRUE(task.isDueThisWeek());

    Clock::reset();
}

TEST(TaskTest, DueDatePreservedOnSerializeDeserialize)
{
    using namespace std::chrono;
    pointless::core::Task original;
    original.dueDate = system_clock::now() + hours(24 * 5);
    auto json_result = glz::write_json(original);
    ASSERT_TRUE(json_result.has_value());
    std::string json_str = json_result.value();

    EXPECT_TRUE(json_str.find("\"dueDate\"") != std::string::npos);
    // std::cout << json_str << std::endl;

    pointless::core::Task deserialized;
    auto result = glz::read_json(deserialized, json_str);
    ASSERT_TRUE(result == glz::error_code::none);
    ASSERT_TRUE(deserialized.dueDate.has_value());
    // Allow a small difference due to possible truncation/rounding
    auto diff = duration_cast<seconds>(*original.dueDate - *deserialized.dueDate);
    EXPECT_LT(std::abs(diff.count()), 2); // less than 2 seconds difference
}

TEST(TaskTest, CalendarPropertiesSerializeDeserialize)
{
    Task original;
    original.uuidInDeviceCalendar = "calendar-uuid-abc";
    original.deviceCalendarUuid = "device-calendar-xyz";
    original.deviceCalendarName = "Work Calendar";

    auto json_result = glz::write_json(original);
    ASSERT_TRUE(json_result.has_value());
    std::string json_str = json_result.value();

    EXPECT_TRUE(json_str.find("\"uuidInDeviceCalendar\":\"calendar-uuid-abc\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"deviceCalendarUuid\":\"device-calendar-xyz\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"deviceCalendarName\":\"Work Calendar\"") != std::string::npos);

    Task deserialized;
    auto result = glz::read_json(deserialized, json_str);
    ASSERT_TRUE(result == glz::error_code::none);
    EXPECT_EQ(deserialized.uuidInDeviceCalendar, "calendar-uuid-abc");
    EXPECT_EQ(deserialized.deviceCalendarUuid, "device-calendar-xyz");
    EXPECT_EQ(deserialized.deviceCalendarName, "Work Calendar");
}

TEST(TaskTest, MergeConflict_IsDone_UndoneWins)
{
    Task task1;
    task1.isDone = true;
    Task task2;
    task2.isDone = false;

    task1.mergeConflict(task2);
    EXPECT_FALSE(task1.isDone);

    Task task3;
    task3.isDone = false;
    Task task4;
    task4.isDone = true;

    task3.mergeConflict(task4);
    EXPECT_FALSE(task3.isDone);

    Task task5;
    task5.isDone = true;
    Task task6;
    task6.isDone = true;

    task5.mergeConflict(task6);
    EXPECT_TRUE(task5.isDone);

    Task task7;
    task7.isDone = false;
    Task task8;
    task8.isDone = false;

    task7.mergeConflict(task8);
    EXPECT_FALSE(task7.isDone);
}

TEST(TaskTest, MergeConflict_IsImportant_ImportantWins)
{
    Task task1;
    task1.isImportant = false;
    Task task2;
    task2.isImportant = true;

    task1.mergeConflict(task2);
    EXPECT_TRUE(task1.isImportant);

    Task task3;
    task3.isImportant = true;
    Task task4;
    task4.isImportant = false;

    task3.mergeConflict(task4);
    EXPECT_TRUE(task3.isImportant);

    Task task5;
    task5.isImportant = true;
    Task task6;
    task6.isImportant = true;

    task5.mergeConflict(task6);
    EXPECT_TRUE(task5.isImportant);

    Task task7;
    task7.isImportant = false;
    Task task8;
    task8.isImportant = false;

    task7.mergeConflict(task8);
    EXPECT_FALSE(task7.isImportant);
}

TEST(TaskTest, MergeConflict_DueDate_BothHaveDueDate_MoreRecentWins)
{
    auto now = pointless::core::Clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    auto twoHoursAgo = now - std::chrono::hours(2);

    Task task1;
    task1.dueDate = now + std::chrono::days(5);
    task1.modificationTimestamp = twoHoursAgo;
    Task task2;
    task2.dueDate = now + std::chrono::days(10);
    task2.modificationTimestamp = oneHourAgo;

    task1.mergeConflict(task2);
    EXPECT_EQ(*task1.dueDate, now + std::chrono::days(10));

    Task task3;
    task3.dueDate = now + std::chrono::days(5);
    task3.modificationTimestamp = oneHourAgo;
    Task task4;
    task4.dueDate = now + std::chrono::days(10);
    task4.modificationTimestamp = twoHoursAgo;

    task3.mergeConflict(task4);
    EXPECT_EQ(*task3.dueDate, now + std::chrono::days(5));
}

TEST(TaskTest, MergeConflict_DueDate_OnlyOtherHasDueDate)
{
    auto now = pointless::core::Clock::now();

    Task task1;
    task1.dueDate = std::nullopt;
    Task task2;
    task2.dueDate = now + std::chrono::days(7);

    task1.mergeConflict(task2);
    ASSERT_TRUE(task1.dueDate.has_value());
    EXPECT_EQ(*task1.dueDate, now + std::chrono::days(7));
}

TEST(TaskTest, MergeConflict_DueDate_OnlyThisHasDueDate)
{
    auto now = pointless::core::Clock::now();

    Task task1;
    task1.dueDate = now + std::chrono::days(7);
    Task task2;
    task2.dueDate = std::nullopt;

    task1.mergeConflict(task2);
    ASSERT_TRUE(task1.dueDate.has_value());
    EXPECT_EQ(*task1.dueDate, now + std::chrono::days(7));
}

TEST(TaskTest, MergeConflict_Title_MoreRecentWins)
{
    auto now = pointless::core::Clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    auto twoHoursAgo = now - std::chrono::hours(2);

    Task task1;
    task1.title = "Old Title";
    task1.modificationTimestamp = twoHoursAgo;
    Task task2;
    task2.title = "New Title";
    task2.modificationTimestamp = oneHourAgo;

    task1.mergeConflict(task2);
    EXPECT_EQ(task1.title, "New Title");

    Task task3;
    task3.title = "Newer Title";
    task3.modificationTimestamp = oneHourAgo;
    Task task4;
    task4.title = "Older Title";
    task4.modificationTimestamp = twoHoursAgo;

    task3.mergeConflict(task4);
    EXPECT_EQ(task3.title, "Newer Title");
}

TEST(TaskTest, MergeConflict_Title_SameTitle_NoChange)
{
    auto now = pointless::core::Clock::now();

    Task task1;
    task1.title = "Same Title";
    task1.modificationTimestamp = now - std::chrono::hours(2);
    Task task2;
    task2.title = "Same Title";
    task2.modificationTimestamp = now - std::chrono::hours(1);

    task1.mergeConflict(task2);
    EXPECT_EQ(task1.title, "Same Title");
}

TEST(TaskTest, MergeConflict_Tags_Union)
{
    Task task1;
    task1.tags = { "tag1", "tag2" };
    Task task2;
    task2.tags = { "tag2", "tag3" };

    task1.mergeConflict(task2);
    ASSERT_EQ(task1.tags.size(), 3);
    EXPECT_TRUE(task1.containsTag("tag1"));
    EXPECT_TRUE(task1.containsTag("tag2"));
    EXPECT_TRUE(task1.containsTag("tag3"));
}

TEST(TaskTest, MergeConflict_Tags_NoDuplicates)
{
    Task task1;
    task1.tags = { "tag1", "tag2" };
    Task task2;
    task2.tags = { "tag1", "tag2" };

    task1.mergeConflict(task2);
    EXPECT_EQ(task1.tags.size(), 2);
}

TEST(TaskTest, MergeConflict_CurrentWinsOverSoon)
{
    Task task1;
    task1.tags = { "current" };
    Task task2;
    task2.tags = { "soon" };

    task1.mergeConflict(task2);
    EXPECT_TRUE(task1.containsTag("current"));
    EXPECT_FALSE(task1.containsTag("soon"));
}

TEST(TaskTest, MergeConflict_CurrentWinsOverSoon_BothOnThis)
{
    Task task1;
    task1.tags = { "current", "soon" };
    Task task2;
    task2.tags = {};

    task1.mergeConflict(task2);
    EXPECT_TRUE(task1.containsTag("current"));
    EXPECT_FALSE(task1.containsTag("soon"));
}

TEST(TaskTest, MergeConflict_CurrentWinsOverSoon_BothOnOther)
{
    Task task1;
    task1.tags = {};
    Task task2;
    task2.tags = { "current", "soon" };

    task1.mergeConflict(task2);
    EXPECT_TRUE(task1.containsTag("current"));
    EXPECT_FALSE(task1.containsTag("soon"));
}

TEST(TaskTest, IsDueTomorrow)
{
    pointless::core::Task task;
    auto now = std::chrono::sys_days { std::chrono::January / 17 / 2026 };
    pointless::core::Clock::setTestNow(now);

    // No due date
    EXPECT_FALSE(task.isDueTomorrow());

    // Due today
    task.dueDate = now;
    EXPECT_FALSE(task.isDueTomorrow());

    // Due tomorrow
    task.dueDate = now + std::chrono::days(1);
    EXPECT_TRUE(task.isDueTomorrow());

    // Due day after tomorrow
    task.dueDate = now + std::chrono::days(2);
    EXPECT_FALSE(task.isDueTomorrow());

    // Due yesterday
    task.dueDate = now - std::chrono::days(1);
    EXPECT_FALSE(task.isDueTomorrow());

    pointless::core::Clock::reset();
}
