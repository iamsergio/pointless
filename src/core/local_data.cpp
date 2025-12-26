// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"

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

std::expected<pointless::core::Data, std::string> LocalData::loadDataFromFile() const
{
    return loadDataFromFile(getDataFilePath());
}

std::expected<pointless::core::Data, std::string> LocalData::loadDataFromFile(const std::string &filename) const
{
    std::ifstream file(filename);
    if (!file) {
        return std::unexpected("Failed to open file: " + filename);
    }

    std::string json_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (json_content.empty()) {
        return std::unexpected("Failed to read JSON from file: " + filename);
    }

    return Data::fromJson(json_content);
}

std::string LocalData::getDataFilePath() const
{
    return std::filesystem::path(_dataDir) / "pointless.json";
}
