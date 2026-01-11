// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Item {
    id: root

    // for Spix
    readonly property alias weekdayModelCount: weekdayModel.count
    readonly property alias weekdayFilterModelCount: weekdayFilterModel.count

    WeekdayModel {
        id: weekdayModel
        mondayDate: GuiController.navigatorStartDate
    }

    WeekdayFilterModel {
        id: weekdayFilterModel
        source: weekdayModel
    }

    ListView {
        id: weekdayListView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        clip: true
        spacing: Style.fromPixel(15)
        model: weekdayFilterModel
        delegate: ColumnLayout {
            id: column
            width: parent ? parent.width : 0

            required property date date
            required property string prettyDate
            required property TaskFilterModel tasks
            required property bool isPast
            required property int index

            spacing: Style.fromPixel(10)
            objectName: "weekdayListView_day_" + index

            RowLayout {
                spacing: 0
                Layout.fillWidth: true
                Text {
                    font.pixelSize: Style.weekdayFontSize
                    font.bold: true
                    color: Style.weekdayTextColor
                    text: column.prettyDate
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                }
                FontAwesomeButton {
                    id: addTaskButton
                    objectName: "addTaskButton_" + column.index
                    fontAwesomeIcon: "\uf067" // plus icon
                    iconSize: 18
                    iconColor: Style.iconColor
                    implicitWidth: 32
                    implicitHeight: 32
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    backgroundColor: "transparent"
                    onClicked: {
                        GuiController.setTaskBeingEdited("", column.date);
                    }
                }
            }

            Rectangle {
                visible: column.tasks.count === 0
                Layout.fillWidth: true
                Layout.preferredHeight: Style.taskHeight
                color: Style.background2
                radius: 8
                border.width: 1
                border.color: "#2a3441"

                Text {
                    anchors.centerIn: parent
                    text: "No tasks for this day."
                    font.pixelSize: Style.fromPixel(14)
                    color: Style.taskSecondaryTextColor
                }
            }

            Repeater {
                model: column.tasks
                Task {
                    required property int index
                    required property string uuid
                    required property string title
                    required property bool isDone
                    required property bool isImportant
                    required property bool isEvening
                    required property string dueDate
                    required property bool hasDueDate
                    required property string tagName
                    required property bool isFromCalendar
                    required property string calendarName

                    objectName: "task_" + column.index + "_" + index

                    Layout.fillWidth: true

                    taskUuid: uuid
                    taskTitle: title
                    taskIsDone: isDone
                    taskIsImportant: isImportant
                    taskIsEvening: isEvening
                    taskDueDate: dueDate
                    taskHasDueDate: hasDueDate
                    taskTagName: tagName
                    taskIsFromCalendar: isFromCalendar
                    taskCalendarName: calendarName

                    onClicked: {
                        GuiController.dumpTaskDebug(uuid);
                    }
                }
            }
        }
    }
}
