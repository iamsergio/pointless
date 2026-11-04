// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts
import pointless 1.0

Rectangle {
    id: root

    property alias text: textItem.text
    property string fontAwesomeIcon: ""
    property color textColor: Style.ink
    property color iconColor: Style.inkSecondary
    property bool chip: false

    signal triggered

    implicitWidth: chip ? chipRow.implicitWidth + Style.fromPixel(28) : 0
    height: chip ? Style.fromPixel(34) : Style.fromPixel(50)
    Layout.fillWidth: !chip
    radius: chip ? height / 2 : 0
    border.width: chip ? 1 : 0
    border.color: Style.line
    color: mouseArea.containsPress ? (chip ? Style.raised : Qt.lighter(Style.raised, 1.2)) : "transparent"

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    RowLayout {
        id: chipRow
        anchors.verticalCenter: parent.verticalCenter
        x: root.chip ? Style.fromPixel(14) : Style.fromPixel(16)
        width: root.chip ? implicitWidth : parent.width - Style.fromPixel(32)
        spacing: Style.fromPixel(root.chip ? 7 : 14)

        Text {
            visible: root.fontAwesomeIcon !== ""
            text: root.fontAwesomeIcon
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(root.chip ? 16 : 18)
            color: root.iconColor
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: root.chip ? implicitWidth : Style.fromPixel(24)
        }

        Text {
            id: textItem
            font.pixelSize: Style.fromPixel(root.chip ? 14 : 15.5)
            color: root.textColor
            Layout.fillWidth: !root.chip
        }
    }

    Rectangle {
        visible: !root.chip
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Style.line
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.triggered()
    }
}
