// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "../core/task_manager.h"
#include "../core/logger.h"

#include <QTimer>

namespace {
QDate firstMondayOfWeek(const QDate &date)
{
    return date.addDays(1 - date.dayOfWeek());
}
}

Controller::Controller(QObject *parent)
    : QObject(parent)
    , _supabase(Supabase::createDefault())
{
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
    if (!_supabase.isAuthenticated()) {
        P_LOG_ERROR("Cannot refresh: not authenticated");
        return;
    }

    std::string json_str = _supabase.retrieveData();
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
    if (_navigatorStartDate == date)
        return;
    _navigatorStartDate = date;
    emit navigatorStartDateChanged();
    emit navigatorEndDateChanged();
}

void Controller::navigatorGotoToday()
{
    QDate today = QDate::currentDate();
    QDate monday = firstMondayOfWeek(today);
    P_LOG_INFO("Today: {}, First Monday: {}", today.toString(Qt::ISODate).toStdString(), monday.toString(Qt::ISODate).toStdString());
    setNavigatorStartDate(firstMondayOfWeek(QDate::currentDate()));
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
    if (_currentViewType == viewType)
        return;

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
