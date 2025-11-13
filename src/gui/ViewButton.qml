// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import pointless 1.0

Button {
    id: root

    property bool isActive: false

    implicitWidth: 120
    implicitHeight: 50

    contentItem: Text {
        text: root.text
        font.pixelSize: 16
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
