// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "gui_controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "date_utils.h"
#include "data_controller.h"
#include "Clock.h"

#include "core/Clock.h"
#include "core/data.h"
#include "core/logger.h"
#include "core/task.h"

#include <QTimer>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QUuid>
#include <QDateTime>

#include <cstdlib>
#include <fstream>
#include <string>

GuiController::GuiController(QObject *parent)
    : QObject(parent)
    , _dataController(new DataController(this))
    , _taskModel(TaskModel::instance(this))
    , _tagModel(new TagModel(this))
    , _taskFilterModel(new TaskFilterModel(this))
{

#ifdef POINTLESS_DEVELOPER_MODE
    // TODO: A better place to put it ?
    if (!_dataController->loginWithDefaults()) {
        P_LOG_CRITICAL("Failed to login with default credentials in developer mode");
        std::abort();
    }
#else
    // TODO: A better place to put it ?
    _dataController->loginWithDefaults();
#endif

    navigatorGotoToday();

    QTimer::singleShot(0, this, &GuiController::refresh);
}

bool GuiController::isDebug()
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif
}

void GuiController::refresh()
{
    auto refreshResult = _dataController->refresh();
    if (!refreshResult) {
        P_LOG_ERROR("GuiController::refresh: {}", refreshResult.error());
        return;
    }

    const std::string json_str = refreshResult.value();

    auto result = pointless::core::Data::fromJson(json_str);
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

TaskFilterModel *GuiController::taskFilterModel() const
{
    return _taskFilterModel;
}


GuiController::ViewType GuiController::currentViewType() const
{
    return _currentViewType;
}

QDate GuiController::navigatorStartDate() const
{
    return _navigatorStartDate;
}

QDate GuiController::navigatorEndDate() const
{
    return _navigatorStartDate.addDays(6);
}

QString GuiController::navigatorPrettyStartDate() const
{
    return _navigatorStartDate.toString(QStringLiteral("MMM d"));
}

QString GuiController::navigatorPrettyEndDate() const
{
    return navigatorEndDate().toString(QStringLiteral("MMM d"));
}

void GuiController::setNavigatorStartDate(QDate date)
{
    if (_navigatorStartDate == date) {
        return;
    }
    _navigatorStartDate = date;
    emit navigatorStartDateChanged();
    emit navigatorEndDateChanged();
}

void GuiController::navigatorGotoToday()
{
    QDate today = Gui::Clock::today();
    QDate monday = Gui::DateUtils::firstMondayOfWeek(today);
    P_LOG_INFO("Today: {}, First Monday: {}", today.toString(Qt::ISODate).toStdString(), monday.toString(Qt::ISODate).toStdString());
    setNavigatorStartDate(Gui::DateUtils::firstMondayOfWeek(Gui::Clock::today()));
}

void GuiController::navigatorGotoNextWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(7));
}

void GuiController::navigatorGotoPreviousWeek()
{
    setNavigatorStartDate(_navigatorStartDate.addDays(-7));
}

void GuiController::setCurrentViewType(ViewType viewType)
{
    if (_currentViewType == viewType) {
        return;
    }

    _currentViewType = viewType;
    emit currentViewTypeChanged();
}

bool GuiController::isMobile()
{
    return isAndroid() || isIOS();
}

bool GuiController::isAndroid()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool GuiController::isIOS()
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

QString GuiController::uuidBeingEdited() const
{
    return _uuidBeingEdited;
}

void GuiController::setUuidBeingEdited(const QString &uuid)
{
    if (_uuidBeingEdited == uuid) {
        return;
    }

    _uuidBeingEdited = uuid;
    emit uuidBeingEditedChanged();
}

bool GuiController::isEditing() const
{
    return _isEditing;
}

void GuiController::setIsEditing(bool isEditing)
{
    if (_isEditing == isEditing) {
        return;
    }

    _isEditing = isEditing;
    emit isEditingChanged();
}

void GuiController::addNewTask(const QString &title)
{
    if (title.isEmpty()) {
        return;
    }

    pointless::core::Task task;
    task.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    task.title = title.toStdString();
    task.creationTimestamp = pointless::core::Clock::now();
    task.modificationTimestamp = task.creationTimestamp;

    _taskModel->addTask(task);
}

QString GuiController::colorFromTag(const QString &tagName) const
{
    // TODO: Store color per tag in the database
    static const QStringList colors = {
        "#2ECC71",
        "#3498DB",
        "#9B5934",
        "#FF8A00"
    };
    if (tagName.isEmpty())
        return QStringLiteral("#555555");
    const auto index = qHash(tagName) % colors.size();
    return colors.at(static_cast<int>(index));
}

bool GuiController::isVerbose()
{
    static bool is = std::getenv("POINTLESS_IS_VERBOSE") != nullptr;
    return is;
}

void GuiController::dumpTaskDebug(const QString &taskUuid) const
{
    const auto *task = _taskModel->taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid.toStdString());
        return;
    }
    task->dumpDebug();
}
