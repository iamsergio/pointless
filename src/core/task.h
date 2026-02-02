// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <glaze/glaze.hpp>

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace pointless::core {

class Task
{
public:
    Task();
    Task(std::string uuid, std::chrono::system_clock::time_point creationTimestamp, std::string title = {});

    [[nodiscard]] bool containsTag(std::string_view tagName) const;
    [[nodiscard]] bool isSoon() const;
    [[nodiscard]] bool isLater() const;
    [[nodiscard]] bool isCurrent() const;
    [[nodiscard]] bool isEvening() const;
    [[nodiscard]] bool isDueIn(std::chrono::days days) const;
    [[nodiscard]] bool isDueTomorrow() const;
    [[nodiscard]] bool isDueThisWeek() const;
    [[nodiscard]] bool isOverdue() const;
    [[nodiscard]] bool shouldBeCleanedUp() const;

    void removeBuiltinTags();
    void mergeConflict(const Task &other);

    void dumpDebug() const;

    [[nodiscard]] std::string tagName() const;
    void setTags(const std::vector<std::string> &tags);
    bool addTag(std::string_view tag);

    int revision = -1;
    bool needsSyncToServer = false;
    std::string uuid;
    std::optional<std::string> parentUuid;
    std::string title;
    bool isDone = false;
    std::optional<bool> isGoal;
    bool isImportant = false;
    std::optional<bool> hideOnWeekends;
    int timesPerWeek = 1;
    std::vector<int> lastCompletions;
    std::string sectionName;
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point creationTimestamp;
    std::optional<std::chrono::system_clock::time_point> modificationTimestamp;
    std::optional<std::chrono::system_clock::time_point> lastPomodoroDate;
    std::optional<std::chrono::system_clock::time_point> dueDate;
    std::optional<std::chrono::system_clock::time_point> completionDate;
    std::optional<std::string> uuidInDeviceCalendar;
    std::optional<std::string> deviceCalendarUuid;
    std::optional<std::string> deviceCalendarName;
};

} // namespace pointless::core

namespace glz {

template<>
struct from<JSON, std::chrono::system_clock::time_point>
{
    template<auto Opts>
    static void op(std::chrono::system_clock::time_point &value, is_context auto &&ctx, auto &&it, auto &&end)
    {
        int64_t millis = 0;
        parse<JSON>::op<Opts>(millis, ctx, it, end);
        value = std::chrono::system_clock::time_point(std::chrono::milliseconds(millis));
    }
};

template<>
struct to<JSON, std::chrono::system_clock::time_point>
{
    template<auto Opts>
    static void op(std::chrono::system_clock::time_point value, is_context auto &&ctx, auto &&b, auto &&ix) noexcept
    {
        int64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
        serialize<JSON>::op<Opts>(millis, ctx, b, ix);
    }
};

}

template<>
struct glz::meta<pointless::core::Task>
{
    using T = pointless::core::Task;
    static constexpr auto value = object(
        "revision", &T::revision,
        "uuid", &T::uuid,
        "parentUuid", &T::parentUuid,
        "title", &T::title,
        "isDone", &T::isDone,
        "isGoal", &T::isGoal,
        "isImportant", &T::isImportant,
        "hideOnWeekends", &T::hideOnWeekends,
        "timesPerWeek", &T::timesPerWeek,
        "lastCompletions", &T::lastCompletions,
        "sectionName", &T::sectionName,
        "tags", &T::tags,
        "creationTimestamp", &T::creationTimestamp,
        "modificationTimestamp", &T::modificationTimestamp,
        "lastPomodoroDate", &T::lastPomodoroDate,
        "dueDate", &T::dueDate,
        "completionDate", &T::completionDate,
        "uuidInDeviceCalendar", &T::uuidInDeviceCalendar,
        "deviceCalendarUuid", &T::deviceCalendarUuid,
        "deviceCalendarName", &T::deviceCalendarName);
};
