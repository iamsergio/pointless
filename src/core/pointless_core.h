
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task_manager.h"

#include <expected>
#include <string>

namespace pointless::core {

std::expected<Data, std::string> loadTaskManagerFromJsonFile(const std::string &filename);

} // namespace pointless::core
