// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "date_utils.h"
#include "Clock.h"
#include "../core/Clock.h"
#include "../core/task_manager.h"
#include "../core/logger.h"
#include "../core/data_provider.h"

#include <QTimer>

#include <cstdlib>
#include <QUuid>
#include <QDateTime>


Controller::Controller(QObject *parent)
    : QObject(parent)
    , _dataProvider(IDataProvider::createProvider())
{

#ifdef POINTLESS_DEVELOPER_MODE
    // TODO: A better place to put it ?
    if (!_dataProvider->loginWithDefaults()) {
        P_LOG_CRITICAL("Failed to login with default credentials in developer mode");
        std::abort();
    }
#else
    // TODO: A better place to put it ?
    _dataProvider->loginWithDefaults();
#endif

    _taskModel = TaskModel::instance(this);
    _tagModel = new TagModel(this);
    _taskFilterModel = new TaskFilterModel(this);
    navigatorGotoToday();

    QTimer::singleShot(0, this, &Controller::refresh);
}

bool Controller::isDebug() const
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif
}

void Controller::refresh()
{
    if (!_dataProvider->isAuthenticated()) {
        P_LOG_ERROR("Cannot refresh: not authenticated");
        return;
    }

    std::string json_str = _dataProvider->retrieveData();
    if (json_str.empty()) {
        P_LOG_ERROR("Cannot refresh: no data retrieved");
        return;
    }

    auto result = PointlessCore::TaskManager::fromJson(json_str);
    if (!result) {
        P_LOG_ERROR("Cannot refresh: failed to parse JSON: {}", result.error());

        std::ofstream debugFile("/tmp/debug.json");
        if (debugFile.is_open()) {
            debugFile << json_str;
            debugFile.close();
        }

        return;
    }

    auto &manager = result.value();
    _taskModel->setTasks(manager.getAllTasks());
    _tagModel->setTags(manager.getAllTags());
}

TaskFilterModel *Controller::taskFilterModel() const
{
    return _taskFilterModel;
}


Controller::ViewType Controller::currentViewType() const
{
    return _currentViewType;
}

QDate Controller::navigatorStartDate() const
{
    return _navigatorStartDate;
}

QDate Controller::navigatorEndDate() const
{
    return _navigatorStartDate.addDays(6);
}

QString Controller::navigatorPrettyStartDate() const
{
    return _navigatorStartDate.toString("MMM d");
}

QString Controller::navigatorPrettyEndDate() const
{
    return navigatorEndDate().toString("MMM d");
}

void Controller::setNavigatorStartDate(const QDate &date)
{
    if (_navigatorStartDate == date) {
        return;
    }
    _navigatorStartDate = date;
    emit navigatorStartDateChanged();
    emit navigatorEndDateChanged();
}

void Controller::navigatorGotoToday()
{
    QDate today = Gui::Clock::today();
    QDate monday = Gui::DateUtils::firstMondayOfWeek(today);
    P_LOG_INFO("Today: {}, First Monday: {}", today.toString(Qt::ISODate).toStdString(), monday.toString(Qt::ISODate).toStdString());
    setNavigatorStartDate(Gui::DateUtils::firstMondayOfWeek(Gui::Clock::today()));
}

void Controller::navigatorGotoNextWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(7));
}

void Controller::navigatorGotoPreviousWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(-7));
}

void Controller::setCurrentViewType(ViewType viewType)
{
    if (_currentViewType == viewType) {
        return;
    }

    _currentViewType = viewType;
    emit currentViewTypeChanged();
}

bool Controller::isMobile() const
{
    return isAndroid() || isIOS();
}

bool Controller::isAndroid() const
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool Controller::isIOS() const
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

QString Controller::uuidBeingEdited() const
{
    return _uuidBeingEdited;
}

void Controller::setUuidBeingEdited(const QString &uuid)
{
    if (_uuidBeingEdited == uuid) {
        return;
    }

    _uuidBeingEdited = uuid;
    emit uuidBeingEditedChanged();
}

bool Controller::isEditing() const
{
    return _isEditing;
}

void Controller::setIsEditing(bool isEditing)
{
    if (_isEditing == isEditing) {
        return;
    }

    _isEditing = isEditing;
    emit isEditingChanged();
}

void Controller::addNewTask(const QString &title)
{
    if (title.isEmpty()) {
        return;
    }

    PointlessCore::Task task;
    task.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    task.title = title.toStdString();
    task.creationTimestamp = PointlessCore::Clock::now();
    task.modificationTimestamp = task.creationTimestamp;

    _taskModel->addTask(task);
}

QString Controller::colorFromTag(const QString &tagName) const
{
    // TODO: Store color per tag in the database
    static const QStringList colors = {
        "#2ECC71",
        "#3498DB",
        "#9B5934",
        "#FF8A00"
    };
    if (tagName.isEmpty())
        return "#555555";
    int index = qAbs(qHash(tagName)) % colors.size();
    return colors.at(index);
}

bool Controller::isVerbose() const
{
    static bool is = std::getenv("POINTLESS_IS_VERBOSE") != nullptr;
    return is;
}

void Controller::dumpTaskDebug(const QString &taskUuid) const
{
    const auto *task = _taskModel->taskForUuid(taskUuid);
    if (!task) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid.toStdString());
        return;
    }
    task->dumpDebug();
}
