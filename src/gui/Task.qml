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
    required property var taskDueDate
    required property bool hasDueDate
    property string taskTagName: ""
    property bool isFromCalendar: false
    property string calendarName: ""

    height: Style.fromPixel(55)
    color: "transparent"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Style.fromPixel(10)
        anchors.rightMargin: Style.fromPixel(10)
        spacing: Style.fromPixel(15)

        CheckBox {
            checked: root.taskIsDone
            backgroundColor: Style.background
            onClicked: {}
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
                    id: dateText
                    text: root.hasDueDate ? root.taskDueDate : ""
                    font.pixelSize: Style.fromPixel(13)
                    color: Style.taskSecondaryTextColor
                    visible: Controller.currentViewType !== Controller.Week
                }

                Text {
                    id: calendarText
                    visible: root.isFromCalendar && root.calendarName !== ""
                    text: "[" + root.calendarName + "]"
                    font.pixelSize: Style.fromPixel(13)
                    color: "#00ff00"
                }
            }
        }

        Rectangle {
            visible: root.taskTagName !== "" && Controller.currentViewType === Controller.Week
            color: "#555555"
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
            onClicked: {}
        }
    }
}
