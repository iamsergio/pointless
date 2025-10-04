// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task_manager.h"
#include <algorithm>

namespace PointlessCore {

TaskManager::TaskManager()
{
}

// Task management methods
void TaskManager::addTask(const Task &task)
{
    // Check if task with same UUID already exists
    auto it = findTaskByUuid(task.uuid);
    if (it == m_tasks.end()) {
        m_tasks.push_back(task);
    }
}

bool TaskManager::removeTask(const std::string &uuid)
{
    auto it = findTaskByUuid(uuid);
    if (it != m_tasks.end()) {
        m_tasks.erase(it);
        return true;
    }
    return false;
}

std::optional<Task> TaskManager::getTask(const std::string &uuid) const
{
    auto it = findTaskByUuid(uuid);
    if (it != m_tasks.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Task> TaskManager::getAllTasks() const
{
    return m_tasks;
}

bool TaskManager::updateTask(const Task &task)
{
    auto it = findTaskByUuid(task.uuid);
    if (it != m_tasks.end()) {
        *it = task;
        return true;
    }
    return false;
}

void TaskManager::clearTasks()
{
    m_tasks.clear();
}

size_t TaskManager::taskCount() const
{
    return m_tasks.size();
}

// Task filtering methods
std::vector<Task> TaskManager::getTasksByTag(const Tag &tag) const
{
    std::vector<Task> result;
    for (const auto &task : m_tasks) {
        auto tagIt = std::find(task.tags.begin(), task.tags.end(), tag);
        if (tagIt != task.tags.end()) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getCompletedTasks() const
{
    std::vector<Task> result;
    for (const auto &task : m_tasks) {
        if (task.isDone) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getPendingTasks() const
{
    std::vector<Task> result;
    for (const auto &task : m_tasks) {
        if (!task.isDone) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getImportantTasks() const
{
    std::vector<Task> result;
    for (const auto &task : m_tasks) {
        if (task.isImportant) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getTasksByParent(const std::string &parentUuid) const
{
    std::vector<Task> result;
    for (const auto &task : m_tasks) {
        if (task.parentUuid && *task.parentUuid == parentUuid) {
            result.push_back(task);
        }
    }
    return result;
}

// Tag management methods
void TaskManager::addTag(const Tag &tag)
{
    // Check if tag with same name already exists
    auto it = findTagByName(tag.name());
    if (it == m_tags.end()) {
        m_tags.push_back(tag);
    }
}

bool TaskManager::removeTag(const std::string &tagName)
{
    auto it = findTagByName(tagName);
    if (it != m_tags.end()) {
        m_tags.erase(it);
        return true;
    }
    return false;
}

std::optional<Tag> TaskManager::getTag(const std::string &tagName) const
{
    auto it = findTagByName(tagName);
    if (it != m_tags.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Tag> TaskManager::getAllTags() const
{
    return m_tags;
}

void TaskManager::clearTags()
{
    m_tags.clear();
}

size_t TaskManager::tagCount() const
{
    return m_tags.size();
}

// Utility methods
std::vector<Tag> TaskManager::getUsedTags() const
{
    std::vector<Tag> result;
    for (const auto &tag : m_tags) {
        // Check if this tag is used by any task
        bool isUsed = false;
        for (const auto &task : m_tasks) {
            auto tagIt = std::find(task.tags.begin(), task.tags.end(), tag);
            if (tagIt != task.tags.end()) {
                isUsed = true;
                break;
            }
        }
        if (isUsed) {
            result.push_back(tag);
        }
    }
    return result;
}

std::vector<Tag> TaskManager::getUnusedTags() const
{
    std::vector<Tag> result;
    for (const auto &tag : m_tags) {
        // Check if this tag is not used by any task
        bool isUsed = false;
        for (const auto &task : m_tasks) {
            auto tagIt = std::find(task.tags.begin(), task.tags.end(), tag);
            if (tagIt != task.tags.end()) {
                isUsed = true;
                break;
            }
        }
        if (!isUsed) {
            result.push_back(tag);
        }
    }
    return result;
}

void TaskManager::removeUnusedTags()
{
    auto unusedTags = getUnusedTags();
    for (const auto &tag : unusedTags) {
        removeTag(tag.name());
    }
}

// Serialization methods
nlohmann::json TaskManager::toJson() const
{
    nlohmann::json j;
    j["tasks"] = nlohmann::json::array();
    for (const auto &task : m_tasks) {
        j["tasks"].push_back(task.toJson());
    }
    j["tags"] = nlohmann::json::array();
    for (const auto &tag : m_tags) {
        j["tags"].push_back(tag.toJson());
    }
    return j;
}

TaskManager TaskManager::fromJson(const nlohmann::json &j)
{
    TaskManager manager;

    if (j.contains("tasks")) {
        for (const auto &taskJson : j["tasks"]) {
            manager.addTask(Task::fromJson(taskJson));
        }
    }

    if (j.contains("tags")) {
        for (const auto &tagJson : j["tags"]) {
            manager.addTag(Tag::fromJson(tagJson));
        }
    }

    return manager;
}

// Helper methods
std::vector<Task>::iterator TaskManager::findTaskByUuid(const std::string &uuid)
{
    return std::find_if(m_tasks.begin(), m_tasks.end(),
                        [&uuid](const Task &task) { return task.uuid == uuid; });
}

std::vector<Task>::const_iterator TaskManager::findTaskByUuid(const std::string &uuid) const
{
    return std::find_if(m_tasks.begin(), m_tasks.end(),
                        [&uuid](const Task &task) { return task.uuid == uuid; });
}

std::vector<Tag>::iterator TaskManager::findTagByName(const std::string &tagName)
{
    return std::find_if(m_tags.begin(), m_tags.end(),
                        [&tagName](const Tag &tag) { return tag.name() == tagName; });
}

std::vector<Tag>::const_iterator TaskManager::findTagByName(const std::string &tagName) const
{
    return std::find_if(m_tags.begin(), m_tags.end(),
                        [&tagName](const Tag &tag) { return tag.name() == tagName; });
}
} // namespace PointlessCore
