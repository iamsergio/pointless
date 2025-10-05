
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../core/pointless_core.h"

#include <print>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::println("Usage: {} <json_file>", argv[0]);
        return 1;
    }
    std::string json_path = argv[1];
    auto result = PointlessCore::loadTaskManagerFromJsonFile(json_path);
    if (!result) {
        std::println("Error: {}", result.error());
        return 1;
    }
    const auto& manager = result.value();
    std::println("Loaded TaskManager successfully from {}", json_path);
    std::println("Task count: {}", manager.taskCount());
    for (const auto& task : manager.getAllTasks()) {
        std::println("Task: {}", task.title);
    }
    return 0;
}
