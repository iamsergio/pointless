// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Style.fromPixel(12)

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
    }
}
