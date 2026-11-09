// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "calendar_provider.h"
#include "logger.h"

#include <fstream>
#include <system_error>

namespace pointless::core {

namespace {

std::filesystem::path &icalDumpDirectory()
{
    static std::filesystem::path directory;
    return directory;
}

std::string sanitizedFileName(std::string_view name)
{
    std::string result;
    result.reserve(name.size());
    for (const char c : name) {
        const bool isSafe = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.';
        result.push_back(isSafe ? c : '_');
    }

    return result.empty() ? std::string("calendar") : result;
}

} // namespace

void setICalDumpDirectory(std::filesystem::path directory)
{
    icalDumpDirectory() = std::move(directory);
}

void dumpICal(std::string_view sourceName, std::string_view icalData)
{
    const std::filesystem::path &directory = icalDumpDirectory();
    if (directory.empty())
        return;

    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    if (ec) {
        P_LOG_WARNING("Could not create {}: {}", directory.string(), ec.message());
        return;
    }

    const auto path = directory / (sanitizedFileName(sourceName) + ".ics");
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file.write(icalData.data(), static_cast<std::streamsize>(icalData.size()));
    if (!file) {
        P_LOG_WARNING("Could not write {}", path.string());
        return;
    }

    P_LOG_INFO("Dumped {} bytes of iCal data from '{}' to {}", icalData.size(), sourceName, path.string());
}

} // namespace pointless::core
