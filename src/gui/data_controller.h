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

class DataController : public QObject
{
    Q_OBJECT
public:
    explicit DataController(QObject *parent = nullptr);

    bool loginWithDefaults();
    std::expected<pointless::core::Data, std::string> refresh();

private:
    pointless::core::LocalData _localData;
    std::unique_ptr<IDataProvider> _dataProvider;
};
