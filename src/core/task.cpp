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
    for (const auto &tag : tags) {
        if (tag == tagName) {
            return true;
        }
    }
    return false;
}

bool Task::isSoon() const
{
    return containsTag("soon");
}

bool Task::isLater() const
{
    return containsTag("later");
}

bool Task::isCurrent() const
{
    return !isSoon() && !isLater();
}

std::string Task::tagName() const
{
    for (const auto &tag : tags) {
        if (tag != "soon" && tag != "later") {
            return tag;
        }
    }
    return {};
}

} // namespace PointlessCore
