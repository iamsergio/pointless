// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import pointless 1.0

Button {
    id: root

    property string fontAwesomeIcon: ""
    property int iconSize: Style.fromPixel(20)
    property color iconColor: Style.inkSecondary
    property color fillColor: "transparent"
    property int diameter: Style.fromPixel(40)

    hoverEnabled: true
    implicitWidth: diameter
    implicitHeight: diameter

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    contentItem: Text {
        text: root.fontAwesomeIcon
        font.family: fontAwesome.name
        font.pixelSize: root.iconSize
        color: root.iconColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    readonly property bool isFilled: fillColor.a > 0

    background: Rectangle {
        radius: width / 2
        color: root.isFilled && (root.pressed || root.hovered) ? Qt.lighter(root.fillColor, root.pressed ? 1.2 : 1.1) : root.fillColor
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Style.raised
            visible: !root.isFilled
            opacity: root.pressed ? 1 : (root.hovered ? 0.6 : 0)
        }
    }
}
