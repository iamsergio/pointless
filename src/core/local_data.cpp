// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"
#include "pointless_core.h"

#include <filesystem>
#include <stdexcept>
#include <cstdlib>

using namespace pointless::core;

LocalData::LocalData()
{
    const char *envVar = std::getenv("POINTLESS_CLIENT_DATA_DIR");
    if (envVar == nullptr) {
        throw std::runtime_error("POINTLESS_CLIENT_DATA_DIR environment variable is not set");
    }

    _dataDir = envVar;
}

std::expected<pointless::core::Data, std::string> LocalData::loadTaskManager() const
{
    const std::string filePath = getDataFilePath();
    return pointless::core::loadTaskManagerFromJsonFile(filePath);
}

std::string LocalData::getDataFilePath() const
{
    return std::filesystem::path(_dataDir) / "pointless.json";
}
