// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"
#include "context.h"

#include <filesystem>
#include <fstream>
#include <cstdlib>

using namespace pointless;
using namespace pointless::core;

LocalData::LocalData() = default;

std::expected<std::monostate, std::string> LocalData::loadDataFromFile()
{
    auto result = loadDataFromFile(getDataFilePath());
    if (result) {
        _data = result.value();
        return std::monostate {};
    }

    _data = {};
    return std::unexpected(result.error());
}

std::expected<pointless::core::Data, std::string> LocalData::loadDataFromFile(const std::string &filename) const
{
    if (!std::filesystem::exists(filename)) {
        return Data {};
    }

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

std::expected<std::monostate, std::string> LocalData::save() const
{
    const auto filename = getDataFilePath();
    std::ofstream file(filename);
    if (!file) {
        return std::unexpected("Failed to open file for writing: " + filename);
    }

    const auto jsonResult = _data.toJson();
    if (!jsonResult) {
        return std::unexpected(jsonResult.error());
    }

    file << jsonResult.value();
    if (!file) {
        return std::unexpected("Failed to write to file: " + filename);
    }

    return std::monostate {};
}

std::expected<std::monostate, std::string> LocalData::setDataAndSave(const Data &data)
{
    _data = data;
    return save();
}

std::string LocalData::getDataFilePath() const
{
    return core::Context::self().localFilePath();
}

void LocalData::clearServerSyncBits()
{
    _data.clearServerSyncBits();
}
