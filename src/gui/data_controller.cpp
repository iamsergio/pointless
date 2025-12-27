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

std::expected<pointless::core::Data, std::string> DataController::pullRemoteData()
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

    return result;
}

std::expected<pointless::core::Data, std::string> DataController::refresh()
{
    P_LOG_DEBUG("Starting refresh");
    auto localDataResult = _localData.loadDataFromFile();
    if (!localDataResult) {
        return std::unexpected("DataController::refresh: Failed to load local data: " + localDataResult.error());
    }

    auto remoteDataResult = pullRemoteData();
    return sync(remoteDataResult ? std::make_optional(*remoteDataResult) : std::nullopt);
}

std::expected<core::Data, std::string> DataController::sync(const std::optional<core::Data> &remoteDataOpt)
{
    core::Data &localData = _localData.data();
    if (!remoteDataOpt.has_value()) {
        // #1. There's no remote data. Reset revision and use local data.
        localData.setRevision(0);
        localData.clearServerSyncBits();
        if (!_localData.save()) {
            return std::unexpected("DataController::sync: Failed to save local data");
        }
        P_LOG_DEBUG("No remote data, using local data");
        return localData;
    }

    const core::Data &remoteData = *remoteDataOpt;
    if (localData.revision() == -1 && localData.isEmpty()) {
        // 2. Local data is empty, use remote data.
        _localData = core::LocalData();

        auto saveResult = _localData.setDataAndSave(remoteData);
        if (!saveResult) {
            return std::unexpected("DataController::sync: Failed to save local data: " + saveResult.error());
        }
        P_LOG_DEBUG("Local data was empty, replaced with remote data");
        return remoteData;
    }

    if (localData.revision() > remoteData.revision()) {
        // 3. Doesn't happen, local data never increments revision
        P_LOG_WARNING("sync(): Incoming has higher revision! incoming.rev={} ; remoteData.rev={}", localData.revision(), remoteData.revision());
        return remoteData;
    }

    P_LOG_DEBUG("Merging local and remote data");
    return remoteData;
}
