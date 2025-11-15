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

    height: 70
    color: "transparent"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 15

        CheckBox {
            checked: root.taskIsDone
            backgroundColor: Style.background
            onClicked: {
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: root.taskTitle
                font.pixelSize: 18
                color: root.taskIsDone ? Style.taskCompletedTextColor : "#ffffff"
                font.strikeout: root.taskIsDone
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text {
                text: root.taskDueDate ? Qt.formatDate(root.taskDueDate, "MMMM yyyy") : ""
                font.pixelSize: 13
                color: Style.taskSecondaryTextColor
                visible: text !== ""
            }
        }

        FontAwesomeButton {
            fontAwesomeIcon: "ellipsis-vertical"
            onClicked: {
            }
        }
    }
}
