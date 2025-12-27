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

std::expected<pointless::core::Data, std::string> DataController::refresh()
{
    auto localDataResult = _localData.loadDataFromFile();
    if (!localDataResult) {
        return std::unexpected("DataController::refresh: Failed to load local data: " + localDataResult.error());
    }

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

    auto remoteData = result.value();

    return sync(remoteData);
}

std::expected<core::Data, std::string> DataController::sync(const core::Data &remoteData)
{
    return remoteData;
}
