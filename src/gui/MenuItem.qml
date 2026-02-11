// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts
import pointless 1.0

Rectangle {
    id: root
    border.width: 0
    color: mouseArea.containsPress ? Qt.lighter(Style.taskBackground, 1.4)
         : mouseArea.containsMouse ? Qt.lighter(Style.taskBackground, 1.2)
         : "transparent"
    height: Style.menuHeight
    Layout.fillWidth: true
    Layout.leftMargin: Style.fromPixel(6)
    Layout.rightMargin: Style.fromPixel(6)
    radius: Style.fromPixel(6)

    signal triggered

    property alias text: textItem.text

    Text {
        id: textItem
        font.pixelSize: Style.fromPixel(16)
        color: Style.taskTextColor
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.triggered()
    }
}
