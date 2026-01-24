// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/task.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <cstdint>

namespace pointless::core {
class LocalData;
}

class DataController;

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles : std::uint16_t {
        UuidRole = Qt::UserRole + 1,
        TitleRole,
        IsDoneRole,
        IsImportantRole,
        DueDateRole,
        HasDueDateRole,
        TagNameRole,
        IsFromCalendarRole,
        CalendarNameRole,
        IsEveningRole,
        NeedsSyncToServerRole,
        RevisionRole,
        IsSoonRole,
        IsLaterRole,
        IsCurrentRole,
        IsDueTomorrowRole,
        HasDueDateTimeRole
    };

    explicit TaskModel(QObject *parent = nullptr);
    ~TaskModel() override;
    TaskModel(const TaskModel &) = delete;
    TaskModel &operator=(const TaskModel &) = delete;
    TaskModel(TaskModel &&) = delete;
    TaskModel &operator=(TaskModel &&) = delete;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int count() const;

    void reload();
    void addTask(const pointless::core::Task &task);
    [[nodiscard]] const pointless::core::Task *taskAt(int row) const;
    [[nodiscard]] const pointless::core::Task *taskForUuid(const QString &taskUuid) const;
    [[nodiscard]] pointless::core::Task *taskForUuid(const QString &taskUuid);
    [[nodiscard]] int indexForTask(const QString &taskUuid) const;

    Q_INVOKABLE void setTaskDone(const QString &taskUuid, bool isDone);
    void updateTask(const pointless::core::Task &task);

Q_SIGNALS:
    void countChanged();

private:
    [[nodiscard]] const pointless::core::LocalData &localData() const;
    [[nodiscard]] pointless::core::LocalData &localData();
    [[nodiscard]] DataController *dataController() const;
};
