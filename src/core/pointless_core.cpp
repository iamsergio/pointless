
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pointless_core.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace PointlessCore {

std::optional<TaskManager> loadTaskManagerFromJsonFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file) {
        return std::nullopt;
    }
    nlohmann::json j;
    try {
        file >> j;
    } catch (...) {
        return std::nullopt;
    }
    return TaskManager::fromJson(j);
}

} // namespace PointlessCore
