// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "task.h"

namespace PointlessCore {

Task::Task()
{
}

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
    if (!dueDate)
        return false;
    const auto now = std::chrono::system_clock::now();
    const auto due = *dueDate;
    return due <= now + days && due >= now;
}

} // namespace PointlessCore
