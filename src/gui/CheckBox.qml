// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls

import pointless 1.0

CheckBox {
    id: root
    indicator: Rectangle {
        implicitWidth: Style.fromPixel(17)
        implicitHeight: Style.fromPixel(17)
        x: root.leftPadding
        y: root.height / 2 - height / 2
        radius: width / 2
        color: Style.taskBackground
        border.color: Style.textColor2
        border.width: Style.fromPixel(1)

        Rectangle {
            width: Style.fromPixel(9)
            height: Style.fromPixel(9)
            anchors.centerIn: parent
            radius: width / 2
            color: Style.textColor2
            visible: root.checked
        }
    }
}
