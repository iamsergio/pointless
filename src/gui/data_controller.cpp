// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_controller.h"
#include "taskmodel.h"
#include "tagmodel.h"

#include "core/data_provider.h"
#include "core/logger.h"
#include "core/context.h"
#include "utils.h"
#include "fatal_message_handler.h"

#include <QtConcurrent/QtConcurrent>

#include <fstream>
#include <chrono>
#include <format>

/// Example of running a single test:
/// ./bin/test_data_controller --gtest_filter=DataControllerTest.MergeNeedsLocalSave

using namespace pointless;

DataController::DataController(QObject *parent)
    : QObject(parent)
    , _dataProvider(IDataProvider::createProvider())
    , _taskModel(new TaskModel(this))
    , _tagModel(new TagModel(this))
    , _refreshWatcher(new QFutureWatcher<std::expected<core::Data, TraceableError>>(this))
    , _loginWatcher(new QFutureWatcher<bool>(this))
{
    qInstallMessageHandler(pointless::gui::qtMessageHandler);

    _saveToDiskTimer.setInterval(std::chrono::seconds(1));
    _saveToDiskTimer.setSingleShot(true);
    connect(&_saveToDiskTimer, &QTimer::timeout, this, [this] {
        if (!_localData.data().needsLocalSave) {
            return;
        }

        auto saveResult = _localData.save();
        if (!saveResult) {
            P_LOG_ERROR("Failed to save data to disk: {}", saveResult.error());
        } else {
            P_LOG_DEBUG("Data saved to disk");
        }
    });

    _tokenCheckTimer.setInterval(std::chrono::minutes(5));
    _tokenCheckTimer.setSingleShot(false);
    connect(&_tokenCheckTimer, &QTimer::timeout, this, [this] {
        const auto currentAccess = accessToken();
        const auto currentRefresh = refreshToken();
        bool changed = false;

        if (!currentAccess.empty() && currentAccess != _localSettings.accessToken()) {
            _localSettings.setAccessToken(currentAccess);
            changed = true;
        }

        if (!currentRefresh.empty() && currentRefresh != _localSettings.refreshToken()) {
            _localSettings.setRefreshToken(currentRefresh);
            changed = true;
        }

        if (changed) {
            _localSettings.save();
            P_LOG_DEBUG("Tokens updated from polling");
        }
    });
    _tokenCheckTimer.start();

    connect(_refreshWatcher, &QFutureWatcherBase::finished, this, [this] {
        _isRefreshing = false;

        auto result = _refreshWatcher->result();
        if (result) {
            P_LOG_INFO("Async refresh completed successfully");
            Q_EMIT refreshFinished(true, QString());
        } else {
            P_LOG_ERROR("Async refresh failed: {}", result.error().toString());
            Q_EMIT refreshFinished(false, QString::fromStdString(result.error().toString()));
        }

        // Reload models on MAIN thread
        _taskModel->reload();
        _tagModel->reload();
    });

    connect(_loginWatcher, &QFutureWatcherBase::finished, this, [this] {
        _isLoggingIn = false;
        bool success = _loginWatcher->result();
        if (success) {
            saveAuth();
            P_LOG_DEBUG("Login successful, saved credentials");
        }
        Q_EMIT isAuthenticatedChanged();
        Q_EMIT loginFinished(success);
    });

    if (core::Context::self().shouldRestoreAuth()) {
        restoreAuth();
    }

    if (Gui::isAutoLogin() && !isAuthenticated()) {
        if (!loginWithDefaults()) {
            pointless::abort("Failed to login with default credentials in developer mode");
        }
        Q_EMIT isAuthenticatedChanged();
    }
}

bool DataController::loginWithDefaults()
{
    if (!_dataProvider) {
        return false;
    }

    auto [username, password] = _dataProvider->defaultLoginPassword();
    if (username.empty() || password.empty()) {
        P_LOG_WARNING("No default credentials available");
        return false;
    }

    if (performLoginSync(username, password)) {
        saveAuth();
        P_LOG_DEBUG("Login successful, saved credentials");
        Q_EMIT isAuthenticatedChanged();
        return true;
    }

    Q_EMIT isAuthenticatedChanged();
    return false;
}

