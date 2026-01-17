// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data.h"
#include "logger.h"

#include <algorithm>

namespace pointless::core {

Data::Data() = default;

void Data::addTask(const Task &task)
{
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

bool Data::updateTask(const Task &task, bool incrementTaskRevision)
{
    auto it = findTaskByUuid(task.uuid);
    if (it != _data.tasks.end()) {
        *it = task;
        if (incrementTaskRevision) {
            it->revision++;
        }
        return true;
    }
    return false;
}

bool Data::setTask(const Task &task)
{
    P_LOG_DEBUG("Setting task '{}' Data={}", task.uuid, static_cast<void *>(this));
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

void Data::addTag(const Tag &tag)
{
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

std::vector<Tag> Data::allTags() const
{
    return _data.tags;
}

bool Data::containsTag(const std::string &tagName) const
{
    return findTagByName(tagName) != _data.tags.end();
}

std::vector<Task> Data::newTasks() const
{
    std::vector<Task> result;
    result.reserve(_data.tasks.size());

    std::ranges::copy_if(_data.tasks, std::back_inserter(result), [](const Task &task) {
        return task.revision == -1;
    });

    return result;
}

std::vector<Task> Data::modifiedTasks() const
{
    std::vector<Task> result;
    result.reserve(_data.tasks.size());

    std::ranges::copy_if(_data.tasks, std::back_inserter(result),
                         [](const Task &task) { return task.needsSyncToServer; });

    return result;
}

const Task &Data::taskAt(size_t index) const
{
    return _data.tasks.at(index);
}

const Task *Data::taskForUuid(const std::string &uuid) const
{
    auto it = std::ranges::find_if(_data.tasks, [&uuid](const Task &task) {
        return task.uuid == uuid;
    });
    if (it != _data.tasks.end()) {
        return &(*it);
    }
    return nullptr;
}

Task *Data::taskForUuid(const std::string &uuid)
{
    auto it = std::ranges::find_if(_data.tasks, [&uuid](const Task &task) {
        return task.uuid == uuid;
    });
    if (it != _data.tasks.end()) {
        return &(*it);
    }
    return nullptr;
}

const Task *Data::taskForTitle(const std::string &title) const
{
    auto it = std::ranges::find_if(_data.tasks, [&title](const Task &task) {
        return task.title == title;
    });
    if (it != _data.tasks.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<Tag> Data::newTags() const
{
    std::vector<Tag> result;
    result.reserve(_data.tags.size());

    std::ranges::copy_if(_data.tags, std::back_inserter(result), [](const Tag &tag) {
        return tag.revision == -1;
    });

    return result;
}

[[nodiscard]] const Tag &Data::tagAt(size_t index) const
{
    return _data.tags.at(index);
}

void Data::clearTags()
{
    _data.tags.clear();
}

size_t Data::tagCount() const
{
    return _data.tags.size();
}

std::vector<Tag> Data::getUsedTags() const
{
    std::vector<Tag> result;
    for (const auto &tag : _data.tags) {
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

void Data::addDeletedTaskUuid(const std::string &uuid)
{
    _data.deletedTaskUuids.push_back(uuid);
}

const std::vector<std::string> &Data::deletedTaskUuids() const
{
    return _data.deletedTaskUuids;
}

void Data::addDeletedTagName(const std::string &tagName)
{
    _data.deletedTagNames.push_back(tagName);
}

const std::vector<std::string> &Data::deletedTagNames() const
{
    return _data.deletedTagNames;
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

std::expected<std::string, std::string> Data::toJson() const
{
    std::string buffer;
    auto result = glz::write<glz::opts {
        .skip_null_members = false,
    }>(_data, buffer);

    if (result) {
        return std::unexpected("Failed to serialize to JSON");
    }

    return buffer;
}

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

void Data::clearServerSyncBits()
{
    for (auto &task : _data.tasks) {
        if (task.revision == -1)
            task.revision = 0;
        task.needsSyncToServer = false;
    }
    for (auto &tag : _data.tags) {
        if (tag.revision == -1)
            tag.revision = 0;
        tag.needsSyncToServer = false;
    }

    _data.deletedTagNames.clear();
    _data.deletedTaskUuids.clear();
    needsUpload = false;
    needsLocalSave = false;
}

void Data::setRevision(int revision)
{
    _data.revision = revision;
}

int Data::revision() const
{
    return _data.revision;
}

bool Data::isEmpty() const
{
    return _data.tasks.empty() && _data.tags.empty();
}

bool Data::isValid() const
{
    return revision() != -1 || !isEmpty();
}

std::string Data::debug_taskUids() const
{
    std::string result;
    for (const auto &task : _data.tasks) {
        result += task.uuid + " ";
    }
    return result;
}

}
