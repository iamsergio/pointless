
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
    auto managerOpt = PointlessCore::loadTaskManagerFromJsonFile(json_path);
    if (!managerOpt) {
        std::println("Failed to load TaskManager from file: {}", json_path);
        return 1;
    }
    std::println("Loaded TaskManager successfully from {}", json_path);
    std::println("Task count: {}", managerOpt->taskCount());
    for (const auto &task : managerOpt->getAllTasks()) {
        std::println("Task: {}", task.title);
    }
    return 0;
}
