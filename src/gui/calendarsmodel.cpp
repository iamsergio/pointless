// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "calendarsmodel.h"

#ifdef Q_OS_APPLE

#include "core/apple_calendar_provider.h"
#include "core/logger.h"

#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

static const QString s_disabledCalendarIdsKey = QStringLiteral("disabledCalendarIds");

CalendarsModel::CalendarsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CalendarsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(_entries.size());
}

QVariant CalendarsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(_entries.size()))
        return {};

    const auto &entry = _entries[static_cast<size_t>(index.row())];

    switch (static_cast<Roles>(role)) {
    case IdRole:
        return QString::fromStdString(entry.calendar.id);
    case TitleRole:
        return QString::fromStdString(entry.calendar.title);
    case ColorRole:
        return QString::fromStdString(entry.calendar.color);
    case WriteableRole:
        return entry.calendar.writeable;
    case EnabledRole:
        return entry.enabled;
    }

    return {};
}

QHash<int, QByteArray> CalendarsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "calendarId";
    roles[TitleRole] = "title";
    roles[ColorRole] = "color";
    roles[WriteableRole] = "writeable";
    roles[EnabledRole] = "enabled";
    return roles;
}

int CalendarsModel::count() const
{
    return rowCount();
}

void CalendarsModel::setEnabled(const QString &calendarId, bool enabled)
{
    QSettings settings;
    QStringList disabledIds = settings.value(s_disabledCalendarIdsKey).toStringList();

    if (enabled)
        disabledIds.removeAll(calendarId);
    else if (!disabledIds.contains(calendarId))
        disabledIds.append(calendarId);

    settings.setValue(s_disabledCalendarIdsKey, disabledIds);

    for (size_t i = 0; i < _entries.size(); ++i) {
        if (QString::fromStdString(_entries[i].calendar.id) == calendarId) {
            _entries[i].enabled = enabled;
            const QModelIndex idx = index(static_cast<int>(i));
            Q_EMIT dataChanged(idx, idx, {EnabledRole});
            break;
        }
    }
}

void CalendarsModel::reload()
{
    beginResetModel();

    pointless::core::AppleCalendarProvider provider;
    auto calendars = provider.getCalendars();

    QSettings settings;
    const QStringList disabledIds = settings.value(s_disabledCalendarIdsKey).toStringList();

    _entries.clear();
    _entries.reserve(calendars.size());
    for (auto &cal : calendars) {
        bool enabled = !disabledIds.contains(QString::fromStdString(cal.id));
        _entries.push_back({std::move(cal), enabled});
    }

    P_LOG_INFO("numCalendars = {}", static_cast<int>(_entries.size()));

    endResetModel();
    Q_EMIT countChanged();
}

#endif
