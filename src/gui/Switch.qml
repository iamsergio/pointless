// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import pointless 1.0

Item {
    id: root

    property bool checked: false

    signal toggled

    implicitWidth: Style.fromPixel(44)
    implicitHeight: Style.fromPixel(26)

    Rectangle {
        anchors.fill: parent
        radius: height / 2
        color: root.checked ? Style.accent : Style.line

        Behavior on color {
            ColorAnimation {
                duration: 120
            }
        }
    }

    Rectangle {
        width: Style.fromPixel(20)
        height: width
        radius: width / 2
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
        x: root.checked ? root.width - width - Style.fromPixel(3) : Style.fromPixel(3)

        Behavior on x {
            NumberAnimation {
                duration: 120
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled()
    }
}
