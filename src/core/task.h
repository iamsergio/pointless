// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "tag.h"
#include <glaze/glaze.hpp>
#include <expected>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace PointlessCore {


class Task
{
public:
    Task();
    Task(const std::string &uuid, const std::chrono::system_clock::time_point &creationTimestamp, const std::string &title = "");

    int revision = -1;
    bool needsSyncToServer = false;
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
};

} // namespace PointlessCore

namespace {
    constexpr auto timestamp_to_millis = [](const std::chrono::system_clock::time_point& tp) -> int64_t {
        return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    };
    
    constexpr auto optional_timestamp_to_millis = [](const std::optional<std::chrono::system_clock::time_point>& opt_tp) -> std::optional<int64_t> {
        if (opt_tp) 
            return timestamp_to_millis(*opt_tp);
        return std::nullopt;
    };
}

template <>
struct glz::meta<PointlessCore::Task> {
    using T = PointlessCore::Task;
    static constexpr auto value = object(
        "revision", &T::revision,
        "needsSyncToServer", &T::needsSyncToServer,
        "uuid", &T::uuid,
        "parentUuid", &T::parentUuid,
        "title", &T::title,
        "isDone", &T::isDone,
        "isImportant", &T::isImportant,
        "hideOnWeekends", &T::hideOnWeekends,
        "tags", [](const T& t) { 
            std::vector<std::string> tag_names;
            for (const auto& tag : t.tags) {
                tag_names.push_back(tag.name());
            }
            return tag_names;
        },
        "creationTimestamp", [](const T& t) { return timestamp_to_millis(t.creationTimestamp); },
        "modificationTimestamp", [](const T& t) { return optional_timestamp_to_millis(t.modificationTimestamp); },
        "lastPomodoroDate", [](const T& t) { return optional_timestamp_to_millis(t.lastPomodoroDate); },
        "dueDate", [](const T& t) { return optional_timestamp_to_millis(t.dueDate); },
        "completionDate", [](const T& t) { return optional_timestamp_to_millis(t.completionDate); },
        "uuidInDeviceCalendar", &T::uuidInDeviceCalendar,
        "deviceCalendarUuid", &T::deviceCalendarUuid,
        "deviceCalendarName", &T::deviceCalendarName
    );
};

