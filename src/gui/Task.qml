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
    required property var taskDueDate
    required property bool taskHasDueDate
    required property string taskTagName
    required property bool taskIsFromCalendar
    required property string taskCalendarName

    height: Style.fromPixel(55)
    color: taskIsImportant ? Style.taskImportantBackground : Style.taskBackground
    radius: 10

    signal clicked;

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

            // CheckBox {
            //     checked: root.taskIsDone
            //     onClicked: {}
            // }

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
                        color: "#00ff00"
                    }

                    Text {
                        id: dateText
                        text: root.taskHasDueDate ? root.taskDueDate : ""
                        font.pixelSize: Style.fromPixel(11)
                        color: Style.taskSecondaryTextColor
                        visible: Controller.currentViewType !== Controller.Week
                    }
                }
            }

            Rectangle {
                visible: root.taskTagName !== "" && Controller.currentViewType === Controller.Week
                color: Controller.colorFromTag(root.taskTagName)
                radius: Style.fromPixel(10)
                implicitHeight: Style.fromPixel(18)
                implicitWidth: tagText.implicitWidth + Style.fromPixel(14)
                Layout.alignment: Qt.AlignVCenter

                Text {
                    id: tagText
                    anchors.centerIn: parent
                    text: root.taskTagName
                    color: "white"
                    font.pixelSize: Style.fromPixel(12)
                }
            }

            FontAwesomeButton {
                fontAwesomeIcon: "ellipsis-vertical"
                iconColor: Style.iconColor
                backgroundColor: "transparent"
                onClicked: optionsMenu.popup()
            }

            Menu {
                id: optionsMenu
                MenuItem {
                    text: "Edit..."
                    onTriggered: {
                        Controller.isEditing = true;
                        Controller.uuidBeingEdited = root.taskUuid;
                    }
                }
                MenuItem { text: "Move to Soon" }
                MenuItem { text: "Move to Later" }
                MenuItem { text: "Move to Tomorrow" }
                MenuItem { text: "Move to Monday" }
                MenuItem { text: "Delete" }
            }
        }
    }
}
