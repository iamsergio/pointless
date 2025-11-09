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

} // namespace PointlessCore
