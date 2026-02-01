// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Rectangle {
    id: root
    color: Style.background

    signal backClicked()

    FontAwesomeButton {
        fontAwesomeIcon: "\uf053"
        iconSize: Style.fromPixel(18)
        iconColor: Style.iconColor
        backgroundColor: "transparent"
        anchors {
            top: parent.top
            left: parent.left
            topMargin: Style.fromPixel(16)
            leftMargin: Style.fromPixel(16)
        }
        onClicked: root.backClicked()
    }

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
