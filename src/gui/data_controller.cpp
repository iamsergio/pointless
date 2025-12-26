// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_controller.h"
#include "core/data_provider.h"

DataController::DataController(QObject *parent)
    : QObject(parent)
    , _dataProvider(IDataProvider::createProvider())
{
}

bool DataController::loginWithDefaults()
{
    return _dataProvider && _dataProvider->loginWithDefaults();
}

std::expected<std::string, std::string> DataController::refresh()
{
    if (!_dataProvider->isAuthenticated()) {
        return std::unexpected("DataController::refresh: Not authenticated");
    }

    std::string json_str = _dataProvider->pullData();
    if (json_str.empty()) {
        return std::unexpected("DataController::refresh:No data retrieved");
    }

    return json_str;
}
