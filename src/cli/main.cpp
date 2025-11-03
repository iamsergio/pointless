
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../core/pointless_core.h"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

#include <string>

int main(int argc, char *argv[])
{
    quill::Backend::start();

    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink");
    auto logger = quill::Frontend::create_or_get_logger("pointless_cli", console_sink);

    if (argc < 2) {
        LOG_INFO(logger, "Usage: {} <json_file>", argv[0]);
        return 1;
    }

    std::string json_path = argv[1];
    auto result = PointlessCore::loadTaskManagerFromJsonFile(json_path);
    if (!result) {
        LOG_ERROR(logger, "Error: {}", result.error());
        return 1;
    }

    const auto &manager = result.value();
    LOG_INFO(logger, "Loaded TaskManager successfully from {}", json_path);
    LOG_INFO(logger, "Task count: {}", manager.taskCount());
    for (const auto &task : manager.getAllTasks()) {
        LOG_INFO(logger, "Task: {}", task.title);
    }

    return 0;
}
