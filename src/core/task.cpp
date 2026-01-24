// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task.h"
#include "logger.h"
#include "tag.h"
#include "Clock.h"
#include "date_utils.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace pointless::core {

Task::Task() = default;

Task::Task(std::string uuid_, std::chrono::system_clock::time_point creationTimestamp_, std::string title_)
    : uuid(std::move(uuid_))
    , title(std::move(title_))
    , creationTimestamp(creationTimestamp_)
{
}

bool Task::containsTag(std::string_view tagName) const
{
    return std::ranges::any_of(tags, [tagName](const auto &tag) {
        return tag == tagName;
    });
}

bool Task::isSoon() const
{
    if (isCurrent())
        return false;

    return containsTag(BUILTIN_TAG_SOON) || isDueIn(std::chrono::days(15));
}

bool Task::isLater() const
{
    return !isSoon() && !isCurrent();
}

bool Task::isCurrent() const
{
    return containsTag(BUILTIN_TAG_CURRENT) || isDueThisWeek();
}

bool Task::isEvening() const
{
    return containsTag(BUILTIN_TAG_EVENING);
}

std::string Task::tagName() const
{
    for (const auto &tag : tags) {
        if (!tagIsBuiltin(tag)) {
            return tag;
        }
    }
    return {};
}

bool Task::isDueIn(std::chrono::days days) const
{
    if (!dueDate) {
        return false;
    }
    const auto now = Clock::now();
    const auto due = *dueDate;
    return due <= now + days && due >= now;
}

bool Task::isDueTomorrow() const
{
    if (!dueDate) {
        return false;
    }
    return DateUtils::isTomorrow(*dueDate);
}

bool Task::isDueThisWeek() const
{
    if (!dueDate) {
        return false;
    }
    return DateUtils::isThisWeek(*dueDate);
}

void Task::setTags(const std::vector<std::string> &newTags)
{
    tags = {};
    for (const auto &tag : newTags) {
        addTag(tag);
    }
}

bool Task::addTag(std::string_view tag)
{
    if (!tag.empty() && !containsTag(tag)) {
        tags.emplace_back(tag);
        return true;
    }
    return false;
}

void Task::removeBuiltinTags()
{
    std::erase_if(tags, [](const std::string &tag) {
        return tagIsBuiltin(tag);
    });
}

void Task::dumpDebug() const
{
    std::string json;
    if (glz::write_json(*this, json)) {
        P_LOG_ERROR("Failed to serialize task to JSON");
    } else {
        auto pretty = glz::prettify_json(json);
        P_LOG_DEBUG("Task JSON: {}", pretty);
    }

    auto timeToString = [](std::chrono::system_clock::time_point tp) {
        auto t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    };

    P_LOG_DEBUG("Creation: {}", timeToString(creationTimestamp));
    if (modificationTimestamp)
        P_LOG_DEBUG("Modification: {}", timeToString(*modificationTimestamp));
    if (lastPomodoroDate)
        P_LOG_DEBUG("Last Pomodoro: {}", timeToString(*lastPomodoroDate));
    if (dueDate)
        P_LOG_DEBUG("Due: {}", timeToString(*dueDate));
    if (completionDate)
        P_LOG_DEBUG("Completion: {}", timeToString(*completionDate));
}


void Task::mergeConflict(const Task &other)
{
    // Completion: Undone > Done
    isDone = isDone && other.isDone;

    // Importance: Important > Not Important
    isImportant = isImportant || other.isImportant;

    auto myTime = modificationTimestamp.value_or(std::chrono::system_clock::time_point::min());
    auto otherTime = other.modificationTimestamp.value_or(std::chrono::system_clock::time_point::min());

    const bool otherIsMoreRecent = otherTime > myTime;

    // Due Date
    if (dueDate.has_value() && other.dueDate.has_value()) {
        if (otherIsMoreRecent) {
            dueDate = other.dueDate;
        }
    } else if (other.dueDate.has_value()) {
        dueDate = other.dueDate;
    }

    // Title
    if (otherIsMoreRecent && other.title != title) {
        title = other.title;
    }

    // Tags: Union
    std::vector<std::string> newTags = tags;
    for (const auto &tag : other.tags) {
        newTags.push_back(tag);
    }
    setTags(newTags);

    // Special Rule: Current wins over Soon
    bool hasCurrent = containsTag(BUILTIN_TAG_CURRENT);
    bool hasSoon = containsTag(BUILTIN_TAG_SOON);

    if (hasCurrent && hasSoon) {
        // Remove Soon
        std::erase(tags, std::string(BUILTIN_TAG_SOON));
    }
}
} // namespace pointless::core