void DataController::login(const std::string &email, const std::string &password)
{
    bool expected = false;
    if (!_isLoggingIn.compare_exchange_strong(expected, true)) {
        P_LOG_WARNING("Login already in progress");
        return;
    }

    Q_EMIT loginStarted();
    QFuture<bool> future = QtConcurrent::run(&DataController::performLoginSync, this, email, password);
    _loginWatcher->setFuture(future);
}

bool DataController::performLoginSync(const std::string &email, const std::string &password)
{
    return _dataProvider && _dataProvider->login(email, password);
}

void DataController::saveAuth()
{
    if (accessToken().empty() || userId().empty() || refreshToken().empty()) {
        P_LOG_INFO("No tokens to save");
        return;
    }

    P_LOG_INFO("Saving authentication tokens");
    _localSettings.setAccessToken(accessToken());
    _localSettings.setUserId(userId());
    _localSettings.setRefreshToken(refreshToken());
    _localSettings.save();
}

bool DataController::restoreAuth()
{
    const auto savedToken = _localSettings.accessToken();
    const auto savedUserId = _localSettings.userId();

    if (!savedToken.empty() && !savedUserId.empty()) {
        setAccessToken(savedToken);
        setUserId(savedUserId);
        const auto savedRefreshToken = _localSettings.refreshToken();
        if (!savedRefreshToken.empty()) {
            setRefreshToken(savedRefreshToken);
        }
        P_LOG_DEBUG("Loaded saved authentication token");
        Q_EMIT isAuthenticatedChanged();
        return true;
    }

    return false;
}

std::string DataController::defaultLoginUsername() const
{
    return _dataProvider ? _dataProvider->defaultLoginPassword().first : std::string {};
}

void DataController::logout()
{
    if (_dataProvider) {
        _dataProvider->logout();
        _localSettings.clear();
        Q_EMIT isAuthenticatedChanged();
    } else {
        P_LOG_INFO("No data provider, nothing to logout");
    }
}

bool DataController::isAuthenticated() const
{
    return _dataProvider && _dataProvider->isAuthenticated();
}

std::string DataController::accessToken() const
{
    return _dataProvider ? _dataProvider->accessToken() : std::string {};
}

std::string DataController::refreshToken() const
{
    return _dataProvider ? _dataProvider->refreshToken() : std::string {};
}

std::string DataController::userId() const
{
    return _dataProvider ? _dataProvider->userId() : std::string {};
}

void DataController::setAccessToken(const std::string &token)
{
    if (_dataProvider) {
        _dataProvider->setAccessToken(token);
    }
}

void DataController::setRefreshToken(const std::string &token)
{
    if (_dataProvider) {
        _dataProvider->setRefreshToken(token);
    }
}

void DataController::setUserId(const std::string &userId)
{
    if (_dataProvider) {
        _dataProvider->setUserId(userId);
    }
}

bool DataController::updateTask(const core::Task &task)
{
    if (_localData.updateTask(task)) {
        _saveToDiskTimer.start();
        return true;
    }
    return false;
}

bool DataController::addTask(const pointless::core::Task &task)
{
    if (_localData.addTask(task)) {
        _saveToDiskTimer.start();
        return true;
    }
    return false;
}

std::expected<core::Data, TraceableError> DataController::pullRemoteData()
{
    if (!_dataProvider->isAuthenticated()) {
        Q_EMIT isAuthenticatedChanged();
        return TraceableError::create("DataController::pullRemoteData: Not authenticated");
    }

    std::expected<std::string, TraceableError> json_str_expr = _dataProvider->pullData();
    if (!json_str_expr) {
        return TraceableError::create("DataController::pullRemoteData", json_str_expr.error());
    }
    const std::string &json_str = *json_str_expr;

    auto result = core::Data::fromJson(json_str);
    if (!result) {
        P_LOG_ERROR("failed to parse JSON: {}", result.error());
#ifdef POINTLESS_DEVELOPER_MODE
        std::ofstream debugFile("/tmp/debug.json");
        if (debugFile.is_open()) {
            debugFile << json_str;
            debugFile.close();
        }
#endif
        return TraceableError::create("failed to parse JSON: " + result.error());
    }

    // in case it got to the server somehow
    result->clearServerSyncBits();

    return *result;
}

