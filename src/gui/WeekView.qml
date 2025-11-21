// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Item {
    id: root

    WeekdayModel {
        id: weekdayModel
        mondayDate: Controller.navigatorStartDate
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
        spacing: Style.fromPixel(5)
        model: weekdayModel
        delegate: ColumnLayout {
            id: column
            width: parent ? parent.width : 0

            required property string prettyDate
            required property TaskFilterModel tasks

            RowLayout {
                spacing: 0
                visible: column.tasks.count > 0
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
                    fontAwesomeIcon: "\uf067" // plus icon
                    iconSize: 18
                    iconColor: Style.weekdayTextColor
                    implicitWidth: 32
                    implicitHeight: 32
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    backgroundColor: "transparent"
                    onClicked: {}
                }
            }

            Repeater {
                model: column.tasks
                Task {
                    required property string uuid
                    required property string title
                    required property bool isDone
                    required property bool isImportant
                    required property var dueDate
                    required property bool hasDueDate
                    required property string tagName
                    required property bool isFromCalendar
                    required property string calendarName

                    Layout.fillWidth: true

                    taskUuid: uuid
                    taskTitle: title
                    taskIsDone: isDone
                    taskIsImportant: isImportant
                    taskDueDate: dueDate
                    taskHasDueDate: hasDueDate
                    taskTagName: tagName
                    taskIsFromCalendar: isFromCalendar
                    taskCalendarName: calendarName
                }
            }
        }
    }
}
