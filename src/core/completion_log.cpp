// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "completion_log.h"
#include "Clock.h"
#include "logger.h"
#include "task.h"
#include "utils.h"

#include <array>
#include <ctime>
#include <fstream>
#include <sstream>
#include <system_error>

namespace pointless::core::CompletionLog {

std::filesystem::path baseDir()
{
    if (auto jPath = getenv_or_empty("J_PATH"); !jPath.empty())
        return jPath;

    return std::filesystem::path(getenv_or_empty("HOME")) / "pointless_logs";
}

std::filesystem::path filePath(std::chrono::system_clock::time_point date)
{
    const std::time_t t = std::chrono::system_clock::to_time_t(date);
    std::tm tm {};
    ( void )localtime_r(&t, &tm);

    // ISO 8601: weeks start on Monday and the year is the week-based year
    std::array<char, 8> year {};
    std::array<char, 8> week {};
    ( void )std::strftime(year.data(), year.size(), "%G", &tm);
    ( void )std::strftime(week.data(), week.size(), "%V", &tm);

    return baseDir() / year.data() / (std::string("w") + std::to_string(std::stoi(week.data())) + ".md");
}

std::string entryForTask(const Task &task)
{
    const auto tag = task.tagName();
    std::string entry = "- " + (tag.empty() ? task.title : tag + ": " + task.title) + '\n';

    std::istringstream notes(task.description.value_or(""));
    for (std::string line; std::getline(notes, line);) {
        const auto first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos)
            continue;
        const auto last = line.find_last_not_of(" \t\r");
        entry += "  - " + line.substr(first, last - first + 1) + '\n';
    }

    return entry;
}

bool append(const Task &task)
{
    const auto path = filePath(Clock::now());

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) {
        P_LOG_ERROR("Failed to create directory {}: {}", path.parent_path().string(), ec.message());
        return false;
    }

    std::ofstream file(path, std::ios::app);
    if (!file) {
        P_LOG_ERROR("Failed to open {}", path.string());
        return false;
    }

    file << entryForTask(task);
    return file.good();
}

}
