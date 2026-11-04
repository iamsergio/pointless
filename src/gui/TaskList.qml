// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls.Fusion as QC

import pointless 1.0

Rectangle {
    id: root

    readonly property alias count: listView.count
    readonly property real contentHeight: listView.contentHeight + 2

    property bool showTags: true
    property bool showsTagsInSecondLine: false
    property bool showsDate: true
    property string searchText: ""

    color: Style.surface
    border.width: 1
    border.color: Style.line
    radius: Style.fromPixel(18)
    visible: count > 0

    TaskFilterModel {
        id: taskFilterModel
        viewType: GuiController.currentViewType
        tagName: GuiController.currentTag
        searchText: root.searchText
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 1
        clip: true
        spacing: 0
        cacheBuffer: 1000
        QC.ScrollBar.vertical: QC.ScrollBar {
            visible: !GuiController.isMobile
        }

        model: taskFilterModel

        delegate: TaskRow {
            required property int index
            required property string uuid
            required property string title
            required property bool isDone
            required property bool isImportant
            required property bool isEvening
            required property var dueDate
            required property var dueDateRaw
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
            required property bool hasNotes
            required property bool isGoal
            required property bool isYearly
            required property string meetingUrl

            objectName: "task_" + index

            width: ListView.view.width
            rowIndex: index
            taskUuid: uuid
            taskTitle: title
            taskIsDone: isDone
            taskIsImportant: isImportant
            taskIsEvening: isEvening
            taskDueDate: dueDate
            taskDueDateRaw: dueDateRaw
            taskTagName: tagName
            taskIsFromCalendar: isFromCalendar
            taskCalendarName: calendarName
            taskAllTags: allTags
            taskHasNotes: hasNotes
            taskIsGoal: isGoal
            taskIsYearly: isYearly
            taskMeetingUrl: meetingUrl
            taskHasDueDate: hasDueDate
            taskHasDueDateTime: hasDueDateTime
            taskIsSoon: isSoon
            taskIsLater: isLater
            taskIsCurrent: isCurrent
            taskIsDueTomorrow: isDueTomorrow
            showTags: root.showTags
            showsTagsInSecondLine: root.showsTagsInSecondLine
            showsDate: root.showsDate

            onClicked: {
                GuiController.setTaskMenuUuid(uuid);
            }
        }
    }
}
