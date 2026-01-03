// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

Rectangle {
    id: root

    required property string tagName

    visible: root.tagName !== ""
    color: GuiController.colorFromTag(root.tagName)
    radius: Style.fromPixel(10)
    implicitHeight: Style.fromPixel(18)
    implicitWidth: tagText.implicitWidth + Style.fromPixel(14)

    Text {
        id: tagText
        anchors.centerIn: parent
        text: root.tagName
        color: "white"
        font.pixelSize: Style.fromPixel(12)
    }
}
