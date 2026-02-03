// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Rectangle {
    id: root

    required property string taskUuid
    required property string taskTitle
    required property bool taskIsDone
    required property bool taskIsImportant
    required property bool taskIsEvening
    required property string taskDueDate
    required property bool taskHasDueDate
    required property bool taskHasDueDateTime
    required property string taskTagName
    required property bool taskIsFromCalendar
    required property string taskCalendarName

    required property bool taskIsSoon
    required property bool taskIsLater
    required property bool taskIsCurrent
    required property bool taskIsDueTomorrow
    property bool showTags: true

    height: Style.taskHeight
    color: taskIsImportant ? Style.taskImportantBackground : (taskIsEvening ? Style.taskEveningBackground : Style.taskBackground)
    radius: 10

    signal clicked

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked();
        }
        onPressAndHold: {
            GuiController.setTaskImportant(root.taskUuid, !root.taskIsImportant);
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Style.fromPixel(10)
            anchors.rightMargin: Style.fromPixel(10)
            spacing: Style.fromPixel(15)

            CheckBox {
                checked: root.taskIsDone
                onClicked: {
                    GuiController.taskModel.setTaskDone(root.taskUuid, !root.taskIsDone);
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Style.fromPixel(5)

                Text {
                    text: root.taskTitle
                    font.pixelSize: Style.fromPixel(16)
                    color: root.taskIsDone ? Style.taskCompletedTextColor : "#ffffff"
                    font.strikeout: root.taskIsDone
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                RowLayout {
                    spacing: Style.fromPixel(8)
                    visible: shouldShowCalendar || shouldShowDueDate

                    readonly property bool shouldShowCalendar: root.taskIsFromCalendar && root.taskCalendarName !== ""
                    readonly property bool shouldShowDueDate: (root.taskHasDueDate && GuiController.currentViewType !== GuiController.Week) || root.taskHasDueDateTime

                    Text {
                        id: calendarText
                        visible: parent.shouldShowCalendar
                        text: "[" + root.taskCalendarName + "]"
                        font.pixelSize: Style.fromPixel(11)
                        color: root.taskIsDone ? Style.taskCalendarTextDisabled : "#00ff00"
                    }

                    Text {
                        id: dateText
                        text: root.taskHasDueDate ? root.taskDueDate : ""
                        font.pixelSize: Style.fromPixel(11)
                        color: Style.taskSecondaryTextColor
                        visible: parent.shouldShowDueDate
                    }
                }
            }

            Tag {
                tagName: root.taskTagName
                isInteractive: false
                visible: root.showTags
                Layout.alignment: Qt.AlignVCenter
                onClicked: {
                    GuiController.currentTag = tagName;
                }
            }

            FontAwesomeButton {
                id: optionsButton
                fontAwesomeIcon: "ellipsis-vertical"
                iconColor: Style.iconColor
                backgroundColor: "transparent"
                onClicked: GuiController.setTaskMenuUuid(root.taskUuid)
            }
        }
    }
}
