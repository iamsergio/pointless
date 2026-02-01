// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageTitle: "About"

    ColumnLayout {
        id: column
        anchors.centerIn: parent
        spacing: Style.fromPixel(12)

        property int numTasks: 0
        property int localRevision: 0

        onVisibleChanged: {
            if (visible) {
                numTasks = GuiController.numTasks();
                localRevision = GuiController.localRevision();
            }
        }

        Text {
            text: "Pointless"
            color: Style.sectionTextColor
            font.pixelSize: Style.fromPixel(28)
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "A simple to-do manager"
            color: Style.taskSecondaryTextColor
            font.pixelSize: Style.fromPixel(16)
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Qt version: " + GuiController.qtVersion()
            color: Style.taskSecondaryTextColor
            font.pixelSize: Style.fromPixel(14)
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Local revision: " + column.localRevision
            color: Style.taskSecondaryTextColor
            font.pixelSize: Style.fromPixel(14)
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Number of tasks: " + column.numTasks
            color: Style.taskSecondaryTextColor
            font.pixelSize: Style.fromPixel(14)
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
