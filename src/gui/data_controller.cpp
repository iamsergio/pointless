// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_controller.h"
#include "core/data_provider.h"
#include "core/logger.h"

#include <fstream>

using namespace pointless;

DataController::DataController(QObject *parent)
    : QObject(parent)
    , _dataProvider(IDataProvider::createProvider())
{
}

bool DataController::loginWithDefaults()
{
    return _dataProvider && _dataProvider->loginWithDefaults();
}

std::expected<core::Data, std::string> DataController::pullRemoteData()
{
    if (!_dataProvider->isAuthenticated()) {
        return std::unexpected("DataController::refresh: Not authenticated");
    }

    std::string json_str = _dataProvider->pullData();
    if (json_str.empty()) {
        return std::unexpected("DataController::refresh: No data retrieved");
    }

    auto result = core::Data::fromJson(json_str);
    if (!result) {
        P_LOG_ERROR("Cannot refresh: failed to parse JSON: {}", result.error());
#ifdef POINTLESS_DEVELOPER_MODE
        std::ofstream debugFile("/tmp/debug.json");
        if (debugFile.is_open()) {
            debugFile << json_str;
            debugFile.close();
        }
#endif
        return std::unexpected("Cannot refresh: failed to parse JSON: " + result.error());
    }

    // in case it got to the server somehow
    result->clearServerSyncBits();

    return result;
}

std::expected<core::Data, std::string> DataController::pushRemoteData(core::Data data)
{
    data.clearServerSyncBits();
    data.setRevision(data.revision() + 1);

    if (!_dataProvider->isAuthenticated()) {
        return std::unexpected("DataController::refresh: Not authenticated");
    }

    auto jsonStrResult = data.toJson();
    if (!jsonStrResult) {
        return std::unexpected("DataController::pushRemoteData: Failed to serialize data to JSON: " + jsonStrResult.error());
    }

    const auto &jsonStr = jsonStrResult.value();
    if (!_dataProvider->pushData(jsonStr)) {
        return std::unexpected("DataController::pushRemoteData: Failed to push data to remote");
    }

    P_LOG_INFO("Data pushed to remote successfully {} bytes", jsonStr.size());
    return data;
}

std::expected<core::Data, std::string> DataController::refresh()
{
    P_LOG_DEBUG("Starting refresh");
    if (!_localData.data().isValid()) {
        // The 1st time we load local data. then it stays in memory and we don't load from disk again
        // we just save to disk
        auto localDataResult = _localData.loadDataFromFile();
        if (!localDataResult) {
            return std::unexpected("DataController::refresh: Failed to load local data: " + localDataResult.error());
        }
    }

    auto remoteDataResult = pullRemoteData();
    auto mergedResult = merge(remoteDataResult ? std::make_optional(*remoteDataResult) : std::nullopt);
    if (!mergedResult) {
        return mergedResult;
    }

    auto mergedData = *mergedResult;

    const bool needsLocalSave = mergedData.needsLocalSave; // since it's overwritten by push

    if (mergedData.needsUpload) {
        auto pushResult = pushRemoteData(mergedData);
        if (pushResult) {
            mergedData = *pushResult;
        } else {
            return std::unexpected("DataController::sync: Failed to push remote data: " + pushResult.error());
        }
    }

    if (needsLocalSave) {
        auto saveResult = _localData.setDataAndSave(mergedData);
        if (!saveResult) {
            return std::unexpected("DataController::sync: Failed to save local data: " + saveResult.error());
        }
    }

    return mergedData;
}

