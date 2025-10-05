
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task_manager.h"

#include <string>
#include <expected>

namespace PointlessCore {

std::expected<TaskManager, std::string> loadTaskManagerFromJsonFile(const std::string &filename);

} // namespace PointlessCore
