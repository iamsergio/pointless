// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts
import pointless 1.0

Rectangle {
    id: root
    border.width: 0
    border.color: "black"
    color: "transparent"
    height: Style.menuHeight
    Layout.fillWidth: true

    signal triggered

    property alias text: textItem.text

    Text {
        id: textItem
        font.pixelSize: Style.fromPixel(16)
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.triggered()
    }
}
