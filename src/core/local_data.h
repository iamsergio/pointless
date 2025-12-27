// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data.h"

#include <expected>
#include <string>
#include <vector>
#include <variant>

namespace pointless::core {

class LocalData
{
public:
    LocalData();

    [[nodiscard]] std::expected<std::monostate, std::string> loadDataFromFile();
    [[nodiscard]] std::expected<Data, std::string> loadDataFromFile(const std::string &filename) const;
    [[nodiscard]] std::expected<std::monostate, std::string> save() const;

    [[nodiscard]] Data &data()
    {
        return _data;
    }

    void setData(const Data &data)
    {
        _data = data;
    }

    void addDeletedTag(const std::string &tag)
    {
        _deletedTags.push_back(tag);
    }

    void addDeletedTask(const std::string &uuid)
    {
        _deletedTasks.push_back(uuid);
    }

    [[nodiscard]] const std::vector<std::string> &deletedTags() const
    {
        return _deletedTags;
    }

    [[nodiscard]] const std::vector<std::string> &deletedTasks() const
    {
        return _deletedTasks;
    }

    void clearServerSyncBits();

private:
    [[nodiscard]] std::string getDataFilePath() const;
    std::string _dataDir;
    Data _data;
    std::vector<std::string> _deletedTags;
    std::vector<std::string> _deletedTasks;
};

}
