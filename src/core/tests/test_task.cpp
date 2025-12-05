
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task.h"
#include "Clock.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>
#include <chrono>

using namespace PointlessCore;

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
    original_task.creationTimestamp = PointlessCore::Clock::now();
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
    PointlessCore::Task task;
    // No due date
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));

    // Due in 2 days
    task.dueDate = PointlessCore::Clock::now() + std::chrono::hours(48);
    EXPECT_TRUE(task.isDueIn(std::chrono::days(3)));
    EXPECT_TRUE(task.isDueIn(std::chrono::days(2)));
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));

    // Due in the past
    task.dueDate = PointlessCore::Clock::now() - std::chrono::hours(24);
    EXPECT_FALSE(task.isDueIn(std::chrono::days(1)));
}

TEST(TaskTest, DueDatePreservedOnSerializeDeserialize)
{
    using namespace std::chrono;
    PointlessCore::Task original;
    original.dueDate = system_clock::now() + hours(24 * 5);
    auto json_result = glz::write_json(original);
    ASSERT_TRUE(json_result.has_value());
    std::string json_str = json_result.value();

    EXPECT_TRUE(json_str.find("\"dueDate\"") != std::string::npos);
    // std::cout << json_str << std::endl;

    PointlessCore::Task deserialized;
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
