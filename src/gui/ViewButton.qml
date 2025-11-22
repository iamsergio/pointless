// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import pointless 1.0

Button {
    id: root

    property bool isActive: false

    implicitWidth: Style.fromPixel(100)
    implicitHeight: Style.fromPixel(32)

    contentItem: Text {
        text: root.text
        font.pixelSize: Style.fromPixel(12)
        font.bold: root.isActive
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        color: root.isActive ? Style.buttonActive : Style.buttonBackground
        radius: 25
    }
}
