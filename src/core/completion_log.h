// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <filesystem>
#include <string>

namespace pointless::core {

class Task;

namespace CompletionLog {

std::filesystem::path baseDir();
std::filesystem::path filePath(std::chrono::system_clock::time_point date);
std::string entryForTask(const Task &task);
bool append(const Task &task);

}

}
