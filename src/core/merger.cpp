// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "merger.h"
#include "logger.h"

#include <algorithm>

namespace pointless::core {

DataPayload merge(const DataPayload &localData, const DataPayload &serverData)
{
    // If server is empty, client initializes it.
    if (serverData.tasks.empty() && serverData.tags.empty() && serverData.revision == 0) {
        DataPayload result = localData;
        result.revision = 0;
        for (auto &task : result.tasks) {
            task.needsSyncToServer = false;
        }
        for (auto &tag : result.tags) {
            tag.needsSyncToServer = false;
        }
        return result;
    }

    // If client is empty/new, it gets server data.
    if (localData.revision == -1 && localData.tasks.empty() && localData.tags.empty()) {
        return serverData;
    }

    // If client is ahead of server? "Doesn't happen".
    if (localData.revision > serverData.revision) {
        P_LOG_WARNING("Client revision {} > Server revision {}. Ignoring.", localData.revision, serverData.revision);
        return serverData;
    }

    DataPayload current = serverData;
    bool changed = false;

    // Add new tags
    for (const auto &tag : localData.tags) {
        bool exists = std::ranges::any_of(current.tags, [&](const Tag &t) {
            return t.name == tag.name;
        });

        if (!exists) {
            Tag newTag = tag;
            newTag.revision = 0;
            newTag.needsSyncToServer = false;
            current.tags.push_back(newTag);
            changed = true;
        }
    }

    // Tasks
    for (const auto &incomingTask : localData.tasks) {
        if (!incomingTask.needsSyncToServer) {
            continue;
        }

        auto it = std::ranges::find_if(current.tasks, [&](const Task &t) {
            return t.uuid == incomingTask.uuid;
        });

        if (it == current.tasks.end()) {
            // Not in server
            if (incomingTask.revision <= 0) {
                // New task
                Task newTask = incomingTask;
                newTask.revision = 0;
                newTask.needsSyncToServer = false;
                current.tasks.push_back(newTask);
                changed = true;
            }
            // Else: deleted on server, ignore.
        } else {
            // Exists in server
            Task &existingTask = *it;

            if (existingTask.revision == incomingTask.revision) {
                // Fast-forward
                existingTask = incomingTask;
                existingTask.revision++;
                existingTask.needsSyncToServer = false;
                changed = true;
            } else if (existingTask.revision > incomingTask.revision) {
                // Conflict
                existingTask.mergeConflict(incomingTask);
                existingTask.revision++;
                existingTask.needsSyncToServer = false;
                changed = true;
            } else {
                P_LOG_WARNING("Task {} has greater revision on client ({}) than server ({})", incomingTask.title, incomingTask.revision, existingTask.revision);
            }
        }
    }

    // Deleted tasks
    for (const auto &uuid : localData.deletedTaskUuids) {
        auto it = std::ranges::find_if(current.tasks, [&](const Task &t) {
            return t.uuid == uuid;
        });
        if (it != current.tasks.end()) {
            current.tasks.erase(it);
            changed = true;
        }
    }

    // Deleted tags
    for (const auto &tagName : localData.deletedTagNames) {
        auto it = std::ranges::find_if(current.tags, [&](const Tag &t) {
            return t.name == tagName;
        });
        if (it != current.tags.end()) {
            current.tags.erase(it);
            changed = true;
        }
    }

    if (changed) {
        current.revision++;
        for (auto &t : current.tasks) {
            t.needsSyncToServer = false;
        }
        for (auto &t : current.tags) {
            t.needsSyncToServer = false;
        }
    }

    return current;
}

} // namespace pointless::core
