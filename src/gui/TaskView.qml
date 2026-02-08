// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls as QC

import pointless 1.0

ListView {
    id: root

    clip: true
    spacing: Style.taskSpacing
    cacheBuffer: 1000
    QC.ScrollBar.vertical: QC.ScrollBar {
        visible: !GuiController.isMobile
    }

    property bool showTags: true

    TaskFilterModel {
        id: taskFilterModel
        viewType: GuiController.currentViewType
        tagName: GuiController.currentTag
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
        required property bool hasDueDate
        required property bool isFromCalendar
        required property string calendarName
        required property bool isSoon
        required property bool isLater
        required property bool isCurrent
        required property bool isDueTomorrow
        required property bool hasDueDateTime
        required property string allTags

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
        taskAllTags: allTags
        taskHasDueDate: hasDueDate
        taskHasDueDateTime: hasDueDateTime
        taskIsSoon: isSoon
        taskIsLater: isLater
        taskIsCurrent: isCurrent
        taskIsDueTomorrow: isDueTomorrow
        showTags: root.showTags
    }
}
