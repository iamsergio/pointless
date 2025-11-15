// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task.h"

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
    original_task.creationTimestamp = std::chrono::system_clock::now();
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
