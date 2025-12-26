// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "tag.h"
#include "task.h"

#include <glaze/glaze.hpp>

#include <optional>
#include <string>
#include <vector>

namespace PointlessCore {

struct Data
{
    int revision = 0;
    std::vector<Task> tasks;
    std::vector<Tag> tags;
    std::vector<std::string> deletedTaskUuids;
    std::vector<std::string> deletedTagNames;
};

class TaskManager
{
public:
    TaskManager();

    // Task management methods
    void addTask(const Task &task);
    bool removeTask(const std::string &uuid);
    [[nodiscard]] std::optional<Task> getTask(const std::string &uuid) const;
    [[nodiscard]] std::vector<Task> getAllTasks() const;
    bool updateTask(const Task &task);
    void clearTasks();
    [[nodiscard]] size_t taskCount() const;

    // Task filtering methods
    [[nodiscard]] std::vector<Task> getTasksByTag(const std::string &tagName) const;
    [[nodiscard]] std::vector<Task> getCompletedTasks() const;
    [[nodiscard]] std::vector<Task> getPendingTasks() const;
    [[nodiscard]] std::vector<Task> getImportantTasks() const;
    [[nodiscard]] std::vector<Task> getTasksByParent(const std::string &parentUuid) const;

    // Tag management methods
    void addTag(const Tag &tag);
    bool removeTag(const std::string &tagName);
    [[nodiscard]] std::optional<Tag> getTag(const std::string &tagName) const;
    [[nodiscard]] std::vector<Tag> getAllTags() const;
    void clearTags();
    [[nodiscard]] size_t tagCount() const;

    // Utility methods
    [[nodiscard]] std::vector<Tag> getUsedTags() const; // Tags that are used by at least one task
    [[nodiscard]] std::vector<Tag> getUnusedTags() const; // Tags that are not used by any task
    void removeUnusedTags();

    // Serialization methods
    static std::expected<TaskManager, std::string> fromJson(const std::string &json_str);

    Data _data;

private:
    // Helper methods
    std::vector<Task>::iterator findTaskByUuid(const std::string &uuid);
    [[nodiscard]] std::vector<Task>::const_iterator findTaskByUuid(const std::string &uuid) const;
    std::vector<Tag>::iterator findTagByName(const std::string &tagName);
    [[nodiscard]] std::vector<Tag>::const_iterator findTagByName(const std::string &tagName) const;
};

} // namespace PointlessCore

template<>
struct glz::meta<PointlessCore::Data>
{
    using T = PointlessCore::Data;
    static constexpr auto value = object(
        "tasks", &T::tasks,
        "tags", &T::tags,
        "revision", &T::revision,
        "deletedTaskUuids", &T::deletedTaskUuids,
        "deletedTagNames", &T::deletedTagNames);
};
