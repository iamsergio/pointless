// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageTitle: "Cleanup"

    ColumnLayout {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: Style.fromPixel(20)
            leftMargin: Style.fromPixel(16)
            rightMargin: Style.fromPixel(16)
        }
        spacing: Style.fromPixel(4)

        Item {
            Layout.fillWidth: true
            implicitHeight: Style.fromPixel(40)

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: "Cleanup old tasks"
                color: Style.taskTextColor
                font.pixelSize: Style.fromPixel(16)
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    GuiController.cleanupOldData()
                }
            }
        }

        Item {
            Layout.fillWidth: true
            implicitHeight: Style.fromPixel(40)

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: "Delete all calendar events"
                color: Style.taskTextColor
                font.pixelSize: Style.fromPixel(16)
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    GuiController.deleteAllCalendarEvents()
                }
            }
        }
    }
}
