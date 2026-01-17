// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "tag.h"
#include "task.h"

#include <glaze/glaze.hpp>

#include <optional>
#include <string>
#include <vector>

namespace pointless::core {

struct DataPayload
{
    int revision = -1;
    std::vector<Task> tasks;
    std::vector<Tag> tags;
    std::vector<std::string> deletedTaskUuids;
    std::vector<std::string> deletedTagNames;
};

class Data
{
public:
    Data();

    // Task management methods
    void addTask(const Task &task);
    bool removeTask(const std::string &uuid);
    [[nodiscard]] std::optional<Task> getTask(const std::string &uuid) const;
    [[nodiscard]] std::vector<Task> getAllTasks() const;
    bool updateTask(const Task &task, bool incrementTaskRevision);
    bool setTask(const Task &task);
    void clearTasks();
    [[nodiscard]] size_t taskCount() const;
    [[nodiscard]] std::vector<Task> newTasks() const;
    [[nodiscard]] std::vector<Task> modifiedTasks() const;

    // Task filtering methods
    [[nodiscard]] std::vector<Task> getTasksByTag(const std::string &tagName) const;
    [[nodiscard]] std::vector<Task> getCompletedTasks() const;
    [[nodiscard]] std::vector<Task> getPendingTasks() const;
    [[nodiscard]] std::vector<Task> getImportantTasks() const;
    [[nodiscard]] std::vector<Task> getTasksByParent(const std::string &parentUuid) const;
    [[nodiscard]] const Task &taskAt(size_t index) const;
    [[nodiscard]] const Task *taskForUuid(const std::string &uuid) const;
    [[nodiscard]] Task *taskForUuid(const std::string &uuid);
    [[nodiscard]] const Task *taskForTitle(const std::string &title) const;
    [[nodiscard]] std::string debug_taskUids() const;

    // Tag management methods
    void addTag(const Tag &tag);
    bool removeTag(const std::string &tagName);
    [[nodiscard]] std::optional<Tag> getTag(const std::string &tagName) const;
    [[nodiscard]] std::vector<Tag> allTags() const;
    void clearTags();
    [[nodiscard]] size_t tagCount() const;
    [[nodiscard]] std::vector<Tag> newTags() const;
    [[nodiscard]] const Tag &tagAt(size_t index) const;
    [[nodiscard]] bool containsTag(const std::string &tagName) const;

    [[nodiscard]] std::vector<Tag> getUsedTags() const;
    [[nodiscard]] std::vector<Tag> getUnusedTags() const;
    void removeUnusedTags();

    // Deleted items management
    void addDeletedTaskUuid(const std::string &uuid);
    [[nodiscard]] const std::vector<std::string> &deletedTaskUuids() const;

    void addDeletedTagName(const std::string &tagName);
    [[nodiscard]] const std::vector<std::string> &deletedTagNames() const;

    void clearServerSyncBits();
    void setRevision(int revision);
    [[nodiscard]] int revision() const;

    static std::expected<Data, std::string> fromJson(const std::string &json_str);
    [[nodiscard]] std::expected<std::string, std::string> toJson() const;
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] bool isValid() const;

    DataPayload _data;
    mutable bool needsUpload = false;
    mutable bool needsLocalSave = false;

private:
    // Helper methods
    std::vector<Task>::iterator findTaskByUuid(const std::string &uuid);
    [[nodiscard]] std::vector<Task>::const_iterator findTaskByUuid(const std::string &uuid) const;
    std::vector<Tag>::iterator findTagByName(const std::string &tagName);
    [[nodiscard]] std::vector<Tag>::const_iterator findTagByName(const std::string &tagName) const;
};

} // namespace pointless::core

template<>
struct glz::meta<pointless::core::DataPayload>
{
    using T = pointless::core::DataPayload;
    static constexpr auto value = object(
        "tasks", &T::tasks,
        "tags", &T::tags,
        "revision", &T::revision,
        "deletedTaskUuids", &T::deletedTaskUuids,
        "deletedTagNames", &T::deletedTagNames);
};