std::expected<core::Data, TraceableError> DataController::pushRemoteData(core::Data data)
{
    data.clearServerSyncBits();
    data.setRevision(data.revision() + 1);

    if (!_dataProvider->isAuthenticated()) {
        return TraceableError::create("Not authenticated");
    }

    auto jsonStrResult = data.toJson();
    if (!jsonStrResult) {
        return TraceableError::create("Failed to serialize data to JSON: " + jsonStrResult.error());
    }

    const auto &jsonStr = jsonStrResult.value();
    auto result = _dataProvider->pushData(jsonStr);
    if (!result) {
        return TraceableError::create("Failed to push data to remote", result.error());
    }

    P_LOG_INFO("Data pushed to remote successfully {} bytes", jsonStr.size());
    return data;
}

std::expected<void, TraceableError> DataController::refresh(bool isOfflineMode)
{
    // Concurrency control: Don't allow multiple simultaneous refreshes
    bool expected = false;
    if (!_isRefreshing.compare_exchange_strong(expected, true)) {
        P_LOG_WARNING("Refresh already in progress, ignoring duplicate request");
        return TraceableError::create("Refresh already in progress");
    }

    // Load local data on MAIN thread before launching background task
    // This prevents race conditions with updateTask()/addTask() accessing _localData
    if (!_localData.data().isValid()) {
        auto localDataResult = _localData.loadDataFromFile();
        if (!localDataResult) {
            _isRefreshing = false;
            return TraceableError::create("Failed to load local data", localDataResult.error());
        }
    }

    if (isOfflineMode) {
        _taskModel->reload();
        _tagModel->reload();
        _isRefreshing = false;
        return {};
    }

    if (!isAuthenticated()) {
        // not authed and not offline mode, nothing to do
        _isRefreshing = false;
        return {};
    }

    Q_EMIT refreshStarted();

    // Launch async refresh
    QFuture<std::expected<core::Data, TraceableError>> future =
        QtConcurrent::run(&DataController::performRefreshInBackground, this);

    _refreshWatcher->setFuture(future);

    // Return immediately - actual result comes via signal
    return {};
}

#ifdef POINTLESS_ENABLE_TESTS
std::expected<core::Data, TraceableError> DataController::refreshBlocking()
{
    P_LOG_INFO("Starting blocking refresh for tests");

    // Load data on main thread if needed (for thread safety consistency)
    if (!_localData.data().isValid()) {
        auto localDataResult = _localData.loadDataFromFile();
        if (!localDataResult) {
            return TraceableError::create("Failed to load local data", localDataResult.error());
        }
    }

    Q_EMIT refreshStarted();

    // Call the background method directly (synchronously)
    auto result = performRefreshInBackground();

    // Reload models on main thread
    if (result) {
        _taskModel->reload();
        _tagModel->reload();
        Q_EMIT refreshFinished(true, {});
    } else {
        Q_EMIT refreshFinished(false, QStringLiteral("Blocking refresh failed: ") + QString::fromStdString(result.error().toString()));
    }

    return result;
}

bool DataController::loginBlocking(const std::string &email, const std::string &password)
{
    P_LOG_INFO("Starting blocking login for tests");
    Q_EMIT loginStarted();
    bool success = performLoginSync(email, password);
    if (success) {
        saveAuth();
        P_LOG_DEBUG("Login successful, saved credentials");
    }
    Q_EMIT isAuthenticatedChanged();
    Q_EMIT loginFinished(success);
    return success;
}
#endif

