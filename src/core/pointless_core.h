
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task_manager.h"

#include <string>
#include <optional>

namespace PointlessCore {

std::optional<TaskManager> loadTaskManagerFromJsonFile(const std::string &filename);

} // namespace PointlessCore
