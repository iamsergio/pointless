// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task.h"
#include "logger.h"
#include "tag.h"
#include "Clock.h"

#include <iomanip>
#include <sstream>

namespace PointlessCore {

Task::Task() = default;

Task::Task(const std::string &uuid_, const std::chrono::system_clock::time_point &creationTimestamp_, const std::string &title_)
    : uuid(uuid_)
    , title(title_)
    , creationTimestamp(creationTimestamp_)
{
}

bool Task::containsTag(std::string_view tagName) const
{
    return std::any_of(tags.begin(), tags.end(), [tagName](const auto &tag) {
        return tag == tagName;
    });
}

bool Task::isSoon() const
{
    return containsTag(BUILTIN_TAG_SOON) || isDueIn(std::chrono::days(15));
}

bool Task::isLater() const
{
    return !isSoon() && !isCurrent();
}

bool Task::isCurrent() const
{
    return containsTag(BUILTIN_TAG_CURRENT);
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

void Task::dumpDebug() const
{
    std::string json;
    if (glz::write_json(*this, json)) {
        P_LOG_ERROR("Failed to serialize task to JSON");
    } else {
        auto pretty = glz::prettify_json(json);
        P_LOG_DEBUG("Task JSON: {}", pretty);
    }

    auto timeToString = [](const std::chrono::system_clock::time_point &tp) {
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
    if (!isDone || !other.isDone) {
        isDone = false;
    } else {
        isDone = true;
    }

    // Importance: Important > Not Important
    if (isImportant || other.isImportant) {
        isImportant = true;
    }

    auto myTime = modificationTimestamp.value_or(std::chrono::system_clock::time_point::min());
    auto otherTime = other.modificationTimestamp.value_or(std::chrono::system_clock::time_point::min());

    // Due Date
    if (dueDate.has_value() && other.dueDate.has_value()) {
        if (otherTime > myTime) {
            dueDate = other.dueDate;
        }
    } else if (other.dueDate.has_value()) {
        dueDate = other.dueDate;
    }

    // Title
    if (otherTime > myTime) {
        title = other.title;
    }

    // Tags: Union
    for (const auto &tag : other.tags) {
        if (!containsTag(tag)) {
            tags.push_back(tag);
        }
    }

    // Special Rule: Current wins over Soon
    bool hasCurrent = containsTag(BUILTIN_TAG_CURRENT);
    bool hasSoon = containsTag(BUILTIN_TAG_SOON);

    if (hasCurrent && hasSoon) {
        // Remove Soon
        std::erase(tags, std::string(BUILTIN_TAG_SOON));
    }
}
} // namespace PointlessCore
