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
    ~LocalData() = default;

    LocalData(const LocalData &) = delete;
    LocalData &operator=(const LocalData &) = delete;
    LocalData(LocalData &&) = delete;
    LocalData &operator=(LocalData &&) = delete;

    [[nodiscard]] std::expected<void, std::string> loadDataFromFile();
    [[nodiscard]] std::expected<Data, std::string> loadDataFromFile(const std::string &filename) const;
    [[nodiscard]] std::expected<void, std::string> save() const;

    [[nodiscard]] Data &data()
    {
        return _data;
    }

    [[nodiscard]] const Data &data() const
    {
        return _data;
    }

    void setData(const Data &data);

    [[nodiscard]] std::expected<void, std::string> setDataAndSave(const Data &data);

    [[nodiscard]] size_t taskCount() const;
    [[nodiscard]] const Task &taskAt(size_t index) const;
    [[nodiscard]] const Task *taskForUuid(const std::string &uuid) const;
    [[nodiscard]] const Task *taskForTitle(const std::string &title) const;
    [[nodiscard]] size_t tagCount() const;
    [[nodiscard]] const Tag &tagAt(size_t index) const;

    [[nodiscard]] const std::vector<std::string> &deletedTags() const
    {
        return _data.deletedTagNames();
    }

    [[nodiscard]] const std::vector<std::string> &deletedTasks() const
    {
        return _data.deletedTaskUuids();
    }

    bool addTask(Task task);
    bool updateTask(Task task);
    bool removeTask(const std::string &uuid);
    bool removeTag(const std::string &tagName);

    void clearServerSyncBits();

private:
    [[nodiscard]] std::string getDataFilePath() const;
    Data _data;
};

}
