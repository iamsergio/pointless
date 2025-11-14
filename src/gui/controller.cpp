// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "../core/task_manager.h"
#include "../core/logger.h"

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
    _taskModel = new TaskModel(this);
    _tagModel = new TagModel(this);
    _taskFilterModel = new TaskFilterModel(this, this);
    _taskFilterModel->setSourceModel(_taskModel);
    navigatorGotoToday();
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
        P_LOG_ERROR("Cannot refresh: failed to parse JSON");
        return;
    }


    std::ofstream debugFile("/tmp/debug.json");
    if (debugFile.is_open()) {
        debugFile << json_str;
        debugFile.close();
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
