// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"
#include "context.h"
#include "logger.h"

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

    _data.needsLocalSave = false;
    return std::monostate {};
}

std::expected<std::monostate, std::string> LocalData::setDataAndSave(const Data &data)
{
    setData(data);
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

void LocalData::setData(const Data &data)
{
    _data = data;
    P_LOG_DEBUG("Set new data");
}

bool LocalData::removeTask(const std::string &uuid)
{
    if (_data.removeTask(uuid)) {
        _data.addDeletedTaskUuid(uuid);
        return true;
    }
    return false;
}

bool LocalData::removeTag(const std::string &tagName)
{
    if (_data.removeTag(tagName)) {
        _data.addDeletedTagName(tagName);
        return true;
    }
    return false;
}

size_t LocalData::taskCount() const
{
    return _data.taskCount();
}

size_t LocalData::tagCount() const
{
    return _data.tagCount();
}
