// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "search"
    pageTitle: "Search"

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.fromPixel(12)

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Style.fromPixel(44)
            color: Style.taskBackground
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.margins: Style.fromPixel(10)
                spacing: Style.fromPixel(8)

                Text {
                    text: "\uf002"
                    font.family: fontAwesome.name
                    font.pixelSize: Style.fromPixel(14)
                    color: "#888888"
                }

                QC.TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    placeholderText: "Search tasks..."
                    color: "white"
                    placeholderTextColor: "#888888"
                    background: null
                    font.pixelSize: Style.fromPixel(14)
                    selectByMouse: true
                    verticalAlignment: Text.AlignVCenter
                    focus: root.visible

                    onTextChanged: {
                        taskFilterModel.searchText = text;
                    }
                }
            }
        }

        TaskFilterModel {
            id: taskFilterModel
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Style.taskSpacing
            cacheBuffer: 1000
            QC.ScrollBar.vertical: QC.ScrollBar {
                visible: !GuiController.isMobile
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
                required property bool hasNotes

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
                taskHasNotes: hasNotes
                taskHasDueDate: hasDueDate
                taskHasDueDateTime: hasDueDateTime
                taskIsSoon: isSoon
                taskIsLater: isLater
                taskIsCurrent: isCurrent
                taskIsDueTomorrow: isDueTomorrow
                showTags: true

                onClicked: {
                    GuiController.setTaskMenuUuid(uuid);
                }
            }
        }
    }
}