std::expected<core::Data, std::string> DataController::merge(const std::optional<core::Data> &remoteDataOpt)
{
    core::Data &localData = _localData.data();
    if (!remoteDataOpt.has_value()) {
        // #1. There's no remote data. Reset revision and use local data.
        localData.setRevision(0);
        localData.clearServerSyncBits();
        localData.needsUpload = true;
        localData.needsLocalSave = true;

        P_LOG_DEBUG("No remote data, using local data");
        return localData;
    }

    core::Data remoteData = *remoteDataOpt;
    if (localData.revision() == -1 && localData.isEmpty()) {
        // 2. Local data is empty, use remote data.
        P_LOG_DEBUG("Local data was empty, replaced with remote data");
        remoteData.needsLocalSave = true;
        return remoteData;
    }

    if (localData.revision() > remoteData.revision()) {
        // 3. Doesn't happen, local data never increments revision
        P_LOG_WARNING("sync(): Incoming has higher revision! incoming.rev={} ; remoteData.rev={}", localData.revision(), remoteData.revision());
        remoteData.needsLocalSave = true;
        return remoteData;
    }

    // 4. Add new tags
    auto newTags = localData.newTags();
    for (const auto &newLocalTag : newTags) {
        const auto tagName = newLocalTag.name;
        if (!remoteData.containsTag(tagName)) {
            core::Tag newTag;
            newTag.name = tagName;
            newTag.revision = 0;
            newTag.needsSyncToServer = false;
            remoteData.addTag(newTag);
            remoteData.needsLocalSave = true;
            remoteData.needsUpload = true;
            P_LOG_DEBUG("Added new tag '{}' to remote data", tagName);
        }
    }

    // 5. Add new tasks
    auto newTasks = localData.newTasks();
    for (const auto &newLocalTask : newTasks) {
        if (!remoteData.getTask(newLocalTask.uuid)) {
            core::Task newTask = newLocalTask;
            newTask.revision = 0;
            newTask.needsSyncToServer = false;
            remoteData.addTask(newTask);
            remoteData.needsLocalSave = true;
            remoteData.needsUpload = true;
            P_LOG_DEBUG("Added new task '{}' to remote data", newTask.title);
        }
    }

    // 6. Merge modified tasks
    auto modifiedTasks = localData.modifiedTasks();
    for (auto &localTask : modifiedTasks) {
        auto remoteTaskOpt = remoteData.getTask(localTask.uuid);
        localTask.needsSyncToServer = false;
        if (remoteTaskOpt) {
            auto remoteTask = *remoteTaskOpt;
            if (remoteTask.revision == localTask.revision) {
                remoteData.updateTask(localTask, /*incrementTaskRevision=*/true);
                remoteData.needsLocalSave = true;
                remoteData.needsUpload = true;
                P_LOG_DEBUG("Updated modified task '{}' in remote data", localTask.title);
            } else if (remoteTask.revision > localTask.revision) {
                remoteTask.mergeConflict(localTask);
                remoteData.updateTask(remoteTask, /*incrementTaskRevision=*/true);
                remoteData.needsLocalSave = true;
                remoteData.needsUpload = true;
            } else {
                P_LOG_WARNING("Ignoring local task '{}' with higher revision than remote (local.rev={} ; remote.rev={})",
                              localTask.uuid, localTask.revision, remoteTask.revision);
            }
        } else {
            // It was deleted by another client, it's fine
            P_LOG_INFO("Modified task '{}' not found in remote data, skipping", localTask.uuid);
        }
    }

    // 7. deletedTasks
    for (const auto &deletedTaskUuid : localData._data.deletedTaskUuids) {
        if (remoteData.removeTask(deletedTaskUuid)) {
            remoteData.needsLocalSave = true;
            remoteData.needsUpload = true;
            P_LOG_DEBUG("Deleted task '{}' from remote data", deletedTaskUuid);
        }
    }

    // 8. deleteTags
    for (const auto &deletedTagName : localData._data.deletedTagNames) {
        if (remoteData.removeTag(deletedTagName)) {
            remoteData.needsLocalSave = true;
            remoteData.needsUpload = true;
            P_LOG_DEBUG("Deleted tag '{}' from remote data", deletedTagName);
        }
    }

    P_LOG_DEBUG("Merged local and remote data");
    return remoteData;
}