std::expected<core::Data, TraceableError> DataController::performRefreshInBackground()
{
    // This runs in a BACKGROUND thread
    // Do NOT touch Qt models or QML-exposed objects here!

    P_LOG_INFO("Starting async refresh in background thread");

    // Network operations (safe in background thread)
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
            return TraceableError::create("Failed to push remote data", pushResult.error());
        }
    }

    if (needsLocalSave) {
        auto saveResult = _localData.setDataAndSave(mergedData);
        if (!saveResult) {
            return TraceableError::create("Failed to save local data: " + saveResult.error());
        }
    }

    // Model reload happens in QFutureWatcher::finished() on MAIN thread
    return mergedData;
}

std::expected<core::Data, TraceableError> DataController::merge(const std::optional<core::Data> &remoteDataOpt)
{
    core::Data &localData = _localData.data();
    P_LOG_INFO("local.numTasks={}, local.revision={}, local.numModifiedTasks={}, local.numDeletedTasks={}, remoteData.has_value={}",
               localData.taskCount(), localData.revision(), localData.modifiedTasks().size(), localData._data.deletedTaskUuids.size(), remoteDataOpt.has_value());

    if (!remoteDataOpt.has_value()) {
        // #1. There's no remote data. Reset revision and use local data.
        localData.setRevision(0);
        localData.clearServerSyncBits();
        localData.needsUpload = true;
        localData.needsLocalSave = true;

        P_LOG_INFO("No remote data, using local data");
        return localData;
    }

    core::Data remoteData = *remoteDataOpt;

    P_LOG_INFO("Remote data has numTasks={}, revision={}",
               remoteData.taskCount(), remoteData.revision());

    if (localData.revision() == -1 && localData.isEmpty()) {
        // 2. Local data is empty, use remote data.
        P_LOG_INFO("Local data was empty, replaced with remote data");
        remoteData.needsLocalSave = true;
        return remoteData;
    }

    if (localData.revision() > remoteData.revision()) {
        // 3. Doesn't happen, local data never increments revision
        P_LOG_CRITICAL("Local has higher revision! local.rev={} ; remote.rev={}", localData.revision(), remoteData.revision());

        // Avoid potential data loss
        std::abort();
    }

    if (localData.revision() < remoteData.revision()) {
        P_LOG_INFO("Local data revision behind remote, replaced with remote data");
        remoteData.needsLocalSave = true;
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
                // 6.1 task was only changed locally, use the local version
                remoteData.updateTask(localTask, /*incrementTaskRevision=*/true);
                remoteData.needsLocalSave = true;
                remoteData.needsUpload = true;
                P_LOG_DEBUG("Updated modified task '{}' in remote data", localTask.title);
            } else if (remoteTask.revision > localTask.revision) {
                // 6.2 task was changed both locally and remotely, merge
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
            P_LOG_INFO("Modified task uuid='{}' not found in remote data, skipping", localTask.uuid);
            P_LOG_DEBUG("Known remote tasks: {}", remoteData.debug_taskUids());
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

    P_LOG_DEBUG("Merged local and remote data. newData.numTasks={}, newData.revision={}, newData.needsLocalSave={}",
                remoteData.taskCount(), remoteData.revision(), remoteData.needsLocalSave);
    return remoteData;
}

core::LocalData &DataController::localData()
{
    return _localData;
}

LocalSettings &DataController::localSettings()
{
    return _localSettings;
}

TaskModel *DataController::taskModel() const
{
    return _taskModel;
}

TagModel *DataController::tagModel() const
{
    return _tagModel;
}

int DataController::taskCountForTag(const QString &tagName) const
{
    const std::string name = tagName.toStdString();
    int count = 0;
    for (size_t i = 0; i < _localData.taskCount(); ++i) {
        if (_localData.taskAt(i).containsTag(name))
            ++count;
    }
    return count;
}

void DataController::scheduleSave()
{
    _saveToDiskTimer.start();
}
