// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task.h"
#include "tag.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <optional>

namespace PointlessCore {

class TaskManager
{
public:
    TaskManager();

    // Task management methods
    void addTask(const Task &task);
    bool removeTask(const std::string &uuid);
    std::optional<Task> getTask(const std::string &uuid) const;
    std::vector<Task> getAllTasks() const;
    bool updateTask(const Task &task);
    void clearTasks();
    size_t taskCount() const;

    // Task filtering methods
    std::vector<Task> getTasksByTag(const Tag &tag) const;
    std::vector<Task> getCompletedTasks() const;
    std::vector<Task> getPendingTasks() const;
    std::vector<Task> getImportantTasks() const;
    std::vector<Task> getTasksByParent(const std::string &parentUuid) const;

    // Tag management methods
    void addTag(const Tag &tag);
    bool removeTag(const std::string &tagName);
    std::optional<Tag> getTag(const std::string &tagName) const;
    std::vector<Tag> getAllTags() const;
    void clearTags();
    size_t tagCount() const;

    // Utility methods
    std::vector<Tag> getUsedTags() const; // Tags that are used by at least one task
    std::vector<Tag> getUnusedTags() const; // Tags that are not used by any task
    void removeUnusedTags();

    // Serialization methods
    nlohmann::json toJson() const;
    static std::expected<TaskManager, std::string> fromJson(const nlohmann::json &j);

private:
    std::vector<Task> m_tasks;
    std::vector<Tag> m_tags;

    // Helper methods
    std::vector<Task>::iterator findTaskByUuid(const std::string &uuid);
    std::vector<Task>::const_iterator findTaskByUuid(const std::string &uuid) const;
    std::vector<Tag>::iterator findTagByName(const std::string &tagName);
    std::vector<Tag>::const_iterator findTagByName(const std::string &tagName) const;
};

} // namespace PointlessCore
