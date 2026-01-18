// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

/// Takes care of controlling the remote source via IDataProvider
/// and merging it with LocalData

#pragma once

#include "core/local_data.h"
#include "core/data_provider.h"
#include "core/data.h"
#include "core/error.h"

#include <QObject>
#include <QTimer>

#include <expected>
#include <optional>

class TaskModel;
class TagModel;

class DataController : public QObject
{
    Q_OBJECT
public:
    explicit DataController(QObject *parent = nullptr);
    ~DataController() override = default;

    bool loginWithDefaults();
    std::expected<pointless::core::Data, TraceableError> refresh();
    bool updateTask(const pointless::core::Task &task);
    bool addTask(const pointless::core::Task &task);

    pointless::core::LocalData &localData();

    [[nodiscard]] TaskModel *taskModel() const;
    [[nodiscard]] TagModel *tagModel() const;

    DataController(const DataController &) = delete;
    DataController &operator=(const DataController &) = delete;
    DataController(DataController &&) = delete;
    DataController &operator=(DataController &&) = delete;

#ifndef POINTLESS_ENABLE_TESTS
private:
#endif
    std::expected<pointless::core::Data, TraceableError> pushRemoteData(pointless::core::Data data);
    std::expected<pointless::core::Data, TraceableError> pullRemoteData();
    std::expected<pointless::core::Data, TraceableError> merge(const std::optional<pointless::core::Data> &remoteData);
    pointless::core::LocalData _localData;
    std::unique_ptr<IDataProvider> _dataProvider;
    TaskModel *_taskModel = nullptr;
    TagModel *_tagModel = nullptr;
    QTimer _saveToDiskTimer;
};
