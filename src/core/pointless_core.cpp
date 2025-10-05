
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pointless_core.h"

#include <fstream>

#include <nlohmann/json.hpp>
#include <expected>

namespace PointlessCore {


std::expected<TaskManager, std::string> loadTaskManagerFromJsonFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file) {
        return std::unexpected("Failed to open file: " + filename);
    }
    nlohmann::json j;
    file >> j;
    if (file.fail()) {
        return std::unexpected("Failed to parse JSON in file: " + filename);
    }
    try {
        return TaskManager::fromJson(j);
    } catch (const std::exception &e) {
        return std::unexpected(std::string("Failed to convert JSON to TaskManager: ") + e.what());
    }
}

} // namespace PointlessCore
