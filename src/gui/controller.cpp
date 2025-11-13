// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include "controller.h"
#include "taskfiltermodel.h"
#include "taskmodel.h"
#include "tagmodel.h"

namespace {
QDate firstMondayOfWeek(const QDate &date)
{
    return date.addDays(1 - date.dayOfWeek());
}
}

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    _taskModel = new TaskModel(this);
    _tagModel = new TagModel(this);
    _taskFilterModel = new TaskFilterModel(this, this);
    _taskFilterModel->setSourceModel(_taskModel);
    navigatorGotoToday();
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
