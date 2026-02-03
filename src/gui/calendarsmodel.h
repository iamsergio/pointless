// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/calendar_provider.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

class CalendarsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles : std::uint16_t {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ColorRole,
        WriteableRole,
        EnabledRole
    };

    explicit CalendarsModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int count() const;

    Q_INVOKABLE void setEnabled(const QString &calendarId, bool enabled);

    void reload();

    void setProvider(pointless::core::CalendarProvider *provider);

Q_SIGNALS:
    void countChanged();

private:
    struct CalendarEntry
    {
        pointless::core::Calendar calendar;
        bool enabled = true;
    };

    pointless::core::CalendarProvider *_provider = nullptr;
    std::vector<CalendarEntry> _entries;
};
