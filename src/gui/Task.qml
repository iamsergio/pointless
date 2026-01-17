// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
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
    required property string taskTagName
    required property bool taskIsFromCalendar
    required property string taskCalendarName

    required property bool taskIsSoon
    required property bool taskIsLater
    required property bool taskIsCurrent

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
                    visible: dateText.visible || calendarText.visible

                    Text {
                        id: calendarText
                        visible: root.taskIsFromCalendar && root.taskCalendarName !== ""
                        text: "[" + root.taskCalendarName + "]"
                        font.pixelSize: Style.fromPixel(11)
                        color: root.taskIsDone ? Style.taskCalendarTextDisabled : "#00ff00"
                    }

                    Text {
                        id: dateText
                        text: root.taskHasDueDate ? root.taskDueDate : ""
                        font.pixelSize: Style.fromPixel(11)
                        color: Style.taskSecondaryTextColor
                        visible: GuiController.currentViewType !== GuiController.Week
                    }
                }
            }

            Tag {
                tagName: root.taskTagName
                Layout.alignment: Qt.AlignVCenter
            }

            FontAwesomeButton {
                id: optionsButton
                fontAwesomeIcon: "ellipsis-vertical"
                iconColor: Style.iconColor
                backgroundColor: "transparent"
                onClicked: optionsMenu.popup()
            }

            Menu {
                id: optionsMenu
                MenuItem {
                    id: editMenuItem
                    text: "Edit..."
                    onTriggered: {
                        GuiController.setTaskBeingEdited(root.taskUuid, null);
                    }
                }
                MenuItem {
                    id: moveToCurrentMenuItem
                    text: "Move to Current"
                    visible: !root.taskIsCurrent
                    height: visible ? implicitHeight : 0
                    onTriggered: {
                        GuiController.moveTaskToCurrent(root.taskUuid);
                    }
                }
                MenuItem {
                    id: moveToSoonMenuItem
                    text: "Move to Soon"
                    visible: !root.taskIsSoon
                    height: visible ? implicitHeight : 0
                    onTriggered: {
                        GuiController.moveTaskToSoon(root.taskUuid);
                    }
                }
                MenuItem {
                    id: moveToLaterMenuItem
                    text: "Move to Later"
                    visible: !root.taskIsLater
                    height: visible ? implicitHeight : 0
                    onTriggered: {
                        GuiController.moveTaskToLater(root.taskUuid);
                    }
                }
                MenuItem {
                    id: moveToTomorrowMenuItem
                    visible: !root.taskIsCurrent
                    height: visible ? implicitHeight : 0
                    text: "Move to Tomorrow"
                    onTriggered: {
                        GuiController.moveTaskToTomorrow(root.taskUuid);
                    }
                }
                MenuItem {
                    id: moveToMondayMenuItem
                    text: "Move to Monday"
                    height: visible ? implicitHeight : 0
                    onTriggered: {
                        GuiController.moveTaskToMonday(root.taskUuid);
                    }
                }
                MenuItem {
                    id: deleteMenuItem
                    text: "Delete"
                    onTriggered: {
                        GuiController.deleteTask(root.taskUuid);
                    }
                }
            }
        }
    }
}
