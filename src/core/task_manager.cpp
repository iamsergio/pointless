// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task_manager.h"

#include <algorithm>

namespace pointless::core {

Data::Data() = default;

// Task management methods
void Data::addTask(const Task &task)
{
    // Check if task with same UUID already exists
    auto it = findTaskByUuid(task.uuid);
    if (it == _data.tasks.end()) {
        _data.tasks.push_back(task);
    }
}

bool Data::removeTask(const std::string &uuid)
{
    auto it = findTaskByUuid(uuid);
    if (it != _data.tasks.end()) {
        _data.tasks.erase(it);
        return true;
    }
    return false;
}

std::optional<Task> Data::getTask(const std::string &uuid) const
{
    auto it = findTaskByUuid(uuid);
    if (it != _data.tasks.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Task> Data::getAllTasks() const
{
    return _data.tasks;
}

bool Data::updateTask(const Task &task)
{
    auto it = findTaskByUuid(task.uuid);
    if (it != _data.tasks.end()) {
        *it = task;
        return true;
    }
    return false;
}

void Data::clearTasks()
{
    _data.tasks.clear();
}

size_t Data::taskCount() const
{
    return _data.tasks.size();
}

// Task filtering methods
std::vector<Task> Data::getTasksByTag(const std::string &tagName) const
{
    std::vector<Task> result;
    result.reserve(_data.tasks.size());
    for (const auto &task : _data.tasks) {
        auto tagIt = std::ranges::find(task.tags, tagName);
        if (tagIt != task.tags.end()) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> Data::getCompletedTasks() const
{
    std::vector<Task> result;
    for (const auto &task : _data.tasks) {
        if (task.isDone) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> Data::getPendingTasks() const
{
    std::vector<Task> result;
    for (const auto &task : _data.tasks) {
        if (!task.isDone) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> Data::getImportantTasks() const
{
    std::vector<Task> result;
    for (const auto &task : _data.tasks) {
        if (task.isImportant) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> Data::getTasksByParent(const std::string &parentUuid) const
{
    std::vector<Task> result;
    for (const auto &task : _data.tasks) {
        if (task.parentUuid && *task.parentUuid == parentUuid) {
            result.push_back(task);
        }
    }
    return result;
}

// Tag management methods
void Data::addTag(const Tag &tag)
{
    // Check if tag with same name already exists
    auto it = findTagByName(tag.name);
    if (it == _data.tags.end()) {
        _data.tags.push_back(tag);
    }
}

bool Data::removeTag(const std::string &tagName)
{
    auto it = findTagByName(tagName);
    if (it != _data.tags.end()) {
        _data.tags.erase(it);
        return true;
    }
    return false;
}

std::optional<Tag> Data::getTag(const std::string &tagName) const
{
    auto it = findTagByName(tagName);
    if (it != _data.tags.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Tag> Data::getAllTags() const
{
    return _data.tags;
}

void Data::clearTags()
{
    _data.tags.clear();
}

size_t Data::tagCount() const
{
    return _data.tags.size();
}

// Utility methods
std::vector<Tag> Data::getUsedTags() const
{
    std::vector<Tag> result;
    for (const auto &tag : _data.tags) {
        // Check if this tag is used by any task
        bool isUsed = false;
        for (const auto &task : _data.tasks) {
            auto tagIt = std::ranges::find(task.tags, tag.name);
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

std::vector<Tag> Data::getUnusedTags() const
{
    std::vector<Tag> result;
    for (const auto &tag : _data.tags) {
        // Check if this tag is not used by any task
        bool isUsed = false;
        for (const auto &task : _data.tasks) {
            auto tagIt = std::ranges::find(task.tags, tag.name);
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

void Data::removeUnusedTags()
{
    auto unusedTags = getUnusedTags();
    for (const auto &tag : unusedTags) {
        removeTag(tag.name);
    }
}

std::expected<Data, std::string> Data::fromJson(const std::string &json_str)
{
    Data manager;
    auto result = glz::read<glz::opts {
        .error_on_unknown_keys = true,
        // .error_on_missing_keys = true,
        .skip_null_members = false,
        .error_on_const_read = true,
    }>(manager._data, json_str);
    if (result != glz::error_code::none) {
        return std::unexpected("Failed to parse JSON: " + std::string(glz::format_error(result, json_str)));
    }

    return manager;
}

// Helper methods
std::vector<Task>::iterator Data::findTaskByUuid(const std::string &uuid)
{
    return std::ranges::find_if(_data.tasks,
                                [&uuid](const Task &task) { return task.uuid == uuid; });
}

std::vector<Task>::const_iterator Data::findTaskByUuid(const std::string &uuid) const
{
    return std::ranges::find_if(_data.tasks,
                                [&uuid](const Task &task) { return task.uuid == uuid; });
}

std::vector<Tag>::iterator Data::findTagByName(const std::string &tagName)
{
    return std::ranges::find_if(_data.tags,
                                [&tagName](const Tag &tag) { return tag.name == tagName; });
}

std::vector<Tag>::const_iterator Data::findTagByName(const std::string &tagName) const
{
    return std::ranges::find_if(_data.tags,
                                [&tagName](const Tag &tag) { return tag.name == tagName; });
}
} // namespace pointless::core
