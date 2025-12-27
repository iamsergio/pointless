// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

/// Takes care of controlling the remote source via IDataProvider
/// and merging it with LocalData

#pragma once

#include "core/local_data.h"
#include "core/data_provider.h"
#include "core/data.h"

#include <QObject>

#include <expected>
#include <optional>

class DataController : public QObject
{
    Q_OBJECT
public:
    explicit DataController(QObject *parent = nullptr);

    bool loginWithDefaults();
    std::expected<pointless::core::Data, std::string> refresh();

private:
    std::expected<pointless::core::Data, std::string> pullRemoteData();
    std::expected<pointless::core::Data, std::string> sync(const std::optional<pointless::core::Data> &remoteData);
    pointless::core::LocalData _localData;
    std::unique_ptr<IDataProvider> _dataProvider;
};
