// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

ListView {
    id: root

    clip: true
    spacing: Style.taskSpacing

    TaskFilterModel {
        id: taskFilterModel
        viewType: GuiController.currentViewType
    }

    model: taskFilterModel

    delegate: Task {
        required property int index
        required property string uuid
        required property string title
        required property bool isDone
        required property bool isImportant
        required property bool isEvening
        required property var dueDate
        required property string tagName
        required property string hasDueDate
        required property bool isFromCalendar
        required property string calendarName

        objectName: "task_" + index

        width: ListView.view.width
        taskUuid: uuid
        taskTitle: title
        taskIsDone: isDone
        taskIsImportant: isImportant
        taskIsEvening: isEvening
        taskDueDate: dueDate
        taskTagName: tagName
        taskIsFromCalendar: isFromCalendar
        taskCalendarName: calendarName
        taskHasDueDate: hasDueDate
    }
}
