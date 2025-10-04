// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "item.h"
#include "tag.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace PointlessCore {


class Task : public Item
{
public:
    Task();
    Task(const std::string &uuid, const std::chrono::system_clock::time_point &creationTimestamp, const std::string &title = "");

    std::string uuid;
    std::optional<std::string> parentUuid;
    std::string title;
    bool isDone = false;
    bool isImportant = false;
    bool hideOnWeekends = false;
    std::vector<Tag> tags;
    std::chrono::system_clock::time_point creationTimestamp;
    std::optional<std::chrono::system_clock::time_point> modificationTimestamp;
    std::optional<std::chrono::system_clock::time_point> lastPomodoroDate;
    std::optional<std::chrono::system_clock::time_point> dueDate;
    std::optional<std::chrono::system_clock::time_point> completionDate;
    std::optional<std::string> uuidInDeviceCalendar;
    std::optional<std::string> deviceCalendarUuid;
    std::optional<std::string> deviceCalendarName;

    nlohmann::json toJson() const;
    static Task fromJson(const nlohmann::json &j);
};

} // namespace PointlessCore
