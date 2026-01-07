// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

Rectangle {
    id: root

    required property string tagName
    property bool isSelected: false
    property bool isInteractive: false

    signal clicked(string tagName)

    visible: root.tagName !== ""
    color: GuiController.colorFromTag(root.tagName)
    radius: Style.fromPixel(10)
    implicitHeight: Style.fromPixel(isInteractive ? 24 : 14)
    implicitWidth: tagText.implicitWidth + Style.fromPixel(14)

    border {
        width: (isInteractive && isSelected) ? Style.fromPixel(2) : 0
        color: "white"
    }

    Text {
        id: tagText
        anchors.centerIn: parent
        text: root.tagName
        color: "white"
        font.pixelSize: Style.fromPixel(12)
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.isInteractive
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.clicked(root.tagName);
        }
    }
}
